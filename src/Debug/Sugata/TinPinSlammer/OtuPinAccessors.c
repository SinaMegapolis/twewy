#include "OtuFieldAccessShared.h"

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
 *   OtuPinTask*  the header's trimmed +0x174-byte view of the same object,
 *                modelling only the fields the nearest-child queries read.
 *   void*        what band 1 and band 7 happen to pass around.
 *
 * OtuPinTask has no field for +0x0DC or +0x0FC, so those two read through
 * OtuBadge, which band 12 declares and which models the whole 0x25C bytes.
 * Every offset OtuPinTask does model agrees with OtuBadge, so the cast is
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
s32 func_ov039_0208e890(void* task) {
    OtuBadge* self = (OtuBadge*)task;

    switch (self->kind) {
        case 0:
        case 2:
        case 3:
        case 4:
            return 0;

        case 1:
        default:
            break;
    }

    return self->unk_0DC;
}

/**
 * @brief The pin child's aim scale, 0x1000 when nothing applies.
 *
 * Kind 8, sub-kind 1: while the +0x100 cursor is under 10 this is the cursor
 * over 10, on a 0x1000 scale.
 *
 * Kind 8, sub-kind 4: the cursor at +0x100 against half the cell's size word,
 * counted down from 0x1000.
 */
s32 func_ov039_0208e8c4(void* task) {
    u8* self  = (u8*)task;
    s32 scale = 0x1000;

    if (*(s32*)(self + 0xF8) == 8) {
        switch (*(s32*)(self + 0xFC)) {
            case 1: {
                s32 cursor = *(s32*)(self + 0x100);

                if (cursor < 0xA) {
                    scale = FX_Divide(cursor << 12, 0xA000);
                }
                break;
            }

            case 4: {
                u16 index = *(u16*)*(s32*)(self + 0x16C);
                s32 base  = *(s32*)(self + 0x170);
                u32 size  = *(u16*)((u8*)(base + index * 0x1C) + 0xE);

                if (*(s32*)(self + 0x100) < (s32)(size >> 1)) {
                    scale = 0x1000 - FX_Divide(*(s32*)(self + 0x100) << 12, (s32)(size >> 1) << 12);
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
 * @brief A three-valued answer, but only for kind 8: subkind 2 answers 2,
 *        subkind 3 answers 3, and every other combination answers 0.
 *
 *  The `bne` past both tests is why this is a nested if rather than a second
 *  switch -- the subkind is never read unless the kind already matched.
 */
s32 func_ov039_0208e950(void* task) {
    OtuBadge* self = (OtuBadge*)task;
    s32       r    = 0;

    if (self->kind == 8) {
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
 *  Written as a local plus a select, not as `return task->kind == 8;`. The
 *  target copies the field into a register, zeroes the result and selects into
 *  it -- `ldr r1, [r0, #0xf8] / mov r0, #0 / cmp r1, #0x8 / moveq r0, #1` --
 *  where a bare comparison compiles to a conditional move against the loaded
 *  field. Same answer, different shape, 58% against 100%.
 */
s32 func_ov039_0208e984(OtuPinTask* task) {
    s32 found = task->kind;
    s32 r     = 0;

    if (found == 8) {
        r = 1;
    }

    return r;
}

/**
 * @brief True when the pin's kind is 7.
 *
 *  Written as a local plus a select, not as `return task->kind == 7;`. The
 *  target copies the field into a register, zeroes the result and selects into
 *  it -- `ldr r1, [r0, #0xf8] / mov r0, #0 / cmp r1, #0x7 / moveq r0, #1` --
 *  where a bare comparison compiles to a conditional move against the loaded
 *  field. Same answer, different shape, 58% against 100%.
 */
s32 func_ov039_0208e998(OtuPinTask* task) {
    s32 found = task->kind;
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
s32 func_ov039_0208e9ac(void* task) {
    OtuBadge* self = (OtuBadge*)task;
    u16       a;
    s32       x = -self->dir.x;
    s32       y = -self->dir.y;

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
 *  Written as a local plus a select, not as `return task->kind == 6;`. The
 *  target copies the field into a register, zeroes the result and selects into
 *  it -- `ldr r1, [r0, #0xf8] / mov r0, #0 / cmp r1, #0x6 / moveq r0, #1` --
 *  where a bare comparison compiles to a conditional move against the loaded
 *  field. Same answer, different shape, 58% against 100%.
 */
s32 func_ov039_0208e9d0(OtuPinTask* task) {
    s32 found = task->kind;
    s32 r     = 0;

    if (found == 6) {
        r = 1;
    }

    return r;
}

/**
 * @brief True when the pin's kind is 9.
 *
 *  Written as a local plus a select, not as `return task->kind == 9;`. The
 *  target copies the field into a register, zeroes the result and selects into
 *  it -- `ldr r1, [r0, #0xf8] / mov r0, #0 / cmp r1, #0x9 / moveq r0, #1` --
 *  where a bare comparison compiles to a conditional move against the loaded
 *  field. Same answer, different shape, 58% against 100%.
 */
s32 func_ov039_0208e9e4(OtuPinTask* task) {
    s32 found = task->kind;
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
s32 func_ov039_0208e9f8(TaskPool* pool, void* task) {
    u8* self    = (u8*)task;
    s32 claimed = 0;
    s32 ret;

    switch (*(s32*)(self + 0xF8)) {
        case 6:
            ret = func_ov039_02091628(EasyTask_GetTaskData(pool, *(s32*)(self + 0x1E8)), (OtuPoint3*)(self + 0x184));
            *(s32*)(self + 0x180) = ret;
            if (ret > 0) {
                claimed = 1;
            }
            break;

        case 7:
            ret = func_ov039_02090e9c(EasyTask_GetTaskData(pool, *(s32*)(self + 0x1E4)), (OtuBar*)(self + 0x184));
            *(s32*)(self + 0x180) = ret;
            if (ret > 0) {
                claimed = 1;
            }
            break;

        case 8:
            /* Two sequential guards with their own `break`, not one combined
             * condition: `&&`, nested `if`s and a negated `||` all fold the
             * pair into `ldreq`/`cmpeq`, where the target branches past the
             * body twice. */
            if (*(s32*)(self + 0xFC) != 5) {
                break;
            }
            if (*(s32*)(self + 0x100) != 0x27) {
                break;
            }

            claimed = 1;
            func_ov039_0208fee0(EasyTask_GetTaskData(pool, *(s32*)(self + 0x1E0)), (OtuPinRecord*)(self + 0x184));
            *(s32*)(self + 0x180) = claimed;
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
// Nonmatching: 89.9%. Three scheduling shape differences left:
//   * the contact block. Target's contact block schedule mixes the y-diff
//     loads before the x-diff stores, laid out: load +0x124 pair, +0x120 self
//     copy, y-sub, +0x120 pair, x-sub, then store +0x12C before +0x130. This
//     source compiles straightforwardly in address order. Reversing the two
//     assignments in C scores worse (89.0).
//   * the tail's pool words. Target reads `data_ov039_0209a398` before the
//     +0x16C/+0x170 cell chain, so its literal pool lists a398 ahead of
//     a3dc; this source's factor-read runs first, giving the reversed pool
//     order. The inline-scale spelling (no scale local) supposedly matched
//     m2c's, but scored 85.9 -- the smull operand order flipped too.
//   * the `i = 0` init lands after the walk-inits here, before them in the
//     target (one `mov r7` row).
// The contact's `!= 0 || != 0` negation (swapped d3c arms) was worth +9% on
// its own and matched subsequently. Everything else -- all three mode bodies,
// the wind-down, the 64-bit smull/adc scale and every branch target -- agrees.
void func_ov039_0208eaa0(void* self_, void* task) {
    u8* other = (u8*)self_;
    u8* self  = (u8*)task;

    if (*(s32*)(self + 0x128) != 0) {
        return;
    }
    if (*(s32*)(self + 0x168) > 0) {
        return;
    }

    switch (*(s32*)(self + 0xF8)) {
        /* m2c's label order: default first (it joins the big block), then the
         * modes that share the body, then mode 8's phase gate, then the
         * modes that return. */
        default:
        case 1:
        case 6:
        case 7:
            break;

        case 8:
            switch (*(s32*)(self + 0xFC)) {
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

    if (*(s32*)(other + 0x180) <= 0) {
        return;
    }
    {
        s32 i     = 0;
        u8* mask  = other;
        u8* entry = other + 0x184;

        do {
            s32 limit = *(s32*)(mask + 0x18C) + 0xC000;

            if (func_ov039_02098ca8((OtuPoint*)(self + 0x120), (OtuPoint*)entry) >= limit) {
                goto next;
            }

            switch (*(s32*)(other + 0xF8)) {
                case 6: {
                    func_ov039_02091668(EasyTask_GetTaskData(*(void**)(other + 8), *(s32*)(other + 0x1E8)));
                    func_ov039_02087d04(0x337, (OtuPoint*)(other + 0x120), (OtuPoint*)(other + 0x110));

                    if (*(s32*)(self + 0x148) <= 0) {
                        if (*(u16*)*(s32*)(self + 0x16C) < 0x130) {
                            func_ov039_0208a490(self_, 1);
                        }
                    }

                    *(s32*)(self + 0x12C) = *(s32*)(self + 0x120) - *(s32*)(other + 0x120);
                    *(s32*)(self + 0x130) = *(s32*)(self + 0x124) - *(s32*)(other + 0x124);

                    if (*(s32*)(self + 0x12C) != 0 || *(s32*)(self + 0x130) != 0) {
                        func_ov039_02098d3c((OtuPoint*)(self + 0x12C), (OtuPoint*)(self + 0x12C));
                    } else {
                        *(s32*)(self + 0x12C) = 0x1000;
                        *(s32*)(self + 0x130) = 0;
                    }
                    break;
                }

                case 7: {
                    void* cand   = EasyTask_GetTaskData(*(void**)(other + 8), *(s32*)(other + 0x1E4));
                    u16   cursor = (u16)(func_ov039_02091060(cand) - 0x4000);
                    s32   pair   = (cursor >> 4) * 2;

                    func_ov039_0209104c(cand);

                    *(s32*)(self + 0x12C) = ((s16*)data_0205e4e0)[pair + 1];
                    *(s32*)(self + 0x130) = ((s16*)data_0205e4e0)[pair];

                    if (*(s32*)(self + 0xF8) == 6) {
                        func_ov039_02091690(EasyTask_GetTaskData(*(void**)(self + 8), *(s32*)(self + 0x1E8)));
                    }
                    func_ov039_02087d04(0x33B, (OtuPoint*)(other + 0x120), (OtuPoint*)(other + 0x110));

                    if (*(s32*)(self + 0x148) <= 0) {
                        if (*(u16*)*(s32*)(self + 0x16C) < 0x130) {
                            func_ov039_0208a490(self_, 1);
                        }
                    }
                    break;
                }

                case 8: {
                    if (*(s32*)(self + 0xF8) == 7) {
                        func_ov039_02091070(EasyTask_GetTaskData(*(void**)(self + 8), *(s32*)(self + 0x1E4)));
                    }
                    func_ov039_02087d04(0x333, (OtuPoint*)(other + 0x120), (OtuPoint*)(other + 0x110));

                    if (*(s32*)(self + 0x148) <= 0) {
                        if (*(u16*)*(s32*)(self + 0x16C) < 0x130) {
                            func_ov039_0208a490(self_, 1);
                        }
                    }

                    *(s32*)(self + 0x12C) = *(s32*)(self + 0x120) - *(s32*)(other + 0x120);
                    *(s32*)(self + 0x130) = *(s32*)(self + 0x124) - *(s32*)(other + 0x124);

                    if (*(s32*)(self + 0x12C) != 0 || *(s32*)(self + 0x130) != 0) {
                        func_ov039_02098d3c((OtuPoint*)(self + 0x12C), (OtuPoint*)(self + 0x12C));
                    } else {
                        *(s32*)(self + 0x12C) = 0x1000;
                        *(s32*)(self + 0x130) = 0;
                    }
                    break;
                }

                default:
                    break;
            }

            if (*(s32*)(self + 0x148) <= 0) {
                *(s32*)(self + 0x148) = *(u16*)(*(u8**)(self + 0x170) + (u32) * (u16*)*(s32*)(self + 0x16C) * 0x1C + 0x18);
                *(s32*)(self + 0xF8)  = 1;
                *(s32*)(self + 0xF4)  = 0;
                func_ov039_0208f7a4(EasyTask_GetTaskData(*(void**)(self + 8), *(s32*)(self + 0x1D8)), *(s32*)(self + 0x148));
            }
            {
                OtuPoint  start;
                OtuPoint* base = (OtuPoint*)(self + 0x12C);
                s32       factor =
                    *(s32*)((u8*)&data_ov039_0209a3dc +
                            *(u8*)(*(u8**)(self + 0x170) + (u32) * (u16*)*(s32*)(self + 0x16C) * 0x1C + 4) * 0x10 + 8);
                s32 scale;

                start.x = 0;
                scale   = (s32)(((s64)data_ov039_0209a398 * factor + 0x800) >> 0xC);
                start.y = 0;

                *(s32*)(self + 0x138) = *(s32*)(self + 0x12C);
                *(s32*)(self + 0x13C) = *(s32*)(self + 0x130);

                func_ov039_02098c00(scale, base, &start, base);
                *(s32*)(other + 0x1B0) = (s32)task;
                *(s32*)(self + 0x1B0)  = (s32)self_;
            }

        next:
            mask += 0xC;
            entry += 0xC;
            i = i + 1;
        } while (i < *(s32*)(other + 0x180));
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
s32 func_ov039_0208ee84(OtuPinTask* task) {
    return *(s32*)((u8*)task + 0x148) > 0;
}

/** The "alive" word at +0x148. This is the one 0208ee84 tests. */
s32 func_ov039_0208ee98(void* task) {
    return *(s32*)((u8*)task + 0x148);
}

/* ------------------------------------------------------------------ */
/* Four consecutive s16s at base + 0x100 + 0x78.                        */
/* ------------------------------------------------------------------ */

/*
 * These four read 0x78, 0x7A, 0x7C and 0x7E off a base already offset by 0x100,
 * so they are four consecutive s16 fields. The `add r0, r0, #0x100` is spelled
 * out in each rather than folded into the load offset: a single
 * `ldrsh [r0, #0x178]` is a valid alternative encoding that this build does not
 * produce.
 */

s16 func_ov039_0208eea0(void* task) {
    return *(s16*)((u8*)task + 0x100 + 0x78);
}

s16 func_ov039_0208eeac(void* task) {
    return *(s16*)((u8*)task + 0x100 + 0x7A);
}

s16 func_ov039_0208eeb8(void* task) {
    return *(s16*)((u8*)task + 0x100 + 0x7C);
}

s16 func_ov039_0208eec4(void* task) {
    return *(s16*)((u8*)task + 0x100 + 0x7E);
}

/**
 * @brief The pin's three-way phase test, read off +0x128/+0xF8/+0xF4.
 *
 * Answers 1 only when +0x128 is zero and both +0xF8 and +0xF4 are 1 *and* the
 * distance between the points at +0x14C and +0x154 is at least 0x10000. The
 * target's `ldreq`/`cmpeq` run of conditional loads is mwcc folding that
 * five-term short-circuit chain into one compare chain.
 */
s32 func_ov039_0208eed0(void* pin) {
    u8* self = (u8*)pin;
    s32 r    = 0;

    if (*(s32*)(self + 0x128) == 0 && *(s32*)(self + 0xF8) == 1 && *(s32*)(self + 0xF4) == 1) {
        if (func_ov039_02098ca8((OtuPoint*)(self + 0x14C), (OtuPoint*)(self + 0x154)) >= 0x10000) {
            r = 1;
        }
    }

    return r;
}

/** Copies the +0x14C and +0x154 pairs out as two points. */
void func_ov039_0208ef14(void* pin, OtuPoint* a, OtuPoint* b) {
    u8* self = (u8*)pin;

    *a = *(OtuPoint*)(self + 0x14C);
    *b = *(OtuPoint*)(self + 0x154);
}

/**
 * @brief True once the pin's age at +0x1CC has reached 0x1E (30).
 *
 * `movge`/`movlt` again: `>= 0x1E`, not `> 0x1E`.
 */
s32 func_ov039_0208ef38(void* task) {
    return *(s32*)((u8*)task + 0x1CC) >= 0x1E;
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
    u8* self = (u8*)task;
    s32 kind = *(s32*)(self + 0xF8);
    s32 r    = 0;

    switch (kind) {
        case 1:
        default:
            if (func_ov039_0208a794((OtuPoint*)(self + 0x120), (OtuCellGrid*)*(s32*)(self + 0xE4)) != 0xC) {
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
s32 func_ov039_0208efb0(OtuPinTask* task, s32 which) {
    u8* self = (u8*)task;

    if (func_ov039_0208a794((OtuPoint*)(self + 0x120), (OtuCellGrid*)*(s32*)(self + 0xE4)) == 0) {
        return 0;
    }
    if (*(u16*)*(s32*)(self + 0x16C) == 0x130) {
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
s32 func_ov039_0208eff8(void* task) {
    s32 value = *(s32*)((u8*)task + 0x1A4);

    *(s32*)((u8*)task + 0x1A4) = 0;
    return value;
}
