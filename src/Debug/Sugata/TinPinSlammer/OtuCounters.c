#include "OtuFieldAccessShared.h"

/*
 * ov039 region 0x020933c0 - 0x02094214. One translation unit of the
 * overlay; dsd gives each file a single contiguous `.text` claim. The
 * shared types, externs and prototypes are in OtuFieldAccessShared.h.
 */
/* ==================================================================== */
/* The background task: Tsk_OtosuGame_ovbg.                             */
/* ==================================================================== */

/**
 * @brief Updates the background task's animated palette, 0x020933c0.
 *
 * `func_ov039_02098dbc` advances the ring buffer at +0x840 by one entry and
 * returns the pointer to the palette that entry selects; that pointer is then
 * handed to `PaletteMgr_SetSource` for the second of the task's four palette
 * resources (+0x10, the one init stage 02092f88 allocated with slot index 1).
 *
 * The other three layers are left alone, so this is a per-frame recolour of
 * one BG layer rather than a repaint.
 */
s32 func_ov039_020933c0(TaskPool* pool, Task* self, void* arg) {
    OtuOvbg* data = (OtuOvbg*)self->data;

    PaletteMgr_SetSource(g_PaletteManagers[1], data->palettes[1], func_ov039_02098dbc(&data->paletteAnim));
    return 1;
}

/**
 * @brief The background task's render stage, 0x020933f0.
 *
 * Returns TRUE and draws nothing, which is what a four-BG-layer task needs: its
 * layers are set up once by the init stage and then repainted by the hardware
 * until the cleanup stage releases them.
 *
 * This is decidable from the table slot rather than guessed. `data_ov039_020999c0`
 * is `{02092f88, 020933c0, 020933f0, 020933f8}`, which is init / update /
 * render / cleanup, and 02092f88 allocates four `BgResMgr` layer triples and
 * no sprites. So this is a real empty render stage for a real reason.
 *
 * Note this does *not* generalise to the other three constant-1 bodies in the
 * overlay (020921f4, 02092bf0, 02096d98). 020921f4 and 02092bf0 are also
 * render slots, but 02096d98 sits at index 1 of `data_ov039_02099ec8` -- an
 * update slot -- for a task whose render (02096da0) is the one that does the
 * work. Same instruction, different meaning; the four are left independent.
 */
s32 func_ov039_020933f0(TaskPool* pool, Task* self, void* arg) {
    return 1;
}

/**
 * @brief Releases the background task's four layer triples, 0x020933f8.
 *
 * One pass per layer, screen first, then char data, then the palette -- the
 * reverse of the order init stage 02092f88 acquired them in. The layer index
 * runs 0..3 and is *not* multiplied into the field offsets: each of the three
 * resources is a four-entry array with a four-byte stride, so the three loads
 * inside the loop all share one `data + i * 4` base and differ only in their
 * displacement. Written that way deliberately; indexing the arrays as arrays
 * costs an add per access.
 */
s32 func_ov039_020933f8(TaskPool* pool, Task* self, void* arg) {
    OtuOvbg* data = (OtuOvbg*)self->data;
    s32      i;

    for (i = 0; i < 4; i++) {
        BgResMgr_ReleaseChar(g_BgResourceManagers[1], data->screens[i]);
        BgResMgr_ReleaseChar(g_BgResourceManagers[1], data->chars[i]);
        PaletteMgr_ReleaseResource(g_PaletteManagers[1], data->palettes[i]);
    }

    DatMgr_ReleaseData(data->fileData);
    return 1;
}

/**
 * @brief The background task's stage dispatcher, 0x02093460.
 *
 * The `TaskStages` table is copied to the stack rather than indexed in place,
 * which is why the target spends four instructions on an `ldm`/`stm` pair
 * before dispatching. `stage` is the fourth argument, which is what
 * `EasyTask` passes when it wants one stage run rather than the whole
 * lifecycle.
 */
s32 func_ov039_02093460(TaskPool* pool, Task* self, void* arg, s32 stage) {
    TaskStages stages = data_ov039_020999c0;

    return stages.iter[stage](pool, self, arg);
}

/**
 * @brief Creates the background task, 0x020934a8.
 *
 * The three arguments are gathered into a stack block and handed to
 * `EasyTask_CreateTask` as its `param`; the init stage 02092f88 reads them back
 * out of there. Priority is 0 and the parent is NULL, so the caller supplies
 * the pool and nothing else.
 *
 * Typed as returning the handle rather than void: a caller elsewhere in this
 * overlay stores the result into a child-handle field. It costs nothing
 * here -- the body has no `mov r0` of its own in either spelling, so the
 * handle already comes back in r0 and both compile to the same
 * instructions. Same finding as func_ov039_02098394 in band 8.
 */
