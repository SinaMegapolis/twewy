#include "Battle/Core/BtlPlayerUtil.h"
#include "Battle/Core/BtlMgr.h"
#include "Battle/Core/BtlObsMgr.h"
#include "Battle/Core/BtlSe.h"
#include "Battle/Core/BtlStage.h"
#include "Battle/Core/BtlTouch.h"
#include "Combat/Core/BtlEff.h"
#include "Combat/Core/BtlPop.h"
#include "Combat/Core/CombatAbility.h"
#include "Engine/Math/Random.h"

#include <nitro/fx.h>

void BtlPlayerUtil_UpdateRegen(s32 side, s16* timer) {
    if (gBattle->state == BATTLE_STATE_FIGHT && BtlMgr_HasAbility(side, 0x3C)) {
        *timer += 1;
        if (*timer > 300) {
            BattleActor* actor;
            s16          amount;

            *timer = 0;
            amount = (gBattle->teamMaxHp * BtlMgr_GetAbilityValue(side, 0x3C)) / 100;
            if (amount <= 0) {
                amount = 1;
            }
            BtlMgr_AddTeamHp(side, amount);
            if (side != 1) {
                actor = gBattle->player;
            } else {
                actor = gBattle->partner;
            }
            func_ov003_0209b564(side, amount, actor, 1);
        }
    }
}

void BtlPlayerUtil_UpdateAuraInvincibility(s32 side) {
    BattleActor* actor;
    BOOL         ready;

    if (side != 1) {
        actor = gBattle->player;
    } else {
        actor = gBattle->partner;
    }
    ready = FALSE;
    if (side != 1) {
        if (gBattle->auraOnPlayer && CombatAbility_IsActiveForPlayer(0x41) && CombatAbility_IsActiveForPlayer(0x42) &&
            CombatAbility_IsActiveForPlayer(0x43) && CombatAbility_IsActiveForPlayer(0x44) &&
            CombatAbility_IsActiveForPlayer(0x45))
        {
            ready = TRUE;
        }
    } else {
        if (gBattle->auraOnPartner && CombatAbility_IsActiveForPartner(0x41) && CombatAbility_IsActiveForPartner(0x42) &&
            CombatAbility_IsActiveForPartner(0x43) && CombatAbility_IsActiveForPartner(0x44) &&
            CombatAbility_IsActiveForPartner(0x45))
        {
            ready = TRUE;
        }
    }
    if (ready && actor->invincibleTimer <= 0) {
        actor->invincibleTimer++;
    }
}

u16 BtlPlayerUtil_UpdateIdlePose(u16* pose, s32 damageTaken, s32 idleFrames) {
    u16 previous = *pose;
    s32 half     = gBattle->teamMaxHp / 2;

    if (damageTaken >= half) {
        *pose = BTL_IDLE_POSE_HURT;
    } else if (idleFrames >= 240) {
        *pose = BTL_IDLE_POSE_BORED;
    } else {
        *pose = BTL_IDLE_POSE_NORMAL;
    }
    if (idleFrames != 0 && *pose == previous) {
        return BTL_IDLE_POSE_UNCHANGED;
    }
    return *pose;
}

BOOL BtlPlayerUtil_IsPlayerDown(void) {
    s32 flags = gBattle->player->flags;

    if ((flags & ACTOR_FLAG_DEFEATED) || (flags & ACTOR_FLAG_STAGGERED) || (gBattle->flags & BATTLE_FLAG_LEVEL_UP)) {
        return TRUE;
    }
    return FALSE;
}

