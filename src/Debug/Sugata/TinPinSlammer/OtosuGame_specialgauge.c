/**
 * @file OtosuGame_specialgauge.c
 * @brief The `Tsk_OtosuGame_specialgauge` task.
 */

#include "OtuFieldAccessShared.h"

/** @brief One cell's single-digit position: two halfwords. */
typedef struct {
    /* 0x00 */ u16 x;
    /* 0x02 */ u16 y;
} OtuGaugePos; // Size: 0x4

/*
 * The two digit-position tables. Both are in `.data`, so they are not
 * declared `const`.
 *
 * Values, recovered with `tools/ov039_bytes.py`: 0x0209a780 holds
 * (0x37,0x9C) (0x27,0x83) (0x14,0x90) (0x1E,0xA7), and 0x0209a790's first
 * pair repeats those exactly while its second is five to the left. So the
 * two-digit table is the one-digit table plus the tens digit, which is exactly
 * the choice the update's `count < 10` / `count >= 10` split makes.
 */

/**
 * @brief The two words `OtosuGame_specialgauge_CreateTask` packs and the init stage reads.
 *
 * The first becomes the task's `dataType` and is folded into the `dataType`
 * bitfield of all eighteen `SpriteAnimation` templates; the second is the pin
 * task's handle, which the update resolves through `EasyTask_GetTaskData` and
 * then queries four ways. This is a struct rather than two scalar locals
 * because only its *address* is ever taken, and with scalars mwcc drops the
 * second store as dead and the frame shrinks by four bytes.
 */
typedef struct {
    /* 0x00 */ s32 dataType;   // SpriteAnimation.dataType, four bits
    /* 0x04 */ s32 pinId;      // handle of the pin task this gauge watches
} OtosuGame_specialgauge_Args; // Size: 0x8

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
} OtuGaugePair;                   // Size: 0x80

/**
 * @brief "Tsk_OtosuGame_specialgauge", 0x4B4 bytes.
 *
 * The size is the target's, not an estimate: the `TaskHandle` at 0x02099b98
 * carries it as its third word.
 *
 * The layout is derived from the strides the code uses, and every offset in it
 * is fixed by at least one load or store in this file. From 0x0C to 0x40B the
 * block is eighteen `Sprite`s with nothing between them, which the arithmetic
 * above pins from three independent directions. The 0x400 tail is
 * `OtuEntryAnim`-shaped: a rotation and an x/y scale pair handed to
 * `OamMgr_AllocAffineGroup`, two halfwords nothing reads, and two
 * four-entry arrays the update compares against.
 *
 * The two wide sprites are named for what the code does to them rather than for
 * what they depict: `plate` (0x40C) is only ever updated and rendered, and
 * `dial` (0x44C) is the one whose OAM attribute word has an affine slot index
 * inserted into it every frame.
 */
typedef struct {
    /* 0x000 */ s32            dataType;     // folded into every template's dataType field
    /* 0x004 */ s32            pinId;        // the tracked pin's task handle
    /* 0x008 */ s32            running;      // raised by Update, tested by Render
    /* 0x00C */ Sprite         spriteA[4];   // four cells' first 0x40-walk sprite
    /* 0x10C */ Sprite         spriteB[4];   // four cells' second 0x40-walk sprite
    /* 0x20C */ OtuGaugePair   digit[4];     // the four two-digit displays
    /* 0x40C */ Sprite         plate;        // the wide backing sprite
    /* 0x44C */ Sprite         dial;         // the wide sprite drawn through the affine path
    /* 0x48C */ OamAffineParam affine;       // the dial spins 0x100 per frame
    /* 0x49C */ s16            lastCount[4]; // the count each cell was last drawn for
    /* 0x4A4 */ s32            alive[4];     // the filter's answer for this cell's pin
} OtosuGame_specialgauge;                    // Size: 0x4B4

s32              OtosuGame_specialgauge_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);
SpriteFrameInfo* OtosuGame_specialgauge_GetFrameInfo(Sprite* sprite, s32 arg, s32 mode);

static const TaskHandle Tsk_OtosuGame_specialgauge = {"Tsk_OtosuGame_specialgauge", OtosuGame_specialgauge_RunTask,
                                                      sizeof(OtosuGame_specialgauge)};

