#include "OtuFieldAccessShared.h"

/*
 * ov039 region 0x0209702c - 0x02098394. One translation unit of the
 * overlay; dsd gives each file a single contiguous `.text` claim. The
 * shared types, externs and prototypes are in OtuFieldAccessShared.h.
 */
/* ------------------------------------------------------------------ */
/* The two 344-byte setup routines: the fourth pair of siblings.       */
/* ------------------------------------------------------------------ */

/*
 * 0x02097240 and 0x02097e9c are one body with four constants varied, and they
 * are the largest pair in the band. Both:
 *
 *   - raise the "running" flag;
 *   - preset the anchor words;
 *   - copy the creation block's two words into the sprite's start position;
 *   - draw two random values out of the base game's s16 pair table;
 *   - derive a speed from the starting spin and store it twice;
 *   - pick a starting animation at random out of a range.
 *
 * What varies:
 *
 *              02097240   02097e9c
 *   anchor        0x1800      0x1000     (1.5 vs 1.0 in Q12.12)
 *   anim range    0x0A ..     0x03 ..
 *                 + RNG(7)    + RNG(0xB)
 *
 * i.e. the same sprite, one covering 1.5x the ground and starting on a different
 * animation band.
 */

/**
 * @brief The signed magnitude of the starting spin, in Q12.12.
 *
 * `FX_Divide` here is the DS hardware divide, so this is a 20-bit-fraction
 * quotient scaled by three and shifted back to Q12.12.
 *
 * The macro form is load-bearing: it evaluates its argument three times, which is
 * exactly what the target does -- once for the sign test and once on whichever
 * side of it is taken. Written as a call to a helper it would evaluate once, and
 * the target would be two `bl`s instead of three inline expandings.
 */
#define OTU_SPIN_SPEED(s) ((FX_Divide((s)->unk_70, data_ov039_0209a310) * 3) >> 0xC)

#define OTU_ABS_SPEED(s) (OTU_SPIN_SPEED(s) < 0 ? -OTU_SPIN_SPEED(s) : OTU_SPIN_SPEED(s))

/* ------------------------------------------------------------------ */
/* The two 300-byte update siblings.  This is the band.                */
/* ------------------------------------------------------------------ */

/*
 * 0x0209702c (the first task) and 0x02097c90 (the fourth) are the same function
 * with one constant changed. Both walk the same path -- fetch the chased pin,
 * count one tick off a budget, read the pin's position, scale the budget into a
 * Q12.12 anchor, bleed a count down, spin the object up, blend its position
 * toward the anchor, and raise the flag the render stage watches -- and their
 * instructions line up one for one apart from two differences:
 *
 *   1. the multiplier. The first computes `budget * 0x1800`, which mwcc emits as
 *      an explicit `mov r0, #0x1800; mul`; the fourth computes `budget * 0x1000`,
 *      which it folds to the `lsl r0, r0, #0xc` the target wants. So the two
 *      constants are genuinely different -- 0x1800 is 1.5 in Q12.12 and 0x1000 is
 *      1.0 -- and it is the *value*, not the spelling, that separates them: the
 *      target itself spells the fourth one as a shift because the constant makes
 *      a shift legal. This is not the divide-form gap from the brief; both sides
 *      are multiplies and both fall out of a plain `*`.
 *
 *   2. where the position read sits. The first calls the position getter and
 *      only then reloads the budget for the multiply; the fourth reads the
 *      position first and decrements after, so the decremented value is still in
 *      a register at the multiply. That is a source-order difference, not a
 *      codegen one: in the first the reload is forced because the call has
 *      already clobbered r0.
 *
 * They are written out twice rather than factored, per the `-inline noauto`
 * rule in the brief.
 */

/** The first pin-sprite's update stage. */
s32 func_ov039_0209702c(TaskPool* pool, Task* task, void* param) {
    OtuTaskSprite* s   = (OtuTaskSprite*)task->data;
    void*          pin = EasyTask_GetTaskData(pool, s->unk_74);

    if (pin != NULL) {
        if (s->unk_78 != 0) {
            s->unk_84 = s->unk_84 - 1;

            if (s->unk_84 <= 0) {
                s->unk_78 = 0;
                s->unk_7C = 0;
            } else {
                func_ov039_0208e85c(pin, (OtuPoint*)&s->unk_50);
                s->unk_48 = _s32_div_f(s->unk_84 * 0x1800, s->unk_80);
                s->unk_44 = s->unk_48;

                s->unk_6C = s->unk_6C - data_ov039_0209a304;

                if (s->unk_6C < 0) {
                    s->unk_6C = 0;
                }

                s->unk_70 = s->unk_70 + data_ov039_0209a310;
                func_ov039_02098c00(s->unk_6C, (OtuPoint*)&s->unk_64, (OtuPoint*)&s->unk_58, (OtuPoint*)&s->unk_58);

                s->unk_60 = s->unk_60 + s->unk_70;

                // Wrap the accumulated offset back to zero, and rescale the spin
                // by the same signed factor when it is still positive.
                //
                // The rescale is a 64-bit multiply-and-reshift with a +0x800
                // rounding bias, not a divide: that is what produces the target's
                // smull / adds #0x800 / lsr #12 / orr sequence, and writing it as
                // `/ 0x1000` instead makes mwcc emit a call to __ll_sdiv.
                if (s->unk_60 > 0) {
                    s->unk_60 = 0;

                    if (s->unk_70 > 0) {
                        s->unk_70 = (s32)(((s64)s->unk_70 * -data_ov039_0209a300 + 0x800) >> 0xC);
                    }
                }

                Sprite_Update((Sprite*)s);
                s->unk_7C = 1;
            }
        }
    } else {
        // The pin is gone: stop, and do not draw this frame.
        s->unk_78 = 0;
        s->unk_7C = 0;
    }

    return 1;
}

