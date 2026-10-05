/**
 * @file OtosuGame_warp.c
 * @brief The `Tsk_OtosuGame_warp` task.
 */

#include "OtuFieldAccessShared.h"

extern const SpriteAnimation data_ov039_02099fb0;
extern const TaskHandle      data_ov039_02099f94;

SpriteFrameInfo* func_ov039_020977d0(Sprite* sprite, s32 arg, s32 mode) {
    OtuWarp* owner = sprite->owner;

    Sprite_FrameInfoCallbackSorted(sprite, mode, func_ov039_02088400(3, owner->pos.y, 0));
}

void func_ov039_0209788c(OtuWarp* self, Sprite* sprite, OtuPinSpriteArgs* args) {
    SpriteAnimation anim = data_ov039_02099fb0;

    anim.owner    = self;
    anim.dataType = (u16)args->dataType;
    anim.posX     = (s16)(self->pos.x >> 0xC);
    anim.posY     = (s16)(self->pos.y >> 0xC);
    _Sprite_Load(sprite, &anim);
}

s32 func_ov039_02097918(TaskPool* pool, Task* task, void* args) {
    OtuWarp*          self = task->data;
    OtuPinSpriteArgs* a    = args;

    self->active   = 0;
    self->visible  = 0;
    self->pinId    = a->childId;
    self->origin.x = 0;
    self->origin.y = 0;
    self->pos.x    = 0;
    self->pos.y    = 0;
    func_ov039_0209788c(self, &self->sprite, a);

    return 1;
}

/**
 * Plays the animation once at its point. Nested positively: written as
 * `if (pin == NULL) ... else if`, mwcc tail-merges the two clears.
 */
s32 func_ov039_02097954(TaskPool* pool, Task* task, void* args) {
    OtuWarp* self = task->data;
    void*    pin  = EasyTask_GetTaskData(pool, self->pinId);

    if (pin != NULL) {
        if (self->active != 0) {
            if (SpriteMgr_IsAnimationFinished(&self->sprite) == 0) {
                func_ov039_0208e85c(pin, &self->origin);
                Sprite_Update(&self->sprite);
                self->visible = 1;
            } else {
                self->active  = 0;
                self->visible = 0;
            }
        }
    } else {
        self->active  = 0;
        self->visible = 0;
    }

    return 1;
}

s32 func_ov039_020979cc(TaskPool* pool, Task* task, void* args) {
    OtuWarp* self = task->data;

    if (self->visible != 0) {
        self->sprite.posX = (s16)((self->pos.x - self->origin.x) >> 0xC);
        self->sprite.posY = (s16)((self->pos.y - self->origin.y) >> 0xC);
        Sprite_RenderFrame(&self->sprite);
    }

    return 1;
}

s32 func_ov039_02097a14(TaskPool* pool, Task* task, void* args) {
    Sprite_Release(&((OtuWarp*)task->data)->sprite);
    return 1;
}

s32 func_ov039_02097a28(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = func_ov039_02097918,
        .update     = func_ov039_02097954,
        .render     = func_ov039_020979cc,
        .cleanup    = func_ov039_02097a14,
    };

    return stages.iter[stage](pool, task, args);
}

s32 func_ov039_02097a70(TaskPool* pool, s32 dataType, s32 pinId) {
    OtuPinSpriteArgs args;

    args.dataType = dataType;
    args.childId  = pinId;
    return EasyTask_CreateTask(pool, &data_ov039_02099f94, NULL, 0, NULL, &args);
}

/**
 * Plays `animation` at `at`. `animation` stays s32: declared s16, mwcc drops
 * the narrowing pair the target has.
 */
// Nonmatching: 68.3%, scheduling of the narrowing pair and the pool load.
void func_ov039_02097aa4(OtuWarp* self, OtuPoint* at, s32 animation) {
    self->active = 1;
    self->pos.x  = at->x;
    self->pos.y  = at->y;
    Sprite_SetAnimation(&self->sprite, self->sprite.animData, (s16)animation, self->sprite.cellTable);
}

/** True while the warp is playing. */
s32 func_ov039_02097ad8(OtuWarp* self) {
    return self->active;
}
