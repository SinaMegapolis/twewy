/* ============================================================================
 * Band B5 -- Tsk_OtosuGame_specialgauge, 0x02094214 - 0x02094ae8.
 *
 * Claim range: .text 0x02094214 .. 0x02094ae8 (11 functions, 2260 bytes,
 * 0x02094214 .. 0x02094ab4). It sits in the gap between band 2
 * (0x02093000-0x02094000) and band 3 (0x02095000-0x02096000) and is one
 * whole task rather than a slice of a call graph, the same shape as band 6
 * and band 7. It is self-contained: nothing outside the band calls into it
 * except the overlay's own dispatcher, so it can sit anywhere in the include
 * order that is convenient.
 *
 * WHAT THE TASK IS
 * ----------------
 * The overlay's data at 0x02099b98 is a `TaskHandle` naming the string at
 * 0x0209a7b0 -- "Tsk_OtosuGame_specialgauge" -- with a taskFunc of
 * 0x02094a6c (this band's dispatcher) and a dataSize of 0x4B4, and the
 * `TaskStages` immediately after it lists this band's four stages in the
 * usual order:
 *
 *   0x02099ba4  020944bc  init     020945c8  update
 *               020948f4  render   020949f4  cleanup
 *
 * It is a four-digit counter: a row of four small cells, each a *pair* of
 * sprites showing a two-digit number out of a gauge. 0x02094418 is the loader
 * for both halves of a pair and takes a third argument selecting which of the
 * two, which is the clearest single piece of evidence for the layout.
 *
 * The four `SpriteAnimation` templates that follow at 0x02099bb4, 0x02099be0,
 * 0x02099c0c and 0x02099c38 all name 0x02094214 -- this band's own first
 * function -- as their `frameInfoCallback`, which is what identifies 0x02094214
 * as a cell builder rather than a stage. 0x02099be0, 0x02099c0c and
 * 0x02099c38 are byte-identical; only 0x02099bb4 differs, and only in its
 * `unk_02` halfword and its initial posX/posY. That is why three of the four
 * loaders have the same body and differ only in which pool word they name.
 *
 * STRUCTURAL FINDINGS
 * -------------------
 *   * The 0x400-byte body is *not* an array of 0x40-stride cells. It is
 *     0x0C of header, then four sprites at 0x40 stride, then four more at
 *     0x40 stride, then eight at 0x40 stride -- but the eight are addressed
 *     as two interleaved 0x80-stride walks (the "low" and "high" sprites of
 *     each pair), which is why the update and the render both keep two
 *     pointers advancing by 0x80 beside two advancing by 0x40. The last
 *     two sprites, at 0x40C and 0x44C, are the pair that gets the affine
 *     transform.
 *   * The four palette-source writes in the update land on `Sprite.unk3C`
 *     (+0x3C) of all four sprites of the cell. The two index expressions
 *     are `self + i*0x40 + 0x48` and `self + i*0x40 + 0x148` for the two
 *     0x40-walk groups and `self + i*0x80 + 0x248` / `+0x288` for the
 *     0x80-walk pair; 0x48 - 0x0C = 0x3C and 0x148 - 0x10C = 0x3C pin the
 *     sprite base at 0x0C and 0x10C, and 0x248 - 0x20C = 0x3C and
 *     0x288 - 0x24C = 0x3C pin the pair's two halves at 0x20C and 0x24C.
 *   * The pointer the update reads at `self + i*0x40 + 0x134` is
 *     `spriteB[i].resourceData` (Sprite +0x28, and 0x10C + 0x28 = 0x134), and
 *     the two-step walk it performs on it -- `buffer + 0x20` then add the
 *     word at `buffer + 0x48` or `buffer + 0x40` -- is the overlay's
 *     pack-entry shape, i.e. `PackEntry[5]` and `PackEntry[4]`. Band 2 spells
 *     the same walk for one of them as `OtuPaletteSource`; both are
 *     reproduced here as one parameterised helper.
 *   * The digit position tables are two differently-strided objects at
 *     0x0209a780 (4 bytes per entry, one position) and 0x0209a790 (8 bytes
 *     per entry, two positions). 0x0209a790's first pair is byte-identical
 *     to 0x0209a780's entry, and its second pair is the first minus five on
 *     x -- i.e. the two-digit table is the one-digit table plus the tens
 *     digit five pixels to its left, which is what the count < 10 /
 *     count >= 10 split in the update is actually choosing between.
 * ==========================================================================*/