/* ------------------------------------------------------------------ */
/* The render stages.                                                  */
/* ------------------------------------------------------------------ */
/*
 * (The four stage dispatchers are at the end of this file, once all sixteen
 * stages they name are in scope -- mwcc will not take a function's address
 * before it is declared.)
 */

/*
 * A render stage sets the sprite's `posX`/`posY` from two Q12.12 differences and
 * renders -- but only if the flag the update stage raised on the previous frame
 * is still set. Three of the four add a third term to `posY`; the fourth has
 * none, which is the only real difference between them.
 */

/** The first pin-sprite's render stage. */
s32 func_ov039_02097160(TaskPool* pool, Task* task, void* param) {
    OtuTaskSprite* s = (OtuTaskSprite*)task->data;

    if (s->unk_7C != 0) {
        s->sprite.posX = (s16)((s->unk_58 - s->unk_50) >> 0xC);
        s->sprite.posY = (s16)((s->unk_5C - s->unk_54 + s->unk_60) >> 0xC);
        Sprite_RenderFrame((Sprite*)s);
    }

    return 1;
}

/* ------------------------------------------------------------------ */
/* The cleanup stages: four identical five-instruction bodies.         */
/* ------------------------------------------------------------------ */

/*
 * The four `Sprite_Release` wrappers. The engine's convention is that a cleanup
 * stage returns 1 to keep the task alive, and all four do so unconditionally --
 * which is what the bare `mov r0, #1` is for here, exactly as in the four
 * `mov r0, #1; bx lr` bodies OtuFieldAccess.c declines to name.
 *
 * They are written out four times rather than factored, because the build uses
 * `-inline noauto` and a `static` helper would compile to a real `bl` and
 * collapse every caller to a stub.
 */

/** Releases the first pin-sprite's sprite. */
s32 func_ov039_020971b0(TaskPool* pool, Task* task, void* param) {
    Sprite_Release((Sprite*)task->data);
    return 1;
}

/** The first pin-sprite's stage dispatcher. */
s32 func_ov039_020971c4(TaskPool* pool, Task* task, void* param, s32 stage) {
    OtuTaskSpriteStage stages[4] = {func_ov039_02096fcc, func_ov039_0209702c, func_ov039_02097160, func_ov039_020971b0};

    return stages[stage](pool, task, param);
}

/* Returns the task handle. The target's last two instructions are the call
 * and the epilogue with no `mov r0` between, so r0 already holds the handle
 * and returning it costs nothing -- and a caller in band 1 assigns it. */
s32 func_ov039_0209720c(TaskPool* pool, s32 word0, s32 word1) {
    OtuTaskParams params;

    params.word0 = word0;
    params.word1 = word1;
    return EasyTask_CreateTask(pool, &data_ov039_02099f04, NULL, 0, NULL, &params);
}

// Nonmatching: 87.6%, eleven instructions out of 79, and all eleven are register
// allocation or load scheduling inside a body that is otherwise settled -- every
// field, offset, constant, call and branch edge is right.
//
// What is left, in order of size:
//
//   * the second word of the creation block is loaded one instruction later than
//     the target loads it, and into r2 rather than r1;
//   * the index arithmetic for the table pair uses r1 where the target uses r3
//     (and takes the pool word into r1 where the target takes it into r3).
//
// Tried and rejected: threading the two block words through locals (86.1%, worse
// -- mwcc then sinks the second load past all three stores).
//
// One thing here is worth recording because it was worth 33 points. These eight
// constants are declared `extern s32`, NOT `extern const s32`. Told a symbol is
// const, mwcc keeps its value in a callee-saved register across the FX_Divide
// call -- here and in func_ov039_0209702c's bleed-down block alike -- where the
// target reloads it from the literal pool every time. Declaring them const cost
// 02097240 33 points (54.8% -> 87.6%) and kept 0209702c and 02097c90 off 100%.
void func_ov039_02097240(OtuTaskSprite* s, OtuTaskParams* param) {
    s32 index;

    s->unk_78 = 1;
    s->unk_44 = 0x1800;
    s->unk_48 = 0x1800;
    s->unk_58 = param->word0;
    s->unk_5C = param->word1;
    s->unk_60 = 0;

    // The starting count is jittered about data_ov039_0209a314 (0x3000) by up to
    // the difference to 0x0209a328, and the starting spin negatively about
    // 0x0200 by up to the difference to 0x4000.
    s->unk_6C = data_ov039_0209a314;
    s->unk_6C = s->unk_6C + RNG_Next(data_ov039_0209a328 - data_ov039_0209a314);
    s->unk_70 = 0 - data_ov039_0209a2fc;
    s->unk_70 = s->unk_70 - RNG_Next(data_ov039_0209a30c - data_ov039_0209a2fc);

    // The speed is the magnitude of that spin; stored twice, once as the divisor
    // the update stage divides by and once as the remaining budget.
    s->unk_80 = OTU_ABS_SPEED(s);
    s->unk_84 = s->unk_80;

    // A random adjacent pair out of the base game's table, taken as a direction.
    index     = RNG_Next(0x10000) >> 4;
    s->unk_64 = *(s16*)((u8*)&data_0205e4e0 + (index + 1) * 2);
    s->unk_68 = *(s16*)((u8*)&data_0205e4e0 + index * 2);

    Sprite_ChangeAnimation((Sprite*)s, s->sprite.animData, (s16)(RNG_Next(7) + 0xA), s->sprite.cellTable);
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
OtuSpriteSlot* func_ov039_02097398(OtuSpriteTask* t, s32 arg, s32 mode) {
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
            slot->depthKey = func_ov039_02088400(*(s32*)((u8*)t + 0x4C), 0, 4);
            return slot;
        }

        default:
            return NULL;
    }
}

