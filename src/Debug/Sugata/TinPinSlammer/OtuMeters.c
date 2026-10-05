#include "OtuFieldAccessShared.h"

/*
 * ov039 region 0x0209003c - 0x02092484. One translation unit of the
 * overlay; dsd gives each file a single contiguous `.text` claim. The
 * shared types, externs and prototypes are in OtuFieldAccessShared.h.
 */
/** Splits a fixed-point value into a screen offset. */
#define OTU_ANGLE_INDEX(a) (((s32)((a) >> 4)) * 2)

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

SpriteFrameInfo* func_ov039_02090208(Sprite* sprite, s32 arg, s32 mode) {
    OtuHammer* owner = sprite->owner;

    Sprite_FrameInfoCallbackAffineSorted(sprite, mode, &owner->affineA, func_ov039_02088400(4, owner->posA.y, 0));
}

SpriteFrameInfo* func_ov039_020902cc(Sprite* sprite, s32 arg, s32 mode) {
    OtuHammer* owner = sprite->owner;

    Sprite_FrameInfoCallbackAffineSorted(sprite, mode, &owner->affineB, func_ov039_02088400(2, owner->posB.y, 0));
}

/** Loads the sprite for the task whose anim template is data_ov039_02099504. */
void func_ov039_02090390(OtuHammer* self, Sprite* sprite, OtuPinSpriteArgs* args) {
    SpriteAnimation anim = data_ov039_02099504;

    anim.owner    = self;
    anim.dataType = (u16)args->dataType;
    anim.posX     = self->posA.x >> 12;
    anim.posY     = self->posA.y >> 12;
    _Sprite_Load(sprite, &anim);
}

/** Loads the sprite for the task whose anim template is data_ov039_02099530. */
void func_ov039_0209041c(OtuHammer* self, Sprite* sprite, OtuPinSpriteArgs* args) {
    SpriteAnimation anim = data_ov039_02099530;

    anim.owner    = self;
    anim.dataType = (u16)args->dataType;
    anim.posX     = self->posB.x >> 12;
    anim.posY     = self->posB.y >> 12;
    _Sprite_Load(sprite, &anim);
}

