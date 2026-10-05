/**
 * @file OtosuGame_meteo.c
 * @brief The `Tsk_OtosuGame_meteo` task.
 */

#include "OtuFieldAccessShared.h"

extern const SpriteAnimation OtosuGame_meteo_Anim;
extern const TaskHandle      Tsk_OtosuGame_meteo;
extern const TaskStages      data_ov039_020993f0;

/* The meteo's wind-up scale keyframes. */
extern const OtuScaleKey data_ov039_0209a54c[3];

SpriteFrameInfo* OtosuGame_meteo_GetFrameInfo(Sprite* sprite, s32 arg, s32 mode) {
    OtosuGame_meteo* owner = sprite->owner;

    Sprite_FrameInfoCallbackAffineSorted(sprite, mode, &owner->affine, func_ov039_02088400(4, owner->pos.y, owner->height));
}

void OtosuGame_meteo_Load(OtosuGame_meteo* self, Sprite* sprite, OtuPinSpriteArgs* args) {
    SpriteAnimation anim = OtosuGame_meteo_Anim;

    anim.owner    = self;
    anim.dataType = args->dataType;
    anim.posX     = F2I(self->pos.x);
    anim.posY     = F2I(self->pos.y);

    _Sprite_Load(sprite, &anim);
}

s32 OtosuGame_meteo_Init(TaskPool* pool, Task* task, OtuPinSpriteArgs* args) {
    OtosuGame_meteo* self = task->data;

    self->pinId           = args->childId;
    self->origin.x        = 0;
    self->origin.y        = 0;
    self->pos.x           = 0;
    self->pos.y           = 0;
    self->height          = 0;
    self->alive           = 0;
    self->state           = 0;
    self->affine.rotation = 0;
    self->affine.scaleX   = 0x1000;
    self->affine.scaleY   = 0x1000;
    self->affine.unk_0C   = 0;
    self->affine.unk_0E   = 0;

    OtosuGame_meteo_Load(self, &self->sprite, args);
    return 1;
}

/**
 * Follows the phase-8 pin and runs the state the 0208fefc/0208ff30/0208ff68
 * entry points set up: 1 holds, 2 eases scaleY back to 1.0, 3 plays the wind-up
 * keyframes and launches the eight fragments on its 30th-from-last frame.
 */
// Nonmatching: 94%. The target emits the null-pin `alive = 0` as its own tail
// block and again as case 0; this build merges the two stores. An early-return
// arm instead of the else was tried and is worse (88%).
s32 OtosuGame_meteo_Update(TaskPool* pool, Task* task, void* args) {
    OtosuGame_meteo* self = task->data;
    void*            pin  = EasyTask_GetTaskData(pool, self->pinId);
    s32              i;

    if (pin == NULL) {
        self->alive = 0;
    } else {
        self->alive = func_ov039_0208e984(pin);

        if (self->alive != 0) {
            func_ov039_0208e85c(pin, &self->origin);
            func_ov039_0208e6e0(pin, &self->pos);
            self->height = func_ov039_0208e6f4(pin);
        } else {
            self->state = 0;
        }

        switch (self->state) {
            case 0:
                self->alive = 0;
                break;

            case 1:
                self->timer = self->timer - 1;
                if (self->timer <= 0) {
                    self->state = 0;
                }
                break;

            case 2:
                if (self->timer > 0) {
                    self->affine.scaleY = self->affine.scaleY + _s32_div_f(0x1000 - self->affine.scaleY, self->timer);
                    self->timer         = self->timer - 1;
                }
                break;

            case 3:
                func_ov039_02087bf8(&self->scaleAnim, &self->affine);

                if (self->timer == 0x1E) {
                    for (i = 0; i < 8; i++) {
                        func_ov039_02097240(EasyTask_GetTaskData(pool, self->hahenIds[i]), &self->pos);
                    }
                }

                self->timer = self->timer - 1;
                if (self->timer <= 0) {
                    self->state = 0;
                }
                break;
        }

        if (self->alive != 0) {
            Sprite_Update(&self->sprite);
        }
    }
    return 1;
}

/** Draws the meteo raised by the pin's height. */
s32 OtosuGame_meteo_Render(TaskPool* pool, Task* task, void* args) {
    OtosuGame_meteo* self = task->data;

    if (self->alive != 0) {
        self->sprite.posX = F2I(self->pos.x - self->origin.x);
        self->sprite.posY = F2I(self->pos.y - self->origin.y + self->height);
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

s32 OtosuGame_meteo_Destroy(TaskPool* pool, Task* task, void* args) {
    OtosuGame_meteo* self = task->data;
    s32              i;

    Sprite_Release(&self->sprite);

    for (i = 0; i < 8; i++) {
        EasyTask_DeleteTask(pool, self->hahenIds[i]);
    }
    return 1;
}

s32 OtosuGame_meteo_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    const TaskStages stages = data_ov039_020993f0;

    return stages.iter[stage](pool, task, args);
}

/** Creates the meteo and its eight fragment tasks. */
s32 OtosuGame_meteo_CreateTask(TaskPool* pool, s32 dataType, s32 childId) {
    OtuPinSpriteArgs args;
    // `id` before `self`: the target keeps the id in r7 and the pool in r6.
    s32              id;
    OtosuGame_meteo* self;
    s32              i;

    args.dataType = dataType;
    args.childId  = childId;

    id   = EasyTask_CreateTask(pool, &Tsk_OtosuGame_meteo, NULL, 0, NULL, &args);
    self = EasyTask_GetTaskData(pool, id);

    for (i = 0; i < 8; i++) {
        self->hahenIds[i] = OtosuGame_meteohahen_CreateTask(pool, dataType, childId);
    }
    return id;
}

/** The meteo's position, with a fixed scale of 29.0. */
// Nonmatching: 98%, only the order of the first two stores.
void func_ov039_0208fee0(OtosuGame_meteo* self, OtuPinRecord* out) {
    s32 x = self->pos.x;
    s32 y = self->pos.y;

    out->scale = 0x1D000;
    out->x     = x;
    out->y     = y;
}

/** State 1: hold animation 1 at full scale for `frames` frames. */
void func_ov039_0208fefc(OtosuGame_meteo* self, s32 frames) {
    self->timer         = frames;
    self->affine.scaleX = 0x1000;
    self->affine.scaleY = 0x1000;

    Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, 1, self->sprite.cellTable);
    self->state = 1;
}

/** State 2: squash to a small scaleY and ease back over four frames. */
void func_ov039_0208ff30(OtosuGame_meteo* self) {
    self->timer         = 4;
    self->affine.scaleX = 0x1000;
    self->affine.scaleY = 0x29;

    Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, 4, self->sprite.cellTable);
    self->state = 2;
}

/** State 3: the 30-frame wind-up, animation 5 plus its scale keyframes. */
void func_ov039_0208ff68(OtosuGame_meteo* self) {
    self->timer = 0x1E;

    Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, 5, self->sprite.cellTable);
    func_ov039_02087ba0(&self->scaleAnim, data_ov039_0209a54c, 6, &self->affine);
    self->state = 3;
}