s32 func_ov039_020934a8(TaskPool* pool, s32 dataType, Heap* heap, OtuBoardLayout* layout) {
    OtuBoardArgs args;

    args.dataType = dataType;
    args.heap     = heap;
    args.layout   = layout;

    return EasyTask_CreateTask(pool, &data_ov039_020999b4, NULL, 0, NULL, &args);
}

/**
 * @brief Re-points the animated palette at the overlay's own table, 0x020934e0.
 *
 * Idempotent: if the object at +0x840 is already running `data_ov039_02099a18`
 * this returns without touching it, so a caller can run it every frame. The
 * source pointer handed over is offset 0x48 into the loaded file's buffer --
 * the same expression init stage 02092f88 uses when it first builds the object,
 * so both agree on which half of the file the animation reads.
 *
 * The `count` argument is 0x12, and it is a count of table entries rather than
 * frames: `func_ov039_02098dbc` steps through the table one entry per call and
 * wraps at this value.
 */
void func_ov039_020934e0(OtuOvbg* data) {
    if (data->paletteAnim.table == data_ov039_02099a18) {
        return;
    }

    func_ov039_02098d7c(&data->paletteAnim, data->fileData ? (s32)OtuPaletteSource(data->fileData) : 0, data_ov039_02099a18,
                        0x12);
}

/* ==================================================================== */
/* Tsk_OtosuGame_badgeradar                                             */
/* ==================================================================== */

SpriteFrameInfo* func_ov039_0209352c(Sprite* sprite, s32 arg, s32 mode) {
    Sprite_FrameInfoCallbackSorted(sprite, mode, 3);
}

/* ==================================================================== */
/* The sprite-load wrappers.                                            */
/* ==================================================================== */

/*
 * Five copies of one body, 0x020935d4 / 0209392c / 020939b0 / 02093a30 /
 * 02093ee4. `-inline noauto` means a shared `static` would compile to a real
 * `bl` and cost every one of them its body, so each is written out.
 *
 * What they all do:
 *
 *   1. copy a 0x2C-byte `SpriteAnimation` template out of `.rodata` onto a
 *      stack local -- the frame is exactly 0x2C, so this is a struct copy;
 *   2. stamp `owner` with the caller, so the cell builder can find its way
 *      back to the task data;
 *   3. rebuild the first halfword's bits 2-5 from a caller-supplied byte:
 *      `ldrh` the template's word, `bic` the field, shift the new value in.
 *      That is the template's `dataType`, and the byte comes from the task
 *      data's own +0x00 -- the same word 020939b0 reads as a `Sprite`'s
 *      display-engine bits. Whether the two are meant to agree is not stated;
 *      what is clear is that the field is four bits wide and the value is
 *      masked to it by the shift rather than by an explicit `and`.
 *   4. set `animIndex` from the task data, in each wrapper's own way;
 *   5. `_Sprite_Load(sprite, &anim)`.
 *
 * The `animIndex` arithmetic is the interesting part, and it varies per
 * wrapper rather than per object:
 *
 *   020935d4   `(base == 1 ? 5 : 1) + (s16)bias`  -- two different animation
 *              choices selected by a flag, then a signed bias added
 *   0209392c   untouched; instead `posY += count * 0x13`
 *   020939b0   `-(index + 1) * 7`   -- `rsb r, r, r, lsl #3`
 *   02093a30   `lut[index]`, and `posX += (index - 2) * 6` for index < 3
 *   02093ee4   untouched; instead `posX = (0x1F - index * 2) * 8` and
 *              `posY = 0x12`
 */

/**
 * @brief Loads the radar task's one sprite, 0x020935d4.
 *
 * The only wrapper that reads two separate fields to build its animation
 * index. `animBase` at +0x5C picks between animation 5 and animation 1, and
 * `animBias` at +0x50 is then added as a signed 16-bit value -- the
 * `lsl #0x10 / asr #0x10` pair, i.e. a sign-extending narrowing of a word.
 */
void func_ov039_020935d4(OtuBadgeRadar* data, Sprite* sprite) {
    SpriteAnimation anim = data_ov039_02099a7c;

    anim.owner     = data;
    anim.dataType  = data->dataType;
    anim.animIndex = (data->isFirst == 1) ? 5 : 1;
    anim.animIndex = anim.animIndex + (s16)data->index;

    _Sprite_Load(sprite, &anim);
}

/* ==================================================================== */
/* The badge-radar task: Tsk_OtosuGame_badgeradar.                       */
/* ==================================================================== */

