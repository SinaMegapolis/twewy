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
 * @brief A three-valued answer, but only for kind 8: subkind 2 answers 2,
 *        subkind 3 answers 3, and every other combination answers 0.
 *
 *  The `bne` past both tests is why this is a nested if rather than a second
 *  switch -- the subkind is never read unless the kind already matched.
 */
s32 func_ov039_0208e950(void* task) {
    OtuBadge* self = (OtuBadge*)task;
    s32        r    = 0;

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
    u16        a;
    s32        x = -self->dir.x;
    s32        y = -self->dir.y;

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