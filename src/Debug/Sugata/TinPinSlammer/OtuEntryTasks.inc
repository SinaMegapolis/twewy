/* Band 3 -- see OtuBands.h for the address range and why this is an include. */

#include "Debug/Sugata/TinPinSlammer.h"
#include "Engine/Core/OamMgr.h"
#include "SndMgr.h"
#include "SpriteMgr.h"

#include <nitro/fx/fx_atan.h>

/**
 * @file OtuEntryTasks.c
 *
 * 0x020950b8 - 0x02095fe4: the four sprite tasks the board stage spawns.
 *
 * The `.rodata` tables a few hundred bytes above this band give the grouping
 * away. Each of the four is a `TaskHandle` (name, entry, data size) followed
 * by a `TaskStages` array of four pointers, and each `TaskStages` array lists
 * exactly four functions in this band or immediately around it:
 *
 *   0x02099cac  "Tsk_OtosuGame_slash"  0x70 bytes, entry 02095420
 *       init 02095144  update 02095194  render 020953c4  destroy 0209540c
 *   0x02099cf4  "Tsk_OtosuGame_track"  0x70 bytes, entry 02095708
 *       init 020955f4  update 02095638  render 020956a4  destroy 020956f0
 *   0x02099d44  "Tsk_OtosuGame_point"  0x174 bytes, entry 02095c58
 *       init 02095a18  update 02095a78  render 02095b6c  destroy 02095c2c
 *   0x02099d8c  "Tsk_OtosuGame_entry"  0xC8 bytes, entry 020960dc
 *       init 02095f18  update 02095f58  render 02095fe4  destroy 020960bc
 *
 * So the four `*_RunTask`-shaped routines are the task entries, and each is
 * byte-identical apart from which `TaskStages` object it copies to the stack.
 * Everything else here is one of the four stages, or a helper only one of them
 * calls -- and each task loads its sprite(s) through its own copy of the
 * shared loader shape, with its own `.rodata` `SpriteAnimation` template.
 *
 * The `data_*` names below are the build's own, and the two that are named
 * rather than numeric (`Tsk_OtosuGame_*` and `Seq_Otosu`) are read off the strings
 * at 0x0209a7e0 onward.
 */

/* ------------------------------------------------------------------ */
/* Declarations this TU does not otherwise have.                      */
/*                                                                     */
/* Everything here lives outside this band and outside OtuFieldAccess's */
/* own bodies, so nothing declares it for us.  They go here rather    */
/* than in the shared header, which five agents share.                 */
/* ------------------------------------------------------------------ */

/* The +0x110/+0x114, +0x118/+0x11C and +0x120/+0x124 pair copy-outs. */
void func_ov039_0208e85c(void* task, OtuPoint* out);
void func_ov039_0208e87c(void* task, OtuPoint* out);

/* The overlay's vector unit's four helpers, 0x02098b8c - 0x02098d10. */
void func_ov039_02098b8c(OtuPoint* a, OtuPoint* b, OtuPoint* out);
s32  func_ov039_02098d10(OtuPoint* v);

/* The pin task's own three queries, 0x0208eed0 - 0x0208ef38. */
s32  func_ov039_0208eed0(void* pin);
void func_ov039_0208ef14(void* pin, OtuPoint* a, OtuPoint* b);
s32  func_ov039_0208ef38(void* pin);

/* The board stage's per-sprite animation stepper, 0x02087bf8. */
void func_ov039_02087bf8(void* anim, void* limit);

/* The base module's fixed-point divide.  No prototype exists in this repo,
 * but the two call sites here both pass a quotient-remainder pair. */
s32 _s32_div_f(s32 a, s32 b);

/* Every stage this band names in a `TaskStages` table.  The tables are written
 * before the stages themselves, so the names have to be in scope first.
 * func_ov039_02095194 and func_ov039_02095a78 are declared but not yet
 * defined here; see the notes on each. */
s32 func_ov039_02095144(TaskPool* pool, Task* task, void* args);
s32 func_ov039_02095194(TaskPool* pool, Task* task, void* args);
s32 func_ov039_020953c4(TaskPool* pool, Task* task, void* args);
s32 func_ov039_0209540c(TaskPool* pool, Task* task, void* args);

s32 func_ov039_020955f4(TaskPool* pool, Task* task, void* args);
s32 func_ov039_02095638(TaskPool* pool, Task* task, void* args);
s32 func_ov039_020956a4(TaskPool* pool, Task* task, void* args);
s32 func_ov039_020956f0(TaskPool* pool, Task* task, void* args);

s32 func_ov039_02095a18(TaskPool* pool, Task* task, void* args);
s32 func_ov039_02095a78(TaskPool* pool, Task* task, void* args);
s32 func_ov039_02095b6c(TaskPool* pool, Task* task, void* args);
s32 func_ov039_02095c2c(TaskPool* pool, Task* task, void* args);

/* The four `SpriteAnimation` templates and the four task descriptors, read by
 * address.  These are in the overlay's gap-filled `.rodata`, so they are
 * declared rather than defined. */
extern TaskHandle data_ov039_02099cac;
extern TaskHandle data_ov039_02099cf4;
extern TaskHandle data_ov039_02099d44;
/* data_ov039_02099d8c is declared const by band 4. */