/**
 * @brief The radar task's init stage, 0x02093668.
 *
 * Copies the five-word argument block in, clears the Q12.12 position and the
 * "linked" flag, then loads the sprite. The sprite's `owner` is the task data
 * block, which is how the cell builder handed to `_Sprite_Load` finds its way
 * back to +0x18/+0x1C/+0x16 when it is called per frame.
 */
s32 func_ov039_02093668(TaskPool* pool, Task* self, OtuBadgeRadarArgs* args) {
    OtuBadgeRadar* data = (OtuBadgeRadar*)self->data;

    data->dataType = args->dataType;
    data->pinId    = args->pinId;
    data->index    = args->index;
    data->board    = args->board;
    data->linked   = 0;
    data->isFirst  = args->isFirst;
    data->x        = 0;
    data->y        = 0;

    func_ov039_020935d4(data, &data->sprite);
    return 1;
}

/**
 * @brief The radar task's update stage, 0x020936b8.
 *
 * Follows one child task and converts its Q12.12 position into the radar
 * sprite's:
 *
 *   1. ask the pool for `targetId`'s data; if it is gone, clear `linked` and
 *      stop -- the render stage then draws nothing at all;
 *   2. ask func_ov039_0208ef4c whether that child is a real pin rather than an
 *      empty tray slot, and clear `linked` if not;
 *   3. copy the child's +0x120/+0x124 pair out (func_ov039_0208e6e0, one
 *      `OtuPoint`) and set `linked`;
 *   4. for each axis: subtract 0xA0000, add a per-child skew read from the
 *      byte table at +0x54 (bytes 2 and 3 respectively), divide the lot by 8,
 *      and add a per-axis origin (0x50000 for x, 0x10000 for y);
 *   5. `Sprite_Update`.
 *
 * Step 4 is a screen-space transform: 0xA0000 is 160.0 in Q12.12, the skew is
 * `((0x32 - b) / 2) << 17`, and the origins differ per axis, so x and y come
 * out of the same arithmetic with different constants. The `/8` is a real
 * signed divide with the sign correction spelled out in the target, not a
 * shift -- writing it as `/ 8` reproduces it.
 *
 * Note `linked` is written twice on the success path (before and after the
 * point copy). That is the target's shape and it is left alone: the second
 * store is dead, but removing it changes nothing and keeping it costs nothing.
 *
 * 0xA0000 is 160.0 in Q12.12, so this reads as "pin position, shifted 160
 * units left and divided by eight", with a per-child skew folded in from the
 * byte table. What the byte table *is* is not established here -- it arrives as
 * a bare pointer in the init stage and nothing in this band writes it.
 */

// Nonmatching: 91.3%. Everything through the position arithmetic matches
// instruction for instruction, including both sign-corrected divides and the
// redundant `linked` store. The remaining gap is where the null-child block
// sits: the target branches past it to a single out-of-line clear at the bottom
// of the function (`beq` forward, then `mov r0, #0; str r0, [r4, #0x58]` after
// the `Sprite_Update` block), where the `else if` chain here puts the same
// store inline ahead of the branch. Same work, different block placement; it
// was not worth a `goto` to chase two instructions.
s32 func_ov039_020936b8(TaskPool* pool, Task* self, void* arg) {
    OtuBadgeRadar* data  = (OtuBadgeRadar*)self->data;
    OtuBadge*      child = (OtuBadge*)EasyTask_GetTaskData(pool, data->pinId);
    OtuPoint       pt;
    s32            skew;

    if (child == NULL) {
        data->linked = 0;
    } else if (func_ov039_0208ef4c(child, 0) == 0) {
        data->linked = 0;
    } else {
        data->linked = 1;
        func_ov039_0208e6e0(child, &pt);
        data->linked = 1;

        data->x = pt.x - 0xA0000;
        skew    = (0x28 - (data->board->width - 0xA)) / 2;
        data->x = (data->x + (skew << 17)) / 8 + 0x50000;

        data->y = pt.y - 0xA0000;
        skew    = (0x28 - (data->board->height - 0xA)) / 2;
        data->y = (data->y + (skew << 17)) / 8 + 0x10000;

        Sprite_Update(&data->sprite);
    }

    return 1;
}

/**
 * @brief The radar task's render stage, 0x020937a0.
 *
 * Converts the Q12.12 position at +0x44/+0x48 to pixels and draws, but only
 * while `linked` is set. So the update stage's job is to keep `linked`
 * tracking whether the tracked child exists, and the render stage does the
 * arithmetic.
 */