/* ------------------------------------------------------------------ */
/* The _Sprite_Load wrappers.                                          */
/* ------------------------------------------------------------------ */

/*
 * Three routines with one body: copy a 0x2C-byte `SpriteAnimation` template onto
 * the stack, stamp it with this object's owner, its `dataType` and its anchor,
 * and load one sprite.
 *
 * The template copy is the `ldm r5!, {r0..r3} / stm r4!, {r0..r3}` triple out of
 * a literal pool, which is mwcc's rendering of a *local array initialiser* -- not
 * of a read from a `static const` object, which it would constant-fold instead.
 * So each wrapper declares its template as a local brace initialiser, and the
 * 0x2C bytes have to be spelled out. They are transcribed from the target's own
 * templates at 0x02099f68, 0x02099fb0 and 0x02099ff8; the two that differ are
 * the `frameInfoCallback` (each wrapper's own cell builder) and six of the small
 * trailing fields.
 *
 * The templates themselves are *not* emitted as named objects. A declared
 * `static const SpriteAnimation` would be emitted into this object's `.rodata`,
 * and this translation unit has no `.rodata` claim in the overlay's delinks --
 * the range is gap-filled from the original overlay -- so the linker would place
 * it at an address of its own choosing and overwrite the target's bytes. The
 * local-initialiser form keeps the bytes in an anonymous template instead, which
 * costs nothing in the ROM because the copy is immediate.
 */

void func_ov039_02097454(OtuTaskSprite* self, Sprite* sprite, s32* dataType) {
    SpriteAnimation anim = {
        /* 0x00 */ {2, 0, 0, 5, 0, 2, 0}, // 0x2282
                                          /* 0x02 */
        {0},
        /* 0x04 */
        0x50, // posX
        /* 0x06 */ 0x50, // posY
        /* 0x08 */ (SpriteFrameInfoCallback)func_ov039_02097398,
        /* 0x0C */ 0, // callbackArg
        /* 0x10 */ NULL, // owner
        /* 0x14 */ &data_ov039_0209a0dc,
        /* 0x18 */ 2,
        /* 0x1A */ 8,
        /* 0x1C */ 1,
        /* 0x1E */ 0,
        /* 0x20 */ 4,
        /* 0x22 */ 2,
        /* 0x24 */ 0,
        /* 0x26 */ 2,
        /* 0x28 */ 3,
        /* 0x2A */ 4  // animIndex
    };

    anim.owner    = self;
    anim.dataType = (u16)*dataType;
    anim.posX     = (s16)(self->unk_48 >> 0xC);
    anim.posY     = (s16)(self->unk_4C >> 0xC);
    _Sprite_Load(sprite, &anim);
}

/* ------------------------------------------------------------------ */
/* The init stages that live in this band.                             */
/* ------------------------------------------------------------------ */

/**
 * @brief The second pin-sprite's init stage.
 *
 * Clears the position block and both flags, keeps the creation block's second
 * word as the task id to chase at +0x68, and loads the sprite.
 */
s32 func_ov039_020974e0(TaskPool* pool, Task* task, void* param) {
    OtuTaskSprite* s = (OtuTaskSprite*)task->data;

    s->unk_6C = 0;
    s->unk_70 = 0;
    s->unk_68 = ((s32*)param)[1];
    s->unk_40 = 0;
    s->unk_44 = 0;
    s->unk_48 = 0;
    s->unk_4C = 0;
    s->unk_50 = 0;
    s->unk_54 = 0;
    s->unk_58 = 0;
    func_ov039_02097454(s, (Sprite*)s, (s32*)param);

    return 1;
}

/* ------------------------------------------------------------------ */
/* The second pin-sprite's update stage: a four-state machine.         */
/* ------------------------------------------------------------------ */

/**
 * @brief The second pin-sprite's update stage, 0x02097528.
 *
 * The third member of the cluster -- it calls both func_ov039_0208e85c and
 * func_ov039_02098c00 like the two 300-byte siblings -- but it is a different
 * shape, not a fourth copy of them. Its blend runs only while the state word is
 * non-zero, and it blends in the *other* direction: the offset is a countdown
 * that is decremented by a fixed step rather than a scale that is bled down by a
 * constant, so this sprite closes on its target rather than easing toward it.
 *
 * The state machine over +0x70 is a genuine `switch` and not an if-chain: the
 * target tests against 3, jumps through a table, and has the four bodies out of
 * line in source order. That matters beyond the layout -- it is also what
 * suppresses if-conversion, which is why case 1 and case 3 test the same
 * bitfield and yet one branches to an out-of-line `bne` and the other writes its
 * result predicated with `moveq`/`streq`.
 *
 * The bitfield tested at +0x00 in cases 1 and 3 is the base game's `Sprite`'s
 * own first word; the target reaches it as `(u32)(w << 0x15) >> 0x1F`, i.e. bit
 * 10, which is `Sprite.isPlaying` in the header's layout.
 *
 * Both tests are written `== TRUE` rather than as bare conditions, and that is
 * not cosmetic: a one-bit field tested bare lets mwcc use the flags the
 * extraction's `lsr` already set and drop the compare altogether, and the target
 * keeps `cmp r0, #1`. Comparing against TRUE is what preserves it.
 */