#include "Debug/Sugata/TinPinSlammer.h"
#include "Engine/Core/OamMgr.h"
#include "Engine/EasyTask.h"
#include "SpriteMgr.h"

/* ------------------------------------------------------------------ */
/* Data the band reads but does not define.                            */
/* ------------------------------------------------------------------ */

/*
 * The overlay's `.rodata` is gap-filled from 0x02098f3c to 0x0209a12c and its
 * `.data` from 0x0209a49c to 0x0209ad00, and every symbol this band names in
 * those windows is in the gap. Declaring an object of our own would put it at
 * an address of the linker choosing rather than the one the target uses, so
 * they keep the build's own names. See the same note in band 5.
 *
 * `data_ov039_0209a0fc` is the `BinIdentifier` all four templates name -- the
 * only thing tying this task's eighteen sprites to one piece of art. It is
 * the same *kind* of record as band 5's `data_ov039_0209a0dc` (one overlay,
 * one bin id) but a different object, so it is declared rather than shared.
 */
extern const TaskHandle data_ov039_02099b98;
extern const TaskStages data_ov039_02099ba4;

extern SpriteAnimation data_ov039_02099bb4;
extern SpriteAnimation data_ov039_02099be0;
extern SpriteAnimation data_ov039_02099c0c;
extern SpriteAnimation data_ov039_02099c38;

extern const BinIdentifier data_ov039_0209a0fc;

/** @brief One cell's single-digit position: two halfwords. */
typedef struct {
    /* 0x00 */ u16 x;
    /* 0x02 */ u16 y;
} OtuGaugePos; // Size: 0x4

/**
 * @brief One cell's two-digit position: four signed halfwords.
 *
 * The last pair is the first minus five on x, so `lowX`/`highX` read left to
 * right as tens-then-units. The halfword stride is what the code uses: 0x02094418
 * addresses entry `index` and then offsets by `which * 4` to pick a half of it.
 */
typedef struct {
    /* 0x00 */ u16 lowX;  // units digit
    /* 0x02 */ u16 lowY;
    /* 0x04 */ u16 highX; // tens digit, five pixels left of lowX
    /* 0x06 */ u16 highY;
} OtuGaugeDigits; // Size: 0x8

/*
 * The two digit-position tables. Both are declared non-`const` because they
 * live in the overlay's `.data`, not its `.rodata` -- band 5 records that
 * telling mwcc a `.data` symbol is const costs it a reload across a call, and
 * neither table is folded here (both are indexed by the loop counter) so the
 * distinction is free either way, but the section is the honest answer.
 *
 * Values, recovered with `tools/ov039_bytes.py`: 0x0209a780 holds
 * (0x37,0x9C) (0x27,0x83) (0x14,0x90) (0x1E,0xA7), and 0x0209a790's first
 * pair repeats those exactly while its second is five to the left. So the
 * two-digit table is the one-digit table plus the tens digit, which is exactly
 * the choice the update's `count < 10` / `count >= 10` split makes.
 */
extern OtuGaugePos    data_ov039_0209a780[4];
extern OtuGaugeDigits data_ov039_0209a790[4];

/**
 * @brief The overlay's fourth "is this pin the kind I care about" filter.
 *
 * The shared header declares the other three (0x0208e984 kind 8, 0x0208e998
 * kind 7, 0x0208e9d0 kind 6) but not this one. The target's body is
 * `ldr r1,[r0,#0xf8]; cmp r1,#9; moveq r0,#1` -- kind 9 -- so it belongs to
 * that family and sits between them at 0x0208e9e4, inside OtuFieldAccess.c's
 * own claim and not yet defined there.
 */
s32 func_ov039_0208e9e4(OtuPinTask* task);

/* ------------------------------------------------------------------ */
/* Task-data shapes.                                                   */
/* ------------------------------------------------------------------ */

