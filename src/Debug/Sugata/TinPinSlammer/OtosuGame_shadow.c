/**
 * @file OtosuGame_shadow.c
 * @brief The `Tsk_OtosuGame_shadow` task.
 */

#include "OtuFieldAccessShared.h"

SpriteFrameInfo* func_ov039_0208f134(Sprite* sprite, s32 arg, s32 mode) {
    OtuShadow* owner = sprite->owner;

    Sprite_FrameInfoCallbackAffineSorted(sprite, mode, &owner->affine, func_ov039_02088400(0, owner->pos.y, 0));
}

/**
 * The only one of the four loaders that touches the OAM priority word, which is
 * what keeps the shadow behind everything else. The dataType edit has to stay
 * after both position stores.
 */
void func_ov039_0208f1f8(OtuShadow* self, Sprite* sprite, OtuPinSpriteArgs* args) {
    SpriteAnimation anim = data_ov039_02099328;

    anim.owner           = self;
    anim.dataType        = args->dataType;
    anim.posX            = F2I(self->pos.x);
    anim.posY            = F2I(self->pos.y);
    anim.unk_02.unk_02   = 1;
    data_0206a890.unk_0C = 0xA;

    _Sprite_Load(sprite, &anim);
}

s32 func_ov039_0208f2a4(TaskPool* pool, Task* task, OtuPinSpriteArgs* args) {
    OtuShadow* self = task->data;

    self->pinId           = args->childId;
    self->origin.x        = 0;
    self->origin.y        = 0;
    self->pos.x           = 0;
    self->pos.y           = 0;
    self->affine.rotation = 0;
    self->affine.scaleX   = 0x1000;
    self->affine.scaleY   = 0x1000;
    self->affine.unk_0C   = 0;
    self->affine.unk_0E   = 0;

    func_ov039_0208f1f8(self, &self->sprite, args);
    return 1;
}

/** Follows the pin and its scale, but only while func_ov039_0208e890 says exactly 1. */
s32 func_ov039_0208f2f0(TaskPool* pool, Task* task, void* args) {
    OtuShadow* self = task->data;
    void*      pin  = EasyTask_GetTaskData(pool, self->pinId);

    if (pin != NULL) {
        self->visible = func_ov039_0208e890(pin);

        if (self->visible == 1) {
            func_ov039_0208e85c(pin, &self->origin);
            func_ov039_0208e6e0(pin, &self->pos);
            self->affine.scaleY = func_ov039_0208e8c4(pin);
            self->affine.scaleX = self->affine.scaleY;
            Sprite_Update(&self->sprite);
        }
    } else {
        self->visible = 0;
    }
    return 1;
}

/** Draws the shadow three pixels down and right of the pin. */
s32 func_ov039_0208f360(TaskPool* pool, Task* task, void* args) {
    OtuShadow* self = task->data;

    if (self->visible != 0) {
        self->sprite.posX = F2I(self->pos.x - self->origin.x) + 3;
        self->sprite.posY = F2I(self->pos.y - self->origin.y) + 3;
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

s32 func_ov039_0208f3b0(TaskPool* pool, Task* task, void* args) {
    Sprite_Release(&((OtuShadow*)task->data)->sprite);
    return 1;
}

s32 func_ov039_0208f3c4(TaskPool* pool, Task* task, void* args, s32 stage) {
    const TaskStages stages = data_ov039_02099318;

    return stages.iter[stage](pool, task, args);
}

s32 func_ov039_0208f40c(TaskPool* pool, s32 dataType, s32 childId) {
    OtuPinSpriteArgs args;

    args.dataType = dataType;
    args.childId  = childId;
    return EasyTask_CreateTask(pool, &data_ov039_0209930c, NULL, 0, NULL, &args);
}
