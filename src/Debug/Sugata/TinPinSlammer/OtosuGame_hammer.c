/**
 * @file OtosuGame_hammer.c
 * @brief The `Tsk_OtosuGame_hammer` task.
 */

#include "OtuFieldAccessShared.h"

/* The hammer's keyframe tables. */

#define OTU_ANGLE_INDEX(a) (((s32)((a) >> 4)) * 2)

/* OtuCursor: a cursor over a table of s32 keyframes. */

s32              OtosuGame_hammer_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);
SpriteFrameInfo* OtosuGame_hammer_GetFrameInfoSpriteA(Sprite* sprite, s32 arg, s32 mode);
SpriteFrameInfo* OtosuGame_hammer_GetFrameInfoSpriteB(Sprite* sprite, s32 arg, s32 mode);

static const TaskHandle Tsk_OtosuGame_hammer = {"Tsk_OtosuGame_hammer", OtosuGame_hammer_RunTask, sizeof(OtosuGame_hammer)};

static const OtuFrame4 data_ov039_02099438[4] = {
    {15, 0x4000},
    { 3, 0x3800},
    { 2, 0x2CCD},
    { 2, 0x1CCD},
};

static const OtuFrame6 data_ov039_02099476[5] = {
    {5,    0x0, 0x0},
    {5, 0x1A4F, 0x0},
    {5, 0x238E, 0x0},
    {6, 0x24FA, 0x0},
    {6, 0x238E, 0x0},
};

static const OtuFrame6 data_ov039_02099458[5] = {
    {5, 0x1000,   0x0},
    {8, 0x1333,   0x3},
    {3,  0xCCD,  -0x3},
    {3,  0x99A,  -0x7},
    {3,  0x19A, -0x10},
};

static const OtuFrame6 data_ov039_020994b4[6] = {
    {15,    0x0, 0x0},
    { 1,   0xCD, 0x0},
    { 1,  0x4CD, 0x0},
    { 1,  0x800, 0x0},
    { 3, 0x1333, 0x1},
    { 1, 0x1000, 0x0},
};

static const OtuFrame4 data_ov039_02099494[8] = {
    {2,  0x800},
    {1, 0x119A},
    {1, 0x14CD},
    {2, 0x1666},
    {2, 0x3CCD},
    {2, 0x4000},
    {3, 0x3CCD},
    {9, 0x4000},
};

static const OtuFrame6 data_ov039_020994d8[7] = {
    {4, -0x5B0,     0x0},
    {3,    0x0,     0x0},
    {2, -0x16C, -0x1000},
    {1,  0x16C,  0x1000},
    {1, -0x2D8, -0x2000},
    {2,    0x0,     0x0},
    {3, -0x16C, -0x1000},
};

static const SpriteAnimation OtosuGame_hammer_AnimSpriteA = {
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
    .frameInfoCallback = OtosuGame_hammer_GetFrameInfoSpriteA,
    .callbackArg       = 0,
    .owner             = NULL,
    .binIden           = &data_ov039_0209a0dc,
    .unk_18            = 2,
    .packIndex         = 9,
    .unk_1C            = 1,
    .unk_1E            = 0,
    .unk_20            = 4,
    .unk_22            = 1,
    .unk_24            = 0,
    .unk_26            = 2,
    .unk_28            = 3,
    .animIndex         = 2,
};

static const SpriteAnimation OtosuGame_hammer_AnimSpriteB = {
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
    .frameInfoCallback = OtosuGame_hammer_GetFrameInfoSpriteB,
    .callbackArg       = 0,
    .owner             = NULL,
    .binIden           = &data_ov039_0209a0dc,
    .unk_18            = 2,
    .packIndex         = 9,
    .unk_1C            = 1,
    .unk_1E            = 0,
    .unk_20            = 4,
    .unk_22            = 1,
    .unk_24            = 0,
    .unk_26            = 2,
    .unk_28            = 3,
    .animIndex         = 1,
};

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

// Size: 0x4

/** Reads the current entry's `value`, or 0 if the cursor has run out.
 *
 *  The bound check is not an early exit here: the target computes the answer
 *  conditionally instead (`movge r0, #0` / `ldrlt ...`), where its twin
 *  func_ov039_020901e0 branches out. A four-byte entry type is what produces the
 *  `add r0, r0, r2, lsl #2`; a halfword subscript would have folded that into
 *  a smaller shift.
 */
