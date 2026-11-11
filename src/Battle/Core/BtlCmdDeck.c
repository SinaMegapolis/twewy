#include "Battle/Core/BtlCmdDeck.h"
#include "Combat/Core/BtlPop.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatAbility.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Friend/Private/Beat.h"
#include "Combat/Friend/Private/Shiki.h"
#include "Combat/Friend/Shiki.h"
#include "Engine/Math/Random.h"
#include "Player/Inventory.h"
#include "Save.h"

#include <nitro/fx.h>
#include <nitro/math.h>

/// Frame counts of the three sync-gauge charge steps (one row per mode).
static const s8 data_ov003_020d7614[4][3] = {
    {4, 8, 16},
    {4, 8, 16},
    {4, 8, 16},
    {0, 0,  0},
};

/// Advances the charge step until it covers the accumulated progress. Returns TRUE if it moved.
static s32 func_ov003_02086b88(BattleCmdDeck* cmd) {
    s32 advanced = 0;

    while (cmd->progress >= cmd->next) {
        if (cmd->current >= cmd->count) {
            break;
        }
        cmd->current++;
        cmd->next = cmd->stage + data_ov003_020d7614[cmd->mode][cmd->current];
        advanced  = 1;
    }
    return advanced;
}

/// Shuffles `table[0..count-1]` into a random permutation (insertion shuffle).
void BtlCmdDeck_Shuffle(u16* table, u32 count) {
    u16 value = 0;
    s32 i     = 0;

    if (count == 0) {
        return;
    }
    do {
        s32 target = RNG_Next(i + 1);
        s32 j      = i;

        while (j > target) {
            table[j] = table[j - 1];
            j--;
        }
        table[target] = value;
        value++;
        i = (s16)(i + 1);
    } while (count > value);
}

/// Eases `*value` towards the sine table entry `*index` by an eighth of the difference.
void BtlCmdDeck_EaseToSine(s32* value, s16* index) {
    *value += ((SIN2(FX_SinCosTable_, (u16)(*index << 9)) * 4) - *value) / 8;
    *index += 1;
}

/// Refills the effect command's queue with a fresh shuffle.
void func_ov003_02086cc0(void) {
    BattleCmdDeck* cmd = g_CombatGlobal->cmdDeck;

    switch (cmd->mode) {
        case 1:
            cmd->remaining = func_ov005_020e74fc(&cmd->queue[0], cmd->capacity, cmd->unk_2C);
            break;
        case 0:
        case 2:
        default:
            BtlCmdDeck_Shuffle(&cmd->queue[0], cmd->capacity);
            cmd->remaining = cmd->capacity;
            break;
    }
}

u16 func_ov003_02086d24(void) {
    BattleCmdDeck* cmd = g_CombatGlobal->cmdDeck;
    u16            value;

    if (cmd->mode == 2) {
        return func_ov006_020e7538(cmd->unk_2C);
    }
    if (cmd->remaining == 0) {
        func_ov003_02086cc0();
    }
    cmd->remaining--;
    value                      = cmd->queue[cmd->remaining];
    cmd->queue[cmd->remaining] = 0xFFFF;
    return value;
}

/// Draws one battle effect frame: a sprite animation for the charge effects, a delegated draw for
/// the special ones.
void func_ov003_02086d8c(BattleSprite* sprite, s32 animId, s32 flipped, s32 x, s16 y, s16 z) {
    switch (g_CombatGlobal->cmdDeck->mode) {
        case 0:
        case 1: {
            u16 anim = animId * 2;

            if (flipped != 0) {
                anim++;
            }
            BtlSprite_SetAnim(sprite, anim, 0);
            BtlSprite_Update(sprite);
            BtlSprite_SetPosition(sprite, x, y);
            BtlSprite_Render(sprite);
            break;
        }
        case 2:
            func_ov006_020e73b8(sprite, animId, flipped, x, y, z);
            break;
    }
}

/// Fires the effect command: the charge effects add a card attack, the special ones spawn their
/// own task.
void func_ov003_02086e38(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    BattleCmdDeck* cmd = g_CombatGlobal->cmdDeck;
    s32            value;

    if (cmd->flags & 1) {
        return;
    }
    value = arg1 + (arg3 << 12);
    switch (cmd->mode) {
        case 2:
            func_ov006_020ea6c0(arg0, value, arg2);
            break;
        case 1:
            func_ov005_020eb29c(arg0, value, arg2);
            break;
        case 0:
        default:
            BtlCmd_Esp_CreateTask(arg0, value, arg2);
            break;
    }
}

