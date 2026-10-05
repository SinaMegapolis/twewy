#include "OtuFieldAccessShared.h"

/*
 * ov039 region 0x0208e890 - 0x0208f00c. One translation unit of the
 * overlay; dsd gives each file a single contiguous `.text` claim. The
 * shared types, externs and prototypes are in OtuFieldAccessShared.h.
 */
/* ============================================================================
 * Band 14 -- the rest of OtuBadge's field accessors.
 *
 * The seven remaining functions in the run that follows band 12, from 0x0208e890
 * to 0x0208e9e4. The seven below them -- 0208e6cc, _6e0, _6f4, _848, _85c, _870
 * and _87c -- are already in OtuFieldAccess.c itself, so this file does not
 * repeat them. Every body here is a handful of instructions copied straight from
 * the target, and where the target used a jump table this keeps the switch so
 * mwcc emits one too.
 *
 * Two signature styles appear here because both already exist for these
 * functions, in bands 1, 7, 13 and the shared header, and the earlier
 * declaration has to be the one the definition agrees with:
 *
 *   OtuBadge*  the header's trimmed +0x174-byte view of the same object,
 *                modelling only the fields the nearest-child queries read.
 *   void*        what band 1 and band 7 happen to pass around.
 *
 * OtuBadge has no field for +0x0DC or +0x0FC, so those two read through
 * OtuBadge, which band 12 declares and which models the whole 0x25C bytes.
 * Every offset OtuBadge does model agrees with OtuBadge, so the cast is
 * always safe.
 *
 * Offsets: +0x0DC unk_0DC, +0x0F8 kind, +0x0FC subKind, +0x138 dir.
 * =========================================================================*/

/**
 * @brief Only kinds 0, 2, 3 and 4 answer with zero; everything else -- kind 1,
 *        and any value past the table -- answers with +0x0DC.
 *
 *  The target reaches its five entries with `cmp r1, #4 / addls pc, pc, r1,
 *  lsl #2`, defaulting to the load, so this stays a switch with one arm per
 *  case value rather than a chain of equality tests.
 */
s32 func_ov039_0208e890(OtuBadge* self) {

    switch (self->phase) {
        case 0:
        case 2:
        case 3:
        case 4:
            return 0;

        case 1:
        default:
            break;
    }

    return self->visible;
}

/**
 * @brief The pin child's aim scale, 0x1000 when nothing applies.
 *
 * Phase 8, sub-kind 1: while the +0x100 cursor is under 10 this is the cursor
 * over 10, on a 0x1000 scale.
 *
 * Phase 8, sub-kind 4: the cursor at +0x100 against half the cell's size word,
 * counted down from 0x1000.
 */
s32 func_ov039_0208e8c4(OtuBadge* self) {
    s32 scale = 0x1000;

    if (self->phase == 8) {
        switch (self->subKind) {
            case 1: {
                s32 cursor = self->frameBudget;

                if (cursor < 0xA) {
                    scale = FX_Divide(cursor << 12, 0xA000);
                }
                break;
            }

            case 4: {
                u32 size = self->slots[*self->pinID].meteoSquash;

                if (self->frameBudget < (s32)(size >> 1)) {
                    scale = 0x1000 - FX_Divide(self->frameBudget << 12, (s32)(size >> 1) << 12);
                }
                break;
            }

            default:
                break;
        }
    }

    return scale;
}

/**
 * @brief A three-valued answer, but only for phase 8: subkind 2 answers 2,
 *        subkind 3 answers 3, and every other combination answers 0.
 *
 *  The `bne` past both tests is why this is a nested if rather than a second
 *  switch -- the subkind is never read unless the kind already matched.
 */
s32 func_ov039_0208e950(OtuBadge* self) {
    s32 r = 0;

    if (self->phase == 8) {
        /* A switch, not two `if`s: with two `if`s mwcc selects both arms with
         * `moveq`, while the target branches to an out-of-line `mov r2, #2` for
         * the first and only then tests for 3 with a conditional move. The arms
         * need their own labels to come out that way. */
        switch (self->subKind) {
            case 2:
                r = 2;
                break;

            case 3:
                r = 3;
                break;
        }
    }

    return r;
}

/**
 * @brief True when the pin's kind is 8.
 *
 *  Written as a local plus a select, not as `return task->phase == 8;`. The
 *  target copies the field into a register, zeroes the result and selects into
 *  it -- `ldr r1, [r0, #0xf8] / mov r0, #0 / cmp r1, #0x8 / moveq r0, #1` --
 *  where a bare comparison compiles to a conditional move against the loaded
 *  field. Same answer, different shape, 58% against 100%.
 */