static const SpriteAnimation OtosuGame_specialgauge_AnimDial = {
    .bits_0_1          = 1,
    .dataType          = 0,
    .bit_6             = 0,
    .bits_7_9          = 5,
    .bits_10_11        = 0,
    .bits_12_13        = 1,
    .bits_14_15        = 0,
    .unk_02.raw        = 7,
    .posX              = 0x20,
    .posY              = 0x98,
    .frameInfoCallback = OtosuGame_specialgauge_GetFrameInfo,
    .callbackArg       = 0,
    .owner             = NULL,
    .binIden           = &data_ov039_0209a0fc,
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

static const SpriteAnimation OtosuGame_specialgauge_AnimPlate = {
    .bits_0_1          = 1,
    .dataType          = 0,
    .bit_6             = 0,
    .bits_7_9          = 5,
    .bits_10_11        = 0,
    .bits_12_13        = 1,
    .bits_14_15        = 0,
    .unk_02.raw        = 0,
    .posX              = 0x80,
    .posY              = 0x60,
    .frameInfoCallback = OtosuGame_specialgauge_GetFrameInfo,
    .callbackArg       = 0,
    .owner             = NULL,
    .binIden           = &data_ov039_0209a0fc,
    .unk_18            = 2,
    .packIndex         = 5,
    .unk_1C            = 1,
    .unk_1E            = 0,
    .unk_20            = 4,
    .unk_22            = 1,
    .unk_24            = 0,
    .unk_26            = 2,
    .unk_28            = 3,
    .animIndex         = 1,
};

static const SpriteAnimation OtosuGame_specialgauge_AnimCell = {
    .bits_0_1          = 1,
    .dataType          = 0,
    .bit_6             = 0,
    .bits_7_9          = 5,
    .bits_10_11        = 0,
    .bits_12_13        = 1,
    .bits_14_15        = 0,
    .unk_02.raw        = 0,
    .posX              = 0x80,
    .posY              = 0x60,
    .frameInfoCallback = OtosuGame_specialgauge_GetFrameInfo,
    .callbackArg       = 0,
    .owner             = NULL,
    .binIden           = &data_ov039_0209a0fc,
    .unk_18            = 2,
    .packIndex         = 1,
    .unk_1C            = 1,
    .unk_1E            = 0,
    .unk_20            = 5,
    .unk_22            = 1,
    .unk_24            = 0,
    .unk_26            = 2,
    .unk_28            = 3,
    .animIndex         = 1,
};

static const SpriteAnimation OtosuGame_specialgauge_AnimDigit = {
    .bits_0_1          = 1,
    .dataType          = 0,
    .bit_6             = 0,
    .bits_7_9          = 5,
    .bits_10_11        = 0,
    .bits_12_13        = 1,
    .bits_14_15        = 0,
    .unk_02.raw        = 0,
    .posX              = 0x80,
    .posY              = 0x60,
    .frameInfoCallback = OtosuGame_specialgauge_GetFrameInfo,
    .callbackArg       = 0,
    .owner             = NULL,
    .binIden           = &data_ov039_0209a0fc,
    .unk_18            = 2,
    .packIndex         = 1,
    .unk_1C            = 1,
    .unk_1E            = 0,
    .unk_20            = 5,
    .unk_22            = 1,
    .unk_24            = 0,
    .unk_26            = 2,
    .unk_28            = 3,
    .animIndex         = 2,
};

static OtuGaugePos data_ov039_0209a780[4] = {
    {0x37, 0x9C},
    {0x27, 0x83},
    {0x14, 0x90},
    {0x1E, 0xA7},
};

static OtuGaugePos data_ov039_0209a790[4][2] = {
    {{0x39, 0x9C}, {0x32, 0x9C}},
    {{0x2A, 0x83}, {0x23, 0x83}},
    {{0x15, 0x90},  {0xE, 0x90}},
    {{0x22, 0xA7}, {0x1B, 0xA7}},
};

/**
 * @brief The palette block inside a loaded sprite resource's buffer.
 *
 * `buffer + 0x20` is the overlay's `PackHeader`; the word at the caller's
 * offset into the table that follows is a `PackEntry`'s `offset` field, and
 * adding it to the table's base is the sub-resource. This is the same walk as
 * `OtuPaletteSource` and `Data_GetPackEntryData`, with the entry index as a
 * parameter because this task needs two of them: the
 * update takes the fifth entry when a cell has gone dead and the fourth when it
 * has come back, and the arithmetic differs only in the constant.
 */
static inline void* OtuGaugePaletteSource(Data* file, s32 packEntry) {
    PackEntry* entries = (PackEntry*)((u8*)file->buffer + sizeof(PackHeader));

    return (u8*)entries + entries[packEntry].offset;
}

/* ============================================================================
 * Data
 * ==========================================================================*/

/* An overlay-global byte block in the main module, at 0x02071cf0. Nothing in
 * the overlay owns it; two unrelated tables inside it are written here. Declared
 * as a bare array so no dsd symbol has to be invented for the block itself. */
extern u8 data_02071cf0[];

SpriteFrameInfo* OtosuGame_specialgauge_GetFrameInfo(Sprite* sprite, s32 arg, s32 mode) {
    Sprite_FrameInfoCallbackSorted(sprite, mode, 3);
}

/* The four sprite loaders. */

/*
 * All four copy a 0x2C-byte `SpriteAnimation` onto the stack, stamp it with
 * this object's `dataType` and owner, and load one sprite. The copy is the
 * `ldm/stm` triple out of a literal pool, which is how mwcc renders a copy
 * from an `extern const` object -- not a local brace initialiser.
 *
 * The `dataType` write is a read-modify-write of the template's first halfword:
 * `bic #0x3C` clears the field's four bits and the `<< 0x1C` / `>> 0x1A` pair
 * is the insert. The `lsl #0x10` / `lsr #0x10` round trip in front of it is
 * the `(u16)` cast, and it is load-bearing: without it the value reaches the
 * field untruncated and mwcc emits a different sequence.
 */

/** Loads the affine-path sprite (0x44C) from the template at 0x02099bb4. */
void OtosuGame_specialgauge_LoadDial(OtosuGame_specialgauge* self, Sprite* sprite) {
    SpriteAnimation anim = OtosuGame_specialgauge_AnimDial;

    anim.owner    = self;
    anim.dataType = (u16)self->dataType;

    _Sprite_Load(sprite, &anim);
}

/** Loads the wide backing sprite (0x40C) from the template at 0x02099be0. */
void OtosuGame_specialgauge_LoadPlate(OtosuGame_specialgauge* self, Sprite* sprite) {
    SpriteAnimation anim = OtosuGame_specialgauge_AnimPlate;

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
 * convention the point task uses on its own digit table.
 */
void OtosuGame_specialgauge_LoadCell(OtosuGame_specialgauge* self, Sprite* sprite, s32 index) {
    SpriteAnimation anim = OtosuGame_specialgauge_AnimCell;

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
void OtosuGame_specialgauge_LoadDigit(OtosuGame_specialgauge* self, Sprite* sprite, s32 index, s32 which) {
    SpriteAnimation anim = OtosuGame_specialgauge_AnimDigit;

    anim.owner     = self;
    anim.dataType  = (u16)self->dataType;
    anim.posX      = data_ov039_0209a790[index][which].x;
    anim.posY      = data_ov039_0209a790[index][which].y;
    anim.animIndex = index + 1;

    _Sprite_Load(sprite, &anim);
}

/* The init stage, 0x020944bc. */

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
 * genuinely `s16`; nothing reads them, which is why they stay
 * `unk`.
 */
s32 OtosuGame_specialgauge_Init(TaskPool* pool, Task* task, void* arg) {
    OtosuGame_specialgauge*      self = (OtosuGame_specialgauge*)task->data;
    OtosuGame_specialgauge_Args* args = (OtosuGame_specialgauge_Args*)arg;
    s32                          i;

    self->dataType = args->dataType;
    self->pinId    = args->pinId;

    OtosuGame_specialgauge_LoadDial(self, &self->dial);
    OtosuGame_specialgauge_LoadPlate(self, &self->plate);

    for (i = 0; i < 4; i++) {
        self->lastCount[i] = 0;
        self->alive[i]     = 0;

        OtosuGame_specialgauge_LoadCell(self, &self->spriteA[i], i);
        Sprite_ChangeAnimation(&self->spriteA[i], self->spriteA[i].animData, 0xC, self->spriteA[i].cellTable);

        OtosuGame_specialgauge_LoadCell(self, &self->spriteB[i], i);
        OtosuGame_specialgauge_LoadDigit(self, &self->digit[i].spriteLow, i, 0);
        OtosuGame_specialgauge_LoadDigit(self, &self->digit[i].spriteHigh, i, 1);
    }

    self->affine.rotation = 0;
    self->affine.scaleX   = 0x1000;
    self->affine.scaleY   = 0x1000;
    self->affine.unk_0C   = 0;
    self->affine.unk_0E   = 0;

    return 1;
}

/* The update stage. */

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
s32 OtosuGame_specialgauge_Update(TaskPool* pool, Task* task, void* args) {
    OtosuGame_specialgauge* self = (OtosuGame_specialgauge*)task->data;
    OtosuGame_badge*        pin;
    s32                     i;
    s32                     count;
    s32                     alive;

    self->running         = 1;
    self->affine.rotation = (u16)(self->affine.rotation + 0x100);

    Sprite_Update(&self->dial);
    Sprite_Update(&self->plate);

    pin = (OtosuGame_badge*)EasyTask_GetTaskData(pool, self->pinId);

    for (i = 0; i < 4; i++) {
        s16               frame;
        s32               tens;
        UnkSmallInternal* pal;
        Data*             file;

        alive = 0;

        switch (i) {
            case 0:
                alive = func_ov039_0208e9d0(pin); // pin in phase 6
                count = func_ov039_0208eea0(pin);
                break;

            case 1:
                alive = func_ov039_0208e984(pin); // pin in phase 8
                count = func_ov039_0208eeac(pin);
                break;

            case 2:
                alive = func_ov039_0208e998(pin); // pin in phase 7
                count = func_ov039_0208eeb8(pin);
                break;

            case 3:
                alive = func_ov039_0208e9e4(pin); // pin in phase 9
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

            self->spriteA[i].unk3C          = pal;
            self->spriteB[i].unk3C          = pal;
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

                self->spriteA[i].unk3C          = pal;
                self->spriteB[i].unk3C          = pal;
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
                self->digit[i].spriteLow.posX = data_ov039_0209a790[i][0].x;
                self->digit[i].spriteLow.posY = data_ov039_0209a790[i][0].y;

                tens  = count / 10;
                frame = (s16)(tens % 10);
                Sprite_ChangeAnimation(&self->digit[i].spriteHigh, self->digit[i].spriteHigh.animData, frame + 2,
                                       self->digit[i].spriteHigh.cellTable);

                self->digit[i].spriteHigh.posX = data_ov039_0209a790[i][1].x;
                self->digit[i].spriteHigh.posY = data_ov039_0209a790[i][1].y;
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

/* The render stage, 0x020948f4. */

/**
 * @brief The gauge's render stage: one affine sprite, then all eighteen.
 *
 * Only the dial is transformed. Its OAM attribute word's affine-slot field --
 * `Sprite.unk_0A.raw`'s bits 5..9 -- is replaced with the slot the allocator
 * returns, and the `(u32)(u16)` around that return is load-bearing twice: it is
 * the `<< 0x10` / `>> 0x10` pair the target emits, and it makes the following
 * `>> 0x16` a logical rather than an arithmetic shift. The entry task's
 * render is the same expression over the same manager array.
 *
 * The manager is picked from the sprite's own display-engine bits, exactly as
 * the entry task does it, and the scale pair comes from the task rather than from a
 * per-sprite limit block.
 *
 * Nothing in the stage is positioned here; every sprite draws where the update
 * left it, which is the opposite of the hammer and the point task
 * and is why this task needs no render-side anchor arithmetic at all.
 */
s32 OtosuGame_specialgauge_Render(TaskPool* pool, Task* task, void* args) {
    OtosuGame_specialgauge* self = (OtosuGame_specialgauge*)task->data;
    s32                     i;

    if (self->running != 0) {
        self->dial.unk_0A.unk_05 = (u16)OamMgr_AllocAffineGroup(&g_OamMgr[self->dial.bits_0_1], self->affine.rotation,
                                                                self->affine.scaleX, self->affine.scaleY, 0);
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

/* The cleanup stage, 0x020949f4. */

/**
 * @brief Releases all eighteen sprites, and keeps running.
 *
 * The dial goes first and the plate second, then the four cells in the same
 * order the update walks them. `Sprite_Release` does not care about order, so
 * this is simply the reverse of the two 0x40/0x80 pointer setups the stage
 * shares with the update -- which is why the target builds its four walking
 * pointers identically in the two functions.
 */
s32 OtosuGame_specialgauge_Destroy(TaskPool* pool, Task* task, void* args) {
    OtosuGame_specialgauge* self = (OtosuGame_specialgauge*)task->data;
    s32                     i;

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

/* The dispatcher and the spawner. */

/**
 * @brief The stage dispatcher, and the `TaskHandle`'s own `taskFunc`.
 *
 * Identical in shape to every task's: the four callbacks are copied
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
s32 OtosuGame_specialgauge_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = OtosuGame_specialgauge_Init,
        .update     = OtosuGame_specialgauge_Update,
        .render     = OtosuGame_specialgauge_Render,
        .cleanup    = OtosuGame_specialgauge_Destroy,
    };

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
s32 OtosuGame_specialgauge_CreateTask(TaskPool* pool, s32 arg1, s32 arg2) {
    OtosuGame_specialgauge_Args args;

    args.dataType = arg1;
    args.pinId    = arg2;

    return EasyTask_CreateTask(pool, &Tsk_OtosuGame_specialgauge, NULL, 0, NULL, &args);
}
