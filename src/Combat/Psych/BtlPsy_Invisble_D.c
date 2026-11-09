#include "Battle/Core/BtlActor.h"
#include "Battle/Core/BtlTouch.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatPsychEffects.h"
#include <nitro/mi/cpumem.h>

#include "SpriteMgr.h"
extern const BinIdentifier data_ov003_020d77f8;
typedef struct BtlPsyInvisible {
    Ov003EffectState* effect;
    BattleSprite      sprite;
    s32               x;
    s32               y;
    void (*update)(struct BtlPsyInvisible*);
    s16 state;
    s16 timer;
    s32 visible;
    s32 active;
} BtlPsyInvisible;
static void func_ov003_02094e40(BtlPsyInvisible*);
static void func_ov003_02094edc(BtlPsyInvisible*);
static s32  func_ov003_02094f24(TaskPool* pool, Task* task, void* args);
static s32  func_ov003_02094fb8(TaskPool* pool, Task* task, void* args);
static s32  func_ov003_02094fe4(TaskPool* pool, Task* task, void* args);
static s32  func_ov003_0209504c(TaskPool* pool, Task* task, void* args);
static s32  func_ov003_02095064(TaskPool* pool, Task* task, void* args, s32 stage);

static char             data_ov003_020e3b84[24] = "Tsk_BtlPsy_Invisble_D";
static const TaskHandle data_ov003_020d7dc8     = {data_ov003_020e3b84, func_ov003_02095064, 0x7C};
// Nonmatching: mwcc places the animation table before this stage table in .rodata.
static const TaskStages data_ov003_020d7dd4 = {
    {func_ov003_02094f24, func_ov003_02094fb8, func_ov003_02094fe4, func_ov003_0209504c}
};
static const SpriteAnimEntry data_ov003_020d7de4[2] = {
    {0x35, 0x37, 0x36, 0},
    {0x35, 0x37, 0x36, 1}
};
static void func_ov003_02094da0(BtlPsyInvisible* self, void (*update)(BtlPsyInvisible*)) {
    self->update = update;
    self->state  = 0;
    self->timer  = 0;
}
static void func_ov003_02094db4(BtlPsyInvisible* self) {
    if (self->state == 0) {
        self->x = g_CombatGlobal->player->position.x;
        self->y = g_CombatGlobal->player->position.y;
        BtlSprite_SetAnim(&self->sprite, 0, 1);
        self->state++;
    }
    if (!SpriteMgr_IsAnimationFinished(&self->sprite.sprite))
        return;
    self->visible = 0;
    func_ov003_02094da0(self, func_ov003_02094e40);
}
static void func_ov003_02094e40(BtlPsyInvisible* self) {
    func_ov003_020a5a94(self->effect);
    if (BtlTouch_IsGesture(BTL_TOUCH_GESTURE_DRAG) && func_ov003_020a5aa4(self->effect) && !func_ov003_020a63a8(self->effect))
        return;
    g_CombatGlobal->unk3D8C8 = 0;
    self->visible            = 1;
    self->x                  = g_CombatGlobal->player->position.x;
    self->y                  = g_CombatGlobal->player->position.y;
    func_ov003_02094da0(self, func_ov003_02094edc);
}
static void func_ov003_02094edc(BtlPsyInvisible* self) {
    if (self->state == 0) {
        BtlSprite_SetAnim(&self->sprite, 1, 1);
        self->state++;
    }
    if (SpriteMgr_IsAnimationFinished(&self->sprite.sprite))
        self->active = 0;
}
static s32 func_ov003_02094f24(TaskPool* pool, Task* task, void* args) {
    BtlPsyInvisible*   self = task->data;
    Ov003EffectState** arg  = args;
    MI_CpuSet(self, 0, sizeof(*self));
    g_CombatGlobal->unk3D8C8 = 1;
    self->effect             = *arg;
    self->visible            = 1;
    self->active             = 1;
    func_ov003_02094da0(self, func_ov003_02094db4);
    BtlSprite_LoadFromTable(0, &self->sprite, &data_ov003_020d77f8, data_ov003_020d7de4, 0, 1, 0x30);
    return 1;
}
static s32 func_ov003_02094fb8(TaskPool* pool, Task* task, void* args) {
    BtlPsyInvisible* self = task->data;
    if (self->update)
        self->update(self);
    BtlSprite_Update(&self->sprite);
    return self->active;
}
static s32 func_ov003_02094fe4(TaskPool* pool, Task* task, void* args) {
    BtlPsyInvisible* self = task->data;
    s16              x, y;
    if (self->visible) {
        x = BtlStage_ToScreenX(0, self->x);
        y = BtlStage_ToScreenY(0, self->y, 0);
        BtlSprite_SetPosition(&self->sprite, x, y);
        BtlSprite_SetSortKey(&self->sprite, 0x7FFFFFFE - self->y);
        BtlSprite_Render(&self->sprite);
    }
    return 1;
}
static s32 func_ov003_0209504c(TaskPool* pool, Task* task, void* args) {
    BtlPsyInvisible* self = task->data;
    BtlSprite_Release(&self->sprite);
    return 1;
}

static s32 func_ov003_02095064(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = data_ov003_020d7dd4;
    return stages.iter[stage](pool, task, args);
}
s32 func_ov003_020950ac(Ov003EffectContext* context, Ov003EffectState* effect) {
    if (g_CombatGlobal->unk3D894 < 100)
        return 0;
    if (!func_ov003_02087704())
        return 0;
    Ov003EffectState** arg = func_ov003_020a5d2c(sizeof(*arg), data_ov003_020d7dc8.taskName);
    *arg                   = effect;
    context->handle        = &data_ov003_020d7dc8;
    context->arg           = arg;
    context->post          = NULL;
    return 1;
}
