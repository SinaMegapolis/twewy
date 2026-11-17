#include "Battle/Core/BtlActor.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Tutorial/BtlTutorial.h"
#include "Engine/EasyTask.h"

#include <nitro/mi/cpumem.h>

static s32 func_ov008_020e7374(TaskPool* pool, Task* task, void* args);
static s32 func_ov008_020e742c(TaskPool* pool, Task* task, void* args);
static s32 func_ov008_020e7464(TaskPool* pool, Task* task, void* args);
static s32 func_ov008_020e746c(TaskPool* pool, Task* task, void* args);
static s32 func_ov008_020e7474(TaskPool* pool, Task* task, void* args, s32 stage);

const TaskHandle Tsk_BtlPlayerNone = {"Tsk_BtlPlayerNone", func_ov008_020e7474, 0x84};

static const TaskStages data_ov008_020e7a10 = {
    .initialize = func_ov008_020e7374,
    .update     = func_ov008_020e742c,
    .render     = func_ov008_020e7464,
    .cleanup    = func_ov008_020e746c,
};

static void func_ov008_020e7360(BtlPlayerNone* data, void (*callback)(BtlPlayerNone*)) {
    data->unk_7C = callback;
    data->unk_80 = 0;
    data->unk_82 = 0;
}

static s32 func_ov008_020e7374(TaskPool* pool, Task* task, void* args) {
    BtlPlayerNone* data = task->data;

    MI_CpuSet(data, 0, sizeof(BtlPlayerNone));
    BtlActor_Init(&data->actor, ACTOR_KIND_PLAYER);
    gBattle->partner         = &data->actor;
    data->actor.isFlipped    = FALSE;
    data->actor.position.x   = gBattle->stages[1].centerX;
    data->actor.position.y   = gBattle->stages[1].centerY;
    data->actor.position.z   = 0;
    data->actor.zGravity     = 0x800;
    data->actor.radius       = 0xC;
    data->actor.height       = 0x30;
    data->actor.centerHeight = 0x18;
    data->actor.flags |= ACTOR_FLAG_INTANGIBLE;
    BtlStage_SetCameraTarget(1, data->actor.position.x, data->actor.position.y, data->actor.position.z);
    func_ov008_020e7360(data, NULL);
    return 1;
}

static s32 func_ov008_020e742c(TaskPool* pool, Task* task, void* args) {
    BtlPlayerNone* data = task->data;

    if (data->unk_7C != NULL) {
        data->unk_7C(data);
    }
    BtlActor_UpdateEffects(1, &data->actor);
    BtlActor_UpdatePhysics(&data->actor);
    return 1;
}

static s32 func_ov008_020e7464(TaskPool* pool, Task* task, void* args) {
    return 1;
}

static s32 func_ov008_020e746c(TaskPool* pool, Task* task, void* args) {
    return 1;
}

static s32 func_ov008_020e7474(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = data_ov008_020e7a10;

    if (BtlMgr_IsStageFrozen(stage) != 0) {
        return 1;
    }
    return stages.iter[stage](pool, task, args);
}
