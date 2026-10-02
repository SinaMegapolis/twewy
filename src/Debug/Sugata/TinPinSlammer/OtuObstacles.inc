/* Band 6 -- see OtuBands.h for the address range and why this is an include. */

#include "Debug/Sugata/TinPinSlammer.h"
#include "Engine/EasyTask.h"
#include "SpriteMgr.h"

/* ============================================================================
 * Band 6: Tsk_OtosuGame_obstacle
 *
 * One of the overlay's obstacle sprites. Every stage callback returns 1, and
 * the state block begins with a Sprite: Sprite_Update, Sprite_RenderFrame and
 * Sprite_Release are all handed `task->data` unchanged, so the task state *is*
 * the sprite rather than holding a pointer to one.
 *
 * The sprite is stationary. Init takes a target position in 12-bit fixed point
 * and Render recomputes the screen position from it every frame; Update spends
 * exactly one frame swapping the animation to its looping variant. The only
 * motion is the whole group being scaled, which is what `scale` is for.
 *
 * Placeholder names, so objdiff can pair these against the target. The roles
 * they stand for, in the order the stage table at 0x02099948 holds them:
 *
 *   func_ov039_020926a8  RunTask    (also the handle's taskFunc)
 *   func_ov039_0209258c  Init
 *   func_ov039_02092608  Update
 *   func_ov039_02092658  Render
 *   func_ov039_02092694  Destroy
 *   func_ov039_02092484  Load
 *   func_ov039_020926f0  CreateTask
 *
 * The roles are not guesses -- the target's own stage table holds these four
 * callback addresses in order, and the handle's taskFunc field is the
 * dispatcher. Renaming them is a separate pass: it means editing symbols.txt
 * for USA *and* JP, and a rename that misses either region breaks pairing.
 * ==========================================================================*/

/** The caller's per-obstacle numbers, reached through OtuObstacle_Args.params. */
typedef struct {
    /* 0x00 */ u16 targetX; // <<12 into OtuObstacle.targetX
    /* 0x02 */ u16 targetY; // <<12 into OtuObstacle.targetY
    /* 0x04 */ u16 kind;    // 0..2; picks a row of each sprite table below
} OtuObstacle_Params; // Size: 0x6

typedef struct {
    /* 0x00 */ Sprite sprite; // Size: 0x40. Passed straight to the sprite API.
    /* 0x40 */ s32 originX;   // Screen position at spawn; the fixed-point target
    /* 0x44 */ s32 originY;   // is measured relative to it.
    /* 0x48 */ s32 targetX;   // Fixed point, <<12.
    /* 0x4C */ s32 targetY;   // Fixed point, <<12.
    /* 0x50 */ s32 scale;     // 16.16 scale factor, picked by kind.
    /* 0x54 */ s32 animPending; // One-shot: set at spawn, cleared by the first Update.
} OtuObstacle; // Size: 0x58

typedef struct {
    /* 0x00 */ s32 oamAttrs; // ORed into the template's OAM word, shifted to bit 6.
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 slot;      // Which of the three palette slots to use.
    /* 0x08 */ OtuObstacle_Params* params;
} OtuObstacle_Args; // Size: 0xC

/** The handle, the stage table and the sprite template are all already in the
 *  overlay's .rodata, so they are referenced rather than redefined. The name
 *  string inside the handle is the ground truth for what this task is. */
extern const TaskHandle data_ov039_02099930; // "Tsk_OtosuGame_obstacle", size 0x58

/** The sprite template. Every field Load does not patch is already correct here,
 *  and the five it does patch all hold their kind-0 values, so this is literally
 *  the index-0 case that the table lookups below then re-derive. */
extern const SpriteAnimation data_ov039_0209996c;

/* Five tables of three, indexed by OtuObstacle_Params.kind. They are separate
 * arrays rather than one array of a struct because the target loads each base
 * address into its own register and indexes them independently. */
extern const s16 data_ov039_02099918[3]; // -> params.unk_26
extern const s16 data_ov039_0209991e[3]; // -> params.unk_1C
extern const s16 data_ov039_02099924[3]; // -> params.unk_28
extern const s16 data_ov039_0209992a[3]; // -> params.packIndex

/** Palette slots. A row per kind, a column per OtuObstacle_Args.slot; all three
 *  rows currently hold the same {4, 5, 6}, so kind does not yet change colour. */
extern const s16 data_ov039_02099958[3][3]; // -> params.unk_20

/** The per-kind 16.16 scale factors, {0x20000, 0x18000, 0x10000}. Init copies
 *  these to the stack and indexes the copy. */
typedef struct {
    s32 v[3];
} OtuObstacle_Scales;
extern const OtuObstacle_Scales data_ov039_0209993c;

/**
 * Fill a SpriteAnimation for this obstacle from the shared template.
 *
 * The template is copied whole and then six fields are overwritten, so nothing
 * here depends on the template's current contents except the OAM word, which is
 * masked rather than replaced.
 */
