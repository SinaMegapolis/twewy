/**
 * @file OtosuGame_slash.c
 * @brief The `Tsk_OtosuGame_slash` task.
 */

#include "OtuFieldAccessShared.h"

SpriteFrameInfo* func_ov039_02094ff4(Sprite* sprite, s32 arg, s32 mode) {
    OtuSlashTask* owner = sprite->owner;

    Sprite_FrameInfoCallbackAffineSorted(sprite, mode, &owner->affine, func_ov039_02088400(6, owner->pos.y, 0));
}

void func_ov039_020950b8(OtuSlashTask* self, Sprite* sprite, OtuPinSpriteArgs* args) {
    SpriteAnimation anim = data_ov039_02099cc8;

    anim.owner    = self;
    anim.dataType = args->dataType;
    anim.posX     = self->pos.x >> 12;
    anim.posY     = self->pos.y >> 12;

    _Sprite_Load(sprite, &anim);
}

s32 func_ov039_02095144(TaskPool* pool, Task* task, void* args) {
    OtuSlashTask*     self = task->data;
    OtuPinSpriteArgs* a    = args;

    self->pinId           = a->childId;
    self->pos.x           = 0;
    self->pos.y           = 0;
    self->affine.rotation = 0;
    self->affine.scaleX   = 0x1000;
    self->affine.scaleY   = 0x1000;
    self->affine.unk_0C   = 0;
    self->affine.unk_0E   = 0;
    self->visible         = 0;
    self->timer           = 0;
    self->state           = 0;

    func_ov039_020950b8(self, &self->sprite, a);
    return 1;
}

/**
 * Aims the arrow from the pin along its pending shot: 0 waits for the pin to
 * start aiming, 1 follows the aim (and plays the arrow sound panned by x and
 * pitched by length once the frame finishes), 2 shrinks the arrow back over
 * `timer` frames after the pin stops. The cases fall through into each other.
 */
s32 func_ov039_02095194(TaskPool* pool, Task* task, void* args) {
    OtuSlashTask* self = task->data;
    OtuBadge*     pin;
    s32           len;
    s32           pan;

    pin = EasyTask_GetTaskData(pool, self->pinId);

    if (pin != NULL) {
        // Declaration order is load-bearing: stack slots are handed out in
        // reverse (vel at sp+0x10, off at sp+0x8, dir at sp+0x0).
        OtuPoint vel;
        OtuPoint off;
        OtuPoint dir;

        s32 phase = func_ov039_0208eed0(pin);

        func_ov039_0208e87c(pin, &self->origin);

        switch (self->state) {
            case 0:
                if (phase != 0) {
                    Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, 2, self->sprite.cellTable);
                    self->visible = 1;
                    self->state   = 1;
                } else {
                    self->visible = 0;
                    break;
                }
                // Falls through.

            case 1:
                if (phase != 0) {
                    func_ov039_0208ef14(pin, &vel, &off);

                    pan = off.x >> 12;

                    func_ov039_02098b8c(&vel, &self->origin, &vel);
                    func_ov039_02098b8c(&off, &self->origin, &off);
                    func_ov039_02098bb0(&off, &vel, &dir);
                    func_ov039_02098c00(0x800, &dir, &vel, &self->pos);

                    len                   = func_ov039_02098d10(&dir);
                    self->affine.scaleY   = len / 48;
                    self->affine.rotation = (u16)(FX_Atan2Idx(dir.y, dir.x) + 0x4000);

                    if (func_ov039_0208ef38(pin) != 0) {
                        Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, 3, self->sprite.cellTable);
                    }

                    if (SpriteMgr_IsFrameFinished(&self->sprite)) {
                        SndMgr_StartPlayingSE(SEIDX_SE_BAYBADGE_ARROW);

                        if (pan < 0) {
                            pan = 0;
                        } else if (pan > 0xFF) {
                            pan = 0xFF;
                        }

                        SndMgr_UpdateSEPan(SEIDX_SE_BAYBADGE_ARROW, pan);
                        SndMgr_SetSequenceSoundPitch(SEIDX_SE_BAYBADGE_ARROW, ((len >> 12) * 3 * 0x100) / 0x100);
                    }
                    break;
                }

                self->timer = 0x10;
                self->state = 2;
                // Falls through.

            case 2:
                if (phase == 1) {
                    self->state = 1;
                    break;
                }

                self->timer = self->timer - 1;

                if (self->timer <= 0) {
                    self->state = 0;
                } else {
                    self->affine.scaleY = self->affine.scaleY + _s32_div_f(0x200 - self->affine.scaleY, self->timer);
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

s32 func_ov039_020953c4(TaskPool* pool, Task* task, void* args) {
    OtuSlashTask* self = task->data;

    if (self->visible != 0) {
        self->sprite.posX = (self->pos.x - self->origin.x) >> 12;
        self->sprite.posY = (self->pos.y - self->origin.y) >> 12;
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

s32 func_ov039_0209540c(TaskPool* pool, Task* task, void* args) {
    Sprite_Release(&((OtuSlashTask*)task->data)->sprite);
    return 1;
}

s32 func_ov039_02095420(TaskPool* pool, Task* task, void* args, s32 stage) {
    const TaskStages stages = data_ov039_02099cb8;
    return stages.iter[stage](pool, task, args);
}

s32 func_ov039_02095468(TaskPool* pool, s32 dataType, s32 pin) {
    OtuPinSpriteArgs args;

    args.dataType = dataType;
    args.childId  = pin;

    return EasyTask_CreateTask(pool, &data_ov039_02099cac, NULL, 0, NULL, &args);
}
