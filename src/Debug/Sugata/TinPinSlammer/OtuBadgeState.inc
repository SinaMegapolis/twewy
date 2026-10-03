/* ============================================================================
 * Band B3 -- Tsk_OtosuGame_badge, 0x0208acc0 - 0x0208c4ec.
 *
 * Claim range: .text 0x0208acc0 .. 0x0208c4ec (22 functions, 6188 bytes,
 * 0x0208acc0 .. 0x0208c45c). It sits in the gap between OtuFieldAccess.c's
 * own claim (0x02088400 - 0x02098b8c) and band 1 (0x0208f000 - 0x02090000),
 * and like bands 6, 7 and 13 it is one whole task rather than a slice of a
 * call graph: nothing in the band calls anything in another band except the
 * shared vector unit, and nothing outside the band calls into it except the
 * overlay's own dispatcher.
 *
 * WHAT THE TASK IS
 * ----------------
 * A "badge" is one of the small round tokens that slides around the Tin Pin
 * Slammer board under the player's own pin. The task owns a badge's whole
 * simulation: a position in Q12.12, a velocity, a rolling set of per-tile
 * behaviours, and a wheel of AI decisions that re-rolls whenever the badge
 * arrives somewhere new.
 *
 * The state block is 0x25C bytes and is driven by a phase counter at +0xF8
 * (2..9) with a finer `step` counter at +0xF4 underneath it. Each phase has
 * its own one-shot entry setter, and every setter opens with the same four
 * instructions: load the phase, compare it against its own number, return if
 * it already matches. So a phase transition is idempotent and the setters
 * double as "have we reached this phase yet" tests. That is the clearest
 * single piece of evidence for what the block is, and it is why 0x0208ad88,
 * 0x0208ade8 and 0x0208ae14 are so alike.
 *
 * STRUCTURAL FINDINGS
 * -------------------
 *   * +0x100 is not a scalar. It is the head of a 0xA0-byte timer block: a
 *     32-bit frame budget at +0x100 (0x1E / 0x3C / 9 depending on phase), the
 *     last-observed tile type and tile cell at +0x104/+0x108/+0x10C, six
 *     16-bit countdown slots at +0x178..+0x17E and +0x19C/+0x19E, and the
 *     trailing trail cursor. They are collected into `OtuTimers` below rather
 *     than left as loose fields, because every one of them is written by a
 *     `ldrsh`/`sub 1`/`strh` triple or a plain word store against the same
 *     base and nothing outside this band reaches into the middle of it.
 *   * +0xE4 is a bare byte cursor, not a struct pointer, and it is used two
 *     different ways. Bytes +2, +3 and +5 are the board's own description
 *     (tile width, tile height, and a flag); a word at +0x10 is the grid of
 *     per-cell tile bytes; and bytes +8 onwards are a two-byte-per-badge
 *     table of starting tiles, which is why the phase-5 entry forms
 *     `add r1, board, index, lsl #1` and then loads at +8 and +9. Reading it
 *     as one struct would lose that second indexing, so it stays a `u8*` and
 *     the layout is documented here instead.
 *   * +0x170 is a pointer to the per-badge-kind behaviour records, indexed by
 *     the tray slot at +0x16C on a 0x1C-byte stride. That record's +0x04 byte
 *     is itself an index into a Q12.12 speed table, so the chain is
 *     tray slot -> behaviour record -> speed -> rate table.
 *   * Every wall bounce masks with 0x1FFFF and then folds the position back to
 *     a cell boundary: the board is a grid of 0x20000 (2.0 in Q12.12) cells
 *     and a badge is snapped to cell edges rather than to exact coordinates.
 *     0x20000 is also the stride the spiral search at 0x0208bed8 walks.
 *   * The two bounce constants are not the same. Moving *into* a wall scales
 *     the velocity by -0x2000; moving *away* from one negates it outright.
 *     The target derives -0x2000 by arithmetic-shifting the very mask constant
 *     it has already materialised -- `(~0x1FFFF) >> 17` -- which is why it
 *     reads as an unrelated constant in the disassembly and shares the
 *     materialisation with the mask beside it.
 * ==========================================================================*/

#include "Debug/Sugata/TinPinSlammer.h"
#include "Engine/EasyTask.h"
#include "Engine/Math/Random.h"
#include <nitro/fx/fx_atan.h>
#include <nitro/fx/fx_division.h>

/* ------------------------------------------------------------------ */
/* Data the band reads but does not define.                            */
/* ------------------------------------------------------------------ */

/*
 * Every symbol here lives in the overlay's own gap-filled `.data` and keeps
 * the build's name for it. Declaring an object of our own would let the
 * linker place it at an address of its choosing rather than the one the target
 * uses, which costs the pool word; see the same note in band 5 and band 13.
 */

/** 0x133 -- the badge's per-frame acceleration, in Q12.12. */
extern s32 data_ov039_0209a318;

/** -0x2000 -- the velocity the phase-7 entry sets going through a corner. */
extern s32 data_ov039_0209a31c;

/** -0x2000 again, as the phase-9 entry's terminal velocity. */
extern s32 data_ov039_0209a320;

/** 0x800 -- the bounce re-normalisation factor. */
extern s32 data_ov039_0209a324;

/** 0x8000 -- the axis speed the four tile-driven launch cases set. */
extern s32 data_ov039_0209a308;

/*
 * The movement-rate table, four words, is declared as
 * `data_ov039_0209a39c[4]` in TinPinSlammer.h -- func_ov039_02084020 and
 * func_ov039_020840c0 reach the same four words from OtuScoreRow.c.
 */

/**
 * @brief One badge kind's speed record: a 16-byte-stride Q12.12 multiplier.
 *
 * Indexed by the behaviour record's +0x04 byte, which is why the target gets
 * the stride as a shift rather than as a scale. Only the leading word is read
 * by this band; the other three are whatever the board data carries.
 */
typedef struct {
    /* 0x00 */ s32 speed; // Q12.12 multiplier applied to the badge's velocity
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ s32 unk_08;
    /* 0x0C */ s32 unk_0C;
} OtuBadgeSpeed;          // Size: 0x10
extern OtuBadgeSpeed data_ov039_0209a3e0[];

/**
 * The base module's sine/cosine table, indexed by `(angle >> 4) * 2`.
 *
 * `entry[n * 2]` is the cosine and `entry[n * 2 + 1]` the sine; 0x0208af6c
 * puts the sine on x and the cosine on y, which is the overlay's convention
 * for turning a heading into a direction vector.
 */
