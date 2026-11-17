#include "Battle/Core/BtlActor.h"
#include "Battle/Core/BtlSprite.h"
#include "Combat/Core/BtlEff.h"
#include "Combat/Core/Combat.h"

#include "Combat/Core/CombatApi.h"
#include <nitro/mi/cpumem.h>

// The level-up effect: a banner and a burst over the partner's head on each screen, played once
// with the level-up voice. The effect is flagged in battle flag 8 while it runs.

/// One screen's half of the effect.
typedef struct BtlEffLvUp {
    /* 0x00 */ BattleSprite sprites[2]; // banner, burst
} BtlEffLvUp;                           // Size: 0xC0

static s32 BtlEff_LvUp_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

const TaskHandle Tsk_BtlEff_LvUp = {"Tsk_BtlEff_LvUp", BtlEff_LvUp_RunTask, sizeof(BtlEffLvUp[2])};

static const SpriteAnimEntry data_ov003_020d8754[2];

// Nonmatching: use the loader's declared u16 palette parameters.
static void func_ov003_020a0290(s32 engine, BtlEffLvUp* data) {
    s32 palette;

    if (engine == 0) {
        palette = 62;
    } else {
        palette = 64;
    }
    BtlSprite_LoadFromTable(engine, &data->sprites[1], &Iden_BtlMenu, data_ov003_020d8754, 1, palette, 72);
    BtlSprite_SetAnimFromTable(&data->sprites[1], 1, 1);
    BtlSprite_SetPriority(&data->sprites[1], 2);
    BtlSprite_LoadFromTable(engine, &data->sprites[0], &Iden_BtlMenu, data_ov003_020d8754, 0, palette, 52);
    BtlSprite_SetAnimFromTable(&data->sprites[0], 0, 1);
    BtlSprite_SetPriority(&data->sprites[0], 2);
}

static s32 func_ov003_020a033c(BtlEffLvUp* data) {
    BtlSprite_Update(&data->sprites[0]);
    BtlSprite_Update(&data->sprites[1]);
    return !SpriteMgr_IsAnimationFinished(&data->sprites[0].sprite);
}

static void func_ov003_020a0368(BtlEffLvUp* data) {
    BattleActor* actor;
    s32          x;

    if (data->sprites[0].sprite.bits_0_1 == 0) {
        actor = gBattle->player;
        x     = BtlStage_ToScreenX(0, actor->position.x);
    } else {
        actor = gBattle->partner;
        x     = BtlStage_ToScreenX(1, actor->position.x);
    }
    BtlSprite_SetPosition(&data->sprites[1], x, 96);
    BtlSprite_SetSortKey(&data->sprites[1], 0x7FFFFFFF - actor->position.y);
    BtlSprite_Render(&data->sprites[1]);
    BtlSprite_SetPosition(&data->sprites[0], x, 96);
    BtlSprite_SetSortKey(&data->sprites[0], 0);
    BtlSprite_Render(&data->sprites[0]);
}

static void func_ov003_020a0418(BtlEffLvUp* data) {
    BtlSprite_Release(&data->sprites[0]);
    BtlSprite_Release(&data->sprites[1]);
}

static s32 BtlEff_LvUp_Init(TaskPool* pool, Task* task, void* args) {
    BtlEffLvUp* data = task->data;

    MI_CpuSet(data, 0, sizeof(BtlEffLvUp[2]));
    gBattle->flags |= BATTLE_FLAG_LEVEL_UP;
    func_ov003_020a0290(0, &data[0]);
    func_ov003_020a0290(1, &data[1]);
    BtlSe_Play(SEIDX_SE_BOY_LEVELUP);
    return 1;
}

static s32 BtlEff_LvUp_Update(TaskPool* pool, Task* task, void* args) {
    BtlEffLvUp* data = task->data;
    s32         active, x;

    active = func_ov003_020a033c(&data[0]);
    func_ov003_020a033c(&data[1]);
    x = BtlStage_ToScreenX(0, gBattle->player->position.x);
    // Both effect attachment calls intentionally use the sub-screen actor +4 in retail.
    BtlHit_AttackAtScreen(BtlHit_GetAttackInfo(31), &gBattle->partner->baseArgs, x, 0);
    x = BtlStage_ToScreenX(1, gBattle->partner->position.x);
    BtlHit_AttackAtScreen(BtlHit_GetAttackInfo(32), &gBattle->partner->baseArgs, x, 0);
    return active;
}

static s32 BtlEff_LvUp_Render(TaskPool* pool, Task* task, void* args) {
    BtlEffLvUp* data = task->data;

    func_ov003_020a0368(&data[0]);
    func_ov003_020a0368(&data[1]);
    return 1;
}

static s32 BtlEff_LvUp_Release(TaskPool* pool, Task* task, void* args) {
    BtlEffLvUp* data = task->data;

    func_ov003_020a0418(&data[0]);
    func_ov003_020a0418(&data[1]);
    gBattle->flags &= ~BATTLE_FLAG_LEVEL_UP;
    return 1;
}

static s32 BtlEff_LvUp_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlEff_LvUp_Init,
        .update     = BtlEff_LvUp_Update,
        .render     = BtlEff_LvUp_Render,
        .cleanup    = BtlEff_LvUp_Release,
    };

    if (BtlMgr_IsStageFrozen(stage) != 0) {
        return 1;
    } else {
        return stages.iter[stage](pool, task, args);
    }
}

static const SpriteAnimEntry data_ov003_020d8754[2] = {
    {4, 6, 5, 0},
    {4, 6, 5, 1}
};
