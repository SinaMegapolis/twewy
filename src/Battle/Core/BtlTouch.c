#include "Battle/Core/BtlTouch.h"
#include "Battle/Core/BtlActor.h"
#include "Combat/Core/BtlTch.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatEffectActor.h"
#include "Engine/EasyTask.h"
#include "Engine/IO/TouchInput.h"
#include "FixedPoint.h"
#include "nitro/math.h"
#include <nitro/fx.h>

/// Distance between the middle of an actor's box and a screen point.
static s32 BtlTouch_DistanceToActor(BattleActor* actor, s32 x, s32 y) {
    s32 centerY = actor->screenY + (12 - actor->height) / 2;
    s32 dx      = I2F(x) - I2F(actor->screenX);
    s32 dy      = I2F(y) - I2F(centerY);

    return FX_Sqrt(FX_Mul(dx, dx) + FX_Mul(dy, dy));
}

/// Whether segment (x0,y0)-(x1,y1) crosses segment (ax,ay)-(bx,by).
static BOOL BtlTouch_SegmentsCross(s32 x0, s32 y0, s32 x1, s32 y1, s32 ax, s32 ay, s32 bx, s32 by) {
    s32 lo;
    s32 hi;
    s32 denominator;
    s32 s;
    s32 t;

    lo = ax < bx ? ax : bx;
    hi = x0 > x1 ? x0 : x1;
    if (hi < lo) {
        return FALSE;
    }
    lo = ax > bx ? ax : bx;
    hi = x0 < x1 ? x0 : x1;
    if (hi > lo) {
        return FALSE;
    }
    lo = ay < by ? ay : by;
    hi = y0 > y1 ? y0 : y1;
    if (hi < lo) {
        return FALSE;
    }
    lo = ay > by ? ay : by;
    hi = y0 < y1 ? y0 : y1;
    if (hi > lo) {
        return FALSE;
    }
    denominator = (x1 - x0) * (by - ay) - (y1 - y0) * (bx - ax);
    if (denominator == 0) {
        return FALSE;
    }
    s = (y0 - ay) * (bx - ax) - (x0 - ax) * (by - ay);
    t = (y0 - ay) * (x1 - x0) - (x0 - ax) * (y1 - y0);
    if (!((denominator > 0 && s >= 0 && s <= denominator) || (denominator < 0 && denominator <= s && s <= 0))) {
        return FALSE;
    }
    if (!((denominator > 0 && t >= 0 && t <= denominator) || (denominator < 0 && denominator <= t && t <= 0))) {
        return FALSE;
    }
    return TRUE;
}

/// A quick short touch.
static BOOL BtlTouch_IsQuickTap(BattleTouchWork* touch) {
    return touch->holdFrames < 12 && touch->distance < I2F(16);
}

/// Counts taps; the fifth quick tap starts the tap effect.
static BOOL BtlTouch_CountRapidTaps(BattleTouchWork* touch) {
    if ((touch->flags & BTL_TOUCH_ALLOW_RAPID_TAP) == 0) {
        return FALSE;
    }
    if (touch->tapTimer > 0) {
        touch->tapCount++;
    }
    touch->tapTimer = 34;
    if (touch->tapCount < 5) {
        return FALSE;
    }
    EasyTask_CreateTask(&g_CombatGlobal->unk_3C2BC, &Tsk_BtlTch_Click, NULL, 0, NULL, NULL);
    return TRUE;
}

/// A short touch that travelled far: a slash.
static BOOL BtlTouch_IsSlash(BattleTouchWork* touch) {
    if ((touch->flags & BTL_TOUCH_ALLOW_SLASH) == 0) {
        return FALSE;
    }
    if (touch->holdFrames < 12 && touch->distance > I2F(16)) {
        return TRUE;
    }
    return FALSE;
}

/// A long touch that stays near where it started.
static BOOL BtlTouch_IsHold(BattleTouchWork* touch) {
    if ((touch->flags & BTL_TOUCH_ALLOW_HOLD) == 0) {
        return FALSE;
    }
    if (touch->watching == FALSE) {
        return FALSE;
    }

    s32 dx = I2F(touch->startX - touch->x);
    s32 dy = I2F(touch->startY - touch->y);

    if (ABS(dx) > I2F(4) || ABS(dy) > I2F(4)) {
        touch->watching = FALSE;
        return FALSE;
    }
    touch->watchFrames++;
    return TRUE;
}