s32 func_ov039_0208e984(OtuBadge* task) {
    s32 found = task->phase;
    s32 r     = 0;

    if (found == 8) {
        r = 1;
    }

    return r;
}

/**
 * @brief True when the pin's kind is 7.
 *
 *  Written as a local plus a select, not as `return task->phase == 7;`. The
 *  target copies the field into a register, zeroes the result and selects into
 *  it -- `ldr r1, [r0, #0xf8] / mov r0, #0 / cmp r1, #0x7 / moveq r0, #1` --
 *  where a bare comparison compiles to a conditional move against the loaded
 *  field. Same answer, different shape, 58% against 100%.
 */
s32 func_ov039_0208e998(OtuBadge* task) {
    s32 found = task->phase;
    s32 r     = 0;

    if (found == 7) {
        r = 1;
    }

    return r;
}

/**
 * @brief The heading of -dir, as an unsigned 16-bit index.
 *
 *  Both components are negated before the call rather than the result being
 *  negated after it, and the answer is cut to sixteen bits with
 *  `lsl #0x10 / lsr #0x10`. That pair is a zero-extend, so the narrowing is to
 *  `u16` and not to `s16` -- an `s16` emits `asr` for the second shift and costs
 *  the match. The return type stays `s32` because band 7 declares it that way
 *  and uses the value.
 */
s32 func_ov039_0208e9ac(OtuBadge* self) {
    u16 a;
    s32 x = -self->dir.x;
    s32 y = -self->dir.y;

    /* y first, x second -- that is the callee's own parameter order
     * (`u16 FX_Atan2Idx(s32 y, s32 x)`), and mwcc evaluates arguments in
     * reverse, which is why the target loads +0x138 into r1 and +0x13C into r2
     * even though r1 is the first argument slot.
     *
     * Nonmatching: the target keeps the call and then narrows with
     * `lsl #0x10 / lsr #0x10`, so its FX_Atan2Idx must have been visible as
     * returning something wider than u16 -- an implicit int. fx_atan.h declares
     * it as u16, so mwcc here proves the narrowing redundant and turns the whole
     * thing into a tail call through a veneer, dropping the mask. Recovering the
     * last 60% means hiding that prototype from this translation unit, which
     * would cost bands 3 and 11 their own narrowing. Not worth it.
     */
    a = FX_Atan2Idx(y, x);

    return a;
}

/**
 * @brief True when the pin's kind is 6.
 *
 *  Written as a local plus a select, not as `return task->phase == 6;`. The
 *  target copies the field into a register, zeroes the result and selects into
 *  it -- `ldr r1, [r0, #0xf8] / mov r0, #0 / cmp r1, #0x6 / moveq r0, #1` --
 *  where a bare comparison compiles to a conditional move against the loaded
 *  field. Same answer, different shape, 58% against 100%.
 */
s32 func_ov039_0208e9d0(OtuBadge* task) {
    s32 found = task->phase;
    s32 r     = 0;

    if (found == 6) {
        r = 1;
    }

    return r;
}

/**
 * @brief True when the pin's kind is 9.
 *
 *  Written as a local plus a select, not as `return task->phase == 9;`. The
 *  target copies the field into a register, zeroes the result and selects into
 *  it -- `ldr r1, [r0, #0xf8] / mov r0, #0 / cmp r1, #0x9 / moveq r0, #1` --
 *  where a bare comparison compiles to a conditional move against the loaded
 *  field. Same answer, different shape, 58% against 100%.
 */
s32 func_ov039_0208e9e4(OtuBadge* task) {
    s32 found = task->phase;
    s32 r     = 0;

    if (found == 9) {
        r = 1;
    }

    return r;
}

/**
 * @brief Runs the pin's current phase handler and reports whether it claimed.
 *
 * The three cases are the three phase handlers at +0xF8: 6 takes the read from
 * `func_ov039_02091628`, 7 the one from `func_ov039_02090e9c`, and 8 the
 * completion from `func_ov039_0208fee0` -- all three writing the same
 * three-word record at +0x184 and stashing their result in +0x180. Cases 6 and
 * 7 set the return value only when the handler reports a positive count;
 * case 8 claims unconditionally but only when +0xFC is 5 and +0x100 is 0x27.
 */
