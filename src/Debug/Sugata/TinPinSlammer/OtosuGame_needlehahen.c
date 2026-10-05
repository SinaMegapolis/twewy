/**
 * @file OtosuGame_needlehahen.c
 * @brief The `Tsk_OtosuGame_needlehahen` task.
 */

#include "OtuFieldAccessShared.h"

SpriteFrameInfo* func_ov039_02097ae0(Sprite* sprite, s32 arg, s32 mode) {
    OtuHahen* owner = sprite->owner;

    Sprite_FrameInfoCallbackAffineSorted(sprite, mode, &owner->affine, func_ov039_02088400(3, owner->pos.y, owner->height));
}

void func_ov039_02097ba4(OtuHahen* self, Sprite* sprite, OtuPinSpriteArgs* args) {
    SpriteAnimation anim = data_ov039_02099ff8;

    anim.owner    = self;
    anim.dataType = (u16)args->dataType;
    anim.posX     = (s16)(self->pos.x >> 0xC);
    anim.posY     = (s16)(self->pos.y >> 0xC);
    _Sprite_Load(sprite, &anim);
}

s32 func_ov039_02097c30(TaskPool* pool, Task* task, void* args) {
    OtuHahen*         self = task->data;
    OtuPinSpriteArgs* a    = args;

    self->active          = 0;
    self->visible         = 0;
    self->pinId           = a->childId;
    self->origin.x        = 0;
    self->origin.y        = 0;
    self->pos.x           = 0;
    self->pos.y           = 0;
    self->height          = 0;
    self->dir.x           = 0;
    self->dir.y           = 0;
    self->affine.rotation = 0;
    self->affine.scaleX   = 0x1000;
    self->affine.scaleY   = 0x1000;
    self->affine.unk_0C   = 0;
    self->affine.unk_0E   = 0;
    func_ov039_02097ba4(self, &self->sprite, a);

    return 1;
}

/** func_ov039_0209702c at full scale, with the origin read before the tick. */
s32 func_ov039_02097c90(TaskPool* pool, Task* task, void* args) {
    OtuHahen* self = task->data;
    s32       life;
    void*     pin = EasyTask_GetTaskData(pool, self->pinId);

    if (pin != NULL) {
        if (self->active != 0) {
            func_ov039_0208e85c(pin, &self->origin);
            life       = self->life - 1;
            self->life = life;

            if (life <= 0) {
                self->active  = 0;
                self->visible = 0;
            } else {
                self->affine.scaleY = _s32_div_f(life * 0x1000, self->lifeMax);
                self->affine.scaleX = self->affine.scaleY;

                self->speed = self->speed - data_ov039_0209a304;

                if (self->speed < 0) {
                    self->speed = 0;
                }

                self->vz = self->vz + data_ov039_0209a310;
                func_ov039_02098c00(self->speed, &self->dir, &self->pos, &self->pos);

                self->height = self->height + self->vz;

                if (self->height > 0) {
                    self->height = 0;

                    if (self->vz > 0) {
                        self->vz = (s32)(((s64)self->vz * -data_ov039_0209a300 + 0x800) >> 0xC);
                    }
                }

                Sprite_Update(&self->sprite);
                self->visible = 1;
            }
        }
    } else {
        self->active  = 0;
        self->visible = 0;
    }

    return 1;
}

s32 func_ov039_02097dbc(TaskPool* pool, Task* task, void* args) {
    OtuHahen* self = task->data;

    if (self->visible != 0) {
        self->sprite.posX = (s16)((self->pos.x - self->origin.x) >> 0xC);
        self->sprite.posY = (s16)((self->pos.y - self->origin.y + self->height) >> 0xC);
        Sprite_RenderFrame(&self->sprite);
    }

    return 1;
}

s32 func_ov039_02097e0c(TaskPool* pool, Task* task, void* args) {
    Sprite_Release(&((OtuHahen*)task->data)->sprite);
    return 1;
}

s32 func_ov039_02097e20(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = func_ov039_02097c30,
        .update     = func_ov039_02097c90,
        .render     = func_ov039_02097dbc,
        .cleanup    = func_ov039_02097e0c,
    };

    return stages.iter[stage](pool, task, args);
}

s32 func_ov039_02097e68(TaskPool* pool, s32 dataType, s32 pinId) {
    OtuPinSpriteArgs args;

    args.dataType = dataType;
    args.childId  = pinId;
    return EasyTask_CreateTask(pool, &data_ov039_02099fdc, NULL, 0, NULL, &args);
}

/** func_ov039_02097240 at full scale, with one of animations 3..13. */
// Nonmatching: 87.6%, the same register allocation as func_ov039_02097240.
void func_ov039_02097e9c(OtuHahen* self, OtuPoint* at) {
    s32 index;

    self->active        = 1;
    self->affine.scaleX = 0x1000;
    self->affine.scaleY = 0x1000;
    self->pos.x         = at->x;
    self->pos.y         = at->y;
    self->height        = 0;
    self->speed         = data_ov039_0209a314;
    self->speed         = self->speed + RNG_Next(data_ov039_0209a328 - data_ov039_0209a314);
    self->vz            = 0 - data_ov039_0209a2fc;
    self->vz            = self->vz - RNG_Next(data_ov039_0209a30c - data_ov039_0209a2fc);
    self->lifeMax       = OTU_ABS_AIRTIME(self->vz);
    self->life          = self->lifeMax;
    index               = RNG_Next(0x10000) >> 4;
    self->dir.x         = ((s16*)data_0205e4e0)[index + 1];
    self->dir.y         = ((s16*)data_0205e4e0)[index];
    Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, (s16)(RNG_Next(0xB) + 3), self->sprite.cellTable);
}