/**
 * @brief The two words `func_ov039_02094ab4` packs and the init stage reads.
 *
 * The first becomes the task's `dataType` and is folded into the `dataType`
 * bitfield of all eighteen `SpriteAnimation` templates; the second is the pin
 * task's handle, which the update resolves through `EasyTask_GetTaskData` and
 * then queries four ways. This is a struct rather than two scalar locals
 * because only its *address* is ever taken, and with scalars mwcc drops the
 * second store as dead and the frame shrinks by four bytes (band 5 records the
 * same for its own block).
 */
typedef struct {
    /* 0x00 */ s32 dataType; // SpriteAnimation.dataType, four bits
    /* 0x04 */ s32 pinId;    // handle of the pin task this gauge watches
} OtuGaugeArgs; // Size: 0x8

/**
 * @brief One cell of the gauge: its two digit sprites, 0x80 apart.
 *
 * The 0x80 stride is load-bearing and is the reason this is a struct rather
 * than a flat `Sprite[8]`: the update and the render each walk the "low" and
 * the "high" sprite with a *separate* pointer advancing 0x80 per cell, side by
 * side with the two 0x40-stride walks, and mwcc's address arithmetic follows
 * the source's. Indexing a flat array by `i * 2` would fold the multiply and
 * change the instruction the loop is built from.
 *
 * "low" is the units digit: it is repositioned and re-animated on every change
 * of the count, and is the one shown alone when the count is below ten.
 * "high" is the tens digit: it is only repositioned, re-animated, updated and
 * drawn once the count reaches ten.
 */
typedef struct {
    /* 0x00 */ Sprite spriteLow;  // units digit
    /* 0x40 */ Sprite spriteHigh; // tens digit
} OtuGaugePair; // Size: 0x80

/**
 * @brief "Tsk_OtosuGame_specialgauge", 0x4B4 bytes.
 *
 * The size is the target's, not an estimate: the `TaskHandle` at 0x02099b98
 * carries it as its third word.
 *
 * The layout is derived from the strides the code uses, and every offset in it
 * is fixed by at least one load or store in this band. From 0x0C to 0x40B the
 * block is eighteen `Sprite`s with nothing between them, which the arithmetic
 * above pins from three independent directions. The 0x400 tail is
 * `OtuEntryAnim`-shaped: a rotation and an x/y scale pair handed to
 * `OamMgr_AllocAffineGroup`, two halfwords nothing in this band reads, and two
 * four-entry arrays the update compares against.
 *
 * The two wide sprites are named for what the code does to them rather than for
 * what they depict: `plate` (0x40C) is only ever updated and rendered, and
 * `dial` (0x44C) is the one whose OAM attribute word has an affine slot index
 * inserted into it every frame.
 */
typedef struct {
    /* 0x000 */ s32          dataType;      // folded into every template's dataType field
    /* 0x004 */ s32          pinId;         // the tracked pin's task handle
    /* 0x008 */ s32          running;       // raised by Update, tested by Render
    /* 0x00C */ Sprite       spriteA[4];    // four cells' first 0x40-walk sprite
    /* 0x10C */ Sprite       spriteB[4];    // four cells' second 0x40-walk sprite
    /* 0x20C */ OtuGaugePair digit[4];      // the four two-digit displays
    /* 0x40C */ Sprite       plate;         // the wide backing sprite
    /* 0x44C */ Sprite       dial;          // the wide sprite drawn through the affine path
    /* 0x48C */ s32          rotation;      // +0x100 per frame, kept to 16 bits
    /* 0x490 */ s32          scaleX;        // 0x1000, handed to the affine allocator
    /* 0x494 */ s32          scaleY;        // 0x1000 likewise
    /* 0x498 */ s16          unk_498;       // cleared by Init, read by nothing in this band
    /* 0x49A */ s16          unk_49A;       // likewise
    /* 0x49C */ s16          lastCount[4];  // the count each cell was last drawn for
    /* 0x4A4 */ s32          alive[4];      // the filter's answer for this cell's pin
} OtuGauge; // Size: 0x4B4