// Nonmatching: 97.4%, three instructions. The target branches past the body
// twice (`bne`/`bne`) for the +0xFC/+0x100 pair; this source has mwcc
// if-convert the pair into a conditional load (`ldreq r1, [r5, #0x100]`) and a
// single `cmpeq`. Three spellings were tried -- a positive `&&`, nested `if`s,
// a negated `||`, and two sequential guards each with its own `break` -- and
// all four compile to the same folded form. mwcc is choosing conditional
// execution over branching here and there is no source shape that changes that.
s32 func_ov039_0208e9f8(TaskPool* pool, OtuBadge* self) {
    s32 claimed = 0;
    s32 ret;

    switch (self->phase) {
        case 6:
            ret            = func_ov039_02091628(EasyTask_GetTaskData(pool, self->needleId), self->hits);
            self->hitCount = ret;
            if (ret > 0) {
                claimed = 1;
            }
            break;

        case 7:
            ret            = func_ov039_02090e9c(EasyTask_GetTaskData(pool, self->hammerId), self->hits);
            self->hitCount = ret;
            if (ret > 0) {
                claimed = 1;
            }
            break;

        case 8:
            /* Two sequential guards with their own `break`, not one combined
             * condition: `&&`, nested `if`s and a negated `||` all fold the
             * pair into `ldreq`/`cmpeq`, where the target branches past the
             * body twice. */
            if (self->subKind != 5) {
                break;
            }
            if (self->frameBudget != 0x27) {
                break;
            }

            claimed = 1;
            func_ov039_0208fee0(EasyTask_GetTaskData(pool, self->meteoId), self->hits);
            self->hitCount = claimed;
            break;

        default:
            break;
    }
    return claimed;
}

/* ------------------------------------------------------------------ */
/* The badge clash wind-up, 0x0208eaa0.                                */
/* ------------------------------------------------------------------ */

/** The Q12 scale word and the 0x10-stride per-speed factor table. */
// extern s32 data_ov039_0209a398;   (already declared in TinPinSlammer.h)
// extern OtuSpeedEntry data_ov039_0209a3dc[];  (declared in OtuPinLogic)

/**
 * @brief Resolves whatever the badge found to clash against and starts the
 *        strike.
 *
 * Gate: +0x128 clear and +0x168 not positive, and the badge's mock mode
 * (+0xF8) one of 1/6/7 or, for mode 8, only while the +0xFC phase is 0 (or
 * past the range). Then for every 0xC-stride entry in the caller's object:
 * the candidate must be within the entry's +0x18C rank plus 0xC000, and the
 * clash sequence runs:
 *
 *   mode 6  fires the latch (mode 0x337), re-raises the 0x130-"no pin" shadow
 *           reset, resolves the contact with d3c (or a unit vector if the
 *           points coincide),
 *   mode 7  drops the +0x4000 phase byte of the active cursor, leaves
 *           `func_ov039_0209104c` to hold it, then starts a directional smoke
 *           pass from the sin/cos table (mode 0x33B) and steps its children
 *           while the badge's kind is still 6,
 *   mode 8  drops the airborne child task, mode 0x333, the contact again, and
 *           the tail that only mode 8 spares.
 * The tail (shared by 6/7/8): the badge's wind-up counter at +0x148 and kind
 * reset (mode 1) with `func_ov039_0208f7a4`, the position delta saved into
 * +0x138/+0x13C, and the two-way "clash partner" handoff at +0x1B0.
 */
