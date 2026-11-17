#include "Battle/Core/BtlActor.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Fusion/BtlFusion.h"
#include "Engine/EasyTask.h"
#include "Engine/IO/Input.h"

#include <nitro/mi/cpumem.h>

static s32 func_ov007_020e757c(TaskPool* pool, Task* task, void* args, s32 stage);

const TaskHandle Tsk_BtlPlayerLast = {"Tsk_BtlPlayerLast", func_ov007_020e757c, 0x84};

static void func_ov007_020e7360(BtlPlayerLast* data, void (*callback)(BtlPlayerLast*)) {
    data->unk_7C = callback;
    data->unk_80 = 0;
    data->unk_82 = 0;
}

static void func_ov007_020e7374(BtlPlayerLast* data, u16 arg) {
    func_ov021_020f11bc((s16)arg, &data->actor.position.x, &data->actor.position.y, &data->actor.position.z);
    data->actor.screenX = BtlStage_ToScreenX(1, data->actor.position.x);
    data->actor.screenY = BtlStage_ToScreenY(1, data->actor.position.y, data->actor.position.z);
    gBattle->auraPass   = 2;
    func_ov021_020f13b0();
}

static void func_ov007_020e73d8(BtlPlayerLast* data) {
    s32 state = func_ov021_020f1174();

    switch (state) {
        case 0:
            if (InputStatus.buttonState.pressedButtons & (INPUT_BUTTON_Y | INPUT_BUTTON_LEFT)) {
                func_ov007_020e7374(data, (u16)state);
            }
            break;
        case 2:
            if (InputStatus.buttonState.pressedButtons & (INPUT_BUTTON_A | INPUT_BUTTON_RIGHT)) {
                func_ov007_020e7374(data, (u16)state);
            }
            break;
        case 1:
            if (InputStatus.buttonState.pressedButtons &
                (INPUT_BUTTON_X | INPUT_BUTTON_UP | INPUT_BUTTON_B | INPUT_BUTTON_DOWN))
            {
                func_ov007_020e7374(data, (u16)state);
            }
            break;
    }
}

static s32 func_ov007_020e7478(TaskPool* pool, Task* task, void* args) {
    BtlPlayerLast* data = task->data;

    MI_CpuSet(data, 0, sizeof(BtlPlayerLast));
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
    func_ov007_020e7360(data, func_ov007_020e73d8);
    return 1;
}

static s32 func_ov007_020e7534(TaskPool* pool, Task* task, void* args) {
    BtlPlayerLast* data = task->data;

    if (data->unk_7C != NULL) {
        data->unk_7C(data);
    }
    BtlActor_UpdateEffects(1, &data->actor);
    BtlActor_UpdatePhysics(&data->actor);
    return 1;
}

static s32 func_ov007_020e756c(TaskPool* pool, Task* task, void* args) {
    return 1;
}

static s32 func_ov007_020e7574(TaskPool* pool, Task* task, void* args) {
    return 1;
}

static s32 func_ov007_020e757c(TaskPool* pool, Task* task, void* args, s32 stage) {
    const TaskStages stages = {
        .initialize = func_ov007_020e7478,
        .update     = func_ov007_020e7534,
        .render     = func_ov007_020e756c,
        .cleanup    = func_ov007_020e7574,
    };

    if (BtlMgr_IsStageFrozen(stage) != 0) {
        return 1;
    }
    return stages.iter[stage](pool, task, args);
}
