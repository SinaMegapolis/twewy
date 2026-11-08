#include "Combat/Core/BtlEff.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatActor.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatEffects.h"
#include "Combat/Core/CombatSprite.h"
#include "Engine/Core/Memory.h"
#include "Engine/EasyTask.h"
#include "Engine/File/BinMgr.h"
#include "Engine/Math/Random.h"
#include "Save.h"

#include <nitro/mi/cpumem.h>

// The enemy-death battle effect: an explosion sprite plus the death sound effect. `ov003_020c41e8`
// spawns it when an enemy unit dies; both screens each play their own sound when the sprite
// finishes, then stamp the partner that fought the battle into the save data.

/// The death effect's spawn parameter, handed to the init stage by `EasyTask_CreateTask`.
typedef struct BtlEffDeadSpawn {
    /* 0x00 */ s32 engine; // display engine the effect plays on
    /* 0x04 */ s32 x;
    /* 0x08 */ s32 y;
    /* 0x0C */ s32 z;
    /* 0x10 */ s32 hasPartner;
} BtlEffDeadSpawn; // Size: 0x14

extern const BinIdentifier data_ov003_020d7800;

static s32 BtlEff_Dead_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

static const TaskHandle Tsk_BtlEff_Dead = {"Tsk_BtlEff_Dead", BtlEff_Dead_RunTask, sizeof(CombatDeadEffect)};

static const SpriteAnimEntry data_ov003_020d86a0[];

/// Picks and plays one of the two death sound effects for the active partner's screen.
static void func_ov003_0209f040(void) {
    u16 rnd = RNG_Next(2);

    if (g_CombatGlobal->unk3D88D == 0) {
        switch (rnd) {
            case 0:
                CombatSe_PlayAtSubX(0x38F, ((CombatActor*)g_CombatGlobal->unk3D89C)->position.x);
                break;
            case 1:
                CombatSe_PlayAtSubX(0x390, ((CombatActor*)g_CombatGlobal->unk3D89C)->position.x);
                break;
        }
    }
    if (g_CombatGlobal->unk3D88D == 1) {
        switch (rnd) {
            case 0:
                CombatSe_PlayAtSubX(0x3B7, ((CombatActor*)g_CombatGlobal->unk3D89C)->position.x);
                break;
            case 1:
                CombatSe_PlayAtSubX(0x3B8, ((CombatActor*)g_CombatGlobal->unk3D89C)->position.x);
                break;
        }
    }
    if (g_CombatGlobal->unk3D88D == 2) {
        switch (rnd) {
            case 0:
                CombatSe_PlayAtSubX(0x3DF, ((CombatActor*)g_CombatGlobal->unk3D89C)->position.x);
                break;
            case 1:
                CombatSe_PlayAtSubX(0x3E0, ((CombatActor*)g_CombatGlobal->unk3D89C)->position.x);
                break;
        }
    }
}

/// Picks and plays one of the two death sound effects for the main screen.
static void func_ov003_0209f154(void) {
    u16 rnd = RNG_Next(2);

    if (rnd != 0) {
        CombatSe_PlayAtMainX(0x367, ((CombatActor*)g_CombatGlobal->unk3D898)->position.x);
        return;
    }
    CombatSe_PlayAtMainX(0x368, ((CombatActor*)g_CombatGlobal->unk3D898)->position.x);
}

/// Plays one of the death sound effects for `engine`'s screen, when the random roll allows it.
static void func_ov003_0209f1b0(u32 engine) {
    if (RNG_Next(2) != 0) {
        return;
    }
    if (engine == 0) {
        func_ov003_0209f040();
        return;
    }
    func_ov003_0209f154();
}

/// Copies the spawn position and loads the death animation for the effect's screen.
// Nonmatching: the retail `CombatSprite_LoadFromTable` materialises its stack arguments in r0/r1
// and reloads `spawn->engine`, while this build keeps the engine in r0 and uses r1/r12 for the
// constants (argument-setup scheduling only; the call sequence itself is identical).
static s32 BtlEff_Dead_Init(TaskPool* pool, Task* task, void* args) {
    CombatDeadEffect* state = task->data;
    BtlEffDeadSpawn*  spawn = args;

    MI_CpuSet(state, 0, sizeof(CombatDeadEffect));
    state->active     = 1;
    state->position.x = spawn->x;
    state->position.y = spawn->y;
    state->position.z = spawn->z;
    state->playVoice  = spawn->hasPartner;
    CombatSprite_LoadFromTable(spawn->engine, &state->sprite, &data_ov003_020d7800, data_ov003_020d86a0, 0,
                               (spawn->engine == 0) ? 0x3D : 0x3F, 0x20);
    CombatSprite_SetAnimFromTable(&state->sprite, 0, 1);
    state->sprite.flags46 |= 2;
    return 1;
}

/// Waits for the animation, then plays the death sound and reports the task done.
static s32 BtlEff_Dead_Update(TaskPool* pool, Task* task, void* args) {
    CombatDeadEffect* state = task->data;

    if (SpriteMgr_IsAnimationFinished(&state->sprite.sprite) != 0) {
        state->active = 0;
        if (state->playVoice != 0) {
            u8 partner = 0xFF;

            if (state->sprite.sprite.bits_0_1 == 1) {
                switch (g_CombatGlobal->unk3D88D) {
                    case 0:
                        partner = 0;
                        break;
                    case 1:
                        partner = 1;
                        break;
                    case 2:
                        partner = 2;
                        break;
                }
            }
            gSaveData.battlePartner = partner;
            func_ov003_0209f1b0(state->sprite.sprite.bits_0_1);
        }
    }
    CombatSprite_Update(&state->sprite);
    return state->active;
}

/// Places the death sprite over the dying enemy and sorts it in front of it.
static s32 BtlEff_Dead_Render(TaskPool* pool, Task* task, void* args) {
    CombatDeadEffect* state  = task->data;
    s32               engine = state->sprite.sprite.bits_0_1;
    s32               posX;
    s32               posY;

    if (engine == 0) {
        posX = CombatStage_ToScreenX(0, state->position.x);
        posY = CombatStage_ToScreenY(0, state->position.y, state->position.z);
    } else {
        posX = CombatStage_ToScreenX(1, state->position.x);
        posY = CombatStage_ToScreenY(1, state->position.y, state->position.z);
    }
    CombatSprite_SetPosition(&state->sprite, posX, posY);
    func_ov003_02082730(&state->sprite, 0x7FFFFFFF - (state->position.y + 0x8000));
    CombatSprite_Render(&state->sprite);
    return 1;
}

/// Releases the death sprite.
static s32 BtlEff_Dead_Release(TaskPool* pool, Task* task, void* args) {
    CombatSprite_Release(task->data);
    return 1;
}

static s32 BtlEff_Dead_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlEff_Dead_Init,
        .update     = BtlEff_Dead_Update,
        .render     = BtlEff_Dead_Render,
        .cleanup    = BtlEff_Dead_Release,
    };

    if (func_ov003_0208b690(stage) != 0) {
        return 1;
    } else {
        return stages.iter[stage](pool, task, args);
    }
}

void func_ov003_0209f444(s32 engine, s32 x, s32 y, s32 z, s32 hasPartner) {
    BtlEffDeadSpawn spawn;

    spawn.engine     = engine;
    spawn.x          = x;
    spawn.y          = y;
    spawn.z          = z;
    spawn.hasPartner = hasPartner;
    EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &Tsk_BtlEff_Dead, NULL, 0, NULL, &spawn);
}

static const SpriteAnimEntry data_ov003_020d86a0[1] = {
    {34, 36, 35, 0},
};