// Nonmatching: 89.6%, scheduling and register naming only: the contact
// block's loads, the order of the two pool words, and which callee-saved
// registers the hit walk lands in. (Walking the hits through a raw byte pointer
// to the badge scored 89.9%, through register naming alone.)
void func_ov039_0208eaa0(OtuBadge* other, OtuBadge* self) {

    if (self->height != 0) {
        return;
    }
    if (self->flags > 0) {
        return;
    }

    switch (self->phase) {
        /* m2c's label order: default first (it joins the big block), then the
         * modes that share the body, then mode 8's phase gate, then the
         * modes that return. */
        default:
        case 1:
        case 6:
        case 7:
            break;

        case 8:
            switch (self->subKind) {
                default:
                case 0:
                    break;

                case 1:
                case 2:
                case 3:
                case 4:
                    return;
            }
            break;

        case 0:
        case 2:
        case 3:
        case 4:
        case 5:
        case 9:
            return;
    }

    if (other->hitCount <= 0) {
        return;
    }
    {
        s32 i = 0;

        do {
            s32 limit = other->hits[i].scale + 0xC000;

            if (func_ov039_02098ca8(&self->pos, (OtuPoint*)&other->hits[i]) >= limit) {
                goto next;
            }

            switch (other->phase) {
                case 6: {
                    func_ov039_02091668(EasyTask_GetTaskData(other->pool, other->needleId));
                    func_ov039_02087d04(0x337, &other->pos, &other->origin);

                    if (self->stun <= 0) {
                        if (*self->pinID < 0x130) {
                            func_ov039_0208a490(other, 1);
                        }
                    }

                    self->vel.x = self->pos.x - other->pos.x;
                    self->vel.y = self->pos.y - other->pos.y;

                    if (self->vel.x != 0 || self->vel.y != 0) {
                        func_ov039_02098d3c(&self->vel, &self->vel);
                    } else {
                        self->vel.x = 0x1000;
                        self->vel.y = 0;
                    }
                    break;
                }

                case 7: {
                    void* cand   = EasyTask_GetTaskData(other->pool, other->hammerId);
                    u16   cursor = (u16)(func_ov039_02091060(cand) - 0x4000);
                    s32   pair   = (cursor >> 4) * 2;

                    func_ov039_0209104c(cand);

                    self->vel.x = ((s16*)data_0205e4e0)[pair + 1];
                    self->vel.y = ((s16*)data_0205e4e0)[pair];

                    if (self->phase == 6) {
                        func_ov039_02091690(EasyTask_GetTaskData(self->pool, self->needleId));
                    }
                    func_ov039_02087d04(0x33B, &other->pos, &other->origin);

                    if (self->stun <= 0) {
                        if (*self->pinID < 0x130) {
                            func_ov039_0208a490(other, 1);
                        }
                    }
                    break;
                }

                case 8: {
                    if (self->phase == 7) {
                        func_ov039_02091070(EasyTask_GetTaskData(self->pool, self->hammerId));
                    }
                    func_ov039_02087d04(0x333, &other->pos, &other->origin);

                    if (self->stun <= 0) {
                        if (*self->pinID < 0x130) {
                            func_ov039_0208a490(other, 1);
                        }
                    }

                    self->vel.x = self->pos.x - other->pos.x;
                    self->vel.y = self->pos.y - other->pos.y;

                    if (self->vel.x != 0 || self->vel.y != 0) {
                        func_ov039_02098d3c(&self->vel, &self->vel);
                    } else {
                        self->vel.x = 0x1000;
                        self->vel.y = 0;
                    }
                    break;
                }

                default:
                    break;
            }

            if (self->stun <= 0) {
                self->stun  = self->slots[*self->pinID].stunFrames;
                self->phase = 1;
                self->step  = 0;
                func_ov039_0208f7a4(EasyTask_GetTaskData(self->pool, self->piyoId), self->stun);
            }
            {
                OtuPoint  start;
                OtuPoint* base   = &self->vel;
                s32       factor = data_ov039_0209a3dc[self->slots[*self->pinID].tuneIndex].power;
                s32       scale;

                start.x = 0;
                scale   = (s32)(((s64)data_ov039_0209a398 * factor + 0x800) >> 0xC);
                start.y = 0;

                self->dir.x = self->vel.x;
                self->dir.y = self->vel.y;

                func_ov039_02098c00(scale, base, &start, base);
                other->partner = self;
                self->partner  = other;
            }

        next:
            i = i + 1;
        } while (i < other->hitCount);
    }
}

/* ------------------------------------------------------------------ */
/* The pin task's own queries, 0x0208ee84 - 0x0208efb0.                */
/* ------------------------------------------------------------------ */

/**
 * @brief True while the pin's "alive" counter at +0x148 is positive.
 *
 * `movgt`/`movle`, not `movne`/`moveq`: the comparison is signed, so this is
 * `> 0` and not `!= 0`. Written as a select into zero, the shape that gives
 * this overlay's predicates their two conditional moves.
 */
s32 func_ov039_0208ee84(OtuBadge* task) {
    return task->stun > 0;
}

/** The badge's remaining stun frames. */
s32 func_ov039_0208ee98(OtuBadge* task) {
    return (task)->stun;
}

/* ------------------------------------------------------------------ */
/* Four consecutive s16s at base + 0x100 + 0x78.                        */
/* ------------------------------------------------------------------ */

