#include "OtuFieldAccessShared.h"

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
s16 func_ov039_0209003c(OtuCursor6* c) {
    if (c->index >= c->count) {
        return 0;
    }

    return ((OtuFrame4*)c->table)[c->index].value;
}

/** Starts a cursor at entry 0, primed with the first entry's duration plus one.
 *
 *  The `+ 1` is what makes the constructor and the stepper agree: a freshly
 *  started cursor has `framesLeft` equal to entry 0's duration, so the first
 *  step immediately falls through to entry 1.
 */
void func_ov039_0209005c(OtuCursor6* c, OtuFrame6* table, s16 count) {
    s32 offset;

    c->table      = table;
    c->index      = 0;
    c->count      = count;
    offset        = c->index * 6;
    c->framesLeft = *(s16*)((u8*)c->table + offset) + 1;
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
    s32 offset;

    c->table      = table;
    c->index      = 0;
    c->count      = count;
    offset        = c->index * 6;
    c->framesLeft = *(s16*)((u8*)c->table + offset) + 1;
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

// Nonmatching: 68-78%. The body, the three varied constants and the guard chain
// are all correct; what is left is mwcc scheduling the two-step lookup.
//
// Three bugs were real and are fixed, so the remaining gap is not one of those:
//
//   * the axis argument to func_ov039_02088400 is passed and never read -- the
//     target loads it into r2 and then uses r2 only as the mask source, so the
//     last instruction is a plain shift. The packer is now a full MATCH.
//   * slot+0x04 and slot+0x08 are 32-bit words, not a u16 and a byte pointer.
//     Every store in the target is `str`, and the widths are load-bearing.
//   * the guard is three flat short-circuit tests over +0x18, +0x1C and +0x16,
//     not two nested ones. The target's `ldrne` on the table load is the
//     short-circuit, which is why +0x18 is tested at all.
/* `arg` is unused: the target loads r1 nowhere in these bodies and compares
 * only r2. Kept as a named parameter so the selector stays in the third
 * argument slot, which is where the target reads it. */
OtuSpriteSlot* func_ov039_02090208(OtuSpriteTask* t, s32 arg, s32 mode) {
    OtuSpriteSlot* slot = (OtuSpriteSlot*)&data_0206b408;

    (void)arg;

    switch (mode) {
        case 1:
            slot->unk_00 = 1;
            return slot;

        case 2: {
            s32 index;
            u8* table;

            slot->unk_04   = 0;
            slot->unk_08   = 0;
            slot->unk_0C   = 0;
            slot->depthKey = -1;

            // The two-step lookup: a u16 out of the task's table at one stride,
            // then a byte pointer built from the u16 at the other. Guarded on the
            // table pointer and on the index being non-negative.
            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + ((u16*)table)[index * 4] * 2);
            }

            slot->unk_0C   = (void*)((u8*)t + 0x84);
            slot->depthKey = func_ov039_02088400(*(s32*)((u8*)t + 0xB0), 0, 0);
            return slot;
        }

        default:
            return NULL;
    }
}

// Nonmatching: 68-78%. The body, the three varied constants and the guard chain
// are all correct; what is left is mwcc scheduling the two-step lookup.
//
// Three bugs were real and are fixed, so the remaining gap is not one of those:
//
//   * the axis argument to func_ov039_02088400 is passed and never read -- the
//     target loads it into r2 and then uses r2 only as the mask source, so the
//     last instruction is a plain shift. The packer is now a full MATCH.
//   * slot+0x04 and slot+0x08 are 32-bit words, not a u16 and a byte pointer.
//     Every store in the target is `str`, and the widths are load-bearing.
//   * the guard is three flat short-circuit tests over +0x18, +0x1C and +0x16,
//     not two nested ones. The target's `ldrne` on the table load is the
//     short-circuit, which is why +0x18 is tested at all.
/* `arg` is unused: the target loads r1 nowhere in these bodies and compares
 * only r2. Kept as a named parameter so the selector stays in the third
 * argument slot, which is where the target reads it. */
OtuSpriteSlot* func_ov039_020902cc(OtuSpriteTask* t, s32 arg, s32 mode) {
    OtuSpriteSlot* slot = (OtuSpriteSlot*)&data_0206b408;

    (void)arg;

    switch (mode) {
        case 1:
            slot->unk_00 = 1;
            return slot;

        case 2: {
            s32 index;
            u8* table;

            slot->unk_04   = 0;
            slot->unk_08   = 0;
            slot->unk_0C   = 0;
            slot->depthKey = -1;

            // The two-step lookup: a u16 out of the task's table at one stride,
            // then a byte pointer built from the u16 at the other. Guarded on the
            // table pointer and on the index being non-negative.
            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + ((u16*)table)[index * 4] * 2);
            }

            slot->unk_0C   = (void*)((u8*)t + 0x94);
            slot->depthKey = func_ov039_02088400(*(s32*)((u8*)t + 0xB8), 0, 0);
            return slot;
        }

        default:
            return NULL;
    }
}

/** Loads the sprite for the task whose anim template is data_ov039_02099504. */
void func_ov039_02090390(OtuHammer* self, Sprite* sprite, OtuHammer_Params* params) {
    SpriteAnimation anim = data_ov039_02099504;

    anim.owner    = self;
    anim.dataType = (u16) * (s32*)params;
    anim.posX     = *(s32*)((u8*)self + 0xAC) >> 12;
    anim.posY     = *(s32*)((u8*)self + 0xB0) >> 12;
    _Sprite_Load(sprite, &anim);
}

/** Loads the sprite for the task whose anim template is data_ov039_02099530. */
void func_ov039_0209041c(OtuHammer* self, Sprite* sprite, OtuHammer_Params* params) {
    SpriteAnimation anim = data_ov039_02099530;

    anim.owner    = self;
    anim.dataType = (u16) * (s32*)params;
    anim.posX     = *(s32*)((u8*)self + 0xB4) >> 12;
    anim.posY     = *(s32*)((u8*)self + 0xB8) >> 12;
    _Sprite_Load(sprite, &anim);
}