/**
 * @brief The palette block inside a loaded sprite resource's buffer.
 *
 * `buffer + 0x20` is the overlay's `PackHeader`; the word at the caller's
 * offset into the table that follows is a `PackEntry`'s `offset` field, and
 * adding it to the table's base is the sub-resource. This is the same walk as
 * band 2's `OtuPaletteSource` and as `Data_GetPackEntryData`, written out with
 * the entry index as a parameter because this band needs two of them: the
 * update takes the fifth entry when a cell has gone dead and the fourth when it
 * has come back, and the arithmetic differs only in the constant.
 *
 * Declared here rather than reusing band 2's helper so this band does not
 * depend on the include order, and named differently so the two can coexist.
 */
static inline void* OtuGaugePaletteSource(Data* file, s32 packEntry) {
    PackEntry* entries = (PackEntry*)((u8*)file->buffer + sizeof(PackHeader));

    return (u8*)entries + entries[packEntry].offset;
}

/* ------------------------------------------------------------------ */
/* The cell builder, 0x02094214.                                       */
/* ------------------------------------------------------------------ */

/**
 * @brief The `frameInfoCallback` all four of this task's templates name.
 *
 * The same body as the twenty-two in OtuFieldAccess.c and the three in band 2,
 * and with the same two known differences: it does not set the slot's +0x0C,
 * and it stores a constant 3 into the depth key instead of calling
 * `func_ov039_02088400`. The three flat short-circuit tests and the two-step
 * lookup are shared verbatim, down to the `ldrne` that is the short-circuit
 * over +0x18 and the fact that both the index and the table are re-loaded
 * between the two reads.
 *
 * +0x18, +0x1C and +0x16 are `Sprite.animData`, `Sprite.cellTable` and
 * `Sprite.cellIndex`, and the eight-byte stride is `SpriteCell` -- so `unk_04`
 * lands on the entry's `pieceCount` and `unk_08` on its `pieceOffset` scaled to
 * bytes. `OtuSpriteTask` is a `u8 pad_16[0x100]` blob, so the three reads are
 * spelled as casts rather than as fields; that is the form OtuFieldAccess.c
 * settled on and it is what the target's raw offsets come from.
 */
/* `arg` is unused -- the target never loads r1 in either body. Kept as a named
 * parameter so the engine's selector stays in the third argument slot. */
OtuSpriteSlot* func_ov039_02094214(OtuSpriteTask* t, s32 arg, s32 mode) {
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

            // The two-step lookup: a u16 out of the cell table at one stride,
            // then a byte pointer built from the u16 at the other.
            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = *(u16*)(table + index * 8 + 2);
                slot->unk_08 = (s32)(u8*)(table + *(u16*)(table + index * 8) * 2);
            }

            // The one place this differs from the twenty-two: a constant, not
            // `func_ov039_02088400`. The `-1` above is still stored first, so
            // the field is written twice on this path.
            slot->depthKey = 3;
            return slot;
        }

        default:
            return NULL;
    }
}

/* ------------------------------------------------------------------ */
/* The four sprite loaders.                                            */
/* ------------------------------------------------------------------ */

/*
 * All four copy a 0x2C-byte `SpriteAnimation` onto the stack, stamp it with
 * this object's `dataType` and owner, and load one sprite. The copy is the
 * `ldm/stm` triple out of a literal pool, which is how mwcc renders a copy
 * from an `extern const` object -- band 3's loaders use that same form
 * (`SpriteAnimation anim = data_ov039_02099cc8;`) and it is preferred here over
 * a local brace initialiser, which band 5 only needs because its templates
 * have no symbol of their own in the target.
 *
 * The `dataType` write is a read-modify-write of the template's first halfword:
 * `bic #0x3C` clears the field's four bits and the `<< 0x1C` / `>> 0x1A` pair
 * is the insert. The `lsl #0x10` / `lsr #0x10` round trip in front of it is
 * the `(u16)` cast, and it is load-bearing: without it the value reaches the
 * field untruncated and mwcc emits a different sequence.
 */

/** Loads the affine-path sprite (0x44C) from the template at 0x02099bb4. */
void func_ov039_020942bc(OtuGauge* self, Sprite* sprite) {
    SpriteAnimation anim = data_ov039_02099bb4;

    anim.owner    = self;
    anim.dataType = (u16)self->dataType;

    _Sprite_Load(sprite, &anim);
}