s32 func_ov039_02097528(TaskPool* pool, Task* task, void* param) {
    OtuTaskSprite* s   = (OtuTaskSprite*)task->data;
    void*          pin = EasyTask_GetTaskData(pool, s->unk_68);

    if (pin != NULL) {
        if (s->unk_70 != 0) {
            func_ov039_0208e85c(pin, (OtuPoint*)&s->unk_40);

            if (s->unk_5C > 0) {
                s->unk_5C = s->unk_5C - s->unk_60;

                if (s->unk_5C < 0) {
                    s->unk_5C = 0;
                }
            }

            func_ov039_02098c00(s->unk_5C, (OtuPoint*)&s->unk_54, (OtuPoint*)&s->unk_48, (OtuPoint*)&s->unk_48);
            s->unk_6C = 1;
        }

        switch (s->unk_70) {
            case 0:
                s->unk_6C = 0;
                break;

            case 1:
                // Wait for the intro animation to end, then move to the hold.
                if (s->sprite.isPlaying == TRUE) {
                    Sprite_ChangeAnimation((Sprite*)s, s->sprite.animData, 8, s->sprite.cellTable);
                    s->unk_70 = 2;
                }
                Sprite_Update((Sprite*)s);
                break;

            case 2:
                // Hold for +0x74 frames, then move to the outro.
                s->unk_74 = s->unk_74 - 1;

                if (s->unk_74 <= 0) {
                    Sprite_ChangeAnimation((Sprite*)s, s->sprite.animData, 9, s->sprite.cellTable);
                    s->unk_70 = 3;
                }
                Sprite_Update((Sprite*)s);
                break;

            case 3:
                // Wait out the outro, then retire the sprite.
                if (s->sprite.isPlaying == TRUE) {
                    s->unk_70 = 0;
                    s->unk_6C = 0;
                }
                Sprite_Update((Sprite*)s);
                break;

            default:
                break;
        }
    } else {
        s->unk_6C = 0;
        s->unk_70 = 0;
    }

    return 1;
}

/**
 * @brief The second pin-sprite's render stage.
 *
 * The same shape as the first's on the first task's layout, which the second
 * task reuses shifted down by 0x10 for these six fields.
 */
s32 func_ov039_02097670(TaskPool* pool, Task* task, void* param) {
    OtuTaskSprite* s = (OtuTaskSprite*)task->data;

    if (s->unk_6C != 0) {
        s->sprite.posX = (s16)((s->unk_48 - s->unk_40) >> 0xC);
        s->sprite.posY = (s16)((s->unk_4C - s->unk_44 + s->unk_50) >> 0xC);
        Sprite_RenderFrame((Sprite*)s);
    }

    return 1;
}

/** Releases the second pin-sprite's sprite. */
s32 func_ov039_020976c0(TaskPool* pool, Task* task, void* param) {
    Sprite_Release((Sprite*)task->data);
    return 1;
}

/** The second pin-sprite's stage dispatcher. */
s32 func_ov039_020976d4(TaskPool* pool, Task* task, void* param, s32 stage) {
    OtuTaskSpriteStage stages[4] = {func_ov039_020974e0, func_ov039_02097528, func_ov039_02097670, func_ov039_020976c0};

    return stages[stage](pool, task, param);
}

s32 func_ov039_0209771c(TaskPool* pool, s32 word0, s32 word1) {
    OtuTaskParams params;

    params.word0 = word0;
    params.word1 = word1;
    return EasyTask_CreateTask(pool, &data_ov039_02099f4c, NULL, 0, NULL, &params);
}

/* ------------------------------------------------------------------ */
/* The two remaining public entry points.                              */
/* ------------------------------------------------------------------ */

/**
 * @brief The second pin-sprite's setup, 0x02097750.
 *
 * The only entry point in the band taking six arguments, and the only one that
 * takes its starting direction from a caller-supplied selector rather than from
 * the RNG: `selector >> 4` indexes the same base-game `s16` pair table the setup
 * routines pick at random, so this one is the same sprite with the direction
 * handed in rather than rolled.
 *
 * The stack arguments are load-bearing. `push {r3, r4, r5, lr}` is a sixteen-byte
 * frame, so the fifth and sixth arguments land at sp+0x10 and sp+0x14 -- a
 * prototype that dropped them would put the reads four bytes out.
 */