s32 func_ov039_020937a0(TaskPool* pool, Task* self, void* arg) {
    OtuBadgeRadar* data = (OtuBadgeRadar*)self->data;

    if (data->linked != 0) {
        data->sprite.posX = data->x >> 12;
        data->sprite.posY = data->y >> 12;
        Sprite_RenderFrame(&data->sprite);
    }

    return 1;
}

/**
 * @brief The radar task's cleanup stage, 0x020937dc.
 */
s32 func_ov039_020937dc(TaskPool* pool, Task* self, void* arg) {
    OtuBadgeRadar* data = (OtuBadgeRadar*)self->data;

    Sprite_Release(&data->sprite);
    return 1;
}

/** The radar task's stage dispatcher, 0x020937f4. */
s32 func_ov039_020937f4(TaskPool* pool, Task* self, void* arg, s32 stage) {
    TaskStages stages = data_ov039_02099a6c;

    return stages.iter[stage](pool, self, arg);
}

/**
 * @brief Creates the radar task, 0x0209383c.
 *
 * Five arguments this time, against three for the background task -- which is
 * the visible difference between the two `sub sp, sp, #imm` frame sizes
 * (0x1C against 0x14) and is how the two create wrappers tell themselves apart.
 */
s32 func_ov039_0209383c(TaskPool* pool, s32 dataType, s32 pinId, s32 index, OtuBoardLayout* board, s32 isFirst) {
    OtuBadgeRadarArgs args;

    args.dataType = dataType;
    args.pinId    = pinId;
    args.index    = index;
    args.board    = board;
    args.isFirst  = isFirst;

    return EasyTask_CreateTask(pool, &data_ov039_02099a60, NULL, 0, NULL, &args);
}

SpriteFrameInfo* func_ov039_02093884(Sprite* sprite, s32 arg, s32 mode) {
    Sprite_FrameInfoCallbackSorted(sprite, mode, 3);
}

/**
 * @brief Loads the counter task's +0x04 sprite, 0x0209392c.
 *
 * Leaves `animIndex` alone and offsets the vertical position instead:
 * `posY += unk_188 * 0x13`. `unk_188` is the index the init stage was handed
 * and the one 02093d18 uses to pick between animation groups, so this is the
 * same per-menu selector -- laid out as 19 pixels of vertical offset rather
 * than as an animation choice.
 */
void func_ov039_0209392c(OtuBadgeCount* data, Sprite* sprite) {
    SpriteAnimation anim = data_ov039_02099af8;

    anim.owner    = data;
    anim.dataType = data->dataType;
    anim.posY     = anim.posY + data->index * 0x13;

    _Sprite_Load(sprite, &anim);
}

/**
 * @brief Loads the counter task's +0x44 sprite, 0x020939b0.
 *
 * `animIndex = (unk_188 + 1) * 7`, which the target spells `add r, r, #1`
 * then `rsb r, r, r, lsl #3` (that is `8x - x`). Seven per `unk_188` again --
 * the same stride 02093d18 walks when it searches for a free tray slot, which
 * is why the two are recognisably the same family: `unk_188` names a menu and
 * each menu owns seven animations, and this sprite is showing the seventh.
 */
void func_ov039_020939b0(OtuBadgeCount* data, Sprite* sprite) {
    SpriteAnimation anim = data_ov039_02099acc;

    anim.owner     = data;
    anim.dataType  = data->dataType;
    anim.animIndex = (data->index + 1) * 7;

    _Sprite_Load(sprite, &anim);
}

/**
 * @brief Loads one of the counter task's four digit sprites, 0x02093a30.
 *
 * The only wrapper that indexes a table: `animIndex = data_ov039_02099aa8[index]`,
 * read as a *signed* halfword (`ldrsh`) from a stack copy of the table. The
 * table is four `s16` at `{0x21, 0x21, 0x21, 0x1d}` -- three equal and one
 * different, so it is an index-to-animation map and not a position list.
 *
 * The horizontal shift is conditional: for index 0..2 the sprite moves by
 * `(2 - index) * 6` pixels, so index 2 stays put and indices 1 and 0 slide left
 * by 6 and 12 -- a three-digit number right-aligned against a fixed column.
 * Index 3 gets no shift at all, which is consistent with the fourth table
 * entry being the odd one out.
 */