/* declared by band 3 as s32[]; bands 5 and 7 read it through a s16* cast -- declared/defined elsewhere in this TU; see the note
 * below on why band 11 does not repeat it. */

/* ------------------------------------------------------------------ */
/* Task-data shapes.                                                   */
/* ------------------------------------------------------------------ */

/**
 * @brief Where a badge was spawned: one flag byte and its starting tile.
 *
 * Read through +0xE8. Bit 0 of `flags` is copied straight into bit 0 of the
 * badge's packed state word by 0x0208acc0, which is the only thing that ties
 * the two objects together; `tileX`/`tileY` are the tile the badge belongs to
 * and are shifted up by twelve to become Q12.12.
 */
typedef struct {
    /* 0x0 */ u8 flags; // bit 0 is mirrored into the badge's stateFlags
    /* 0x1 */ u8 pad_01;
    /* 0x2 */ u8 tileX;
    /* 0x3 */ u8 tileY;
} OtuHomePos;            // Size: 0x4

/**
 * @brief One badge kind's behaviour record, 0x1C-byte stride.
 *
 * Reached through +0x170 and indexed by the tray slot at +0x16C, so the stride
 * is a `mla` against 0x1C rather than an array subscript the compiler could
 * fold away. The two groups of fields serve two different phases and nothing
 * suggests they are related: +0x04/+0x06 drive the rolling phase, +0x08
 * onward the scripted set-piece phases.
 */
typedef struct {
    /* 0x00 */ u8  pad_00[0x04];
    /* 0x04 */ u8  speedIndex; // index into data_ov039_0209a3e0
    /* 0x05 */ u8  pad_05;
    /* 0x06 */ s16 friction;   // Q12.12, scaled by unk_144 / 3 into a heading offset
    /* 0x08 */ u16 animX;
    /* 0x0A */ u16 animY;
    /* 0x0C */ u8  pad_0C[0x04];
    /* 0x10 */ s16 curveA;
    /* 0x12 */ s16 curveB;
    /* 0x14 */ u16 curveC;
    /* 0x16 */ u16 curveD;
    /* 0x18 */ u8  pad_18[0x04];
} OtuBadgeSlot;          // Size: 0x1C

/**
 * @brief The badge's six 16-bit countdown slots, 0x28 bytes at +0x178.
 *
 * Every one is read-modify-written with a `ldrsh`/`sub 1`/`strh` triple, which
 * is what identifies them as counters that are allowed to go negative rather
 * than as flags, and each is the one and only guard on its phase's entry
 * setter.
 *
 * The task's own code reaches them through a `+0x100` base register and then a
 * small displacement -- `add rX, self, #0x100` followed by `ldrsh [rX, #0x78]`
 * -- rather than folding 0x178 into the load. Keeping them in a struct of their
 * own at 0x178 preserves that split, because the displacement is too large for
 * the short form to be what this build emits.
 */
typedef struct {
    /* 0x00 */ s16 trackFrames; // self +0x178: phase 6
    /* 0x02 */ s16 bounceTimer; // self +0x17A: phase 8
    /* 0x04 */ s16 arcFrames;   // self +0x17C: phase 7
    /* 0x06 */ s16 spinFrames;  // self +0x17E: phase 9
    /* 0x08 */ u8  pad_08[0x1C];
    /* 0x24 */ s16 trailIndex;  // self +0x19C: cursor into the trail id table
    /* 0x26 */ s16 trailTimer;  // self +0x19E: frames between trail marks
} OtuTimers;               // Size: 0x28

/**
 * @brief "Tsk_OtosuGame_badge", 0x25C bytes of state.
 *
 * The same object is modelled twice more in this translation unit:
 * OtuPinTray.inc as `OtuBadge` (the task's own view) and the header as the
 * 0x174 prefix `OtuPinTask` (the view the pool queries use). The three share
 * the handle at 0x0209926c; they are not three tasks.
 *
 * Every offset below is fixed by at least one load or store in this band, and
 * the padding is sized so each field lands on the displacement the target uses.
 * The six point-shaped regions are `OtuPoint` because the overlay's vector
 * helpers take them by address and this band hands them straight to
 * `func_ov039_02098b8c` and friends.
 *
 * `stateFlags` is a packed word rather than a set of booleans: bit 0 mirrors
 * the home tile's flag, bit 1 records that the badge disagrees with it, bit 2
 * records that it used to agree. `contactFlags` is a mask of the set-piece
 * phases allowed to fire this frame.
 */