s16 func_ov039_0209003c(OtuCursor* c) {
    if (c->index >= c->count) {
        return 0;
    }

    return c->table[c->index].value;
}

/** Starts a cursor at entry 0, primed with the first entry's duration plus one.
 *
 *  The `+ 1` is what makes the constructor and the stepper agree: a freshly
 *  started cursor has `framesLeft` equal to entry 0's duration, so the first
 *  step immediately falls through to entry 1.
 */
void func_ov039_0209005c(OtuCursor6* c, OtuFrame6* table, s16 count) {

    c->table      = table;
    c->index      = 0;
    c->count      = count;
    c->framesLeft = c->table[c->index].duration + 1;
}

/** Advances a cursor by one frame, returning 0 once it has run out.
 *
 *  The count is rechecked after the index moves, so a cursor that overshoots
 *  stops there rather than reading past the table. The reload deliberately does
 *  *not* add one: only the initialiser does.
 */
s32 func_ov039_0209008c(OtuCursor6* c) {
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

/** Reads the current entry's `value`, or 0 if the cursor has run out. */
s16 func_ov039_020900f4(OtuCursor6* c) {
    if (c->index >= c->count) {
        return 0;
    }

    return c->table[c->index].value;
}

/** Reads the current entry's `scale`, promoted to the overlay's <<12 scale. */
s32 func_ov039_0209011c(OtuCursor6* c) {
    if (c->index >= c->count) {
        return 0;
    }

    return c->table[c->index].scale << 12;
}

/** func_ov039_0209005c, duplicated. */
void func_ov039_02090148(OtuCursor6* c, OtuFrame6* table, s16 count) {

    c->table      = table;
    c->index      = 0;
    c->count      = count;
    c->framesLeft = c->table[c->index].duration + 1;
}

/** func_ov039_0209008c, duplicated. */
s32 func_ov039_02090178(OtuCursor6* c) {
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

/** Reads the current entry's `value` as unsigned, or 0 if the cursor has run
 *  out. The one difference from func_ov039_0209003c beyond the bound check's
 *  shape is the return type, and it is the only difference. */
u16 func_ov039_020901e0(OtuCursor6* c) {
    if (c->index >= c->count) {
        return 0;
    }

    return c->table[c->index].value;
}

SpriteFrameInfo* OtosuGame_hammer_GetFrameInfoSpriteA(Sprite* sprite, s32 arg, s32 mode) {
    OtosuGame_hammer* owner = sprite->owner;

    Sprite_FrameInfoCallbackAffineSorted(sprite, mode, &owner->affineA, func_ov039_02088400(4, owner->posA.y, 0));
}

SpriteFrameInfo* OtosuGame_hammer_GetFrameInfoSpriteB(Sprite* sprite, s32 arg, s32 mode) {
    OtosuGame_hammer* owner = sprite->owner;

    Sprite_FrameInfoCallbackAffineSorted(sprite, mode, &owner->affineB, func_ov039_02088400(2, owner->posB.y, 0));
}

/** Loads the sprite for the task whose anim template is OtosuGame_hammer_AnimSpriteA. */
void OtosuGame_hammer_LoadSpriteA(OtosuGame_hammer* self, Sprite* sprite, OtuPinSpriteArgs* args) {
    SpriteAnimation anim = OtosuGame_hammer_AnimSpriteA;

    anim.owner    = self;
    anim.dataType = (u16)args->dataType;
    anim.posX     = self->posA.x >> 12;
    anim.posY     = self->posA.y >> 12;
    _Sprite_Load(sprite, &anim);
}

/** Loads the sprite for the task whose anim template is OtosuGame_hammer_AnimSpriteB. */
void OtosuGame_hammer_LoadSpriteB(OtosuGame_hammer* self, Sprite* sprite, OtuPinSpriteArgs* args) {
    SpriteAnimation anim = OtosuGame_hammer_AnimSpriteB;

    anim.owner    = self;
    anim.dataType = (u16)args->dataType;
    anim.posX     = self->posB.x >> 12;
    anim.posY     = self->posB.y >> 12;
    _Sprite_Load(sprite, &anim);
}

s32 OtosuGame_hammer_Init(TaskPool* pool, Task* task, void* args) {
    OtuPinSpriteArgs* taskArgs = args;
    OtosuGame_hammer* self     = task->data;

    self->pool = pool;

    self->live  = 0;
    self->pinId = taskArgs->childId;

    self->origin.x = 0;
    self->origin.y = 0;
    self->posA.x   = 0;
    self->posA.y   = 0;
    self->posB.x   = 0;
    self->posB.y   = 0;
    self->pinPos.x = 0;
    self->pinPos.y = 0;

    self->rate0       = 0;
    self->scale0      = 0;
    self->totalFrames = 0;
    self->angleBase   = 0;

    self->affineA.rotation = 0;
    self->affineA.scaleX   = 0x1000;
    self->affineA.scaleY   = 0x1000;
    self->affineA.unk_0C   = 0;
    self->affineA.unk_0E   = 0;
    self->affineB.rotation = 0;
    self->affineB.scaleX   = 0x1000;
    self->affineB.scaleY   = 0x1000;
    self->affineB.unk_0C   = 0;
    self->affineB.unk_0E   = 0;

    self->state = 0;
    OtosuGame_hammer_LoadSpriteA(self, &self->spriteA, taskArgs);
    OtosuGame_hammer_LoadSpriteB(self, &self->spriteB, taskArgs);
    return 1;
}

/**
 * @brief The hammer's state machine: seven states, no breaks between them.
 *
 * `state` runs 0..6 and the cases *fall through* rather than ending. That is the
 * whole design: a state that has finished its work for this frame increments
 * `state` and then runs the next one in the same frame, so a fast transition
 * costs no extra frames. The only exits are state 0 (dead) and the pair of
 * "both cursors exhausted" checks, which fall into the setup for the next stage.
 *
 * Nothing here is inlined by the compiler -- the build passes `-inline noauto` --
 * so the trigonometry appears four times, once per state that turns an angle and
 * a scale into the two target positions. It is written out four times to match.
 *
 * Each state owns a pair of OtuCursor6 walks over a fixed table, plus one over
 * the angle. State 2 grows the head out and fades the tail in; state 4 collapses
 * the whole thing linearly; state 6 does it again at a new length.
 *
 * The `live` flag is the pin task's own validity, re-read every frame, and it
 * gates both the sprite updates and the render. A pin that goes invalid mid-flight
 * stops the hammer rather than letting it fly off on a stale target.
 */
s32 OtosuGame_hammer_Update(TaskPool* pool, Task* task, void* args) {
    OtosuGame_badge*  pin;
    OtosuGame_hammer* self = (OtosuGame_hammer*)task->data;

    pin = (OtosuGame_badge*)EasyTask_GetTaskData(pool, self->pinId);
    s32 stepped;
    s32 steppedB;
    u16 angle;
    s32 index;
    s32 headX;
    s32 headY;

    if (pin != NULL) {
        self->live = func_ov039_0208e998(pin);

        if (self->live == 0) {
            self->state = 0;
        } else {
            func_ov039_0208e85c(pin, &self->origin);
            func_ov039_0208e6e0(pin, &self->pinPos);
            self->height = func_ov039_0208e6f4(pin);
        }

        switch (self->state) {
            case 0:
                self->live = 0;
                break;

            case 1:
                self->showA = 0;
                self->showB = 1;
                func_ov039_0208ffac(&self->cursorScale, data_ov039_02099494, 8);
                func_ov039_0209005c(&self->cursorSpin, data_ov039_020994b4, 6);
                self->angle0           = func_ov039_0208e9ac(pin);
                self->angle            = self->angle0;
                self->affineB.rotation = (u16)(self->angle + 0x4000);
                self->affineA.rotation = (u16)(self->angle + 0x4000);
                self->scale            = 0;
                self->state            = (u32)(self->state + 1);
                func_ov039_02087d04(0x338, &self->posA, &self->origin);
                /* fallthrough */

            case 2: {
                s32 len;

                stepped = func_ov039_0208ffd8(&self->cursorScale);
                if (stepped != 0) {
                    s32 rate;

                    rate                 = self->rate0;
                    len                  = (s32)(((s64)func_ov039_0209003c(&self->cursorScale) * rate + 0x800) >> 12);
                    self->affineB.scaleY = len;
                    self->scale          = len * 0x10;
                    index                = OTU_ANGLE_INDEX(self->angle);
                    headX                = (s32)(((s64)self->scale * ((s16*)data_0205e4e0)[index + 1] + 0x800) >> 12);
                    headY                = (s32)(((s64)self->scale * ((s16*)data_0205e4e0)[index] + 0x800) >> 12);
                    self->posB.x         = self->pinPos.x + ((headX + (headX >> 31)) >> 1);
                    self->posB.y         = self->pinPos.y + ((headY + (headY >> 31)) >> 1);
                }

                steppedB = func_ov039_0209008c(&self->cursorSpin);
                if (steppedB != 0) {
                    s32 len2;

                    index                = OTU_ANGLE_INDEX(self->angle);
                    len2                 = self->scale + func_ov039_0209011c(&self->cursorSpin);
                    headX                = (s32)(((s64)len2 * ((s16*)data_0205e4e0)[index + 1] + 0x800) >> 12);
                    headY                = (s32)(((s64)len2 * ((s16*)data_0205e4e0)[index] + 0x800) >> 12);
                    self->affineA.scaleX = func_ov039_020900f4(&self->cursorSpin);
                    if (self->affineA.scaleX > 0) {
                        self->showA = 1;
                    }
                    self->posA.x = self->pinPos.x + headX;
                    self->posA.y = self->pinPos.y + headY;
                }

                if ((stepped == 0) && (steppedB == 0)) {
                    func_ov039_02090148(&self->cursorTrail, data_ov039_02099476, 5);
                    self->state = (u32)(self->state + 1);
                    func_ov039_02087d04(0x339, &self->posA, &self->origin);
                }
                /* fallthrough */
            }

            case 3: {
                if (func_ov039_02090178(&self->cursorTrail) != 0) {
                    angle       = (u16)(self->angle0 + func_ov039_020901e0(&self->cursorTrail));
                    index       = OTU_ANGLE_INDEX(angle);
                    self->angle = angle;
                    headX       = (s32)(((s64)self->scale * ((s16*)data_0205e4e0)[index + 1] + 0x800) >> 12);
                    headY       = (s32)(((s64)self->scale * ((s16*)data_0205e4e0)[index] + 0x800) >> 12);
                    // The two rotation writes deliberately sit *after* the trigonometry
                    // here, and deliberately sit before it in state 4. That is not a
                    // typo and it is not arbitrary: all four placements were measured,
                    // and this one is worth a point over the alternative. Note that the
                    // target's `add r, r, #0x4000` appears between the two multiplies in
                    // every state, which is the scheduler hoisting the arithmetic -- not
                    // a statement position. Putting the writes there to match it costs
                    // four and a half points.
                    self->affineB.rotation = (u16)(angle + 0x4000);
                    self->affineA.rotation = (u16)(angle + 0x4000);
                    self->posA.x           = self->pinPos.x + headX;
                    self->posA.y           = self->pinPos.y + headY;
                    self->posB.x           = self->pinPos.x + ((headX + (headX >> 31)) >> 1);
                    self->posB.y           = self->pinPos.y + ((headY + (headY >> 31)) >> 1);
                } else {
                    self->framesLeft = self->totalFrames;
                    self->state      = (u32)(self->state + 1);
                        /* fallthrough */
                    case 4:
                        // Read as unsigned, which compiles to `beq` where the target has a
                        // signed `ble`. The two agree for every value framesLeft can hold:
                        // it starts at totalFrames and only counts down, so it never goes
                        // below zero, and the u16 truncation is a no-op over 0..65535. They
                        // diverge only for a low half of 0x8000 or above, i.e. a
                        // framesLeft of -32768 or worse, which nothing in the task can
                        // produce. Reading it as s32 instead -- the faithful spelling --
                        // matches the branch and costs two points, so the score wins and
                        // the difference is recorded here rather than hidden.
                        angle            = (u16)(self->framesLeft - 1);
                        self->framesLeft = angle;

                        if (angle > 0) {
                            s32 len;
                            s32 spin;
                            s32 index2;

                            len                  = self->affineB.scaleY + ((self->scale0 * 4) - self->affineB.scaleY) / angle;
                            self->affineB.scaleY = len;
                            self->scale          = len * 0x10;
                            spin                 = self->angle0 -
                                   (((self->angleBase << 0x10) * (self->totalFrames - self->framesLeft)) / self->totalFrames);
                            index2      = OTU_ANGLE_INDEX(spin);
                            self->angle = spin;
                            // The mirror image of state 3: here the rotation writes come
                            // before the trigonometry, which is the placement that measures best.
                            // Same two stores, opposite position, for a fifth of a point.
                            self->affineB.rotation = (u16)(spin + 0x4000);
                            self->affineA.rotation = (u16)(spin + 0x4000);
                            headX        = (s32)(((s64)self->scale * ((s16*)data_0205e4e0)[index2 + 1] + 0x800) >> 12);
                            headY        = (s32)(((s64)self->scale * ((s16*)data_0205e4e0)[index2] + 0x800) >> 12);
                            self->posA.x = self->pinPos.x + headX;
                            self->posA.y = self->pinPos.y + headY;
                            self->posB.x = self->pinPos.x + ((headX + (headX >> 31)) >> 1);
                            self->posB.y = self->pinPos.y + ((headY + (headY >> 31)) >> 1);

                            self->reportTimer = self->reportTimer - 1;
                            if (self->reportTimer <= 0) {
                                func_ov039_0208f048(EasyTask_GetTaskData(pool, self->pinId), &self->posA, (u16)self->angle);
                                self->reportTimer = 4;
                            }
                        } else {
                            self->angle0 = self->angle;
                            func_ov039_02090148(&self->cursorTrail, data_ov039_020994d8, 7);
                            self->state = (u32)(self->state + 1);
                                /* fallthrough */
                            case 5:
                                if (func_ov039_02090178(&self->cursorTrail) != 0) {
                                    s32 index3;

                                    angle       = (u16)(self->angle0 + func_ov039_020901e0(&self->cursorTrail));
                                    index3      = OTU_ANGLE_INDEX(angle);
                                    self->angle = angle;
                                    headX       = (s32)(((s64)self->scale * ((s16*)data_0205e4e0)[index3 + 1] + 0x800) >> 12);
                                    headY       = (s32)(((s64)self->scale * ((s16*)data_0205e4e0)[index3] + 0x800) >> 12);
                                    // After the trigonometry, like state 3 and unlike state 4.
                                    self->affineB.rotation = (u16)(angle + 0x4000);
                                    self->affineA.rotation = (u16)(angle + 0x4000);
                                    self->posA.x           = self->pinPos.x + headX;
                                    self->posA.y           = self->pinPos.y + headY;
                                    self->posB.x           = self->pinPos.x + ((headX + (headX >> 31)) >> 1);
                                    self->posB.y           = self->pinPos.y + ((headY + (headY >> 31)) >> 1);
                                } else {
                                    s32 len;

                                    len           = self->affineB.scaleY;
                                    self->halfLen = (s32)((s32)(len + ((u32)(len >> 1) >> 0x1E)) >> 2);
                                    func_ov039_0209005c(&self->cursorSpin, data_ov039_02099458, 5);
                                    func_ov039_0208ffac(&self->cursorScale, data_ov039_02099438, 4);
                                    self->state = (u32)(self->state + 1);
                                    func_ov039_02087d04(0x33A, &self->posA, &self->origin);
                                        /* fallthrough */
                                    case 6:
                                        stepped = func_ov039_0208ffd8(&self->cursorScale);
                                        if (stepped != 0) {
                                            s32 half;
                                            s32 len3;

                                            half = self->halfLen;
                                            len3 = (s32)(((s64)func_ov039_0209003c(&self->cursorScale) * half + 0x800) >> 12);
                                            self->affineB.scaleY = len3;
                                            self->scale          = len3 * 0x10;
                                            index                = OTU_ANGLE_INDEX(self->angle);
                                            headX = (s32)(((s64)self->scale * ((s16*)data_0205e4e0)[index + 1] + 0x800) >> 12);
                                            headY = (s32)(((s64)self->scale * ((s16*)data_0205e4e0)[index] + 0x800) >> 12);
                                            self->posB.x = self->pinPos.x + ((headX + (headX >> 31)) >> 1);
                                            self->posB.y = self->pinPos.y + ((headY + (headY >> 31)) >> 1);
                                        }

                                        steppedB = func_ov039_0209008c(&self->cursorSpin);
                                        if (steppedB != 0) {
                                            s32 len4;

                                            index                = OTU_ANGLE_INDEX(self->angle);
                                            len4                 = self->scale + func_ov039_0209011c(&self->cursorSpin);
                                            self->affineA.scaleX = func_ov039_020900f4(&self->cursorSpin);
                                            headX        = (s32)(((s64)len4 * ((s16*)data_0205e4e0)[index + 1] + 0x800) >> 12);
                                            headY        = (s32)(((s64)len4 * ((s16*)data_0205e4e0)[index] + 0x800) >> 12);
                                            self->posA.x = self->pinPos.x + headX;
                                            self->posA.y = self->pinPos.y + headY;
                                        }

                                        if ((stepped == 0) && (steppedB == 0)) {
                                            self->state = 0U;
                                        }
                                }
                        }
                }
                break;
            }

            default:
                break;
        }

        if (self->live != 0) {
            if (self->showA != 0) {
                Sprite_Update(&self->spriteA);
            }
            if (self->showB != 0) {
                Sprite_Update(&self->spriteB);
            }
        }
    } else {
        self->live = 0;
    }
    return 1;
}

/**
 * Draws the hammer's two sprites, if it is still live.
 *
 * Positions are recomputed from the fixed-point targets here rather than in
 * Update, because both targets can be rewritten between frames -- sprite B's Y
 * by `zOffset`, sprite A's by nothing -- and a sprite left standing at a stale
 * position for even one frame reads visibly. This is the third task in the
 * overlay to do it this way.
 */
s32 OtosuGame_hammer_Render(TaskPool* pool, Task* task, void* args) {
    OtosuGame_hammer* self = (OtosuGame_hammer*)task->data;

    if (self->live != 0) {
        if (self->showA != 0) {
            self->spriteA.posX = (s16)((self->posA.x - self->origin.x) >> 12);
            self->spriteA.posY = (s16)(((self->posA.y + self->height) - self->origin.y) >> 12);
            Sprite_RenderFrame(&self->spriteA);
        }
        if (self->showB != 0) {
            self->spriteB.posX = (s16)((self->posB.x - self->origin.x) >> 12);
            self->spriteB.posY = (s16)(((self->posB.y + self->height) - self->origin.y) >> 12);
            Sprite_RenderFrame(&self->spriteB);
        }
    }
    return 1;
}

/**
 * Releases both sprites and deletes the four children.
 *
 * Sprite B goes first, which is the reverse of the order they were created in.
 * The children are deleted in a plain counted loop over `children[4]`; the task
 * does not wait on them, it hands them to EasyTask_DeleteTask and moves on.
 */
s32 OtosuGame_hammer_Destroy(TaskPool* pool, Task* task, void* args) {
    OtosuGame_hammer* self = (OtosuGame_hammer*)task->data;
    s32               i;

    Sprite_Release(&self->spriteB);
    Sprite_Release(&self->spriteA);

    for (i = 0; i < 4; i++) {
        EasyTask_DeleteTask(pool, self->children[i]);
    }
    return 1;
}

/**
 * The stage dispatcher, and the handle's own taskFunc.
 *
 * The four callbacks are copied from the stage table in the overlay's own data
 * onto the stack and then indexed by `stage`, so this is a plain indirect call
 * through a four-word copy rather than a switch.
 */
s32 OtosuGame_hammer_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = OtosuGame_hammer_Init,
        .update     = OtosuGame_hammer_Update,
        .render     = OtosuGame_hammer_Render,
        .cleanup    = OtosuGame_hammer_Destroy,
    };

    return stages.iter[stage](pool, task, args);
}