// Nonmatching: 97.8%. Two instructions of mwcc scheduling and nothing else: it
// hoists `index * 2` above the two `ldrh`s that copy the table into the stack
// local, where the target computes it after them. The copy, the table read, the
// guarded position shift and the `_Sprite_Load` tail are all correct and in
// order.
void func_ov039_02093a30(OtuBadgeCount* data, Sprite* sprite, s32 index) {
    SpriteAnimation anim = data_ov039_02099b24;
    s16             lut[4];

    lut[0] = data_ov039_02099aa8[0];
    lut[1] = data_ov039_02099aa8[1];
    lut[2] = data_ov039_02099aa8[2];
    lut[3] = data_ov039_02099aa8[3];

    anim.owner     = data;
    anim.dataType  = data->dataType;
    anim.animIndex = lut[index];

    if (index < 3) {
        anim.posX = anim.posX + (2 - index) * 6;
    }

    anim.posY = anim.posY + data->index * 0x13;

    _Sprite_Load(sprite, &anim);
}

/* ==================================================================== */
/* The badge-counter task: Tsk_OtosuGame_badgescounter.                 */
/* ==================================================================== */

/**
 * @brief The counter task's init stage, 0x02093b08.
 *
 * Four sprites are loaded: the pair at +0x04/+0x44, plus the four digit
 * sprites at +0x84..+0x184. `unk_190` from the argument block decides whether
 * the +0x04 sprite is loaded at all -- and the update and cleanup stages both
 * test the same flag before touching it, so the field is set once here and
 * never changes.
 *
 * The digit sprites' visible bitmask at +0x194 is raised to 0xC (bits 2 and 3)
 * here: two of the four digits start showing and two start hidden.
 */
s32 func_ov039_02093b08(TaskPool* pool, Task* self, OtuBadgeCountArgs* args) {
    OtuBadgeCount* data = (OtuBadgeCount*)self->data;
    s32            i;

    data->dataType   = args->dataType;
    data->pinId      = args->pinId;
    data->index      = args->index;
    data->resolved   = 0;
    data->hasSpriteA = args->hasSpriteA;
    data->visible    = 0xC;

    if (data->hasSpriteA != 0) {
        func_ov039_0209392c(data, &data->spriteA);
    }

    func_ov039_020939b0(data, &data->spriteB);

    for (i = 0; i < 4; i++) {
        func_ov039_02093a30(data, &data->digits[i], i);
    }

    return 1;
}

/**
 * @brief The counter task's update stage, 0x02093b98.
 *
 * Asks the task pool for the data behind `sourceId` and records whether it
 * resolved. That is the whole stage: whether the counter has anything to show
 * is decided here, and the render stage 02093bc0 does nothing at all while
 * `resolved` is clear.
 */
s32 func_ov039_02093b98(TaskPool* pool, Task* self, void* arg) {
    OtuBadgeCount* data = (OtuBadgeCount*)self->data;

    data->resolved = EasyTask_GetTaskData(pool, data->pinId) != NULL;
    return 1;
}

/* ==================================================================== */
/* The counter task's render stage and its two score helpers.          */
/* ==================================================================== */

/**
 * @brief The counter task's render stage, 0x02093bc0.
 *
 * Draws the pair at +0x04/+0x44 (the +0x04 one only when `hasSpriteA` says it
 * was loaded) and then the four digit sprites, each gated on its own bit in
 * the `visible` mask at +0x194: bit N is `digits[N]`.
 *
 * `Sprite_Update` is called immediately before `Sprite_RenderFrame` for each
 * sprite rather than once for the lot, which is the engine's per-sprite
 * contract -- these are independent sprites, not one animated object.
 *
 * The whole stage is skipped while `resolved` is clear, so a counter with no
 * source task costs three loads and a return.
 *
 * The mask is reached as `data + 0x100 + 0x94` rather than `data + 0x194`
 * because that is how the target forms the address, and mwcc keeps the split.
 */

s32 func_ov039_02093bc0(TaskPool* pool, Task* self, void* arg) {
    OtuBadgeCount* data = (OtuBadgeCount*)self->data;
    s32            i;

    if (data->resolved != 0) {
        if (data->hasSpriteA != 0) {
            Sprite_Update(&data->spriteA);
            Sprite_RenderFrame(&data->spriteA);
        }

        Sprite_Update(&data->spriteB);
        Sprite_RenderFrame(&data->spriteB);

        for (i = 0; i < 4; i++) {
            if (data->visible & (1 << i)) {
                Sprite_Update(&data->digits[i]);
                Sprite_RenderFrame(&data->digits[i]);
            }
        }
    }

    return 1;
}

/**
 * @brief The counter task's cleanup stage, 0x02093c44.
 */
s32 func_ov039_02093c44(TaskPool* pool, Task* self, void* arg) {
    OtuBadgeCount* data = (OtuBadgeCount*)self->data;
    s32            i;

    if (data->hasSpriteA != 0) {
        Sprite_Release(&data->spriteA);
    }

    Sprite_Release(&data->spriteB);

    for (i = 0; i < 4; i++) {
        Sprite_Release(&data->digits[i]);
    }

    return 1;
}

