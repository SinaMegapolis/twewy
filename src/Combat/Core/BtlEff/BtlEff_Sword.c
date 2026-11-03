#include "Combat/Core/BtlEff.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatActor.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatEffects.h"
#include "Combat/Core/CombatSprite.h"
#include "Engine/Core/Memory.h"
#include "Engine/EasyTask.h"
#include "Engine/File/BinMgr.h"

#include <nitro/mi/cpumem.h>

// The sword-slash battle effect: a single one-shot sprite spawned at the hit position by
// `ov003_02087b7c` and played back to completion. Its task data is a bare `CombatSprite`, so the
// spawn only carries the animation (slash direction) to play.

/// The sword effect's spawn parameter, handed to the init stage by `EasyTask_CreateTask`.
typedef struct BtlEffSwordSpawn {
    /* 0x00 */ s16 animIndex;
} BtlEffSwordSpawn;

extern const BinIdentifier data_ov003_020d77d8;

static s32 BtlEff_Sword_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

static const TaskHandle Tsk_BtlEff_Sword = {"Tsk_BtlEff_Sword", BtlEff_Sword_RunTask, sizeof(CombatSprite)};

static const SpriteAnimEntry data_ov003_020d8650[];

/// Clears the effect sprite and loads the slash animation named by the spawn parameter.
static s32 BtlEff_Sword_Init(TaskPool* pool, Task* task, void* args) {
    CombatSprite*     sprite = task->data;
    BtlEffSwordSpawn* spawn  = args;

    MI_CpuSet(sprite, 0, sizeof(CombatSprite));
    CombatSprite_LoadFromTable(0, sprite, &data_ov003_020d77d8, data_ov003_020d8650, spawn->animIndex, 0x3A, 0x68);
    CombatSprite_SetAnimFromTable(sprite, spawn->animIndex, 1);
    return (sprite->flags46 & 0x8000) ? 0 : 1;
}

/// Retires the effect once the battle leaves the effect-visible state, the hit-stun
/// is over, or the slash animation has finished.
static s32 BtlEff_Sword_Update(TaskPool* pool, Task* task, void* args) {
    CombatSprite* sprite = task->data;

    if (data_ov003_020e71b8->unk3D878 & 0x40) {
        return 1;
    }
    if (func_ov003_020876c0() != 0) {
        return 0;
    }
    if (SpriteMgr_IsAnimationFinished(&sprite->sprite) != 0) {
        return 0;
    }
    CombatSprite_Update(sprite);
    return 1;
}

/// Pins the slash to the main display's target actor and sorts it in front of it.
static s32 BtlEff_Sword_Render(TaskPool* pool, Task* task, void* args) {
    CombatSprite* sprite = task->data;
    CombatActor*  actor  = data_ov003_020e71b8->unk3D898;
    s32           posX;
    s32           posY;

    if (actor->isFlipped == 0) {
        CombatSprite_SetFlip(sprite, 0);
    } else {
        CombatSprite_SetFlip(sprite, 1);
    }
    posX = func_ov003_020843b0(0, actor->position.x);
    posY = func_ov003_020843ec(0, actor->position.y, actor->position.z);
    CombatSprite_SetPosition(sprite, posX, posY);
    func_ov003_02082730(sprite, 0x7FFFFFFF - actor->position.y);
    CombatSprite_Render(sprite);
    return 1;
}

/// Releases the slash sprite.
static s32 BtlEff_Sword_Release(TaskPool* pool, Task* task, void* args) {
    CombatSprite_Release(task->data);
    return 1;
}

static s32 BtlEff_Sword_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlEff_Sword_Init,
        .update     = BtlEff_Sword_Update,
        .render     = BtlEff_Sword_Render,
        .cleanup    = BtlEff_Sword_Release,
    };

    if (func_ov003_0208b690(stage) != 0) {
        return 1;
    } else {
        return stages.iter[stage](pool, task, args);
    }
}

void func_ov003_0209f000(s16 animIndex) {
    // The spawn parameter is handed over by address, so keep a local copy of it rather than
    // taking the address of an incoming register argument.
    s16 anim = animIndex;

    EasyTask_CreateTask(&data_ov003_020e71b8->unk_00000, &Tsk_BtlEff_Sword, NULL, 0, NULL, &anim);
}

static const SpriteAnimEntry data_ov003_020d8650[10] = {
    {59, 61, 60, 0},
    {62, 64, 63, 0},
    {65, 67, 66, 0},
    {68, 70, 69, 0},
    {68, 70, 69, 4},
    {71, 73, 72, 1},
    {74, 76, 75, 0},
    {77, 79, 78, 0},
    {80, 82, 81, 2},
    {83, 85, 84, 0},
};