/// A fast back and forth scribble.
static BOOL BtlTouch_IsScribble(BattleTouchWork* touch) {
    s32 speed;

    if ((touch->flags & BTL_TOUCH_ALLOW_SCRIBBLE) == 0) {
        return FALSE;
    }
    speed =
        FX_Sqrt(FX_Mul(I2F(touch->velocityX), I2F(touch->velocityX)) + FX_Mul(I2F(touch->velocityY), I2F(touch->velocityY)));
    if (touch->sweepFrames == 0) {
        touch->sweepCenterX = touch->sweepMinX = touch->sweepMaxX = touch->x;
        touch->sweepCenterY = touch->sweepMinY = touch->sweepMaxY = touch->y;
    }
    if (speed >= I2F(12)) {
        if (touch->sweepMinX > touch->x) {
            touch->sweepMinX = touch->x;
        } else if (touch->sweepMaxX < touch->x) {
            touch->sweepMaxX = touch->x;
        }
        if (touch->sweepMinY > touch->y) {
            touch->sweepMinY = touch->y;
        } else if (touch->sweepMaxY < touch->y) {
            touch->sweepMaxY = touch->y;
        }
        touch->sweepFrames++;
    } else {
        touch->sweepFrames--;
    }
    if (touch->sweepFrames <= 0) {
        touch->sweepFrames = 0;
    }
    if (touch->sweepFrames <= 8) {
        return FALSE;
    }
    touch->sweepCenterX = (touch->sweepMinX + touch->sweepMaxX) >> 1;
    touch->sweepCenterY = (touch->sweepMinY + touch->sweepMaxY) >> 1;
    touch->sweepMinX = touch->sweepMaxX = touch->x;
    touch->sweepMinY = touch->sweepMaxY = touch->y;
    touch->sweepFrames                  = 9;
    return TRUE;
}

/// Whether the current touch point is on `actor`.
static BOOL BtlTouch_IsOnActor(BattleActor* actor) {
    s32 x = g_CombatGlobal->touchWork.x;
    s32 y = g_CombatGlobal->touchWork.y;

    if ((actor->flags & ACTOR_FLAG_DEFEATED) || (actor->flags & ACTOR_FLAG_INTANGIBLE)) {
        return FALSE;
    }
    if (actor->kind == ACTOR_KIND_PLAYER) {
        if (ABS(actor->screenX - x) < actor->radius + 24 && actor->screenY + 30 >= y &&
            actor->screenY - actor->height - 18 <= y)
        {
            return TRUE;
        }
    } else if (ABS(actor->screenX - x) <= actor->radius && actor->screenY + 12 >= y && actor->screenY - actor->height <= y) {
        return TRUE;
    }
    return FALSE;
}

/// Whether segment (x0,y0)-(x1,y1) touches the box of `actor`.
static BOOL BtlTouch_StrokeHitsActor(BattleActor* actor, s32 x0, s32 y0, s32 x1, s32 y1) {
    s32 left   = actor->screenX - actor->radius;
    s32 right  = actor->screenX + actor->radius;
    s32 bottom = actor->screenY;
    s32 top    = actor->screenY - actor->height;
    s32 min;
    s32 max;

    if (left > x1 && left > x0) {
        return FALSE;
    }
    if (right < x1 && right < x0) {
        return FALSE;
    }
    if (top > y1 && top > y0) {
        return FALSE;
    }
    if (bottom < y1 && bottom < y0) {
        return FALSE;
    }
    min = x0 < x1 ? x0 : x1;
    if (left < min) {
        max = x0 > x1 ? x0 : x1;
        if (max < right) {
            min = y0 < y1 ? y0 : y1;
            if (top < min) {
                max = y0 > y1 ? y0 : y1;
                if (max < bottom) {
                    return TRUE;
                }
            }
        }
    }
    if (BtlTouch_SegmentsCross(x0, y0, x1, y1, left, top, right, top)) {
        return TRUE;
    }
    if (BtlTouch_SegmentsCross(x0, y0, x1, y1, left, bottom, right, bottom)) {
        return TRUE;
    }
    if (BtlTouch_SegmentsCross(x0, y0, x1, y1, left, top, left, bottom)) {
        return TRUE;
    }
    if (BtlTouch_SegmentsCross(x0, y0, x1, y1, right, top, right, bottom)) {
        return TRUE;
    }
    return FALSE;
}

