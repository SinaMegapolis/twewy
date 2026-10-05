/**
 * @file OtosuGame_needle.c
 * @brief The `Tsk_OtosuGame_needle` task.
 */

#include "OtuFieldAccessShared.h"

s32              OtosuGame_needle_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);
SpriteFrameInfo* OtosuGame_needle_GetFrameInfo(Sprite* sprite, s32 arg, s32 mode);

static const TaskHandle Tsk_OtosuGame_needle = {"Tsk_OtosuGame_needle", OtosuGame_needle_RunTask, sizeof(OtosuGame_needle)};

static const SpriteAnimation OtosuGame_needle_Anim = {
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
    .frameInfoCallback = OtosuGame_needle_GetFrameInfo,
    .callbackArg       = 0,
    .owner             = NULL,
    .binIden           = &data_ov039_0209a0dc,
    .unk_18            = 2,
    .packIndex         = 7,
    .unk_1C            = 1,
    .unk_1E            = 0,
    .unk_20            = 4,
    .unk_22            = 1,
    .unk_24            = 0,
    .unk_26            = 2,
    .unk_28            = 3,
    .animIndex         = 0xE,
};

SpriteFrameInfo* OtosuGame_needle_GetFrameInfo(Sprite* sprite, s32 arg, s32 mode) {
    OtosuGame_needle* owner = sprite->owner;

    Sprite_FrameInfoCallbackAffineSorted(sprite, mode, &owner->affine, func_ov039_02088400(4, owner->pos.y, 0));
}

void OtosuGame_needle_Load(OtosuGame_needle* self, Sprite* sprite, OtuPinSpriteArgs* args) {
    SpriteAnimation anim = OtosuGame_needle_Anim;

    anim.owner    = self;
    anim.dataType = (u16)args->dataType;
    anim.posX     = self->pos.x >> 12;
    anim.posY     = self->pos.y >> 12;
    _Sprite_Load(sprite, &anim);
}

/** The stores are in the target's order, not by address. */
s32 OtosuGame_needle_Init(TaskPool* pool, Task* task, void* args) {
    OtuPinSpriteArgs* taskArgs = args;
    OtosuGame_needle* self     = task->data;

    self->pool            = pool;
    self->visible         = 0;
    self->pinId           = taskArgs->childId;
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

    OtosuGame_needle_Load(self, &self->sprite, taskArgs);
    return 1;
}

/**
 * Follows the phase-6 pin through the needle's states: 1 starts the charge,
 * 2 grows the needle to full size over `chargeFrames`, 3 holds it for
 * `holdFrames`, 4 shrinks it back. The cases fall through on the frame a state
 * finishes.
 */
s32 OtosuGame_needle_Update(TaskPool* pool, Task* task, void* args) {
    OtosuGame_needle* self = task->data;
    void*             pin;

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

s32 OtosuGame_needle_Render(TaskPool* pool, Task* task, void* args) {
    OtosuGame_needle* self = task->data;

    if (self->visible != 0) {
        self->sprite.posX = (s16)((self->pos.x - self->origin.x) >> 12);
        self->sprite.posY = (s16)((self->pos.y + self->height - self->origin.y) >> 12);
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

s32 OtosuGame_needle_Destroy(TaskPool* pool, Task* task, void* args) {
    OtosuGame_needle* self = task->data;
    s32               i;

    Sprite_Release(&self->sprite);

    for (i = 0; i < 8; i++) {
        EasyTask_DeleteTask(pool, self->hahenIds[i]);
    }
    return 1;
}

s32 OtosuGame_needle_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = OtosuGame_needle_Init,
        .update     = OtosuGame_needle_Update,
        .render     = OtosuGame_needle_Render,
        .cleanup    = OtosuGame_needle_Destroy,
    };

    return stages.iter[stage](pool, task, args);
}

/** Creates the needle and its eight fragment tasks. */
s32 OtosuGame_needle_CreateTask(TaskPool* pool, s32 dataType, s32 pinId) {
    OtuPinSpriteArgs  args;
    s32               handle;
    OtosuGame_needle* self;
    s32               i;

    args.dataType = dataType;
    args.childId  = pinId;

    handle = EasyTask_CreateTask(pool, &Tsk_OtosuGame_needle, NULL, 0, NULL, &args);
    self   = EasyTask_GetTaskData(pool, handle);

    for (i = 0; i < 8; i++) {
        self->hahenIds[i] = OtosuGame_needlehahen_CreateTask(pool, dataType, pinId);
    }
    return handle;
}

/** The needle's tip while it is held, with a fixed scale of 32.0. */
s32 func_ov039_02091628(OtosuGame_needle* self, OtuPinRecord* out) {
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
void func_ov039_02091654(OtosuGame_needle* self, s32 chargeFrames, s32 holdFrames) {
    self->chargeFrames = chargeFrames;
    self->holdFrames   = holdFrames;
    self->state        = 1;
}

/** While held, cuts the hold short to what is left of its first 30 frames. */
void func_ov039_02091668(OtosuGame_needle* self) {
    if (self->state == 3) {
        self->timer = self->holdTimer;
    }
}

/** True while the needle is out. */
s32 func_ov039_0209167c(OtosuGame_needle* self) {
    return self->state != 0;
}

/** Throws the eight fragments from the needle's tip. */
void func_ov039_02091690(OtosuGame_needle* self) {
    s32 i;

    for (i = 0; i < 8; i++) {
        func_ov039_02097e9c(EasyTask_GetTaskData(self->pool, self->hahenIds[i]), &self->pos);
    }
}
