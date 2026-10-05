/**
 * @file OtosuGame_wricon.c
 * @brief The `Tsk_OtosuGame_wricon` task.
 */

#include "OtuFieldAccessShared.h"

/** "Tsk_OtosuGame_wricon": the wireless signal-strength icon. */
typedef struct {
    /* 0x00 */ s32    dataType;
    /* 0x04 */ Sprite sprite;
    /* 0x44 */ s32    visible;
} OtosuGame_wricon; // Size: 0x48

extern const TaskHandle      Tsk_OtosuGame_wricon;
extern const TaskStages      data_ov039_02099ec8;
extern const SpriteAnimation OtosuGame_wricon_Anim;

/* Overlay 40's animation-phase counter, read by 02096da0. */
extern s32 func_ov040_0209cb5c(void);

SpriteFrameInfo* OtosuGame_wricon_GetFrameInfo(Sprite* sprite, s32 arg, s32 mode) {
    Sprite_FrameInfoCallbackSorted(sprite, mode, 3);
}

void OtosuGame_wricon_Load(OtosuGame_wricon* self, Sprite* sprite) {
    SpriteAnimation anim = OtosuGame_wricon_Anim;

    anim.owner    = self;
    anim.dataType = (u16)self->dataType;
    _Sprite_Load(sprite, &anim);
}

s32 OtosuGame_wricon_Init(void* pool, void* task, OtuTaskArgs1* args) {
    OtosuGame_wricon* self = ((Task*)task)->data;

    self->visible  = 1;
    self->dataType = args->dataType;
    OtosuGame_wricon_Load(self, &self->sprite);
    return 1;
}

s32 OtosuGame_wricon_Update(void) {
    return 1;
}

/**
 * Shows the link strength: animation `5 - level`, where ov040 reports the
 * level. Narrowed inline so it goes straight into the argument register.
 */
s32 OtosuGame_wricon_Render(void* pool, void* task, void* args) {
    OtosuGame_wricon* self = ((Task*)task)->data;

    if (self->visible != 0) {
        Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, (s16)(5 - func_ov040_0209cb5c()), self->sprite.cellTable);
        Sprite_Update(&self->sprite);
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

s32 OtosuGame_wricon_Destroy(void* pool, void* task, void* args) {
    OtosuGame_wricon* self = ((Task*)task)->data;

    Sprite_Release(&self->sprite);
    return 1;
}

s32 OtosuGame_wricon_RunTask(TaskPool* pool, Task* task, void* data, s32 stage) {
    TaskStages stages = data_ov039_02099ec8;

    return stages.iter[stage](pool, task, data);
}

s32 OtosuGame_wricon_CreateTask(TaskPool* pool, s32 dataType) {
    OtuTaskArgs1 args;

    args.dataType = dataType;
    return EasyTask_CreateTask(pool, &Tsk_OtosuGame_wricon, NULL, 0, NULL, &args);
}
