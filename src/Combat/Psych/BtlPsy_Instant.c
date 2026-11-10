#include "Battle/Core/BtlActor.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatPsychEffects.h"

static char BtlPsy_Pierce_ArgName[0x10]   = "Sword_Pierce_T";
static char BtlPsy_Syoryu_ArgName[0x14]   = "ARG_Sword_Syoryu_S";
static char BtlPsy_Kabuto_ArgName[0x14]   = "ARG_Sword_Kabuto_S";
static char BtlPsy_Teleport_ArgName[0x10] = "ARG_Teleport_T";

/// TRUE when the last stroke was mostly vertical: upwards for `dir == 0`, downwards for `dir == 1`.
// Nonmatching: register allocation of the four cached halfword positions
static s32 BtlPsy_Instant_IsVerticalStroke(s32 arg0) {
    s16 curX   = g_CombatGlobal->touchWork.startX;
    s16 curY   = g_CombatGlobal->touchWork.startY;
    s16 prevX  = g_CombatGlobal->touchWork.endX;
    s16 prevY  = g_CombatGlobal->touchWork.endY;
    s32 result = 0;

    switch (arg0) {
        case 0: {
            s32 dx = curY - prevY;
            s32 dy = curX - prevX;

            if (dx < 0) {
                dx = -dx;
            }
            if (dy < 0) {
                dy = -dy;
            }
            if (dy < dx && curY > prevY) {
                result = 1;
            }
            break;
        }
        case 1: {
            s32 dx = curY - prevY;
            s32 dy = curX - prevX;

            if (dx < 0) {
                dx = -dx;
            }
            if (dy < 0) {
                dy = -dy;
            }
            if (dy < dx && curY < prevY) {
                result = 1;
            }
            break;
        }
    }
    return result;
}

static void BtlPsy_Pierce_Fire(Ov003AttackData* data) {
    func_ov003_02087250(data->effect, data->target, 0);
}

/// Registers the "pierce" attack: the handler is `BtlPsy_Pierce_Fire`.
s32 BtlPsy_Pierce_Start(Ov003AttackCmd* cmd, Ov003EffectState* effect) {
    Ov003AttackData* data;
    BattleActor*     state = BtlTouch_GetTapped(ACTOR_KIND_ENEMY);

    if (state == 0) {
        return 0;
    }
    data         = (Ov003AttackData*)func_ov003_020a5d2c(8, BtlPsy_Pierce_ArgName);
    data->effect = effect;
    data->target = state;
    cmd->fire    = BtlPsy_Pierce_Fire;
    cmd->data    = data;
    return 1;
}

static void BtlPsy_Syoryu_Fire(Ov003AttackData* data) {
    func_ov003_02087250(data->effect, data->target, 0);
    func_ov003_020a5a94(data->effect);
}

/// Registers the "slash" attack: the handler is `BtlPsy_Syoryu_Fire`.
s32 BtlPsy_Syoryu_Start(Ov003AttackCmd* cmd, Ov003EffectState* effect) {
    Ov003AttackData* data;
    BattleActor*     state;

    if (BtlPsy_Instant_IsVerticalStroke(0) == 0) {
        return 0;
    }
    state = BtlTouch_GetSlashed(ACTOR_KIND_ENEMY);
    if (state == 0) {
        return 0;
    }
    data         = (Ov003AttackData*)func_ov003_020a5d2c(8, BtlPsy_Syoryu_ArgName);
    data->effect = effect;
    data->target = state;
    cmd->fire    = BtlPsy_Syoryu_Fire;
    cmd->data    = data;
    return 1;
}

static void BtlPsy_Kabuto_Fire(Ov003AttackData* data) {
    func_ov003_02087250(data->effect, data->target, 0);
    func_ov003_020a5a94(data->effect);
}

/// Registers the "kabuto" attack: the handler is `BtlPsy_Kabuto_Fire`.
s32 BtlPsy_Kabuto_Start(Ov003AttackCmd* cmd, Ov003EffectState* effect) {
    Ov003AttackData* data;
    BattleActor*     state;

    if (g_CombatGlobal->unk3D894 < 0x64) {
        return 0;
    }
    if (BtlPsy_Instant_IsVerticalStroke(1) == 0) {
        return 0;
    }
    state = BtlTouch_GetSlashed(ACTOR_KIND_ENEMY);
    if (state == 0) {
        return 0;
    }
    data         = (Ov003AttackData*)func_ov003_020a5d2c(8, BtlPsy_Kabuto_ArgName);
    data->effect = effect;
    data->target = state;
    cmd->fire    = BtlPsy_Kabuto_Fire;
    cmd->data    = data;
    return 1;
}

static void BtlPsy_Teleport_Fire(Ov003AttackData* data) {
    s32 x;
    s32 y;

    CombatSe_PlayAtMainX(0x3B, g_CombatGlobal->player->position.x);
    x = BtlStage_ToWorldX(0, g_CombatGlobal->touchWork.x);
    y = BtlStage_ToWorldY(0, g_CombatGlobal->touchWork.y, 0);
    BtlStage_ClampToFloor(&x, &y, 0x8000, 0x8000);
    CombatSe_PlayAtMainX(0x3C, x);
    CombatSys_WarpCharacter(0, x, y, 0);
    func_ov003_020a5aa4(data->effect);
    func_ov003_020a5a94(data->effect);
}

/// Registers the "teleport" attack: the handler is `BtlPsy_Teleport_Fire`.
s32 BtlPsy_Teleport_Start(Ov003AttackCmd* cmd, Ov003EffectState* effect) {
    Ov003EffectState** data;

    if (g_CombatGlobal->unk3D894 < 0x64) {
        return 0;
    }
    if (BtlTouch_IsGesture(BTL_TOUCH_GESTURE_TAP) == 0) {
        return 0;
    }
    if (BtlTouch_IsTargetKind(BTL_TOUCH_TARGET_NONE) == 0) {
        return 0;
    }
    data      = (Ov003EffectState**)func_ov003_020a5d2c(4, BtlPsy_Teleport_ArgName);
    *data     = effect;
    cmd->fire = (void (*)(Ov003AttackData*))BtlPsy_Teleport_Fire;
    cmd->data = (Ov003AttackData*)data;
    return 1;
}