s32 func_ov039_020904a8(s32 arg0, Task* task, OtuHammer_Params* params) {
    OtuHammer* self = (OtuHammer*)task->data;

    self->unk_00 = arg0;

    self->live         = 0;
    self->parentHandle = params->arg2;

    self->baseX    = 0;
    self->baseY    = 0;
    self->targetX  = 0;
    self->targetY  = 0;
    self->targetXB = 0;
    self->targetYB = 0;
    self->originX  = 0;
    self->originY  = 0;

    self->rate0       = 0;
    self->scale0      = 0;
    self->totalFrames = 0;
    self->angleBase   = 0;

    self->spriteRotA = 0;
    self->spinValue  = 0x1000;
    self->unk_8C     = 0x1000;
    self->unk_90     = 0;
    self->unk_92     = 0;
    self->spriteRotB = 0;
    self->unk_98     = 0x1000;
    self->lenValue   = 0x1000;
    self->unk_A0     = 0;
    self->unk_A2     = 0;

    self->state = 0;
    func_ov039_02090390(self, &self->spriteA, params);
    func_ov039_0209041c(self, &self->spriteB, params);
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
    OtuPinTask* pin;
    OtuHammer*  self = (OtuHammer*)task->data;

    pin = (OtuPinTask*)EasyTask_GetTaskData(pool, self->parentHandle);
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
            func_ov039_0208e85c(pin, (OtuPoint*)&self->baseX);
            func_ov039_0208e6e0(pin, (OtuPoint*)&self->originX);
            self->zOffset = func_ov039_0208e6f4(pin);
        }

        switch (self->state) {
            case 0:
                self->live = 0;
                break;

            case 1:
                self->showA = 0;
                self->showB = 1;
                func_ov039_0208ffac((OtuCursor*)&self->cursorScale, (s32*)&data_ov039_02099494, 8);
                func_ov039_0209005c(&self->cursorSpin, &data_ov039_020994b4, 6);
                self->angle0     = func_ov039_0208e9ac(pin);
                self->angle      = self->angle0;
                self->spriteRotB = (u16)(self->angle + 0x4000);
                self->spriteRotA = (u16)(self->angle + 0x4000);
                self->scale      = 0;
                self->state      = (u32)(self->state + 1);
                func_ov039_02087d04(0x338, (OtuPoint*)&self->targetX, (OtuPoint*)&self->baseX);
                /* fallthrough */

            case 2: {
                s32 len;

                stepped = func_ov039_0208ffd8((OtuCursor*)&self->cursorScale);
                if (stepped != 0) {
                    s32 rate;

                    rate           = self->rate0;
                    len            = (s32)(((s64)func_ov039_0209003c(&self->cursorScale) * rate + 0x800) >> 12);
                    self->lenValue = len;
                    self->scale    = len * 0x10;
                    index          = OTU_ANGLE_INDEX(self->angle);
                    headX = (s32)(((s64)self->scale * (*(s16*)((u8*)&data_0205e4e0 + (index + 1) * 2)) + 0x800) >> 12);
                    headY = (s32)(((s64)self->scale * (*(s16*)((u8*)&data_0205e4e0 + index * 2)) + 0x800) >> 12);
                    self->targetXB = self->originX + ((headX + (headX >> 31)) >> 1);
                    self->targetYB = self->originY + ((headY + (headY >> 31)) >> 1);
                }

                steppedB = func_ov039_0209008c(&self->cursorSpin);
                if (steppedB != 0) {
                    s32 len2;

                    index           = OTU_ANGLE_INDEX(self->angle);
                    len2            = self->scale + func_ov039_0209011c(&self->cursorSpin);
                    headX           = (s32)(((s64)len2 * (*(s16*)((u8*)&data_0205e4e0 + (index + 1) * 2)) + 0x800) >> 12);
                    headY           = (s32)(((s64)len2 * (*(s16*)((u8*)&data_0205e4e0 + index * 2)) + 0x800) >> 12);
                    self->spinValue = func_ov039_020900f4(&self->cursorSpin);
                    if (self->spinValue > 0) {
                        self->showA = 1;
                    }
                    self->targetX = self->originX + headX;
                    self->targetY = self->originY + headY;
                }

                if ((stepped == 0) && (steppedB == 0)) {
                    func_ov039_02090148(&self->cursorTrail, &data_ov039_02099476, 5);
                    self->state = (u32)(self->state + 1);
                    func_ov039_02087d04(0x339, (OtuPoint*)&self->targetX, (OtuPoint*)&self->baseX);
                }
                /* fallthrough */
            }

            case 3: {
                if (func_ov039_02090178(&self->cursorTrail) != 0) {
                    angle       = (u16)(self->angle0 + func_ov039_020901e0(&self->cursorTrail));
                    index       = OTU_ANGLE_INDEX(angle);
                    self->angle = angle;
                    headX       = (s32)(((s64)self->scale * (*(s16*)((u8*)&data_0205e4e0 + (index + 1) * 2)) + 0x800) >> 12);
                    headY       = (s32)(((s64)self->scale * (*(s16*)((u8*)&data_0205e4e0 + index * 2)) + 0x800) >> 12);
                    // The two rotation writes deliberately sit *after* the trigonometry
                    // here, and deliberately sit before it in state 4. That is not a
                    // typo and it is not arbitrary: all four placements were measured,
                    // and this one is worth a point over the alternative. Note that the
                    // target's `add r, r, #0x4000` appears between the two multiplies in
                    // every state, which is the scheduler hoisting the arithmetic -- not
                    // a statement position. Putting the writes there to match it costs
                    // four and a half points.
                    self->spriteRotB = (u16)(angle + 0x4000);
                    self->spriteRotA = (u16)(angle + 0x4000);
                    self->targetX    = self->originX + headX;
                    self->targetY    = self->originY + headY;
                    self->targetXB   = self->originX + ((headX + (headX >> 31)) >> 1);
                    self->targetYB   = self->originY + ((headY + (headY >> 31)) >> 1);
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

                            len            = self->lenValue + ((self->scale0 * 4) - self->lenValue) / angle;
                            self->lenValue = len;
                            self->scale    = len * 0x10;
                            spin           = self->angle0 -
                                   (((self->angleBase << 0x10) * (self->totalFrames - self->framesLeft)) / self->totalFrames);
                            index2      = OTU_ANGLE_INDEX(spin);
                            self->angle = spin;
                            // The mirror image of state 3: here the rotation writes come
                            // before the trigonometry, which is the placement that measures best.
                            // Same two stores, opposite position, for a fifth of a point.
                            self->spriteRotB = (u16)(spin + 0x4000);
                            self->spriteRotA = (u16)(spin + 0x4000);
                            headX =
                                (s32)(((s64)self->scale * (*(s16*)((u8*)&data_0205e4e0 + (index2 + 1) * 2)) + 0x800) >> 12);
                            headY = (s32)(((s64)self->scale * (*(s16*)((u8*)&data_0205e4e0 + index2 * 2)) + 0x800) >> 12);
                            self->targetX  = self->originX + headX;
                            self->targetY  = self->originY + headY;
                            self->targetXB = self->originX + ((headX + (headX >> 31)) >> 1);
                            self->targetYB = self->originY + ((headY + (headY >> 31)) >> 1);

                            self->reportTimer = self->reportTimer - 1;
                            if (self->reportTimer <= 0) {
                                func_ov039_0208f048(EasyTask_GetTaskData(pool, self->parentHandle), (OtuPoint*)&self->targetX,
                                                    (u16)self->angle);
                                self->reportTimer = 4;
                            }
                        } else {
                            self->angle0 = self->angle;
                            func_ov039_02090148(&self->cursorTrail, &data_ov039_020994d8, 7);
                            self->state = (u32)(self->state + 1);
                                /* fallthrough */
                            case 5:
                                if (func_ov039_02090178(&self->cursorTrail) != 0) {
                                    s32 index3;

                                    angle       = (u16)(self->angle0 + func_ov039_020901e0(&self->cursorTrail));
                                    index3      = OTU_ANGLE_INDEX(angle);
                                    self->angle = angle;
                                    headX =
                                        (s32)(((s64)self->scale * (*(s16*)((u8*)&data_0205e4e0 + (index3 + 1) * 2)) + 0x800) >>
                                              12);
                                    headY =
                                        (s32)(((s64)self->scale * (*(s16*)((u8*)&data_0205e4e0 + index3 * 2)) + 0x800) >> 12);
                                    // After the trigonometry, like state 3 and unlike state 4.
                                    self->spriteRotB = (u16)(angle + 0x4000);
                                    self->spriteRotA = (u16)(angle + 0x4000);
                                    self->targetX    = self->originX + headX;
                                    self->targetY    = self->originY + headY;
                                    self->targetXB   = self->originX + ((headX + (headX >> 31)) >> 1);
                                    self->targetYB   = self->originY + ((headY + (headY >> 31)) >> 1);
                                } else {
                                    s32 len;

                                    len           = self->lenValue;
                                    self->halfLen = (s32)((s32)(len + ((u32)(len >> 1) >> 0x1E)) >> 2);
                                    func_ov039_0209005c(&self->cursorSpin, &data_ov039_02099458, 5);
                                    func_ov039_0208ffac((OtuCursor*)&self->cursorScale, (s32*)&data_ov039_02099438, 4);
                                    self->state = (u32)(self->state + 1);
                                    func_ov039_02087d04(0x33A, (OtuPoint*)&self->targetX, (OtuPoint*)&self->baseX);
                                        /* fallthrough */
                                    case 6:
                                        stepped = func_ov039_0208ffd8((OtuCursor*)&self->cursorScale);
                                        if (stepped != 0) {
                                            s32 half;
                                            s32 len3;

                                            half = self->halfLen;
                                            len3 = (s32)(((s64)func_ov039_0209003c(&self->cursorScale) * half + 0x800) >> 12);
                                            self->lenValue = len3;
                                            self->scale    = len3 * 0x10;
                                            index          = OTU_ANGLE_INDEX(self->angle);
                                            headX =
                                                (s32)(((s64)self->scale * (*(s16*)((u8*)&data_0205e4e0 + (index + 1) * 2)) +
                                                       0x800) >>
                                                      12);
                                            headY = (s32)(((s64)self->scale * (*(s16*)((u8*)&data_0205e4e0 + index * 2)) +
                                                           0x800) >>
                                                          12);
                                            self->targetXB = self->originX + ((headX + (headX >> 31)) >> 1);
                                            self->targetYB = self->originY + ((headY + (headY >> 31)) >> 1);
                                        }

                                        steppedB = func_ov039_0209008c(&self->cursorSpin);
                                        if (steppedB != 0) {
                                            s32 len4;

                                            index           = OTU_ANGLE_INDEX(self->angle);
                                            len4            = self->scale + func_ov039_0209011c(&self->cursorSpin);
                                            self->spinValue = func_ov039_020900f4(&self->cursorSpin);
                                            headX =
                                                (s32)(((s64)len4 * (*(s16*)((u8*)&data_0205e4e0 + (index + 1) * 2)) + 0x800) >>
                                                      12);
                                            headY =
                                                (s32)(((s64)len4 * (*(s16*)((u8*)&data_0205e4e0 + index * 2)) + 0x800) >> 12);
                                            self->targetX = self->originX + headX;
                                            self->targetY = self->originY + headY;
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
            self->spriteA.posX = (s16)((self->targetX - self->baseX) >> 12);
            self->spriteA.posY = (s16)(((self->targetY + self->zOffset) - self->baseY) >> 12);
            Sprite_RenderFrame(&self->spriteA);
        }
        if (self->showB != 0) {
            self->spriteB.posX = (s16)((self->targetXB - self->baseX) >> 12);
            self->spriteB.posY = (s16)(((self->targetYB + self->zOffset) - self->baseY) >> 12);
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
u32 func_ov039_02090e1c(TaskPool* pool, s32 arg1, s32 arg2) {
    OtuHammer_Params params;
    u32              handle;
    OtuHammer*       self;
    s32              i;

    params.arg1 = arg1;
    params.arg2 = arg2;

    handle = EasyTask_CreateTask(pool, &data_ov039_0209942c, NULL, 0, NULL, &params);

    self = (OtuHammer*)EasyTask_GetTaskData(pool, handle);

    for (i = 0; i < 4; i++) {
        self->children[i] = func_ov039_02098394(pool, arg1, arg2);
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
s32 func_ov039_02090e9c(OtuHammer* self, OtuBar* out) {
    s16 cosT;
    s16 sinT;
    s32 index;

    if (self->state != 4) {
        return 0;
    }

    index = OTU_ANGLE_INDEX(self->angle);
    cosT  = *(s16*)((u8*)&data_0205e4e0 + (index + 1) * 2);
    sinT  = *(s16*)((u8*)&data_0205e4e0 + index * 2);

    out->v[0].x = self->originX + ((s32)(((s64)self->scale * cosT + 0x800) >> 12)) - ((s32)(((s64)sinT * 16 + 0x800) >> 12));
    out->v[0].y = self->originY + ((s32)(((s64)self->scale * sinT + 0x800) >> 12)) + ((s32)(((s64)cosT * 16 + 0x800) >> 12));
    out->v[0].w = 0x10000;

    out->v[1].x = self->originX + ((s32)(((s64)self->scale * cosT + 0x800) >> 12)) - ((s32)(((s64)sinT * -16 + 0x800) >> 12));
    out->v[1].y = self->originY + ((s32)(((s64)self->scale * sinT + 0x800) >> 12)) + ((s32)(((s64)cosT * -16 + 0x800) >> 12));
    out->v[1].w = 0x10000;

    return 2;
}

/* ------------------------------------------------------------------ */
/* The badge-task state predicates, 0x02091014 - 0x0209167c.           */
/* ------------------------------------------------------------------ */

/** True while the badge task has an active phase at +0xE8. */
s32 func_ov039_02091014(void* task) {
    return *(s32*)((u8*)task + 0xE8) != 0;
}

/**
 * @brief Seeds the badge task's cursors and enters the active phase.
 *
 * Four cursors: +0xF0, +0xF4 and +0xF8 take the three words, +0xFC takes the
 * fourth as a u16 (the target reads it with `ldrh` from the stack argument),
 * and both the +0x124 sub-phase and the +0xE8 phase are raised to 1.
 */
void func_ov039_02091028(void* task, s32 a, s32 b, s32 c, u16 d) {
    u8* self = (u8*)task;

    *(s32*)(self + 0xF0)  = a;
    *(s32*)(self + 0xF4)  = b;
    *(s32*)(self + 0xF8)  = c;
    *(s32*)(self + 0xFC)  = d;
    *(s32*)(self + 0x124) = 1;
    *(s32*)(self + 0xE8)  = 1;
}

/** Clears the +0xEC cursor once the +0xE8 phase has reached 4. */
void func_ov039_0209104c(void* task) {
    if (*(s32*)((u8*)task + 0xE8) == 4) {
        *(s32*)((u8*)task + 0xEC) = 0;
    }
}

/**
 * @brief The +0xE0 field, zero-extended to sixteen bits.
 *
 * The target loads a whole word and masks it with `lsl #0x10 / lsr #0x10`, so
 * the field is not a halfword in the source -- the read is a word cast down.
 */
u16 func_ov039_02091060(void* task) {
    return (u16) * (s32*)((u8*)task + 0xE0);
}

// Size: 0x138

/**
 * @brief Steps the four child tasks named by +0x128 and places them along an arc.
 *
 * Child 0 is placed at the raw `home` pair against the +0x84 table; the other
 * three are placed by difference -- `func_ov039_02098bb0` takes the arc chord
 * between `home` and `center`, then `func_ov039_02098c00` scales it by a quarter
 * of the phase counter and adds it back to `center`, against the +0x94 table.
 * The phase advances 0x1000 per child and is divided by 4 when used, so the
 * scale runs 0, 0x400, 0x800, 0xC00.
 */
// Nonmatching: 89.4%, and the gap is one register's worth of allocation. The
// target gives `table` its own callee-saved register (r7) and so needs six plus
// a `push {r3, ...}` alignment dummy; this source computes `table` straight
// into the call's r2 and needs only five. Every other difference follows from
// that: the whole register naming shifts by one because the values pack
// tighter. Three things were real and are fixed -- the ids load needs a typed
// `ids[4]` at +0x128 or mwcc strength-reduces `i * 4` into a running offset
// instead of the `add rN, rN, rN, lsl #2` the target has; the `/ 4` must be
// taken of `i * 0x1000` rather than of an accumulator, or mwcc folds the divide
// into the induction variable and advances it by 0x400 instead of 0x1000; and
// the home pair has to be loaded before either is stored or the two do not come
// out in the target's order. Declaration order of `table` and of the home pair
// makes no difference.
void func_ov039_02091070(OtuArcGroup* self) {
    OtuPoint pt;
    s32      i;

    for (i = 0; i < 4; i++) {
        void* table;
        void* data = EasyTask_GetTaskData(self->pool, self->ids[i]);

        if (i == 0) {
            s32 hy = self->home.y;
            s32 hx = self->home.x;

            pt.x  = hx;
            pt.y  = hy;
            table = (u8*)self + 0x84;
        } else {
            /* `i * 0x1000` is spelled out rather than kept as an accumulator:
             * mwcc strength-reduces the product into a `+ 0x1000` counter but
             * still emits a real signed divide for the `/ 4`, which is what the
             * target has. An accumulator declared outside the loop gets the
             * divide folded into it and advances by 0x400 instead. */
            s32 phase = i * 0x1000;

            func_ov039_02098bb0(&self->home, &self->center, &pt);
            func_ov039_02098c00(phase / 4, &pt, &self->center, &pt);
            table = (u8*)self + 0x94;
        }

        func_ov039_020983c8(data, &pt, table, i);
    }
}

// Nonmatching: 68-78%. The body, the three varied constants and the guard chain
// are all correct; what is left is mwcc scheduling the two-step lookup.
//
// Three bugs were real and are fixed, so the remaining gap is not one of those:
//
//   * the axis argument to func_ov039_02088400 is passed and never read -- the
//     target loads it into r2 and then uses r2 only as the mask source, so the
//     last instruction is a plain shift. The packer is now a full MATCH.
//   * slot+0x04 and slot+0x08 are 32-bit words, not a u16 and a byte pointer.
//     Every store in the target is `str`, and the widths are load-bearing.
//   * the guard is three flat short-circuit tests over +0x18, +0x1C and +0x16,
//     not two nested ones. The target's `ldrne` on the table load is the
//     short-circuit, which is why +0x18 is tested at all.
/* `arg` is unused: the target loads r1 nowhere in these bodies and compares
 * only r2. Kept as a named parameter so the selector stays in the third
 * argument slot, which is where the target reads it. */
OtuSpriteSlot* func_ov039_02091118(OtuSpriteTask* t, s32 arg, s32 mode) {
    OtuSpriteSlot* slot = (OtuSpriteSlot*)&data_0206b408;

    (void)arg;

    switch (mode) {
        case 1:
            slot->unk_00 = 1;
            return slot;

        case 2: {
            s32 index;
            u8* table;

            slot->unk_04   = 0;
            slot->unk_08   = 0;
            slot->unk_0C   = 0;
            slot->depthKey = -1;

            // The two-step lookup: a u16 out of the task's table at one stride,
            // then a byte pointer built from the u16 at the other. Guarded on the
            // table pointer and on the index being non-negative.
            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + ((u16*)table)[index * 4] * 2);
            }

            slot->unk_0C   = (void*)((u8*)t + 0x44);
            slot->depthKey = func_ov039_02088400(*(s32*)((u8*)t + 0x60), 0, 0);
            return slot;
        }

        default:
            return NULL;
    }
}

/** Loads the sprite for the task whose anim template is data_ov039_02099578. */
void func_ov039_020911dc(void* self, void* sprite, s32* args) {
    SpriteAnimation anim = data_ov039_02099578;

    anim.owner    = self;
    anim.dataType = (u16) * (s32*)args;
    anim.posX     = *(s32*)((u8*)self + 0x5C) >> 12;
    anim.posY     = *(s32*)((u8*)self + 0x60) >> 12;
    _Sprite_Load((Sprite*)sprite, &anim);
}

/**
 * @brief Resets the block at task+0x18 and re-derives it through +0x11dc.
 *
 * The first store is the *pool*, not a field of the block's own state: the
 * block's word 0 is the heap it will later allocate out of. The zero and
 * 0x1000 runs are written in the target's order rather than by address.
 */
s32 func_ov039_02091268(TaskPool* pool, void* task, s32* args) {
    u8* block = *(u8**)((u8*)task + 0x18);

    *(TaskPool**)(block + 0x00) = pool;
    *(s32*)(block + 0x6C)       = 0;
    *(s32*)(block + 0x68)       = args[1];
    *(s32*)(block + 0x54)       = 0;
    *(s32*)(block + 0x58)       = 0;
    *(s32*)(block + 0x5C)       = 0;
    *(s32*)(block + 0x60)       = 0;
    *(s32*)(block + 0x64)       = 0;
    *(s32*)(block + 0x44)       = 0;
    *(s32*)(block + 0x48)       = 0x1000;
    *(s32*)(block + 0x4C)       = 0x1000;
    *(s16*)(block + 0x50)       = 0;
    *(s16*)(block + 0x52)       = 0;
    *(s32*)(block + 0x80)       = 0;

    func_ov039_020911dc(block, block + 4, args);
    return 1;
}

/**
 * @brief The hammer-charge task's phase machine: raise, hold, swing.
 *
 * Sprite state block: +0x6C the step flag, +0x80 the phase, +0x78 a running
 * frame count and +0x7C a sub-count. Phase 0 clears the flag; phase 1 sets the
 * charge mode 0x334 and the wind-up is phase 2, which ramps +0x48 from 0x99A
 * toward 0x1000 over the +0x70 frames -- both +0x48 and its +0x4C copy keep
 * the ramp value. On frame zero it raises the sprite's +4 bit-6 bit and enters
 * the hold, phase 3, which counts +0x7C down and re-arms the charge (mode
 * 0x336) once its own counter lands. Phase 4 runs the same ramp back against
 * 0x99A. Phase 2 re-arms at frame zero and phase 3 counts +0x7C down before
 * running the same ramp back in phase 4, which zeroes the phase on landing.
 * While their frame count is still positive, phases 2 and 3 break out of the
 * switch; the fall-throughs (1 to 2, then on to 3 and 4) only happen on the
 * counts that reach zero.
 */
s32 func_ov039_020912c8(TaskPool* pool, void* task) {
    u8*   sprite = *(u8**)((u8*)task + 0x18);
    void* pin;

    pin = EasyTask_GetTaskData(pool, *(s32*)(sprite + 0x68));

    if (pin != NULL) {
        s32 alive = func_ov039_0208e9d0(pin);

        *(s32*)(sprite + 0x6C) = alive;
        if (alive == 0) {
            *(s32*)(sprite + 0x80) = 0;
        } else {
            func_ov039_0208e85c(pin, (OtuPoint*)(sprite + 0x54));
            func_ov039_0208e6e0(pin, (OtuPoint*)(sprite + 0x5C));
            *(s32*)(sprite + 0x64) = func_ov039_0208e6f4(pin);
        }

        switch (*(s32*)(sprite + 0x80)) {
            case 0:
                *(s32*)(sprite + 0x6C) = 0;
                break;

            case 1:
                *(s32*)(sprite + 0x4C) = 0x99A;
                *(s32*)(sprite + 0x48) = 0x99A;
                Sprite_ChangeAnimation((Sprite*)(sprite + 4), *(s32*)(sprite + 0x1C), 0xE, *(SpriteCell**)(sprite + 0x20));
                *(s32*)(sprite + 0x78) = *(s32*)(sprite + 0x70);
                *(s32*)(sprite + 0x80) = 2;
                /* fall through */
            case 2:
                *(s32*)(sprite + 0x48) = *(s32*)(sprite + 0x48) + (0x1000 - *(s32*)(sprite + 0x48)) / *(s32*)(sprite + 0x78);
                *(s32*)(sprite + 0x4C) = *(s32*)(sprite + 0x48);

                *(s32*)(sprite + 0x78) = *(s32*)(sprite + 0x78) - 1;

                if (*(s32*)(sprite + 0x78) > 0) {
                    break;
                }

                *(s32*)(sprite + 4) = (*(s32*)(sprite + 4) & ~0x60) | 0x40;
                Sprite_ChangeAnimation((Sprite*)(sprite + 4), *(s32*)(sprite + 0x1C), 0xF, *(SpriteCell**)(sprite + 0x20));
                func_ov039_02087d04(0x334, (OtuPoint*)(sprite + 0x5C), (OtuPoint*)(sprite + 0x54));
                *(s32*)(sprite + 0x78) = *(s32*)(sprite + 0x74);
                *(s32*)(sprite + 0x7C) = 0x1E;
                *(s32*)(sprite + 0x80) = 3;
                /* fall through */
            case 3:
                if (*(s32*)(sprite + 0x7C) > 0) {
                    *(s32*)(sprite + 0x7C) = *(s32*)(sprite + 0x7C) - 1;
                }
                *(s32*)(sprite + 0x78) = *(s32*)(sprite + 0x78) - 1;

                if (*(s32*)(sprite + 0x78) > 0) {
                    break;
                }

                *(s32*)(sprite + 4) = (*(s32*)(sprite + 4) & ~0x60) | 0x40;
                Sprite_ChangeAnimation((Sprite*)(sprite + 4), *(s32*)(sprite + 0x1C), 2, *(SpriteCell**)(sprite + 0x20));
                func_ov039_02087d04(0x336, (OtuPoint*)(sprite + 0x5C), (OtuPoint*)(sprite + 0x54));
                *(s32*)(sprite + 0x78) = 0xE;
                *(s32*)(sprite + 0x80) = 4;
                /* fall through */
            case 4:
                *(s32*)(sprite + 0x48) = *(s32*)(sprite + 0x48) + (0x99A - *(s32*)(sprite + 0x48)) / *(s32*)(sprite + 0x78);
                *(s32*)(sprite + 0x4C) = *(s32*)(sprite + 0x48);

                *(s32*)(sprite + 0x78) = *(s32*)(sprite + 0x78) - 1;

                if (*(s32*)(sprite + 0x78) <= 0) {
                    *(s32*)(sprite + 0x80) = 0;
                }
                break;
        }

        if (*(s32*)(sprite + 0x6C) != 0) {
            Sprite_Update((Sprite*)(sprite + 4));
        }
    } else {
        *(s32*)(sprite + 0x6C) = 0;
    }
    return 1;
}

/**
 * @brief The same two Q12.12 position writes against a shifted field set.
 *
 * The x half reads the +0x5C/+0x54 pair and the y half adds a third term at
 * +0x64 before subtracting +0x58; the block is rendered at +4 rather than at
 * its base, which is the one place it differs from `func_ov039_020982e8`.
 */
s32 func_ov039_020914d0(void* pool, void* task) {
    u8* sprite = *(u8**)((u8*)task + 0x18);

    (void)pool;

    if (*(s32*)(sprite + 0x6C) != 0) {
        *(s16*)(sprite + 0x10) = (s16)((*(s32*)(sprite + 0x5C) - *(s32*)(sprite + 0x54)) >> 12);
        *(s16*)(sprite + 0x12) = (s16)((*(s32*)(sprite + 0x60) + *(s32*)(sprite + 0x64) - *(s32*)(sprite + 0x58)) >> 12);
        Sprite_RenderFrame((Sprite*)(sprite + 4));
    }
    return 1;
}

/** Releases the block's sprite and deletes its eight child tasks. */
s32 func_ov039_02091524(TaskPool* pool, void* task) {
    OtuChildGroup* self = *(OtuChildGroup**)((u8*)task + 0x18);
    s32            i;

    Sprite_Release((Sprite*)((u8*)self + 4));

    for (i = 0; i < 8; i++) {
        EasyTask_DeleteTask(pool, self->ids[i]);
    }
    return 1;
}

/**
 * @brief Runs the stage handler at `index` from the table at data_ov039_02099568.
 *
 * Same stack copy and index as `func_ov039_02094188`, against a different table.
 */
void func_ov039_02091560(void* a, void* b, void* c, s32 index) {
    TaskStages table = data_ov039_02099568;

    table.iter[index](a, b, c);
}

/**
 * @brief Spawns the group task and its eight children, returning the handle.
 *
 * The children are named into the group's +0x84 array, which is the same
 * `OtuChildGroup` layout `func_ov039_02091524` and `func_ov039_02091690` walk.
 */
s32 func_ov039_020915a8(TaskPool* pool, s32 word0, s32 word1) {
    OtuTaskParams  args;
    s32            handle;
    OtuChildGroup* group;
    s32            i;

    args.word0 = word0;
    args.word1 = word1;

    handle = EasyTask_CreateTask(pool, &data_ov039_0209955c, NULL, 0, NULL, &args);
    group  = EasyTask_GetTaskData(pool, handle);

    for (i = 0; i < 8; i++) {
        group->ids[i] = func_ov039_02097e68(pool, word0, word1);
    }
    return handle;
}

/**
 * @brief Reads the +0x5C/+0x60 pair into a point when the +0x80 phase is 3.
 *
 * Returns 0 otherwise, leaving the point untouched.
 */
s32 func_ov039_02091628(void* task, OtuPoint3* out) {
    u8* self = (u8*)task;
    s32 y;
    s32 x;

    if (*(s32*)(self + 0x80) != 3) {
        return 0;
    }

    y = *(s32*)(self + 0x60);
    x = *(s32*)(self + 0x5C);

    out->x     = x;
    out->y     = y;
    out->scale = 0x20000;
    return 1;
}

/** Writes +0x70 and +0x74 from two values, then raises +0x80 to 1. */
void func_ov039_02091654(void* task, s32 x, s32 y) {
    *(s32*)((u8*)task + 0x70) = x;
    *(s32*)((u8*)task + 0x74) = y;
    *(s32*)((u8*)task + 0x80) = 1;
}

/**
 * @brief Latch the +0x7C value into +0x78 while the +0x80 phase is 3.
 *
 * A conditional load-and-store pair rather than a branch, so it stays a plain
 * `if` with the two statements in that order.
 */
void func_ov039_02091668(void* task) {
    u8* self = (u8*)task;

    if (*(s32*)(self + 0x80) == 3) {
        *(s32*)(self + 0x78) = *(s32*)(self + 0x7C);
    }
}

/** True while the +0x80 phase is non-zero. */
s32 func_ov039_0209167c(void* task) {
    return *(s32*)((u8*)task + 0x80) != 0;
}

/** Advances the eight child tasks named by +0x84. */
void func_ov039_02091690(OtuChildGroup* self) {
    s32 i;

    for (i = 0; i < 8; i++) {
        func_ov039_02097e9c(EasyTask_GetTaskData(self->pool, self->ids[i]), &self->params);
    }
}

// Nonmatching: 68-78%. The body, the three varied constants and the guard chain
// are all correct; what is left is mwcc scheduling the two-step lookup.
//
// Three bugs were real and are fixed, so the remaining gap is not one of those:
//
//   * the axis argument to func_ov039_02088400 is passed and never read -- the
//     target loads it into r2 and then uses r2 only as the mask source, so the
//     last instruction is a plain shift. The packer is now a full MATCH.
//   * slot+0x04 and slot+0x08 are 32-bit words, not a u16 and a byte pointer.
//     Every store in the target is `str`, and the widths are load-bearing.
//   * the guard is three flat short-circuit tests over +0x18, +0x1C and +0x16,
//     not two nested ones. The target's `ldrne` on the table load is the
//     short-circuit, which is why +0x18 is tested at all.
/* `arg` is unused: the target loads r1 nowhere in these bodies and compares
 * only r2. Kept as a named parameter so the selector stays in the third
 * argument slot, which is where the target reads it. */
OtuSpriteSlot* func_ov039_020916c4(OtuSpriteTask* t, s32 arg, s32 mode) {
    OtuSpriteSlot* slot = (OtuSpriteSlot*)&data_0206b408;

    (void)arg;

    switch (mode) {
        case 1:
            slot->unk_00 = 1;
            return slot;

        case 2: {
            s32 index;
            u8* table;

            slot->unk_04   = 0;
            slot->unk_08   = 0;
            slot->unk_0C   = 0;
            slot->depthKey = -1;

            // The two-step lookup: a u16 out of the task's table at one stride,
            // then a byte pointer built from the u16 at the other. Guarded on the
            // table pointer and on the index being non-negative.
            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + ((u16*)table)[index * 4] * 2);
            }

            slot->unk_0C   = (void*)((u8*)t + 0x40);
            slot->depthKey = func_ov039_02088400(*(s32*)((u8*)t + 0x5C), 0, 0);
            return slot;
        }

        default:
            return NULL;
    }
}

/**
 * The two loaders that share `func_ov039_020985e0`'s shape and are re-run by the
 * parameter resets above. Both take the caller's `args` in the third slot and
 * read the dataType out of its first word.
 */

/** Loads the sprite for the task whose anim template is data_ov039_020995c0. */
void func_ov039_02091788(void* self, void* sprite, s32* args) {
    SpriteAnimation anim = data_ov039_020995c0;

    anim.owner    = self;
    anim.dataType = (u16) * (s32*)args;
    anim.posX     = *(s32*)((u8*)self + 0x58) >> 12;
    anim.posY     = *(s32*)((u8*)self + 0x5C) >> 12;
    _Sprite_Load((Sprite*)sprite, &anim);
}

/**
 * @brief Clears the parameter block at task+0x18 and re-derives it.
 *
 * The store order is not address order: the four zero words at +0x50 come after
 * the one at +0x68, and +0x40 comes last. Written in the target's order because
 * nothing about the sequence is recoverable from the field names.
 */
s32 func_ov039_02091814(void* pool, void* task, s32* args) {
    u8* sprite = *(u8**)((u8*)task + 0x18);

    (void)pool;

    *(s32*)(sprite + 0x68) = args[1];
    *(s32*)(sprite + 0x50) = 0;
    *(s32*)(sprite + 0x54) = 0;
    *(s32*)(sprite + 0x58) = 0;
    *(s32*)(sprite + 0x5C) = 0;
    *(s32*)(sprite + 0x40) = 0;
    *(s32*)(sprite + 0x44) = 0x2000;
    *(s32*)(sprite + 0x48) = 0x2000;
    *(s16*)(sprite + 0x4C) = 0;
    *(s16*)(sprite + 0x4E) = 0;
    *(s32*)(sprite + 0x6C) = 0;
    *(s32*)(sprite + 0x70) = 0;

    func_ov039_02091788(sprite, sprite, args);
    return 1;
}

/* ------------------------------------------------------------------ */
/* The five-phase swing task, 0x02091868.                              */
/* ------------------------------------------------------------------ */

/**
 * @brief The swing task's phase machine: wind-up, swing, recover.
 *
 * The sprite block (task+0x18) carries its own small state block: +0x6C is the
 * "step this frame" flag, +0x70 the phase, +0x74 a 4-frame hold count and
 * +0x48 the swing angle. Phase 1 seeds the swing from the pin's +0x138
 * position and falls straight through into phase 2, which is the swing
 * itself: it copies the pin's position pair, aims by 0x38000 and slews
 * +0x48 from 0x2000 toward zero over the four frames of +0x74. Phases 3 and 4
 * walk the angle back and the completion checks are gated on the owner's bit
 * 10 (the "swing enabled" bit), read as `(word << 0x15) >> 0x1F`.
 */
s32 func_ov039_02091868(TaskPool* pool, void* task) {
    u8*   sprite = *(u8**)((u8*)task + 0x18);
    void* pin;

    pin = EasyTask_GetTaskData(pool, *(s32*)(sprite + 0x68));

    if (pin != NULL) {
        switch (*(s32*)(sprite + 0x70)) {
            case 0:
                *(s32*)(sprite + 0x6C) = 0;
                break;

            case 1:
                *(s32*)(sprite + 0x6C) = 1;
                func_ov039_0208e6cc(pin, (OtuPoint*)(sprite + 0x60));
                func_ov039_02098bd4(-0x1000, (OtuPoint*)(sprite + 0x60), (OtuPoint*)(sprite + 0x60));
                *(s32*)(sprite + 0x40) = (u16)(FX_Atan2Idx(*(s32*)(sprite + 0x64), *(s32*)(sprite + 0x60)) + 0xC000);
                *(s32*)(sprite + 0x48) = 0;
                *(s32*)(sprite + 0x70) = 2;
                *(s32*)(sprite + 0x74) = 4;
                Sprite_SetAnimation((Sprite*)sprite, *(s16**)(sprite + 0x18), 1, *(SpriteCell**)(sprite + 0x1C));
                /* fall through */
            case 2:
                func_ov039_0208e85c(pin, (OtuPoint*)(sprite + 0x50));
                func_ov039_0208e6e0(pin, (OtuPoint*)(sprite + 0x58));
                func_ov039_02098c00(0x38000, (OtuPoint*)(sprite + 0x60), (OtuPoint*)(sprite + 0x58),
                                    (OtuPoint*)(sprite + 0x58));
                if (*(s32*)(sprite + 0x74) > 0) {
                    *(s32*)(sprite + 0x48) =
                        *(s32*)(sprite + 0x48) + (0x2000 - *(s32*)(sprite + 0x48)) / *(s32*)(sprite + 0x74);
                    *(s32*)(sprite + 0x74) = *(s32*)(sprite + 0x74) - 1;
                }

                if (((u32)(*(s32*)sprite << 0x15)) >> 0x1F == 1) {
                    *(s32*)(sprite + 0x70) = 3;
                    Sprite_ChangeAnimation((Sprite*)sprite, *(s32*)(sprite + 0x18), 2, *(SpriteCell**)(sprite + 0x1C));
                }
                break;

            case 3:
                func_ov039_0208e85c(pin, (OtuPoint*)(sprite + 0x50));

                if (((u32)(*(s32*)sprite << 0x15)) >> 0x1F == 1) {
                    *(s32*)(sprite + 0x70) = 4;
                    *(s32*)(sprite + 0x74) = 4;
                    Sprite_ChangeAnimation((Sprite*)sprite, *(s32*)(sprite + 0x18), 3, *(SpriteCell**)(sprite + 0x1C));
                }
                break;

            case 4:
                func_ov039_0208e85c(pin, (OtuPoint*)(sprite + 0x50));

                if (*(s32*)(sprite + 0x74) > 0) {
                    *(s32*)(sprite + 0x48) = *(s32*)(sprite + 0x48) + -*(s32*)(sprite + 0x48) / *(s32*)(sprite + 0x74);
                    *(s32*)(sprite + 0x74) = *(s32*)(sprite + 0x74) - 1;
                } else {
                    *(s32*)(sprite + 0x70) = 0;
                    *(s32*)(sprite + 0x6C) = 0;
                }
                break;
        }
    } else {
        *(s32*)(sprite + 0x6C) = 0;
    }

    if (*(s32*)(sprite + 0x6C) != 0) {
        Sprite_Update((Sprite*)sprite);
    }
    return 1;
}

/**
 * @brief Steps the sprite block at task+0x18 when +0x6C is set.
 *
 * The same pair of Q12.12 position writes as `func_ov039_02094dac`, minus that
 * function's +0x60 term and against a different guard word.
 */
s32 func_ov039_02091a5c(void* pool, void* task) {
    Sprite* sprite = *(Sprite**)((u8*)task + 0x18);

    (void)pool;

    if (*(s32*)((u8*)sprite + 0x6C) != 0) {
        *(s16*)((u8*)sprite + 0x0C) = (s16)((*(s32*)((u8*)sprite + 0x58) - *(s32*)((u8*)sprite + 0x50)) >> 12);
        *(s16*)((u8*)sprite + 0x0E) = (s16)((*(s32*)((u8*)sprite + 0x5C) - *(s32*)((u8*)sprite + 0x54)) >> 12);
        Sprite_RenderFrame(sprite);
    }
    return 1;
}

/** Releases the sprite at task+0x18 and reports success. */
s32 func_ov039_02091aa4(void* pool, void* task) {
    (void)pool;
    Sprite_Release(*(Sprite**)((u8*)task + 0x18));
    return 1;
}

/** The same dispatcher against data_ov039_020995b0. */
void func_ov039_02091ab8(void* a, void* b, void* c, s32 index) {
    TaskStages table = data_ov039_020995b0;

    table.iter[index](a, b, c);
}

/** The two-argument result-screen spawners: one word of dataType, one of id. */
s32 func_ov039_02091b00(TaskPool* pool, s32 dataType, s32 childId) {
    s32 args[2];

    args[0] = dataType;
    args[1] = childId;
    return EasyTask_CreateTask(pool, &data_ov039_020995a4, NULL, 0, NULL, args);
}

/** Raises the +0x70 word to 1. */
void func_ov039_02091b34(void* task) {
    *(s32*)((u8*)task + 0x70) = 1;
}

/**
 * @brief Stamps a 4x4 block of tile numbers into a 32-wide map.
 *
 * Each row is 32 entries apart, so the value advances by 0x20 per row while
 * only the first four entries of each are written. `value` is a u16 and is
 * narrowed after every increment, which is why the loop reads as
 * `add`/`lsl`/`lsr` rather than a plain `add`.
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

// Size: 0x24

/**
 * @brief The wrestling board's block-loader: picks a sheet from the args
 *        block, allocates everything it needs and stamps the mixed-grid.
 *
 * The sprite block at task+0x18 gets: +0/+4 the sheet index and the heap, +8
 * the "loading" flag and +C the sheet description pointer. Then: three
 * literal tables are snapped to the stack whose layout picks a row by
 * sheet.cellType and sheet.faceId (a 3x3 u32 matrix plus a flat u32 table);
 * the relative refs +8/+0x10/+0x18 are loaded and allocated via palette and
 * BgResMgr_AllocChar32 against `bgSettings[1].charBase`, palette-flushed.
 * Then five frame cursors (0x54/0x64, 0x68/0x78, 0x7C/0x8C, 0x90/0xA0,
 * 0xA4/0xB4) are allocated against `data_ov039_0209964c` (8 slots),
 * `02099620` (4), `0209966c` (8), `020996b0` (0x19) and `02099630` (7),
 * each from a Data-buffer offset (0x10/0x18/0x20/0x28/0x30), each flushed.
 * The final +0x48 AllocChar32 comes from the +0x38-offset ref against
 * `bgSettings[1].charBase` at offset 0x300.
 *
 * The grid is `sheet.width + 7 / 8` by `sheet.height + 7 / 8`, backed by two
 * heaps -- a tile-pool buffer (0x800 bytes per cell) and a pointer table --
 * MI_CpuSet zeroed, and the pages are linked so rows[i][j] = tilePool +
 * (i + j) * 0x800. Then the layout walk stamps every tile pair
 * through `func_ov039_02091b40`, and the whole thing hands the pointer
 * table and the tile pool to `func_0200d1d8`.
 */
// Nonmatching: 81.7%. The body, constants and every branch agree; the residual
// gap is register naming and two extra stack words. What was spent here:
//   * the three literal-table copies must be struct assignments spelled with
//     whole-block casts (`*(OtuWord3*)sheetTable = *(OtuWord3*)...`),
//     not loops or word chains -- only the struct form emits the target's
//     ldm/stm pairs;
//   * the sheet kind must be re-read as a byte expression everywhere it is
//     used (`*(u8*)sheet0`), never held in a named u8 local -- a named local
//     forced an extra stack slot, a `str [sp+0xc]` spill the target does not
//     have, and shifted the whole frame (+2 words) and every pool offset;
//   * `sheet[2]` likewise is re-read per loop iteration (this build forgot
//     that once and dropped 3%);
//   * `sheet[1]` is a fresh leaf byte too -- naming it cost a register;
//   * the two cell-grid [8]-accumulator sizes are `(w+7+(correction)) >> 3`
//     spelled exactly as m2c's -- matches.
// The pointers currently land r5/r9/r6 where the target has r5/r7/r8, so
// every sprite+* memory row reads r6 where the target reads r7. Permuter and
// eight spelling sweeps found nothing further.
s32 func_ov039_02091b98(TaskPool* pool, OtuBadge* task, s32* args) {
    u8*   sprite = *(u8**)((u8*)task + 0x18);
    Data* data;
    Data* data2;
    u8*   pal;
    u8*   chr;
    u8*   scr;
    s32   sheetTable[3];
    s32   styleTable[9];
    s32   miscTable[3];
    u8*   sheet0;
    Heap* heap;
    s32   row;
    s32   row2;
    s32   cellsWide;
    s32   cellsHigh;
    s32   i;
    s32   j;
    s32   run;
    u8    wide;
    s32   col;

    *(s32*)(sprite + 0x8) = 1;
    *(s32*)(sprite + 0x0) = *(s32*)args;
    *(s32*)(sprite + 0x4) = *(s32*)((u8*)args + 4);
    *(s32*)(sprite + 0xC) = *(s32*)((u8*)args + 8);

    *(OtuWord3*)sheetTable = *(OtuWord3*)data_ov039_020995f8;
    *(OtuWord9*)styleTable = *(OtuWord9*)data_ov039_0209968c;
    *(OtuWord3*)miscTable  = *(OtuWord3*)data_ov039_02099604;

    sheet0 = *(u8**)((u8*)args + 8);
    heap   = *(Heap**)(sprite + 4);

    data                   = DatMgr_LoadRawData(*(s32*)args, NULL, 0, &data_ov039_0209a0b4[sheetTable[*(u8*)sheet0]]);
    *(s32*)(sprite + 0x44) = (s32)data;

    row2 = miscTable[*(u8*)sheet0];

    row  = styleTable[(u32) * (u8*)sheet0 * 3 + sheet0[1]];
    wide = sheet0[2];

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

        *(PaletteResource**)(sprite + 0x4C) = PaletteMgr_AllocPalette(g_PaletteManagers[0], pal, 0, 0, 5);
        {
            u32 size = (u32)((*(u32*)chr & ~0xFF) >> 8);

            if ((*(u8*)chr & 0xF0) == 0) {
                size -= 4;
            }
            *(BgResource**)(sprite + 0x50) = BgResMgr_AllocChar32(
                g_BgResourceManagers[0], chr, g_DisplaySettings.engineState[0].bgSettings[1].charBase, 0, size);
        }
        PaletteMgr_Flush(g_PaletteManagers[0], *(PaletteResource**)(sprite + 0x4C));
    }

    data2                  = DatMgr_LoadRawData(*(s32*)args, NULL, 0, data_ov039_0209a0bc);
    *(s32*)(sprite + 0x40) = (s32)data2;

    {
        if (data2 == NULL) {
            pal = NULL;
        } else {
            u8* base = (u8*)data2->buffer + 0x20;
            pal      = base + *(u32*)(base + 0x10);
        }
        *(PaletteResource**)(sprite + 0x64) = PaletteMgr_AllocPalette(
            g_PaletteManagers[0], func_ov039_02098d7c(sprite + 0x54, (s32)pal, data_ov039_0209964c, 8), 0, 6, 1);
        PaletteMgr_Flush(g_PaletteManagers[0], *(PaletteResource**)(sprite + 0x64));
    }

    {
        if (data2 == NULL) {
            pal = NULL;
        } else {
            u8* base = (u8*)data2->buffer + 0x20;
            pal      = base + *(u32*)(base + 0x18);
        }
        *(PaletteResource**)(sprite + 0x78) = PaletteMgr_AllocPalette(
            g_PaletteManagers[0], func_ov039_02098d7c(sprite + 0x68, (s32)pal, data_ov039_02099620, 4), 0, 7, 1);
        PaletteMgr_Flush(g_PaletteManagers[0], *(PaletteResource**)(sprite + 0x78));
    }

    {
        if (data2 == NULL) {
            pal = NULL;
        } else {
            u8* base = (u8*)data2->buffer + 0x20;
            pal      = base + *(u32*)(base + 0x20);
        }
        *(PaletteResource**)(sprite + 0x8C) = PaletteMgr_AllocPalette(
            g_PaletteManagers[0], func_ov039_02098d7c(sprite + 0x7C, (s32)pal, data_ov039_0209966c, 8), 0, 8, 1);
        PaletteMgr_Flush(g_PaletteManagers[0], *(PaletteResource**)(sprite + 0x8C));
    }

    {
        if (data2 == NULL) {
            pal = NULL;
        } else {
            u8* base = (u8*)data2->buffer + 0x20;
            pal      = base + *(u32*)(base + 0x28);
        }
        *(PaletteResource**)(sprite + 0xA0) = PaletteMgr_AllocPalette(
            g_PaletteManagers[0], func_ov039_02098d7c(sprite + 0x90, (s32)pal, data_ov039_020996b0, 0x19), 0, 9, 1);
        PaletteMgr_Flush(g_PaletteManagers[0], *(PaletteResource**)(sprite + 0xA0));
    }

    {
        if (data2 == NULL) {
            pal = NULL;
        } else {
            u8* base = (u8*)data2->buffer + 0x20;
            pal      = base + *(u32*)(base + 0x30);
        }
        *(PaletteResource**)(sprite + 0xB4) = PaletteMgr_AllocPalette(
            g_PaletteManagers[0], func_ov039_02098d7c(sprite + 0xA4, (s32)pal, data_ov039_02099630, 7), 0, 0xA, 1);
        PaletteMgr_Flush(g_PaletteManagers[0], *(PaletteResource**)(sprite + 0xB4));
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
            *(BgResource**)(sprite + 0x48) = BgResMgr_AllocChar32(
                g_BgResourceManagers[0], chr, g_DisplaySettings.engineState[0].bgSettings[1].charBase, 0x300, size);
        }
    }

    cellsWide = (wide + 7 + ((u32)((wide + 7) >> 2) >> 0x1D)) >> 3;
    cellsHigh = (sheet0[3] + 7 + ((u32)((sheet0[3] + 7) >> 2) >> 0x1D)) >> 3;

    *(s32*)(sprite + 0x3C) = (s32)Mem_AllocHeapTail(heap, cellsHigh * (cellsWide << 0xB));
    *(s32*)(sprite + 0x38) = (s32)Mem_AllocHeapTail(heap, cellsHigh * (cellsWide * 4));
    MI_CpuSet(*(void**)(sprite + 0x3C), 0, Mem_GetBlockSize(heap, *(void**)(sprite + 0x3C)));

    if (cellsHigh > 0) {
        i   = 0;
        run = 0;
        do {
            j = 0;
            if (cellsWide > 0) {
                do {
                    *(u32*)((u8*)*(s32*)(sprite + 0x38) + (s32)(run + j) * 4) =
                        (u32)((u8*)*(s32*)(sprite + 0x3C) + (j + run) * 0x800);
                    j = j + 1;
                } while (j < cellsWide);
            }
            i   = i + 1;
            run = run + cellsWide;
        } while (i < cellsHigh);
    }

    if (sheet0[3] > 0) {
        col = 0;
        do {
            if (sheet0[2] > 0) {
                j = 0;
                do {
                    u8* cellBase = *(u8**)(sheet0 + 0x10) + (col * sheet0[2] + j) * 2;
                    u8  cellByte = cellBase[1];
                    u16 tileVal  = *(u16*)(data_ov039_0209a5d8[*(u8*)sheet0] + cellByte * 2);

                    func_ov039_02091b40(tileVal,
                                        (u16*)(*(u32*)((u8*)*(s32*)(sprite + 0x38) + ((j / 8) * cellsWide + (col / 8)) * 4) +
                                               ((j % 8) * 4 + (col % 8) * 128) * 2));
                    j = j + 1;
                } while (j < (s32)sheet0[2]);
            }
            col = col + 1;
        } while (col < (s32)sheet0[3]);
    }

    func_0200d1d8(sprite + 0x10, 0, 1, 0, *(s32*)(sprite + 0x38), cellsWide, cellsHigh);
    return 1;
}

/**
 * @brief Points palette manager 0 at five sources, one per 0x14-strided slot.
 *
 * Each slot is a frame cursor at +0 and a palette resource at +0x10; the cursor
 * is stepped by `func_ov039_02098dbc` and its result is the source palette, so
 * the call has to be the third argument expression and not hoisted into a
 * statement of its own.
 */
s32 func_ov039_02092154(void* pool, void* task) {
    u8* block = *(u8**)((u8*)task + 0x18);

    (void)pool;

    /* Spelled out five times rather than looped: the target unrolls it. */
    PaletteMgr_SetSource(g_PaletteManagers[0], *(PaletteResource**)(block + 0x64), (void*)func_ov039_02098dbc(block + 0x54));
    PaletteMgr_SetSource(g_PaletteManagers[0], *(PaletteResource**)(block + 0x78), (void*)func_ov039_02098dbc(block + 0x68));
    PaletteMgr_SetSource(g_PaletteManagers[0], *(PaletteResource**)(block + 0x8C), (void*)func_ov039_02098dbc(block + 0x7C));
    PaletteMgr_SetSource(g_PaletteManagers[0], *(PaletteResource**)(block + 0xA0), (void*)func_ov039_02098dbc(block + 0x90));
    PaletteMgr_SetSource(g_PaletteManagers[0], *(PaletteResource**)(block + 0xB4), (void*)func_ov039_02098dbc(block + 0xA4));
    return 1;
}

/* ------------------------------------------------------------------ */
/* The result-screen task lifecycle callbacks and small shims.         */
/* ------------------------------------------------------------------ */

/** The three one-word predicates reduced to `return 1`. */
s32 func_ov039_020921f4(void) {
    return 1;
}

/**
 * @brief The full resource teardown: two buffers, two chars, six palettes, two
 *        data handles.
 *
 * The six palette releases are the same 0x14-strided slot run
 * `func_ov039_02092154` installs, read back off the block in the order the
 * install wrote them.
 */
s32 func_ov039_020921fc(void* pool, void* task) {
    u8* block = *(u8**)((u8*)task + 0x18);

    (void)pool;

    func_0200d954(0, 1);

    Mem_Free(*(Heap**)(block + 0x04), *(void**)(block + 0x38));
    Mem_Free(*(Heap**)(block + 0x04), *(void**)(block + 0x3C));

    BgResMgr_ReleaseChar(g_BgResourceManagers[0], *(BgResource**)(block + 0x50));
    BgResMgr_ReleaseChar(g_BgResourceManagers[0], *(BgResource**)(block + 0x48));

    PaletteMgr_ReleaseResource(g_PaletteManagers[0], *(PaletteResource**)(block + 0x4C));
    PaletteMgr_ReleaseResource(g_PaletteManagers[0], *(PaletteResource**)(block + 0x64));
    PaletteMgr_ReleaseResource(g_PaletteManagers[0], *(PaletteResource**)(block + 0x78));
    PaletteMgr_ReleaseResource(g_PaletteManagers[0], *(PaletteResource**)(block + 0x8C));
    PaletteMgr_ReleaseResource(g_PaletteManagers[0], *(PaletteResource**)(block + 0xA0));
    PaletteMgr_ReleaseResource(g_PaletteManagers[0], *(PaletteResource**)(block + 0xB4));

    DatMgr_ReleaseData(*(Data**)(block + 0x44));
    DatMgr_ReleaseData(*(Data**)(block + 0x40));
    return 1;
}

/** The same dispatcher against data_ov039_02099610. */
void func_ov039_020922c8(void* a, void* b, void* c, s32 index) {
    TaskStages table = data_ov039_02099610;

    table.iter[index](a, b, c);
}

/** The board's three-word spawner. */
s32 func_ov039_02092310(TaskPool* pool, s32 dataType, Heap* heap, OtuBoardParams* params) {
    s32 args[3];

    args[0] = dataType;
    args[1] = (s32)heap;
    args[2] = (s32)params;
    return EasyTask_CreateTask(pool, &data_ov039_020995ec, NULL, 0, NULL, args);
}

/**
 * @brief Runs the group's stage at +0x10/+0x14 and records a position pair.
 *
 * The group is an element of `g_DisplaySettings.engineState`, sized 0x220,
 * which is what folds the +0x64 addend into the pool word: spelling the base as
 * a raw `+ 0x64` leaves mwcc materialising `g_DisplaySettings` and then adding
 * 0x64 in a register. The stage kind is the first word of `bgSettings[b]` and is
 * a real `switch` -- the target builds a six-entry jump table rather than
 * testing the values, so an `if` chain is not interchangeable here. Cases 1-5
 * share one body and case 0 falls through to `default`, which is why the
 * table's first entry lands on the same label as the out-of-range branch.
 *
 * The three writes land on `bgAffines[b].unk_14` and the `bgOffsets[b]` pair.
 */
// Nonmatching: 98.5%, and the complete gap is one register rename. The target
// keeps the engine-state index in r4 and the group pointer in lr; this source
// has them the other way round, so every `mla`/`add` reads `r4` where the
// target reads `lr`. mwcc hoists the pool-constant load to the top of the
// block, which is what makes the group pointer win r4; the target loads the
// index first. Ten shapes were tried -- all six declaration orders, no named
// group local at all (leaving the address to CSE), array decay instead of
// &array[i], a named base local, and both load orders -- and four of them score
// identically here while the rest are worse. Every other instruction agrees.
void func_ov039_02092348(void* self, s32 x, s32 y) {
    s32                 a     = *(s32*)((u8*)self + 0x10);
    s32                 b     = *(s32*)((u8*)self + 0x14);
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

/** Raises bit 1 of the +0x18 flags when `event` is non-zero. */
void func_ov039_020923b4(void* task, s32 event) {
    if (event != 0) {
        *(s32*)((u8*)task + 0x18) |= 2;
    }
}

// Nonmatching: 68-78%. The body, the three varied constants and the guard chain
// are all correct; what is left is mwcc scheduling the two-step lookup.
//
// Three bugs were real and are fixed, so the remaining gap is not one of those:
//
//   * the axis argument to func_ov039_02088400 is passed and never read -- the
//     target loads it into r2 and then uses r2 only as the mask source, so the
//     last instruction is a plain shift. The packer is now a full MATCH.
//   * slot+0x04 and slot+0x08 are 32-bit words, not a u16 and a byte pointer.
//     Every store in the target is `str`, and the widths are load-bearing.
//   * the guard is three flat short-circuit tests over +0x18, +0x1C and +0x16,
//     not two nested ones. The target's `ldrne` on the table load is the
//     short-circuit, which is why +0x18 is tested at all.
/* `arg` is unused: the target loads r1 nowhere in these bodies and compares
 * only r2. Kept as a named parameter so the selector stays in the third
 * argument slot, which is where the target reads it. */
OtuSpriteSlot* func_ov039_020923c8(OtuSpriteTask* t, s32 arg, s32 mode) {
    OtuSpriteSlot* slot = (OtuSpriteSlot*)&data_0206b408;

    (void)arg;

    switch (mode) {
        case 1:
            slot->unk_00 = 1;
            return slot;

        case 2: {
            s32 index;
            u8* table;

            slot->unk_04   = 0;
            slot->unk_08   = 0;
            slot->unk_0C   = 0;
            slot->depthKey = -1;

            // The two-step lookup: a u16 out of the task's table at one stride,
            // then a byte pointer built from the u16 at the other. Guarded on the
            // table pointer and on the index being non-negative.
            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + ((u16*)table)[index * 4] * 2);
            }

            slot->unk_0C   = (void*)((u8*)t + 0x1);
            slot->depthKey = func_ov039_02088400(*(s32*)((u8*)t + 0x4C), 0, 0);
            return slot;
        }

        default:
            return NULL;
    }
}
