/**
 * @file OtosuGame_piyo.c
 * @brief The `Tsk_OtosuGame_piyo` task.
 */

#include "OtuFieldAccessShared.h"

SpriteFrameInfo* func_ov039_0208f440(Sprite* sprite, s32 arg, s32 mode) {
    OtuPiyo* owner = sprite->owner;

    Sprite_FrameInfoCallbackSorted(sprite, mode, func_ov039_02088400(3, owner->pos.y, 0));
}

/** The dataType edit has to stay after both position stores. */
void func_ov039_0208f4fc(OtuPiyo* self, Sprite* sprite, OtuPinSpriteArgs* args) {
    SpriteAnimation anim = data_ov039_02099370;

    anim.owner    = self;
    anim.dataType = args->dataType;
    anim.posX     = F2I(self->pos.x - self->origin.x);
    anim.posY     = F2I(self->pos.y - self->origin.y);
    anim.bits_7_9 = 5;

    _Sprite_Load(sprite, &anim);
}

s32 func_ov039_0208f5a8(TaskPool* pool, Task* task, OtuPinSpriteArgs* args) {
    OtuPiyo* self = task->data;

    self->visible  = 0;
    self->pinId    = args->childId;
    self->origin.x = 0;
    self->origin.y = 0;
    self->pos.x    = 0;
    self->pos.y    = 0;

    func_ov039_0208f4fc(self, &self->sprite, args);
    return 1;
}

/**
 * Picks one of five animations from the pin's remaining stun as a percentage of
 * `lifeMax` (20/40/60/80 are the band edges), and nudges the pin every 30 frames.
 */
s32 func_ov039_0208f5e0(TaskPool* pool, Task* task, void* args) {
    OtuPiyo* self = task->data;
    void*    pin  = EasyTask_GetTaskData(pool, self->pinId);
    s16      frame;
    s32      band;
    s32      stun;

    if (pin != NULL) {
        stun = func_ov039_0208ee98(pin);

        if (stun > 0) {
            func_ov039_0208e85c(pin, &self->origin);
            func_ov039_0208e6e0(pin, &self->pos);

            band = stun * 100 / self->stunMax;

            if (band <= 0x14) {
                frame = 8;
            } else if (band <= 0x28) {
                frame = 7;
            } else if (band <= 0x3C) {
                frame = 6;
            } else if (band <= 0x50) {
                frame = 5;
            } else {
                frame = 4;
            }

            Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, frame, self->sprite.cellTable);
            Sprite_Update(&self->sprite);

            self->timer = self->timer - 1;
            if (self->timer <= 0) {
                func_ov039_02087d04(0x330, &self->pos, &self->origin);
                self->timer = 0x1E;
            }

            self->visible = 1;
        } else {
            self->visible = 0;
        }
    } else {
        self->visible = 0;
    }
    return 1;
}

s32 func_ov039_0208f6cc(TaskPool* pool, Task* task, void* args) {
    OtuPiyo* self = task->data;

    if (self->visible != 0) {
        self->sprite.posX = F2I(self->pos.x - self->origin.x);
        self->sprite.posY = F2I(self->pos.y - self->origin.y);
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

s32 func_ov039_0208f714(TaskPool* pool, Task* task, void* args) {
    Sprite_Release(&((OtuPiyo*)task->data)->sprite);
    return 1;
}

s32 func_ov039_0208f728(TaskPool* pool, Task* task, void* args, s32 stage) {
    const TaskStages stages = data_ov039_02099360;

    return stages.iter[stage](pool, task, args);
}

s32 func_ov039_0208f770(TaskPool* pool, s32 dataType, s32 childId) {
    OtuPinSpriteArgs args;

    args.dataType = dataType;
    args.childId  = childId;
    return EasyTask_CreateTask(pool, &data_ov039_02099354, NULL, 0, NULL, &args);
}

/** Sets the stun the piyo bands against and forces a nudge on the next update. */
void func_ov039_0208f7a4(OtuPiyo* self, s32 stunMax) {
    self->stunMax = stunMax;
    self->timer   = 1;
}