// Nonmatching: 46.3%. Same instruction count as the target (32 each), and the
// last eleven -- the two `ldrsh`s' *results*, the two stack-argument loads, the
// `Sprite_ChangeAnimation` call and `+0x70 = 1` -- all match. What differs is
// entirely the scheduler's ordering of the first twenty-one.
//
// The target interleaves the stores with the loads in a way that minimises live
// ranges by hand: `+0x5C` and `+0x60` are stored, *then* the first `ldrsh`, then
// `+0x64`, then the second `ldrsh`. mwcc instead sinks the two block-word loads
// to the top and hoists both `ldrsh`s above the `+0x64` store, which shifts
// everything by a few positions.
//
// Tried and rejected, all landing within a point of this: threading the two block
// words through locals, hoisting the two table addresses into `s16*` locals, and
// typing the block as `OtuTaskParams*` rather than `s32*`. Every field, offset,
// constant and argument position is settled; this is mwcc's block scheduler and
// the source cannot ask for a different one.
void func_ov039_02097750(OtuTaskSprite* s, OtuTaskParams* param, s32 selector, s32 closeBy, s32 step, s32 holdFor) {
    s32 index = selector >> 4;

    s->unk_48 = param->word0;
    s->unk_4C = param->word1;
    s->unk_50 = 0;
    s->unk_5C = closeBy;
    s->unk_60 = step;
    s->unk_64 = 0;
    s->unk_54 = *(s16*)((u8*)&data_0205e4e0 + (index + 1) * 2);
    s->unk_58 = *(s16*)((u8*)&data_0205e4e0 + index * 2);
    s->unk_74 = holdFor;
    Sprite_ChangeAnimation((Sprite*)s, s->sprite.animData, 7, s->sprite.cellTable);
    s->unk_70 = 1;
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
OtuSpriteSlot* func_ov039_020977d0(OtuSpriteTask* t, s32 arg, s32 mode) {
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

/**
 * @brief The second pin-sprite's sprite load, 0x0209788c.
 *
 * The same body as func_ov039_02097454 over a template that differs only in its
 * cell builder (func_ov039_020977d0), four of the trailing halfwords, and its
 * final `animIndex`.
 */
void func_ov039_0209788c(OtuTaskSprite* self, Sprite* sprite, s32* dataType) {
    SpriteAnimation anim = {
        /* 0x00 */ {2, 0, 0, 5, 0, 2, 0}, // 0x2282
                                          /* 0x02 */
        {0},
        /* 0x04 */
        0x50, // posX
        /* 0x06 */ 0x50, // posY
        /* 0x08 */ (SpriteFrameInfoCallback)func_ov039_020977d0,
        /* 0x0C */ 0,
        /* 0x10 */ NULL,
        /* 0x14 */ &data_ov039_0209a0dc,
        /* 0x18 */ 2,
        /* 0x1A */ 5,
        /* 0x1C */ 1,
        /* 0x1E */ 0,
        /* 0x20 */ 4,
        /* 0x22 */ 1,
        /* 0x24 */ 0,
        /* 0x26 */ 2,
        /* 0x28 */ 3,
        /* 0x2A */ 1
    };

    anim.owner    = self;
    anim.dataType = (u16)*dataType;
    anim.posX     = (s16)(self->unk_48 >> 0xC);
    anim.posY     = (s16)(self->unk_4C >> 0xC);
    _Sprite_Load(sprite, &anim);
}

/**
 * @brief The third pin-sprite's init stage.
 *
 * The same shape as the second's over the shorter field set: it keeps the
 * creation block's second word at +0x50 and clears +0x40 through +0x4C.
 */
s32 func_ov039_02097918(TaskPool* pool, Task* task, void* param) {
    OtuTaskSprite* s = (OtuTaskSprite*)task->data;

    s->unk_54 = 0;
    s->unk_58 = 0;
    s->unk_50 = ((s32*)param)[1];
    s->unk_40 = 0;
    s->unk_44 = 0;
    s->unk_48 = 0;
    s->unk_4C = 0;
    func_ov039_0209788c(s, (Sprite*)s, (s32*)param);

    return 1;
}

/**
 * @brief The third pin-sprite's update stage, 0x02097954.
 *
 * The shortest of the four updates. It runs the blend only while the sprite's
 * animation is still playing, and when the animation finishes it drops both the
 * "running" and "drawn" flags -- the sprite plays once, tracks its target for as
 * long as it lasts, and then stops without any of the countdown machinery the
 * other three carry.
 */
s32 func_ov039_02097954(TaskPool* pool, Task* task, void* param) {
    OtuTaskSprite* s   = (OtuTaskSprite*)task->data;
    void*          pin = EasyTask_GetTaskData(pool, s->unk_50);

    // Positively nested on purpose. Spelled as `if (pin == NULL) {...} else if`,
    // mwcc if-converts both clears into the branches above them and tail-merges
    // them into one block; the target emits the same four instructions twice,
    // out of line at the bottom, which is what the nested form produces.
    if (pin != NULL) {
        if (s->unk_54 != 0) {
            if (SpriteMgr_IsAnimationFinished((Sprite*)s) == 0) {
                func_ov039_0208e85c(pin, (OtuPoint*)&s->unk_40);
                Sprite_Update((Sprite*)s);
                s->unk_58 = 1;
            } else {
                s->unk_54 = 0;
                s->unk_58 = 0;
            }
        }
    } else {
        s->unk_54 = 0;
        s->unk_58 = 0;
    }

    return 1;
}

/**
 * @brief The third pin-sprite's render stage.
 *
 * The odd one out. It watches +0x58 rather than +0x7C, and its `posY` has no
 * third term -- which follows from the third task being the short one: it never
 * keeps a value at the offset that term would come from.
 */
s32 func_ov039_020979cc(TaskPool* pool, Task* task, void* param) {
    OtuTaskSprite* s = (OtuTaskSprite*)task->data;

    if (s->unk_58 != 0) {
        s->sprite.posX = (s16)((s->unk_48 - s->unk_40) >> 0xC);
        s->sprite.posY = (s16)((s->unk_4C - s->unk_44) >> 0xC);
        Sprite_RenderFrame((Sprite*)s);
    }

    return 1;
}

/** Releases the third pin-sprite's sprite. */
s32 func_ov039_02097a14(TaskPool* pool, Task* task, void* param) {
    Sprite_Release((Sprite*)task->data);
    return 1;
}

/** The third pin-sprite's stage dispatcher. */
s32 func_ov039_02097a28(TaskPool* pool, Task* task, void* param, s32 stage) {
    OtuTaskSpriteStage stages[4] = {func_ov039_02097918, func_ov039_02097954, func_ov039_020979cc, func_ov039_02097a14};

    return stages[stage](pool, task, param);
}

s32 func_ov039_02097a70(TaskPool* pool, s32 word0, s32 word1) {
    OtuTaskParams params;

    params.word0 = word0;
    params.word1 = word1;
    return EasyTask_CreateTask(pool, &data_ov039_02099f94, NULL, 0, NULL, &params);
}

/**
 * @brief The third pin-sprite's setup, 0x02097aa4.
 *
 * A tail call, and the target says so: `ldr ip, .L_Sprite_SetAnimation` followed
 * by `bx ip`, with no frame at all. mwcc emits that when the call is the last
 * thing the function does.
 *
 * Note it is `Sprite_SetAnimation`, not `Sprite_ChangeAnimation` as the other
 * three setups use -- the two differ in the base game by the animation restart
 * `Sprite_ChangeAnimation` performs, and this one does not restart.
 */
// Nonmatching: 68.3%, four instructions out of 13, all scheduling.
//
// The `lsl r2, r2, #0x10` / `asr r2, r2, #0x10` pair is present in both -- that is
// the third argument truncated to `s16`, and it is why the parameter is typed
// `s32` here: declared `s16`, mwcc knows the value is already in range and drops
// the pair entirely (59.5%). Where the two halves land differs: the target keeps
// them together after the two stores, mwcc sinks the shift up above them. The
// pool word is likewise taken a slot earlier.
void func_ov039_02097aa4(OtuTaskSprite* s, OtuTaskParams* param, s32 animation) {
    s->unk_54 = 1;
    s->unk_48 = param->word0;
    s->unk_4C = param->word1;
    Sprite_SetAnimation((Sprite*)s, s->sprite.animData, (s16)animation, s->sprite.cellTable);
}

/** A single word at +0x54, read as s32 by a different caller. */
s32 func_ov039_02097ad8(void* task) {
    return *(s32*)((u8*)task + 0x54);
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
OtuSpriteSlot* func_ov039_02097ae0(OtuSpriteTask* t, s32 arg, s32 mode) {
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
            slot->depthKey = func_ov039_02088400(*(s32*)((u8*)t + 0x5C), 0, 3);
            return slot;
        }

        default:
            return NULL;
    }
}

/**
 * @brief The fourth pin-sprite's sprite load, 0x02097ba4.
 *
 * The same body again. Its template's first word is 0x1282 rather than 0x2282 --
 * `bits_12_13` is 1 instead of 2 -- and it reads its anchor from the other pair
 * of words, +0x58/+0x5C, matching the first and fourth tasks' position block.
 */
void func_ov039_02097ba4(OtuTaskSprite* self, Sprite* sprite, s32* dataType) {
    SpriteAnimation anim = {
        /* 0x00 */ {2, 0, 0, 5, 0, 1, 0}, // 0x1282
                                          /* 0x02 */
        {0},
        /* 0x04 */
        0x50, // posX
        /* 0x06 */ 0x50, // posY
        /* 0x08 */ (SpriteFrameInfoCallback)func_ov039_02097ae0,
        /* 0x0C */ 0,
        /* 0x10 */ NULL,
        /* 0x14 */ &data_ov039_0209a0dc,
        /* 0x18 */ 2,
        /* 0x1A */ 7,
        /* 0x1C */ 1,
        /* 0x1E */ 0,
        /* 0x20 */ 4,
        /* 0x22 */ 1,
        /* 0x24 */ 0,
        /* 0x26 */ 2,
        /* 0x28 */ 3,
        /* 0x2A */ 3
    };

    anim.owner    = self;
    anim.dataType = (u16)*dataType;
    anim.posX     = (s16)(self->unk_58 >> 0xC);
    anim.posY     = (s16)(self->unk_5C >> 0xC);
    _Sprite_Load(sprite, &anim);
}

/**
 * @brief The fourth pin-sprite's init stage.
 *
 * The first task's shape, with +0x44 and +0x48 preset to 0x1000 rather than
 * 0x1800 and with the last two clears written as halfwords -- the only place in
 * the band where a field is stored as two `s16`s instead of one word, so they
 * are written that way here.
 */
s32 func_ov039_02097c30(TaskPool* pool, Task* task, void* param) {
    OtuTaskSprite* s = (OtuTaskSprite*)task->data;

    s->unk_78              = 0;
    s->unk_7C              = 0;
    s->unk_74              = ((s32*)param)[1];
    s->unk_50              = 0;
    s->unk_54              = 0;
    s->unk_58              = 0;
    s->unk_5C              = 0;
    s->unk_60              = 0;
    s->unk_64              = 0;
    s->unk_68              = 0;
    s->unk_40              = 0;
    s->unk_44              = 0x1000;
    s->unk_48              = 0x1000;
    *(s16*)&s->unk_4C      = 0;
    *(s16*)((u8*)s + 0x4E) = 0;
    func_ov039_02097ba4(s, (Sprite*)s, (s32*)param);

    return 1;
}

/**
 * @brief The fourth pin-sprite's update stage.
 *
 * 0x02097c90. The same body as func_ov039_0209702c with 0x1800 replaced by
 * 0x1000 and the position read hoisted above the decrement; see the note above
 * the two together for why they are not one body.
 */
s32 func_ov039_02097c90(TaskPool* pool, Task* task, void* param) {
    OtuTaskSprite* s = (OtuTaskSprite*)task->data;
    s32            budget;
    void*          pin = EasyTask_GetTaskData(pool, s->unk_74);

    if (pin != NULL) {
        if (s->unk_78 != 0) {
            func_ov039_0208e85c(pin, (OtuPoint*)&s->unk_50);
            budget    = s->unk_84 - 1;
            s->unk_84 = budget;

            if (budget <= 0) {
                s->unk_78 = 0;
                s->unk_7C = 0;
            } else {
                s->unk_48 = _s32_div_f(budget * 0x1000, s->unk_80);
                s->unk_44 = s->unk_48;

                s->unk_6C = s->unk_6C - data_ov039_0209a304;

                if (s->unk_6C < 0) {
                    s->unk_6C = 0;
                }

                s->unk_70 = s->unk_70 + data_ov039_0209a310;
                func_ov039_02098c00(s->unk_6C, (OtuPoint*)&s->unk_64, (OtuPoint*)&s->unk_58, (OtuPoint*)&s->unk_58);

                s->unk_60 = s->unk_60 + s->unk_70;

                // Same rescale as the first task's: a 64-bit multiply-and-reshift
                // with a rounding bias, which is what the smull sequence is.
                if (s->unk_60 > 0) {
                    s->unk_60 = 0;

                    if (s->unk_70 > 0) {
                        s->unk_70 = (s32)(((s64)s->unk_70 * -data_ov039_0209a300 + 0x800) >> 0xC);
                    }
                }

                Sprite_Update((Sprite*)s);
                s->unk_7C = 1;
            }
        }
    } else {
        s->unk_78 = 0;
        s->unk_7C = 0;
    }

    return 1;
}

/** The fourth pin-sprite's render stage -- the first's, unchanged. */
s32 func_ov039_02097dbc(TaskPool* pool, Task* task, void* param) {
    OtuTaskSprite* s = (OtuTaskSprite*)task->data;

    if (s->unk_7C != 0) {
        s->sprite.posX = (s16)((s->unk_58 - s->unk_50) >> 0xC);
        s->sprite.posY = (s16)((s->unk_5C - s->unk_54 + s->unk_60) >> 0xC);
        Sprite_RenderFrame((Sprite*)s);
    }

    return 1;
}

/** Releases the fourth pin-sprite's sprite. */
s32 func_ov039_02097e0c(TaskPool* pool, Task* task, void* param) {
    Sprite_Release((Sprite*)task->data);
    return 1;
}

/** The fourth pin-sprite's stage dispatcher. */
s32 func_ov039_02097e20(TaskPool* pool, Task* task, void* param, s32 stage) {
    OtuTaskSpriteStage stages[4] = {func_ov039_02097c30, func_ov039_02097c90, func_ov039_02097dbc, func_ov039_02097e0c};

    return stages[stage](pool, task, param);
}

s32 func_ov039_02097e68(TaskPool* pool, s32 word0, s32 word1) {
    OtuTaskParams params;

    params.word0 = word0;
    params.word1 = word1;
    return EasyTask_CreateTask(pool, &data_ov039_02099fdc, NULL, 0, NULL, &params);
}

// Nonmatching: 87.6%, the same eleven instructions as func_ov039_02097240 and for
// the same reason -- both bodies are identical apart from four constants, so this
// is one mwcc behaviour reached twice, not two misreadings. See the note above.
void func_ov039_02097e9c(OtuTaskSprite* s, OtuTaskParams* param) {
    s32 index;

    s->unk_78 = 1;
    s->unk_44 = 0x1000;
    s->unk_48 = 0x1000;
    s->unk_58 = param->word0;
    s->unk_5C = param->word1;
    s->unk_60 = 0;
    s->unk_6C = data_ov039_0209a314;
    s->unk_6C = s->unk_6C + RNG_Next(data_ov039_0209a328 - data_ov039_0209a314);
    s->unk_70 = 0 - data_ov039_0209a2fc;
    s->unk_70 = s->unk_70 - RNG_Next(data_ov039_0209a30c - data_ov039_0209a2fc);
    s->unk_80 = OTU_ABS_SPEED(s);
    s->unk_84 = s->unk_80;
    index     = RNG_Next(0x10000) >> 4;
    s->unk_64 = *(s16*)((u8*)&data_0205e4e0 + (index + 1) * 2);
    s->unk_68 = *(s16*)((u8*)&data_0205e4e0 + index * 2);
    Sprite_ChangeAnimation((Sprite*)s, s->sprite.animData, (s16)(RNG_Next(0xB) + 3), s->sprite.cellTable);
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
OtuSpriteSlot* func_ov039_02097ff4(OtuSpriteTask* t, s32 arg, s32 mode) {
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
            slot->depthKey = func_ov039_02088400(*(s32*)((u8*)t + 0x5C), 0, 3);
            return slot;
        }

        default:
            return NULL;
    }
}

