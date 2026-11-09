#include "Battle/Core/BtlActor.h"
#include "Battle/Core/BtlTouch.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatPsychEffects.h"
#include <nitro/mi/cpumem.h>

typedef struct BtlPsySlashArgs {
    Ov003EffectState* effect;
    void*             target;
    u16               direction;
} BtlPsySlashArgs;
typedef struct BtlPsySlash {
    Ov003EffectState* effect;
    void*             target;
    s16               state;
    s16               timer;
    void (*update)(struct BtlPsySlash*);
    s32 active;
} BtlPsySlash;
static u16 func_ov003_02095120(void) {
    Ov003Global* global = g_CombatGlobal;
    s32          x      = global->touchWork.startX;
    s32          y      = global->touchWork.startY;
    s32          px     = global->touchWork.endX;
    s32          py     = global->touchWork.endY;
    s32          dy     = y - py;
    s32          dx;
    if (dy < 0)
        dy = -dy;
    dx = x - px;
    if (dx < 0)
        dx = -dx;
    if (dx > dy)
        return x < px ? 1 : 2;
    return y < py ? 0 : 3;
}
static void func_ov003_0209519c(BtlPsySlash* self) {
    void* target;
    if (!func_ov003_020a5ab4(self->effect))
        return;
    if (BtlTouch_SlashCrosses(self->target)) {
        func_ov003_02087250(self->effect, self->target, func_ov003_02095120());
        return;
    }
    target = BtlTouch_GetSlashed(ACTOR_KIND_ENEMY);
    if (!target)
        return;
    func_ov003_02087250(self->effect, self->target, func_ov003_02095120());
    // Retail launches this attack at the old target before saving the replacement.
    self->target = target;
}
static s32 func_ov003_02095200(TaskPool* pool, Task* task, void* args) {
    BtlPsySlash*     self = task->data;
    BtlPsySlashArgs* arg  = args;
    MI_CpuSet(self, 0, sizeof(*self));
    self->effect = arg->effect;
    self->target = arg->target;
    self->active = 1;
    self->update = func_ov003_0209519c;
    self->timer  = 0;
    self->state  = 0;
    func_ov003_02087250(self->effect, self->target, arg->direction);
    g_CombatGlobal->unk3D8C8 = 1;
    return 1;
}
static s32 func_ov003_02095274(TaskPool* pool, Task* task, void* args) {
    BtlPsySlash* self = task->data;
    if (!g_CombatGlobal->unk3D8C8 || func_ov003_020a63a8(self->effect)) {
        self->update = NULL;
        self->timer  = 0;
        self->state  = 0;
        self->active = 0;
    }
    if (self->update)
        self->update(self);
    return self->active;
}
static s32 func_ov003_020952d8(TaskPool* pool, Task* task, void* args) {
    return 1;
}
static s32 func_ov003_020952e0(TaskPool* pool, Task* task, void* args) {
    g_CombatGlobal->unk3D8C8 = 0;
    func_ov003_02087310();
    return 1;
}

static s32              func_ov003_02095308(TaskPool* pool, Task* task, void* args, s32 stage);
static char             data_ov003_020e3b9c[28] = "Tsk_BtlPsy_Sword_Slash_S";
static const TaskHandle data_ov003_020d7df4     = {data_ov003_020e3b9c, func_ov003_02095308, 0x14};
static const TaskStages data_ov003_020d7e00     = {
    {func_ov003_02095200, func_ov003_02095274, func_ov003_020952d8, func_ov003_020952e0}
};
static s32 func_ov003_02095308(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = data_ov003_020d7e00;
    return stages.iter[stage](pool, task, args);
}
s32 func_ov003_02095350(Ov003EffectContext* context, Ov003EffectState* effect) {
    void* target = BtlTouch_GetSlashed(ACTOR_KIND_ENEMY);
    if (!target)
        return 0;
    BtlPsySlashArgs* arg = func_ov003_020a5d2c(sizeof(*arg), data_ov003_020d7df4.taskName);
    arg->effect          = effect;
    arg->target          = target;
    arg->direction       = func_ov003_02095120();
    context->handle      = &data_ov003_020d7df4;
    context->arg         = arg;
    context->post        = NULL;
    return 1;
}
