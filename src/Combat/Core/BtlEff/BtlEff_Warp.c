#include "Battle/Core/BtlActor.h"
#include "Battle/Core/BtlSprite.h"
#include "Combat/Core/BtlEff.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatEffects.h"
#include "Engine/EasyTask.h"
#include "SpriteMgr.h"
#include <nitro/mi/cpumem.h>

extern const BinIdentifier data_ov003_020d77f8;

/// Task data of the warp effect (`Tsk_BtlEff_Warp`): a pair of sprites, one left behind at the
/// previous position of the display actor and one at its new position.
typedef struct BtlEff_Warp {
    /* 0x00 */ CombatEffect halves[2];
} BtlEff_Warp; // Size: 0xE0

/// Parameters handed to the warp task by `func_ov003_0209f770`.
typedef struct BtlEff_WarpArgs {
    /* 0x00 */ volatile s32 mode; // display engine: 1 = sub screen, 0 = main screen
    /* 0x04 */ s32          x;    // previous world position of the display actor
    /* 0x08 */ s32          y;
    /* 0x0C */ s32          z;
} BtlEff_WarpArgs; // Size: 0x10

static s32 BtlEff_Warp_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

static const TaskHandle Tsk_BtlEff_Warp = {"Tsk_BtlEff_Warp", BtlEff_Warp_RunTask, sizeof(BtlEff_Warp)};

static const SpriteAnimEntry data_ov003_020d86e0[2];

/// Loads one half of the warp effect: animation `animIndex` of the warp sprite, flipped to match
/// the display actor it belongs to, and marks that half as playing.
static s32 func_ov003_0209f4a0(s32 mode, CombatEffect* sprite, u16 animIndex, s32 flip) {
    BtlSprite_LoadFromTable(mode, &sprite->sprite, &data_ov003_020d77f8, data_ov003_020d86e0, animIndex, 1, 0x30);
    BtlSprite_SetAnimFromTable(&sprite->sprite, animIndex, 1);
    BtlSprite_SetFlip(&sprite->sprite, flip);
    sprite->active = 1;
    return 1;
}

/// Advances one half of the warp effect and stops it once its animation is over.
static void func_ov003_0209f508(CombatEffect* sprite) {
    if (SpriteMgr_IsAnimationFinished(&sprite->sprite.sprite)) {
        sprite->active = 0;
    }
    BtlSprite_Update(&sprite->sprite);
}

/// Draws one half of the warp effect at its world position, depth-sorted by Y.
static void func_ov003_0209f52c(CombatEffect* sprite) {
    s32 posX;
    s32 posY;

    if (!(sprite->active == 0)) {
        if (sprite->sprite.sprite.bits_0_1 == 1) {
            posX = BtlStage_ToScreenX(1, sprite->position.x);
            posY = BtlStage_ToScreenY(1, sprite->position.y, sprite->position.z);
        } else {
            posX = BtlStage_ToScreenX(0, sprite->position.x);
            posY = BtlStage_ToScreenY(0, sprite->position.y, sprite->position.z);
        }
        BtlSprite_SetPosition(&sprite->sprite, posX, posY);
        BtlSprite_SetSortKey(&sprite->sprite, 0x7FFFFFFF - (sprite->position.y + 0x8000));
        BtlSprite_Render(&sprite->sprite);
    }
}

static void func_ov003_0209f5c8(BattleSprite* sprite) {
    BtlSprite_Release(sprite);
}

/// Moves one half of the warp effect to a world position.
static void func_ov003_0209f5d4(CombatEffect* sprite, s32 x, s32 y, s32 z) {
    sprite->position.x = x;
    sprite->position.y = y;
    sprite->position.z = z;
}

/// Builds the previous-position half and current-position half, both flipped like the actor.
static s32 BtlEff_Warp_Init(TaskPool* pool, Task* task, void* args) {
    BtlEff_Warp*     eff = task->data;
    BtlEff_WarpArgs* p   = args;
    BattleActor*     actor;
    s32              flip;

    MI_CpuSet(eff, 0, sizeof(BtlEff_Warp));
    if (p->mode == 1) {
        actor = g_CombatGlobal->partner;
    } else {
        actor = g_CombatGlobal->player;
    }
    flip = actor->isFlipped != 0;
    // Volatile mode preserves the retail reload before each load call.
    func_ov003_0209f4a0(p->mode, &eff->halves[0], 0, flip);
    func_ov003_0209f4a0(p->mode, &eff->halves[1], 1, flip);
    func_ov003_0209f5d4(&eff->halves[0], p->x, p->y, p->z);
    func_ov003_0209f5d4(&eff->halves[1], actor->position.x, actor->position.y, actor->position.z);
    return 1;
}

/// Advances both halves, keeping the task alive while either is still playing.
static s32 BtlEff_Warp_Update(TaskPool* pool, Task* task, void* args) {
    BtlEff_Warp* eff = task->data;

    func_ov003_0209f508(&eff->halves[0]);
    func_ov003_0209f508(&eff->halves[1]);
    return eff->halves[0].active != 0 || eff->halves[1].active != 0;
}

/// Draws both halves.
static s32 BtlEff_Warp_Render(TaskPool* pool, Task* task, void* args) {
    BtlEff_Warp* eff = task->data;

    func_ov003_0209f52c(&eff->halves[0]);
    func_ov003_0209f52c(&eff->halves[1]);
    return 1;
}

/// Releases both halves.
static s32 BtlEff_Warp_Release(TaskPool* pool, Task* task, void* args) {
    BtlEff_Warp* eff = task->data;

    func_ov003_0209f5c8(&eff->halves[0].sprite);
    func_ov003_0209f5c8(&eff->halves[1].sprite);
    return 1;
}

static s32 BtlEff_Warp_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlEff_Warp_Init,
        .update     = BtlEff_Warp_Update,
        .render     = BtlEff_Warp_Render,
        .cleanup    = BtlEff_Warp_Release,
    };

    if (func_ov003_0208b690(stage) != 0) {
        return 1;
    } else {
        return stages.iter[stage](pool, task, args);
    }
}

static const SpriteAnimEntry data_ov003_020d86e0[2] = {
    {53, 55, 54, 0},
    {53, 55, 54, 1},
};

void func_ov003_0209f770(s32 mode, s32 x, s32 y, s32 z) {
    BtlEff_WarpArgs args;

    args.mode = mode;
    args.x    = x;
    args.y    = y;
    args.z    = z;
    EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &Tsk_BtlEff_Warp, NULL, 0, NULL, &args);
}