/** Loads the wide backing sprite (0x40C) from the template at 0x02099be0. */
void func_ov039_0209432c(OtuGauge* self, Sprite* sprite) {
    SpriteAnimation anim = data_ov039_02099be0;

    anim.owner    = self;
    anim.dataType = (u16)self->dataType;

    _Sprite_Load(sprite, &anim);
}

/**
 * @brief Loads one of the eight 0x40-walk sprites, 0x0209439c.
 *
 * The template at 0x02099c0c is byte-identical to the one 0x0209432c uses, so
 * this body is 0x0209432c's plus one argument -- the cell index, whose `+ 1`
 * becomes the template's `packIndex`, selecting one of the eight walk packs.
 * It is deliberately not the animation: `animIndex` at +0x2A keeps the
 * template's own value. Zero is therefore not a valid pack index, the same
 * convention band 3's point task uses on its own digit table.
 */
void func_ov039_0209439c(OtuGauge* self, Sprite* sprite, s32 index) {
    SpriteAnimation anim = data_ov039_02099c0c;

    anim.owner     = self;
    anim.dataType  = (u16)self->dataType;
    anim.packIndex = index + 1;

    _Sprite_Load(sprite, &anim);
}

/**
 * @brief Loads one half of a cell's two-digit display, 0x02094418.
 *
 * The only loader that also positions its sprite. `which` picks the low or the
 * high digit of cell `index`, and the target reaches the table as
 * `base + index * 8` and `base + which * 4` as two separate index
 * expressions, so the read is a halfword subscript with a doubled index rather
 * than a field access -- a field access would need a branch to choose between
 * `lowX/lowY` and `highX/highY`.
 *
 * The `+ 1` on the index is the same one the 0x40-walk loader applies, so all
 * four of a cell's sprites come from the same template with the same animation.
 */
void func_ov039_02094418(OtuGauge* self, Sprite* sprite, s32 index, s32 which) {
    SpriteAnimation anim = data_ov039_02099c38;

    anim.owner     = self;
    anim.dataType  = (u16)self->dataType;
    anim.posX      = ((u16*)&data_ov039_0209a790[index])[which * 2];
    anim.posY      = ((u16*)&data_ov039_0209a790[index])[which * 2 + 1];
    anim.animIndex = index + 1;

    _Sprite_Load(sprite, &anim);
}

/* ------------------------------------------------------------------ */
/* The init stage, 0x020944bc.                                         */
/* ------------------------------------------------------------------ */

/**
 * @brief The gauge's init stage: records the two arguments, loads all
 *        eighteen sprites, and seeds the affine block.
 *
 * `dataType` is written before the two argument words are read back out, which
 * is what lets the loaders take `self` rather than the argument block: the
 * target's first store is the first thing read by the first loader.
 *
 * Each cell gets both 0x40-walk sprites at `index + 1` and both of its digit
 * sprites at the same index, and then the *first* 0x40-walk sprite is switched
 * to animation 0xC -- the only `Sprite_ChangeAnimation` in the stage, and the
 * reason the target hoists 0xC into a callee-saved register before the loop
 * rather than re-materialising it on each of the four passes.
 *
 * The two halfwords at +0x498 and +0x49A are cleared with `strh`, so they are
 * genuinely `s16`; nothing in this band reads them, which is why they stay
 * `unk`.
 */
s32 func_ov039_020944bc(TaskPool* pool, Task* task, void* arg) {
    OtuGauge*     self = (OtuGauge*)task->data;
    OtuGaugeArgs* args = (OtuGaugeArgs*)arg;
    s32           i;

    self->dataType = args->dataType;
    self->pinId    = args->pinId;

    func_ov039_020942bc(self, &self->dial);
    func_ov039_0209432c(self, &self->plate);

    for (i = 0; i < 4; i++) {
        self->lastCount[i] = 0;
        self->alive[i]     = 0;

        func_ov039_0209439c(self, &self->spriteA[i], i);
        Sprite_ChangeAnimation(&self->spriteA[i], self->spriteA[i].animData, 0xC, self->spriteA[i].cellTable);

        func_ov039_0209439c(self, &self->spriteB[i], i);
        func_ov039_02094418(self, &self->digit[i].spriteLow, i, 0);
        func_ov039_02094418(self, &self->digit[i].spriteHigh, i, 1);
    }

    self->rotation = 0;
    self->scaleX   = 0x1000;
    self->scaleY   = 0x1000;
    self->unk_498  = 0;
    self->unk_49A  = 0;

    return 1;
}

