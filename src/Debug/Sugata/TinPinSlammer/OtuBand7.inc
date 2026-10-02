/* ============================================================================
 * The cursor family at 0x0209003c - 0x020901e0.
 *
 * Eight functions in two identical pairs, and both halves of the band-7 hammer
 * task call all eight, so they come first: nothing else in this file links
 * until these exist.
 *
 * The shape is OtuCursor from band 1 -- table, index, count, framesLeft, 0xA
 * bytes -- but the table is walked with a stride of six *bytes*, so these entries are
 * 6 where band 1's are 4. Two functions are declared over a
 * four-byte entry instead; see func_ov039_0209003c for why that is not a typo.
 *
 *   func_ov039_0209005c / func_ov039_02090148   start a cursor at entry 0
 *   func_ov039_0209008c / func_ov039_02090178   step it by one frame
 *   func_ov039_0209003c / func_ov039_020901e0   read the entry's `value`
 *   func_ov039_020900f4                         read the entry's `value`
 *   func_ov039_0209011c                         read the entry's `scale`, <<12
 *
 * Each pair is byte-for-byte the same code, which is why the two families can
 * be read as one: the hammer task keeps one cursor per animated quantity and
 * the compiler was pointed at the same routine for each.
 * ==========================================================================*/

/** One entry of the table: three halfwords, a six-byte stride.
 *
 * The stride is six *bytes*, not six halfwords -- the target's only multiply is
 * `smulbb r0, r2, r0` against an immediate of 6 feeding a byte offset, so the
 * immediate is a byte count. The three readers take the halfword at +0, +2 and
 * +4 respectively.
 */
typedef struct {
    /* 0x00 */ s16 duration; // frames to hold this entry
    /* 0x02 */ s16 value;    // what func_ov039_020900f4 / _1e0 return
    /* 0x04 */ s16 scale;    // <<12 on use, by func_ov039_0209011c
} OtuFrame6; // Size: 0x6

/** A cursor over a table of OtuFrame6, one entry per call. */
typedef struct {
    /* 0x00 */ OtuFrame6* table;
    /* 0x04 */ s16 index;
    /* 0x06 */ s16 count;
    /* 0x08 */ s16 framesLeft;
} OtuCursor6; // Size: 0xA

/** One entry as func_ov039_0209003c reads it: four bytes, not twelve.
 *
 *  Only this one function indexes the cursor's table with a stride of four.
 *  Everything else in the family -- including the cursor's own initialiser and
 *  stepper -- uses six, so this is a genuine disagreement in the original code
 *  rather than a transcription slip, and it is reproduced rather than tidied.
 */
typedef struct {
    /* 0x00 */ s16 duration;
    /* 0x02 */ s16 value;
} OtuFrame4; // Size: 0x4

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

    c->table = table;
    c->index = 0;
    c->count = count;
    offset = c->index * 6;
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
        index = c->index;

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

    c->table = table;
    c->index = 0;
    c->count = count;
    offset = c->index * 6;
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
        index = c->index;

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