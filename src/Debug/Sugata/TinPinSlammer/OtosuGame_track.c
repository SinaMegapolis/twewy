/**
 * @file OtosuGame_track.c
 * @brief The `Tsk_OtosuGame_track` task.
 */

#include "OtuFieldAccessShared.h"

extern TaskHandle       data_ov039_02099cf4;
extern const TaskStages data_ov039_02099d00;
extern SpriteAnimation  data_ov039_02099d10;

/** The track task's creation block: it keeps the pool to find its pin. */
typedef struct {
    s32       dataType;
    TaskPool* pool;
    s32       pinId;
} OtuTaskArgs3;

SpriteFrameInfo* func_ov039_0209549c(Sprite* sprite, s32 arg, s32 mode) {
    OtuTrackTask* owner = sprite->owner;

    Sprite_FrameInfoCallbackSorted(sprite, mode, func_ov039_02088400(4, owner->pos.y, 0));
}

void func_ov039_02095558(OtuTrackTask* self, Sprite* sprite, OtuTaskArgs3* args) {
    SpriteAnimation anim = data_ov039_02099d10;

    anim.owner    = self;
    anim.dataType = args->dataType;
    anim.posX     = (self->pos.x - self->origin.x) >> 12;
    anim.posY     = (self->pos.y - self->origin.y) >> 12;

    _Sprite_Load(sprite, &anim);
}

s32 func_ov039_020955f4(TaskPool* pool, Task* task, void* args) {
    OtuTrackTask* self = task->data;
    OtuTaskArgs3* a    = args;

    self->pool     = a->pool;
    self->pinId    = a->pinId;
    self->active   = 0;
    self->visible  = 0;
    self->origin.x = 0;
    self->origin.y = 0;
    self->pos.x    = 0;
    self->pos.y    = 0;

    func_ov039_02095558(self, &self->sprite, a);
    return 1;
}

/** Drifts the mark by `vel` until its animation ends. */
s32 func_ov039_02095638(TaskPool* pool, Task* task, void* args) {
    OtuTrackTask* self = task->data;

    if (self->active != 0) {
        if (SpriteMgr_IsAnimationFinished(&self->sprite)) {
            self->active  = 0;
            self->visible = 0;
        } else {
            OtuBadge* pin = EasyTask_GetTaskData(self->pool, self->pinId);

            func_ov039_0208e85c(pin, &self->origin);
            func_ov039_02098b8c(&self->pos, &self->vel, &self->pos);
            Sprite_Update(&self->sprite);
            self->visible = 1;
        }
    }
    return 1;
}

s32 func_ov039_020956a4(TaskPool* pool, Task* task, void* args) {
    OtuTrackTask* self = task->data;

    if (self->visible != 0) {
        self->sprite.posX = (self->pos.x - self->origin.x) >> 12;
        self->sprite.posY = (self->pos.y - self->origin.y) >> 12;
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

s32 func_ov039_020956f0(TaskPool* pool, Task* task, void* args) {
    Sprite_Release(&((OtuTrackTask*)task->data)->sprite);
    return 1;
}

s32 func_ov039_02095708(TaskPool* pool, Task* task, void* args, s32 stage) {
    const TaskStages stages = data_ov039_02099d00;
    return stages.iter[stage](pool, task, args);
}

s32 func_ov039_02095750(TaskPool* pool, s32 dataType, s32 pin) {
    OtuTaskArgs3 args;

    args.dataType = dataType;
    args.pool     = pool;
    args.pinId    = pin;

    return EasyTask_CreateTask(pool, &data_ov039_02099cf4, NULL, 0, NULL, &args);
}

/**
 * Drops a trail mark beside `at`: offset 12 units to one side of `angle` (the
 * side picked by `dir`), drifting outward at half speed, with an animation
 * chosen by the badge's speed `len`.
 *
 * The 64-bit `>> 12` is load-bearing: `/ 0x1000` calls `_ll_sdiv`.
 */
// Nonmatching: scratch-register choice in the index arithmetic only.
void func_ov039_02095788(OtuTrackTask* self, OtuPoint* at, s32 angle, s32 dir, s32 len) {
    s32 index;
    s16 scaleX;
    s16 scaleY;
    s16 anim;

    self->active = 1;

    // The (u16) is the target's `lsl #0x10 / lsr #0x10` before the `asr #4`.
    index       = ((u16)(angle + ((dir != 0) ? -0x6000 : 0x6000)) >> 4) * 2;
    scaleX      = ((s16*)data_0205e4e0)[index + 1];
    scaleY      = ((s16*)data_0205e4e0)[index];
    self->pos.x = scaleX * 12;
    self->pos.y = scaleY * 12;
    func_ov039_02098b8c(at, &self->pos, &self->pos);

    index       = ((u16)(angle + ((dir != 0) ? -0x4000 : 0x4000)) >> 4) * 2;
    scaleX      = ((s16*)data_0205e4e0)[index + 1];
    scaleY      = ((s16*)data_0205e4e0)[index];
    self->vel.x = (s32)(((s64)scaleX * 0x800 + 0x800) >> 12);
    self->vel.y = (s32)(((s64)scaleY * 0x800 + 0x800) >> 12);

    if (len < 0x2000) {
        anim = 3;
    } else if (len > 0x3000) {
        anim = 1;
    } else {
        anim = 2;
    }

    Sprite_SetAnimation(&self->sprite, self->sprite.animData, anim, self->sprite.cellTable);
}
