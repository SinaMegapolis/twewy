#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatSprite.h"

#include "Combat/Core/BtlInf.h"

// The "Auto" HUD label: one fixed sprite, animated and drawn only while battle flag 0x10 is set.

static s32 BtlInf_Auto_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

const TaskHandle Tsk_BtlInf_Auto = {"Tsk_BtlInf_Auto", BtlInf_Auto_RunTask, sizeof(CombatSprite)};

extern const BinIdentifier data_ov003_020d7800;

static s32 BtlInf_Auto_Init(TaskPool* pool, Task* task, void* args) {
    CombatSprite* sprite = task->data;

    CombatSprite_LoadDirect(1, sprite, &data_ov003_020d7800, 1, 3, 2, 0x3F, 0);
    CombatSprite_SetPosition(sprite, 128, 184);
    func_ov003_02082738(sprite, 0);
    return 1;
}

static s32 BtlInf_Auto_Update(TaskPool* pool, Task* task, void* args) {
    CombatSprite* sprite = task->data;

    if (data_ov003_020e71b8->unk3D878 & 0x10) {
        CombatSprite_Update(sprite);
    }
    return 1;
}

static s32 BtlInf_Auto_Render(TaskPool* pool, Task* task, void* args) {
    CombatSprite* sprite = task->data;

    if (data_ov003_020e71b8->unk3D878 & 0x10) {
        CombatSprite_Render(sprite);
    }
    return 1;
}

static s32 BtlInf_Auto_Release(TaskPool* pool, Task* task, void* args) {
    CombatSprite_Release(task->data);
    return 1;
}

static s32 BtlInf_Auto_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlInf_Auto_Init,
        .update     = BtlInf_Auto_Update,
        .render     = BtlInf_Auto_Render,
        .cleanup    = BtlInf_Auto_Release,
    };
    if (func_ov003_0208b690(stage) != 0) {
        return 1;
    } else {
        return stages.iter[stage](pool, task, args);
    }
}