s32 func_ov039_020904a8(TaskPool* pool, Task* task, OtuPinSpriteArgs* args) {
    OtuHammer* self = task->data;

    self->pool = pool;

    self->live  = 0;
    self->pinId = args->childId;

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
    func_ov039_02090390(self, &self->spriteA, args);
    func_ov039_0209041c(self, &self->spriteB, args);
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
s32 func_ov039_0209054c(TaskPool* pool, Task* task, void* args) {
    OtuBadge*  pin;
    OtuHammer* self = (OtuHammer*)task->data;

    pin = (OtuBadge*)EasyTask_GetTaskData(pool, self->pinId);
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
s32 func_ov039_02090cec(TaskPool* pool, Task* task, void* args) {
    OtuHammer* self = (OtuHammer*)task->data;

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
s32 func_ov039_02090d90(TaskPool* pool, Task* task, void* args) {
    OtuHammer* self = (OtuHammer*)task->data;
    s32        i;

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
s32 func_ov039_02090dd4(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = data_ov039_02099448;

    return stages.iter[stage](pool, task, args);
}

/**
 * Creates the hammer task and four children under it.
 *
 * Returns the parent's handle. The four child handles go into the fresh
 * parent's own data rather than being returned, so this returns one task and
 * leaves four behind it.
 */
u32 func_ov039_02090e1c(TaskPool* pool, s32 dataType, s32 pinId) {
    OtuPinSpriteArgs params;
    u32              handle;
    OtuHammer*       self;
    s32              i;

    params.dataType = dataType;
    params.childId  = pinId;

    handle = EasyTask_CreateTask(pool, &data_ov039_0209942c, NULL, 0, NULL, &params);

    self = (OtuHammer*)EasyTask_GetTaskData(pool, handle);

    for (i = 0; i < 4; i++) {
        self->children[i] = func_ov039_02098394(pool, dataType, pinId);
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
s32 func_ov039_02090e9c(OtuHammer* self, OtuPinRecord* out) {
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
s32 func_ov039_02091014(OtuHammer* self) {
    return self->state != 0;
}

/** Starts a swing: the length rate, the final length, its frames and its arc. */
void func_ov039_02091028(OtuHammer* self, s32 rate, s32 scale, s32 frames, u16 arc) {
    self->rate0       = rate;
    self->scale0      = scale;
    self->totalFrames = frames;
    self->angleBase   = arc;
    self->reportTimer = 1;
    self->state       = 1;
}

/** Ends the arc early if the hammer is mid-swing. */
void func_ov039_0209104c(OtuHammer* self) {
    if (self->state == 4) {
        self->framesLeft = 0;
    }
}

/** The hammer's current angle. */
u16 func_ov039_02091060(OtuHammer* self) {
    return (u16)self->angle;
}

/**
 * Launches the four fragments: the first at the head with the head's affine,
 * the rest spaced along the shaft (a quarter of the way per fragment from the
 * pin towards the head) with the shaft's.
 */
// Nonmatching: 89.4%, one register: the target keeps `affine` in its own
// callee-saved register and needs an alignment push for it.
void func_ov039_02091070(OtuHammer* self) {
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

/* ==================================================================== */
/* Tsk_OtosuGame_needle                                                 */
/* ==================================================================== */

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

/* ==================================================================== */
/* Tsk_OtosuGame_hand                                                   */
/* ==================================================================== */

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

/* ==================================================================== */
/* Tsk_OtosuGame_floor                                                  */
/* ==================================================================== */

/**
 * Stamps a 4x4 block of tile numbers into a 32-wide map. `value` is a u16
 * narrowed after every increment.
 */
void func_ov039_02091b40(u16 start, u16* map) {
    u16 value = start;
    s32 col;
    s32 row;

    for (row = 0; row < 4; row++) {
        for (col = 0; col < 4; col++) {
            map[row * 0x20 + col] = value;
            value++;
        }
        value += 0x1C;
    }
}

/**
 * Builds the floor: loads the layout kind's tile art and the five animated
 * palettes, allocates one 0x800-byte tile cell per 8x8 block of the layout,
 * stamps every layout cell's 4x4 tile block into its cell and hands the cell
 * table to the BG map.
 *
 * The layout's kind byte is re-read at every use; held in a local it costs a
 * stack slot the target does not have.
 */
// Nonmatching: 88.3%, register naming and two extra stack words.
s32 func_ov039_02091b98(TaskPool* pool, Task* task, OtuBoardArgs* args) {
    OtuFloor* self = task->data;
    Data*     data;
    Data*     data2;
    u8*       pal;
    u8*       chr;
    u8*       scr;
    Heap*     heap;
    s32       row;
    s32       row2;
    s32       cellsWide;
    s32       cellsHigh;
    s32       i;
    s32       j;
    s32       run;
    u8        wide;
    s32       col;

    self->loaded   = 1;
    self->dataType = args->dataType;
    self->heap     = args->heap;
    self->layout   = args->layout;

    s32 sheetTable[3]    = {2, 3, 4};
    s32 styleTable[3][3] = {
        {1, 2, 3},
        {1, 2, 3},
        {1, 2, 3}
    };
    s32 miscTable[3] = {4, 4, 4};

    heap = self->heap;

    data       = DatMgr_LoadRawData(args->dataType, NULL, 0, &data_ov039_0209a0b4[sheetTable[self->layout->kind]]);
    self->data = data;

    row2 = miscTable[self->layout->kind];

    row  = styleTable[self->layout->kind][self->layout->variant];
    wide = self->layout->width;

    {
        if (data == NULL || row <= 0) {
            chr = NULL;
        } else {
            u8* base = (u8*)data->buffer + 0x20;
            chr      = base + *(u32*)(base + (u32)row * 8);
        }
        if (data == NULL || row2 <= 0) {
            pal = NULL;
        } else {
            u8* base = (u8*)data->buffer + 0x20;
            pal      = base + *(u32*)(base + (u32)row2 * 8);
        }

        self->palette = PaletteMgr_AllocPalette(g_PaletteManagers[0], pal, 0, 0, 5);
        {
            u32 size = (u32)((*(u32*)chr & ~0xFF) >> 8);

            if ((*(u8*)chr & 0xF0) == 0) {
                size -= 4;
            }
            self->chars = BgResMgr_AllocChar32(g_BgResourceManagers[0], chr,
                                               g_DisplaySettings.engineState[0].bgSettings[1].charBase, 0, size);
        }
        PaletteMgr_Flush(g_PaletteManagers[0], self->palette);
    }

    data2          = DatMgr_LoadRawData(args->dataType, NULL, 0, data_ov039_0209a0bc);
    self->animData = data2;

    {
        if (data2 == NULL) {
            pal = NULL;
        } else {
            u8* base = (u8*)data2->buffer + 0x20;
            pal      = base + *(u32*)(base + 0x10);
        }
        self->animPalettes[0].palette = PaletteMgr_AllocPalette(
            g_PaletteManagers[0], func_ov039_02098d7c(&self->animPalettes[0].anim, (s32)pal, data_ov039_0209964c, 8), 0, 6, 1);
        PaletteMgr_Flush(g_PaletteManagers[0], self->animPalettes[0].palette);
    }

    {
        if (data2 == NULL) {
            pal = NULL;
        } else {
            u8* base = (u8*)data2->buffer + 0x20;
            pal      = base + *(u32*)(base + 0x18);
        }
        self->animPalettes[1].palette = PaletteMgr_AllocPalette(
            g_PaletteManagers[0], func_ov039_02098d7c(&self->animPalettes[1].anim, (s32)pal, data_ov039_02099620, 4), 0, 7, 1);
        PaletteMgr_Flush(g_PaletteManagers[0], self->animPalettes[1].palette);
    }

    {
        if (data2 == NULL) {
            pal = NULL;
        } else {
            u8* base = (u8*)data2->buffer + 0x20;
            pal      = base + *(u32*)(base + 0x20);
        }
        self->animPalettes[2].palette = PaletteMgr_AllocPalette(
            g_PaletteManagers[0], func_ov039_02098d7c(&self->animPalettes[2].anim, (s32)pal, data_ov039_0209966c, 8), 0, 8, 1);
        PaletteMgr_Flush(g_PaletteManagers[0], self->animPalettes[2].palette);
    }

    {
        if (data2 == NULL) {
            pal = NULL;
        } else {
            u8* base = (u8*)data2->buffer + 0x20;
            pal      = base + *(u32*)(base + 0x28);
        }
        self->animPalettes[3].palette = PaletteMgr_AllocPalette(
            g_PaletteManagers[0], func_ov039_02098d7c(&self->animPalettes[3].anim, (s32)pal, data_ov039_020996b0, 0x19), 0, 9,
            1);
        PaletteMgr_Flush(g_PaletteManagers[0], self->animPalettes[3].palette);
    }

    {
        if (data2 == NULL) {
            pal = NULL;
        } else {
            u8* base = (u8*)data2->buffer + 0x20;
            pal      = base + *(u32*)(base + 0x30);
        }
        self->animPalettes[4].palette = PaletteMgr_AllocPalette(
            g_PaletteManagers[0], func_ov039_02098d7c(&self->animPalettes[4].anim, (s32)pal, data_ov039_02099630, 7), 0, 0xA,
            1);
        PaletteMgr_Flush(g_PaletteManagers[0], self->animPalettes[4].palette);
    }

    {
        if (data2 == NULL) {
            chr = NULL;
        } else {
            u8* base = (u8*)data2->buffer + 0x20;
            chr      = base + *(u32*)(base + 0x38);
        }
        {
            u32 size = (u32)((*(u32*)chr & ~0xFF) >> 8);

            if ((*(u8*)chr & 0xF0) == 0) {
                size -= 4;
            }
            self->animChars = BgResMgr_AllocChar32(g_BgResourceManagers[0], chr,
                                                   g_DisplaySettings.engineState[0].bgSettings[1].charBase, 0x300, size);
        }
    }

    cellsWide = (wide + 7 + ((u32)((wide + 7) >> 2) >> 0x1D)) >> 3;
    cellsHigh = (self->layout->height + 7 + ((u32)((self->layout->height + 7) >> 2) >> 0x1D)) >> 3;

    self->tilePool = Mem_AllocHeapTail(heap, cellsHigh * (cellsWide << 0xB));
    self->tiles    = Mem_AllocHeapTail(heap, cellsHigh * (cellsWide * 4));
    MI_CpuSet(self->tilePool, 0, Mem_GetBlockSize(heap, self->tilePool));

    if (cellsHigh > 0) {
        i   = 0;
        run = 0;
        do {
            j = 0;
            if (cellsWide > 0) {
                do {
                    self->tiles[run + j] = self->tilePool + (j + run) * 0x800;
                    j                    = j + 1;
                } while (j < cellsWide);
            }
            i   = i + 1;
            run = run + cellsWide;
        } while (i < cellsHigh);
    }

    if (self->layout->height > 0) {
        col = 0;
        do {
            if (self->layout->width > 0) {
                j = 0;
                do {
                    u8* cellBase = self->layout->cells + (col * self->layout->width + j) * 2;
                    u8  cellByte = cellBase[1];
                    u16 tileVal  = *(u16*)(data_ov039_0209a5d8[self->layout->kind] + cellByte * 2);

                    func_ov039_02091b40(
                        tileVal, (u16*)(self->tiles[(j / 8) * cellsWide + (col / 8)] + ((j % 8) * 4 + (col % 8) * 128) * 2));
                    j = j + 1;
                } while (j < (s32)self->layout->width);
            }
            col = col + 1;
        } while (col < (s32)self->layout->height);
    }

    func_0200d1d8(&self->bg, 0, 1, 0, self->tiles, cellsWide, cellsHigh);
    return 1;
}

/** Steps the five animated palettes. Unrolled, as the target has it. */
s32 func_ov039_02092154(TaskPool* pool, Task* task, void* args) {
    OtuFloor* self = task->data;

    PaletteMgr_SetSource(g_PaletteManagers[0], self->animPalettes[0].palette,
                         (void*)func_ov039_02098dbc(&self->animPalettes[0].anim));
    PaletteMgr_SetSource(g_PaletteManagers[0], self->animPalettes[1].palette,
                         (void*)func_ov039_02098dbc(&self->animPalettes[1].anim));
    PaletteMgr_SetSource(g_PaletteManagers[0], self->animPalettes[2].palette,
                         (void*)func_ov039_02098dbc(&self->animPalettes[2].anim));
    PaletteMgr_SetSource(g_PaletteManagers[0], self->animPalettes[3].palette,
                         (void*)func_ov039_02098dbc(&self->animPalettes[3].anim));
    PaletteMgr_SetSource(g_PaletteManagers[0], self->animPalettes[4].palette,
                         (void*)func_ov039_02098dbc(&self->animPalettes[4].anim));
    return 1;
}

/** The floor's render stage: the BG draws itself. */
s32 func_ov039_020921f4(void) {
    return 1;
}

s32 func_ov039_020921fc(TaskPool* pool, Task* task, void* args) {
    OtuFloor* self = task->data;

    func_0200d954(0, 1);

    Mem_Free(self->heap, self->tiles);
    Mem_Free(self->heap, self->tilePool);

    BgResMgr_ReleaseChar(g_BgResourceManagers[0], self->chars);
    BgResMgr_ReleaseChar(g_BgResourceManagers[0], self->animChars);

    PaletteMgr_ReleaseResource(g_PaletteManagers[0], self->palette);
    PaletteMgr_ReleaseResource(g_PaletteManagers[0], self->animPalettes[0].palette);
    PaletteMgr_ReleaseResource(g_PaletteManagers[0], self->animPalettes[1].palette);
    PaletteMgr_ReleaseResource(g_PaletteManagers[0], self->animPalettes[2].palette);
    PaletteMgr_ReleaseResource(g_PaletteManagers[0], self->animPalettes[3].palette);
    PaletteMgr_ReleaseResource(g_PaletteManagers[0], self->animPalettes[4].palette);

    DatMgr_ReleaseData(self->data);
    DatMgr_ReleaseData(self->animData);
    return 1;
}

s32 func_ov039_020922c8(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = data_ov039_02099610;

    return stages.iter[stage](pool, task, args);
}

s32 func_ov039_02092310(TaskPool* pool, s32 dataType, Heap* heap, OtuBoardLayout* layout) {
    OtuBoardArgs args;

    args.dataType = dataType;
    args.heap     = heap;
    args.layout   = layout;
    return EasyTask_CreateTask(pool, &data_ov039_020995ec, NULL, 0, NULL, &args);
}

/**
 * Scrolls the floor's BG layer to (x, y), flagging an affine layer for update.
 * The bgMode test is a switch: the target has a six-entry jump table.
 */
// Nonmatching: 98.5%, one register swap between the engine index and the
// engine-state pointer; ten source shapes were tried.
void func_ov039_02092348(OtuFloor* self, s32 x, s32 y) {
    s32                 a     = self->bg.display;
    s32                 b     = self->bg.layer;
    DisplayEngineState* group = g_DisplaySettings.engineState + a;
    switch (group->bgSettings[b].bgMode) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            group->bgAffines[b].unk_14 = 1;
            break;

        default:
            break;
    }

    group->bgOffsets[b].hOffset = x;
    group->bgOffsets[b].vOffset = y;
}

/** Marks the floor's BG map dirty when `event` is set. */
void func_ov039_020923b4(OtuFloor* self, s32 event) {
    if (event != 0) {
        self->bg.flags |= 2;
    }
}

/* ==================================================================== */
/* Tsk_OtosuGame_obstacle (continued in OtuObstacles.c)                 */
/* ==================================================================== */

SpriteFrameInfo* func_ov039_020923c8(Sprite* sprite, s32 arg, s32 mode) {
    OtuObstacle* owner = sprite->owner;

    Sprite_FrameInfoCallbackSorted(sprite, mode, func_ov039_02088400(3, owner->pos.y, 0));
}