extern const TaskStages data_ov039_02099cb8;
extern const TaskStages data_ov039_02099d00;
extern const TaskStages data_ov039_02099d50;

extern SpriteAnimation data_ov039_02099cc8;
extern SpriteAnimation data_ov039_02099d10;
extern SpriteAnimation data_ov039_02099d60;
extern SpriteAnimation data_ov039_02099da8;

/* Four `s16` values, 12, 11, 1, 1 -- one per sprite of the point task. */
extern const s16 data_ov039_02099d3c[4];

/* The base module's shared `s16` slope table, indexed by `angle >> 4`. */
extern s32 data_0205e4e0[];

/* ------------------------------------------------------------------ */
/* Task-data shapes.                                                   */
/* ------------------------------------------------------------------ */

/**
 * @brief One of the four slots in "Tsk_OtosuGame_point", 0x14 bytes.
 *
 * A 0x40-stride array of these at +0x120, one per sprite. `live` gates the
 * whole slot; `delay` counts the sprite's own animation down; `offsetX` and
 * `accum` are the two components of the position the render adds to the
 * anchor difference; `phase` is the step the update integrates into `accum`.
 */
typedef struct {
    /* 0x00 */ s32 live;
    /* 0x04 */ s32 delay;
    /* 0x08 */ s32 offsetX;
    /* 0x0C */ s32 accum;
    /* 0x10 */ s32 phase;
} OtuPointSlot; // Size: 0x14

/**
 * @brief "Tsk_OtosuGame_slash", 0x70 bytes.
 *
 * One `Sprite` at the base. +0x48/+0x4C is the pair the loader subtracts, and
 * +0x58/+0x5C the pair it subtracts *it* from -- so the sprite is drawn at the
 * difference between them, which the render recomputes as
 * `(+0x58 - +0x50) >> 12` against the pair the update refreshes.
 */
typedef struct {
    /* 0x00 */ Sprite sprite;
    /* 0x40 */ s32    unk_40;
    /* 0x44 */ s32    unk_44;
    /* 0x48 */ s32    unk_48;
    /* 0x4C */ s16    unk_4C;
    /* 0x4E */ s16    unk_4E;
    /* 0x50 */ s32    unk_50;
    /* 0x54 */ s32    unk_54;
    /* 0x58 */ s32    unk_58;
    /* 0x5C */ s32    unk_5C;
    /* 0x60 */ s32    unk_60;
    /* 0x64 */ s32    unk_64;
    /* 0x68 */ s32    unk_68;
    /* 0x6C */ s32    unk_6C;
} OtuSlashTask; // Size: 0x70

/**
 * @brief "Tsk_OtosuGame_track", 0x70 bytes.
 *
 * Same size as the slash task but the `Sprite` sits at +0x08 rather than at
 * +0, and the subtracted pair is +0x50 against +0x48. The render therefore
 * computes the mirror of the slash task's expression -- `(+0x50 - +0x48) >> 12`
 * where the slash task computes `(+0x58 - +0x50) >> 12` -- which is why the
 * two tasks' loaders differ by which word they load.
 */
typedef struct {
    /* 0x00 */ s32    unk_00;
    /* 0x04 */ s32    unk_04;
    /* 0x08 */ Sprite sprite;
    /* 0x48 */ s32    unk_48;
    /* 0x4C */ s32    unk_4C;
    /* 0x50 */ s32    unk_50;
    /* 0x54 */ s32    unk_54;
    /* 0x58 */ s32    unk_58;
    /* 0x5C */ s32    unk_5C;
    /* 0x60 */ s32    unk_60;
    /* 0x64 */ s32    unk_64;
    /* 0x68 */ s32    unk_68;
    /* 0x6C */ s32    unk_6C;
} OtuTrackTask; // Size: 0x70

/**
 * @brief "Tsk_OtosuGame_point", 0x174 bytes.
 *
 * Four `Sprite`s at 0x40 stride from +0x00 and four `OtuPointSlot`s at +0x120.
 * The anchor pair at +0x100/+0x104 is where the tracked pin is and the one at
 * +0x108/+0x10C is where this task is, both Q12.12; +0x110 holds the pin's
 * task id; +0x114 and +0x118 are the frame counter and the "still live" flag;
 * +0x170 is the counter the update decrements and clamps.
 */
typedef struct {
    /* 0x000 */ Sprite       sprite[4];
    /* 0x100 */ s32          unk_100;
    /* 0x104 */ s32          unk_104;
    /* 0x108 */ s32          unk_108;
    /* 0x10C */ s32          unk_10C;
    /* 0x110 */ s32          unk_110;
    /* 0x114 */ s32          unk_114;
    /* 0x118 */ s32          unk_118;
    /* 0x11C */ s32          unk_11C;
    /* 0x120 */ OtuPointSlot slot[4];
    /* 0x170 */ s32          unk_170;
} OtuPointTask; // Size: 0x174

/**
 * @brief One 0x10-byte animation-state block of the entry task.
 *
 * The two sprites each have one of these as their limit, and each also has a
 * 0x0C-byte running block that 0x02087bf8 writes. Only the scale words at the
 * block's +0x04 and +0x08 are read here -- they are what
 * `OamMgr_AllocAffineGroup` is handed -- but the whole 0x10 is there because
 * the stride is what keeps the two sprites apart.
 */
