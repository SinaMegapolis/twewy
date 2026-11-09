#include "Battle/Core/BtlActor.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatPsychEffects.h"
#include "Combat/Core/CombatTouch.h"
#include <nitro/mi/cpumem.h>

typedef struct BtlPsyTackle {
    Ov003EffectState* effect;
    s32               x;
    s32               y;
    void (*update)(struct BtlPsyTackle*);
    s16 state;
    s16 timer;
    s32 active;
    s32 handle;
} BtlPsyTackle;
static void func_ov003_02094b14(BtlPsyTackle*);
static void func_ov003_02094abc(BtlPsyTackle* self, void (*update)(BtlPsyTackle*)) {
    self->update = update;
    self->state  = 0;
    self->timer  = 0;
}
static void func_ov003_02094ad0(BtlPsyTackle* self) {
    if (!g_CombatGlobal->unk3D8C8)
        return;
    self->handle = func_ov003_020aced0(self->effect);
    func_ov003_02094abc(self, func_ov003_02094b14);
}
static void func_ov003_02094b14(BtlPsyTackle* self) {
    BattleActor* actor;
    s32          x, y, oldX;
    u16          angle;
    if (!g_CombatGlobal->unk3D8C8)
        self->active = 0;
    actor = g_CombatGlobal->player;
    x     = actor->position.x;
    oldX  = self->x;
    if (oldX == x && self->y == actor->position.y)
        return;
    y     = actor->position.y;
    angle = FX_Atan2Idx(y - self->y, actor->position.x - oldX);
    func_ov003_020acd04();
    func_ov003_020acf0c(self->handle, angle);
    self->x = g_CombatGlobal->player->position.x;
    self->y = g_CombatGlobal->player->position.y;
}
static s32 func_ov003_02094bc0(TaskPool* pool, Task* task, void* args) {
    BtlPsyTackle*      self = task->data;
    Ov003EffectState** arg  = args;
    MI_CpuSet(self, 0, sizeof(*self));
    self->effect = *arg;
    self->x      = g_CombatGlobal->player->position.x;
    self->y      = g_CombatGlobal->player->position.y;
    func_ov003_02094abc(self, func_ov003_02094ad0);
    self->active = 1;
    self->handle = -1;
    return 1;
}
static s32 func_ov003_02094c38(TaskPool* pool, Task* task, void* args) {
    BtlPsyTackle* self = task->data;
    if (func_ov003_020a63a8(self->effect) || !func_ov003_020a5aa4(self->effect))
        return 0;
    if (self->update)
        self->update(self);
    return self->active;
}
static s32 func_ov003_02094c84(TaskPool* pool, Task* task, void* args) {
    return 1;
}
static s32 func_ov003_02094c8c(TaskPool* pool, Task* task, void* args) {
    BtlPsyTackle* self = task->data;
    if (self->handle != -1)
        func_ov003_020acef4(self->handle);
    return 1;
}

static s32              func_ov003_02094cb0(TaskPool* pool, Task* task, void* args, s32 stage);
static char             data_ov003_020e3b70[20] = "Tsk_BtlPsy_Tackle";
static const TaskHandle data_ov003_020d7dac     = {data_ov003_020e3b70, func_ov003_02094cb0, 0x1C};
static const TaskStages data_ov003_020d7db8     = {
    {func_ov003_02094bc0, func_ov003_02094c38, func_ov003_02094c84, func_ov003_02094c8c}
};
static s32 func_ov003_02094cb0(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = data_ov003_020d7db8;
    return stages.iter[stage](pool, task, args);
}
s32 func_ov003_02094cf8(Ov003EffectContext* context, Ov003EffectState* effect) {
    if (!func_ov003_02087704())
        return 0;
    Ov003EffectState** arg = func_ov003_020a5d2c(sizeof(*arg), data_ov003_020d7dac.taskName);
    *arg                   = effect;
    context->handle        = &data_ov003_020d7dac;
    context->arg           = arg;
    context->post          = NULL;
    return 1;
}
s32 func_ov003_02094d4c(Ov003EffectContext* context, Ov003EffectState* effect) {
    if (!func_ov003_02087704())
        return 0;
    Ov003EffectState** arg = func_ov003_020a5d2c(sizeof(*arg), data_ov003_020d7dac.taskName);
    *arg                   = effect;
    context->handle        = &data_ov003_020d7dac;
    context->arg           = arg;
    context->post          = NULL;
    return 1;
}