/** Loads the sprite for the task whose anim template is data_ov039_0209a040. */
void func_ov039_020980b8(void* self, void* sprite, s32* args) {
    SpriteAnimation anim = data_ov039_0209a040;

    anim.owner    = self;
    anim.dataType = (u16) * (s32*)args;
    anim.posX     = *(s32*)((u8*)self + 0x58) >> 12;
    anim.posY     = *(s32*)((u8*)self + 0x5C) >> 12;
    _Sprite_Load((Sprite*)sprite, &anim);
}

/** The same reset against the +0x80b8 helper, over a longer zero run. */
s32 func_ov039_02098144(TaskPool* pool, void* task, s32* args) {
    u8* block = *(u8**)((u8*)task + 0x18);

    (void)pool;

    *(s32*)(block + 0x7C) = 0;
    *(s32*)(block + 0x80) = 0;
    *(s32*)(block + 0x78) = args[1];
    *(s32*)(block + 0x50) = 0;
    *(s32*)(block + 0x54) = 0;
    *(s32*)(block + 0x58) = 0;
    *(s32*)(block + 0x5C) = 0;
    *(s32*)(block + 0x60) = 0;
    *(s32*)(block + 0x64) = 0;
    *(s32*)(block + 0x68) = 0;
    *(s32*)(block + 0x40) = 0;
    *(s32*)(block + 0x44) = 0x2000;
    *(s32*)(block + 0x48) = 0x2000;
    *(s16*)(block + 0x4C) = 0;
    *(s16*)(block + 0x4E) = 0;

    func_ov039_020980b8(block, block, args);
    return 1;
}