/*
 * These four read 0x78, 0x7A, 0x7C and 0x7E off a base already offset by 0x100,
 * so they are four consecutive s16 fields -- `OtuBadge.tile0`..`tile3`, which sit
 * at 0x178. Note the offset was *not* spelled as `0x100 + 0x78`: the split is
 * what makes the target emit a separate `add r0, r0, #0x100` before the `ldrsh`,
 * and naming the field folds it into a single `ldrsh [r0, #0x178]` that this
 * build does not produce. Naming the field is still codegen-neutral -- mwcc keeps
 * the add -- so the accessors read as members and the note records why.
 */

s16 func_ov039_0208eea0(OtuBadge* task) {
    return (task)->trackFrames;
}

s16 func_ov039_0208eeac(OtuBadge* task) {
    return (task)->bounceTimer;
}

s16 func_ov039_0208eeb8(OtuBadge* task) {
    return (task)->arcFrames;
}

s16 func_ov039_0208eec4(OtuBadge* task) {
    return (task)->spinFrames;
}

/**
 * @brief The pin's three-way phase test, read off +0x128/+0xF8/+0xF4.
 *
 * Answers 1 only when +0x128 is zero and both +0xF8 and +0xF4 are 1 *and* the
 * distance between the points at +0x14C and +0x154 is at least 0x10000. The
 * target's `ldreq`/`cmpeq` run of conditional loads is mwcc folding that
 * five-term short-circuit chain into one compare chain.
 */
s32 func_ov039_0208eed0(OtuBadge* self) {
    s32 r = 0;

    if (self->height == 0 && self->phase == 1 && self->step == 1) {
        if (func_ov039_02098ca8(&self->aimStart, &self->aimCur) >= 0x10000) {
            r = 1;
        }
    }

    return r;
}

/** Copies the +0x14C and +0x154 pairs out as two points. */
void func_ov039_0208ef14(void* pin, OtuPoint* a, OtuPoint* b) {
    OtuBadge* self = (OtuBadge*)pin;

    *a = self->aimStart;
    *b = self->aimCur;
}

/**
 * @brief True once the pin's age at +0x1CC has reached 0x1E (30).
 *
 * `movge`/`movlt` again: `>= 0x1E`, not `> 0x1E`.
 */
s32 func_ov039_0208ef38(OtuBadge* task) {
    return (task)->aimFrames >= 0x1E;
}

/**
 * @brief True when the pin should be removed this frame.
 *
 * A switch on the kind at +0xF8 in which only kind 1 answers zero: every other
 * kind, and any kind past the table, goes to the body. That is why the jump
 * table has one distinct arm and the rest share the default -- the case list is
 * `0, 2, 3, 4, 5` plus `default`, with `1` out of line.
 *
 * The body applies the age filter and the RNG gate: a pin whose cell query is
 * not exactly 0xC is removed outright; one that is gets removed with
 * probability `which / 0x10000`.
 */
s32 func_ov039_0208ef4c(void* task, u32 which) {
    OtuBadge* self = (OtuBadge*)task;
    s32       kind = self->phase;
    s32       r    = 0;

    switch (kind) {
        case 1:
        default:
            if (func_ov039_0208a794((OtuPoint*)&self->pos.x, self->board) != 0xC) {
                r = 1;
            } else if (RNG_Next(0x10000) < which) {
                r = 1;
            }
            break;

        case 0:
        case 2:
        case 3:
        case 4:
        case 5:
            break;
    }

    return r;
}

/**
 * @brief The pin-scoring filter: true when a child is a real pin.
 *
 * Two guards open it -- the nearest-child query at +0x120 must answer non-zero,
 * and the pin id at +0x16C must not be the 0x130 "no pin" sentinel -- and the
 * real answer comes from func_ov039_0208ef4c.
 */
s32 func_ov039_0208efb0(OtuBadge* task, s32 which) {
    OtuBadge* self = (OtuBadge*)task;

    if (func_ov039_0208a794((OtuPoint*)&self->pos.x, self->board) == 0) {
        return 0;
    }
    if (*self->pinID == 0x130) {
        return 0;
    }

    return func_ov039_0208ef4c(task, which);
}

/**
 * The +0x1A4 word, read-and-cleared.
 *
 * Returns the old value and stores zero, so a caller polling this sees each
 * value exactly once. It is the only accessor here with that shape.
 */
s32 func_ov039_0208eff8(OtuBadge* self) {
    s32 value = self->unk_1A4;

    self->unk_1A4 = 0;
    return value;
}