typedef struct {
    /* 0x000 */ TinPinSlammer_Scene* scene;
    /* 0x004 */ u8                pad_004[0x04];
    /* 0x008 */ TaskPool*         pool;
    /* 0x00C */ u8                pad_00C[0xD0];
    /* 0x0DC */ s32               unk_0DC;    // cleared on arriving at tile 12
    /* 0x0E0 */ s32               index;      // which badge this is
    /* 0x0E4 */ u8*               board;      // see the note at the top of the file
    /* 0x0E8 */ OtuHomePos*       home;
    /* 0x0EC */ u16               unk_0EC;
    /* 0x0EE */ u16               contactFlags;
    /* 0x0F0 */ u16               stateFlags;
    /* 0x0F2 */ u8                pad_0F2[0x02];
    /* 0x0F4 */ s32               step;       // sub-phase within `phase`
    /* 0x0F8 */ s32               phase;      // 2..9
    /* 0x0FC */ s32               unk_FC;
    /* 0x100 */ s32               frameBudget;// the current phase's frame budget
    /* 0x104 */ s32               lastTileType;
    /* 0x108 */ s32               lastCellX;
    /* 0x10C */ s32               lastCellY;
    /* 0x110 */ OtuPoint          origin;     // the sound cue's anchor
    /* 0x118 */ OtuPoint          unk_118;    // shared offset added to both aim points
    /* 0x120 */ OtuPoint          pos;        // Q12.12
    /* 0x128 */ s32               unk_128;    // "something else is steering", in frames
    /* 0x12C */ OtuPoint          vel;        // Q12.12
    /* 0x134 */ s32               unk_134;    // per-frame velocity nudge
    /* 0x138 */ OtuPoint          velNorm;    // the last unit vector taken
    /* 0x140 */ s32               unk_140;
    /* 0x144 */ s32               unk_144;    // the wobble, in Q12.12
    /* 0x148 */ s32               unk_148;    // frames left of a live interaction
    /* 0x14C */ OtuPoint          startPos;   // where the current aim began
    /* 0x154 */ OtuPoint          curPos;     // where it is aiming now
    /* 0x15C */ s32               unk_15C;    // the phase-5 start x, in Q12.12
    /* 0x160 */ s32               unk_160;    // the phase-5 start y, in Q12.12
    /* 0x164 */ s32               unk_164;
    /* 0x168 */ u8                pad_168[0x04];
    /* 0x16C */ u16*              pinID;      // a tray slot's value; 0x130 = no pin
    /* 0x170 */ OtuBadgeSlot*     slots;      // per-kind behaviour records, 0x1C stride
    /* 0x174 */ u16*              chanceTbl;  // one 0x10000 chance per AI
    /* 0x178 */ OtuTimers         timers;
    /* 0x1A0 */ u8                pad_1A0[0x04];
    /* 0x1A4 */ s32               unk_1A4;    // set to 1 when phase 5 starts
    /* 0x1A8 */ s32               unk_1A8;    // non-zero: fade the display out first
    /* 0x1AC */ s32               unk_1AC;    // stepped by the sound helper
    /* 0x1B0 */ void*             unk_1B0;    // live sound handle, retired at phase 4
    /* 0x1B4 */ s32               curAI;      // which AI is driving; 0x11 = parked
    /* 0x1B8 */ s32               unk_1B8;
    /* 0x1BC */ OtuPinTask*       chaseTarget;
    /* 0x1C0 */ OtuPoint          anchorPt;   // where the current AI sent us
    /* 0x1C8 */ u8                pad_1C8[0x04];
    /* 0x1CC */ s32               unk_1CC;    // frames spent aiming; 0x1E = commit
    /* 0x1D0 */ s32               mode;       // 0x10 once the badge is committed
    /* 0x1D4 */ u8                pad_1D4[0x10];
    /* 0x1E4 */ u32               taskId1;    // the arc task
    /* 0x1E8 */ u32               taskId2;    // the track task
    /* 0x1EC */ u32               taskId3;    // the spin task
    /* 0x1F0 */ u8                pad_1F0[0x08];
    /* 0x1F8 */ u32               trailId[12];
    /* 0x228 */ u8                pad_228[0x30];
    /* 0x258 */ u32               taskId;     // the badge's own sprite task
} OtuBadgeState; // Size: 0x25C

/* ------------------------------------------------------------------ */
/* Declarations this TU does not otherwise have.                       */
/* ------------------------------------------------------------------ */

/*
 * Everything below is outside the band and outside OtuFieldAccess's own
 * bodies, so nothing in this translation unit declares it. Undefined callees
 * are not a problem: dsd resolves them against the original overlay.
 */

/** The shared vector unit's normalise helper. */

/*
 * func_ov039_02098b78 is really a point copy (`out = *(OtuPoint*)src`) but
 * OtuFieldAccess.c above declares it as the dispatch container's word pair,
 * and that declaration is what this TU sees. It is called through that
 * prototype here; the type is being corrected separately, not here.
 */

/** The board's tile lookup: which tile kind is under this Q12.12 point. */
/* defined in band 10 as u8 (OtuPoint*, OtuCellGrid*) -- declared/defined elsewhere in this TU; see the note
 * below on why band 11 does not repeat it. */

/** Scales a direction to `scale`, applies it to the badge, then plays it. */
/* defined in band 10 as void (OtuPinLogic*, OtuPoint*, s32) -- declared/defined elsewhere in this TU; see the note
 * below on why band 11 does not repeat it. */

/** Joins two directions through `out` and returns the length; 0 = unreachable. */
s32 func_ov039_0208a624(OtuPoint* a, OtuPoint* b, OtuPoint* out);

/** The turn between two directions, against the 0x24000 turn radius. */
s32 func_ov039_0208a530(OtuPoint* a, OtuPoint* b, OtuPoint* out);

/** The arc task's per-frame stepper, driven from phase 7. */
void func_ov039_02091028(void* task, s32 x, s32 y, s32 z, u16 w);

/** The overlay's nearest-child queries, by pin kind. */
OtuPinTask* func_ov039_02087f4c(TaskPool* pool, TinPinSlammer_Scene* scene, s32 which);
OtuPinTask* func_ov039_02088064(TaskPool* pool, TinPinSlammer_Scene* scene, s32 which);

/* ------------------------------------------------------------------ */
/* The band.                                                           */
/* ------------------------------------------------------------------ */

/**
 * The mask of the sub-tile bits of one board cell.
 *
 * A cell is 0x20000 across in Q12.12, so this is `0x20000 - 1`: masking with it
 * and then adding or subtracting a whole cell walks from one cell boundary to
 * the next without having to know where in the cell the badge actually is.
 */
#define OTU_CELL_BITS 0x1FFFF

/** One board cell, in Q12.12. */
#define OTU_CELL_SIZE 0x20000

/*
 * Every Q12.12 multiply below is written out rather than hidden behind a macro
 * so each one can be read against the `smull`/`adds #0x800`/`adc`/`lsr #0xC`
 * quartet it produces. The rounding constant is in all of them: the target
 * rounds to nearest, and dropping it is a one-bit-per-call difference that a
 * match will not forgive.
 */

/** Rebuilds the packed state word from the home tile's flag. */
void func_ov039_0208acc0(OtuBadgeState* self) {
    u16 home  = (self->home->flags & 1) != 0 ? 1 : 0;
    u16 flags = self->stateFlags;

    // Bit 1 records "the badge is not where its home tile says it is". The
    // condition is the XOR masked back down to one bit rather than a plain
    // compare, which is what produces the target's eor/and/tst trio.
    if ((flags ^ home) & home & 1) {
        flags |= 2;
    } else {
        flags &= ~2;
    }
    self->stateFlags = flags;

    // Bit 2 records the transition itself, so it runs the other way round: the
    // badge has to have been home and no longer is.
    flags = self->stateFlags;
    if ((flags ^ home) & flags & 1) {
        flags |= 4;
    } else {
        flags &= ~4;
    }
    self->stateFlags = flags;

    // Bit 0 is not a flag at all; it is the home tile's bit 0 latched in.
    self->stateFlags &= ~1;
    self->stateFlags |= home;
}

/**
 * Scales a velocity down to at most `shortfall`, never up.
 *
 * The magnitude is taken first, so the vector is left normalised on return,
 * and a zero-length vector is left alone rather than normalised into the
 * direction of nothing.
 */