typedef struct {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 scaleX;
    /* 0x08 */ s32 scaleY;
    /* 0x0C */ s16 unk_0C;
    /* 0x0E */ s16 unk_0E;
} OtuEntryAnim; // Size: 0x10

/**
 * @brief "Tsk_OtosuGame_entry", 0xC8 bytes.
 *
 * Two `Sprite`s at +0x00 and +0x40, each with a 0x10-byte limit block and a
 * 0x0C-byte running block behind it, so the second sprite's animation state
 * begins at +0xA0 and the two running blocks at +0xB0 and +0xBC.
 */
typedef struct {
    /* 0x00 */ Sprite       sprite;
    /* 0x40 */ Sprite       sprite2;
    /* 0x80 */ s32          unk_80;
    /* 0x84 */ s32          unk_84;
    /* 0x88 */ s32          unk_88;
    /* 0x8C */ s32          unk_8C;
    /* 0x90 */ OtuEntryAnim anim0;
    /* 0xA0 */ OtuEntryAnim anim1;
    /* 0xB0 */ u8           run0[0x0C];
    /* 0xBC */ u8           run1[0x0C];
} OtuEntryTask; // Size: 0xC8

/**
 * @brief The block the three task-creating helpers pack and pass by pointer.
 *
 * Which words each one packs is its own business, so there are three types
 * rather than one over-declared union. The track task's middle word is the
 * caller's own pool pointer, which is why its block is three words and the
 * other two are two.
 */
typedef struct {
    s32 unk_00;
    s32 unk_04;
} OtuTaskArgs2;

typedef struct {
    s32       unk_00;
    TaskPool* unk_04;
    s32       unk_08;
} OtuTaskArgs3;

typedef struct {
    s32 unk_00;
} OtuTaskArgs1;

/* ------------------------------------------------------------------ */
/* "Tsk_OtosuGame_slash"                                                   */
/* ------------------------------------------------------------------ */

/** Releases the one sprite the slash task owns, and keeps running. */
s32 func_ov039_0209540c(TaskPool* pool, Task* task, void* args) {
    Sprite_Release(&((OtuSlashTask*)task->data)->sprite);
    return 1;
}

/** The slash task's entry point: dispatches to one of its four stages. */
s32 func_ov039_02095420(TaskPool* pool, Task* task, void* args, s32 stage) {
    const TaskStages stages = data_ov039_02099cb8;
    return stages.iter[stage](pool, task, args);
}

/**
 * @brief Spawns a slash task.
 *
 * The two-word argument block is built in the outgoing-argument area rather
 * than in a frame of its own, which is why the frame is exactly the 0x10 the
 * two outgoing words and the block need together.
 */
s32 func_ov039_02095468(TaskPool* pool, s32 dataType, s32 pin) {
    OtuTaskArgs2 args;

    args.unk_00 = dataType;
    args.unk_04 = pin;

    return EasyTask_CreateTask(pool, &data_ov039_02099cac, NULL, 0, NULL, &args);
}

/**
 * @brief The slash task's sprite loader.
 *
 * Copies the overlay's shared `SpriteAnimation` template for this sprite onto
 * the stack, patches the owner, the `dataType` out of the argument block and
 * the position out of the task's own pair, then loads it. The `bic #0x3C` is
 * mwcc clearing the `dataType` field's four bits before inserting the new
 * value, and the `<< 0x1C` / `>> 0x1A` pair is the insert itself.
 */
void func_ov039_020950b8(OtuSlashTask* self, Sprite* sprite, OtuTaskArgs2* args) {
    SpriteAnimation anim = data_ov039_02099cc8;

    anim.owner    = self;
    anim.dataType = args->unk_00;
    anim.posX     = self->unk_58 >> 12;
    anim.posY     = self->unk_5C >> 12;

    _Sprite_Load(sprite, &anim);
}

/**
 * @brief The slash task's init stage.
 *
 * Records the tracked pin's task id, seeds the pair the loader reads and clears
 * the four state words. +0x44 and +0x48 both start at 0x1000, and the two
 * halfwords at +0x4C/+0x4E are cleared with 16-bit stores, so those two are
 * genuinely narrower than the words around them.
 */
s32 func_ov039_02095144(TaskPool* pool, Task* task, void* args) {
    OtuSlashTask* self = (OtuSlashTask*)task->data;
    OtuTaskArgs2* a    = (OtuTaskArgs2*)args;

    self->unk_60 = a->unk_04;
    self->unk_58 = 0;
    self->unk_5C = 0;
    self->unk_40 = 0;
    self->unk_44 = 0x1000;
    self->unk_48 = 0x1000;
    self->unk_4C = 0;
    self->unk_4E = 0;
    self->unk_64 = 0;
    self->unk_6C = 0;
    self->unk_68 = 0;

    func_ov039_020950b8(self, &self->sprite, a);
    return 1;
}

/**
 * @brief The slash task's render stage.
 *
 * Draws only while +0x64 is set, and puts the sprite at the difference between
 * the pair the update refreshes and the pair the init seeded.
 */