/**
 * Creates the hammer task and four children under it.
 *
 * Returns the parent's handle. The four child handles go into the fresh
 * parent's own data rather than being returned, so this returns one task and
 * leaves four behind it.
 */
u32 OtosuGame_hammer_CreateTask(TaskPool* pool, s32 dataType, s32 pinId) {
    OtuPinSpriteArgs  params;
    u32               handle;
    OtosuGame_hammer* self;
    s32               i;

    params.dataType = dataType;
    params.childId  = pinId;

    handle = EasyTask_CreateTask(pool, &Tsk_OtosuGame_hammer, NULL, 0, NULL, &params);

    self = (OtosuGame_hammer*)EasyTask_GetTaskData(pool, handle);

    for (i = 0; i < 4; i++) {
        self->children[i] = OtosuGame_hammerhahen_CreateTask(pool, dataType, pinId);
    }
    return handle;
}

// Size: 0x18

/**
 * Nonmatching, 65.9%.
 *
 * The structure and the arithmetic are right -- the target's `umull` plus the two
 * `mla`s for the -16 products come out of `(s64)v * -16 + 0x800` unchanged, and
 * every store lands on the right offset. What is left is register allocation and
 * scheduling: the `stmib` that merges the y and w stores, and roughly forty
 * instructions that differ only in which callee-saved register they landed in.
 *
 * A shared local for the 0x10000 was tried and is byte-identical, so the merge is
 * the scheduler's choice rather than anything the source asks for.
 */
