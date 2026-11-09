#include "Combat/Core/BtlAura.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatAura.h"
#include "Combat/Core/CombatCutin.h"
#include "Combat/Core/CombatSprite.h"
#include "Engine/EasyTask.h"
#include "Engine/Math/Random.h"
#include "SpriteMgr.h"
#include "main_02026528.h"

#include <nitro/mi/cpumem.h>

// The aura on the sub screen (`Tsk_BtlAuraSub`): sixteen small sprites that drift up out of the
// ground around the owner's world position, fade with a shared sort palette and disappear once the
// aura's timer runs out. The record is `CombatSprite`-shaped at the front because the `CombatSprite`
// helpers drive the whole task data; the flames themselves live in the 0x60..0x760 range.

/// The battle sprite sheet every flame is loaded from.
extern const BinIdentifier data_ov003_020d7800;

static s32 BtlAuraSub_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

static const TaskHandle Tsk_BtlAuraSub = {"Tsk_BtlAuraSub", BtlAuraSub_RunTask, sizeof(BtlAuraSub)};

/// The animation entry `BtlAuraSub_Init` loads the burst sprite from.
static const SpriteAnimEntry data_ov003_020d7ec8[1] = {
    {16, 18, 17, 1},
};

/// Gives every flame the same start position, a random angle and spread around the aura, a random
/// drift speed and a random first animation.
static void func_ov003_0209703c(s32 engine, BtlAuraSub* aura) {
    s32           i;
    s32           palette;
    u32           spread;
    s16           anim;
    BtlAuraFlame* flame;

    i      = 0;
    spread = RNG_Next(0x100) << 8;
    if (engine == 0) {
        palette = 0x3E;
    } else {
        palette = 0x40;
    }
    flame = aura->flames;
    do {
        flame->x      = aura->x;
        flame->y      = aura->y;
        flame->height = spread + (RNG_Next(0x100) << 8) / 32;
        BtlMgr_MoveByAngle(&flame->x, &flame->y, flame->height, (RNG_Next(9) + 0x18) << 12);
        if (i & 1) {
            flame->riseSpeed = 0;
            anim             = RNG_Next(3) + 2;
        } else {
            flame->riseSpeed = (RNG_Next(3) + 1) << 12;
            anim             = RNG_Next(2);
        }
        CombatSprite_LoadDirectNoProto(engine, &flame->sprite, &data_ov003_020d7800, 0x13, 0x15, 0x14, palette, anim);
        CombatSprite_SetAnim((CombatSprite*)flame, (u16)anim, 1);
        i++;
        spread += 0x1000;
        flame++;
    } while (i < 16);
}

/// Advances the still-running flames and records whether any of them is left.
static void func_ov003_02097158(BtlAuraSub* aura) {
    s32           active;
    s32           i;
    BtlAuraFlame* flame;

    active = 0;
    i      = 0;
    flame  = aura->flames;
    do {
        if (SpriteMgr_IsAnimationFinished(&flame->sprite.sprite) == 0) {
            if (flame->riseSpeed != 0) {
                BtlMgr_MoveByAngle(&flame->x, &flame->y, flame->height, flame->riseSpeed);
            }
            CombatSprite_Update((CombatSprite*)flame);
            active = 1;
        }
        i++;
        flame++;
    } while (i < 16);
    aura->active = active;
}

/// Draws every still-running flame, depth-sorted against the shared aura palette.
static void func_ov003_020971c0(BtlAuraSub* aura) {
    s32           sortKey;
    s32           i;
    BattleActor*  owner;
    BtlAuraFlame* flame;

    i     = 0;
    flame = aura->flames;
    do {
        if (flame->sprite.sprite.bits_0_1 == 0) {
            owner = g_CombatGlobal->player;
        } else {
            owner = g_CombatGlobal->partner;
        }
        sortKey = 0x80000000 - owner->position.y;
        if (SpriteMgr_IsAnimationFinished(&flame->sprite.sprite) == 0) {
            CombatSprite_SetPosition(&flame->sprite, (s32)(flame->x * 0x10) >> 16, (s32)(flame->y * 0x10) >> 16);
            func_ov003_02082730((CombatSprite*)flame, sortKey);
            CombatSprite_Render((CombatSprite*)flame);
        }
        i++;
        flame++;
    } while (i < 16);
}