BOOL BtlPlayerUtil_IsLongPlayerDrag(void) {
    BattleActor* actor;
    s32          x;
    s32          y;
    s32          dx;
    s32          dy;

    if (!BtlTouch_IsGesture(BTL_TOUCH_GESTURE_DRAG) || !BtlTouch_IsTargetKind(BTL_TOUCH_TARGET_PLAYER)) {
        return FALSE;
    }
    x = BtlStage_ToWorldX(0, gBattle->touchWork.x);
    y = BtlStage_ToWorldY(0, gBattle->touchWork.y, 0);
    BtlStage_ClampToFloor(&x, &y, 0x8000, 0x8000);
    actor = gBattle->player;
    dx    = x - actor->position.x;
    dy    = y - actor->position.y;
    if ((FX_Sqrt(FX_Mul(dx, dx) + FX_Mul(dy, dy)) >> 12) > 80) {
        return TRUE;
    }
    return FALSE;
}

void BtlPlayerUtil_PlayAttackVoice(BOOL finisher) {
    u16 roll;

    if (finisher) {
        roll = RNG_Next(0xFFFF) % 9;
        switch (roll) {
            case 0:
                BtlSe_PlayAtMainX(SEIDX_NEKU_BTL_FB01, gBattle->player->position.x);
                return;
            case 1:
                BtlSe_PlayAtMainX(SEIDX_NEKU_BTL_FB02, gBattle->player->position.x);
                return;
            case 2:
                BtlSe_PlayAtMainX(SEIDX_NEKU_BTL_FB03, gBattle->player->position.x);
                return;
            case 3:
                BtlSe_PlayAtMainX(SEIDX_NEKU_BTL_FB04, gBattle->player->position.x);
                return;
            case 4:
                BtlSe_PlayAtMainX(SEIDX_NEKU_BTL_FB05, gBattle->player->position.x);
                return;
            case 5:
                BtlSe_PlayAtMainX(SEIDX_NEKU_BTL_FB06, gBattle->player->position.x);
                return;
            case 6:
                BtlSe_PlayAtMainX(SEIDX_NEKU_BTL_FB07, gBattle->player->position.x);
                return;
            case 7:
                BtlSe_PlayAtMainX(SEIDX_NEKU_BTL_FB08, gBattle->player->position.x);
                return;
            case 8:
                BtlSe_PlayAtMainX(SEIDX_NEKU_BTL_FB09, gBattle->player->position.x);
                return;
        }
    }
    roll = RNG_Next(0xFFFF) % 8;
    switch (roll) {
        case 0:
            BtlSe_PlayAtMainX(SEIDX_NEKU_BTL_ATC01, gBattle->player->position.x);
            return;
        case 1:
            BtlSe_PlayAtMainX(SEIDX_NEKU_BTL_ATC02, gBattle->player->position.x);
            return;
        case 2:
            BtlSe_PlayAtMainX(SEIDX_NEKU_BTL_ATC03, gBattle->player->position.x);
            return;
        case 3:
            BtlSe_PlayAtMainX(SEIDX_NEKU_BTL_ATC04, gBattle->player->position.x);
            return;
        case 4:
            BtlSe_PlayAtMainX(SEIDX_NEKU_BTL_ATC05, gBattle->player->position.x);
            return;
        case 5:
            BtlSe_PlayAtMainX(SEIDX_NEKU_BTL_ATC06, gBattle->player->position.x);
            return;
        case 6:
            BtlSe_PlayAtMainX(SEIDX_NEKU_BTL_ATC07, gBattle->player->position.x);
            return;
        case 7:
            BtlSe_PlayAtMainX(SEIDX_NEKU_BTL_ATC08, gBattle->player->position.x);
            return;
    }
}

void BtlPlayerUtil_WarpCharacter(s32 side, s32 x, s32 y, s32 z) {
    BattleActor* actor;
    s32          oldY;
    s32          oldZ;
    s32          oldX;

    if (side != 1) {
        actor = gBattle->player;
    } else {
        actor = gBattle->partner;
    }
    oldX = actor->position.x;
    oldY = actor->position.y;
    oldZ = actor->position.z;
    if (oldX > x) {
        actor->isFlipped = FALSE;
    }
    if (actor->position.x < x) {
        actor->isFlipped = TRUE;
    }
    actor->position.x = x;
    actor->position.y = y;
    actor->position.z = z;
    func_ov003_0209f770(side, oldX, oldY, oldZ);
}
