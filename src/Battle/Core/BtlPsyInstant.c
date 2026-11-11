#include "Battle/Core/BtlPsyInstant.h"
#include "Battle/Core/BtlActor.h"
#include "Battle/Core/BtlAttackQueue.h"
#include "Battle/Core/BtlPlayerUtil.h"
#include "Battle/Core/BtlSe.h"
#include "Battle/Core/BtlStage.h"
#include "Battle/Core/BtlTouch.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatPsychEffects.h"

/// TRUE when the last stroke was mostly vertical: upwards for `dir == 0`, downwards for `dir == 1`.
// Nonmatching: register allocation of the four cached halfword positions
static s32 BtlPsyInstant_IsVerticalStroke(s32 arg0) {
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

static void BtlPsyInstant_FirePierce(BattleInstantAttack* data) {
    func_ov003_02087250(data->effect, data->target, 0);
}

/// Registers the "pierce" attack: the handler is `BtlPsyInstant_FirePierce`.
s32 BtlPsyInstant_StartPierce(BattleInstantAttackCmd* cmd, Ov003EffectState* effect) {
    BattleInstantAttack* data;
    BattleActor*         state = BtlTouch_GetTapped(ACTOR_KIND_ENEMY);

    if (state == 0) {
        return 0;
    }
    data         = (BattleInstantAttack*)func_ov003_020a5d2c(8, "Sword_Pierce_T");
    data->effect = effect;
    data->target = state;
    cmd->fire    = BtlPsyInstant_FirePierce;
    cmd->data    = data;
    return 1;
}

static void BtlPsyInstant_FireSyoryu(BattleInstantAttack* data) {
    func_ov003_02087250(data->effect, data->target, 0);
    func_ov003_020a5a94(data->effect);
}

/// Registers the "slash" attack: the handler is `BtlPsyInstant_FireSyoryu`.
s32 BtlPsyInstant_StartSyoryu(BattleInstantAttackCmd* cmd, Ov003EffectState* effect) {
    BattleInstantAttack* data;
    BattleActor*         state;

    if (BtlPsyInstant_IsVerticalStroke(0) == 0) {
        return 0;
    }
    state = BtlTouch_GetSlashed(ACTOR_KIND_ENEMY);
    if (state == 0) {
        return 0;
    }
    data         = (BattleInstantAttack*)func_ov003_020a5d2c(8, "ARG_Sword_Syoryu_S");
    data->effect = effect;
    data->target = state;
    cmd->fire    = BtlPsyInstant_FireSyoryu;
    cmd->data    = data;
    return 1;
}

static void BtlPsyInstant_FireKabuto(BattleInstantAttack* data) {
    func_ov003_02087250(data->effect, data->target, 0);
    func_ov003_020a5a94(data->effect);
}

/// Registers the "kabuto" attack: the handler is `BtlPsyInstant_FireKabuto`.
s32 BtlPsyInstant_StartKabuto(BattleInstantAttackCmd* cmd, Ov003EffectState* effect) {
    BattleInstantAttack* data;
    BattleActor*         state;

    if (g_CombatGlobal->unk3D894 < 0x64) {
        return 0;
    }
    if (BtlPsyInstant_IsVerticalStroke(1) == 0) {
        return 0;
    }
    state = BtlTouch_GetSlashed(ACTOR_KIND_ENEMY);
    if (state == 0) {
        return 0;
    }
    data         = (BattleInstantAttack*)func_ov003_020a5d2c(8, "ARG_Sword_Kabuto_S");
    data->effect = effect;
    data->target = state;
    cmd->fire    = BtlPsyInstant_FireKabuto;
    cmd->data    = data;
    return 1;
}

static void BtlPsyInstant_FireTeleport(BattleInstantAttack* data) {
    s32 x;
    s32 y;

    BtlSe_PlayAtMainX(0x3B, g_CombatGlobal->player->position.x);
    x = BtlStage_ToWorldX(0, g_CombatGlobal->touchWork.x);
    y = BtlStage_ToWorldY(0, g_CombatGlobal->touchWork.y, 0);
    BtlStage_ClampToFloor(&x, &y, 0x8000, 0x8000);
    BtlSe_PlayAtMainX(0x3C, x);
    BtlPlayerUtil_WarpCharacter(0, x, y, 0);
    func_ov003_020a5aa4(data->effect);
    func_ov003_020a5a94(data->effect);
}

/// Registers the "teleport" attack: the handler is `BtlPsyInstant_FireTeleport`.
s32 BtlPsyInstant_StartTeleport(BattleInstantAttackCmd* cmd, Ov003EffectState* effect) {
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
    data      = (Ov003EffectState**)func_ov003_020a5d2c(4, "ARG_Teleport_T");
    *data     = effect;
    cmd->fire = (void (*)(BattleInstantAttack*))BtlPsyInstant_FireTeleport;
    cmd->data = (BattleInstantAttack*)data;
    return 1;
}