s16* func_ov003_02086e94(void) {
    return (s16*)g_CombatGlobal->cmdDeck->unk_2C;
}

/// Number of charge steps the active partner's sticker grants, clamped to 1..3.
static u8 func_ov003_02086eb0(void) {
    u32 count = 0;

    switch (g_CombatGlobal->unk3D88D) {
        case 0:
            count = Inventory_GetOwnedCount(0x2B0);
            break;
        case 1:
            count = Inventory_GetOwnedCount(0x2B8);
            break;
        case 2:
            count = Inventory_GetOwnedCount(0x2C0);
            break;
        default:
            break;
    }
    if (count < 1) {
        count = 1;
    }
    if (count > 3) {
        count = 3;
    }
    return count;
}

s32 func_ov003_02086f20(s32 arg0, s32 arg1) {
    BattleCmdDeck* cmd = g_CombatGlobal->cmdDeck;

    if (cmd->current >= cmd->count) {
        return 0;
    }
    func_ov003_0209d038(arg0, arg1 + 7, 0x12);
    cmd->progress += arg0;
    return func_ov003_02086b88(cmd);
}

s32 func_ov003_02086f7c(void) {
    s32 done = 0;

    switch (g_CombatGlobal->cmdDeck->mode) {
        case 1:
            if (*func_ov003_02086e94() >= 5) {
                done = 1;
            }
            break;
        case 2:
            done = 1;
            break;
        default:
        case 0:
            if (*func_ov003_02086e94() >= 3) {
                done = 1;
            }
            break;
    }
    return done;
}

s8 func_ov003_02086fec(void) {
    return g_CombatGlobal->cmdDeck->current;
}

/// Starts the next charge step: advances the stage, re-shuffles the queue and restarts the effect.
void func_ov003_02087008(void) {
    BattleCmdDeck* cmd      = g_CombatGlobal->cmdDeck;
    s8             stage    = cmd->current;
    u16            previous = stage - 1;

    cmd->stage++;
    cmd->current  = 0;
    cmd->progress = 0;
    cmd->next     = cmd->stage + data_ov003_020d7614[cmd->mode][cmd->current];
    func_ov003_020871cc(1);
    func_ov003_020c1f18(&g_CombatGlobal->unk_34230, previous);
}

void func_ov003_02087094(BattleCmdDeck* cmd, u8 mode, s32 arg2) {
    g_CombatGlobal->cmdDeck = cmd;
    cmd->mode               = mode;
    cmd->unk_2C             = arg2;
    cmd->flags              = 0;
    cmd->current            = 0;
    cmd->count              = func_ov003_02086eb0();
    cmd->stage              = gSaveData.battleChargeStage;
    cmd->progress           = gSaveData.battleChargeProgress;
    cmd->next               = cmd->stage + data_ov003_020d7614[cmd->mode][cmd->current];
    if (g_CombatGlobal->unk3D88C == 0 && gSaveData.battleTimer == 0) {
        if (CombatAbility_IsActive(7) != 0) {
            cmd->progress += (s8)CombatAbility_GetValue(7);
        }
    }
    func_ov003_02086b88(cmd);
    switch (cmd->mode) {
        case 1:
            cmd->capacity = 10;
            break;
        case 2:
            cmd->capacity = 4;
            break;
        default:
        case 0:
            cmd->capacity = func_ov004_020e737c();
            break;
    }
    func_ov003_02086cc0();
}

void func_ov003_020871b0(BattleCmdDeck* cmd) {
    gSaveData.battleChargeProgress = cmd->progress;
    gSaveData.battleChargeStage    = cmd->stage;
}

void func_ov003_020871cc(s32 arg0) {
    BattleCmdDeck* cmd = g_CombatGlobal->cmdDeck;

    switch (cmd->mode) {
        case 1:
            func_ov005_020e740c(cmd->unk_2C, arg0);
            break;
        case 2:
            func_ov006_020e74b0(cmd->unk_2C, arg0);
            break;
        default:
        case 0:
            func_ov004_020e739c(cmd->unk_2C, arg0);
            break;
    }
}

void func_ov003_0208722c(void) {
    g_CombatGlobal->cmdDeck->flags |= 1;
}
