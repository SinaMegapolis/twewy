#include "Battle/Core/BtlActor.h"
#include "Battle/Core/BtlSprite.h"
#include "Combat/Core/Combat.h"

#include "Combat/Core/CombatApi.h"

// The target marker: a reticle drawn on both screens over the battle unit the player is targeting.

typedef struct BtlTarget {
    /* 0x00 */ BattleSprite sprites[2];
    /* 0xC0 */ BattleActor* target;
    /* 0xC4 */ void (*update)(struct BtlTarget*);
    /* 0xC8 */ s32 active;
} BtlTarget; // Size: 0xCC

static s32 BtlTarget_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

const TaskHandle Tsk_BtlTarget = {"Tsk_BtlTarget", BtlTarget_RunTask, sizeof(BtlTarget)};

static const SpriteAnimEntry data_ov003_020d7fec[2];

static void func_ov003_0209abdc(BtlTarget* target, void (*update)(BtlTarget*)) {
    target->update = update;
}

static void func_ov003_0209abe4(BattleSprite* sprite, BattleActor* actor) {
    s32 x;
    s32 y;

    if (!(actor->flags & ACTOR_FLAG_NO_TARGET_MARKER)) {
        s32 positionX = actor->position.x + (actor->centerOffsetX << 12);
        if (sprite->sprite.bits_0_1 == 0) {
            x = BtlStage_ToScreenX(0, positionX);
            y = BtlStage_ToScreenY(0, actor->position.y, actor->position.z - (actor->centerHeight << 12));
        } else {
            x = BtlStage_ToScreenX(1, positionX);
            y = BtlStage_ToScreenY(1, actor->position.y, actor->position.z - (actor->centerHeight << 12));
        }
        BtlSprite_SetPosition(sprite, x, y);
        BtlSprite_SetSortKey(sprite, 10);
        BtlSprite_Render(sprite);
    }
}

static void func_ov003_0209ac8c(BtlTarget* target) {
    if (SpriteMgr_IsAnimationFinished(&target->sprites[0].sprite)) {
        BtlSprite_SetAnimFromTable(&target->sprites[0], 0, 0);
        BtlSprite_SetAnimFromTable(&target->sprites[1], 0, 0);
        func_ov003_0209abdc(target, NULL);
    }
}

static s32 BtlTarget_Init(TaskPool* pool, Task* task, void* args) {
    BtlTarget* target = task->data;

    target->active = 1;
    target->target = NULL;
    func_ov003_0209abdc(target, NULL);
    BtlSprite_LoadFromTable(1, &target->sprites[0], &Iden_BtlMenu, data_ov003_020d7fec, 0, 0x3F, 0x10);
    BtlSprite_LoadFromTable(0, &target->sprites[1], &Iden_BtlMenu, data_ov003_020d7fec, 0, 0x3D, 0x10);
    BtlSprite_SetAnimFromTable(&target->sprites[0], 1, 1);
    BtlSprite_SetAnimFromTable(&target->sprites[1], 1, 1);
    return 1;
}

static s32 BtlTarget_Update(TaskPool* pool, Task* task, void* args) {
    BtlTarget*   target = task->data;
    BattleActor* actor  = gBattle->lockOnTarget;

    if (actor != NULL && ((actor->flags & ACTOR_FLAG_DEFEATED) || (actor->flags & ACTOR_FLAG_INTANGIBLE))) {
        gBattle->lockOnTarget = NULL;
    }
    if (target->target != gBattle->lockOnTarget) {
        target->target = gBattle->lockOnTarget;
        BtlSprite_SetAnim(&target->sprites[0], 2, 1);
        BtlSprite_SetAnim(&target->sprites[1], 2, 1);
        BtlSprite_Restart(&target->sprites[0]);
        BtlSprite_Restart(&target->sprites[1]);
        func_ov003_0209abdc(target, func_ov003_0209ac8c);
    }
    if (target->update != NULL) {
        target->update(target);
    }
    BtlSprite_Update(&target->sprites[0]);
    BtlSprite_Update(&target->sprites[1]);
    return target->active;
}

static s32 BtlTarget_Render(TaskPool* pool, Task* task, void* args) {
    BtlTarget* target = task->data;

    if (gBattle->state != BATTLE_STATE_FIGHT) {
        return 1;
    }
    if (target->target != NULL) {
        func_ov003_0209abe4(&target->sprites[0], target->target);
        if (target->target->linkedActor != NULL) {
            func_ov003_0209abe4(&target->sprites[1], target->target->linkedActor);
        }
    }
    return 1;
}

static s32 BtlTarget_Release(TaskPool* pool, Task* task, void* args) {
    BtlTarget* target = task->data;

    BtlSprite_Release(&target->sprites[0]);
    BtlSprite_Release(&target->sprites[1]);
    return 1;
}

static s32 BtlTarget_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlTarget_Init,
        .update     = BtlTarget_Update,
        .render     = BtlTarget_Render,
        .cleanup    = BtlTarget_Release,
    };

    if (BtlMgr_IsStageFrozen(stage)) {
        return 1;
    }
    return stages.iter[stage](pool, task, args);
}

static const SpriteAnimEntry data_ov003_020d7fec[2] = {
    {49, 51, 50, 1},
    {49, 51, 50, 2}
};
