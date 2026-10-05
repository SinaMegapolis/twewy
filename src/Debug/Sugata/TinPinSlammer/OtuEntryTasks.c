#include "OtuFieldAccessShared.h"

/*
 * ov039 region 0x020950b8 - 0x020960bc: the slash, track and point tasks and
 * the head of the entry task. The shared types, externs and prototypes are in
 * OtuFieldAccessShared.h.
 */

/* ==================================================================== */
/* Tsk_OtosuGame_slash (its frame-info callback is in OtuGauge.c)       */
/* ==================================================================== */

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

/* ==================================================================== */
/* Tsk_OtosuGame_track                                                  */
/* ==================================================================== */

SpriteFrameInfo* func_ov039_0209549c(Sprite* sprite, s32 arg, s32 mode) {
    OtuTrackTask* owner = sprite->owner;

    Sprite_FrameInfoCallbackSorted(sprite, mode, func_ov039_02088400(4, owner->pos.y, 0));
}

void func_ov039_02095558(OtuTrackTask* self, Sprite* sprite, OtuTaskArgs3* args) {
    SpriteAnimation anim = data_ov039_02099d10;

    anim.owner    = self;
    anim.dataType = args->dataType;
    anim.posX     = (self->pos.x - self->origin.x) >> 12;
    anim.posY     = (self->pos.y - self->origin.y) >> 12;

    _Sprite_Load(sprite, &anim);
}

s32 func_ov039_020955f4(TaskPool* pool, Task* task, void* args) {
    OtuTrackTask* self = task->data;
    OtuTaskArgs3* a    = args;

    self->pool     = a->pool;
    self->pinId    = a->pinId;
    self->active   = 0;
    self->visible  = 0;
    self->origin.x = 0;
    self->origin.y = 0;
    self->pos.x    = 0;
    self->pos.y    = 0;

    func_ov039_02095558(self, &self->sprite, a);
    return 1;
}

/** Drifts the mark by `vel` until its animation ends. */
s32 func_ov039_02095638(TaskPool* pool, Task* task, void* args) {
    OtuTrackTask* self = task->data;

    if (self->active != 0) {
        if (SpriteMgr_IsAnimationFinished(&self->sprite)) {
            self->active  = 0;
            self->visible = 0;
        } else {
            OtuBadge* pin = EasyTask_GetTaskData(self->pool, self->pinId);

            func_ov039_0208e85c(pin, &self->origin);
            func_ov039_02098b8c(&self->pos, &self->vel, &self->pos);
            Sprite_Update(&self->sprite);
            self->visible = 1;
        }
    }
    return 1;
}

