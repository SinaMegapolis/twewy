#include "OtuFieldAccessShared.h"

/*
 * ov039 region 0x02094214 - 0x020950b8. One translation unit of the
 * overlay; dsd gives each file a single contiguous `.text` claim. The
 * shared types, externs and prototypes are in OtuFieldAccessShared.h.
 */
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
            if (t->unk_18 != 0 && (table = t->cellTable) != NULL && (index = t->index) >= 0) {
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
    OtuGauge*   self = (OtuGauge*)task->data;
    OtuPinTask* pin;
    s32         i;
    s32         count;
    s32         alive;

    self->running  = 1;
    self->rotation = (u16)(self->rotation + 0x100);

    Sprite_Update(&self->dial);
    Sprite_Update(&self->plate);

    pin = (OtuPinTask*)EasyTask_GetTaskData(pool, self->pinId);

    for (i = 0; i < 4; i++) {
        s16               frame;
        s32               tens;
        UnkSmallInternal* pal;
        Data*             file;

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
                self->digit[i].spriteLow.posX = data_ov039_0209a790[i].lowX;
                self->digit[i].spriteLow.posY = data_ov039_0209a790[i].lowY;

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
        self->dial.unk_0A.raw =
            (self->dial.unk_0A.raw & ~0x3E0) |
            ((u32)(u16)OamMgr_AllocAffineGroup(&g_OamMgr[self->dial.bits_0_1], self->rotation, self->scaleX, self->scaleY, 0)
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
OtuSpriteSlot* func_ov039_02094ae8(OtuSpriteTask* t, s32 arg, s32 mode) {
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
            if (t->unk_18 != 0 && (table = t->cellTable) != NULL && (index = t->index) >= 0) {
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
 * Loads the sprite for the task whose anim template is data_ov039_02099c80.
 *
 * The only member of the family whose position is a difference rather than a
 * raw value: x is `+0x58 - +0x50` and y adds a third term at +0x60, the same
 * pair `func_ov039_02094dac` writes into the sprite's posX/posY.
 */
void func_ov039_02094bac(void* self, void* sprite, s32* args) {
    SpriteAnimation anim = data_ov039_02099c80;

    anim.owner    = self;
    anim.dataType = (u16) * (s32*)args;
    anim.posX     = (s32)((*(s32*)((u8*)self + 0x58) - *(s32*)((u8*)self + 0x50)) >> 12);
    anim.posY     = (s32)((*(s32*)((u8*)self + 0x60) + (*(s32*)((u8*)self + 0x5C) - *(s32*)((u8*)self + 0x54))) >> 12);
    _Sprite_Load((Sprite*)sprite, &anim);
}

/** The same reset against the sibling helper, over a longer zero run. */
s32 func_ov039_02094c50(void* pool, void* task, s32* args) {
    u8* sprite = *(u8**)((u8*)task + 0x18);

    (void)pool;

    *(s32*)(sprite + 0x74) = 0;
    *(s32*)(sprite + 0x78) = 0;
    *(s32*)(sprite + 0x50) = 0;
    *(s32*)(sprite + 0x54) = 0;
    *(s32*)(sprite + 0x58) = 0;
    *(s32*)(sprite + 0x5C) = 0;
    *(s32*)(sprite + 0x60) = 0;
    *(s32*)(sprite + 0x64) = 0;
    *(s32*)(sprite + 0x68) = 0;
    *(s32*)(sprite + 0x40) = 0;
    *(s32*)(sprite + 0x44) = 0x2000;
    *(s32*)(sprite + 0x48) = 0x2000;
    *(s16*)(sprite + 0x4C) = 0;
    *(s16*)(sprite + 0x4E) = 0;

    func_ov039_02094bac(sprite, sprite, args);
    return 1;
}

/**
 * @brief The spring step: a scaled value, then a damped velocity and a bounce.
 *
 * +0x80 is a countdown. While it runs, +0x44 and +0x48 are both set to
 * `(count << 13) / +0x7C`, the +0x6C axis is stepped down by
 * `data_ov039_0209a304` and clamped at zero, +0x70 is stepped up by
 * `data_ov039_0209a310`, and `func_ov039_02098c00` integrates the pair at
 * +0x64 into +0x58. The +0x60 accumulator is then moved by +0x70 and, when it
 * overshoots, is clamped and its velocity damped by `-data_ov039_0209a300`.
 */
s32 func_ov039_02094ca8(void* pool, void* task) {
    u8* sprite = *(u8**)((u8*)task + 0x18);
    s32 count;
    s32 scale;
    s32 v;

    (void)pool;

    /* One `return 1` at the end, and the guard arms ordered so the *clear* is
     * the `then`: the target lays that inline as the fall-through and branches
     * to the spring body, which `if (count > 0) { spring } else { clear }`
     * inverts. `func_ov039_020981a4` needed the opposite polarity on its first
     * guard -- in both cases it is the layout, not the meaning, that decides. */
    if (*(s32*)(sprite + 0x74) != 0) {
        count                  = *(s32*)(sprite + 0x80) - 1;
        *(s32*)(sprite + 0x80) = count;

        if (count <= 0) {
            *(s32*)(sprite + 0x74) = 0;
            *(s32*)(sprite + 0x78) = 0;
        } else {
            scale                  = (count << 13) / *(s32*)(sprite + 0x7C);
            *(s32*)(sprite + 0x48) = scale;
            *(s32*)(sprite + 0x44) = scale;

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
            *(s32*)(sprite + 0x78) = 1;
        }
    }
    return 1;
}

/** Steps and renders the sprite block at task+0x18 when +0x78 is set. */
s32 func_ov039_02094dac(void* pool, void* task) {
    Sprite* sprite = *(Sprite**)((u8*)task + 0x18);

    (void)pool;

    if (*(s32*)((u8*)sprite + 0x78) != 0) {
        *(s16*)((u8*)sprite + 0x0C) = (s16)((*(s32*)((u8*)sprite + 0x58) - *(s32*)((u8*)sprite + 0x50)) >> 12);
        *(s16*)((u8*)sprite + 0x0E) =
            (s16)((*(s32*)((u8*)sprite + 0x60) + (*(s32*)((u8*)sprite + 0x5C) - *(s32*)((u8*)sprite + 0x54))) >> 12);
        Sprite_RenderFrame(sprite);
    }
    return 1;
}

/** Releases the sprite at task+0x18 and reports success. */
s32 func_ov039_02094dfc(void* pool, void* task) {
    (void)pool;
    Sprite_Release(*(Sprite**)((u8*)task + 0x18));
    return 1;
}

void func_ov039_02094e10(void* a, void* b, void* c, s32 index) {
    TaskStages table = data_ov039_02099c70;

    table.iter[index](a, b, c);
}

/** Spawns the task table data_ov039_02099c64 with one word of args. */
s32 func_ov039_02094e58(TaskPool* pool, s32 arg) {
    s32 args = arg;

    return EasyTask_CreateTask(pool, &data_ov039_02099c64, NULL, 0, NULL, &args);
}

/** Writes the +0x50/+0x54 pair from a point -- the pair's setter. */
void func_ov039_02094e88(void* task, OtuPoint* in) {
    *(OtuPoint*)((u8*)task + 0x50) = *in;
}

/**
 * @brief Randomises the sprite's launch vector and picks a starting animation.
 *
 * The two velocity components are drawn from opposite ends of their ranges:
 * +0x6C starts at `data_ov039_0209a314` and is pushed *up* by a random span,
 * +0x70 starts at the negation of `data_ov039_0209a2fc` and is pushed *down*.
 * The magnitude at +0x7C/+0x80 is then that second component scaled by 3 and
 * taken as an absolute value -- spelled as the three-way ternary `abs` is a
 * macro for, which is why `FX_Divide` is called three times in the target
 * rather than once and reused.
 */
// Nonmatching: 85.5%, all of it scheduling and register choice at the top of
// the body. The target loads both `at` components before either is stored (x in
// r2, y in r1); this source stores x before it loads y. Hoisting the two reads
// into named locals is the obvious fix and scores *worse* (84%), because it
// also moves the pool-constant loads. Everything from the `data_ov039_0209a314`
// store down agrees instruction for instruction, including the three
// `FX_Divide` calls.
void func_ov039_02094e9c(void* task, OtuPoint* at) {
    u8* sprite = (u8*)task;
    s32 cell;
    s32 mag;

    *(s32*)(sprite + 0x74) = 1;
    *(s32*)(sprite + 0x44) = 0x2000;
    *(s32*)(sprite + 0x48) = 0x2000;
    *(s32*)(sprite + 0x58) = at->x;
    *(s32*)(sprite + 0x5C) = at->y;
    *(s32*)(sprite + 0x60) = 0;

    *(s32*)(sprite + 0x6C) = data_ov039_0209a314;
    *(s32*)(sprite + 0x6C) = *(s32*)(sprite + 0x6C) + RNG_Next(data_ov039_0209a328 - data_ov039_0209a314);

    *(s32*)(sprite + 0x70) = -data_ov039_0209a2fc;
    *(s32*)(sprite + 0x70) = *(s32*)(sprite + 0x70) - RNG_Next(data_ov039_0209a30c - data_ov039_0209a2fc);

#define OTU_SPRING_MAG ((FX_Divide(*(s32*)(sprite + 0x70), data_ov039_0209a310) * 3) >> 12)
    /* `abs` as the macro it is: the argument is evaluated once for the test and
     * once per arm, so the three-way form is load-bearing. The `< 0` polarity
     * is too -- the target lays the negating arm out inline and branches to the
     * plain one (`bpl`), and the `>= 0` spelling inverts that. */
    mag = OTU_SPRING_MAG < 0 ? -OTU_SPRING_MAG : OTU_SPRING_MAG;
#undef OTU_SPRING_MAG

    *(s32*)(sprite + 0x7C) = mag;
    *(s32*)(sprite + 0x80) = mag;

    /* The sin/cos table is s32[] but holds pairs of s16, so each half has to be
     * fetched through a s16* at a doubled byte offset. */
    cell                   = RNG_Next(0x10000) >> 4;
    *(s32*)(sprite + 0x64) = *(s16*)((u8*)&data_0205e4e0 + (cell * 2 + 1) * 2);
    *(s32*)(sprite + 0x68) = *(s16*)((u8*)&data_0205e4e0 + (cell * 2) * 2);

    Sprite_ChangeAnimation((Sprite*)sprite, *(s32*)(sprite + 0x18), *(s32*)(sprite + 0x1C), (s16)(RNG_Next(3) + 1));
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
OtuSpriteSlot* func_ov039_02094ff4(OtuSpriteTask* t, s32 arg, s32 mode) {
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
            if (t->unk_18 != 0 && (table = t->cellTable) != NULL && (index = t->index) >= 0) {
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
