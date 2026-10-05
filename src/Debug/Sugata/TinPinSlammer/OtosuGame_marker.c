/**
 * @file OtosuGame_marker.c
 * @brief The `Tsk_OtosuGame_marker` task.
 */

#include "OtuFieldAccessShared.h"

extern const SpriteAnimation OtosuGame_marker_Anim;
extern const TaskHandle      Tsk_OtosuGame_marker;
extern const TaskStages      data_ov039_020993a8;

/** "Tsk_OtosuGame_marker": a sprite whose animation is chosen by the pin. */
typedef struct {
    /* 0x00 */ Sprite   sprite;
    /* 0x40 */ OtuPoint origin;
    /* 0x48 */ OtuPoint pos;
    /* 0x50 */ s32      pinId;
    /* 0x54 */ s32      frame; // func_ov039_0208e950's answer; 0 hides the marker
} OtosuGame_marker;            // Size: 0x58

/** Sorted one row below the pin, without the packer call. */
SpriteFrameInfo* OtosuGame_marker_GetFrameInfo(Sprite* sprite, s32 arg, s32 mode) {
    OtosuGame_marker* owner = sprite->owner;

    Sprite_FrameInfoCallbackSorted(sprite, mode, (((owner->pos.y >> 12) & 0x7FF) + 1) << 12);
}

void OtosuGame_marker_Load(OtosuGame_marker* self, Sprite* sprite, OtuPinSpriteArgs* args) {
    SpriteAnimation anim = OtosuGame_marker_Anim;

    anim.owner    = self;
    anim.dataType = args->dataType;
    anim.posX     = F2I(self->pos.x);
    anim.posY     = F2I(self->pos.y);

    _Sprite_Load(sprite, &anim);
}

s32 OtosuGame_marker_Init(TaskPool* pool, Task* task, OtuPinSpriteArgs* args) {
    OtosuGame_marker* self = task->data;

    self->pinId    = args->childId;
    self->origin.x = 0;
    self->origin.y = 0;
    self->pos.x    = 0;
    self->pos.y    = 0;
    self->frame    = 0;

    OtosuGame_marker_Load(self, &self->sprite, args);
    return 1;
}

/** Shows whichever animation func_ov039_0208e950 picks for the pin. */
s32 OtosuGame_marker_Update(TaskPool* pool, Task* task, void* args) {
    OtosuGame_marker* self = task->data;
    void*             pin  = EasyTask_GetTaskData(pool, self->pinId);
    s32               prev;
    s32               frame;

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

s32 OtosuGame_marker_Render(TaskPool* pool, Task* task, void* args) {
    OtosuGame_marker* self = task->data;

    if (self->frame != 0) {
        self->sprite.posX = F2I(self->pos.x - self->origin.x);
        self->sprite.posY = F2I(self->pos.y - self->origin.y);
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

s32 OtosuGame_marker_Destroy(TaskPool* pool, Task* task, void* args) {
    Sprite_Release(&((OtosuGame_marker*)task->data)->sprite);
    return 1;
}

s32 OtosuGame_marker_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    const TaskStages stages = data_ov039_020993a8;

    return stages.iter[stage](pool, task, args);
}

s32 OtosuGame_marker_CreateTask(TaskPool* pool, s32 dataType, s32 childId) {
    OtuPinSpriteArgs args;

    args.dataType = dataType;
    args.childId  = childId;
    return EasyTask_CreateTask(pool, &Tsk_OtosuGame_marker, NULL, 0, NULL, &args);
}
