/**
 * @file OtosuGame_meteohahen.c
 * @brief The `Tsk_OtosuGame_meteohahen` task.
 */

#include "OtuFieldAccessShared.h"

s32              OtosuGame_meteohahen_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);
SpriteFrameInfo* OtosuGame_meteohahen_GetFrameInfo(Sprite* sprite, s32 arg, s32 mode);

static const TaskHandle Tsk_OtosuGame_meteohahen = {"Tsk_OtosuGame_meteohahen", OtosuGame_meteohahen_RunTask,
                                                    sizeof(OtuHahen)};

static const SpriteAnimation OtosuGame_meteohahen_Anim = {
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
    .frameInfoCallback = OtosuGame_meteohahen_GetFrameInfo,
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
    .animIndex         = 4,
};

SpriteFrameInfo* OtosuGame_meteohahen_GetFrameInfo(Sprite* sprite, s32 arg, s32 mode) {
    OtuHahen* owner = sprite->owner;

    Sprite_FrameInfoCallbackAffineSorted(sprite, mode, &owner->affine, func_ov039_02088400(3, owner->pos.y, owner->height));
}

void OtosuGame_meteohahen_Load(OtuHahen* self, Sprite* sprite, OtuPinSpriteArgs* args) {
    SpriteAnimation anim = OtosuGame_meteohahen_Anim;

    anim.owner    = self;
    anim.dataType = (u16)args->dataType;
    anim.posX     = self->pos.x >> 12;
    anim.posY     = self->pos.y >> 12;
    _Sprite_Load(sprite, &anim);
}

/** The stores are in the target's order: the flags, then the pin, then the rest. */
s32 OtosuGame_meteohahen_Init(TaskPool* pool, Task* task, void* args) {
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
    self->affine.scaleX   = 0x1800;
    self->affine.scaleY   = 0x1800;
    self->affine.unk_0C   = 0;
    self->affine.unk_0E   = 0;
    OtosuGame_meteohahen_Load(self, &self->sprite, a);
    return 1;
}

/**
 * Flies the fragment: shrinks it with its remaining life, bleeds its speed,
 * moves it along `dir` and bounces its height off the ground with damping.
 * The rescale is a rounded 64-bit Q12 multiply; `/ 0x1000` calls __ll_sdiv.
 */
s32 OtosuGame_meteohahen_Update(TaskPool* pool, Task* task, void* args) {
    OtuHahen* self = task->data;
    void*     pin  = EasyTask_GetTaskData(pool, self->pinId);

    if (pin != NULL) {
        if (self->active != 0) {
            self->life = self->life - 1;

            if (self->life <= 0) {
                self->active  = 0;
                self->visible = 0;
            } else {
                func_ov039_0208e85c(pin, &self->origin);
                self->affine.scaleY = _s32_div_f(self->life * 0x1800, self->lifeMax);
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

s32 OtosuGame_meteohahen_Render(TaskPool* pool, Task* task, void* args) {
    OtuHahen* self = task->data;

    if (self->visible != 0) {
        self->sprite.posX = (s16)((self->pos.x - self->origin.x) >> 0xC);
        self->sprite.posY = (s16)((self->pos.y - self->origin.y + self->height) >> 0xC);
        Sprite_RenderFrame(&self->sprite);
    }

    return 1;
}

s32 OtosuGame_meteohahen_Destroy(TaskPool* pool, Task* task, void* args) {
    Sprite_Release(&((OtuHahen*)task->data)->sprite);
    return 1;
}

s32 OtosuGame_meteohahen_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = OtosuGame_meteohahen_Init,
        .update     = OtosuGame_meteohahen_Update,
        .render     = OtosuGame_meteohahen_Render,
        .cleanup    = OtosuGame_meteohahen_Destroy,
    };

    return stages.iter[stage](pool, task, args);
}

s32 OtosuGame_meteohahen_CreateTask(TaskPool* pool, s32 dataType, s32 pinId) {
    OtuPinSpriteArgs args;

    args.dataType = dataType;
    args.childId  = pinId;
    return EasyTask_CreateTask(pool, &Tsk_OtosuGame_meteohahen, NULL, 0, NULL, &args);
}

/**
 * Launches a meteor fragment from `at`: a jittered speed, an upward kick, a
 * random direction from the base game's table and one of animations 10..16.
 */
// Nonmatching: 94.3%, load scheduling of the two table reads only.
void func_ov039_02097240(OtuHahen* self, OtuPoint* at) {
    s32 index;

    self->active        = 1;
    self->affine.scaleX = 0x1800;
    self->affine.scaleY = 0x1800;
    self->pos           = *at;
    self->height        = 0;

    self->speed = data_ov039_0209a314;
    self->speed = self->speed + RNG_Next(data_ov039_0209a328 - data_ov039_0209a314);
    self->vz    = 0 - data_ov039_0209a2fc;
    self->vz    = self->vz - RNG_Next(data_ov039_0209a30c - data_ov039_0209a2fc);

    self->lifeMax = OTU_ABS_AIRTIME(self->vz);
    self->life    = self->lifeMax;

    index       = (RNG_Next(0x10000) >> 4) * 2;
    self->dir.x = ((s16*)data_0205e4e0)[index + 1];
    self->dir.y = ((s16*)data_0205e4e0)[index];

    Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, (s16)(RNG_Next(7) + 0xA), self->sprite.cellTable);
}
