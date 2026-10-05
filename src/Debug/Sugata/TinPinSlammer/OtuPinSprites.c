#include "OtuFieldAccessShared.h"

/*
 * ov039 region 0x0208f00c - 0x0209003c: the tail of the badge's accessors, the
 * four pin-effect sprite tasks (shadow, piyo, marker, meteo) and the start of
 * the hammer's cursor helpers. The shared types, externs and prototypes are in
 * OtuFieldAccessShared.h.
 */

/* ==================================================================== */
/* Badge accessors: 0208f00c - 0208f104.                                */
/* ==================================================================== */

/** True when the badge's tray slot holds a real pin (0x130 means empty). */
s32 func_ov039_0208f00c(OtuBadge* self) {
    return *self->pinID < 0x130;
}

/** Tail-calls func_ov039_0208a490 with a second argument of 10. */
void func_ov039_0208f024(OtuBadge* self) {
    func_ov039_0208a490(self, 0xA);
}

s32 func_ov039_0208f034(OtuBadge* task) {
    return (task)->score;
}

/** Parks the badge's AI. */
void func_ov039_0208f03c(OtuBadge* task) {
    (task)->curAI = 0x11;
}

/**
 * Hands the next of the badge's eight group children a point and a selector,
 * round-robin. The cursor is written back before it wraps, and is re-read from
 * the badge on both sides of the call rather than held in a register.
 */
void func_ov039_0208f048(OtuBadge* self, OtuPoint* at, s32 selector) {
    func_ov039_02097750(EasyTask_GetTaskData(self->pool, self->smokeIds[self->smokeCursor]), at, selector, 0x1000, 0x66, 6);

    self->smokeCursor = self->smokeCursor + 1;
    if (self->smokeCursor >= 8) {
        self->smokeCursor = 0;
    }
}

/** True when the badge's tray slot is empty. */
s32 func_ov039_0208f0b0(OtuBadge* self) {
    return *self->pinID == 0x130;
}

/** Resets the badge's label child, if it has one. */
void func_ov039_0208f0c8(OtuBadge* self) {
    if (self->hasLabel != 0) {
        func_ov039_02096270(EasyTask_GetTaskData(self->pool, self->entryId));
    }
}

/** Forgets `other` as this badge's last contact. */
void func_ov039_0208f0f0(OtuBadge* self, OtuBadge* other) {
    if (self->partner == other) {
        self->partner = NULL;
    }
}

/** Resets both of the badge's pair children. */
void func_ov039_0208f104(OtuBadge* self) {
    s32 i;

    for (i = 0; i < 2; i++) {
        func_ov039_02095ddc(EasyTask_GetTaskData(self->pool, self->pointIds[i]));
    }
}

/* ==================================================================== */
/* Tsk_OtosuGame_shadow                                                 */
/* ==================================================================== */

SpriteFrameInfo* func_ov039_0208f134(Sprite* sprite, s32 arg, s32 mode) {
    OtuShadow* owner = sprite->owner;

    Sprite_FrameInfoCallbackAffineSorted(sprite, mode, &owner->affine, func_ov039_02088400(0, owner->pos.y, 0));
}

/**
 * The only one of the four loaders that touches the OAM priority word, which is
 * what keeps the shadow behind everything else. The dataType edit has to stay
 * after both position stores.
 */
void func_ov039_0208f1f8(OtuShadow* self, Sprite* sprite, OtuPinSpriteArgs* args) {
    SpriteAnimation anim = data_ov039_02099328;

    anim.owner           = self;
    anim.dataType        = args->dataType;
    anim.posX            = F2I(self->pos.x);
    anim.posY            = F2I(self->pos.y);
    anim.unk_02.unk_02   = 1;
    data_0206a890.unk_0C = 0xA;

    _Sprite_Load(sprite, &anim);
}

s32 func_ov039_0208f2a4(TaskPool* pool, Task* task, OtuPinSpriteArgs* args) {
    OtuShadow* self = task->data;

    self->pinId           = args->childId;
    self->origin.x        = 0;
    self->origin.y        = 0;
    self->pos.x           = 0;
    self->pos.y           = 0;
    self->affine.rotation = 0;
    self->affine.scaleX   = 0x1000;
    self->affine.scaleY   = 0x1000;
    self->affine.unk_0C   = 0;
    self->affine.unk_0E   = 0;

    func_ov039_0208f1f8(self, &self->sprite, args);
    return 1;
}

