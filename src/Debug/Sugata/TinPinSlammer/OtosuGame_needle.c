/**
 * @file OtosuGame_needle.c
 * @brief The `Tsk_OtosuGame_needle` task.
 */

#include "OtuFieldAccessShared.h"

SpriteFrameInfo* func_ov039_02091118(Sprite* sprite, s32 arg, s32 mode) {
    OtuNeedle* owner = sprite->owner;

    Sprite_FrameInfoCallbackAffineSorted(sprite, mode, &owner->affine, func_ov039_02088400(4, owner->pos.y, 0));
}

void func_ov039_020911dc(OtuNeedle* self, Sprite* sprite, OtuPinSpriteArgs* args) {
    SpriteAnimation anim = data_ov039_02099578;

    anim.owner    = self;
    anim.dataType = (u16)args->dataType;
    anim.posX     = self->pos.x >> 12;
    anim.posY     = self->pos.y >> 12;
    _Sprite_Load(sprite, &anim);
}

/** The stores are in the target's order, not by address. */
s32 func_ov039_02091268(TaskPool* pool, Task* task, OtuPinSpriteArgs* args) {
    OtuNeedle* self = task->data;

    self->pool            = pool;
    self->visible         = 0;
    self->pinId           = args->childId;
    self->origin.x        = 0;
    self->origin.y        = 0;
    self->pos.x           = 0;
    self->pos.y           = 0;
    self->height          = 0;
    self->affine.rotation = 0;
    self->affine.scaleX   = 0x1000;
    self->affine.scaleY   = 0x1000;
    self->affine.unk_0C   = 0;
    self->affine.unk_0E   = 0;
    self->state           = 0;

    func_ov039_020911dc(self, &self->sprite, args);
    return 1;
}

/**
 * Follows the phase-6 pin through the needle's states: 1 starts the charge,
 * 2 grows the needle to full size over `chargeFrames`, 3 holds it for
 * `holdFrames`, 4 shrinks it back. The cases fall through on the frame a state
 * finishes.
 */
s32 func_ov039_020912c8(TaskPool* pool, Task* task, void* args) {
    OtuNeedle* self = task->data;
    void*      pin;

    pin = EasyTask_GetTaskData(pool, self->pinId);

    if (pin != NULL) {
        s32 alive = func_ov039_0208e9d0(pin);

        self->visible = alive;
        if (alive == 0) {
            self->state = 0;
        } else {
            func_ov039_0208e85c(pin, &self->origin);
            func_ov039_0208e6e0(pin, &self->pos);
            self->height = func_ov039_0208e6f4(pin);
        }

        switch (self->state) {
            case 0:
                self->visible = 0;
                break;

            case 1:
                self->affine.scaleY = 0x99A;
                self->affine.scaleX = 0x99A;
                Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, 0xE, self->sprite.cellTable);
                self->timer = self->chargeFrames;
                self->state = 2;
                /* fall through */
            case 2:
                self->affine.scaleX = self->affine.scaleX + (0x1000 - self->affine.scaleX) / self->timer;
                self->affine.scaleY = self->affine.scaleX;

                self->timer = self->timer - 1;

                if (self->timer > 0) {
                    break;
                }

                self->sprite.animationMode = ANIM_MODE_ONCE;
                Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, 0xF, self->sprite.cellTable);
                func_ov039_02087d04(0x334, &self->pos, &self->origin);
                self->timer     = self->holdFrames;
                self->holdTimer = 0x1E;
                self->state     = 3;
                /* fall through */
            case 3:
                if (self->holdTimer > 0) {
                    self->holdTimer = self->holdTimer - 1;
                }
                self->timer = self->timer - 1;

                if (self->timer > 0) {
                    break;
                }

                self->sprite.animationMode = ANIM_MODE_ONCE;
                Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, 2, self->sprite.cellTable);
                func_ov039_02087d04(0x336, &self->pos, &self->origin);
                self->timer = 0xE;
                self->state = 4;
                /* fall through */
            case 4:
                self->affine.scaleX = self->affine.scaleX + (0x99A - self->affine.scaleX) / self->timer;
                self->affine.scaleY = self->affine.scaleX;

                self->timer = self->timer - 1;

                if (self->timer <= 0) {
                    self->state = 0;
                }
                break;
        }

        if (self->visible != 0) {
            Sprite_Update(&self->sprite);
        }
    } else {
        self->visible = 0;
    }
    return 1;
}

s32 func_ov039_020914d0(TaskPool* pool, Task* task, void* args) {
    OtuNeedle* self = task->data;

    if (self->visible != 0) {
        self->sprite.posX = (s16)((self->pos.x - self->origin.x) >> 12);
        self->sprite.posY = (s16)((self->pos.y + self->height - self->origin.y) >> 12);
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

s32 func_ov039_02091524(TaskPool* pool, Task* task, void* args) {
    OtuNeedle* self = task->data;
    s32        i;

    Sprite_Release(&self->sprite);

    for (i = 0; i < 8; i++) {
        EasyTask_DeleteTask(pool, self->hahenIds[i]);
    }
    return 1;
}

s32 func_ov039_02091560(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = data_ov039_02099568;

    return stages.iter[stage](pool, task, args);
}

/** Creates the needle and its eight fragment tasks. */
s32 func_ov039_020915a8(TaskPool* pool, s32 dataType, s32 pinId) {
    OtuPinSpriteArgs args;
    s32              handle;
    OtuNeedle*       self;
    s32              i;

    args.dataType = dataType;
    args.childId  = pinId;

    handle = EasyTask_CreateTask(pool, &data_ov039_0209955c, NULL, 0, NULL, &args);
    self   = EasyTask_GetTaskData(pool, handle);

    for (i = 0; i < 8; i++) {
        self->hahenIds[i] = func_ov039_02097e68(pool, dataType, pinId);
    }
    return handle;
}

/** The needle's tip while it is held, with a fixed scale of 32.0. */
s32 func_ov039_02091628(OtuNeedle* self, OtuPinRecord* out) {
    s32 y;
    s32 x;

    if (self->state != 3) {
        return 0;
    }

    y = self->pos.y;
    x = self->pos.x;

    out->x     = x;
    out->y     = y;
    out->scale = 0x20000;
    return 1;
}

/** Starts the needle: charge for `chargeFrames`, hold for `holdFrames`. */
void func_ov039_02091654(OtuNeedle* self, s32 chargeFrames, s32 holdFrames) {
    self->chargeFrames = chargeFrames;
    self->holdFrames   = holdFrames;
    self->state        = 1;
}

/** While held, cuts the hold short to what is left of its first 30 frames. */
void func_ov039_02091668(OtuNeedle* self) {
    if (self->state == 3) {
        self->timer = self->holdTimer;
    }
}

/** True while the needle is out. */
s32 func_ov039_0209167c(OtuNeedle* self) {
    return self->state != 0;
}

/** Throws the eight fragments from the needle's tip. */
void func_ov039_02091690(OtuNeedle* self) {
    s32 i;

    for (i = 0; i < 8; i++) {
        func_ov039_02097e9c(EasyTask_GetTaskData(self->pool, self->hahenIds[i]), &self->pos);
    }
}
