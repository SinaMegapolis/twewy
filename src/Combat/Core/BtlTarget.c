#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatActor.h"
#include "Combat/Core/CombatSprite.h"

#include "Combat/Core/CombatApi.h"

// The target marker: a reticle drawn on both screens over the battle unit the player is targeting.

typedef struct BtlTarget {
    /* 0x00 */ CombatSprite sprites[2];
    /* 0xC0 */ CombatActor* target;
    /* 0xC4 */ void (*update)(struct BtlTarget*);
    /* 0xC8 */ s32 active;
} BtlTarget; // Size: 0xCC

extern const BinIdentifier data_ov003_020d7800;

static s32 BtlTarget_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

const TaskHandle Tsk_BtlTarget = {"Tsk_BtlTarget", BtlTarget_RunTask, sizeof(BtlTarget)};

static const SpriteAnimEntry data_ov003_020d7fec[2];

static void func_ov003_0209abdc(BtlTarget* target, void (*update)(BtlTarget*)) {
    target->update = update;
}

static void func_ov003_0209abe4(CombatSprite* sprite, CombatActor* actor) {
    s32 x;
    s32 y;

    if (!(actor->flags & 0x20)) {
        s32 positionX = actor->position.x + (actor->unk_74 << 12);
        if (sprite->sprite.bits_0_1 == 0) {
            x = CombatStage_ToScreenX(0, positionX);
            y = CombatStage_ToScreenY(0, actor->position.y, actor->position.z - (actor->unk_76 << 12));
        } else {
            x = CombatStage_ToScreenX(1, positionX);
            y = CombatStage_ToScreenY(1, actor->position.y, actor->position.z - (actor->unk_76 << 12));
        }
        CombatSprite_SetPosition(sprite, x, y);
        func_ov003_02082730(sprite, 10);
        CombatSprite_Render(sprite);
    }
}

static void func_ov003_0209ac8c(BtlTarget* target) {
    if (SpriteMgr_IsAnimationFinished(&target->sprites[0].sprite)) {
        CombatSprite_SetAnimFromTable(&target->sprites[0], 0, 0);
        CombatSprite_SetAnimFromTable(&target->sprites[1], 0, 0);
        func_ov003_0209abdc(target, NULL);
    }
}

static s32 BtlTarget_Init(TaskPool* pool, Task* task, void* args) {
    BtlTarget* target = task->data;

    target->active = 1;
    target->target = NULL;
    func_ov003_0209abdc(target, NULL);
    CombatSprite_LoadFromTable(1, &target->sprites[0], &data_ov003_020d7800, data_ov003_020d7fec, 0, 0x3F, 0x10);
    CombatSprite_LoadFromTable(0, &target->sprites[1], &data_ov003_020d7800, data_ov003_020d7fec, 0, 0x3D, 0x10);
    CombatSprite_SetAnimFromTable(&target->sprites[0], 1, 1);
    CombatSprite_SetAnimFromTable(&target->sprites[1], 1, 1);
    return 1;
}

static s32 BtlTarget_Update(TaskPool* pool, Task* task, void* args) {
    BtlTarget*   target = task->data;
    CombatActor* actor  = g_CombatGlobal->unk3D8A0;

    if (actor != NULL && ((actor->flags & 4) || (actor->flags & 0x10))) {
        g_CombatGlobal->unk3D8A0 = NULL;
    }
    if (target->target != g_CombatGlobal->unk3D8A0) {
        target->target = g_CombatGlobal->unk3D8A0;
        CombatSprite_SetAnim(&target->sprites[0], 2, 1);
        CombatSprite_SetAnim(&target->sprites[1], 2, 1);
        CombatSprite_Restart(&target->sprites[0]);
        CombatSprite_Restart(&target->sprites[1]);
        func_ov003_0209abdc(target, func_ov003_0209ac8c);
    }
    if (target->update != NULL) {
        target->update(target);
    }
    CombatSprite_Update(&target->sprites[0]);
    CombatSprite_Update(&target->sprites[1]);
    return target->active;
}

static s32 BtlTarget_Render(TaskPool* pool, Task* task, void* args) {
    BtlTarget* target = task->data;

    if (g_CombatGlobal->unk3D874 != 2) {
        return 1;
    }
    if (target->target != NULL) {
        func_ov003_0209abe4(&target->sprites[0], target->target);
        if (target->target->unk_78 != 0) {
            func_ov003_0209abe4(&target->sprites[1], (CombatActor*)target->target->unk_78);
        }
    }
    return 1;
}

static s32 BtlTarget_Release(TaskPool* pool, Task* task, void* args) {
    BtlTarget* target = task->data;

    CombatSprite_Release(&target->sprites[0]);
    CombatSprite_Release(&target->sprites[1]);
    return 1;
}

static s32 BtlTarget_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlTarget_Init,
        .update     = BtlTarget_Update,
        .render     = BtlTarget_Render,
        .cleanup    = BtlTarget_Release,
    };

    if (func_ov003_0208b690(stage)) {
        return 1;
    }
    return stages.iter[stage](pool, task, args);
}

static const SpriteAnimEntry data_ov003_020d7fec[2] = {
    {49, 51, 50, 1},
    {49, 51, 50, 2}
};