/** The counter task's stage dispatcher, 0x02093c90. */
s32 func_ov039_02093c90(TaskPool* pool, Task* self, void* arg, s32 stage) {
    TaskStages stages = data_ov039_02099abc;

    return stages.iter[stage](pool, self, arg);
}

/**
 * @brief Creates the counter task, 0x02093cd8.
 *
 * Four arguments, and a 0x18 frame -- between the background task's three and
 * the radar task's five. All three create wrappers are the same body; the
 * argument count and the `TaskHandle` are the only differences.
 */
s32 func_ov039_02093cd8(TaskPool* pool, s32 dataType, s32 pinId, s32 index, s32 hasSpriteA) {
    OtuBadgeCountArgs args;

    args.dataType   = dataType;
    args.pinId      = pinId;
    args.index      = index;
    args.hasSpriteA = hasSpriteA;

    return EasyTask_CreateTask(pool, &data_ov039_02099ab0, NULL, 0, NULL, &args);
}

/**
 * @brief Points the counter at the animation for a free tray slot, 0x02093d18.
 *
 * Scans six `u16` for the 0x130 "empty" sentinel -- the same value
 * func_ov039_020824a0 fills the pin tray with and func_ov039_0208efb0 rejects
 * -- and takes the first index that carries it. That index is folded into an
 * animation number as `unk_188 * 7 + index + 1`, i.e. **seven animations per
 * `unk_188`**, which is the same stride 020939b0 bakes into its own
 * `animIndex` and the only reason to read `unk_188` the same way twice.
 *
 * A caller passing a slot value of 0x130 therefore gets animation
 * `unk_188 * 7 + 1` for that slot, and a full tray gets the last one tried.
 *
 * The whole function is a tail call: the target jumps to `Sprite_ChangeAnimation`
 * rather than calling and returning, so there is no frame of its own.
 */
s32 func_ov039_02093d18(OtuBadgeCount* data, u16* slots) {
    s32 i;

    for (i = 0; i < 6; i++) {
        if (slots[i] == 0x130) {
            break;
        }
    }

    return Sprite_ChangeAnimation(&data->spriteB, data->spriteB.animData, (s16)(data->index * 7 + i + 1),
                                  data->spriteB.cellTable);
}

/**
 * @brief Writes a score into the counter task's digit sprites, 0x02093d68.
 *
 * Two halves, and the order matters:
 *
 *   1. format `value` into up to three glyph indices, least significant first,
 *      stopping as soon as the value runs out -- so `0` costs one iteration,
 *      not three;
 *   2. raise bit 3 of `visible` (the sprite at +0x144, which this function
 *      never touches otherwise and which is therefore always showing), then
 *      walk the three digit sprites from `digits[2]` down to `digits[0]`,
 *      reading the formatted digits back-to-front and raising bits 2, 1 and 0
 *      as it goes.
 *
 * **The glyph base is 0x21, not ASCII '0' (0x30).** The target's `add r9, r9,
 * #0x21` is not a coincidence with the animation indices in
 * `data_ov039_02099aa8` -- which are also 0x21 -- it is the same numbering, and
 * both are offset from ASCII by 0x0F. So this is not "write the characters of
 * the number"; it is "write the number in the overlay's own glyph space",
 * which is the same space the tray's 0x130 empty sentinel lives in. That is
 * the link between the two halves of this file: 02093d18 searches the tray for
 * 0x130, and 02093d68 pushes 0x21..0x2A, so both are working in the pin-tray
 * value space rather than in text.
 *
 * The early `return` inside the second loop is the target's own shape --
 * `addmi sp, sp, #8` / `popmi {...}` -- and it is what stops a one-digit score
 * from writing stale digits into the two sprites it does not use: it returns
 * as soon as the digit index has gone below zero, *after* raising that
 * sprite's bit. So the visible mask ends up describing exactly the digits the
 * score needed, and every sprite's animation is left alone otherwise.
 *
 * The buffer is `s16` and the target stores and reloads it with
 * `strh`/`ldrsh`, so the glyph index is a halfword throughout.
 */

