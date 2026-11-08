#include "Combat/Core/CombatTouch.h"
#include "Combat/Core/BtlTch.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatActor.h"
#include "Combat/Core/CombatApi.h"
#include "Engine/EasyTask.h"
#include "Engine/IO/TouchInput.h"
#include <nitro/fx.h>

/// Distance between the middle of an actor's box and a screen point.
static s32 CombatTouch_DistanceToActor(CombatActor* actor, s32 x, s32 y) {
    s32 centerY = actor->screenY + (12 - actor->unk_72) / 2;
    s32 dx      = (x << 12) - (actor->screenX << 12);
    s32 dy      = (y << 12) - (centerY << 12);

    return FX_Sqrt(FX_Mul(dx, dx) + FX_Mul(dy, dy));
}

/// Whether segment (x0,y0)-(x1,y1) crosses segment (ax,ay)-(bx,by).
static BOOL CombatTouch_SegmentsCross(s32 x0, s32 y0, s32 x1, s32 y1, s32 ax, s32 ay, s32 bx, s32 by) {
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
static BOOL CombatTouch_IsQuickTap(BtlTouchWork* touch) {
    return touch->holdFrames < 12 && touch->distance < 0x10000;
}

/// Counts taps; the fifth quick tap starts the tap effect.
static BOOL CombatTouch_CountRapidTaps(BtlTouchWork* touch) {
    if (!(touch->flags & 2)) {
        return FALSE;
    }
    if (touch->tapTimer > 0) {
        touch->tapCount++;
    }
    touch->tapTimer = 0x22;
    if (touch->tapCount < 5) {
        return FALSE;
    }
    EasyTask_CreateTask(&g_CombatGlobal->unk_3C2BC, &Tsk_BtlTch_Click, NULL, 0, NULL, NULL);
    return TRUE;
}

/// A short touch that travelled far: a flick.
static BOOL CombatTouch_IsSlash(BtlTouchWork* touch) {
    if (!(touch->flags & 4)) {
        return FALSE;
    }
    if (touch->holdFrames < 12 && touch->distance > 0x10000) {
        return TRUE;
    }
    return FALSE;
}

/// A long touch that stays near where it started.
static BOOL CombatTouch_IsHold(BtlTouchWork* touch) {
    s32 dx;
    s32 dy;

    if (!(touch->flags & 8)) {
        return FALSE;
    }
    if (touch->watching == 0) {
        return FALSE;
    }
    dx = (touch->startX - touch->x) << 12;
    dy = (touch->startY - touch->y) << 12;
    if (dx < 0) {
        dx = -dx;
    }
    if (dx > 0x4000 || ABS(dy) > 0x4000) {
        touch->watching = 0;
        return FALSE;
    }
    touch->watchFrames++;
    return TRUE;
}

/// A fast back and forth scribble.
static BOOL CombatTouch_IsScribble(BtlTouchWork* touch) {
    s32 speed;

    if (!(touch->flags & 0x20)) {
        return FALSE;
    }
    speed = FX_Sqrt(FX_Mul(touch->velocityX << 12, touch->velocityX << 12) +
                    FX_Mul(touch->velocityY << 12, touch->velocityY << 12));
    if (touch->sweepFrames == 0) {
        touch->sweepCenterX = touch->sweepMinX = touch->sweepMaxX = touch->x;
        touch->sweepCenterY = touch->sweepMinY = touch->sweepMaxY = touch->y;
    }
    if (speed >= 0xC000) {
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
static BOOL CombatTouch_IsOnActor(CombatActor* actor) {
    s32 x     = g_CombatGlobal->touchWork.x;
    s32 y     = g_CombatGlobal->touchWork.y;
    s32 flags = actor->flags;

    if ((flags & 4) || (flags & 0x10)) {
        return FALSE;
    }
    if (actor->unk_00 == 0) {
        s32 dx = actor->screenX - x;

        if (dx < 0) {
            dx = -dx;
        }
        if (dx < actor->unk_70 + 0x18) {
            if (actor->screenY + 0x1E >= y) {
                if (actor->screenY - actor->unk_72 - 0x12 <= y) {
                    return TRUE;
                }
            }
        }
    } else {
        s32 dx = actor->screenX - x;

        if (dx < 0) {
            dx = -dx;
        }
        if (dx <= actor->unk_70) {
            if (actor->screenY + 0xC >= y) {
                if (actor->screenY - actor->unk_72 <= y) {
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

/// Whether segment (x0,y0)-(x1,y1) touches the box of `actor`.
static BOOL CombatTouch_StrokeHitsActor(CombatActor* actor, s32 x0, s32 y0, s32 x1, s32 y1) {
    s32 left   = actor->screenX - actor->unk_70;
    s32 right  = actor->screenX + actor->unk_70;
    s32 bottom = actor->screenY;
    s32 top    = actor->screenY - actor->unk_72;
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
    if (CombatTouch_SegmentsCross(x0, y0, x1, y1, left, top, right, top)) {
        return TRUE;
    }
    if (CombatTouch_SegmentsCross(x0, y0, x1, y1, left, bottom, right, bottom)) {
        return TRUE;
    }
    if (CombatTouch_SegmentsCross(x0, y0, x1, y1, left, top, left, bottom)) {
        return TRUE;
    }
    return CombatTouch_SegmentsCross(x0, y0, x1, y1, right, top, right, bottom) != 0;
}

/// Keeps `actor` as the best candidate when the stroke crosses it near its middle.
static void CombatTouch_PickStrokeTarget(CombatActor** best, s32* bestDistance, CombatActor* actor) {
    Ov003Global* global = g_CombatGlobal;
    s16          x1     = global->touchWork.endX;
    s16          x0     = global->touchWork.startX;
    s16          y0     = global->touchWork.startY;
    s16          y1     = global->touchWork.endY;
    s32          distance;

    if (!CombatTouch_StrokeHitsActor(actor, x0, y0, x1, y1)) {
        return;
    }
    distance = CombatTouch_DistanceToActor(actor, (s16)((x0 + x1) / 2), (s16)((y0 + y1) / 2));
    if (distance < *bestDistance) {
        *bestDistance = distance;
        *best         = actor;
    }
}

/// Keeps `actor` as the best candidate when the touch point is on it and closer than the current best.
static void CombatTouch_PickPointTarget(CombatActor** best, s32* bestDistance, CombatActor* actor) {
    s32 distance;

    if (!CombatTouch_IsOnActor(actor)) {
        return;
    }
    distance = CombatTouch_DistanceToActor(actor, g_CombatGlobal->touchWork.x, g_CombatGlobal->touchWork.y);
    if (distance < *bestDistance) {
        *bestDistance = distance;
        *best         = actor;
    }
}

static void CombatTouch_PickEnemyAtPoint(CombatActor** best, s32* bestDistance) {
    CombatActor* actor = CombatList_GetHead(&g_CombatGlobal->unk3D8A4);

    if (actor == NULL) {
        return;
    }
    do {
        CombatTouch_PickPointTarget(best, bestDistance, actor);
        actor = CombatList_GetNext(&((Ov003EnemyActor*)actor)->node);
    } while (actor != NULL);
}

static void CombatTouch_PickObjectAtPoint(CombatActor** best, s32* bestDistance) {
    u8* item = CombatList_GetHead(&g_CombatGlobal->unk3D8B4);

    if (item == NULL) {
        return;
    }
    do {
        CombatTouch_PickPointTarget(best, bestDistance, (CombatActor*)(item + 4));
        item = CombatList_GetNext((CombatListNode*)(item + 0xB8));
    } while (item != NULL);
}

/// The actor under the touch point, the player first and then the closest enemy.
static CombatActor* CombatTouch_FindActorAtPoint(void) {
    CombatActor* best     = NULL;
    s32          distance = 0x7FFFFFFF;

    if (CombatTouch_IsOnActor(g_CombatGlobal->unk3D898)) {
        return g_CombatGlobal->unk3D898;
    }
    CombatTouch_PickEnemyAtPoint(&best, &distance);
    CombatTouch_PickObjectAtPoint(&best, &distance);
    return best;
}

/// The stylus came down.
static void CombatTouch_OnPress(BtlTouchWork* touch) {
    CombatActor* actor = CombatTouch_FindActorAtPoint();

    if (actor != NULL) {
        switch (actor->unk_00) {
            case 0:
                touch->targetKind = 1;
                break;
            case 1:
                touch->targetKind = 2;
                break;
            case 2:
                touch->targetKind = 3;
                break;
        }
        actor->flags |= 1;
        touch->target = actor;
    } else {
        touch->targetKind = 0;
        touch->target     = NULL;
    }
    touch->held = touch->target;
}

/// The stylus came up.
static void CombatTouch_OnRelease(BtlTouchWork* touch) {
    s32 dx;
    s32 dy;

    touch->endX     = touch->x;
    touch->endY     = touch->y;
    dx              = (touch->endX - touch->startX) << 12;
    dy              = (touch->endY - touch->startY) << 12;
    touch->distance = FX_Sqrt(FX_Mul(dx, dx) + FX_Mul(dy, dy));
    touch->angle    = FX_Atan2Idx(dy, dx);
    touch->released = touch->held;
    touch->held     = NULL;
}

/// Re-targets the held actor when the stylus slid off it.
static void CombatTouch_UpdateHeld(BtlTouchWork* touch) {
    if (touch->held != NULL) {
        if (CombatTouch_IsOnActor(touch->held)) {
            return;
        }
        touch->held = CombatTouch_FindActorAtPoint();
    } else {
        touch->held = CombatTouch_FindActorAtPoint();
    }
}

/// Classifies the touch of this frame: 0 none, 1 down, 2/3/4 tap kinds, 5 hold, 6 drag, 7 scribble.
static s32 CombatTouch_Classify(BtlTouchWork* touch) {
    if (TouchInput_WasTouchReleased()) {
        if (CombatTouch_IsQuickTap(touch)) {
            if (CombatTouch_CountRapidTaps(touch)) {
                return 3;
            }
            if (touch->flags & 1) {
                EasyTask_CreateTask(&g_CombatGlobal->unk_3C2BC, &Tsk_BtlTch_Click, NULL, 0, NULL, NULL);
                return 2;
            }
        }
        if (CombatTouch_IsSlash(touch)) {
            EasyTask_CreateTask(&g_CombatGlobal->unk_3C2BC, &Tsk_BtlTch_Slash, NULL, 0, NULL, NULL);
            return 4;
        }
    }
    if (TouchInput_IsTouchActive()) {
        if (touch->holdFrames >= 12) {
            if (CombatTouch_IsHold(touch)) {
                return 5;
            }
            if (CombatTouch_IsScribble(touch)) {
                return 7;
            }
            if (touch->flags & 0x10) {
                return 6;
            }
        }
        return 1;
    }
    return 0;
}

void CombatTouch_Clear(void) {
    g_CombatGlobal->touchWork.gesture       = 0;
    g_CombatGlobal->touchWork.active        = 0;
    g_CombatGlobal->touchWork.holdFrames    = 0;
    g_CombatGlobal->touchWork.releaseFrames = 0;
    g_CombatGlobal->touchWork.watchFrames   = 0;
    g_CombatGlobal->touchWork.targetKind    = 0;
    g_CombatGlobal->touchWork.target        = NULL;
    g_CombatGlobal->touchWork.held          = NULL;
    g_CombatGlobal->touchWork.released      = NULL;
}

/// Whether the touch state is in gesture `gesture`.
BOOL CombatTouch_IsGesture(u16 gesture) {
    if (g_CombatGlobal->touchWork.active != 0) {
        return g_CombatGlobal->touchWork.gesture == gesture;
    }
    return FALSE;
}

BOOL CombatTouch_IsTargetKind(s32 kind) {
    return g_CombatGlobal->touchWork.targetKind == kind;
}

/// Whether the current drag crosses `actor`.
BOOL CombatTouch_SlashCrosses(CombatActor* actor) {
    Ov003Global* global;

    if (!CombatTouch_IsGesture(4)) {
        return FALSE;
    }
    global = g_CombatGlobal;
    return CombatTouch_StrokeHitsActor(actor, global->touchWork.startX, global->touchWork.startY, global->touchWork.endX,
                                       global->touchWork.endY);
}

/// The actor of `kind` under the stylus on a tap.
CombatActor* CombatTouch_GetTapped(s32 kind) {
    CombatActor* best     = NULL;
    s32          distance = 0x7FFFFFFF;

    if (!CombatTouch_IsGesture(2)) {
        return NULL;
    }
    switch (kind) {
        case 0:
            if (CombatTouch_IsOnActor(g_CombatGlobal->unk3D898)) {
                best = g_CombatGlobal->unk3D898;
            }
            break;
        case 1:
            CombatTouch_PickEnemyAtPoint(&best, &distance);
            break;
        case 2:
            CombatTouch_PickObjectAtPoint(&best, &distance);
            break;
    }
    return best;
}

/// The held actor, when it is of side `side`.
CombatActor* CombatTouch_GetDragged(s32 side) {
    CombatActor* held = g_CombatGlobal->touchWork.held;

    if (!CombatTouch_IsGesture(6)) {
        return NULL;
    }
    if (held != NULL && held->unk_00 != side) {
        held = NULL;
    }
    return held;
}

/// The actor of `kind` the drag crosses closest to its middle.
CombatActor* CombatTouch_GetSlashed(s32 kind) {
    s32          distance;
    CombatActor* best = NULL;

    if (!CombatTouch_IsGesture(4)) {
        return NULL;
    }
    if (kind == 0) {
        Ov003Global* global = g_CombatGlobal;

        if (CombatTouch_StrokeHitsActor(global->unk3D898, global->touchWork.startX, global->touchWork.startY,
                                        global->touchWork.endX, global->touchWork.endY))
        {
            best = g_CombatGlobal->unk3D898;
        }
    }
    if (kind == 1) {
        CombatActor* actor;

        best     = NULL;
        distance = 0x7FFFFFFF;
        actor    = CombatList_GetHead(&g_CombatGlobal->unk3D8A4);
        if (actor != NULL) {
            do {
                CombatTouch_PickStrokeTarget(&best, &distance, actor);
                actor = CombatList_GetNext(&((Ov003EnemyActor*)actor)->node);
            } while (actor != NULL);
        }
    }
    if (kind == 2) {
        u8* item;

        best     = NULL;
        distance = 0x7FFFFFFF;
        item     = CombatList_GetHead(&g_CombatGlobal->unk3D8B4);
        if (item != NULL) {
            do {
                CombatTouch_PickStrokeTarget(&best, &distance, (CombatActor*)(item + 4));
                item = CombatList_GetNext((CombatListNode*)(item + 0xB8));
            } while (item != NULL);
        }
    }
    return best;
}

/// The actor of `kind` under the stylus on a flick.
CombatActor* CombatTouch_GetRapidTapped(s32 kind) {
    CombatActor* best     = NULL;
    s32          distance = 0x7FFFFFFF;

    if (!CombatTouch_IsGesture(3)) {
        return NULL;
    }
    switch (kind) {
        case 0:
            if (CombatTouch_IsOnActor(g_CombatGlobal->unk3D898)) {
                best = g_CombatGlobal->unk3D898;
            }
            break;
        case 1:
            CombatTouch_PickEnemyAtPoint(&best, &distance);
            break;
        case 2:
            CombatTouch_PickObjectAtPoint(&best, &distance);
            break;
    }
    return best;
}

void CombatTouch_Init(void) {
    BtlTouchWork* touch = &g_CombatGlobal->touchWork;

    touch->active      = 0;
    touch->flags       = 0;
    touch->targetKind  = 0;
    touch->held        = NULL;
    touch->target      = NULL;
    touch->released    = NULL;
    touch->tapTimer    = 0;
    touch->tapCount    = 0;
    touch->watching    = 1;
    touch->watchFrames = 0;
    touch->x           = 0x400;
    touch->y           = 0x400;
    touch->velocityX   = 0;
    touch->velocityY   = 0;
    touch->flags |= 0x15;
}

/// Reads the stylus and advances the gesture recognizer by one frame.
void CombatTouch_Update(void) {
    BtlTouchWork* touch = &g_CombatGlobal->touchWork;
    TouchCoord    coord;
    TouchCoord    delta;

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
    touch->active = 1;
    if (TouchInput_WasTouchPressed()) {
        touch->startX      = touch->x;
        touch->startY      = touch->y;
        touch->distance    = 0;
        touch->angle       = 0;
        touch->watching    = 1;
        touch->watchFrames = 0;
        touch->reserved24  = 0;
        touch->sweepFrames = 0;
        touch->stillFrames = 0;
        touch->anchorX     = touch->x;
        touch->anchorY     = touch->y;
        CombatTouch_OnPress(touch);
    }
    if (TouchInput_WasTouchReleased()) {
        CombatTouch_OnRelease(touch);
    }
    if (TouchInput_IsTouchActive()) {
        s32 dx;
        s32 dy;

        touch->holdFrames = TouchInput_GetHoldDuration();
        CombatTouch_UpdateHeld(touch);
        dx = touch->anchorX - touch->x;
        if (dx < 0) {
            dx = -dx;
        }
        if (dx < 4) {
            dy = touch->anchorY - touch->y;
            if (dy < 0) {
                dy = -dy;
            }
            if (dy < 4) {
                touch->stillFrames++;
                goto classify;
            }
        }
        touch->stillFrames = 0;
        touch->anchorX     = touch->x;
        touch->anchorY     = touch->y;
    } else {
        touch->releaseFrames = TouchInput_GetHoldDuration();
    }
classify:
    touch->gesture = CombatTouch_Classify(touch);
}