void func_ov039_0208ad2c(OtuPoint* vel, s32 shortfall) {
    OtuPoint norm;
    OtuPoint zero;
    s32      len = func_ov039_02098d10(vel);
    s32      scale;

    if (len <= 0) {
        return;
    }

    scale  = len - shortfall;
    zero.x = 0;
    zero.y = 0;

    // `shortfall` is an allowance, not a demand: a vector already shorter than
    // it is left alone rather than stretched.
    if (scale < 0) {
        scale = 0;
    }

    func_ov039_02098d3c(vel, &norm);
    func_ov039_02098c00(scale, &norm, &zero, vel);
}

/** Enters phase 2: pick the badge's sprite up and send it back to the home tile. */
void func_ov039_0208ad88(OtuBadgeState* self) {
    if (self->phase == 2) {
        return;
    }

    func_ov039_02097aa4(EasyTask_GetTaskData(self->pool, self->taskId), (void*)&self->pos, 1);
    func_ov039_02087d04(0x343, &self->pos, &self->origin);

    self->vel.x  = 0;
    self->vel.y  = 0;
    self->step   = 0;
    self->phase  = 2;
    self->unk_FC = 0;
}

/** Enters phase 3: sit still for 0x1E frames with the badge visible. */
void func_ov039_0208ade8(OtuBadgeState* self) {
    if (self->phase == 3) {
        return;
    }

    self->step          = 0;
    self->phase         = 3;
    self->frameBudget = 0x1E;
    self->unk_148       = 0;
}

/** Enters phase 4: retire whatever the badge was attached to. */
void func_ov039_0208ae14(OtuBadgeState* self) {
    if (self->phase == 4) {
        return;
    }

    // A badge still carrying a sound handle hands it back with a different cue
    // depending on whether it had a live rival to chase.
    if (self->unk_1B0 != NULL && *self->pinID < 0x130) {
        func_ov039_0208a490(self, self->unk_148 > 0 ? 5 : 2);
    }
    self->unk_1B0 = NULL;

    func_ov039_02087dc0(self->scene, self);

    self->unk_148       = 0;
    self->step          = 0;
    self->phase         = 4;
    self->frameBudget = 0x3C;
}

/** Enters phase 5: place the badge on its starting tile and start it rolling. */
void func_ov039_0208ae8c(OtuBadgeState* self) {
    if (self->unk_1A8 != 0) {
        EasyFade_FadeMainDisplay(2, 0x10, 0x1000);
    }

    self->unk_140 = 0;
    self->unk_144 = 0;
    self->unk_164 = 0x50;
    self->step    = 0;
    self->phase   = 5;
    self->unk_FC  = 0;

    // Tiles are 0x20 pixels and every badge is inset by half a tile, which is
    // what lands it in the middle of its cell rather than on a corner. The
    // starting tile comes from the board's two-bytes-per-badge table at +8.
    self->unk_15C = ((self->board[self->index * 2 + 8] << 5) + 0x10) << 0xC;
    self->unk_160 = ((self->board[self->index * 2 + 9] << 5) + 0x10) << 0xC;
    self->pos.x   = self->unk_15C;
    self->pos.y   = self->unk_160;
    self->vel.x   = 0;
    self->vel.y   = 0;

    self->velNorm.x = 0x1000;
    self->velNorm.y = 0;
    self->unk_148   = 0;
    self->unk_1A4   = 1;
}

/** Enters phase 7's approach: kill the velocity and play the corner sound. */
void func_ov039_0208af38(OtuBadgeState* self) {
    self->unk_134 = data_ov039_0209a31c;
    self->step    = 0;

    func_ov039_02087d04(0x344, &self->pos, &self->origin);
}

/**
 * The rolling phase: accelerate, turn, clamp to the board, bounce off walls
 * and wall corners, drop trail marks, and finally react to the tile the badge
 * has just arrived on.
 */