// Nonmatching: 88.9%. The logic is settled -- the divide loop, the break, the
// mask init, the descending sprite walk, the early return and the bit raising
// all match. What differs is register allocation, and it cascades: the target
// spends r7 on the digit count and starts its saved set at r4, where mwcc here
// puts the count in r3 and shifts the whole saved set down by one register. The
// per-iteration address arithmetic follows from that (the target recomputes
// `data + j * 0x40` for the cell table, where this build walks the sprite
// pointer). Nothing about the overlay is in doubt here.
void func_ov039_02093d68(OtuBadgeCount* data, s32 value) {
    s16 buf[4];
    s32 count;
    s32 idx;
    s32 j;

    count = 0;
    for (idx = 0; idx < 3; idx++) {
        buf[idx] = (s16)(0x21 + value % 10);
        count++;
        value /= 10;
        if (value == 0) {
            break;
        }
    }

    data->visible = 8;

    idx = count - 1;
    for (j = 2; j >= 0; j--) {
        Sprite* s = &data->digits[j];

        Sprite_ChangeAnimation(s, s->animData, buf[idx], s->cellTable);
        data->visible |= (1 << j);

        idx--;
        if (idx < 0) {
            return;
        }
    }
}

SpriteFrameInfo* func_ov039_02093e3c(Sprite* sprite, s32 arg, s32 mode) {
    Sprite_FrameInfoCallbackSorted(sprite, mode, 3);
}

/**
 * @brief Loads one of the countdown task's three digit sprites, 0x02093ee4.
 *
 * Positions three sprites across the screen from the index alone:
 * `posX = (0x1F - index * 2) * 8` and a fixed `posY = 0x12`. The three x
 * values are 0xF8, 0xE8, 0xD8 -- eight pixels apart, evenly spaced, which is
 * what a three-digit readout wants. No `animIndex` and no `unk_188`: this task
 * is laid out by count rather than by menu.
 */
void func_ov039_02093ee4(OtuTimer* data, Sprite* sprite, s32 index) {
    SpriteAnimation anim = data_ov039_02099b6c;

    anim.owner    = data;
    anim.dataType = data->dataType;
    anim.posX     = (0x1F - index * 2) * 8;
    anim.posY     = 0x12;

    _Sprite_Load(sprite, &anim);
}

/* ==================================================================== */
/* The countdown task: init, the radar task's update, and the update.  */
/* ==================================================================== */

/**
 * @brief The countdown task's init stage, 0x02093f70.
 *
 * The one argument that matters is `unk_04`, and it is seconds: the target
 * multiplies it by 0x3C to get the countdown stored at +0xC4. So the overlay
 * keeps this clock in 1/60th ticks and 0x4B0 (1200) -- the alarm point
 * 02093fcc compares against -- is exactly twenty of them.
 *
 * `unk_0C8` is raised to 1 here and never written again by anything in this
 * band; the render stage 0209411c reads it as the "draw the digits" gate, so
 * it is set once at construction and the countdown's own state lives at +0xC4
 * and +0xCC.
 */
s32 func_ov039_02093f70(TaskPool* pool, Task* self, OtuTimerArgs* args) {
    OtuTimer* data = (OtuTimer*)self->data;
    s32       i;

    data->visible   = 1;
    data->alarmed   = 0;
    data->dataType  = args->dataType;
    data->countdown = args->seconds * 0x3C;

    for (i = 0; i < 3; i++) {
        func_ov039_02093ee4(data, &data->digits[i], i);
    }

    return 1;
}

/**
 * @brief The countdown task's update stage, 0x02093fcc.
 *
 * Four things, in this order:
 *
 *   1. **Step the clock.** If the countdown is positive, decrement it and, on
 *      the frame it crosses a whole second (`/60`), and while it is at or
 *      below 0x4B0, play SE 0x33E. So the last twenty seconds tick.
 *   2. **Fire the alarm edge.** `alarmed` goes to 1 only on the frame the
 *      countdown passes from 0x4B0 to 0x4AF, and back to 0 on every other
 *      frame. That is a rising edge, not a level -- whatever reads +0xCC (and
 *      something does: `func_ov039_0209420c` is a one-instruction accessor for
 *      that word) sees it high for exactly one frame.
 *   3. **Swap the palette.** On that same edge the three digit sprites get
 *      their `unk3C` (palette-source) pointers pointed at the block the first
 *      sprite's own resource data points into -- `buffer + 0x20 + the word at
 *      buffer + 0x48`, which is the shape 020934e0 uses for the background
 *      task's animated palette. So the digits change colour when the alarm
 *      sounds rather than when they are drawn.
 *   4. **Draw the number.** `countdown / 60` in seconds, split three ways by
 *      repeated `% 10` and `/ 10`, each digit pushed as a `Sprite_ChangeAnimation`
 *      frame of `digit + 1` followed by `Sprite_Update`.
 *
 * The `+ 1` on the frame is not obvious from the code: the digit glyphs are
 * selected by frame index, and the target indexes them from 1. What the +1
 * buys is that frame 0 is free -- presumably for a blank or a minus sign, so a
 * one-digit remainder in the tens place does not show a stale 0.
 *
 * The two magic constants here are worth naming, because they are the only
 * places this file divides and both are non-obvious: `0x88888889` with an
 * `asr #5` is `/60` and `0x66666667` with an `asr #2` is `/10`, both with the
 * sign correction mwcc emits. The `/60` is what makes the countdown a clock
 * (02093f70 stores `args->unk_04 * 0x3C`), and the `/10` is the digit split.
 * The seconds figure is therefore drawn least-significant digit first, into
 * `digits[0]`, `digits[1]`, `digits[2]` in order -- which is why 02093ee4 lays
 * those three sprites out right to left.
 */

