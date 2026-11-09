#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatPsychEffects.h"
#include <nitro/mi/cpumem.h>

static s32 BtlPsy_ShockWaveBlur_Init(TaskPool* pool, Task* task, void* args);
static s32 BtlPsy_ShockWaveBlur_Update(TaskPool* pool, Task* task, void* args);
static s32 BtlPsy_ShockWaveBlur_Render(TaskPool* pool, Task* task, void* args);
static s32 BtlPsy_ShockWaveBlur_Release(TaskPool* pool, Task* task, void* args);

static const TaskHandle Tsk_BtlPsy_Blur;

void BtlPsy_ShockWaveBlur_CreateTask(s32 animation, s32 x, s32 y, s32 z, s32 angle) {
    Ov003ShockWaveBlurArgs args;
    args.animation  = animation;
    args.position.x = x;
    args.position.y = y;
    args.position.z = z;
    args.angle      = angle;
    func_ov003_020a6000(&Tsk_BtlPsy_Blur, &args);
}

static s32 BtlPsy_ShockWaveBlur_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlPsy_ShockWaveBlur_Init,
        .update     = BtlPsy_ShockWaveBlur_Update,
        .render     = BtlPsy_ShockWaveBlur_Render,
        .cleanup    = BtlPsy_ShockWaveBlur_Release,
    };
    return stages.iter[stage](pool, task, args);
}

static char             data_ov003_020e4f0c[16] = "Tsk_BtlPsy_Blur";
static const TaskHandle Tsk_BtlPsy_Blur = {data_ov003_020e4f0c, BtlPsy_ShockWaveBlur_RunTask, sizeof(Ov003ShockWaveBlur)};

static s32 BtlPsy_ShockWaveBlur_Init(TaskPool* pool, Task* task, void* args) {
    Ov003ShockWaveBlur*     self = task->data;
    Ov003ShockWaveBlurArgs* arg  = args;
    MI_CpuSet(self, 0, sizeof(*self));
    self->position.x   = arg->position.x;
    self->position.y   = arg->position.y;
    self->position.z   = arg->position.z;
    self->fixedOpacity = 0x13000;
    self->angle        = arg->angle;
    CombatPsyAnim_Load(&self->sprite, 2, 1, arg->animation, 0);
    return 1;
}

static s32 BtlPsy_ShockWaveBlur_Update(TaskPool* pool, Task* task, void* args) {
    Ov003ShockWaveBlur* self = task->data;
    BtlSprite_Update(&self->sprite);
    self->fixedOpacity -= 0x1000;
    self->opacity = self->fixedOpacity >> 12;
    return self->opacity >= 5;
}

static s32 BtlPsy_ShockWaveBlur_Render(TaskPool* pool, Task* task, void* args) {
    Ov003ShockWaveBlur* self = task->data;
    if (self->opacity > 0) {
        s32 x = BtlStage_ToScreenX(0, self->position.x);
        s32 y = BtlStage_ToScreenY(0, self->position.y, self->position.z);
        BtlSprite_SetPosition(&self->sprite, x, y);
        BtlSprite_SetAffineTransform(&self->sprite, self->angle, 0x1000, 0x1000, 0);
        BtlSprite_RenderTranslucent(&self->sprite, self->opacity);
    }
    return 1;
}

static s32 BtlPsy_ShockWaveBlur_Release(TaskPool* pool, Task* task, void* args) {
    Ov003ShockWaveBlur* self = task->data;
    BtlSprite_Release(&self->sprite);
    return 1;
}
