/**
 * @file OtosuGame_track.c
 * @brief The `Tsk_OtosuGame_track` task.
 */

#include "OtuFieldAccessShared.h"

/** The track task's creation block: it keeps the pool to find its pin. */
typedef struct {
    s32       dataType;
    TaskPool* pool;
    s32       pinId;
} OtosuGame_track_Args;

s32              OtosuGame_track_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);
SpriteFrameInfo* OtosuGame_track_GetFrameInfo(Sprite* sprite, s32 arg, s32 mode);

static const TaskHandle Tsk_OtosuGame_track = {"Tsk_OtosuGame_track", OtosuGame_track_RunTask, sizeof(OtosuGame_track)};

static const SpriteAnimation OtosuGame_track_Anim = {
    .bits_0_1          = 2,
    .dataType          = 0,
    .bit_6             = 0,
    .bits_7_9          = 5,
    .bits_10_11        = 0,
    .bits_12_13        = 2,
    .bits_14_15        = 0,
    .unk_02.raw        = 0,
    .posX              = 0x50,
    .posY              = 0x50,
    .frameInfoCallback = OtosuGame_track_GetFrameInfo,
    .callbackArg       = 0,
    .owner             = NULL,
    .binIden           = &data_ov039_0209a0dc,
    .unk_18            = 2,
    .packIndex         = 3,
    .unk_1C            = 1,
    .unk_1E            = 0,
    .unk_20            = 4,
    .unk_22            = 1,
    .unk_24            = 0,
    .unk_26            = 2,
    .unk_28            = 3,
    .animIndex         = 2,
};

SpriteFrameInfo* OtosuGame_track_GetFrameInfo(Sprite* sprite, s32 arg, s32 mode) {
    OtosuGame_track* owner = sprite->owner;

    Sprite_FrameInfoCallbackSorted(sprite, mode, func_ov039_02088400(4, owner->pos.y, 0));
}

void OtosuGame_track_Load(OtosuGame_track* self, Sprite* sprite, OtosuGame_track_Args* args) {
    SpriteAnimation anim = OtosuGame_track_Anim;

    anim.owner    = self;
    anim.dataType = args->dataType;
    anim.posX     = (self->pos.x - self->origin.x) >> 12;
    anim.posY     = (self->pos.y - self->origin.y) >> 12;

    _Sprite_Load(sprite, &anim);
}

s32 OtosuGame_track_Init(TaskPool* pool, Task* task, void* args) {
    OtosuGame_track*      self = task->data;
    OtosuGame_track_Args* a    = args;

    self->pool     = a->pool;
    self->pinId    = a->pinId;
    self->active   = 0;
    self->visible  = 0;
    self->origin.x = 0;
    self->origin.y = 0;
    self->pos.x    = 0;
    self->pos.y    = 0;

    OtosuGame_track_Load(self, &self->sprite, a);
    return 1;
}

/** Drifts the mark by `vel` until its animation ends. */
s32 OtosuGame_track_Update(TaskPool* pool, Task* task, void* args) {
    OtosuGame_track* self = task->data;

    if (self->active != 0) {
        if (SpriteMgr_IsAnimationFinished(&self->sprite)) {
            self->active  = 0;
            self->visible = 0;
        } else {
            OtosuGame_badge* pin = EasyTask_GetTaskData(self->pool, self->pinId);

            func_ov039_0208e85c(pin, &self->origin);
            func_ov039_02098b8c(&self->pos, &self->vel, &self->pos);
            Sprite_Update(&self->sprite);
            self->visible = 1;
        }
    }
    return 1;
}

s32 OtosuGame_track_Render(TaskPool* pool, Task* task, void* args) {
    OtosuGame_track* self = task->data;

    if (self->visible != 0) {
        self->sprite.posX = (self->pos.x - self->origin.x) >> 12;
        self->sprite.posY = (self->pos.y - self->origin.y) >> 12;
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

s32 OtosuGame_track_Destroy(TaskPool* pool, Task* task, void* args) {
    Sprite_Release(&((OtosuGame_track*)task->data)->sprite);
    return 1;
}

s32 OtosuGame_track_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = OtosuGame_track_Init,
        .update     = OtosuGame_track_Update,
        .render     = OtosuGame_track_Render,
        .cleanup    = OtosuGame_track_Destroy,
    };
    return stages.iter[stage](pool, task, args);
}

s32 OtosuGame_track_CreateTask(TaskPool* pool, s32 dataType, s32 pin) {
    OtosuGame_track_Args args;

    args.dataType = dataType;
    args.pool     = pool;
    args.pinId    = pin;

    return EasyTask_CreateTask(pool, &Tsk_OtosuGame_track, NULL, 0, NULL, &args);
}

/**
 * Drops a trail mark beside `at`: offset 12 units to one side of `angle` (the
 * side picked by `dir`), drifting outward at half speed, with an animation
 * chosen by the badge's speed `len`.
 *
 * The 64-bit `>> 12` is load-bearing: `/ 0x1000` calls `_ll_sdiv`.
 */
// Nonmatching: scratch-register choice in the index arithmetic only.
void func_ov039_02095788(OtosuGame_track* self, OtuPoint* at, s32 angle, s32 dir, s32 len) {
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