// Nonmatching: 88.6%. All four stages are present and correct -- the
// decrement, the `/60` edge test and its sound, the alarm edge, the palette
// swap and the three-digit loop all match. What differs is register allocation
// throughout: the target keeps seven callee-saved registers live (r3..r9) where
// mwcc here needs five (r4..r8), so the base pointer, the magic constant, the
// seconds value and the walked sprite pointer land in different registers
// throughout. One instruction of substance differs: the target forms the null
// palette pointer with `moveq r2, #0` inside the null test, where the `pal =
// NULL` here hoists a `mov r2, #0` above the test. Both are register choices,
// not logic.
s32 func_ov039_02093fcc(TaskPool* pool, Task* self, void* arg) {
    OtuTimer* data = (OtuTimer*)self->data;
    s32       before;
    s32       secs;
    s32       i;

    before = data->countdown;
    if (before > 0) {
        data->countdown = before - 1;

        if (before / 60 != (before - 1) / 60 && before - 1 <= 0x4B0) {
            SndMgr_StartPlayingSE(0x33E);
        }
    }

    if (before >= 0x4B0 && data->countdown < 0x4B0) {
        Data* file = data->digits[0].resourceData;
        void* pal;

        pal = NULL;
        if (file != NULL) {
            pal = OtuPaletteSource(file);
        }

        for (i = 0; i < 3; i++) {
            data->digits[i].unk3C = (UnkSmallInternal*)pal;
        }

        data->alarmed = 1;
    } else {
        data->alarmed = 0;
    }

    secs = data->countdown / 60;
    for (i = 0; i < 3; i++) {
        Sprite* s = &data->digits[i];

        Sprite_ChangeAnimation(s, s->animData, (s16)(secs % 10 + 1), s->cellTable);
        Sprite_Update(s);
        secs /= 10;
    }

    return 1;
}

/** Renders the three sprites at sprite+4, 0x40 apart, when +0xC8 is set. */
s32 func_ov039_0209411c(TaskPool* pool, Task* task, void* args) {
    OtuTimer* data = task->data;
    s32       i;

    if (data->visible != 0) {
        for (i = 0; i < 3; i++) {
            Sprite_RenderFrame(&data->digits[i]);
        }
    }
    return 1;
}

/** Releases the three sprites at sprite+4, 0x40 apart. */
s32 func_ov039_02094158(TaskPool* pool, Task* task, void* args) {
    OtuTimer* data = task->data;
    s32       i;

    for (i = 0; i < 3; i++) {
        Sprite_Release(&data->digits[i]);
    }
    return 1;
}

/**
 * @brief Runs the stage handler at `index` from the table.
 *
 * The table is copied onto the stack first (the `ldm`/`stm` pair) and only then
 * indexed, which is why it is a struct copy rather than four assigns: mwcc
 * emits the copy for the whole struct and a plain `ldr` of the selected entry.
 */
s32 func_ov039_02094188(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = data_ov039_02099b5c;

    return stages.iter[stage](pool, task, args);
}

/** Spawns the task table data_ov039_02099b50 with two words of args. */
s32 func_ov039_020941d0(TaskPool* pool, s32 dataType, s32 seconds) {
    OtuTimerArgs args;

    args.dataType = dataType;
    args.seconds  = seconds;
    return EasyTask_CreateTask(pool, &data_ov039_02099b50, NULL, 0, NULL, &args);
}

/* ------------------------------------------------------------------ */
/* Single words elsewhere.                                             */
/* ------------------------------------------------------------------ */

/** A single word at +0xC4. */
s32 func_ov039_02094204(OtuTimer* self) {
    return self->countdown;
}

/** A single word at +0xCC. */
s32 func_ov039_0209420c(OtuTimer* self) {
    return self->alarmed;
}