/// Keeps `actor` as the best candidate when the stroke crosses it near its middle.
static void BtlTouch_PickStrokeTarget(BattleActor** best, s32* bestDistance, BattleActor* actor) {
    Ov003Global* global = g_CombatGlobal;
    s16          x1     = global->touchWork.endX;
    s16          x0     = global->touchWork.startX;
    s16          y0     = global->touchWork.startY;
    s16          y1     = global->touchWork.endY;
    s32          distance;

    if (BtlTouch_StrokeHitsActor(actor, x0, y0, x1, y1)) {
        distance = BtlTouch_DistanceToActor(actor, (s16)((x0 + x1) / 2), (s16)((y0 + y1) / 2));
        if (distance < *bestDistance) {
            *bestDistance = distance;
            *best         = actor;
        }
    }
}

/// Keeps `actor` as the best candidate when the touch point is on it and closer than the current best.
static void BtlTouch_PickPointTarget(BattleActor** best, s32* bestDistance, BattleActor* actor) {
    if (BtlTouch_IsOnActor(actor)) {
        s32 distance = BtlTouch_DistanceToActor(actor, g_CombatGlobal->touchWork.x, g_CombatGlobal->touchWork.y);
        if (distance < *bestDistance) {
            *bestDistance = distance;
            *best         = actor;
        }
    }
}

static void BtlTouch_PickEnemyAtPoint(BattleActor** best, s32* bestDistance) {
    Ov003EnemyActor* unit = BtlList_GetHead(&g_CombatGlobal->unk3D8A4);

    while (unit != NULL) {
        BtlTouch_PickPointTarget(best, bestDistance, &unit->actor);
        unit = BtlList_GetNext(&unit->node);
    }
}

static void BtlTouch_PickObjectAtPoint(BattleActor** best, s32* bestDistance) {
    Ov003EffectActor* obs = BtlList_GetHead(&g_CombatGlobal->unk3D8B4);

    while (obs != NULL) {
        BtlTouch_PickPointTarget(best, bestDistance, &obs->actor);
        obs = BtlList_GetNext(&obs->node);
    }
}

/// The actor under the touch point, the player first and then the closest enemy.
static BattleActor* BtlTouch_FindActorAtPoint(void) {
    BattleActor* best     = NULL;
    s32          distance = 0x7FFFFFFF;

    if (BtlTouch_IsOnActor(g_CombatGlobal->player)) {
        return g_CombatGlobal->player;
    }
    BtlTouch_PickEnemyAtPoint(&best, &distance);
    BtlTouch_PickObjectAtPoint(&best, &distance);
    return best;
}

/// The stylus came down.
static void BtlTouch_OnPress(BattleTouchWork* touch) {
    BattleActor* actor = BtlTouch_FindActorAtPoint();

    if (actor != NULL) {
        switch (actor->kind) {
            case ACTOR_KIND_PLAYER:
                touch->targetKind = BTL_TOUCH_TARGET_PLAYER;
                break;
            case ACTOR_KIND_ENEMY:
                touch->targetKind = BTL_TOUCH_TARGET_ENEMY;
                break;
            case ACTOR_KIND_OBJECT:
                touch->targetKind = BTL_TOUCH_TARGET_OBJECT;
                break;
        }
        actor->flags |= ACTOR_FLAG_TOUCHED;
        touch->target = actor;
    } else {
        touch->targetKind = BTL_TOUCH_TARGET_NONE;
        touch->target     = NULL;
    }
    touch->held = touch->target;
}

/// The stylus came up.
static void BtlTouch_OnRelease(BattleTouchWork* touch) {
    touch->endX     = touch->x;
    touch->endY     = touch->y;
    s32 dx          = I2F(touch->endX - touch->startX);
    s32 dy          = I2F(touch->endY - touch->startY);
    touch->distance = FX_Sqrt(FX_Mul(dx, dx) + FX_Mul(dy, dy));
    touch->angle    = FX_Atan2Idx(dy, dx);
    touch->released = touch->held;
    touch->held     = NULL;
}

/// Re-targets the held actor when the stylus slid off it.
static void BtlTouch_UpdateHeld(BattleTouchWork* touch) {
    if (touch->held != NULL) {
        if (BtlTouch_IsOnActor(touch->held) == FALSE) {
            touch->held = BtlTouch_FindActorAtPoint();
        }
    } else {
        touch->held = BtlTouch_FindActorAtPoint();
    }
}