/* ------------------------------------------------------------------ */
/* The update stage, 0x020945c8 -- the largest thing in the band.     */
/* ------------------------------------------------------------------ */

/**
 * @brief The gauge's update stage: spin the dial, then walk the four cells.
 *
 * The prologue is two statements -- raise the running flag, step the dial's
 * rotation by 0x100 with a 16-bit truncation -- followed by the two wide
 * sprites' `Sprite_Update`s and a resolution of the pin task. The target
 * interleaves the rotation arithmetic with the two call setups, which is the
 * scheduler and not a source order.
 *
 * What the four-cell walk does, per cell:
 *
 *   1. Ask the pin two questions through a `switch` on the cell index: which
 *      of four "is this pin the kind I want" filters, and which of four
 *      counters off the pin's own +0x178..+0x17E run. The switch is a real one
 *      -- the target emits the `cmp r9, #3` / `addls pc, pc, r9, lsl #2` jump
 *      table -- and the `alive` result is the one the target pre-sets to zero
 *      before the dispatch, while `count` is not initialised at all, which is
 *      only safe because every value the loop can reach has a case.
 *   2. If both answers are zero the cell is dead: point all four of its
 *      sprites' palette sources at pack entry 5. A cell that has come back to
 *      life is repointed at pack entry 4 instead, but only on the frame its
 *      count actually changes.
 *   3. On a change of count, re-animate the units digit to `count % 10 + 2`
 *      and move it to its place. Below ten it takes the one-digit position out
 *      of 0x0209a780; from ten up it takes the two-digit pair out of
 *      0x0209a790 and additionally re-animates the tens digit to
 *      `(count / 10) % 10 + 2` and moves it. So the two tables are the same
 *      positions with and without the tens digit, and the split is what
 *      decides whether a second sprite appears.
 *   4. Advance the sprites: the first 0x40-walk sprite only if its filter
 *      answered true, the second one always, the units digit always, and the
 *      tens digit only once the stored count has reached ten.
 *
 * The two `% 10` narrowings are deliberate and both are in the target: the
 * `lsl #0x10` / `asr #0x10` in front of each is the assignment into the `s16`
 * local, and the second round of the pair is the `s16` parameter of
 * `Sprite_ChangeAnimation`. Writing `count % 10 + 2` inline would collapse
 * them into one.
 *
 * `count / 10` is computed once and reused by the second `% 10`, and the target
 * keeps it in a register across the `Sprite_ChangeAnimation` that sits between
 * the two divides -- which is why the tens digit's frame is spelled through a
 * local rather than as `(count / 10) % 10 + 2` inline.
 */