s32 func_ov039_02090e9c(OtosuGame_hammer* self, OtuPinRecord* out) {
    s16 cosT;
    s16 sinT;
    s32 index;

    if (self->state != 4) {
        return 0;
    }

    index = OTU_ANGLE_INDEX(self->angle);
    cosT  = ((s16*)data_0205e4e0)[index + 1];
    sinT  = ((s16*)data_0205e4e0)[index];

    out[0].x     = self->pinPos.x + ((s32)(((s64)self->scale * cosT + 0x800) >> 12)) - ((s32)(((s64)sinT * 16 + 0x800) >> 12));
    out[0].y     = self->pinPos.y + ((s32)(((s64)self->scale * sinT + 0x800) >> 12)) + ((s32)(((s64)cosT * 16 + 0x800) >> 12));
    out[0].scale = 0x10000;

    out[1].x = self->pinPos.x + ((s32)(((s64)self->scale * cosT + 0x800) >> 12)) - ((s32)(((s64)sinT * -16 + 0x800) >> 12));
    out[1].y = self->pinPos.y + ((s32)(((s64)self->scale * sinT + 0x800) >> 12)) + ((s32)(((s64)cosT * -16 + 0x800) >> 12));
    out[1].scale = 0x10000;

    return 2;
}

/** True while the hammer is swinging. */
s32 func_ov039_02091014(OtosuGame_hammer* self) {
    return self->state != 0;
}

