#include "Battle/Core/BtlPlayerUtil.h"
#include "Battle/Core/BtlMgr.h"
#include "Battle/Core/BtlSe.h"
#include "Battle/Core/BtlStage.h"
#include "Battle/Core/BtlTouch.h"
#include "Combat/Core/BtlEff.h"
#include "Combat/Core/BtlPop.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatAbility.h"
#include "Engine/Math/Random.h"

#include <nitro/fx.h>

/// Ticks the level-up gauge: every 300 frames it banks a percentage of the gauge maximum.
void func_ov003_02087458(s32 mode, s16* timer) {
    BattleActor* actor;
    s16          amount;

    if (g_CombatGlobal->unk3D874 != 2) {
        return;
    }
    if (BtlMgr_HasAbility(mode, 0x3C) == 0) {
        return;
    }
    *timer += 1;
    if (*timer <= 0x12C) {
        return;
    }
    *timer = 0;
    amount = (g_CombatGlobal->unk3D8D2 * BtlMgr_GetAbilityValue(mode, 0x3C)) / 100;
    if (amount <= 0) {
        amount = 1;
    }
    BtlMgr_AddTeamHp(mode, amount);
    if (mode != 1) {
        actor = g_CombatGlobal->player;
    } else {
        actor = g_CombatGlobal->partner;
    }
    func_ov003_0209b564(mode, amount, actor, 1);
}

/// Advances the on-screen level-up gauge timer when every attack pin of the engine has been used.
void func_ov003_02087540(s32 mode) {
    BattleActor* actor;
    s32          ready;

    if (mode != 1) {
        actor = g_CombatGlobal->player;
    } else {
        actor = g_CombatGlobal->partner;
    }
    ready = 0;
    if (mode != 1) {
        if (g_CombatGlobal->unk3D8F4 != 0 && CombatAbility_IsActiveForPlayer(0x41) != 0 &&
            CombatAbility_IsActiveForPlayer(0x42) != 0 && CombatAbility_IsActiveForPlayer(0x43) != 0 &&
            CombatAbility_IsActiveForPlayer(0x44) != 0 && CombatAbility_IsActiveForPlayer(0x45) != 0)
        {
            ready = 1;
        }
    } else {
        if (g_CombatGlobal->unk3D8F0 != 0 && CombatAbility_IsActiveForPartner(0x41) != 0 &&
            CombatAbility_IsActiveForPartner(0x42) != 0 && CombatAbility_IsActiveForPartner(0x43) != 0 &&
            CombatAbility_IsActiveForPartner(0x44) != 0 && CombatAbility_IsActiveForPartner(0x45) != 0)
        {
            ready = 1;
        }
    }
    if (ready == 0) {
        return;
    }
    if (actor->invincibleTimer <= 0) {
        actor->invincibleTimer++;
    }
}

u16 func_ov003_0208765c(u16* state, s32 value, s32 arg2) {
    u16 previous = *state;
    s32 half     = g_CombatGlobal->unk3D8D2 / 2;

    if (value >= half) {
        *state = 2;
    } else if (arg2 >= 0xF0) {
        *state = 3;
    } else {
        *state = 1;
    }
    if (arg2 != 0 && *state == previous) {
        return 0;
    }
    return *state;
}

s32 func_ov003_020876c0(void) {
    s32 flags = g_CombatGlobal->player->flags;

    if ((flags & ACTOR_FLAG_DEFEATED) || (flags & ACTOR_FLAG_STAGGERED) || (g_CombatGlobal->unk3D878 & 8)) {
        return 1;
    }
    return 0;
}

