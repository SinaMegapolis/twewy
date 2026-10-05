/**
 * @file OtosuGame_marker.c
 * @brief The `Tsk_OtosuGame_marker` task.
 */

#include "OtuFieldAccessShared.h"

/** "Tsk_OtosuGame_marker": a sprite whose animation is chosen by the pin. */
typedef struct {
    /* 0x00 */ Sprite   sprite;
    /* 0x40 */ OtuPoint origin;
    /* 0x48 */ OtuPoint pos;
    /* 0x50 */ s32      pinId;
    /* 0x54 */ s32      frame; // func_ov039_0208e950's answer; 0 hides the marker
} OtosuGame_marker;            // Size: 0x58

s32              OtosuGame_marker_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);
SpriteFrameInfo* OtosuGame_marker_GetFrameInfo(Sprite* sprite, s32 arg, s32 mode);

static const TaskHandle Tsk_OtosuGame_marker = {"Tsk_OtosuGame_marker", OtosuGame_marker_RunTask, sizeof(OtosuGame_marker)};

static const SpriteAnimation OtosuGame_marker_Anim = {
    .bits_0_1          = 2,
    .dataType          = 0,
    .bit_6             = 0,
    .bits_7_9          = 5,
    .bits_10_11        = 0,
    .bits_12_13        = 1,
    .bits_14_15        = 0,
    .unk_02.raw        = 0,
    .posX              = 0x50,
    .posY              = 0x50,
    .frameInfoCallback = OtosuGame_marker_GetFrameInfo,
    .callbackArg       = 0,
    .owner             = NULL,
    .binIden           = &data_ov039_0209a0dc,
    .unk_18            = 2,
    .packIndex         = 8,
    .unk_1C            = 1,
    .unk_1E            = 0,
    .unk_20            = 4,
    .unk_22            = 2,
    .unk_24            = 0,
    .unk_26            = 2,
    .unk_28            = 3,
    .animIndex         = 2,
};

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

s32 OtosuGame_marker_Init(TaskPool* pool, Task* task, void* args) {
    OtuPinSpriteArgs* taskArgs = args;
    OtosuGame_marker* self     = task->data;

    self->pinId    = taskArgs->childId;
    self->origin.x = 0;
    self->origin.y = 0;
    self->pos.x    = 0;
    self->pos.y    = 0;
    self->frame    = 0;

    OtosuGame_marker_Load(self, &self->sprite, taskArgs);
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
    TaskStages stages = {
        .initialize = OtosuGame_marker_Init,
        .update     = OtosuGame_marker_Update,
        .render     = OtosuGame_marker_Render,
        .cleanup    = OtosuGame_marker_Destroy,
    };

    return stages.iter[stage](pool, task, args);
}

s32 OtosuGame_marker_CreateTask(TaskPool* pool, s32 dataType, s32 childId) {
    OtuPinSpriteArgs args;

    args.dataType = dataType;
    args.childId  = childId;
    return EasyTask_CreateTask(pool, &Tsk_OtosuGame_marker, NULL, 0, NULL, &args);
}
