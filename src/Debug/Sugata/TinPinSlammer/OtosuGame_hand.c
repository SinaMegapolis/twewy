/**
 * @file OtosuGame_hand.c
 * @brief The `Tsk_OtosuGame_hand` task.
 */

#include "OtuFieldAccessShared.h"

SpriteFrameInfo* func_ov039_020916c4(Sprite* sprite, s32 arg, s32 mode) {
    OtuHand* owner = sprite->owner;

    Sprite_FrameInfoCallbackAffineSorted(sprite, mode, &owner->affine, func_ov039_02088400(4, owner->pos.y, 0));
}

void func_ov039_02091788(OtuHand* self, Sprite* sprite, OtuPinSpriteArgs* args) {
    SpriteAnimation anim = data_ov039_020995c0;

    anim.owner    = self;
    anim.dataType = (u16)args->dataType;
    anim.posX     = self->pos.x >> 12;
    anim.posY     = self->pos.y >> 12;
    _Sprite_Load(sprite, &anim);
}

/** The stores are in the target's order, not by address. */
s32 func_ov039_02091814(TaskPool* pool, Task* task, OtuPinSpriteArgs* args) {
    OtuHand* self = task->data;

    self->pinId           = args->childId;
    self->origin.x        = 0;
    self->origin.y        = 0;
    self->pos.x           = 0;
    self->pos.y           = 0;
    self->affine.rotation = 0;
    self->affine.scaleX   = 0x2000;
    self->affine.scaleY   = 0x2000;
    self->affine.unk_0C   = 0;
    self->affine.unk_0E   = 0;
    self->visible         = 0;
    self->state           = 0;

    func_ov039_02091788(self, &self->sprite, args);
    return 1;
}

/**
 * The hand's states: 1 aims backwards along the pin's facing, 2 thrusts out
 * along it while stretching to full length over four frames, 3 waits out the
 * grab animation, 4 shrinks back. `== 1` keeps the target's compare on the
 * isPlaying bit.
 */
s32 func_ov039_02091868(TaskPool* pool, Task* task, void* args) {
    OtuHand* self = task->data;
    void*    pin;

    pin = EasyTask_GetTaskData(pool, self->pinId);

    if (pin != NULL) {
        switch (self->state) {
            case 0:
                self->visible = 0;
                break;

            case 1:
                self->visible = 1;
                func_ov039_0208e6cc(pin, &self->dir);
                func_ov039_02098bd4(-0x1000, &self->dir, &self->dir);
                self->affine.rotation = (u16)(FX_Atan2Idx(self->dir.y, self->dir.x) + 0xC000);
                self->affine.scaleY   = 0;
                self->state           = 2;
                self->timer           = 4;
                Sprite_SetAnimation(&self->sprite, self->sprite.animData, 1, self->sprite.cellTable);
                /* fall through */
            case 2:
                func_ov039_0208e85c(pin, &self->origin);
                func_ov039_0208e6e0(pin, &self->pos);
                func_ov039_02098c00(0x38000, &self->dir, &self->pos, &self->pos);
                if (self->timer > 0) {
                    self->affine.scaleY = self->affine.scaleY + (0x2000 - self->affine.scaleY) / self->timer;
                    self->timer         = self->timer - 1;
                }

                if (self->sprite.isPlaying == 1) {
                    self->state = 3;
                    Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, 2, self->sprite.cellTable);
                }
                break;

            case 3:
                func_ov039_0208e85c(pin, &self->origin);

                if (self->sprite.isPlaying == 1) {
                    self->state = 4;
                    self->timer = 4;
                    Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, 3, self->sprite.cellTable);
                }
                break;

            case 4:
                func_ov039_0208e85c(pin, &self->origin);

                if (self->timer > 0) {
                    self->affine.scaleY = self->affine.scaleY + -self->affine.scaleY / self->timer;
                    self->timer         = self->timer - 1;
                } else {
                    self->state   = 0;
                    self->visible = 0;
                }
                break;
        }
    } else {
        self->visible = 0;
    }

    if (self->visible != 0) {
        Sprite_Update(&self->sprite);
    }
    return 1;
}

s32 func_ov039_02091a5c(TaskPool* pool, Task* task, void* args) {
    OtuHand* self = task->data;

    if (self->visible != 0) {
        self->sprite.posX = (s16)((self->pos.x - self->origin.x) >> 12);
        self->sprite.posY = (s16)((self->pos.y - self->origin.y) >> 12);
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

s32 func_ov039_02091aa4(TaskPool* pool, Task* task, void* args) {
    Sprite_Release(&((OtuHand*)task->data)->sprite);
    return 1;
}

s32 func_ov039_02091ab8(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = data_ov039_020995b0;

    return stages.iter[stage](pool, task, args);
}

s32 func_ov039_02091b00(TaskPool* pool, s32 dataType, s32 pinId) {
    OtuPinSpriteArgs args;

    args.dataType = dataType;
    args.childId  = pinId;
    return EasyTask_CreateTask(pool, &data_ov039_020995a4, NULL, 0, NULL, &args);
}

/** Starts the grab. */
void func_ov039_02091b34(OtuHand* self) {
    self->state = 1;
}
