/**
 * @file OtosuGame_badgecount.c
 * @brief The `Tsk_OtosuGame_badgecount` task.
 */

#include "OtuFieldAccessShared.h"

typedef struct {
    s32 dataType;
    s32 pinId;
    s32 index;
    s32 hasSpriteA;
} OtosuGame_badgecount_Args;

extern const SpriteAnimation OtosuGame_badgecount_AnimSpriteB;
extern const SpriteAnimation OtosuGame_badgecount_AnimSpriteA;
extern const SpriteAnimation OtosuGame_badgecount_AnimDigit;

/** Four `s16` offsets: the x positions of the counter task's digit sprites. */
extern const u16        data_ov039_02099aa8[4];
extern const TaskStages data_ov039_02099abc;
extern const TaskHandle Tsk_OtosuGame_badgecount;

SpriteFrameInfo* OtosuGame_badgecount_GetFrameInfo(Sprite* sprite, s32 arg, s32 mode) {
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
void OtosuGame_badgecount_LoadSpriteA(OtosuGame_badgecount* data, Sprite* sprite) {
    SpriteAnimation anim = OtosuGame_badgecount_AnimSpriteA;

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
void OtosuGame_badgecount_LoadSpriteB(OtosuGame_badgecount* data, Sprite* sprite) {
    SpriteAnimation anim = OtosuGame_badgecount_AnimSpriteB;

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
void OtosuGame_badgecount_LoadDigit(OtosuGame_badgecount* data, Sprite* sprite, s32 index) {
    SpriteAnimation anim = OtosuGame_badgecount_AnimDigit;
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

/* The badge-counter task: Tsk_OtosuGame_badgescounter. */

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
s32 OtosuGame_badgecount_Init(TaskPool* pool, Task* self, OtosuGame_badgecount_Args* args) {
    OtosuGame_badgecount* data = (OtosuGame_badgecount*)self->data;
    s32                   i;

    data->dataType   = args->dataType;
    data->pinId      = args->pinId;
    data->index      = args->index;
    data->resolved   = 0;
    data->hasSpriteA = args->hasSpriteA;
    data->visible    = 0xC;

    if (data->hasSpriteA != 0) {
        OtosuGame_badgecount_LoadSpriteA(data, &data->spriteA);
    }

    OtosuGame_badgecount_LoadSpriteB(data, &data->spriteB);

    for (i = 0; i < 4; i++) {
        OtosuGame_badgecount_LoadDigit(data, &data->digits[i], i);
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
s32 OtosuGame_badgecount_Update(TaskPool* pool, Task* self, void* arg) {
    OtosuGame_badgecount* data = (OtosuGame_badgecount*)self->data;

    data->resolved = EasyTask_GetTaskData(pool, data->pinId) != NULL;
    return 1;
}

/* The counter task's render stage and its two score helpers. */

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

s32 OtosuGame_badgecount_Render(TaskPool* pool, Task* self, void* arg) {
    OtosuGame_badgecount* data = (OtosuGame_badgecount*)self->data;
    s32                   i;

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
s32 OtosuGame_badgecount_Destroy(TaskPool* pool, Task* self, void* arg) {
    OtosuGame_badgecount* data = (OtosuGame_badgecount*)self->data;
    s32                   i;

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
s32 OtosuGame_badgecount_RunTask(TaskPool* pool, Task* self, void* arg, s32 stage) {
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
s32 OtosuGame_badgecount_CreateTask(TaskPool* pool, s32 dataType, s32 pinId, s32 index, s32 hasSpriteA) {
    OtosuGame_badgecount_Args args;

    args.dataType   = dataType;
    args.pinId      = pinId;
    args.index      = index;
    args.hasSpriteA = hasSpriteA;

    return EasyTask_CreateTask(pool, &Tsk_OtosuGame_badgecount, NULL, 0, NULL, &args);
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
s32 func_ov039_02093d18(OtosuGame_badgecount* data, u16* slots) {
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
void func_ov039_02093d68(OtosuGame_badgecount* data, s32 value) {
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
