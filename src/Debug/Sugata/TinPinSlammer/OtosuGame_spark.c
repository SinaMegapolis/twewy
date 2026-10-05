/**
 * @file OtosuGame_spark.c
 * @brief The `Tsk_OtosuGame_spark` task.
 */

#include "OtuFieldAccessShared.h"

SpriteFrameInfo* func_ov039_02094ae8(Sprite* sprite, s32 arg, s32 mode) {
    OtuSpark* owner = sprite->owner;

    Sprite_FrameInfoCallbackAffineSorted(sprite, mode, &owner->affine, func_ov039_02088400(3, owner->pos.y, owner->height));
}

void func_ov039_02094bac(OtuSpark* self, Sprite* sprite, OtuTaskArgs1* args) {
    SpriteAnimation anim = data_ov039_02099c80;

    anim.owner    = self;
    anim.dataType = (u16)args->dataType;
    anim.posX     = (s32)((self->pos.x - self->origin.x) >> 12);
    anim.posY     = (s32)((self->height + (self->pos.y - self->origin.y)) >> 12);
    _Sprite_Load(sprite, &anim);
}

s32 func_ov039_02094c50(TaskPool* pool, Task* task, OtuTaskArgs1* args) {
    OtuSpark* self = task->data;

    self->active          = 0;
    self->visible         = 0;
    self->origin.x        = 0;
    self->origin.y        = 0;
    self->pos.x           = 0;
    self->pos.y           = 0;
    self->height          = 0;
    self->dir.x           = 0;
    self->dir.y           = 0;
    self->affine.rotation = 0;
    self->affine.scaleX   = 0x2000;
    self->affine.scaleY   = 0x2000;
    self->affine.unk_0C   = 0;
    self->affine.unk_0E   = 0;

    func_ov039_02094bac(self, &self->sprite, args);
    return 1;
}

/**
 * Flies the spark like an OtuHahen. The clear is the `then` arm and there is
 * one shared `return 1`: that is the target's block layout.
 */
s32 func_ov039_02094ca8(TaskPool* pool, Task* task, void* args) {
    OtuSpark* self = task->data;
    s32       count;
    s32       scale;
    s32       v;

    if (self->active != 0) {
        count      = self->life - 1;
        self->life = count;

        if (count <= 0) {
            self->active  = 0;
            self->visible = 0;
        } else {
            scale               = (count << 13) / self->lifeMax;
            self->affine.scaleY = scale;
            self->affine.scaleX = scale;

            v           = self->speed - data_ov039_0209a304;
            self->speed = v;
            if (v < 0) {
                self->speed = 0;
            }

            self->vz = self->vz + data_ov039_0209a310;

            func_ov039_02098c00(self->speed, &self->dir, &self->pos, &self->pos);

            v            = self->height + self->vz;
            self->height = v;
            if (v > 0) {
                self->height = 0;
                if (self->vz > 0) {
                    self->vz = (s32)(((s64)self->vz * -data_ov039_0209a300 + 0x800) >> 12);
                }
            }

            Sprite_Update(&self->sprite);
            self->visible = 1;
        }
    }
    return 1;
}

s32 func_ov039_02094dac(TaskPool* pool, Task* task, void* args) {
    OtuSpark* self = task->data;

    if (self->visible != 0) {
        self->sprite.posX = (s16)((self->pos.x - self->origin.x) >> 12);
        self->sprite.posY = (s16)((self->height + (self->pos.y - self->origin.y)) >> 12);
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

s32 func_ov039_02094dfc(TaskPool* pool, Task* task, void* args) {
    Sprite_Release(&((OtuSpark*)task->data)->sprite);
    return 1;
}

s32 func_ov039_02094e10(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = data_ov039_02099c70;

    return stages.iter[stage](pool, task, args);
}

s32 func_ov039_02094e58(TaskPool* pool, s32 dataType) {
    OtuTaskArgs1 args;

    args.dataType = dataType;
    return EasyTask_CreateTask(pool, &data_ov039_02099c64, NULL, 0, NULL, &args);
}

/** Sets the point the spark is drawn relative to. */
void func_ov039_02094e88(OtuSpark* self, OtuPoint* origin) {
    self->origin = *origin;
}

/**
 * Launches a spark from `at`: a jittered speed, an upward kick, a random
 * direction from the base game's table and one of animations 1..3.
 */
// Nonmatching: 88.9%, scheduling at the top: the target loads both halves of
// `at` before storing either.
void func_ov039_02094e9c(OtuSpark* self, OtuPoint* at) {
    s32 cell;
    s32 airtime;

    self->active        = 1;
    self->affine.scaleX = 0x2000;
    self->affine.scaleY = 0x2000;
    self->pos.x         = at->x;
    self->pos.y         = at->y;
    self->height        = 0;

    self->speed = data_ov039_0209a314;
    self->speed = self->speed + RNG_Next(data_ov039_0209a328 - data_ov039_0209a314);

    self->vz = -data_ov039_0209a2fc;
    self->vz = self->vz - RNG_Next(data_ov039_0209a30c - data_ov039_0209a2fc);

    airtime = OTU_ABS_AIRTIME(self->vz);

    self->lifeMax = airtime;
    self->life    = airtime;

    cell        = RNG_Next(0x10000) >> 4;
    self->dir.x = ((s16*)data_0205e4e0)[cell * 2 + 1];
    self->dir.y = ((s16*)data_0205e4e0)[cell * 2];

    Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, (s16)(RNG_Next(3) + 1), self->sprite.cellTable);
}