s32 func_ov039_020953c4(TaskPool* pool, Task* task, void* args) {
    OtuSlashTask* self = (OtuSlashTask*)task->data;

    if (self->unk_64 != 0) {
        self->sprite.posX = (self->unk_58 - self->unk_50) >> 12;
        self->sprite.posY = (self->unk_5C - self->unk_54) >> 12;
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

/* ------------------------------------------------------------------ */
/* "Tsk_OtosuGame_track"                                                   */
/* ------------------------------------------------------------------ */

/** Releases the track task's sprite, which sits at +0x08. */
s32 func_ov039_020956f0(TaskPool* pool, Task* task, void* args) {
    Sprite_Release(&((OtuTrackTask*)task->data)->sprite);
    return 1;
}

/** The track task's entry point. */
s32 func_ov039_02095708(TaskPool* pool, Task* task, void* args, s32 stage) {
    const TaskStages stages = data_ov039_02099d00;
    return stages.iter[stage](pool, task, args);
}

/**
 * @brief Spawns a track task.
 *
 * Same shape as the slash spawner but with a three-word argument block whose
 * middle word is the pool itself, so the frame is 0x14 rather than 0x10.
 */
s32 func_ov039_02095750(TaskPool* pool, s32 dataType, s32 pin) {
    OtuTaskArgs3 args;

    args.unk_00 = dataType;
    args.unk_04 = pool;
    args.unk_08 = pin;

    return EasyTask_CreateTask(pool, &data_ov039_02099cf4, NULL, 0, NULL, &args);
}

/** The track task's sprite loader; see func_ov039_020950b8 for the shape. */
void func_ov039_02095558(OtuTrackTask* self, Sprite* sprite, OtuTaskArgs3* args) {
    SpriteAnimation anim = data_ov039_02099d10;

    anim.owner    = self;
    anim.dataType = args->unk_00;
    anim.posX     = (self->unk_50 - self->unk_48) >> 12;
    anim.posY     = (self->unk_54 - self->unk_4C) >> 12;

    _Sprite_Load(sprite, &anim);
}

/** The track task's init stage: records the pin id and pool, clears, loads. */
s32 func_ov039_020955f4(TaskPool* pool, Task* task, void* args) {
    OtuTrackTask* self = (OtuTrackTask*)task->data;
    OtuTaskArgs3* a    = (OtuTaskArgs3*)args;

    self->unk_00 = a->unk_04;
    self->unk_04 = a->unk_08;
    self->unk_60 = 0;
    self->unk_64 = 0;
    self->unk_48 = 0;
    self->unk_4C = 0;
    self->unk_50 = 0;
    self->unk_54 = 0;

    func_ov039_02095558(self, &self->sprite, a);
    return 1;
}

/**
 * @brief The track task's update stage.
 *
 * Only runs while +0x60 is set. When the sprite's animation runs out it clears
 * +0x60 and +0x64 and stops; otherwise it re-reads the tracked pin's position
 * into the task's pair, integrates it, and advances the sprite.
 */
s32 func_ov039_02095638(TaskPool* pool, Task* task, void* args) {
    OtuTrackTask* self = (OtuTrackTask*)task->data;

    if (self->unk_60 != 0) {
        if (SpriteMgr_IsAnimationFinished(&self->sprite)) {
            self->unk_60 = 0;
            self->unk_64 = 0;
        } else {
            OtuPinTask* pin = (OtuPinTask*)EasyTask_GetTaskData(self->unk_00, self->unk_04);

            func_ov039_0208e85c(pin, (OtuPoint*)&self->unk_48);
            func_ov039_02098b8c((OtuPoint*)&self->unk_50, (OtuPoint*)&self->unk_58, (OtuPoint*)&self->unk_50);
            Sprite_Update(&self->sprite);
            self->unk_64 = 1;
        }
    }
    return 1;
}

/** The track task's render stage. */
s32 func_ov039_020956a4(TaskPool* pool, Task* task, void* args) {
    OtuTrackTask* self = (OtuTrackTask*)task->data;

    if (self->unk_64 != 0) {
        self->sprite.posX = (self->unk_50 - self->unk_48) >> 12;
        self->sprite.posY = (self->unk_54 - self->unk_4C) >> 12;
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

/* ------------------------------------------------------------------ */
/* "Tsk_OtosuGame_point"                                                   */
/* ------------------------------------------------------------------ */

/** Releases all four of the point task's sprites, and keeps running. */
s32 func_ov039_02095c2c(TaskPool* pool, Task* task, void* args) {
    OtuPointTask* self = (OtuPointTask*)task->data;
    s32           i;

    for (i = 0; i < 4; i++) {
        Sprite_Release(&self->sprite[i]);
    }
    return 1;
}

/** The point task's entry point. */
s32 func_ov039_02095c58(TaskPool* pool, Task* task, void* args, s32 stage) {
    const TaskStages stages = data_ov039_02099d50;
    return stages.iter[stage](pool, task, args);
}

/** Spawns a point task; the same two-word block as the slash task's. */
s32 func_ov039_02095ca0(TaskPool* pool, s32 dataType, s32 pin) {
    OtuTaskArgs2 args;

    args.unk_00 = dataType;
    args.unk_04 = pin;

    return EasyTask_CreateTask(pool, &data_ov039_02099d44, NULL, 0, NULL, &args);
}

/**
 * @brief The point task's sprite loader, for one of its four sprites.
 *
 * The fourth argument indexes a four-entry `s16` table at 0x02099d3c -- values
 * 12, 11, 1, 1 -- and the selected entry becomes the template's `animIndex`.
 * It is read with `ldrsh`, so the table is signed.
 */
void func_ov039_02095964(OtuPointTask* self, Sprite* sprite, OtuTaskArgs2* args, s32 index) {
    SpriteAnimation anim = data_ov039_02099d60;

    anim.owner     = self;
    anim.dataType  = args->unk_00;
    anim.posX      = (self->unk_108 - self->unk_100) >> 12;
    anim.posY      = (self->unk_10C - self->unk_104) >> 12;
    anim.animIndex = data_ov039_02099d3c[index];

    _Sprite_Load(sprite, &anim);
}

/**
 * @brief The point task's init stage.
 *
 * Records the tracked pin's task id and clears the two anchor pairs, then loads
 * all four sprites. The index walks separately from the sprite pointer so the
 * call passes the loop counter as the table index.
 */
s32 func_ov039_02095a18(TaskPool* pool, Task* task, void* args) {
    OtuPointTask* self = (OtuPointTask*)task->data;
    OtuTaskArgs2* a    = (OtuTaskArgs2*)args;
    s32           i;

    self->unk_110 = a->unk_04;
    self->unk_100 = 0;
    self->unk_104 = 0;
    self->unk_108 = 0;
    self->unk_10C = 0;
    self->unk_114 = 0;
    self->unk_118 = 0;

    for (i = 0; i < 4; i++) {
        func_ov039_02095964(self, &self->sprite[i], a, i);
    }
    return 1;
}

/**
 * @brief The point task's render stage.
 *
 * Reads the tracked pin's two position pairs once, then walks the four slots.
 * A slot draws only once both its live flag and its delay counter agree, and
 * at the anchor difference plus its own two offsets.
 */
s32 func_ov039_02095b6c(TaskPool* pool, Task* task, void* args) {
    OtuPointTask* self = (OtuPointTask*)task->data;
    OtuPinTask*   pin;
    s32           i;

    if (self->unk_118 != 0) {
        pin = (OtuPinTask*)EasyTask_GetTaskData(pool, self->unk_110);

        if (pin != NULL) {
            func_ov039_0208e85c(pin, (OtuPoint*)&self->unk_100);
            func_ov039_0208e6e0(pin, (OtuPoint*)&self->unk_108);

            for (i = 0; i < 4; i++) {
                if (self->slot[i].live != 0 && self->slot[i].delay <= 0) {
                    // Both positions are held in locals and shifted only at the
                    // store, and the y expression is written first: the target
                    // computes that chain before the x one even though it
                    // stores posX first.
                    s32 py = self->unk_10C - self->unk_104 + self->slot[i].accum - 0x10000;
                    s32 px = self->unk_108 - self->unk_100 + self->slot[i].offsetX;

                    self->sprite[i].posX = px >> 12;
                    self->sprite[i].posY = py >> 12;
                    Sprite_RenderFrame(&self->sprite[i]);
                }
            }
        }
    }
    return 1;
}

/**
 * @brief The point task's update stage.
 *
 * Only runs while +0x114 is set, and walks the four slots. A live slot whose
 * delay has run out integrates its phase into its own vertical offset, steps
 * the phase on by 0x800, and once the offset has gone positive clamps it back
 * to zero and divides the phase down so the next lap is slower. The sprite is
 * advanced only on that path, so a slot still counting down holds still.
 *
 * After the walk the task raises its "still live" flag and counts +0x170 down,
 * clearing both +0x114 and +0x118 when it runs out.
 *
 * The phase divide is a Q12 scale by -0.25, not a division: see the note at the
 * assignment. Walking `self->slot[i]` and `self->sprite[i]` directly rather than
 * folding `&self->slot[0]` into a base pointer is what makes mwcc keep `self` as
 * the loop's base register and reach the slots with absolute +0x120.. offsets.
 */
s32 func_ov039_02095a78(TaskPool* pool, Task* task, void* args) {
    OtuPointTask* self = (OtuPointTask*)task->data;
    s32           i;

    if (self->unk_114 != 0) {
        for (i = 0; i < 4; i++) {
            if (self->slot[i].live != 0) {
                if (self->slot[i].delay > 0) {
                    self->slot[i].delay = self->slot[i].delay - 1;
                } else {
                    self->slot[i].accum = self->slot[i].accum + self->slot[i].phase;
                    self->slot[i].phase = self->slot[i].phase + 0x800;

                    if (self->slot[i].accum > 0) {
                        self->slot[i].accum = 0;

                        if (self->slot[i].phase > 0) {
                            // A Q12 scale by -0.25 with the usual 0x800
                            // rounding bias, spelled as an explicit 64-bit
                            // expression. That is what makes mwcc hoist the
                            // magic constants (-1024, -1 and a second zero)
                            // into r4/r5/r11 ahead of the loop and take the
                            // `umull`/`mla`/sign-fold path here; a plain
                            // `/ -4` collapses to three shift instructions.
                            self->slot[i].phase = (s32)(((s64)self->slot[i].phase * -0x400 + 0x800) >> 12);
                        }
                    }
                    Sprite_Update(&self->sprite[i]);
                }
            }
        }

        self->unk_118 = 1;
        self->unk_170 = self->unk_170 - 1;

        if (self->unk_170 <= 0) {
            self->unk_114 = 0;
            self->unk_118 = 0;
        }
    }
    return 1;
}

/**
 * @brief The point task's setup step: seeds all four slots and both sprites.
 *
 * Called with the point task's data rather than a task, so it is not a stage
 * function -- it is the "start bouncing" entry point. Every slot gets its
 * delay staggered by four and its horizontal offset staggered by eight (in
 * screen units, pre-shifted), all four start at a phase of -0x3000, and the
 * frame count is seeded at sixty.
 *
 * The two sprites are the third and fourth of the four the task owns, and
 * their animation index comes from the argument's decimal digits: the low
 * digit picks the first one's animation and the tens digit the second's, with
 * +1 on each because zero is not a valid index. The second sprite is switched
 * off outright when its index comes out at 1 or below.
 *
 * The argument is stored at +0x11C and read back for the second digit, which
 * is why the target reloads it rather than keeping it in a register.
 */
// Nonmatching: 96.9%. Every field, constant, both loop strides and the whole
// loop body match, as do both magic-multiply divides, the early return and the
// second animation narrowing. The single residual is mwcc's scheduling of the
// *first* narrowing: the target keeps the `lsl #0x10` / `asr #0x10` pair
// adjacent and emits it before the call's argument loads, whereas this version
// splits the pair and sinks the `asr` past them. The value, the register (r2) and
// the instruction count are all the same -- only the placement differs. Writing
// the cast inline at the call site instead of through the `first` local makes no
// difference, and neither does dropping the now-unused offset counter.
void func_ov039_02095cd4(OtuPointTask* self, s32 count) {
    s32 i;
    s16 first;
    s16 last;

    self->unk_114 = 1;
    self->unk_170 = 0x3C;

    for (i = 0; i < 4; i++) {
        self->slot[i].live    = 1;
        self->slot[i].delay   = (3 - i) * 4;
        self->slot[i].offsetX = (0xC - i * 8) << 12;
        self->slot[i].accum   = 0;
        self->slot[i].phase   = -0x3000;
    }

    self->unk_11C = count;

    // Both indices are narrowed to s16 *before* they are used, not at the call:
    // the target's `lsl #0x10` / `asr #0x10` pair lands ahead of the argument
    // loads, and for the second one ahead of the `<= 1` compare. Passing the
    // narrowed value on is what reproduces that.
    first = (s16)(count % 10 + 1);
    Sprite_ChangeAnimation(&self->sprite[2], self->sprite[2].animData, first, self->sprite[2].cellTable);

    last = (s16)((self->unk_11C / 10) % 10 + 1);

    if ((s16)last <= 1) {
        self->slot[3].live = 0;
        return;
    }

    Sprite_ChangeAnimation(&self->sprite[3], self->sprite[3].animData, last, self->sprite[3].cellTable);
}

/**
 * @brief The slash task's update stage -- the largest thing in this band.
 *
 * Runs a three-state machine on +0x68 (0 = start, 1 = follow, 2 = settle) and
 * returns 1 either way, so the task is never deleted from here.
 *
 * State 0 either switches to the "follow" animation and falls through into
 * state 1, or gives up and clears the visible flag. State 1 is the real work:
 * ask the pin for its position and velocity, integrate the tracked point
 * through four vector-unit calls, turn the result into an angle, and -- once
 * the sprite's frame finishes -- start the arrow sound and steer its pan and
 * pitch. State 2 eases +0x48 toward 0x200 by a shrinking fraction.
 *
 * The two `case` bodies fall through into each other (0 into 1, and 1 into 2
 * when the pin has gone away), which is why the state is written as a `switch`
 * with real fallthrough rather than as a chain of ifs.
 */
s32 func_ov039_02095194(TaskPool* pool, Task* task, void* args) {
    OtuSlashTask* self = (OtuSlashTask*)task->data;
    OtuPinTask*   pin;
    s32           len;
    s32           pan;

    pin = (OtuPinTask*)EasyTask_GetTaskData(pool, self->unk_60);

    if (pin != NULL) {
        // Declaration order here is load-bearing: mwcc hands out stack slots in
        // reverse, and the target puts the velocity at sp+0x10, the offset at
        // sp+0x8 and the direction at sp+0x0.
        OtuPoint vel;
        OtuPoint off;
        OtuPoint dir;

        s32 phase = func_ov039_0208eed0(pin);

        func_ov039_0208e87c(pin, (OtuPoint*)&self->unk_50);

        switch (self->unk_68) {
            case 0:
                if (phase != 0) {
                    Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, 2, self->sprite.cellTable);
                    self->unk_64 = 1;
                    self->unk_68 = 1;
                    // Falls through into state 1.
                } else {
                    self->unk_64 = 0;
                    break;
                }
                // Falls through.

            case 1:
                if (phase != 0) {
                    func_ov039_0208ef14(pin, &vel, &off);

                    pan = off.x >> 12;

                    func_ov039_02098b8c(&vel, (OtuPoint*)&self->unk_50, &vel);
                    func_ov039_02098b8c(&off, (OtuPoint*)&self->unk_50, &off);
                    func_ov039_02098bb0(&off, &vel, &dir);
                    func_ov039_02098c00(0x800, &dir, &vel, (OtuPoint*)&self->unk_58);

                    len          = func_ov039_02098d10(&dir);
                    self->unk_48 = len / 48;
                    self->unk_40 = (u16)(FX_Atan2Idx(dir.y, dir.x) + 0x4000);

                    if (func_ov039_0208ef38(pin) != 0) {
                        Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, 3, self->sprite.cellTable);
                    }

                    if (SpriteMgr_IsFrameFinished(&self->sprite)) {
                        SndMgr_StartPlayingSE(SEIDX_SE_BAYBADGE_ARROW);

                        if (pan < 0) {
                            pan = 0;
                        } else if (pan > 0xFF) {
                            pan = 0xFF;
                        }

                        SndMgr_UpdateSEPan(SEIDX_SE_BAYBADGE_ARROW, pan);
                        SndMgr_SetSequenceSoundPitch(SEIDX_SE_BAYBADGE_ARROW, ((len >> 12) * 3 * 0x100) / 0x100);
                    }
                    break;
                }

                // The pin has gone: start the settle countdown and fall
                // straight into state 2, which is what the target's layout
                // shows -- there is no branch between these two blocks.
                self->unk_6C = 0x10;
                self->unk_68 = 2;
                // Falls through.

            case 2:
                if (phase == 1) {
                    self->unk_68 = 1;
                    break;
                }

                self->unk_6C = self->unk_6C - 1;

                if (self->unk_6C <= 0) {
                    self->unk_68 = 0;
                } else {
                    self->unk_48 = self->unk_48 + _s32_div_f(0x200 - self->unk_48, self->unk_6C);
                }
                break;
        }
    } else {
        self->unk_64 = 0;
    }

    if (self->unk_64 != 0) {
        Sprite_Update(&self->sprite);
    }
    return 1;
}

/**
 * @brief Points one of the board's arrow slots along a direction.
 *
 * Not a stage function and not one of this task's own: the board's tick calls
 * it with this overlay's task data, its own slot, an angle, a direction sign
 * and a length. It writes a pair of Q12.12 offsets into the task data and then
 * picks the arrow sprite's animation from the length.
 *
 * Both offset pairs come out of the shared `s16` table at 0x0205e4e0, indexed
 * by `angle >> 4` after a direction-dependent bias of +/-0x6000 for the first
 * pair and +/-0x4000 for the second. Each index reads two consecutive entries
 * and scales them differently: by 12 into +0x50/+0x54, and by 0x800 into
 * +0x58/+0x5C. The `+ 1` entry is the x component and the plain entry the y,
 * and the two components are stored in the order x then y.
 *
 * The 0x800 scale is a rounded Q12 halving done in 64 bits: the target
 * sign-extends `scale`, shifts it left by 11 with `asr #0x1f`/`orr .. lsr
 * #0x15`, adds the 0x800 bias with `adds`/`adc`, then shifts the 64-bit result
 * right by 12 as `lsr low, #0xc` / `orr low, low, high, lsl #0x14` and keeps the
 * low word. `>> 12` spelled directly is load-bearing: `/ 0x1000` makes mwcc call
 * `_ll_sdiv` instead.
 *
 * `index` carries the doubled index so both subscripts reduce to one `<< 1`
 * plus an `add #1` off the same base register, which is the shape the target
 * emits (`lsl ip, r0, #1` / `add r0, ip, #1` / `mov r2, r0, lsl #1`).
 *
 * `anim` is `s16` rather than `s32` because the parameter it feeds is `s16`;
 * with an `s32` local mwcc re-sign-extends it (`lsl #0x10` / `asr #0x10`)
 * before the call. Selecting into the local and calling once -- rather than
 * three separate `Sprite_SetAnimation` calls -- is what produces the target's
 * single call site that `blt` jumps forward to.
 *
 * The fifth argument is stack-passed, which is what fixes the argument count.
 */
// Nonmatching: the instruction sequence, operands and structure all agree with
// the target; the residual is purely mwcc's scratch-register choice in the index
// arithmetic (the target parks the index in r12 for the first table read and
// r3 for the second, mine uses r2 for both, and the two loaded halves
// consequently land in a rotated set of registers). Declaration order and the
// order of the table reads were both varied without moving the allocator.
void func_ov039_02095788(OtuTrackTask* self, OtuPointSlot* slot, s32 angle, s32 dir, s32 len) {
    s32 index;
    s16 scaleX;
    s16 scaleY;
    s16 anim;

    self->unk_60 = 1;

    // The `(u16)` cast is load-bearing twice: it is the target's
    // `lsl #0x10 / lsr #0x10` pair, and only an unsigned narrowing followed by
    // an arithmetic shift reproduces the `asr #0x4` on the biased angle.
    index        = ((u16)(angle + ((dir != 0) ? -0x6000 : 0x6000)) >> 4) * 2;
    scaleX       = ((s16*)data_0205e4e0)[index + 1];
    scaleY       = ((s16*)data_0205e4e0)[index];
    self->unk_50 = scaleX * 12;
    self->unk_54 = scaleY * 12;
    func_ov039_02098b8c((OtuPoint*)slot, (OtuPoint*)&self->unk_50, (OtuPoint*)&self->unk_50);

    index        = ((u16)(angle + ((dir != 0) ? -0x4000 : 0x4000)) >> 4) * 2;
    scaleX       = ((s16*)data_0205e4e0)[index + 1];
    scaleY       = ((s16*)data_0205e4e0)[index];
    self->unk_58 = (s32)(((s64)scaleX * 0x800 + 0x800) >> 12);
    self->unk_5C = (s32)(((s64)scaleY * 0x800 + 0x800) >> 12);

    if (len < 0x2000) {
        anim = 3;
    } else if (len > 0x3000) {
        anim = 1;
    } else {
        anim = 2;
    }

    Sprite_SetAnimation(&self->sprite, self->sprite.animData, anim, self->sprite.cellTable);
}

/* ------------------------------------------------------------------ */
/* "Tsk_OtosuGame_entry"                                                   */
/* ------------------------------------------------------------------ */

/**
 * @brief The entry task's sprite loader.
 *
 * The same shape as the other three loaders but with no position patch at
 * all -- it sets the owner and the dataType and nothing else.
 */
void func_ov039_02095ea4(OtuEntryTask* self, Sprite* sprite, OtuTaskArgs1* args) {
    SpriteAnimation anim = data_ov039_02099da8;

    anim.owner    = self;
    anim.dataType = args->unk_00;

    _Sprite_Load(sprite, &anim);
}

/** The entry task's init stage: loads both sprites and seeds the delay. */
s32 func_ov039_02095f18(TaskPool* pool, Task* task, void* args) {
    OtuEntryTask* self = (OtuEntryTask*)task->data;
    OtuTaskArgs1* a    = (OtuTaskArgs1*)args;

    self->unk_84 = 0;

    func_ov039_02095ea4(self, &self->sprite, a);
    func_ov039_02095ea4(self, &self->sprite2, a);

    self->sprite2.posY = 0x91;
    return 1;
}

/**
 * @brief The entry task's update stage.
 *
 * A two-case `switch` on the state word at +0x84, the compare chain emitted
 * before either body. State 0 just clears the visible flag; state 1 steps both
 * sprites' animations, advances the second only when +0x8C says it exists,
 * counts the frame counter down, and either keeps or clears the state.
 */
s32 func_ov039_02095f58(TaskPool* pool, Task* task, void* args) {
    OtuEntryTask* self = (OtuEntryTask*)task->data;

    switch (self->unk_84) {
        case 0:
            self->unk_80 = 0;
            break;

        case 1:
            func_ov039_02087bf8(&self->run0, &self->anim0);
            Sprite_Update(&self->sprite);

            if (self->unk_8C != 0) {
                func_ov039_02087bf8(&self->run1, &self->anim1);
                Sprite_Update(&self->sprite2);
            }

            self->unk_88 = self->unk_88 - 1;

            if (self->unk_88 > 0) {
                self->unk_80 = 1;
            } else {
                self->unk_84 = 0;
                self->unk_80 = 0;
            }
            break;
    }
    return 1;
}

/**
 * @brief The entry task's render stage.
 *
 * Draws the first sprite when +0x80 is set and the second when +0x8C is, and
 * on each path allocates an affine slot whose index goes into bits 5..9 of the
 * sprite's OAM attribute word. The manager is picked from the sprite's own
 * display-engine bits, and the scale comes from that sprite's own limit block.
 *
 * The `(u32)(u16)` around the allocation's return is load-bearing twice over:
 * it is the `<< 0x10` / `>> 0x10` pair the target emits, and it makes the
 * `>> 0x16` a logical rather than an arithmetic shift -- without the u32 cast
 * mwcc emits `asr #0x16` where the target has `lsr`.
 */
// Nonmatching: 72.6%. The body is right -- every field, mask and shift matches,
// and the instruction multiset is identical -- but mwcc schedules the call's
// arguments in the other order. The target builds the manager address
// (`&g_OamMgr[sprite->bits_0_1]`) *before* spilling the fifth argument to
// sp+0; mwcc spills first and computes the address second, on both call sites.
// Five structurally different spellings were tried -- the address hoisted into an
// `OamManager*` local, the call result hoisted into a `u32` local, the shared
// zero between the rotation and flip-flag arguments made an explicit local, the
// two combined, and the plain inline form -- and all five compile to exactly the
// same schedule, so the order is mwcc's argument setup rather than something the
// source asks for. This is the same gap Shop_item2.c already carries for the
// same expression.
s32 func_ov039_02095fe4(TaskPool* pool, Task* task, void* args) {
    OtuEntryTask* self = (OtuEntryTask*)task->data;

    if (self->unk_80 != 0) {
        self->sprite.unk_0A.raw =
            (self->sprite.unk_0A.raw & ~0x3E0) |
            ((u32)(u16)OamMgr_AllocAffineGroup(&g_OamMgr[self->sprite.bits_0_1], 0, self->anim0.scaleX, self->anim0.scaleY, 0)
                 << 0x1B >>
             0x16);
        Sprite_RenderFrame(&self->sprite);

        if (self->unk_8C != 0) {
            self->sprite2.unk_0A.raw = (self->sprite2.unk_0A.raw & ~0x3E0) |
                                       ((u32)(u16)OamMgr_AllocAffineGroup(&g_OamMgr[self->sprite2.bits_0_1], 0,
                                                                          self->anim1.scaleX, self->anim1.scaleY, 0)
                                            << 0x1B >>
                                        0x16);
            Sprite_RenderFrame(&self->sprite2);
        }
    }
    return 1;
}