/**
 * @brief The +0x81a4 sibling of `func_ov039_02094ca8`'s spring step.
 *
 * Same integral, same damping, same bounce -- but the countdown is at +0x88
 * against a +0x84 divisor, the magnitude is written to +0x44/+0x48, and the
 * step runs only when the +0x78 task's data resolves *and* +0x7C is set. The
 * +0x40 word is a u16 accumulator advanced by the u16 at +0x74 and narrowed
 * after the add, then stored back into the 32-bit slot.
 */
s32 func_ov039_020981a4(TaskPool* pool, void* task) {
    u8*   sprite = *(u8**)((u8*)task + 0x18);
    void* data   = EasyTask_GetTaskData(pool, *(s32*)(sprite + 0x78));
    s32   count;
    s32   scale;
    s32   v;

    /* One `return 1` at the end rather than one per exit: the target shares a
     * single epilogue across the three ways out. The first guard is also
     * negated -- `data != NULL` with the clear as its `else` -- because that is
     * what makes mwcc branch *to* the clear block and lay the main body out as
     * the fall-through, which is the target's shape. */
    if (data != NULL) {
        if (*(s32*)(sprite + 0x7C) != 0) {
            func_ov039_0208e85c(data, (OtuPoint*)(sprite + 0x50));

            count                  = *(s32*)(sprite + 0x88) - 1;
            *(s32*)(sprite + 0x88) = count;

            if (count <= 0) {
                *(s32*)(sprite + 0x7C) = 0;
                *(s32*)(sprite + 0x80) = 0;
            } else {
                scale                  = (count << 13) / *(s32*)(sprite + 0x84);
                *(s32*)(sprite + 0x48) = scale;
                *(s32*)(sprite + 0x44) = scale;

                *(s32*)(sprite + 0x40) = (u16)(*(s32*)(sprite + 0x40) + *(u16*)(sprite + 0x74));

                v                      = *(s32*)(sprite + 0x6C) - data_ov039_0209a304;
                *(s32*)(sprite + 0x6C) = v;
                if (v < 0) {
                    *(s32*)(sprite + 0x6C) = 0;
                }

                *(s32*)(sprite + 0x70) = *(s32*)(sprite + 0x70) + data_ov039_0209a310;

                func_ov039_02098c00(*(s32*)(sprite + 0x6C), (OtuPoint*)(sprite + 0x64), (OtuPoint*)(sprite + 0x58),
                                    (OtuPoint*)(sprite + 0x58));

                v                      = *(s32*)(sprite + 0x60) + *(s32*)(sprite + 0x70);
                *(s32*)(sprite + 0x60) = v;
                if (v > 0) {
                    *(s32*)(sprite + 0x60) = 0;
                    if (*(s32*)(sprite + 0x70) > 0) {
                        *(s32*)(sprite + 0x70) = (s32)(((s64) * (s32*)(sprite + 0x70) * -data_ov039_0209a300 + 0x800) >> 12);
                    }
                }

                Sprite_Update((Sprite*)sprite);
                *(s32*)(sprite + 0x80) = 1;
            }
        }
    } else {
        *(s32*)(sprite + 0x7C) = 0;
        *(s32*)(sprite + 0x80) = 0;
    }
    return 1;
}

