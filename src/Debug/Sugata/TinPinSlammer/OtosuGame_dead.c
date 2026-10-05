/**
 * @file OtosuGame_dead.c
 * @brief The `Tsk_OtosuGame_dead` task.
 */

#include "OtuFieldAccessShared.h"

extern const TaskHandle Tsk_OtosuGame_dead;
extern const TaskStages data_ov039_02099de0;

/* The sprite template the loader copies to the stack. */
extern const SpriteAnimation OtosuGame_dead_Anim;

SpriteFrameInfo* OtosuGame_dead_GetFrameInfo(Sprite* sprite, s32 arg, s32 mode) {
    OtosuGame_dead* owner = sprite->owner;

    Sprite_FrameInfoCallbackSorted(sprite, mode, func_ov039_02088400(3, owner->pos.y, 0));
}

void OtosuGame_dead_Load(OtosuGame_dead* self, Sprite* sprite, OtuPinSpriteArgs* args) {
    SpriteAnimation anim = OtosuGame_dead_Anim;

    anim.owner    = self;
    anim.dataType = (u16)args->dataType;
    anim.posX     = self->pos.x >> 12;
    anim.posY     = self->pos.y >> 12;
    _Sprite_Load(sprite, &anim);
}

s32 OtosuGame_dead_Init(void* pool, void* task, OtuPinSpriteArgs* args) {
    OtosuGame_dead* self = ((Task*)task)->data;

    self->pinId    = args->childId;
    self->origin.x = 0;
    self->origin.y = 0;
    self->pos.x    = 0;
    self->pos.y    = 0;
    self->visible  = 0;
    self->state    = 0;
    OtosuGame_dead_Load(self, &self->sprite, args);
    return 1;
}

/** Follows the pin while playing, and hides once the animation has finished. */
s32 OtosuGame_dead_Update(void* pool, void* task, void* args) {
    OtosuGame_dead* self = ((Task*)task)->data;
    void*           pin  = EasyTask_GetTaskData(pool, self->pinId);

    if (pin != NULL) {
        switch (self->state) {
            case 0:
                self->visible = 0;
                break;

            case 1:
                func_ov039_0208e85c(pin, &self->origin);
                func_ov039_0208e6e0(pin, &self->pos);
                if (self->sprite.isPlaying != 1) {
                    self->visible = 1;
                } else {
                    self->state   = 0;
                    self->visible = 0;
                }
                break;
        }
    } else {
        self->visible = 0;
    }

    if (self->visible != 0) {
        Sprite_Update(&self->sprite);
    }
    return 1;
}

s32 OtosuGame_dead_Render(void* pool, void* task, void* args) {
    OtosuGame_dead* self = ((Task*)task)->data;

    if (self->visible != 0) {
        self->sprite.posX = (self->pos.x - self->origin.x) >> 12;
        self->sprite.posY = (self->pos.y - self->origin.y) >> 12;
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

s32 OtosuGame_dead_Destroy(void* pool, void* task, void* args) {
    OtosuGame_dead* self = ((Task*)task)->data;

    Sprite_Release(&self->sprite);
    return 1;
}

s32 OtosuGame_dead_RunTask(TaskPool* pool, Task* task, void* data, s32 stage) {
    TaskStages stages = data_ov039_02099de0;

    return stages.iter[stage](pool, task, data);
}

s32 OtosuGame_dead_CreateTask(TaskPool* pool, s32 dataType, s32 pinId) {
    OtuPinSpriteArgs args;

    args.dataType = dataType;
    args.childId  = pinId;
    return EasyTask_CreateTask(pool, &Tsk_OtosuGame_dead, NULL, 0, NULL, &args);
}

/** Plays the knockout animation. */
void func_ov039_0209657c(OtosuGame_dead* self) {
    self->state = 1;
    Sprite_SetAnimation(&self->sprite, self->sprite.animData, 4, self->sprite.cellTable);
}