void func_ov039_0208af6c(OtuBadgeState* self) {
    OtuPoint       probe;
    OtuPoint       norm;
    OtuPoint       zero;
    OtuBadgeSlot*  slot;
    OtuBadgeSpeed* speed;
    s32            tile;
    s32            cellX;
    s32            cellY;
    s32            len;
    s32            hit;

    self->unk_134 = self->unk_134 + data_ov039_0209a318;

    // Acceleration, unless something has already committed the badge to a
    // mode of its own.
    if (self->mode <= 0) {
        s32 rate;

        if (self->unk_128 == 0) {
            tile = func_ov039_0208a794(&self->pos, (OtuCellGrid*)self->board);

            switch (tile) {
                case 3:
                    rate = data_ov039_0209a39c[2];
                    break;

                case 4:
                    rate = data_ov039_0209a39c[1];
                    break;

                default:
                    rate = data_ov039_0209a39c[0];
                    break;
            }
        } else {
            rate = data_ov039_0209a39c[3];
        }

        slot  = &self->slots[self->pinID[0]];
        speed = &data_ov039_0209a3e0[slot->speedIndex];

        rate = (s32)(((s64)speed->speed * rate + 0x800) >> 12);

        // While the badge is still being set up its rate is doubled. The test
        // is `phase <= 3`, i.e. every phase before the rolling one.
        if (self->phase <= 3) {
            rate = rate * 2;
        }

        func_ov039_0208ad2c(&self->vel, rate);
    }

    // Turning, but only while nothing external is steering the badge.
    if (self->unk_128 == 0) {
        s32 turn;
        s16 dir;
        s32 cell;

        len  = func_ov039_02098d10(&self->vel);
        slot = &self->slots[self->pinID[0]];

        // How hard the badge curves is the friction coefficient scaled by the
        // wobble 0x0208be30 left behind.
        turn = (s32)(((s64)slot->friction * (self->unk_144 / 3) + 0x800) >> 12);

        dir  = FX_Atan2Idx(self->vel.y, self->vel.x) + turn;
        cell = dir >> 4;

        /* The sin/cos table is s32[] by band 3's declaration but holds pairs of
         * s16, so each element has to be fetched through a s16* at a doubled
         * byte offset -- the same shape bands 5 and 7 use. */
        self->vel.x = (s32)(((s64)len *
                          *(s16*)((u8*)&data_0205e4e0 + (cell * 2 + 1) * 2) + 0x800) >> 12);
        self->vel.y = (s32)(((s64)len *
                          *(s16*)((u8*)&data_0205e4e0 + (cell * 2) * 2) + 0x800) >> 12);
    }

    func_ov039_02098b8c(&self->pos, &self->vel, &self->pos);

    // The board is a fixed playfield, so both axes are clamped to a hard inset
    // rather than to whatever extent the tile lookup would accept.
    if (self->pos.x < 0x80000) {
        self->pos.x = 0x80000;
    } else {
        s32 limit = ((s32)self->board[2] << 5) - 0x80;

        if (self->pos.x >= (limit << 0xC)) {
            self->pos.x = (limit << 0xC) - 1;
        }
    }
    if (self->pos.y < 0x60000) {
        self->pos.y = 0x60000;
    } else {
        s32 limit = ((s32)self->board[3] << 5) - 0x60;

        if (self->pos.y >= (limit << 0xC)) {
            self->pos.y = (limit << 0xC) - 1;
        }
    }

    // A negative accumulator is an impulse off something solid. Once it has
    // been absorbed both accumulators are cleared together.
    self->unk_128 = self->unk_128 + self->unk_134;
    if (self->unk_128 > 0) {
        self->unk_134 = 0;
        self->unk_128 = 0;
    }

    // Trail marks, dropped every few frames while the badge is moving fast.
    if (self->unk_128 == 0 && self->phase != 3) {
        s32 trailLen = func_ov039_02098d10(&self->vel);

        if (trailLen > 0x1000) {
            s32 angle;

            self->timers.trailTimer = self->timers.trailTimer - 1;
            if (self->timers.trailTimer <= 0) {
                angle = FX_Atan2Idx(self->vel.y, self->vel.x);

                // Two halves, so the mark is centred on the badge rather than
                // trailing off one side of it.
                func_ov039_02095788(
                    (void*)EasyTask_GetTaskData(self->pool, self->trailId[self->timers.trailIndex]),
                    (void*)&self->pos, angle, 1, trailLen);
                self->timers.trailIndex = self->timers.trailIndex + 1;

                func_ov039_02095788(
                    (void*)EasyTask_GetTaskData(self->pool, self->trailId[self->timers.trailIndex]),
                    (void*)&self->pos, angle, 0, trailLen);
                self->timers.trailIndex = self->timers.trailIndex + 1;

                if (self->timers.trailIndex >= 0xC) {
                    self->timers.trailIndex = 0;
                }
                self->timers.trailTimer = 5;
            }
        }
    }

    // Walls. A wall is tile type 2; the badge is folded back onto the boundary
    // it crossed rather than stopped at the probe point. An axis that has
    // already reflected suppresses all four corner tests below.
    if (self->unk_128 == 0) {
        hit = 0;

        if (self->vel.x > 0) {
            func_ov039_02098b78((OtuStageDispatch*)&self->pos, (s32*)&probe);
            probe.x = probe.x + 0xC000;
            tile     = func_ov039_0208a794(&probe, (OtuCellGrid*)self->board);
            if (tile == 2) {
                self->pos.x = (probe.x & ~OTU_CELL_BITS) - 0xC000;
                self->vel.x = self->vel.x * -0x2000;
                hit        = 1;
            }
        } else if (self->vel.x < 0) {
            func_ov039_02098b78((OtuStageDispatch*)&self->pos, (s32*)&probe);
            probe.x = probe.x - 0xC000;
            tile     = func_ov039_0208a794(&probe, (OtuCellGrid*)self->board);
            if (tile == 2) {
                self->pos.x = (probe.x | OTU_CELL_BITS) + 0xC000;
                self->vel.x = -self->vel.x;
                hit        = 1;
            }
        }

        if (self->vel.y > 0) {
            func_ov039_02098b78((OtuStageDispatch*)&self->pos, (s32*)&probe);
            probe.y = probe.y + 0xC000;
            tile     = func_ov039_0208a794(&probe, (OtuCellGrid*)self->board);
            if (tile == 2) {
                self->pos.y = (probe.y & ~OTU_CELL_BITS) - 0xC000;
                self->vel.y = self->vel.y * -0x2000;
                hit        = 1;
            }
        } else if (self->vel.y < 0) {
            func_ov039_02098b78((OtuStageDispatch*)&self->pos, (s32*)&probe);
            probe.y = probe.y - 0xC000;
            tile     = func_ov039_0208a794(&probe, (OtuCellGrid*)self->board);
            if (tile == 2) {
                self->pos.y = (probe.y | OTU_CELL_BITS) + 0xC000;
                self->vel.y = -self->vel.y;
                hit        = 1;
            }
        }

        // Corners. The probe is pushed a whole cell along each velocity
        // component, snapped to the cell boundary, and only then measured: a
        // corner the badge could follow round is not worth reversing for, so
        // it has to be closer than the reversal's own distance.
        if (hit == 0) {
            if (self->vel.x > 0 || self->vel.y < 0) {
                func_ov039_02098b78((OtuStageDispatch*)&self->pos, (s32*)&probe);
                probe.x = probe.x + OTU_CELL_SIZE;
                probe.y = probe.y - OTU_CELL_SIZE;
                tile     = func_ov039_0208a794(&probe, (OtuCellGrid*)self->board);
                if (tile == 2) {
                    probe.x = probe.x & ~OTU_CELL_BITS;
                    probe.y = probe.y | OTU_CELL_BITS;
                    len     = func_ov039_02098ca8(&self->pos, &probe);
                    if (len < 0xC000) {
                        self->pos.x = probe.x - 0xC000;
                        hit        = 1;
                        self->pos.y = probe.y + 0xC000;
                        self->vel.x = -self->vel.x;
                        self->vel.y = -self->vel.y;
                    }
                }
            }

            if (self->vel.x > 0 || self->vel.y > 0) {
                func_ov039_02098b78((OtuStageDispatch*)&self->pos, (s32*)&probe);
                probe.x = probe.x + OTU_CELL_SIZE;
                probe.y = probe.y + OTU_CELL_SIZE;
                tile     = func_ov039_0208a794(&probe, (OtuCellGrid*)self->board);
                if (tile == 2) {
                    probe.x = probe.x & ~OTU_CELL_BITS;
                    probe.y = probe.y & ~OTU_CELL_BITS;
                    len     = func_ov039_02098ca8(&self->pos, &probe);
                    if (len < 0xC000) {
                        self->pos.x = probe.x - 0xC000;
                        hit        = 1;
                        self->pos.y = probe.y - 0xC000;
                        self->vel.x = -self->vel.x;
                        self->vel.y = -self->vel.y;
                    }
                }
            }

            if (self->vel.x < 0 || self->vel.y < 0) {
                func_ov039_02098b78((OtuStageDispatch*)&self->pos, (s32*)&probe);
                probe.x = probe.x - OTU_CELL_SIZE;
                probe.y = probe.y - OTU_CELL_SIZE;
                tile     = func_ov039_0208a794(&probe, (OtuCellGrid*)self->board);
                if (tile == 2) {
                    probe.x = probe.x | OTU_CELL_BITS;
                    probe.y = probe.y | OTU_CELL_BITS;
                    len     = func_ov039_02098ca8(&self->pos, &probe);
                    if (len < 0xC000) {
                        self->pos.x = probe.x + 0xC000;
                        hit        = 1;
                        self->pos.y = probe.y + 0xC000;
                        self->vel.x = -self->vel.x;
                        self->vel.y = -self->vel.y;
                    }
                }
            }

            if (self->vel.x < 0 || self->vel.y > 0) {
                func_ov039_02098b78((OtuStageDispatch*)&self->pos, (s32*)&probe);
                probe.x = probe.x - OTU_CELL_SIZE;
                probe.y = probe.y + OTU_CELL_SIZE;
                tile     = func_ov039_0208a794(&probe, (OtuCellGrid*)self->board);
                if (tile == 2) {
                    probe.x = probe.x | OTU_CELL_BITS;
                    probe.y = probe.y & ~OTU_CELL_BITS;
                    len     = func_ov039_02098ca8(&self->pos, &probe);
                    if (len < 0xC000) {
                        self->pos.x = probe.x + 0xC000;
                        hit        = 1;
                        self->pos.y = probe.y - 0xC000;
                        self->vel.x = -self->vel.x;
                        self->vel.y = -self->vel.y;
                    }
                }
            }
        }

        // Having hit something, re-normalise so the next turn is taken from the
        // reflected heading rather than the incoming one.
        if (hit != 0) {
            zero.x = 0;
            zero.y = 0;

            len = func_ov039_02098d10(&self->vel);
            if (len > 0) {
                func_ov039_02098d3c(&self->vel, &norm);
                func_ov039_02098c00((s32)(((s64)len * data_ov039_0209a324 + 0x800) >> 12),
                                    &norm, &zero, &self->vel);
                self->velNorm = norm;
            }
        }
    }

    // `unk_128` is the "something else owns this badge" latch; while it is set
    // the tile under the badge is not read at all.
    if (self->unk_128 != 0) {
        return;
    }

    tile = func_ov039_0208a794(&self->pos, (OtuCellGrid*)self->board);

    if (tile == 0) {
        func_ov039_0208ade8(self);
    } else if (tile == 12) {
        self->unk_0DC = 0;
    }

    // Tile changes are detected by the cell the badge is standing in rather
    // than by its position, so a badge drifting inside one tile does not
    // re-trigger anything.
    cellX = (self->pos.x >> 0xC) / 32;
    cellY = (self->pos.y >> 0xC) / 32;
    if (self->lastCellX == (cellX >> 5) && self->lastCellY == (cellY >> 5)) {
        return;
    }

    switch (tile) {
        case 5:
            // Only the board that admits it has a home to go back to.
            if (self->board[5] != 0) {
                func_ov039_0208ad88(self);
            }
            break;

        case 7:
            func_ov039_0208af38(self);
            break;

        case 8:
            self->vel.x = 0;
            self->vel.y = -data_ov039_0209a308;
            func_ov039_02098d3c(&self->vel, &self->velNorm);
            func_ov039_02087d04(0x345, &self->pos, &self->origin);
            break;

        case 9:
            self->vel.x = data_ov039_0209a308;
            self->vel.y = 0;
            func_ov039_02098d3c(&self->vel, &self->velNorm);
            func_ov039_02087d04(0x345, &self->pos, &self->origin);
            break;

        case 10:
            self->vel.x = 0;
            self->vel.y = data_ov039_0209a308;
            func_ov039_02098d3c(&self->vel, &self->velNorm);
            func_ov039_02087d04(0x345, &self->pos, &self->origin);
            break;

        case 11:
            self->vel.x = -data_ov039_0209a308;
            self->vel.y = 0;
            func_ov039_02098d3c(&self->vel, &self->velNorm);
            func_ov039_02087d04(0x345, &self->pos, &self->origin);
            break;
    }

    self->lastTileType = tile;
    self->lastCellX    = cellX >> 5;
    self->lastCellY    = cellY >> 5;
}

