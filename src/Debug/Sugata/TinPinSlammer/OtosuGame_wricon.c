/**
 * @file OtosuGame_wricon.c
 * @brief The `Tsk_OtosuGame_wricon` task.
 */

#include "OtuFieldAccessShared.h"

SpriteFrameInfo* func_ov039_02096c58(Sprite* sprite, s32 arg, s32 mode) {
    Sprite_FrameInfoCallbackSorted(sprite, mode, 3);
}

void func_ov039_02096d00(OtuWricon* self, Sprite* sprite) {
    SpriteAnimation anim = data_ov039_02099ed8;

    anim.owner    = self;
    anim.dataType = (u16)self->dataType;
    _Sprite_Load(sprite, &anim);
}

s32 func_ov039_02096d70(void* pool, void* task, OtuTaskArgs1* args) {
    OtuWricon* self = ((Task*)task)->data;

    self->visible  = 1;
    self->dataType = args->dataType;
    func_ov039_02096d00(self, &self->sprite);
    return 1;
}

s32 func_ov039_02096d98(void) {
    return 1;
}

/**
 * Shows the link strength: animation `5 - level`, where ov040 reports the
 * level. Narrowed inline so it goes straight into the argument register.
 */
s32 func_ov039_02096da0(void* pool, void* task, void* args) {
    OtuWricon* self = ((Task*)task)->data;

    if (self->visible != 0) {
        Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, (s16)(5 - func_ov040_0209cb5c()), self->sprite.cellTable);
        Sprite_Update(&self->sprite);
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

s32 func_ov039_02096dec(void* pool, void* task, void* args) {
    OtuWricon* self = ((Task*)task)->data;

    Sprite_Release(&self->sprite);
    return 1;
}

s32 func_ov039_02096e04(TaskPool* pool, Task* task, void* data, s32 stage) {
    TaskStages stages = data_ov039_02099ec8;

    return stages.iter[stage](pool, task, data);
}

s32 func_ov039_02096e4c(TaskPool* pool, s32 dataType) {
    OtuTaskArgs1 args;

    args.dataType = dataType;
    return EasyTask_CreateTask(pool, &data_ov039_02099ebc, NULL, 0, NULL, &args);
}
