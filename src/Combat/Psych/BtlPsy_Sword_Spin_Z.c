#include "Battle/Core/BtlActor.h"
#include "Battle/Core/BtlTouch.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatPsychEffects.h"
#include <nitro/mi/cpumem.h>

static s32 func_ov003_02095d08(TaskPool* pool, Task* task, void* args) {
    Ov003PsychTrail*   self = task->data;
    Ov003EffectState** arg  = args;
    MI_CpuSet(self, 0, sizeof(*self));
    self->effect = *arg;
    CombatPsyTrail_Init(&self->trail);
    g_CombatGlobal->unk3D8C8 = 1;
    return 1;
}
static s32 func_ov003_02095d50(TaskPool* pool, Task* task, void* args) {
    Ov003PsychTrail* self = task->data;
    if (func_ov003_020a63a8(self->effect))
        return 0;
    switch (CombatPsyTrail_Update(&self->trail)) {
        case 0:
            func_ov003_020a5aa4(self->effect);
            func_ov003_020a5a94(self->effect);
            func_ov003_02087250(self->effect, g_CombatGlobal->player, 0);
            return 0;
        case 1:
            return 1;
        case 2:
            break;
    }
    return 0;
}
static s32 func_ov003_02095dd4(TaskPool* pool, Task* task, void* args) {
    Ov003PsychTrail* self = task->data;
    CombatPsyTrail_Render(&self->trail);
    return 1;
}
static s32 func_ov003_02095dec(TaskPool* pool, Task* task, void* args) {
    Ov003PsychTrail* self = task->data;
    CombatPsyTrail_Release(&self->trail);
    g_CombatGlobal->unk3D8C8 = 0;
    return 1;
}

static s32              func_ov003_02095e1c(TaskPool* pool, Task* task, void* args, s32 stage);
static char             data_ov003_020e3c08[24] = "Tsk_BtlPsy_Sword_Spin_Z";
static const TaskHandle data_ov003_020d7e64     = {data_ov003_020e3c08, func_ov003_02095e1c, 0x2B4};
static const TaskStages data_ov003_020d7e70     = {
    {func_ov003_02095d08, func_ov003_02095d50, func_ov003_02095dd4, func_ov003_02095dec}
};
static s32 func_ov003_02095e1c(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = data_ov003_020d7e70;
    return stages.iter[stage](pool, task, args);
}
s32 func_ov003_02095e64(Ov003EffectContext* context, Ov003EffectState* effect) {
    if (!BtlTouch_IsGesture(BTL_TOUCH_GESTURE_DRAG))
        return 0;
    if (BtlTouch_IsTargetKind(BTL_TOUCH_TARGET_PLAYER))
        return 0;
    Ov003EffectState** arg = func_ov003_020a5d2c(sizeof(*arg), data_ov003_020d7e64.taskName);
    *arg                   = effect;
    context->handle        = &data_ov003_020d7e64;
    context->arg           = arg;
    context->post          = NULL;
    return 1;
}