/** Enters phase 6: hand the badge to the track task for `trackFrames`. */
void func_ov039_0208b94c(OtuBadgeState* self) {
    OtuBadgeSlot* slot;

    if (self->timers.trackFrames <= 0) {
        return;
    }

    self->step  = 0;
    self->phase = 6;

    slot = &self->slots[self->pinID[0]];
    func_ov039_02091654(EasyTask_GetTaskData(self->pool, self->taskId2), slot->animX, slot->animY);

    self->timers.trackFrames = self->timers.trackFrames - 1;
}

/** Enters phase 7: hand the badge to the arc task for `arcFrames`. */
void func_ov039_0208b9b4(OtuBadgeState* self) {
    OtuBadgeSlot* slot;

    if (self->timers.arcFrames <= 0) {
        return;
    }

    self->step  = 0;
    self->phase = 7;

    slot = &self->slots[self->pinID[0]];
    func_ov039_02091028(EasyTask_GetTaskData(self->pool, self->taskId1),
                        slot->curveA, slot->curveB, slot->curveC, slot->curveD);

    self->timers.arcFrames = self->timers.arcFrames - 1;
}

/** Enters phase 8: stop dead for `bounceFrames`. */
void func_ov039_0208ba34(OtuBadgeState* self) {
    if (self->timers.bounceTimer <= 0) {
        return;
    }

    self->vel.x  = 0;
    self->vel.y  = 0;
    self->phase  = 8;
    self->step   = 0;
    self->unk_FC = 0;

    self->timers.bounceTimer = self->timers.bounceTimer - 1;
}

/** Enters phase 9: spin down to a stop over `spinFrames`. */
void func_ov039_0208ba70(OtuBadgeState* self) {
    OtuPoint zero;

    if (self->timers.spinFrames <= 0) {
        return;
    }

    zero.x = 0;
    zero.y = 0;

    func_ov039_02098c00(data_ov039_0209a38c, &self->velNorm, &zero, &self->vel);
    func_ov039_02098d3c(&self->vel, &self->velNorm);

    self->unk_134       = data_ov039_0209a320;
    self->step          = 0;
    self->phase         = 9;
    self->step          = 0;
    self->frameBudget = 9;

    func_ov039_02091b34(EasyTask_GetTaskData(self->pool, self->taskId3));

    if (self->unk_148 > 0) {
        func_ov039_0208a490(self, 1);
    }

    self->timers.spinFrames = self->timers.spinFrames - 1;
}