void func_ov039_02092484(OtuObstacle* self, Sprite* sprite, OtuObstacle_Args* args) {
    SpriteAnimation params = data_ov039_0209996c;
    u16 kind = args->params->kind;

    /* Bits 0-1 and 6-15 of the OAM word survive; 2-5 are cleared and rebuilt
     * from the caller's value, which needs a shift of 2 to line up. The target
     * spells it `lsl #0x1c` then `orr ..., lsr #0x1a`, a net shift of 2 -- and
     * because it truncates the caller to 16 bits first, only the caller's low
     * four bits can reach bits 2-5.
     *
     * Written as one expression on purpose. The target reads the word out of the
     * stack frame as the first instruction after the template copy and stores it
     * back twenty-odd instructions later; splitting the read into a named local
     * to try to schedule it there produced byte-identical code, and 720
     * permutations of the statement order could not reproduce it either. mwcc
     * hoists the `args->params` and `args->oamAttrs` loads above the stack read
     * regardless of how the source is written. */
    *(u16*)&params = (u16)(*(u16*)&params & ~0x3C) | (u16)((u16)args->oamAttrs << 2);
    params.owner = self;
    params.packIndex = data_ov039_0209992a[kind];
    params.unk_20 = data_ov039_02099958[kind][args->slot];
    params.unk_26 = data_ov039_02099918[kind];
    params.unk_1C = data_ov039_0209991e[kind];
    params.unk_28 = data_ov039_02099924[kind];
    params.posX = (s16)((self->targetX - self->originX) >> 12);
    params.posY = (s16)((self->targetY - self->originY) >> 12);
    _Sprite_Load(sprite, &params);
}

/** Spawn-time setup. Nothing moves on this frame; the first Render does. */
s32 func_ov039_0209258c(TaskPool* pool, Task* task, void* args) {
    OtuObstacle* self = (OtuObstacle*)task->data;
    OtuObstacle_Args* a = (OtuObstacle_Args*)args;
    /* The 16.16 scale per kind, copied from the overlay's data rather than spelled
     * out here: the target's literal pool names data_ov039_0209993c for this load,
     * where a brace-initialised local would name a compiler-generated template
     * instead. Same ldm/stm copy either way, different literal.
     *
     * A struct wrapper, not an array: `s32 x[3] = someArray;` is an illegal
     * initialisation, but copying one struct from another is exactly the
     * 12-byte ldm/stm the target performs. */
    OtuObstacle_Scales scales = data_ov039_0209993c;

    self->originX = 0;
    self->originY = 0;
    self->targetX = (s32)a->params->targetX << 12;
    self->targetY = (s32)a->params->targetY << 12;
    self->scale = scales.v[a->params->kind];
    self->animPending = 0;

    func_ov039_02092484(self, &self->sprite, a);
    return 1;
}

/** One frame of work: swap to the looping animation, then advance the sprite. */
s32 func_ov039_02092608(TaskPool* pool, Task* task, void* args) {
    OtuObstacle* self = (OtuObstacle*)task->data;

    if (self->animPending != 0) {
        /* Bits 5-6 of the sprite's first word are animationMode; this selects
         * mode 2. */
        *(u32*)&self->sprite = (*(u32*)&self->sprite & ~0x60) | 0x40;
        /* animFrame is the literal 2 and cellTable comes last. Passing them the
         * other way round compiles -- the cell table pointer truncates to an
         * s16 frame without complaint -- and is wrong. */
        Sprite_SetAnimation(&self->sprite, self->sprite.animData, 2, self->sprite.cellTable);
        self->animPending = 0;
    }

    Sprite_Update(&self->sprite);
    return 1;
}

/** The sprite is stationary, so this only refreshes its position from the
 *  fixed-point target. It runs every frame rather than on movement, because the
 *  target can be rewritten while the sprite sits still. */
s32 func_ov039_02092658(TaskPool* pool, Task* task, void* args) {
    OtuObstacle* self = (OtuObstacle*)task->data;

    self->sprite.posX = (s16)((self->targetX - self->originX) >> 12);
    self->sprite.posY = (s16)((self->targetY - self->originY) >> 12);

    Sprite_RenderFrame(&self->sprite);
    return 1;
}

s32 func_ov039_02092694(TaskPool* pool, Task* task, void* args) {
    OtuObstacle* self = (OtuObstacle*)task->data;

    Sprite_Release(&self->sprite);
    return 1;
}

s32 func_ov039_020926a8(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = func_ov039_0209258c,
        .update = func_ov039_02092608,
        .render = func_ov039_02092658,
        .cleanup = func_ov039_02092694,
    };
    return stages.iter[stage](pool, task, args);
}

s32 func_ov039_020926f0(TaskPool* pool, s32 oamAttrs, s16 unk_04, s16 slot, OtuObstacle_Params* params) {
    OtuObstacle_Args args;

    args.oamAttrs = oamAttrs;
    args.unk_04 = unk_04;
    args.slot = slot;
    args.params = params;

    return EasyTask_CreateTask(pool, &data_ov039_02099930, NULL, 0, NULL, &args);
}