/**
 * @brief Steps the sprite block at task+0x18 when +0x80 is set.
 *
 * `func_ov039_02094dac` again, one guard word further along.
 */
s32 func_ov039_020982e8(void* pool, void* task) {
    Sprite* sprite = *(Sprite**)((u8*)task + 0x18);

    (void)pool;

    if (*(s32*)((u8*)sprite + 0x80) != 0) {
        *(s16*)((u8*)sprite + 0x0C) = (s16)((*(s32*)((u8*)sprite + 0x58) - *(s32*)((u8*)sprite + 0x50)) >> 12);
        *(s16*)((u8*)sprite + 0x0E) =
            (s16)((*(s32*)((u8*)sprite + 0x60) + (*(s32*)((u8*)sprite + 0x5C) - *(s32*)((u8*)sprite + 0x54))) >> 12);
        Sprite_RenderFrame(sprite);
    }
    return 1;
}

/** Releases the sprite at task+0x18 and reports success. */
s32 func_ov039_02098338(void* pool, void* task) {
    (void)pool;
    Sprite_Release(*(Sprite**)((u8*)task + 0x18));
    return 1;
}

/** The same dispatcher against data_ov039_0209a030. */
void func_ov039_0209834c(void* a, void* b, void* c, s32 index) {
    TaskStages table = data_ov039_0209a030;

    table.iter[index](a, b, c);
}