/** The per-frame decision: which set-piece phase, if any, runs this frame. */
void func_ov039_0208bb2c(OtuBadgeState* self) {
    OtuPoint fromHome;
    OtuPoint fromPos;
    OtuPoint mid;
    OtuPoint legA;
    OtuPoint legB;

    // A badge that still has a rival attached is finishing an interaction. The
    // two-frame decrement only happens once the interaction is flagged as over,
    // so the frames are counted from the end rather than from the start.
    if (self->unk_148 > 0) {
        if (self->stateFlags & 2) {
            self->unk_148 = self->unk_148 - 2;
            if (self->unk_148 < 0) {
                self->unk_148 = 0;
            }
        }
        self->step = 0;
        return;
    }

    if (self->unk_128 != 0) {
        self->step = 0;
        return;
    }

    // The set-piece phases, in the order they are allowed to claim the frame.
    if (self->contactFlags & 0x11) {
        self->step = 0;
        func_ov039_0208b94c(self);
        return;
    }
    if (self->contactFlags & 0x820) {
        self->step = 0;
        func_ov039_0208b9b4(self);
        return;
    }
    if (self->contactFlags & 0x440) {
        self->step = 0;
        func_ov039_0208ba34(self);
        return;
    }

    if (self->step == 0) {
        // Wait for the home tile to report itself before aiming anywhere.
        if (!(self->stateFlags & 2)) {
            return;
        }

        self->unk_1CC = 0;
        self->step    = 1;

        self->startPos.x = self->home->tileX << 0xC;
        self->startPos.y = self->home->tileY << 0xC;
        self->curPos.x   = self->startPos.x;
        self->curPos.y   = self->startPos.y;

    } else if (self->step == 1) {
        s32 turn;
        s32 scale;
        s32 reach;
        s32 sign;

        if (self->unk_1CC < 0x1E) {
            self->unk_1CC = self->unk_1CC + 1;
        }

        // The aim point tracks the home tile, so until the badge has somewhere
        // else to be it is still following it.
        self->curPos.x = self->home->tileX << 0xC;
        self->curPos.y = self->home->tileY << 0xC;

        if (!(self->stateFlags & 4)) {
            return;
        }

        self->step = 0;

        turn = func_ov039_02098ca8(&self->startPos, &self->curPos);
        if (turn <= 0) {
            return;
        }

        // Join the two aim points by an offset from where the badge is now.
        func_ov039_02098b8c(&self->startPos, &self->unk_118, &fromHome);
        func_ov039_02098b8c(&self->curPos, &self->unk_118, &fromPos);

        reach = func_ov039_0208a624(&fromHome, &fromPos, &self->pos);
        if (reach == 0) {
            return;
        }

        func_ov039_02098bb0(&fromPos, &fromHome, &mid);

        turn = func_ov039_02098d10(&mid);
        if (turn > 0x50000) {
            turn = 0x50000;
        }
        scale = FX_Divide(turn, 0x50000);

        // Having aimed for a full half second the badge commits: the mode flag
        // goes up and the turn is sharpened from here on.
        if (self->unk_1CC >= 0x1E) {
            scale      = (s32)(((s64)scale * 0x1800 + 0x800) >> 12);
            self->mode = 0x10;
        }

        func_ov039_02098d3c(&mid, &mid);
        func_ov039_0208a6f8((OtuPinLogic*)self, &mid, scale);

        if (self->unk_1CC >= 0x1E) {
            return;
        }

        func_ov039_02098bb0(&fromPos, &fromHome, &legB);
        func_ov039_02098bb0(&self->pos, &fromHome, &legA);

        reach = func_ov039_0208a530(&fromHome, &fromPos, &self->pos);
        if (reach > 0x24000) {
            reach = 0x24000;
        }
        reach = FX_Divide(reach, 0x24000);

        if (*self->pinID >= 0x130) {
            return;
        }

        // Which way round the turn is decides the sign, and with it whether
        // the badge ends up winding tight or winding loose.
        sign = (func_ov039_02098c70(&legB, &legA) >= 0) ? data_ov039_0209a394
                                                        : -data_ov039_0209a394;

        self->unk_144 = (s32)(((s64)(sign * scale) * reach + 0x800) >> 12) * 3;
    }
}

/** Steers the badge along `dir` at full speed, jittering its wobble. */
void func_ov039_0208be30(OtuBadgeState* self, OtuPoint* dir) {
    OtuPoint step;

    func_ov039_02098bb0(dir, &self->pos, &step);

    // A zero vector has no direction to normalise, and normalising it would
    // hand back whatever the divide unit happens to leave in the register.
    if (step.x != 0 || step.y != 0) {
        func_ov039_02098d3c(&step, &step);
    } else {
        step.x = 0x1000;
        step.y = 0;
    }

    func_ov039_0208a6f8((OtuPinLogic*)self, &step, 0x1000);

    if (*self->pinID >= 0x130) {
        return;
    }

    // The wobble is a full-width random offset, tripled to match the three
    // the caller divides it back down by.
    self->unk_144 =
        (s32)((RNG_Next(data_ov039_0209a394 * 2) - data_ov039_0209a394) << 0xC) * 3;
}

/**
 * Walks the board outwards from the badge in a square spiral, looking for the
 * first cell whose tile type falls in [loType, hiType]. True when one is
 * found, in which case `out` receives that cell's point.
 */
s32 func_ov039_0208bed8(OtuBadgeState* self, s32 rings, s32 loType, s32 hiType, OtuPoint* out) {
    OtuPoint at;
    s32     ring;
    s32     span = 2;
    s32     i;
    s32     tile;

    at.x = self->pos.x - OTU_CELL_SIZE;
    at.y = self->pos.y - OTU_CELL_SIZE;

    for (ring = 1; ring <= rings; ring++) {
        // Four straight walks round the ring: right, down, left, up. The span
        // grows by two per ring, which is what makes this a spiral rather than
        // a sweep. The ring is entered one cell down-left of where the badge
        // is, so the search never revisits the cell it started from.
        for (i = 0; i < span; i++) {
            tile = func_ov039_0208a794(&at, (OtuCellGrid*)self->board);
            if (tile >= loType && tile <= hiType) {
                out->x = at.x;
                out->y = at.y;
                return 1;
            }
            at.x = at.x + OTU_CELL_SIZE;
        }

        for (i = 0; i < span; i++) {
            tile = func_ov039_0208a794(&at, (OtuCellGrid*)self->board);
            if (tile >= loType && tile <= hiType) {
                out->x = at.x;
                out->y = at.y;
                return 1;
            }
            at.y = at.y + OTU_CELL_SIZE;
        }

        for (i = 0; i < span; i++) {
            tile = func_ov039_0208a794(&at, (OtuCellGrid*)self->board);
            if (tile >= loType && tile <= hiType) {
                out->x = at.x;
                out->y = at.y;
                return 1;
            }
            at.x = at.x - OTU_CELL_SIZE;
        }

        for (i = 0; i < span; i++) {
            tile = func_ov039_0208a794(&at, (OtuCellGrid*)self->board);
            if (tile >= loType && tile <= hiType) {
                out->x = at.x;
                out->y = at.y;
                return 1;
            }
            at.y = at.y - OTU_CELL_SIZE;
        }

        at.x = at.x - OTU_CELL_SIZE;
        at.y = at.y - OTU_CELL_SIZE;
        span = span + 2;
    }

    return 0;
}

