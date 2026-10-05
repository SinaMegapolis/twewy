/**
 * @file OtosuGame_obstacle.c
 * @brief The `Tsk_OtosuGame_obstacle` task.
 */

#include "OtuFieldAccessShared.h"

SpriteFrameInfo* func_ov039_020923c8(Sprite* sprite, s32 arg, s32 mode) {
    OtuObstacle* owner = sprite->owner;

    Sprite_FrameInfoCallbackSorted(sprite, mode, func_ov039_02088400(3, owner->pos.y, 0));
}

/**
 * Loads the obstacle sprite, patching the template's art by `kind` and its
 * palette by `kind` and `slot`. The dataType edit is one expression because
 * mwcc hoists the argument loads above it regardless.
 */
void func_ov039_02092484(OtuObstacle* self, Sprite* sprite, OtuObstacle_Args* args) {
    SpriteAnimation params = data_ov039_0209996c;
    u16             kind   = args->params->kind;

    *(u16*)&params   = (u16)(*(u16*)&params & ~0x3C) | (u16)((u16)args->oamAttrs << 2);
    params.owner     = self;
    params.packIndex = data_ov039_0209992a[kind];
    params.unk_20    = data_ov039_02099958[kind][args->slot];
    params.unk_26    = data_ov039_02099918[kind];
    params.unk_1C    = data_ov039_0209991e[kind];
    params.unk_28    = data_ov039_02099924[kind];
    params.posX      = (s16)((self->pos.x - self->origin.x) >> 12);
    params.posY      = (s16)((self->pos.y - self->origin.y) >> 12);
    _Sprite_Load(sprite, &params);
}

s32 func_ov039_0209258c(TaskPool* pool, Task* task, void* args) {
    OtuObstacle*      self      = task->data;
    OtuObstacle_Args* a         = args;
    s32               scales[3] = {0x20000, 0x18000, 0x10000}; // 16.16, per kind

    self->origin.x    = 0;
    self->origin.y    = 0;
    self->pos.x       = (s32)a->params->targetX << 12;
    self->pos.y       = (s32)a->params->targetY << 12;
    self->scale       = scales[a->params->kind];
    self->animPending = 0;

    func_ov039_02092484(self, &self->sprite, a);
    return 1;
}

/** Switches to the hit animation once when asked, then steps the sprite. */
s32 func_ov039_02092608(TaskPool* pool, Task* task, void* args) {
    OtuObstacle* self = task->data;

    if (self->animPending != 0) {
        self->sprite.animationMode = ANIM_MODE_ONCE;
        Sprite_SetAnimation(&self->sprite, self->sprite.animData, 2, self->sprite.cellTable);
        self->animPending = 0;
    }

    Sprite_Update(&self->sprite);
    return 1;
}

s32 func_ov039_02092658(TaskPool* pool, Task* task, void* args) {
    OtuObstacle* self = task->data;

    self->sprite.posX = (s16)((self->pos.x - self->origin.x) >> 12);
    self->sprite.posY = (s16)((self->pos.y - self->origin.y) >> 12);

    Sprite_RenderFrame(&self->sprite);
    return 1;
}

s32 func_ov039_02092694(TaskPool* pool, Task* task, void* args) {
    OtuObstacle* self = task->data;

    Sprite_Release(&self->sprite);
    return 1;
}

s32 func_ov039_020926a8(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = func_ov039_0209258c,
        .update     = func_ov039_02092608,
        .render     = func_ov039_02092658,
        .cleanup    = func_ov039_02092694,
    };
    return stages.iter[stage](pool, task, args);
}

s32 func_ov039_020926f0(TaskPool* pool, s32 oamAttrs, s16 unk_04, s16 slot, OtuObstacle_Params* params) {
    OtuObstacle_Args args;

    args.oamAttrs = oamAttrs;
    args.unk_04   = unk_04;
    args.slot     = slot;
    args.params   = params;

    return EasyTask_CreateTask(pool, &data_ov039_02099930, NULL, 0, NULL, &args);
}

/** Sets the point the obstacle is drawn relative to. */
void func_ov039_02092730(OtuObstacle* self, OtuPoint* origin) {
    self->origin = *origin;
}

void func_ov039_02092744(OtuObstacle* self, OtuPoint* out) {
    *out = self->pos;
}

s32 func_ov039_02092758(OtuObstacle* self) {
    return self->scale;
}

/** Plays the hit animation on the next update. */
void func_ov039_02092760(OtuObstacle* self) {
    self->animPending = 1;
}
