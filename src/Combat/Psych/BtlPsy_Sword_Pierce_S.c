#include "Battle/Core/BtlActor.h"
#include "Battle/Core/BtlTouch.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatPsychEffects.h"
#include <nitro/mi/cpumem.h>

typedef struct BtlPsyPierce {
    Ov003EffectState* effect;
    s32               unk_04;
    void (*update)(struct BtlPsyPierce*);
    s16 state;
    s16 timer;
    s32 active;
} BtlPsyPierce;
static s32 func_ov003_020958e0(void) {
    s32     y, py, x, px, dx, dy;
    Battle* global;
    if (!BtlTouch_SlashCrosses(gBattle->player))
        return 0;
    global = gBattle;
    y      = global->touchWork.startY;
    py     = global->touchWork.endY;
    x      = global->touchWork.startX;
    px     = global->touchWork.endX;
    dy     = y - py;
    if (dy < 0)
        dy = -dy;
    dx = x - px;
    if (dx < 0)
        dx = -dx;
    return dx > dy;
}
static s32 func_ov003_02095954(TaskPool* pool, Task* task, void* args) {
    BtlPsyPierce*      self = task->data;
    Ov003EffectState** arg  = args;
    MI_CpuSet(self, 0, sizeof(*self));
    gBattle->unk3D8CC = BtlStage_ToWorldX(0, gBattle->touchWork.endX);
    gBattle->unk3D8C8 = 1;
    self->effect      = *arg;
    self->active      = 1;
    self->update      = NULL;
    self->state       = 0;
    self->timer       = 0;
    BtlAttackQueue_Push(self->effect, gBattle->player, gBattle->touchWork.angle);
    return 1;
}
static s32 func_ov003_020959f4(TaskPool* pool, Task* task, void* args) {
    BtlPsyPierce* self = task->data;
    if (!gBattle->unk3D8C8 || func_ov003_020a63a8(self->effect)) {
        self->update = NULL;
        self->state  = 0;
        self->timer  = 0;
        self->active = 0;
    }
    if (self->update)
        self->update(self);
    return self->active;
}
static s32 func_ov003_02095a58(TaskPool* pool, Task* task, void* args) {
    return 1;
}
static s32 func_ov003_02095a60(TaskPool* pool, Task* task, void* args) {
    gBattle->unk3D8C8 = 0;
    return 1;
}

static s32              func_ov003_02095a80(TaskPool* pool, Task* task, void* args, s32 stage);
static char             data_ov003_020e3bd4[28] = "Tsk_BtlPsy_Sword_Pierce_S";
static const TaskHandle data_ov003_020d7e2c     = {data_ov003_020e3bd4, func_ov003_02095a80, 0x14};
static const TaskStages data_ov003_020d7e38     = {
    {func_ov003_02095954, func_ov003_020959f4, func_ov003_02095a58, func_ov003_02095a60}
};
static s32 func_ov003_02095a80(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = data_ov003_020d7e38;
    return stages.iter[stage](pool, task, args);
}
s32 func_ov003_02095ac8(Ov003EffectContext* context, Ov003EffectState* effect) {
    if (!func_ov003_020958e0())
        return 0;
    Ov003EffectState** arg = func_ov003_020a5d2c(sizeof(*arg), data_ov003_020d7e2c.taskName);
    *arg                   = effect;
    context->handle        = &data_ov003_020d7e2c;
    context->arg           = arg;
    context->post          = NULL;
    return 1;
}