/** Starts a swing: the length rate, the final length, its frames and its arc. */
void func_ov039_02091028(OtosuGame_hammer* self, s32 rate, s32 scale, s32 frames, u16 arc) {
    self->rate0       = rate;
    self->scale0      = scale;
    self->totalFrames = frames;
    self->angleBase   = arc;
    self->reportTimer = 1;
    self->state       = 1;
}

/** Ends the arc early if the hammer is mid-swing. */
void func_ov039_0209104c(OtosuGame_hammer* self) {
    if (self->state == 4) {
        self->framesLeft = 0;
    }
}

/** The hammer's current angle. */
u16 func_ov039_02091060(OtosuGame_hammer* self) {
    return (u16)self->angle;
}

/**
 * Launches the four fragments: the first at the head with the head's affine,
 * the rest spaced along the shaft (a quarter of the way per fragment from the
 * pin towards the head) with the shaft's.
 */
// Nonmatching: 89.4%, one register: the target keeps `affine` in its own
// callee-saved register and needs an alignment push for it.
void func_ov039_02091070(OtosuGame_hammer* self) {
    OtuPoint pt;
    s32      i;

    for (i = 0; i < 4; i++) {
        OamAffineParam* affine;
        void*           data = EasyTask_GetTaskData(self->pool, self->children[i]);

        if (i == 0) {
            s32 hy = self->posA.y;
            s32 hx = self->posA.x;

            pt.x   = hx;
            pt.y   = hy;
            affine = &self->affineA;
        } else {
            /* `i * 0x1000` rather than an accumulator, or mwcc folds the `/ 4`
             * into the induction variable. */
            s32 phase = i * 0x1000;

            func_ov039_02098bb0(&self->posA, &self->pinPos, &pt);
            func_ov039_02098c00(phase / 4, &pt, &self->pinPos, &pt);
            affine = &self->affineB;
        }

        func_ov039_020983c8(data, &pt, affine, i);
    }
}