/** True when two points are in the same board cell. */
s32 func_ov039_0208c0e4(OtuPoint* a, OtuPoint* b) {
    if ((a->x & ~OTU_CELL_BITS) != (b->x & ~OTU_CELL_BITS)) {
        return 0;
    }
    if ((a->y & ~OTU_CELL_BITS) == (b->y & ~OTU_CELL_BITS)) {
        return 1;
    }
    return 0;
}

/** The "wander to an interesting tile" AI, entry `which` of the chance table. */
s32 func_ov039_0208c128(OtuBadgeState* self, s32 which, s32 loType, s32 hiType, s32 kind) {
    OtuPoint found;

    // Re-rolling only happens once the badge is running a different AI than
    // last frame. While it is still on the old one the previous decision is
    // simply carried forward.
    if (self->curAI != which) {
        if (self->chanceTbl[which] < RNG_Next(0x10000)) {
            return 0;
        }

        if (func_ov039_0208bed8(self, 3, loType, hiType, &found) == 0) {
            return 0;
        }

        // Landing back on the anchor is not a move, so it is rejected.
        if (func_ov039_0208c0e4(&found, &self->anchorPt) != 0) {
            return 0;
        }

        func_ov039_0208be30(self, &found);

        self->curAI    = which;
        self->unk_1B8  = kind;
        self->anchorPt = found;
        return 1;
    }

    if (func_ov039_0208c0e4(&self->pos, &self->anchorPt) != 0) {
        self->curAI = 0x11;
    } else {
        func_ov039_0208be30(self, &self->anchorPt);
    }

    return 1;
}

/** The "spin down in place" AI. */
s32 func_ov039_0208c218(OtuBadgeState* self) {
    // Once the badge is committed to something it stops re-rolling, so the
    // chance test is only worth making while it is still free.
    if (self->curAI != 0 && self->chanceTbl[0] < RNG_Next(0x10000)) {
        return 0;
    }

    func_ov039_0208ba70(self);
    return 1;
}

/** The "keep rolling forward until something opens up" AI. */
s32 func_ov039_0208c258(OtuBadgeState* self) {
    OtuPoint at;
    s32     tries = 0;
    s32     tile;

    if (self->chanceTbl[1] < RNG_Next(0x10000)) {
        return 0;
    }

    at.x = self->pos.x;
    at.y = self->pos.y;

    // Look four cells ahead for open road. Giving up after four is the point
    // of the loop: a boxed-in badge should stop rather than commit.
    do {
        func_ov039_02098b8c(&at, &self->vel, &at);
        tile = func_ov039_0208a794(&at, (OtuCellGrid*)self->board);
        if (tile == 0) {
            break;
        }
        tries++;
    } while (tries < 4);

    if (tries >= 4) {
        return 0;
    }

    func_ov039_02098bb0(&self->pos, &self->vel, &at);
    func_ov039_0208be30(self, &at);
    return 1;
}

/** The "chase the nearest rival badge" AI. */
s32 func_ov039_0208c304(OtuBadgeState* self) {
    OtuPinTask* target;
    s32         alive;
    s32         gap;

    if (self->curAI != 2) {
        if (self->chanceTbl[2] < RNG_Next(0x10000)) {
            return 0;
        }

        target = func_ov039_02087f4c(self->pool, self->scene, self->index);
        if (target == NULL) {
            return 0;
        }

        gap = func_ov039_02098ca8(&self->pos, (OtuPoint*)&target->x);
        if (gap > 0xC8000) {
            return 0;
        }

        func_ov039_0208be30(self, (OtuPoint*)&target->x);

        self->curAI       = 2;
        self->unk_1B8     = 1;
        self->chaseTarget = target;
        return 1;
    }

    // Already chasing: keep going while the target is still worth chasing.
    target = self->chaseTarget;
    alive  = func_ov039_0208efb0(target, 0x444);
    if (alive != 0) {
        func_ov039_0208be30(self, (OtuPoint*)&target->x);
    }
    return 1;
}

/** The "run away from the nearest rival badge" AI. */
s32 func_ov039_0208c3bc(OtuBadgeState* self) {
    OtuPinTask* target;
    OtuPoint    away;
    s32         gap;

    if (self->chanceTbl[3] < RNG_Next(0x10000)) {
        return 0;
    }

    target = func_ov039_02088064(self->pool, self->scene, self->index);
    if (target == NULL) {
        return 0;
    }

    gap = func_ov039_02098ca8(&self->pos, (OtuPoint*)&target->x);
    if (gap > 0x46000) {
        return 0;
    }

    // Two subtractions, which net out to `target - (self - target)`: running
    // away is the chase reflected through the target.
    func_ov039_02098bb0((OtuPoint*)&target->x, &self->pos, &away);
    func_ov039_02098bb0(&self->pos, &away, &away);

    func_ov039_0208be30(self, &away);
    return 1;
}

/** The "take the scripted arc if there is one left" AI. */
s32 func_ov039_0208c45c(OtuBadgeState* self) {
    OtuPinTask* target;
    s32         speed;
    s32         gap;

    if (self->chanceTbl[4] < RNG_Next(0x10000)) {
        return 0;
    }

    // An arc needs an arc task with frames left on it, and is not worth
    // starting for a badge already moving too fast to join it.
    if (self->timers.arcFrames <= 0) {
        return 0;
    }
    speed = func_ov039_02098d10(&self->vel);
    if (speed >= 0x3000) {
        return 0;
    }

    target = func_ov039_02088064(self->pool, self->scene, self->index);
    if (target == NULL) {
        return 0;
    }

    gap = func_ov039_02098ca8(&self->pos, (OtuPoint*)&target->x);
    if (gap > 0x46000) {
        return 0;
    }

    func_ov039_0208b9b4(self);
    return 1;
}
