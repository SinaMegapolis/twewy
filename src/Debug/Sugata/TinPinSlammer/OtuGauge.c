#include "OtuFieldAccessShared.h"

/*
 * ov039 region 0x02094214 - 0x020950b8. One translation unit of the
 * overlay; dsd gives each file a single contiguous `.text` claim. The
 * shared types, externs and prototypes are in OtuFieldAccessShared.h.
 */

/* ==================================================================== */
/* Tsk_OtosuGame_specialgauge                                           */
/* ==================================================================== */

SpriteFrameInfo* func_ov039_02094214(Sprite* sprite, s32 arg, s32 mode) {
    Sprite_FrameInfoCallbackSorted(sprite, mode, 3);
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
    anim.posX      = data_ov039_0209a790[index][which].x;
    anim.posY      = data_ov039_0209a790[index][which].y;
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

    self->affine.rotation = 0;
    self->affine.scaleX   = 0x1000;
    self->affine.scaleY   = 0x1000;
    self->affine.unk_0C   = 0;
    self->affine.unk_0E   = 0;

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
    OtuGauge* self = (OtuGauge*)task->data;
    OtuBadge* pin;
    s32       i;
    s32       count;
    s32       alive;

    self->running         = 1;
    self->affine.rotation = (u16)(self->affine.rotation + 0x100);

    Sprite_Update(&self->dial);
    Sprite_Update(&self->plate);

    pin = (OtuBadge*)EasyTask_GetTaskData(pool, self->pinId);

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

/* ==================================================================== */
/* Tsk_OtosuGame_spark                                                  */
/* ==================================================================== */

SpriteFrameInfo* func_ov039_02094ae8(Sprite* sprite, s32 arg, s32 mode) {
    OtuSpark* owner = sprite->owner;

    Sprite_FrameInfoCallbackAffineSorted(sprite, mode, &owner->affine, func_ov039_02088400(3, owner->pos.y, owner->height));
}

void func_ov039_02094bac(OtuSpark* self, Sprite* sprite, OtuTaskArgs1* args) {
    SpriteAnimation anim = data_ov039_02099c80;

    anim.owner    = self;
    anim.dataType = (u16)args->dataType;
    anim.posX     = (s32)((self->pos.x - self->origin.x) >> 12);
    anim.posY     = (s32)((self->height + (self->pos.y - self->origin.y)) >> 12);
    _Sprite_Load(sprite, &anim);
}

s32 func_ov039_02094c50(TaskPool* pool, Task* task, OtuTaskArgs1* args) {
    OtuSpark* self = task->data;

    self->active          = 0;
    self->visible         = 0;
    self->origin.x        = 0;
    self->origin.y        = 0;
    self->pos.x           = 0;
    self->pos.y           = 0;
    self->height          = 0;
    self->dir.x           = 0;
    self->dir.y           = 0;
    self->affine.rotation = 0;
    self->affine.scaleX   = 0x2000;
    self->affine.scaleY   = 0x2000;
    self->affine.unk_0C   = 0;
    self->affine.unk_0E   = 0;

    func_ov039_02094bac(self, &self->sprite, args);
    return 1;
}

/**
 * Flies the spark like an OtuHahen. The clear is the `then` arm and there is
 * one shared `return 1`: that is the target's block layout.
 */
s32 func_ov039_02094ca8(TaskPool* pool, Task* task, void* args) {
    OtuSpark* self = task->data;
    s32       count;
    s32       scale;
    s32       v;

    if (self->active != 0) {
        count      = self->life - 1;
        self->life = count;

        if (count <= 0) {
            self->active  = 0;
            self->visible = 0;
        } else {
            scale               = (count << 13) / self->lifeMax;
            self->affine.scaleY = scale;
            self->affine.scaleX = scale;

            v           = self->speed - data_ov039_0209a304;
            self->speed = v;
            if (v < 0) {
                self->speed = 0;
            }

            self->vz = self->vz + data_ov039_0209a310;

            func_ov039_02098c00(self->speed, &self->dir, &self->pos, &self->pos);

            v            = self->height + self->vz;
            self->height = v;
            if (v > 0) {
                self->height = 0;
                if (self->vz > 0) {
                    self->vz = (s32)(((s64)self->vz * -data_ov039_0209a300 + 0x800) >> 12);
                }
            }

            Sprite_Update(&self->sprite);
            self->visible = 1;
        }
    }
    return 1;
}

s32 func_ov039_02094dac(TaskPool* pool, Task* task, void* args) {
    OtuSpark* self = task->data;

    if (self->visible != 0) {
        self->sprite.posX = (s16)((self->pos.x - self->origin.x) >> 12);
        self->sprite.posY = (s16)((self->height + (self->pos.y - self->origin.y)) >> 12);
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

s32 func_ov039_02094dfc(TaskPool* pool, Task* task, void* args) {
    Sprite_Release(&((OtuSpark*)task->data)->sprite);
    return 1;
}

s32 func_ov039_02094e10(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = data_ov039_02099c70;

    return stages.iter[stage](pool, task, args);
}

s32 func_ov039_02094e58(TaskPool* pool, s32 dataType) {
    OtuTaskArgs1 args;

    args.dataType = dataType;
    return EasyTask_CreateTask(pool, &data_ov039_02099c64, NULL, 0, NULL, &args);
}

/** Sets the point the spark is drawn relative to. */
void func_ov039_02094e88(OtuSpark* self, OtuPoint* origin) {
    self->origin = *origin;
}

/**
 * Launches a spark from `at`: a jittered speed, an upward kick, a random
 * direction from the base game's table and one of animations 1..3.
 */
// Nonmatching: 88.9%, scheduling at the top: the target loads both halves of
// `at` before storing either.
void func_ov039_02094e9c(OtuSpark* self, OtuPoint* at) {
    s32 cell;
    s32 airtime;

    self->active        = 1;
    self->affine.scaleX = 0x2000;
    self->affine.scaleY = 0x2000;
    self->pos.x         = at->x;
    self->pos.y         = at->y;
    self->height        = 0;

    self->speed = data_ov039_0209a314;
    self->speed = self->speed + RNG_Next(data_ov039_0209a328 - data_ov039_0209a314);

    self->vz = -data_ov039_0209a2fc;
    self->vz = self->vz - RNG_Next(data_ov039_0209a30c - data_ov039_0209a2fc);

    airtime = OTU_ABS_AIRTIME(self->vz);

    self->lifeMax = airtime;
    self->life    = airtime;

    cell        = RNG_Next(0x10000) >> 4;
    self->dir.x = ((s16*)data_0205e4e0)[cell * 2 + 1];
    self->dir.y = ((s16*)data_0205e4e0)[cell * 2];

    Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, (s16)(RNG_Next(3) + 1), self->sprite.cellTable);
}

/* ==================================================================== */
/* Tsk_OtosuGame_slash (continued in OtuEntryTasks.c)                   */
/* ==================================================================== */

SpriteFrameInfo* func_ov039_02094ff4(Sprite* sprite, s32 arg, s32 mode) {
    OtuSlashTask* owner = sprite->owner;

    Sprite_FrameInfoCallbackAffineSorted(sprite, mode, &owner->affine, func_ov039_02088400(6, owner->pos.y, 0));
}