/// Classifies the touch of this frame as a BattleTouchGesture.
static s32 BtlTouch_Classify(BattleTouchWork* touch) {
    if (TouchInput_WasTouchReleased()) {
        if (BtlTouch_IsQuickTap(touch)) {
            if (BtlTouch_CountRapidTaps(touch)) {
                return BTL_TOUCH_GESTURE_RAPID_TAP;
            }
            if (touch->flags & BTL_TOUCH_ALLOW_TAP) {
                EasyTask_CreateTask(&g_CombatGlobal->unk_3C2BC, &Tsk_BtlTch_Click, NULL, 0, NULL, NULL);
                return BTL_TOUCH_GESTURE_TAP;
            }
        }
        if (BtlTouch_IsSlash(touch)) {
            EasyTask_CreateTask(&g_CombatGlobal->unk_3C2BC, &Tsk_BtlTch_Slash, NULL, 0, NULL, NULL);
            return BTL_TOUCH_GESTURE_SLASH;
        }
    }
    if (TouchInput_IsTouchActive()) {
        if (touch->holdFrames >= 12) {
            if (BtlTouch_IsHold(touch)) {
                return BTL_TOUCH_GESTURE_HOLD;
            }
            if (BtlTouch_IsScribble(touch)) {
                return BTL_TOUCH_GESTURE_SCRIBBLE;
            }
            if (touch->flags & BTL_TOUCH_ALLOW_DRAG) {
                return BTL_TOUCH_GESTURE_DRAG;
            }
        }
        return BTL_TOUCH_GESTURE_DOWN;
    }
    return BTL_TOUCH_GESTURE_NONE;
}

void BtlTouch_Clear(void) {
    g_CombatGlobal->touchWork.gesture       = BTL_TOUCH_GESTURE_NONE;
    g_CombatGlobal->touchWork.active        = FALSE;
    g_CombatGlobal->touchWork.holdFrames    = 0;
    g_CombatGlobal->touchWork.releaseFrames = 0;
    g_CombatGlobal->touchWork.watchFrames   = 0;
    g_CombatGlobal->touchWork.targetKind    = BTL_TOUCH_TARGET_NONE;
    g_CombatGlobal->touchWork.target        = NULL;
    g_CombatGlobal->touchWork.held          = NULL;
    g_CombatGlobal->touchWork.released      = NULL;
}

BOOL BtlTouch_IsGesture(u16 gesture) {
    if (g_CombatGlobal->touchWork.active) {
        return g_CombatGlobal->touchWork.gesture == gesture;
    }
    return FALSE;
}

BOOL BtlTouch_IsTargetKind(s32 kind) {
    return g_CombatGlobal->touchWork.targetKind == kind;
}

BOOL BtlTouch_SlashCrosses(BattleActor* actor) {
    if (BtlTouch_IsGesture(BTL_TOUCH_GESTURE_SLASH) == FALSE) {
        return FALSE;
    }

    Ov003Global* global = g_CombatGlobal;
    return BtlTouch_StrokeHitsActor(actor, global->touchWork.startX, global->touchWork.startY, global->touchWork.endX,
                                    global->touchWork.endY);
}

BattleActor* BtlTouch_GetTapped(BattleActorKind kind) {
    BattleActor* best     = NULL;
    s32          distance = 0x7FFFFFFF;

    if (BtlTouch_IsGesture(BTL_TOUCH_GESTURE_TAP) == FALSE) {
        return NULL;
    }
    switch (kind) {
        case ACTOR_KIND_PLAYER:
            if (BtlTouch_IsOnActor(g_CombatGlobal->player)) {
                best = g_CombatGlobal->player;
            }
            break;
        case ACTOR_KIND_ENEMY:
            BtlTouch_PickEnemyAtPoint(&best, &distance);
            break;
        case ACTOR_KIND_OBJECT:
            BtlTouch_PickObjectAtPoint(&best, &distance);
            break;
    }
    return best;
}

BattleActor* BtlTouch_GetDragged(BattleActorKind kind) {
    BattleActor* held = g_CombatGlobal->touchWork.held;

    if (BtlTouch_IsGesture(BTL_TOUCH_GESTURE_DRAG) == FALSE) {
        return NULL;
    }
    if (held != NULL && held->kind != kind) {
        held = NULL;
    }
    return held;
}