/// TRUE when the main display actor is far enough from the projected sub-screen position.
s32 func_ov003_02087704(void) {
    BattleActor* actor;
    s32          x;
    s32          y;
    s32          dx;
    s32          dy;

    if (BtlTouch_IsGesture(BTL_TOUCH_GESTURE_DRAG) == 0 || BtlTouch_IsTargetKind(BTL_TOUCH_TARGET_PLAYER) == 0) {
        return 0;
    }
    x = BtlStage_ToWorldX(0, g_CombatGlobal->touchWork.x);
    y = BtlStage_ToWorldY(0, g_CombatGlobal->touchWork.y, 0);
    BtlStage_ClampToFloor(&x, &y, 0x8000, 0x8000);
    actor = g_CombatGlobal->player;
    dx    = x - actor->position.x;
    dy    = y - actor->position.y;
    if ((FX_Sqrt(FX_Mul(dx, dx) + FX_Mul(dy, dy)) >> 12) > 0x50) {
        return 1;
    }
    return 0;
}

/// Plays one of the two hit-reaction SE sets, picked at random.
void func_ov003_02087804(s32 sub) {
    u16 roll;

    if (sub != 0) {
        roll = RNG_Next(0xFFFF) % 9;
        switch (roll) {
            case 0:
                BtlSe_PlayAtMainX(0x35A, g_CombatGlobal->player->position.x);
                return;
            case 1:
                BtlSe_PlayAtMainX(0x35B, g_CombatGlobal->player->position.x);
                return;
            case 2:
                BtlSe_PlayAtMainX(0x35C, g_CombatGlobal->player->position.x);
                return;
            case 3:
                BtlSe_PlayAtMainX(0x35D, g_CombatGlobal->player->position.x);
                return;
            case 4:
                BtlSe_PlayAtMainX(0x35E, g_CombatGlobal->player->position.x);
                return;
            case 5:
                BtlSe_PlayAtMainX(0x35F, g_CombatGlobal->player->position.x);
                return;
            case 6:
                BtlSe_PlayAtMainX(0x360, g_CombatGlobal->player->position.x);
                return;
            case 7:
                BtlSe_PlayAtMainX(0x361, g_CombatGlobal->player->position.x);
                return;
            case 8:
                BtlSe_PlayAtMainX(0x362, g_CombatGlobal->player->position.x);
                return;
        }
    }
    roll = RNG_Next(0xFFFF) % 8;
    switch (roll) {
        case 0:
            BtlSe_PlayAtMainX(0x352, g_CombatGlobal->player->position.x);
            return;
        case 1:
            BtlSe_PlayAtMainX(0x353, g_CombatGlobal->player->position.x);
            return;
        case 2:
            BtlSe_PlayAtMainX(0x354, g_CombatGlobal->player->position.x);
            return;
        case 3:
            BtlSe_PlayAtMainX(0x355, g_CombatGlobal->player->position.x);
            return;
        case 4:
            BtlSe_PlayAtMainX(0x356, g_CombatGlobal->player->position.x);
            return;
        case 5:
            BtlSe_PlayAtMainX(0x357, g_CombatGlobal->player->position.x);
            return;
        case 6:
            BtlSe_PlayAtMainX(0x358, g_CombatGlobal->player->position.x);
            return;
        case 7:
            BtlSe_PlayAtMainX(0x359, g_CombatGlobal->player->position.x);
            return;
    }
}

/// Moves the display actor of one engine to a new position and tells the HUD about it.
void BtlPlayerUtil_WarpCharacter(s32 mode, s32 x, s32 y, s32 z) {
    BattleActor* actor;
    s32          oldY;
    s32          oldZ;
    s32          oldX;

    if (mode != 1) {
        actor = g_CombatGlobal->player;
    } else {
        actor = g_CombatGlobal->partner;
    }
    oldX = actor->position.x;
    oldY = actor->position.y;
    oldZ = actor->position.z;
    if (oldX > x) {
        actor->isFlipped = 0;
    }
    if (actor->position.x < x) {
        actor->isFlipped = 1;
    }
    actor->position.x = x;
    actor->position.y = y;
    actor->position.z = z;
    func_ov003_0209f770(mode, oldX, oldY, oldZ);
}