s32 func_ov039_020945c8(TaskPool* pool, Task* task, void* args) {
    OtuGauge*  self = (OtuGauge*)task->data;
    OtuPinTask* pin;
    s32        i;
    s32        count;
    s32        alive;

    self->running  = 1;
    self->rotation = (u16)(self->rotation + 0x100);

    Sprite_Update(&self->dial);
    Sprite_Update(&self->plate);

    pin = (OtuPinTask*)EasyTask_GetTaskData(pool, self->pinId);

    for (i = 0; i < 4; i++) {
        s16                frame;
        s32                tens;
        UnkSmallInternal* pal;
        Data*              file;

        alive = 0;

        switch (i) {
            case 0:
                alive = func_ov039_0208e9d0(pin); // pin kind 6
                count = func_ov039_0208eea0(pin);
                break;

            case 1:
                alive = func_ov039_0208e984(pin); // pin kind 8
                count = func_ov039_0208eeac(pin);
                break;

            case 2:
                alive = func_ov039_0208e998(pin); // pin kind 7
                count = func_ov039_0208eeb8(pin);
                break;

            case 3:
                alive = func_ov039_0208e9e4(pin); // pin kind 9
                count = func_ov039_0208eec4(pin);
                break;

            default:
                break;
        }

        // Nothing left in the cell: repoint all four sprites at the "empty"
        // palette block, which is pack entry five.
        if (count == 0 && alive == 0) {
            file = self->spriteB[i].resourceData;

            if (file != NULL) {
                pal = (UnkSmallInternal*)OtuGaugePaletteSource(file, 5);
            } else {
                pal = NULL;
            }

            self->spriteA[i].unk3C        = pal;
            self->spriteB[i].unk3C        = pal;
            self->digit[i].spriteLow.unk3C  = pal;
            self->digit[i].spriteHigh.unk3C = pal;
        }

        self->alive[i] = alive;

        if (count != (s32)self->lastCount[i]) {
            // A live cell goes back to the normal palette block, entry four.
            // This is deliberately inside the change test and not beside the
            // dead-cell test above: the target's `cmp r6, #0; ble` skips it
            // entirely for a count of zero or less, so a cell that has just
            // emptied keeps the "empty" palette it was just given.
            if (count > 0) {
                file = self->spriteB[i].resourceData;

                if (file != NULL) {
                    pal = (UnkSmallInternal*)OtuGaugePaletteSource(file, 4);
                } else {
                    pal = NULL;
                }

                self->spriteA[i].unk3C        = pal;
                self->spriteB[i].unk3C        = pal;
                self->digit[i].spriteLow.unk3C  = pal;
                self->digit[i].spriteHigh.unk3C = pal;
            }

            frame = (s16)(count % 10);
            Sprite_ChangeAnimation(&self->digit[i].spriteLow, self->digit[i].spriteLow.animData, frame + 2,
                                  self->digit[i].spriteLow.cellTable);

            if (count < 0xA) {
                // One digit: the units digit takes the standalone position.
                self->digit[i].spriteLow.posX = data_ov039_0209a780[i].x;
                self->digit[i].spriteLow.posY = data_ov039_0209a780[i].y;
            } else {
                // Two digits: both take their places and the tens digit
                // appears.
                self->digit[i].spriteLow.posX  = data_ov039_0209a790[i].lowX;
                self->digit[i].spriteLow.posY  = data_ov039_0209a790[i].lowY;

                tens  = count / 10;
                frame = (s16)(tens % 10);
                Sprite_ChangeAnimation(&self->digit[i].spriteHigh, self->digit[i].spriteHigh.animData, frame + 2,
                                      self->digit[i].spriteHigh.cellTable);

                self->digit[i].spriteHigh.posX = data_ov039_0209a790[i].highX;
                self->digit[i].spriteHigh.posY = data_ov039_0209a790[i].highY;
            }

            self->lastCount[i] = (s16)count;
        }

        if (self->alive[i] != 0) {
            Sprite_Update(&self->spriteA[i]);
        }
        Sprite_Update(&self->spriteB[i]);
        Sprite_Update(&self->digit[i].spriteLow);

        if (self->lastCount[i] >= 0xA) {
            Sprite_Update(&self->digit[i].spriteHigh);
        }
    }

    return 1;
}

/* ------------------------------------------------------------------ */
/* The render stage, 0x020948f4.                                       */
/* ------------------------------------------------------------------ */

/**
 * @brief The gauge's render stage: one affine sprite, then all eighteen.
 *
 * Only the dial is transformed. Its OAM attribute word's affine-slot field --
 * `Sprite.unk_0A.raw`'s bits 5..9 -- is replaced with the slot the allocator
 * returns, and the `(u32)(u16)` around that return is load-bearing twice: it is
 * the `<< 0x10` / `>> 0x10` pair the target emits, and it makes the following
 * `>> 0x16` a logical rather than an arithmetic shift. Band 3's entry-task
 * render is the same expression over the same manager array and carries the
 * same note.
 *
 * The manager is picked from the sprite's own display-engine bits, exactly as
 * band 3 does it, and the scale pair comes from the task rather than from a
 * per-sprite limit block.
 *
 * Nothing in the stage is positioned here; every sprite draws where the update
 * left it, which is the opposite of band 7's hammer and of band 3's point task
 * and is why this task needs no render-side anchor arithmetic at all.
 */
