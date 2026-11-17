#include "Battle/Core/BtlPsyInstant.h"
#include "Battle/Core/BtlActor.h"
#include "Battle/Core/BtlAttackQueue.h"
#include "Battle/Core/BtlObsMgr.h"
#include "Battle/Core/BtlPlayerUtil.h"
#include "Battle/Core/BtlSe.h"
#include "Battle/Core/BtlStage.h"
#include "Battle/Core/BtlTouch.h"

/// Whether the last stroke was mostly vertical: upwards for `direction == 0`, downwards for `direction == 1`.
// Nonmatching: register allocation of the four cached halfword positions
static BOOL BtlPsyInstant_IsVerticalStroke(s32 direction) {
    s16  startX = gBattle->touchWork.startX;
    s16  startY = gBattle->touchWork.startY;
    s16  endX   = gBattle->touchWork.endX;
    s16  endY   = gBattle->touchWork.endY;
    BOOL result = FALSE;

    switch (direction) {
        case 0: {
            s32 dy = startY - endY;
            s32 dx = startX - endX;

            if (dy < 0) {
                dy = -dy;
            }
            if (dx < 0) {
                dx = -dx;
            }
            if (dx < dy && startY > endY) {
                result = TRUE;
            }
            break;
        }
        case 1: {
            s32 dy = startY - endY;
            s32 dx = startX - endX;

            if (dy < 0) {
                dy = -dy;
            }
            if (dx < 0) {
                dx = -dx;
            }
            if (dx < dy && startY < endY) {
                result = TRUE;
            }
            break;
        }
    }
    return result;
}

/// Queues the player's attack on the tapped enemy.
static void BtlPsyInstant_FirePierce(BattleInstantAttack* data) {
    BtlAttackQueue_Push(data->effect, data->target, 0);
}

s32 BtlPsyInstant_StartPierce(BattleInstantAttackCmd* cmd, Ov003EffectState* effect) {
    BattleInstantAttack* data;
    BattleActor*         target = BtlTouch_GetTapped(ACTOR_KIND_ENEMY);

    if (target == NULL) {
        return 0;
    }
    data         = func_ov003_020a5d2c(sizeof(BattleInstantAttack), "Sword_Pierce_T");
    data->effect = effect;
    data->target = target;
    cmd->fire    = BtlPsyInstant_FirePierce;
    cmd->data    = data;
    return 1;
}

/// Queues the player's attack on the slashed enemy and signals the psych's badge.
static void BtlPsyInstant_FireSyoryu(BattleInstantAttack* data) {
    BtlAttackQueue_Push(data->effect, data->target, 0);
    func_ov003_020a5a94(data->effect);
}

s32 BtlPsyInstant_StartSyoryu(BattleInstantAttackCmd* cmd, Ov003EffectState* effect) {
    BattleInstantAttack* data;
    BattleActor*         target;

    if (!BtlPsyInstant_IsVerticalStroke(0)) {
        return 0;
    }
    target = BtlTouch_GetSlashed(ACTOR_KIND_ENEMY);
    if (target == NULL) {
        return 0;
    }
    data         = func_ov003_020a5d2c(sizeof(BattleInstantAttack), "ARG_Sword_Syoryu_S");
    data->effect = effect;
    data->target = target;
    cmd->fire    = BtlPsyInstant_FireSyoryu;
    cmd->data    = data;
    return 1;
}

/// Queues the player's attack on the slashed enemy and signals the psych's badge.
static void BtlPsyInstant_FireKabuto(BattleInstantAttack* data) {
    BtlAttackQueue_Push(data->effect, data->target, 0);
    func_ov003_020a5a94(data->effect);
}

s32 BtlPsyInstant_StartKabuto(BattleInstantAttackCmd* cmd, Ov003EffectState* effect) {
    BattleInstantAttack* data;
    BattleActor*         target;

    if (gBattle->moveSpeed < 100) {
        return 0;
    }
    if (!BtlPsyInstant_IsVerticalStroke(1)) {
        return 0;
    }
    target = BtlTouch_GetSlashed(ACTOR_KIND_ENEMY);
    if (target == NULL) {
        return 0;
    }
    data         = func_ov003_020a5d2c(sizeof(BattleInstantAttack), "ARG_Sword_Kabuto_S");
    data->effect = effect;
    data->target = target;
    cmd->fire    = BtlPsyInstant_FireKabuto;
    cmd->data    = data;
    return 1;
}

/// Warps the player to the tapped point and signals the psych's badge.
static void BtlPsyInstant_FireTeleport(BattleInstantAttack* data) {
    s32 x;
    s32 y;

    BtlSe_PlayAtMainX(SEIDX_SE_PSY_TELEPORT01_01, gBattle->player->position.x);
    x = BtlStage_ToWorldX(0, gBattle->touchWork.x);
    y = BtlStage_ToWorldY(0, gBattle->touchWork.y, 0);
    BtlStage_ClampToFloor(&x, &y, 0x8000, 0x8000);
    BtlSe_PlayAtMainX(SEIDX_SE_PSY_TELEPORT01_02, x);
    BtlPlayerUtil_WarpCharacter(0, x, y, 0);
    func_ov003_020a5aa4(data->effect);
    func_ov003_020a5a94(data->effect);
}

s32 BtlPsyInstant_StartTeleport(BattleInstantAttackCmd* cmd, Ov003EffectState* effect) {
    BattleInstantAttack* data;

    if (gBattle->moveSpeed < 100) {
        return 0;
    }
    if (!BtlTouch_IsGesture(BTL_TOUCH_GESTURE_TAP)) {
        return 0;
    }
    if (!BtlTouch_IsTargetKind(BTL_TOUCH_TARGET_NONE)) {
        return 0;
    }
    data         = func_ov003_020a5d2c(sizeof(Ov003EffectState*), "ARG_Teleport_T");
    data->effect = effect;
    cmd->fire    = BtlPsyInstant_FireTeleport;
    cmd->data    = data;
    return 1;
}