/** Follows the pin and its scale, but only while func_ov039_0208e890 says exactly 1. */
s32 func_ov039_0208f2f0(TaskPool* pool, Task* task, void* args) {
    OtuShadow* self = task->data;
    void*      pin  = EasyTask_GetTaskData(pool, self->pinId);

    if (pin != NULL) {
        self->visible = func_ov039_0208e890(pin);

        if (self->visible == 1) {
            func_ov039_0208e85c(pin, &self->origin);
            func_ov039_0208e6e0(pin, &self->pos);
            self->affine.scaleY = func_ov039_0208e8c4(pin);
            self->affine.scaleX = self->affine.scaleY;
            Sprite_Update(&self->sprite);
        }
    } else {
        self->visible = 0;
    }
    return 1;
}

/** Draws the shadow three pixels down and right of the pin. */
s32 func_ov039_0208f360(TaskPool* pool, Task* task, void* args) {
    OtuShadow* self = task->data;

    if (self->visible != 0) {
        self->sprite.posX = F2I(self->pos.x - self->origin.x) + 3;
        self->sprite.posY = F2I(self->pos.y - self->origin.y) + 3;
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

s32 func_ov039_0208f3b0(TaskPool* pool, Task* task, void* args) {
    Sprite_Release(&((OtuShadow*)task->data)->sprite);
    return 1;
}

s32 func_ov039_0208f3c4(TaskPool* pool, Task* task, void* args, s32 stage) {
    const TaskStages stages = data_ov039_02099318;

    return stages.iter[stage](pool, task, args);
}

s32 func_ov039_0208f40c(TaskPool* pool, s32 dataType, s32 childId) {
    OtuPinSpriteArgs args;

    args.dataType = dataType;
    args.childId  = childId;
    return EasyTask_CreateTask(pool, &data_ov039_0209930c, NULL, 0, NULL, &args);
}

/* ==================================================================== */
/* Tsk_OtosuGame_piyo                                                   */
/* ==================================================================== */

SpriteFrameInfo* func_ov039_0208f440(Sprite* sprite, s32 arg, s32 mode) {
    OtuPiyo* owner = sprite->owner;

    Sprite_FrameInfoCallbackSorted(sprite, mode, func_ov039_02088400(3, owner->pos.y, 0));
}

/** The dataType edit has to stay after both position stores. */
void func_ov039_0208f4fc(OtuPiyo* self, Sprite* sprite, OtuPinSpriteArgs* args) {
    SpriteAnimation anim = data_ov039_02099370;

    anim.owner    = self;
    anim.dataType = args->dataType;
    anim.posX     = F2I(self->pos.x - self->origin.x);
    anim.posY     = F2I(self->pos.y - self->origin.y);
    anim.bits_7_9 = 5;

    _Sprite_Load(sprite, &anim);
}

s32 func_ov039_0208f5a8(TaskPool* pool, Task* task, OtuPinSpriteArgs* args) {
    OtuPiyo* self = task->data;

    self->visible  = 0;
    self->pinId    = args->childId;
    self->origin.x = 0;
    self->origin.y = 0;
    self->pos.x    = 0;
    self->pos.y    = 0;

    func_ov039_0208f4fc(self, &self->sprite, args);
    return 1;
}

/**
 * Picks one of five animations from the pin's remaining stun as a percentage of
 * `lifeMax` (20/40/60/80 are the band edges), and nudges the pin every 30 frames.
 */
s32 func_ov039_0208f5e0(TaskPool* pool, Task* task, void* args) {
    OtuPiyo* self = task->data;
    void*    pin  = EasyTask_GetTaskData(pool, self->pinId);
    s16      frame;
    s32      band;
    s32      stun;

    if (pin != NULL) {
        stun = func_ov039_0208ee98(pin);

        if (stun > 0) {
            func_ov039_0208e85c(pin, &self->origin);
            func_ov039_0208e6e0(pin, &self->pos);

            band = stun * 100 / self->stunMax;

            if (band <= 0x14) {
                frame = 8;
            } else if (band <= 0x28) {
                frame = 7;
            } else if (band <= 0x3C) {
                frame = 6;
            } else if (band <= 0x50) {
                frame = 5;
            } else {
                frame = 4;
            }

            Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, frame, self->sprite.cellTable);
            Sprite_Update(&self->sprite);

            self->timer = self->timer - 1;
            if (self->timer <= 0) {
                func_ov039_02087d04(0x330, &self->pos, &self->origin);
                self->timer = 0x1E;
            }

            self->visible = 1;
        } else {
            self->visible = 0;
        }
    } else {
        self->visible = 0;
    }
    return 1;
}

s32 func_ov039_0208f6cc(TaskPool* pool, Task* task, void* args) {
    OtuPiyo* self = task->data;

    if (self->visible != 0) {
        self->sprite.posX = F2I(self->pos.x - self->origin.x);
        self->sprite.posY = F2I(self->pos.y - self->origin.y);
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

s32 func_ov039_0208f714(TaskPool* pool, Task* task, void* args) {
    Sprite_Release(&((OtuPiyo*)task->data)->sprite);
    return 1;
}

s32 func_ov039_0208f728(TaskPool* pool, Task* task, void* args, s32 stage) {
    const TaskStages stages = data_ov039_02099360;

    return stages.iter[stage](pool, task, args);
}

s32 func_ov039_0208f770(TaskPool* pool, s32 dataType, s32 childId) {
    OtuPinSpriteArgs args;

    args.dataType = dataType;
    args.childId  = childId;
    return EasyTask_CreateTask(pool, &data_ov039_02099354, NULL, 0, NULL, &args);
}

/** Sets the stun the piyo bands against and forces a nudge on the next update. */
void func_ov039_0208f7a4(OtuPiyo* self, s32 stunMax) {
    self->stunMax = stunMax;
    self->timer   = 1;
}

/* ==================================================================== */
/* Tsk_OtosuGame_marker                                                 */
/* ==================================================================== */

/** Sorted one row below the pin, without the packer call. */
SpriteFrameInfo* func_ov039_0208f7b4(Sprite* sprite, s32 arg, s32 mode) {
    OtuMarker* owner = sprite->owner;

    Sprite_FrameInfoCallbackSorted(sprite, mode, (((owner->pos.y >> 12) & 0x7FF) + 1) << 12);
}

void func_ov039_0208f874(OtuMarker* self, Sprite* sprite, OtuPinSpriteArgs* args) {
    SpriteAnimation anim = data_ov039_020993b8;

    anim.owner    = self;
    anim.dataType = args->dataType;
    anim.posX     = F2I(self->pos.x);
    anim.posY     = F2I(self->pos.y);

    _Sprite_Load(sprite, &anim);
}

s32 func_ov039_0208f900(TaskPool* pool, Task* task, OtuPinSpriteArgs* args) {
    OtuMarker* self = task->data;

    self->pinId    = args->childId;
    self->origin.x = 0;
    self->origin.y = 0;
    self->pos.x    = 0;
    self->pos.y    = 0;
    self->frame    = 0;

    func_ov039_0208f874(self, &self->sprite, args);
    return 1;
}

/** Shows whichever animation func_ov039_0208e950 picks for the pin. */
s32 func_ov039_0208f938(TaskPool* pool, Task* task, void* args) {
    OtuMarker* self = task->data;
    void*      pin  = EasyTask_GetTaskData(pool, self->pinId);
    s32        prev;
    s32        frame;

    if (pin != NULL) {
        prev        = self->frame;
        frame       = func_ov039_0208e950(pin);
        self->frame = frame;

        if (frame > 0) {
            if (frame != prev) {
                Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, (s16)frame, self->sprite.cellTable);
            }
            func_ov039_0208e85c(pin, &self->origin);
            func_ov039_0208e6e0(pin, &self->pos);
            Sprite_Update(&self->sprite);
        }
    } else {
        self->frame = 0;
    }
    return 1;
}

s32 func_ov039_0208f9b8(TaskPool* pool, Task* task, void* args) {
    OtuMarker* self = task->data;

    if (self->frame != 0) {
        self->sprite.posX = F2I(self->pos.x - self->origin.x);
        self->sprite.posY = F2I(self->pos.y - self->origin.y);
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

s32 func_ov039_0208fa00(TaskPool* pool, Task* task, void* args) {
    Sprite_Release(&((OtuMarker*)task->data)->sprite);
    return 1;
}

s32 func_ov039_0208fa14(TaskPool* pool, Task* task, void* args, s32 stage) {
    const TaskStages stages = data_ov039_020993a8;

    return stages.iter[stage](pool, task, args);
}

s32 func_ov039_0208fa5c(TaskPool* pool, s32 dataType, s32 childId) {
    OtuPinSpriteArgs args;

    args.dataType = dataType;
    args.childId  = childId;
    return EasyTask_CreateTask(pool, &data_ov039_0209939c, NULL, 0, NULL, &args);
}

/* ==================================================================== */
/* Tsk_OtosuGame_meteo                                                  */
/* ==================================================================== */

SpriteFrameInfo* func_ov039_0208fa90(Sprite* sprite, s32 arg, s32 mode) {
    OtuMeteo* owner = sprite->owner;

    Sprite_FrameInfoCallbackAffineSorted(sprite, mode, &owner->affine, func_ov039_02088400(4, owner->pos.y, owner->height));
}

void func_ov039_0208fb54(OtuMeteo* self, Sprite* sprite, OtuPinSpriteArgs* args) {
    SpriteAnimation anim = data_ov039_02099400;

    anim.owner    = self;
    anim.dataType = args->dataType;
    anim.posX     = F2I(self->pos.x);
    anim.posY     = F2I(self->pos.y);

    _Sprite_Load(sprite, &anim);
}

s32 func_ov039_0208fbe0(TaskPool* pool, Task* task, OtuPinSpriteArgs* args) {
    OtuMeteo* self = task->data;

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

    func_ov039_0208fb54(self, &self->sprite, args);
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
s32 func_ov039_0208fc38(TaskPool* pool, Task* task, void* args) {
    OtuMeteo* self = task->data;
    void*     pin  = EasyTask_GetTaskData(pool, self->pinId);
    s32       i;

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
s32 func_ov039_0208fd8c(TaskPool* pool, Task* task, void* args) {
    OtuMeteo* self = task->data;

    if (self->alive != 0) {
        self->sprite.posX = F2I(self->pos.x - self->origin.x);
        self->sprite.posY = F2I(self->pos.y - self->origin.y + self->height);
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

s32 func_ov039_0208fddc(TaskPool* pool, Task* task, void* args) {
    OtuMeteo* self = task->data;
    s32       i;

    Sprite_Release(&self->sprite);

    for (i = 0; i < 8; i++) {
        EasyTask_DeleteTask(pool, self->hahenIds[i]);
    }
    return 1;
}

s32 func_ov039_0208fe18(TaskPool* pool, Task* task, void* args, s32 stage) {
    const TaskStages stages = data_ov039_020993f0;

    return stages.iter[stage](pool, task, args);
}

/** Creates the meteo and its eight fragment tasks. */
s32 func_ov039_0208fe60(TaskPool* pool, s32 dataType, s32 childId) {
    OtuPinSpriteArgs args;
    // `id` before `self`: the target keeps the id in r7 and the pool in r6.
    s32       id;
    OtuMeteo* self;
    s32       i;

    args.dataType = dataType;
    args.childId  = childId;

    id   = EasyTask_CreateTask(pool, &data_ov039_020993e4, NULL, 0, NULL, &args);
    self = EasyTask_GetTaskData(pool, id);

    for (i = 0; i < 8; i++) {
        self->hahenIds[i] = func_ov039_0209720c(pool, dataType, childId);
    }
    return id;
}

/** The meteo's position, with a fixed scale of 29.0. */
// Nonmatching: 98%, only the order of the first two stores.
void func_ov039_0208fee0(OtuMeteo* self, OtuPinRecord* out) {
    s32 x = self->pos.x;
    s32 y = self->pos.y;

    out->scale = 0x1D000;
    out->x     = x;
    out->y     = y;
}

/** State 1: hold animation 1 at full scale for `frames` frames. */
void func_ov039_0208fefc(OtuMeteo* self, s32 frames) {
    self->timer         = frames;
    self->affine.scaleX = 0x1000;
    self->affine.scaleY = 0x1000;

    Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, 1, self->sprite.cellTable);
    self->state = 1;
}

/** State 2: squash to a small scaleY and ease back over four frames. */
void func_ov039_0208ff30(OtuMeteo* self) {
    self->timer         = 4;
    self->affine.scaleX = 0x1000;
    self->affine.scaleY = 0x29;

    Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, 4, self->sprite.cellTable);
    self->state = 2;
}

/** State 3: the 30-frame wind-up, animation 5 plus its scale keyframes. */
void func_ov039_0208ff68(OtuMeteo* self) {
    self->timer = 0x1E;

    Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, 5, self->sprite.cellTable);
    func_ov039_02087ba0(&self->scaleAnim, data_ov039_0209a54c, 6, &self->affine);
    self->state = 3;
}

/* ==================================================================== */
/* OtuCursor: a cursor over a table of s32 keyframes.                   */
/* ==================================================================== */

/** Starts the cursor at entry 0, holding for that entry's duration plus one. */
void func_ov039_0208ffac(OtuCursor* c, OtuFrame4* table, s16 count) {

    c->table      = table;
    c->index      = 0;
    c->count      = count;
    c->framesLeft = c->table[c->index].duration + 1;
}

/** Advances the cursor one frame; returns 0 once it has run off the end. */
s32 func_ov039_0208ffd8(OtuCursor* c) {
    s16 index;

    if (c->index >= c->count) {
        return 0;
    }

    c->framesLeft = c->framesLeft - 1;

    if (c->framesLeft <= 0) {
        c->index = c->index + 1;
        index    = c->index;

        if (index >= c->count) {
            return 0;
        }

        c->framesLeft = c->table[index].duration;
    }
    return 1;
}