s32 func_ov039_020948f4(TaskPool* pool, Task* task, void* args) {
    OtuGauge* self = (OtuGauge*)task->data;
    s32       i;

    if (self->running != 0) {
        self->dial.unk_0A.raw = (self->dial.unk_0A.raw & ~0x3E0) |
                                ((u32)(u16)OamMgr_AllocAffineGroup(&g_OamMgr[self->dial.bits_0_1], self->rotation,
                                                                    self->scaleX, self->scaleY, 0)
                                     << 0x1B >>
                                 0x16);
        Sprite_RenderFrame(&self->dial);
        Sprite_RenderFrame(&self->plate);

        for (i = 0; i < 4; i++) {
            Sprite_RenderFrame(&self->spriteB[i]);
            Sprite_RenderFrame(&self->digit[i].spriteLow);

            if (self->lastCount[i] >= 0xA) {
                Sprite_RenderFrame(&self->digit[i].spriteHigh);
            }

            if (self->alive[i] != 0) {
                Sprite_RenderFrame(&self->spriteA[i]);
            }
        }
    }

    return 1;
}

/* ------------------------------------------------------------------ */
/* The cleanup stage, 0x020949f4.                                      */
/* ------------------------------------------------------------------ */

/**
 * @brief Releases all eighteen sprites, and keeps running.
 *
 * The dial goes first and the plate second, then the four cells in the same
 * order the update walks them. `Sprite_Release` does not care about order, so
 * this is simply the reverse of the two 0x40/0x80 pointer setups the stage
 * shares with the update -- which is why the target builds its four walking
 * pointers identically in the two functions.
 */
s32 func_ov039_020949f4(TaskPool* pool, Task* task, void* args) {
    OtuGauge* self = (OtuGauge*)task->data;
    s32       i;

    Sprite_Release(&self->dial);
    Sprite_Release(&self->plate);

    for (i = 0; i < 4; i++) {
        Sprite_Release(&self->spriteA[i]);
        Sprite_Release(&self->spriteB[i]);
        Sprite_Release(&self->digit[i].spriteLow);
        Sprite_Release(&self->digit[i].spriteHigh);
    }

    return 1;
}

/* ------------------------------------------------------------------ */
/* The dispatcher and the spawner.                                     */
/* ------------------------------------------------------------------ */

/**
 * @brief The stage dispatcher, and the `TaskHandle`'s own `taskFunc`.
 *
 * Identical in shape to band 3's and band 7's: the four callbacks are copied
 * out of the overlay's own `TaskStages` onto the stack and then indexed by
 * `stage`, so the frame is the 0x10 the copy needs and the indirect call is
 * through the copy. The pool word is the target's `data_ov039_02099ba4`, which
 * is why that object is declared by address rather than being written out as a
 * local initialiser.
 *
 * The result of the stage is not touched -- the target has no `mov r0` after
 * the `blx` -- so the return type is `s32` only because that is what the four
 * stages return and what the engine's taskFunc signature says.
 */
s32 func_ov039_02094a6c(TaskPool* pool, Task* task, void* args, s32 stage) {
    const TaskStages stages = data_ov039_02099ba4;

    return stages.iter[stage](pool, task, args);
}

/**
 * @brief Creates the gauge task. 0x02094ab4.
 *
 * The two-word block is built in the task's own outgoing-argument area rather
 * than in a frame of its own, which is why the frame is exactly the 0x10 that
 * the two outgoing words and the block need together: `[sp]` and `[sp+4]` are
 * the fifth and sixth arguments, `[sp+8]` and `[sp+0xC]` the block.
 *
 * The handle is read by address -- it is in the gap-filled `.rodata` -- and the
 * target's last two instructions are the call and the epilogue with no `mov r0`
 * between, so the handle comes back in r0 for free.
 */
s32 func_ov039_02094ab4(TaskPool* pool, s32 arg1, s32 arg2) {
    OtuGaugeArgs args;

    args.dataType = arg1;
    args.pinId    = arg2;

    return EasyTask_CreateTask(pool, &data_ov039_02099b98, NULL, 0, NULL, &args);
}
