/**
 * @file OtosuGame_badgeradar.c
 * @brief The `Tsk_OtosuGame_badgeradar` task.
 */

#include "OtuFieldAccessShared.h"

/* The task's creation block. */
typedef struct {
    s32                    dataType;
    s32                    pinId;
    s32                    index;
    struct OtuBoardLayout* board;
    s32                    isFirst;
} OtosuGame_badgeradar_Args;

/* The task's sprite template, stage table and handle. */
extern const SpriteAnimation OtosuGame_badgeradar_Anim;
extern const TaskStages      data_ov039_02099a6c;
extern const TaskHandle      Tsk_OtosuGame_badgeradar;

SpriteFrameInfo* OtosuGame_badgeradar_GetFrameInfo(Sprite* sprite, s32 arg, s32 mode) {
    Sprite_FrameInfoCallbackSorted(sprite, mode, 3);
}

/* The sprite-load wrappers. */

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
void OtosuGame_badgeradar_Load(OtosuGame_badgeradar* data, Sprite* sprite) {
    SpriteAnimation anim = OtosuGame_badgeradar_Anim;

    anim.owner     = data;
    anim.dataType  = data->dataType;
    anim.animIndex = (data->isFirst == 1) ? 5 : 1;
    anim.animIndex = anim.animIndex + (s16)data->index;

    _Sprite_Load(sprite, &anim);
}

/* The badge-radar task: Tsk_OtosuGame_badgeradar. */

/**
 * @brief The radar task's init stage, 0x02093668.
 *
 * Copies the five-word argument block in, clears the Q12.12 position and the
 * "linked" flag, then loads the sprite. The sprite's `owner` is the task data
 * block, which is how the cell builder handed to `_Sprite_Load` finds its way
 * back to +0x18/+0x1C/+0x16 when it is called per frame.
 */
s32 OtosuGame_badgeradar_Init(TaskPool* pool, Task* self, OtosuGame_badgeradar_Args* args) {
    OtosuGame_badgeradar* data = (OtosuGame_badgeradar*)self->data;

    data->dataType = args->dataType;
    data->pinId    = args->pinId;
    data->index    = args->index;
    data->board    = args->board;
    data->linked   = 0;
    data->isFirst  = args->isFirst;
    data->x        = 0;
    data->y        = 0;

    OtosuGame_badgeradar_Load(data, &data->sprite);
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
 * a bare pointer in the init stage and nothing here writes it.
 */

// Nonmatching: 91.3%. Everything through the position arithmetic matches
// instruction for instruction, including both sign-corrected divides and the
// redundant `linked` store. The remaining gap is where the null-child block
// sits: the target branches past it to a single out-of-line clear at the bottom
// of the function (`beq` forward, then `mov r0, #0; str r0, [r4, #0x58]` after
// the `Sprite_Update` block), where the `else if` chain here puts the same
// store inline ahead of the branch. Same work, different block placement; it
// was not worth a `goto` to chase two instructions.
s32 OtosuGame_badgeradar_Update(TaskPool* pool, Task* self, void* arg) {
    OtosuGame_badgeradar* data  = (OtosuGame_badgeradar*)self->data;
    OtosuGame_badge*      child = (OtosuGame_badge*)EasyTask_GetTaskData(pool, data->pinId);
    OtuPoint              pt;
    s32                   skew;

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
s32 OtosuGame_badgeradar_Render(TaskPool* pool, Task* self, void* arg) {
    OtosuGame_badgeradar* data = (OtosuGame_badgeradar*)self->data;

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
s32 OtosuGame_badgeradar_Destroy(TaskPool* pool, Task* self, void* arg) {
    OtosuGame_badgeradar* data = (OtosuGame_badgeradar*)self->data;

    Sprite_Release(&data->sprite);
    return 1;
}

/** The radar task's stage dispatcher, 0x020937f4. */
s32 OtosuGame_badgeradar_RunTask(TaskPool* pool, Task* self, void* arg, s32 stage) {
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
s32 OtosuGame_badgeradar_CreateTask(TaskPool* pool, s32 dataType, s32 pinId, s32 index, OtuBoardLayout* board, s32 isFirst) {
    OtosuGame_badgeradar_Args args;

    args.dataType = dataType;
    args.pinId    = pinId;
    args.index    = index;
    args.board    = board;
    args.isFirst  = isFirst;

    return EasyTask_CreateTask(pool, &Tsk_OtosuGame_badgeradar, NULL, 0, NULL, &args);
}