BattleActor* BtlTouch_GetSlashed(BattleActorKind kind) {
    s32          distance;
    BattleActor* best = NULL;

    if (BtlTouch_IsGesture(BTL_TOUCH_GESTURE_SLASH) == FALSE) {
        return NULL;
    }

    if (kind == ACTOR_KIND_PLAYER) {
        Ov003Global* global = g_CombatGlobal;

        if (BtlTouch_StrokeHitsActor(global->player, global->touchWork.startX, global->touchWork.startY,
                                     global->touchWork.endX, global->touchWork.endY))
        {
            best = g_CombatGlobal->player;
        }
    }
    if (kind == ACTOR_KIND_ENEMY) {
        Ov003EnemyActor* unit;

        best     = NULL;
        distance = 0x7FFFFFFF;
        unit     = BtlList_GetHead(&g_CombatGlobal->unk3D8A4);
        while (unit != NULL) {
            BtlTouch_PickStrokeTarget(&best, &distance, &unit->actor);
            unit = BtlList_GetNext(&unit->node);
        }
    }
    if (kind == ACTOR_KIND_OBJECT) {
        Ov003EffectActor* obs;

        best     = NULL;
        distance = 0x7FFFFFFF;
        obs      = BtlList_GetHead(&g_CombatGlobal->unk3D8B4);
        while (obs != NULL) {
            BtlTouch_PickStrokeTarget(&best, &distance, &obs->actor);
            obs = BtlList_GetNext(&obs->node);
        }
    }
    return best;
}

BattleActor* BtlTouch_GetRapidTapped(BattleActorKind kind) {
    BattleActor* best     = NULL;
    s32          distance = 0x7FFFFFFF;

    if (BtlTouch_IsGesture(BTL_TOUCH_GESTURE_RAPID_TAP) == FALSE) {
        return NULL;
    }
    switch (kind) {
        case ACTOR_KIND_PLAYER:
            if (BtlTouch_IsOnActor(g_CombatGlobal->player)) {
                best = g_CombatGlobal->player;
            }
            break;
        case ACTOR_KIND_ENEMY:
            BtlTouch_PickEnemyAtPoint(&best, &distance);
            break;
        case ACTOR_KIND_OBJECT:
            BtlTouch_PickObjectAtPoint(&best, &distance);
            break;
    }
    return best;
}

void BtlTouch_Init(void) {
    BattleTouchWork* touch = &g_CombatGlobal->touchWork;

    touch->active      = FALSE;
    touch->flags       = 0;
    touch->targetKind  = BTL_TOUCH_TARGET_NONE;
    touch->held        = NULL;
    touch->target      = NULL;
    touch->released    = NULL;
    touch->tapTimer    = 0;
    touch->tapCount    = 0;
    touch->watching    = TRUE;
    touch->watchFrames = 0;
    touch->x           = 1024;
    touch->y           = 1024;
    touch->velocityX   = 0;
    touch->velocityY   = 0;
    touch->flags |= BTL_TOUCH_ALLOW_TAP | BTL_TOUCH_ALLOW_SLASH | BTL_TOUCH_ALLOW_DRAG;
}

void BtlTouch_Update(void) {
    BattleTouchWork* touch = &g_CombatGlobal->touchWork;
    TouchCoord       coord;
    TouchCoord       delta;

    TouchInput_GetCoord(&coord);
    TouchInput_GetDelta(&delta);
    touch->x         = coord.x;
    touch->y         = coord.y;
    touch->velocityX = delta.x;
    touch->velocityY = delta.y;
    touch->tapTimer--;
    if (touch->tapTimer <= 0) {
        touch->tapCount = 0;
        touch->tapTimer = 0;
    }
    touch->active = TRUE;
    if (TouchInput_WasTouchPressed()) {
        touch->startX      = touch->x;
        touch->startY      = touch->y;
        touch->distance    = 0;
        touch->angle       = 0;
        touch->watching    = TRUE;
        touch->watchFrames = 0;
        touch->reserved24  = 0;
        touch->sweepFrames = 0;
        touch->stillFrames = 0;
        touch->anchorX     = touch->x;
        touch->anchorY     = touch->y;
        BtlTouch_OnPress(touch);
    }
    if (TouchInput_WasTouchReleased()) {
        BtlTouch_OnRelease(touch);
    }
    if (TouchInput_IsTouchActive()) {
        touch->holdFrames = TouchInput_GetHoldDuration();
        BtlTouch_UpdateHeld(touch);
        if (ABS(touch->anchorX - touch->x) < 4 && ABS(touch->anchorY - touch->y) < 4) {
            touch->stillFrames++;
        } else {
            touch->stillFrames = 0;
            touch->anchorX     = touch->x;
            touch->anchorY     = touch->y;
        }
    } else {
        touch->releaseFrames = TouchInput_GetHoldDuration();
    }
    touch->gesture = BtlTouch_Classify(touch);
}