s32 func_ov039_020956a4(TaskPool* pool, Task* task, void* args) {
    OtuTrackTask* self = task->data;

    if (self->visible != 0) {
        self->sprite.posX = (self->pos.x - self->origin.x) >> 12;
        self->sprite.posY = (self->pos.y - self->origin.y) >> 12;
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

s32 func_ov039_020956f0(TaskPool* pool, Task* task, void* args) {
    Sprite_Release(&((OtuTrackTask*)task->data)->sprite);
    return 1;
}

s32 func_ov039_02095708(TaskPool* pool, Task* task, void* args, s32 stage) {
    const TaskStages stages = data_ov039_02099d00;
    return stages.iter[stage](pool, task, args);
}

s32 func_ov039_02095750(TaskPool* pool, s32 dataType, s32 pin) {
    OtuTaskArgs3 args;

    args.dataType = dataType;
    args.pool     = pool;
    args.pinId    = pin;

    return EasyTask_CreateTask(pool, &data_ov039_02099cf4, NULL, 0, NULL, &args);
}

/**
 * Drops a trail mark beside `at`: offset 12 units to one side of `angle` (the
 * side picked by `dir`), drifting outward at half speed, with an animation
 * chosen by the badge's speed `len`.
 *
 * The 64-bit `>> 12` is load-bearing: `/ 0x1000` calls `_ll_sdiv`.
 */
// Nonmatching: scratch-register choice in the index arithmetic only.
void func_ov039_02095788(OtuTrackTask* self, OtuPoint* at, s32 angle, s32 dir, s32 len) {
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

/* ==================================================================== */
/* Tsk_OtosuGame_point                                                  */
/* ==================================================================== */

SpriteFrameInfo* func_ov039_020958a8(Sprite* sprite, s32 arg, s32 mode) {
    OtuPointTask* owner = sprite->owner;

    Sprite_FrameInfoCallbackSorted(sprite, mode, func_ov039_02088400(5, owner->pos.y, 0));
}

/** Loads sprite `index`, whose animation comes from data_ov039_02099d3c. */
void func_ov039_02095964(OtuPointTask* self, Sprite* sprite, OtuPinSpriteArgs* args, s32 index) {
    SpriteAnimation anim = data_ov039_02099d60;

    anim.owner     = self;
    anim.dataType  = args->dataType;
    anim.posX      = (self->pos.x - self->origin.x) >> 12;
    anim.posY      = (self->pos.y - self->origin.y) >> 12;
    anim.animIndex = data_ov039_02099d3c[index];

    _Sprite_Load(sprite, &anim);
}

s32 func_ov039_02095a18(TaskPool* pool, Task* task, void* args) {
    OtuPointTask*     self = task->data;
    OtuPinSpriteArgs* a    = args;
    s32               i;

    self->pinId    = a->childId;
    self->origin.x = 0;
    self->origin.y = 0;
    self->pos.x    = 0;
    self->pos.y    = 0;
    self->active   = 0;
    self->visible  = 0;

    for (i = 0; i < 4; i++) {
        func_ov039_02095964(self, &self->sprite[i], a, i);
    }
    return 1;
}

/**
 * Bounces each live digit: once its delay is spent it falls under gravity
 * (0x800 per frame) and, on landing, rebounds at a quarter of its speed. The
 * whole popup hides once `timer` runs out.
 */
s32 func_ov039_02095a78(TaskPool* pool, Task* task, void* args) {
    OtuPointTask* self = task->data;
    s32           i;

    if (self->active != 0) {
        for (i = 0; i < 4; i++) {
            if (self->slot[i].live != 0) {
                if (self->slot[i].delay > 0) {
                    self->slot[i].delay = self->slot[i].delay - 1;
                } else {
                    self->slot[i].accum = self->slot[i].accum + self->slot[i].phase;
                    self->slot[i].phase = self->slot[i].phase + 0x800;

                    if (self->slot[i].accum > 0) {
                        self->slot[i].accum = 0;

                        if (self->slot[i].phase > 0) {
                            // A Q12 multiply by -0.25; `/ -4` compiles to shifts.
                            self->slot[i].phase = (s32)(((s64)self->slot[i].phase * -0x400 + 0x800) >> 12);
                        }
                    }
                    Sprite_Update(&self->sprite[i]);
                }
            }
        }

        self->visible = 1;
        self->timer   = self->timer - 1;

        if (self->timer <= 0) {
            self->active  = 0;
            self->visible = 0;
        }
    }
    return 1;
}

s32 func_ov039_02095b6c(TaskPool* pool, Task* task, void* args) {
    OtuPointTask* self = task->data;
    OtuBadge*     pin;
    s32           i;

    if (self->visible != 0) {
        pin = EasyTask_GetTaskData(pool, self->pinId);

        if (pin != NULL) {
            func_ov039_0208e85c(pin, &self->origin);
            func_ov039_0208e6e0(pin, &self->pos);

            for (i = 0; i < 4; i++) {
                if (self->slot[i].live != 0 && self->slot[i].delay <= 0) {
                    // The target computes y before x.
                    s32 py = self->pos.y - self->origin.y + self->slot[i].accum - 0x10000;
                    s32 px = self->pos.x - self->origin.x + self->slot[i].offsetX;

                    self->sprite[i].posX = px >> 12;
                    self->sprite[i].posY = py >> 12;
                    Sprite_RenderFrame(&self->sprite[i]);
                }
            }
        }
    }
    return 1;
}

s32 func_ov039_02095c2c(TaskPool* pool, Task* task, void* args) {
    OtuPointTask* self = task->data;
    s32           i;

    for (i = 0; i < 4; i++) {
        Sprite_Release(&self->sprite[i]);
    }
    return 1;
}

s32 func_ov039_02095c58(TaskPool* pool, Task* task, void* args, s32 stage) {
    const TaskStages stages = data_ov039_02099d50;
    return stages.iter[stage](pool, task, args);
}

s32 func_ov039_02095ca0(TaskPool* pool, s32 dataType, s32 pin) {
    OtuPinSpriteArgs args;

    args.dataType = dataType;
    args.childId  = pin;

    return EasyTask_CreateTask(pool, &data_ov039_02099d44, NULL, 0, NULL, &args);
}

/**
 * Pops up `count` for 60 frames: the digits drop in staggered by four frames,
 * sprite 2 shows the ones digit and sprite 3 the tens (hidden when zero).
 */
void func_ov039_02095cd4(OtuPointTask* self, s32 count) {
    s32 i;
    s16 first;
    s16 last;

    self->active = 1;
    self->timer  = 0x3C;

    for (i = 0; i < 4; i++) {
        self->slot[i].live    = 1;
        self->slot[i].delay   = (3 - i) * 4;
        self->slot[i].offsetX = (0xC - i * 8) << 12;
        self->slot[i].accum   = 0;
        self->slot[i].phase   = -0x3000;
    }

    self->count = count;

    // Both digits are narrowed before use; routing the first through `last`
    // is what the register allocation needs (found by the permuter).
    last  = (count % 10) + 1;
    first = (s16)last;
    Sprite_ChangeAnimation(&self->sprite[2], self->sprite[2].animData, first, self->sprite[2].cellTable);

    last = (s16)((self->count / 10) % 10 + 1);

    if ((s16)last <= 1) {
        self->slot[3].live = 0;
        return;
    }

    Sprite_ChangeAnimation(&self->sprite[3], self->sprite[3].animData, last, self->sprite[3].cellTable);
}

/** Hides the popup. */
void func_ov039_02095ddc(OtuPointTask* self) {
    self->active  = 0;
    self->visible = 0;
}

/* ==================================================================== */
/* Tsk_OtosuGame_entry (continued in OtuTaskStages.c)                   */
/* ==================================================================== */

SpriteFrameInfo* func_ov039_02095dec(Sprite* sprite, s32 arg, s32 mode) {
    Sprite_FrameInfoCallbackSorted(sprite, mode, func_ov039_02088400(7, 0, 0));
}

void func_ov039_02095ea4(OtuEntryTask* self, Sprite* sprite, OtuTaskArgs1* args) {
    SpriteAnimation anim = data_ov039_02099da8;

    anim.owner    = self;
    anim.dataType = args->dataType;

    _Sprite_Load(sprite, &anim);
}

s32 func_ov039_02095f18(TaskPool* pool, Task* task, void* args) {
    OtuEntryTask* self = task->data;
    OtuTaskArgs1* a    = args;

    self->state = 0;

    func_ov039_02095ea4(self, &self->sprite, a);
    func_ov039_02095ea4(self, &self->labelSprite, a);

    self->labelSprite.posY = 0x91;
    return 1;
}

s32 func_ov039_02095f58(TaskPool* pool, Task* task, void* args) {
    OtuEntryTask* self = task->data;

    switch (self->state) {
        case 0:
            self->visible = 0;
            break;

        case 1:
            func_ov039_02087bf8(&self->scaleAnim0, &self->affine0);
            Sprite_Update(&self->sprite);

            if (self->hasLabel != 0) {
                func_ov039_02087bf8(&self->scaleAnim1, &self->affine1);
                Sprite_Update(&self->labelSprite);
            }

            self->timer = self->timer - 1;

            if (self->timer > 0) {
                self->visible = 1;
            } else {
                self->state   = 0;
                self->visible = 0;
            }
            break;
    }
    return 1;
}

/**
 * Draws both sprites through affine groups sized by their scale keyframes;
 * the group index goes into bits 5..9 of the OAM attributes.
 */
// Nonmatching: 72.6%, argument scheduling of OamMgr_AllocAffineGroup: the
// target forms the manager address before spilling the fifth argument (the
// same gap Shop_item2.c carries for this expression).
s32 func_ov039_02095fe4(TaskPool* pool, Task* task, void* args) {
    OtuEntryTask* self = task->data;

    if (self->visible != 0) {
        self->sprite.unk_0A.unk_05 =
            (u16)OamMgr_AllocAffineGroup(&g_OamMgr[self->sprite.bits_0_1], 0, self->affine0.scaleX, self->affine0.scaleY, 0);
        Sprite_RenderFrame(&self->sprite);

        if (self->hasLabel != 0) {
            self->labelSprite.unk_0A.unk_05 = (u16)OamMgr_AllocAffineGroup(&g_OamMgr[self->labelSprite.bits_0_1], 0,
                                                                           self->affine1.scaleX, self->affine1.scaleY, 0);
            Sprite_RenderFrame(&self->labelSprite);
        }
    }
    return 1;
}
