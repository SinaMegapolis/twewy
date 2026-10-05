/**
 * @file OtosuGame_marker.c
 * @brief The `Tsk_OtosuGame_marker` task.
 */

#include "OtuFieldAccessShared.h"

/** Sorted one row below the pin, without the packer call. */
SpriteFrameInfo* func_ov039_0208f7b4(Sprite* sprite, s32 arg, s32 mode) {
    OtuMarker* owner = sprite->owner;

    Sprite_FrameInfoCallbackSorted(sprite, mode, (((owner->pos.y >> 12) & 0x7FF) + 1) << 12);
}

void func_ov039_0208f874(OtuMarker* self, Sprite* sprite, OtuPinSpriteArgs* args) {
    SpriteAnimation anim = data_ov039_020993b8;

    anim.owner    = self;
    anim.dataType = args->dataType;
    anim.posX     = F2I(self->pos.x);
    anim.posY     = F2I(self->pos.y);

    _Sprite_Load(sprite, &anim);
}

s32 func_ov039_0208f900(TaskPool* pool, Task* task, OtuPinSpriteArgs* args) {
    OtuMarker* self = task->data;

    self->pinId    = args->childId;
    self->origin.x = 0;
    self->origin.y = 0;
    self->pos.x    = 0;
    self->pos.y    = 0;
    self->frame    = 0;

    func_ov039_0208f874(self, &self->sprite, args);
    return 1;
}

/** Shows whichever animation func_ov039_0208e950 picks for the pin. */
s32 func_ov039_0208f938(TaskPool* pool, Task* task, void* args) {
    OtuMarker* self = task->data;
    void*      pin  = EasyTask_GetTaskData(pool, self->pinId);
    s32        prev;
    s32        frame;

    if (pin != NULL) {
        prev        = self->frame;
        frame       = func_ov039_0208e950(pin);
        self->frame = frame;

        if (frame > 0) {
            if (frame != prev) {
                Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, (s16)frame, self->sprite.cellTable);
            }
            func_ov039_0208e85c(pin, &self->origin);
            func_ov039_0208e6e0(pin, &self->pos);
            Sprite_Update(&self->sprite);
        }
    } else {
        self->frame = 0;
    }
    return 1;
}

s32 func_ov039_0208f9b8(TaskPool* pool, Task* task, void* args) {
    OtuMarker* self = task->data;

    if (self->frame != 0) {
        self->sprite.posX = F2I(self->pos.x - self->origin.x);
        self->sprite.posY = F2I(self->pos.y - self->origin.y);
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

s32 func_ov039_0208fa00(TaskPool* pool, Task* task, void* args) {
    Sprite_Release(&((OtuMarker*)task->data)->sprite);
    return 1;
}

s32 func_ov039_0208fa14(TaskPool* pool, Task* task, void* args, s32 stage) {
    const TaskStages stages = data_ov039_020993a8;

    return stages.iter[stage](pool, task, args);
}

s32 func_ov039_0208fa5c(TaskPool* pool, s32 dataType, s32 childId) {
    OtuPinSpriteArgs args;

    args.dataType = dataType;
    args.childId  = childId;
    return EasyTask_CreateTask(pool, &data_ov039_0209939c, NULL, 0, NULL, &args);
}