/// Releases the sixteen flame sprites.
static void func_ov003_02097254(BtlAuraSub* aura) {
    s32           i;
    BtlAuraFlame* flame;

    i     = 0;
    flame = aura->flames;
    do {
        CombatSprite_Release((CombatSprite*)flame);
        i++;
        flame++;
    } while (i < 16);
}

/// Seeds the record, picks the screen-side palette and spawns the flames.
static s32 BtlAuraSub_Init(TaskPool* pool, Task* task, void* args) {
    BtlAuraSub*  aura = task->data;
    BtlAuraArgs* p    = args;
    s32          engine;
    s32          palette;

    MI_CpuSet(aura, NULL, sizeof(BtlAuraSub));
    engine        = 1;
    aura->active  = 1;
    aura->x       = p->x;
    aura->y       = p->y;
    aura->timer   = 0x14;
    aura->sortKey = 0;
    if (aura->y < 0xC0000) {
        palette = 0x40;
    } else {
        engine = 0;
        aura->y -= 0xC0000;
        palette = 0x3E;
    }
    CombatSprite_LoadFromTableNoProto(engine, &aura->sprite, &data_ov003_020d7800, data_ov003_020d7ec8, 0, palette, 0x90);
    CombatSprite_SetPosition(&aura->sprite, (s32)(aura->x * 0x10) >> 16, (s32)(aura->y * 0x10) >> 16);
    func_ov003_0209703c(engine, aura);
    CombatSe_PlayPanned(SEIDX_SE_GREEN_LIGHT, (s32)(aura->x * 0x10) >> 16);
    return 1;
}

/// Runs the descent timer, updates the flames and reports whether the aura is done.
static s32 BtlAuraSub_Update(TaskPool* pool, Task* task, void* args) {
    BtlAuraSub* aura = task->data;

    if (g_CombatGlobal->unk3D878 & 0x40) {
        return 1;
    }
    if ((s32)aura->timer <= 0) {
        aura->sortKey = 0;
    } else {
        func_020265d4(&aura->sortKey, 0x1000, (u16)aura->timer);
        aura->timer = (s16)(aura->timer - 1);
    }
    CombatSprite_Update(&aura->sprite);
    func_ov003_02097158(aura);
    return aura->active;
}

/// Draws the burst that follows the aura down while its timer runs, then the flames.
static s32 BtlAuraSub_Render(TaskPool* pool, Task* task, void* args) {
    BtlAuraSub*  aura = task->data;
    BattleActor* owner;

    if (aura->sortKey > 0) {
        if (aura->sprite.sprite.bits_0_1 == 0) {
            owner = g_CombatGlobal->player;
        } else {
            owner = g_CombatGlobal->partner;
        }
        CombatSprite_SetAffineTransform(&aura->sprite, 0, aura->sortKey, aura->sortKey, 1);
        func_ov003_02082730(&aura->sprite, 0x80000000 - owner->position.y);
        CombatSprite_Render(&aura->sprite);
    }
    func_ov003_020971c0(aura);
    return 1;
}

/// Releases the burst sprite and the flames.
static s32 BtlAuraSub_Release(TaskPool* pool, Task* task, void* args) {
    BtlAuraSub* aura = task->data;

    CombatSprite_Release(&aura->sprite);
    func_ov003_02097254(aura);
    return 1;
}

static s32 BtlAuraSub_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlAuraSub_Init,
        .update     = BtlAuraSub_Update,
        .render     = BtlAuraSub_Render,
        .cleanup    = BtlAuraSub_Release,
    };

    if (func_ov003_0208b690(stage) != 0) {
        return 1;
    } else {
        return stages.iter[stage](pool, task, args);
    }
}

void func_ov003_020974f8(s32 x, s32 y) {
    BtlAuraArgs args;

    args.x = x;
    args.y = y;
    EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &Tsk_BtlAuraSub, NULL, 0, NULL, &args);
}
