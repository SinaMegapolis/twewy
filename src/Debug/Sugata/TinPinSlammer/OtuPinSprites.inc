/* Band 1 -- see OtuBands.h for the address range and why this is an include. */

#include "Debug/Sugata/TinPinSlammer.h"
#include "Engine/EasyTask.h"
#include "Engine/File/BinMgr.h"
#include "SpriteMgr.h"
#include "common_data.h"

/**
 * @file OtuPinSprites.c
 *
 * Band 1 of the overlay: 0x0208f000 - 0x02090000.
 *
 * This band is four sibling sprite tasks and the handful of small helpers that
 * surround them. The four tasks are near-copies of each other and the target
 * says so explicitly: each one is a TaskHandle plus a four-entry TaskStages
 * table plus a 0x2C-byte SpriteAnimation template, and the templates' only
 * differences are a resource id and the animation frame count. The task names
 * are still in the ROM, at 0x0209a4f4 onwards:
 *
 *   0x0209930c  Tsk_OtosuGame_shadow   Init 0208f2a4  Update 0208f2f0  Render 0208f360  Release 0208f3b0
 *   0x02099354  Tsk_OtosuGame_piyo     Init 0208f5a8  Update 0208f5e0  Render 0208f6cc  Release 0208f714
 *   0x0209939c  Tsk_OtosuGame_marker   Init 0208f900  Update 0208f938  Render 0208f9b8  Release 0208fa00
 *   0x020993e4  Tsk_OtosuGame_meteo    Init 0208fbe0  Update 0208fc38  Render 0208fd8c  Release 0208fddc
 *
 * So the recurring constants across the four are resource ids (the SpriteAnimation
 * templates at 0x02099328/0x2099370/0x020993b8/0x02099400), not frame counts.
 * All four templates carry the same 0x0050 initial x/y and the same callback
 * slot, and differ only in `packIndex` (1, 2, 8, 8) and the anim/cell indices --
 * which is exactly the set a pinball table needs: one shadow, one "piyo"
 * (Japanese for the springy part), one marker and one weather effect.
 */

/* The overlay's four sprite-task resources. Unidentified: keep the names. */
extern const SpriteAnimation data_ov039_02099328;
extern const SpriteAnimation data_ov039_02099370;
extern const SpriteAnimation data_ov039_020993b8;
extern const SpriteAnimation data_ov039_02099400;

/* The four task handles and their TaskStages tables. */
extern const TaskHandle data_ov039_0209930c;
extern const TaskHandle data_ov039_02099354;
extern const TaskHandle data_ov039_0209939c;
extern const TaskHandle data_ov039_020993e4;
extern const TaskStages data_ov039_02099318;
extern const TaskStages data_ov039_02099360;
extern const TaskStages data_ov039_020993a8;
extern const TaskStages data_ov039_020993f0;

/* A 0x24-byte table passed to func_ov039_02087ba0 by the state-3 setup. */
extern const s32 data_ov039_0209a54c[9];

/* ------------------------------------------------------------------ */
/* Sibling accessors this band calls.                                  */
/*                                                                      */
/* The five defined in OtuFieldAccess.c are declared with the same     */
/* `void*` first parameter they are defined with, because OtuPinSprites.c  */
/* is #included into that file and mwcc rejects a mismatched redeclaration. */
/* ------------------------------------------------------------------ */

/* Defined in OtuFieldAccess.c below the band include; declared here so mwcc
 * does not fall back to an implicit int() prototype and then reject the real
 * definition. */
void func_ov039_02096270(void* task);
void func_ov039_02095ddc(void* task);
s32  func_ov039_0208e6f4(void* task);
s32  func_ov039_0208ee98(void* task);

s32  func_ov039_0208e890(void* task);
s32  func_ov039_0208e8c4(void* task);
s32  func_ov039_0208e950(void* task);
s32  func_ov039_0208a490(void* task, s32 arg2);


/* ==================================================================== */
/* The band-level helpers: 0208f00c - 0208f104.                        */
/* ==================================================================== */

/**
 * Reads the tray-slot pointer at +0x16C and asks whether the value it points
 * at is below the "no pin" sentinel 0x130.
 *
 * That is the same test as func_ov039_0208efb0 and the mirror image of
 * func_ov039_0208f0b0: 0x130 means the slot is empty, so `<` means "there is a
 * real pin here". It is the overlay's only predicate on a pin's identity that
 * is not a `kind` comparison.
 */
s32 func_ov039_0208f00c(void* task) {
    return *(u16*)(*(u8**)((u8*)task + 0x16C)) < 0x130;
}

/**
 * Tail-call thunk: forwards to func_ov039_0208a490 with a literal second
 * argument of 10.
 *
 * The target is `ldr ip, [pool]; mov r1, #0xa; bx ip` -- a three-instruction
 * thunk, not a body of its own. Written as a plain call so mwcc emits the same
 * shape.
 */
void func_ov039_0208f024(void* arg) {
    func_ov039_0208a490(arg, 0xA);
}

/**
 * Round-robins a slot index at +0x1C8 through the eight words at +0x238, handing
 * each slot to func_ov039_02097750 with two caller-supplied values and two
 * constants (0x1000 and 0x66/0x6).
 *
 * The `cmp #8 / movge #0 / strge` at the end is the wrap: the index is written
 * unconditionally first and then overwritten to zero only when it reached 8, so
 * the modulus is a post-store clamp rather than a pre-store one.
 */
void func_ov039_0208f048(void* task, s32 arg1, s32 arg2) {
    // No local for the slot index: the target reloads +0x1C8 from memory on both
    // sides of the call rather than keeping it in a callee-saved register, which
    // is what a local forces.
    func_ov039_02097750(
        EasyTask_GetTaskData(*(void**)((u8*)task + 8), *(s32*)((u8*)task + *(s32*)((u8*)task + 0x1C8) * 4 + 0x238)), arg1,
        arg2, 0x1000, 0x66, 6);

    *(s32*)((u8*)task + 0x1C8) = *(s32*)((u8*)task + 0x1C8) + 1;
    if (*(s32*)((u8*)task + 0x1C8) >= 8) {
        *(s32*)((u8*)task + 0x1C8) = 0;
    }
}

/* ==================================================================== */
/* The four _Sprite_Load wrappers: 0208f1f8, 0208f4fc, 0208f874,        */
/* 0208fb54.  One body, four times, three constants varied.             */
/* ==================================================================== */

/**
 * @brief The pin-shadow task's data block, 0x68 bytes.
 *
 * `Sprite` first, because _Sprite_Load is handed `self` itself (the sprite lives
 * at offset 0) and the two point pairs follow the two coordinate words the
 * wrapper writes.
 *
 * This is the only one of the four with a point pair at +0x50 and another at
 * +0x58 *and* the `+3` offset in its render step, which is what distinguishes
 * the shadow's placement from the other three.
 */
/*
 * One struct for all four tasks, deliberately minimal.
 *
 * The four tasks use the same word range for *different* things, and the
 * layouts genuinely overlap rather than sitting side by side:
 *
 *   +0x40/+0x44   piyo, marker   -- the pin's +0x110/+0x114 pair
 *   +0x48/+0x4C   piyo, marker   -- the pin's +0x120/+0x124 pair
 *   +0x44/+0x48   shadow, meteo  -- a Q12 scale pair, seeded to 0x1000
 *   +0x50/+0x54   shadow, meteo  -- the pin's +0x110/+0x114 pair
 *   +0x58/+0x5C   shadow, meteo  -- the pin's +0x120/+0x124 pair
 *
 * So +0x44 is a point's y on the piyo and a scale on the shadow, and naming both
 * readings would mean a union whose members mwcc then lays out a word apart from
 * where the target has them. Only the words the code reaches *unambiguously*
 * across all four tasks are named; the overlapping ones are read by raw offset at
 * the use site, which is also how OtuFieldAccess.c models this overlay's
 * structures.
 */
typedef struct {
    /* 0x00 */ Sprite   sprite;
    /* 0x40 */ s32      unk_40;
    /* 0x44 */ s32      unk_44;
    /* 0x48 */ s32      unk_48;
    /* 0x4C */ u16      unk_4C;
    /* 0x4E */ u16      unk_4E;
    /* 0x50 */ OtuPoint pinPosA_hi;   // shadow/meteo: the pin's +0x110/+0x114 pair
    /* 0x58 */ OtuPoint pinPosB_hi;   // shadow/meteo: the pin's +0x120/+0x124 pair
    /* 0x60 */ s32      childId;      // shadow: the pin id it tracks
    /* 0x64 */ s32      meteoChildId; // meteo: its pin id sits one word higher
    /* 0x68 */ s32      alive;        // meteo only: the kind == 8 filter's result
    /* 0x6C */ s32      state;        // meteo only: 0-3, the sub-state machine
    /* 0x70 */ s32      timer;        // meteo only: the state machine's work counter
} OtuShadowTask;                      // Size: 0x74

/* The piyo and marker keep their two pin pairs at +0x40 and +0x48, where the
 * shadow and meteo keep a scale pair. Reached by offset rather than by member. */
#define OTU_PIN_POS_A(task) ((OtuPoint*)((u8*)(task) + 0x40))
#define OTU_PIN_POS_B(task) ((OtuPoint*)((u8*)(task) + 0x48))

/*
 * +0x60 is deliberately shared. The shadow and the piyo keep their pin id there;
 * the meteo instead keeps a y velocity at +0x60 and puts its own id at +0x64, so
 * its render step can read the velocity and its update step the id without
 * either colliding with the other two. That overlap is why `meteoChildId` is a
 * separate member rather than a second reading of `childId`: naming one word
 * twice forces a union, and mwcc then lays the later members out a word past
 * where the target puts them.
 */

/**
 * Loads the shadow sprite: copies its SpriteAnimation template onto the stack,
 * points it at its owner, sets the dataType nibble from the argument struct and
 * the initial x/y from the task's own +0x58/+0x5C, then lowers the OAM priority
 * to 10.
 *
 * `data_0206a890.unk_0C` is the OAM priority/slot word (see Engine/Core/OamMgr.c,
 * where it is masked with 0x1F); the only one of these four wrappers that writes
 * it, and the reason the shadow sits behind everything else on its screen.
 *
 * The statement order is load-bearing. The target's `ldrh [sp,#2] / bic / orr`
 * pair sits *after* both position stores, and moving the halfword edit between
 * them is a real 77%-vs-100% difference rather than cosmetic reordering.
 */
void func_ov039_0208f1f8(OtuShadowTask* self, Sprite* sprite, s32* args) {
    SpriteAnimation anim = data_ov039_02099328;

    anim.owner           = self;
    anim.dataType        = *args;
    anim.posX            = F2I(*(s32*)((u8*)self + 0x58));
    anim.posY            = F2I(*(s32*)((u8*)self + 0x5C));
    anim.unk_02.unk_02   = 1;
    data_0206a890.unk_0C = 0xA;

    _Sprite_Load(sprite, &anim);
}

/**
 * Loads the "piyo" sprite.
 *
 * Same body as the shadow wrapper except for three things: the position comes
 * from the *difference* of two task words (+0x48 - +0x40, +0x4C - +0x44) rather
 * than from a single one, the flags halfword's 3-bit field at bits 7-9 is forced
 * to 5, and the OAM priority is left alone. So the piyo is positioned relative to
 * a previous position stored on the task, while the shadow is positioned
 * absolutely.
 *
 * The flags edit also goes after both position stores, as in the shadow wrapper.
 */
void func_ov039_0208f4fc(OtuShadowTask* self, Sprite* sprite, s32* args) {
    SpriteAnimation anim = data_ov039_02099370;

    anim.owner    = self;
    anim.dataType = *args;
    anim.posX     = F2I(*(s32*)((u8*)self + 0x48) - *(s32*)((u8*)self + 0x40));
    anim.posY     = F2I(*(s32*)((u8*)self + 0x4C) - *(s32*)((u8*)self + 0x44));
    anim.bits_7_9 = 5;

    _Sprite_Load(sprite, &anim);
}

/** Loads the marker sprite: absolute position from +0x48/+0x4C, nothing forced. */
void func_ov039_0208f874(OtuShadowTask* self, Sprite* sprite, s32* args) {
    SpriteAnimation anim = data_ov039_020993b8;

    anim.owner    = self;
    anim.dataType = *args;
    anim.posX     = F2I(*(s32*)((u8*)self + 0x48));
    anim.posY     = F2I(*(s32*)((u8*)self + 0x4C));

    _Sprite_Load(sprite, &anim);
}

/* ==================================================================== */
/* The four render steps and the release steps.                          */
/* ==================================================================== */

/*
 * Every lifecycle function in this band is a TaskStages callback, so it takes
 * the full `(TaskPool*, Task*, void*)` triple and reaches its data through
 * `task->data` -- which is `Task`'s own +0x18 field, and is the sprite block's
 * base. Getting that signature wrong costs a whole register: with only
 * `(Task*)` mwcc loads `[r0, #0x18]` (reading the field off the wrong
 * register) instead of `[r1, #0x18]`, which is 99% rather than a match on
 * every function here.
 */

/* Release step, one per task. Identical bodies -- they differ only in which
 * TaskHandle/TaskStages pair points at them. */

/** Releases the shadow sprite. */
s32 func_ov039_0208f3b0(TaskPool* pool, Task* task, void* args) {
    Sprite_Release((Sprite*)((OtuShadowTask*)task->data));
    return 1;
}

/** Releases the piyo sprite. */
s32 func_ov039_0208f714(TaskPool* pool, Task* task, void* args) {
    Sprite_Release((Sprite*)((OtuShadowTask*)task->data));
    return 1;
}

/** Releases the marker sprite. */
s32 func_ov039_0208fa00(TaskPool* pool, Task* task, void* args) {
    Sprite_Release((Sprite*)((OtuShadowTask*)task->data));
    return 1;
}

/**
 * Renders the shadow, offset by three pixels.
 *
 * The only one of the four with a `+ 3` on both axes, and the only one that
 * reads +0x58/+0x5C rather than +0x48/+0x4C. The `+3` is almost certainly a
 * deliberate nudge: a drop shadow offset down-and-right by three pixels is the
 * cheapest way to fake depth on a 2D pinball table, and it is consistent with
 * this being the only task that also drops the OAM priority.
 */
s32 func_ov039_0208f360(TaskPool* pool, Task* task, void* args) {
    OtuShadowTask* self = (OtuShadowTask*)task->data;

    if (*(s32*)((u8*)self + 0x64) != 0) {
        self->sprite.posX = F2I(*(s32*)((u8*)self + 0x58) - self->pinPosA_hi.x) + 3;
        self->sprite.posY = F2I(*(s32*)((u8*)self + 0x5C) - *(s32*)((u8*)self + 0x54)) + 3;
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

/**
 * Renders the piyo: absolute offset from the stored base pair, no nudge.
 *
 * Guarded on +0x54, the word the piyo's update step raises when it has decided
 * the sprite is worth drawing at all.
 */
s32 func_ov039_0208f6cc(TaskPool* pool, Task* task, void* args) {
    OtuShadowTask* self = (OtuShadowTask*)task->data;

    if (*(s32*)((u8*)self + 0x54) != 0) {
        self->sprite.posX = F2I(*(s32*)((u8*)self + 0x48) - self->unk_40);
        self->sprite.posY = F2I(*(s32*)((u8*)self + 0x4C) - self->unk_44);
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

/** Renders the marker. Byte-for-byte the same body as the piyo's render step. */
s32 func_ov039_0208f9b8(TaskPool* pool, Task* task, void* args) {
    OtuShadowTask* self = (OtuShadowTask*)task->data;

    if (*(s32*)((u8*)self + 0x54) != 0) {
        self->sprite.posX = F2I(*(s32*)((u8*)self + 0x48) - self->unk_40);
        self->sprite.posY = F2I(*(s32*)((u8*)self + 0x4C) - self->unk_44);
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

/**
 * Renders the meteo.
 *
 * The only one of the four that adds a third term: y is
 * `(unk_60 + unk_5C - unk_54) >> 12`, where +0x60 is the child's +0x128 read
 * through func_ov039_0208e6f4. So the meteo sprite is displaced vertically by
 * whatever the pin's own vertical velocity is, which is what makes it read as
 * falling rather than sitting still.
 */
s32 func_ov039_0208fd8c(TaskPool* pool, Task* task, void* args) {
    OtuShadowTask* self = (OtuShadowTask*)task->data;

    if (*(s32*)((u8*)self + 0x68) != 0) {
        self->sprite.posX = F2I(*(s32*)((u8*)self + 0x58) - *(s32*)((u8*)self + 0x50));
        // (0x5C - 0x54) + 0x60, not 0x60 + (0x5C - 0x54): the target's
        // `sub r1, r2, r1` then `add r1, r3, r1` puts the difference first.
        self->sprite.posY = F2I(*(s32*)((u8*)self + 0x5C) - *(s32*)((u8*)self + 0x54) + *(s32*)((u8*)self + 0x60));
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

/**
 * @brief The two-word argument block the pin tasks are created with.
 *
 * `dataType` is read back by the four `_Sprite_Load` wrappers (it lands in the
 * animation template's dataType nibble); `childId` is read by the four init steps,
 * which store it as the id of the pin the new task will track.
 */
typedef struct {
    s32 dataType;
    s32 childId;
} OtuPinSpriteArgs; // Size: 0x8

/* The four load wrappers, defined further down but called by the init steps. */
void func_ov039_0208f1f8(OtuShadowTask* self, Sprite* sprite, s32* args);
void func_ov039_0208f4fc(OtuShadowTask* self, Sprite* sprite, s32* args);
void func_ov039_0208f874(OtuShadowTask* self, Sprite* sprite, s32* args);
void func_ov039_0208fb54(OtuShadowTask* self, Sprite* sprite, s32* args);

/* ==================================================================== */
/* The four init steps: 0208f2a4, 0208f5a8, 0208f900, 0208fbe0.        */
/* ==================================================================== */

/*
 * These four are the same body with three things varied -- which child id word
 * is written, which words get cleared, and whether the two scale words are
 * seeded with 0x1000. The seeds are the interesting part: shadow and meteo both
 * set scaleX = scaleY = 0x1000 (= 1.0 in Q12) and are the two that scale
 * anything later, while piyo and marker leave the whole block at zero because
 * they only ever move the sprite by an offset.
 *
 * The store order within each is load-bearing and follows the target exactly:
 * the child id lands first in the shadow and meteo (which write it into the
 * *highest* of their cleared words) but the clearing runs downward from it.
 */

/**
 * Shadow init: tracks the pin at `args->childId`, clears its two point pairs and
 * seeds the scales at 1.0, then loads the shadow sprite.
 */
s32 func_ov039_0208f2a4(TaskPool* pool, Task* task, OtuPinSpriteArgs* args) {
    OtuShadowTask* self = (OtuShadowTask*)task->data;

    // +0x60, not +0x64: the shadow shares the lower of the two id words with the
    // piyo, and only the meteo's id sits at +0x64.
    self->childId      = args->childId;
    self->pinPosA_hi.x = 0;
    self->pinPosA_hi.y = 0;
    self->pinPosB_hi.x = 0;
    self->pinPosB_hi.y = 0;
    self->unk_40       = 0;
    self->unk_44       = 0x1000;
    self->unk_48       = 0x1000;
    self->unk_4C       = 0;
    self->unk_4E       = 0;

    func_ov039_0208f1f8(self, &self->sprite, &args->dataType);
    return 1;
}

/**
 * Piyo init: same shape, but it stores the child id into +0x50 (which the
 * piyo's own update and render then read) and clears one extra word below it.
 * No scale seeds.
 */
s32 func_ov039_0208f5a8(TaskPool* pool, Task* task, OtuPinSpriteArgs* args) {
    OtuShadowTask* self = (OtuShadowTask*)task->data;

    self->pinPosA_hi.y = 0;
    self->pinPosA_hi.x = args->childId;
    self->unk_40       = 0;
    self->unk_44       = 0;
    self->unk_48       = 0;
    // A word store, not the halfword the shadow and meteo use here.
    *(s32*)((u8*)self + 0x4C) = 0;

    func_ov039_0208f4fc(self, &self->sprite, &args->dataType);
    return 1;
}

/** Marker init: the piyo's body with the child-id and zero stores reordered. */
s32 func_ov039_0208f900(TaskPool* pool, Task* task, OtuPinSpriteArgs* args) {
    OtuShadowTask* self = (OtuShadowTask*)task->data;

    self->pinPosA_hi.x        = args->childId;
    self->unk_40              = 0;
    self->unk_44              = 0;
    self->unk_48              = 0;
    *(s32*)((u8*)self + 0x4C) = 0; // a word store, as in the piyo
    self->pinPosA_hi.y        = 0;

    func_ov039_0208f874(self, &self->sprite, &args->dataType);
    return 1;
}

/**
 * Meteo init: the widest of the four.
 *
 * Clears seven words (two point pairs, the +0x60 velocity and the +0x68/+0x6C
 * state pair), seeds the scales at 1.0, and puts the child id in +0x64 -- two
 * words higher than the shadow's, which is what leaves +0x60 free for the
 * velocity the render step adds into y.
 */
s32 func_ov039_0208fbe0(TaskPool* pool, Task* task, OtuPinSpriteArgs* args) {
    OtuShadowTask* self = (OtuShadowTask*)task->data;

    self->meteoChildId = args->childId;
    self->pinPosA_hi.x = 0;
    self->pinPosA_hi.y = 0;
    self->pinPosB_hi.x = 0;
    self->pinPosB_hi.y = 0;
    self->childId      = 0; // the meteo's y velocity, cleared before first use
    self->alive        = 0;
    self->state        = 0;
    self->unk_40       = 0;
    self->unk_44       = 0x1000;
    self->unk_48       = 0x1000;
    self->unk_4C       = 0;
    self->unk_4E       = 0;

    func_ov039_0208fb54(self, &self->sprite, &args->dataType);
    return 1;
}

/* ==================================================================== */
/* The four update steps: 0208f2f0, 0208f5e0, 0208f938, 0208fc38.        */
/* ==================================================================== */

/**
 * Shadow update.
 *
 * Resolves the pin id to its data, asks func_ov039_0208e890 whether that pin is
 * still on the table, and only if the answer is exactly 1 copies the pin's two
 * coordinate pairs in and advances the sprite one frame.
 *
 * The `== 1` rather than `!= 0` is the interesting part: +0x64 stores the raw
 * filter result, and the render step only draws the shadow when it is non-zero,
 * so a filter that returns 2 or 3 would move the shadow without drawing it. The
 * filter's exact value space is not established here, but the equality test is
 * definitely not a truthiness test.
 */
s32 func_ov039_0208f2f0(TaskPool* pool, Task* task, void* args) {
    OtuShadowTask* self = (OtuShadowTask*)task->data;
    void*          pin  = EasyTask_GetTaskData(pool, self->childId);

    if (pin != NULL) {
        *(s32*)((u8*)self + 0x64) = func_ov039_0208e890(pin);

        if (*(s32*)((u8*)self + 0x64) == 1) {
            func_ov039_0208e85c(pin, &self->pinPosA_hi);
            func_ov039_0208e6e0(pin, &self->pinPosB_hi);
            self->unk_48 = func_ov039_0208e8c4(pin);
            self->unk_44 = self->unk_48;
            Sprite_Update(&self->sprite);
        }
    } else {
        *(s32*)((u8*)self + 0x64) = 0;
    }
    return 1;
}

/**
 * Piyo update: the distance-banded sprite swap.
 *
 * `func_ov039_0208ee98` reads the pin's +0x148 "still in play" word, and the
 * value `(that * 100) / self->unk_58` is banded into one of five animation frames:
 * 20/40/60/80 are the thresholds, giving frames 8, 7, 6, 5, 4. So the sprite
 * gets visibly bigger or smaller depending on how live the pin is -- this is the
 * overlay's "how close to scoring is this pin" readout, rendered as a sprite.
 *
 * The `+ 0x1E` reload of the timer is a 30-frame hysteresis on the
 * func_ov039_02087d04 recomputation, so the recompute happens at most once every
 * half second rather than every frame it is eligible.
 */
s32 func_ov039_0208f5e0(TaskPool* pool, Task* task, void* args) {
    OtuShadowTask* self = (OtuShadowTask*)task->data;
    void*          pin  = EasyTask_GetTaskData(pool, self->pinPosA_hi.x);
    // s16, not s32: the target carries the chosen frame in r2 straight into
    // Sprite_ChangeAnimation's `s16 animIndex` with no truncation, and an s32
    // local forces an `lsl #0x10 / asr #0x10` pair on the way in.
    s16 frame;
    s32 band;
    s32 live;

    if (pin != NULL) {
        live = func_ov039_0208ee98(pin);

        if (live > 0) {
            func_ov039_0208e85c(pin, OTU_PIN_POS_A(self));
            func_ov039_0208e6e0(pin, OTU_PIN_POS_B(self));

            // The quotient stays a full s32: the target compares it at 32-bit width
            // (`cmp r0, #0x14` straight after `_s32_div_f`) and only the chosen
            // frame is narrow.
            band = live * 100 / *(s32*)((u8*)self + 0x58);

            // Written as four flat `ble` tests rather than a switch: the target's
            // `cmp / movle / ble` triples are the if-chain shape, and a switch
            // would hoist all the compares to the top.
            if (band <= 0x14) {
                frame = 8;
            } else if (band <= 0x28) {
                frame = 7;
            } else if (band <= 0x3C) {
                frame = 6;
            } else if (band <= 0x50) {
                frame = 5;
            } else {
                frame = 4;
            }

            Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, frame, self->sprite.cellTable);
            Sprite_Update(&self->sprite);

            *(s32*)((u8*)self + 0x5C) = *(s32*)((u8*)self + 0x5C) - 1;
            if (*(s32*)((u8*)self + 0x5C) <= 0) {
                func_ov039_02087d04(0x330, OTU_PIN_POS_B(self), OTU_PIN_POS_A(self));
                *(s32*)((u8*)self + 0x5C) = 0x1E;
            }

            *(s32*)((u8*)self + 0x54) = 1;
        } else {
            *(s32*)((u8*)self + 0x54) = 0;
        }
    } else {
        *(s32*)((u8*)self + 0x54) = 0;
    }
    return 1;
}

/**
 * Marker update.
 *
 * Asks func_ov039_0208e950 for the pin's current marker frame and adopts it,
 * changing the sprite's animation only when it differs from the previous value --
 * so the sprite tracks a frame number the *pin* owns rather than computing its
 * own.
 */
s32 func_ov039_0208f938(TaskPool* pool, Task* task, void* args) {
    OtuShadowTask* self = (OtuShadowTask*)task->data;
    void*          pin  = EasyTask_GetTaskData(pool, self->pinPosA_hi.x);
    s32            prev;
    s32            frame;

    if (pin != NULL) {
        // Read before the store: the target holds the old frame in r6 across the
        // write, so the compare is against the previous value, not the new one.
        prev                      = *(s32*)((u8*)self + 0x54);
        frame                     = func_ov039_0208e950(pin);
        *(s32*)((u8*)self + 0x54) = frame;

        if (frame > 0) {
            if (frame != prev) {
                Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, (s16)frame, self->sprite.cellTable);
            }
            func_ov039_0208e85c(pin, OTU_PIN_POS_A(self));
            func_ov039_0208e6e0(pin, OTU_PIN_POS_B(self));
            Sprite_Update(&self->sprite);
        }
    } else {
        *(s32*)((u8*)self + 0x54) = 0;
    }
    return 1;
}

/**
 * Meteo update: the band's largest function and the only real state machine.
 *
 * Four states, dispatched by a jump table (`addls pc, pc, r0, lsl #2` over
 * `cmp #3`), which the brief's rule makes unambiguous: a switch, not an if-chain.
 * Case bodies run 0, 1, 2, 3 and each ends in an unconditional branch to the
 * common `Sprite_Update` tail.
 *
 *   0  the pin died or was never there -- stop drawing
 *   1  a plain countdown on +0x70, falling back to 0 when it runs out
 *   2  easing scaleY toward 1.0 in Q12, `(0x1000 - y) / frames` per step
 *   3  the wind-up: func_ov039_02087bf8 lays out a path, and once +0x70 hits 30
 *      it walks eight further children through func_ov039_02097240
 *
 * State 2's `rsb r0, r0, #0x1000` is the tell that this is a converging ease
 * rather than a linear one: it always moves scaleY *toward* 0x1000 and the step
 * shrinks as it gets there.
 */
// Nonmatching: 94%. The logic is settled -- the four-state jump table, all four
// case bodies, the shared Sprite_Update guard and the null-pin arm are correct,
// and the filter is the kind == 8 test.
//
// What is left is one duplicated store and the block placement around it. The
// target emits the null-pin clear as its own tail block at 0x0208fd68
// (\mov r0, #0 / str r0, [r4, #0x68]\) which the \eq\ at the top jumps *forward*
// to, and separately emits the same two instructions as \case 0\. This build
// recognises the two as the same value and merges them, so the null arm's clear
// is hoisted above the \eq\ and the tail block disappears.
//
// Both an \if (pin == NULL) {...} else {...}\ and an early eturn 1\ arm (with
// the Sprite_Update guard duplicated by hand) were tried; the else form is the
// better of the two at 94% against 88%. Splitting the two stores into something
// mwcc cannot see as the same value would work but would mean writing code the
// target does not, so the honest form is kept.
s32 func_ov039_0208fc38(TaskPool* pool, Task* task, void* args) {
    OtuShadowTask* self = (OtuShadowTask*)task->data;
    void*          pin  = EasyTask_GetTaskData(pool, self->meteoChildId);
    s32            i;

    if (pin == NULL) {
        self->alive = 0;
    } else {
        self->alive = func_ov039_0208e984(pin);

        if (self->alive != 0) {
            func_ov039_0208e85c(pin, &self->pinPosA_hi);
            func_ov039_0208e6e0(pin, &self->pinPosB_hi);
            *(s32*)((u8*)self + 0x60) = func_ov039_0208e6f4(pin);
        } else {
            self->state = 0;
        }

        switch (self->state) {
            case 0:
                self->alive = 0;
                break;

            case 1:
                self->timer = self->timer - 1;
                if (self->timer <= 0) {
                    self->state = 0;
                }
                break;

            case 2:
                if (self->timer > 0) {
                    self->unk_48 = self->unk_48 + _s32_div_f(0x1000 - self->unk_48, self->timer);
                    self->timer  = self->timer - 1;
                }
                break;

            case 3:
                func_ov039_02087bf8((u8*)self + 0x94, (u8*)self + 0x40);

                if (self->timer == 0x1E) {
                    for (i = 0; i < 8; i++) {
                        // The target reads exactly two words from this pointer
                        // (+0x00 and +0x04), which is what OtuTaskParams is; the
                        // address is already a pointer to that pair.
                        func_ov039_02097240(EasyTask_GetTaskData(pool, ((s32*)((u8*)self + 0x74))[i]),
                                            (OtuTaskParams*)&self->pinPosB_hi);
                    }
                }

                self->timer = self->timer - 1;
                if (self->timer <= 0) {
                    self->state = 0;
                }
                break;
        }

        if (self->alive != 0) {
            Sprite_Update(&self->sprite);
        }
    }
    return 1;
}

/**
 * Meteo destroy.
 *
 * Releases the sprite, then deletes eight tasks listed at +0x74. So the meteo
 * owns a fan-out of eight children -- the same eight func_ov039_0208fe60 created
 * for it -- and this is their teardown.
 */
s32 func_ov039_0208fddc(TaskPool* pool, Task* task, void* args) {
    OtuShadowTask* self = (OtuShadowTask*)task->data;
    s32            i;

    Sprite_Release((Sprite*)self);

    for (i = 0; i < 8; i++) {
        EasyTask_DeleteTask(pool, ((s32*)((u8*)self + 0x74))[i]);
    }
    return 1;
}

/**
 * Creates a meteo task and its eight children.
 *
 * Unlike the other three constructors this one does work after creating the task:
 * it resolves the new task's data pointer and fills +0x74 with eight ids from
 * func_ov039_0209720c -- which is what func_ov039_0208fddc later deletes and what
 * func_ov039_0208fc38's state 3 walks.
 */
s32 func_ov039_0208fe60(TaskPool* pool, s32 dataType, s32 arg2) {
    OtuPinSpriteArgs args;
    // `id` is declared before `self` on purpose: mwcc hands out callee-saved
    // registers in declaration order, and the target keeps the task id in r7 with
    // the pool in r6. Declaring them the other way round puts `self` in r7 and
    // costs four instructions of register shuffling.
    s32            id;
    OtuShadowTask* self;
    s32            i;

    args.dataType = dataType;
    args.childId  = arg2;

    id   = EasyTask_CreateTask(pool, &data_ov039_020993e4, NULL, 0, NULL, &args);
    self = (OtuShadowTask*)EasyTask_GetTaskData(pool, id);

    for (i = 0; i < 8; i++) {
        ((s32*)((u8*)self + 0x74))[i] = func_ov039_0209720c(pool, dataType, arg2);
    }
    return id;
}

/* ------------------------------------------------------------------ */
/* The meteo's three state-entry steps and its position copier.        */
/* ------------------------------------------------------------------ */

/*
 * These write `state` last and set whatever the new state needs, so they are
 * entry points called from elsewhere rather than from the update step -- the
 * update step's switch *consumes* state and never sets anything but 0.
 */

/** Meteo state 1: re-enter animation frame 1 at full scale, for `frames` frames. */
void func_ov039_0208fefc(OtuShadowTask* self, s32 frames) {
    self->timer  = frames;
    self->unk_44 = 0x1000;
    self->unk_48 = 0x1000;

    Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, 1, self->sprite.cellTable);
    self->state = 1;
}

/** Meteo state 2: enter the scale-ease, four frames of it, with a small scaleY. */
void func_ov039_0208ff30(OtuShadowTask* self) {
    self->timer  = 4;
    self->unk_44 = 0x1000;
    self->unk_48 = 0x29;

    Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, 4, self->sprite.cellTable);
    self->state = 2;
}

/**
 * Meteo state 3: the wind-up. Frame 5, thirty frames of timer, and a path laid
 * out by func_ov039_02087ba0 from the table at 0x0209a54c.
 */
void func_ov039_0208ff68(OtuShadowTask* self) {
    self->timer = 0x1E;

    Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, 5, self->sprite.cellTable);
    func_ov039_02087ba0((u8*)self + 0x94, data_ov039_0209a54c, 6, (u8*)self + 0x40);
    self->state = 3;
}

/**
 * @brief A position plus a scale, the three-word record func_ov039_0208fee0 fills.
 *
 * Named rather than spelled as an `s32 out[3]` because the array form lets mwcc
 * merge the first two stores into one `stmia`, which the target does not do.
 */
typedef struct {
    s32 x;
    s32 y;
    s32 scale;
} OtuPinRecord; // Size: 0xC

/**
 * Copies the sprite's +0x58/+0x5C pair out to a three-word record, with a fixed
 * 0x1D000 third word.
 *
 * 0x1D000 is 29.0 in Q12 -- the overlay's own scale constant rather than anything
 * derived here, the same 0x1000-family values the four init steps seed.
 */
// Nonmatching: 98%, and only in the order of the first two stores -- the target
// stores +0x58 then +0x5C where this build stores +0x5C then +0x58. The two
// `ldr`s, the constant, the third store and the epilogue all match. The gap is
// mwcc's store scheduling for two adjacent same-base writes, which it will not
// reorder back; eight source spellings were tried (locals in both orders, the
// three field orders, and a raw `s32*` cast) and none produces the target's
// order together with the loads the target has.
void func_ov039_0208fee0(OtuShadowTask* self, OtuPinRecord* out) {
    s32 x = self->pinPosB_hi.x;
    s32 y = self->pinPosB_hi.y;

    out->scale = 0x1D000;
    out->x     = x;
    out->y     = y;
}

/* OtuCursor, func_ov039_0208ffac and func_ov039_0208ffd8 are declared in
 * include/Debug/Sugata/TinPinSlammer.h rather than here: the hammer task in band
 * 7 drives these same two functions over its own cursor, and band 7 comes first
 * in the include order. Both bands are callers, so neither should own it. */

/**
 * Starts a cursor at entry zero and primes it with the first entry's value plus
 * one.
 *
 * The `+ 1` is why the constructor and the stepper agree: a freshly started
 * cursor has `framesLeft` equal to entry 0's duration, so the first step
 * immediately falls through to entry 1.
 */
void func_ov039_0208ffac(OtuCursor* c, s32* table, s16 count) {
    // No locals for the table or the index: the target reloads both from the
    // cursor rather than reusing the incoming `table` and the zero it just
    // stored, which is what `ldr r1, [r0, #4] / ldr r2, [r0, #0]` says.
    s32 offset;

    c->table = table;
    c->index = 0;
    c->count = count;
    // The scale is computed as its own step: the target emits `lsl r1, r1, #2`
    // and then a separate `ldrsh r1, [r2, r1]`, whereas a single subscript folds
    // the two into one indexed load.
    offset        = c->index * 4;
    c->framesLeft = *(s16*)((u8*)c->table + offset) + 1;
}

/**
 * Advances a cursor by one frame.
 *
 * Returns 0 once the index reaches the count -- including from inside the
 * reload, so a cursor that runs off the end stops there rather than reading past
 * the table. This is the "both returns are `cmp/ge/bxge`" shape, so it is two
 * separate early exits rather than one shared one.
 */
s32 func_ov039_0208ffd8(OtuCursor* c) {
    s16 index;

    if (c->index >= c->count) {
        return 0;
    }

    c->framesLeft = c->framesLeft - 1;

    if (c->framesLeft <= 0) {
        c->index = c->index + 1;
        // A local for the new index: the target re-reads +0x4 into a *third*
        // register (r3) after the store, so the compare and the table lookup both
        // use the stored value rather than the one left over from the add.
        index = c->index;

        if (index >= c->count) {
            return 0;
        }

        c->framesLeft = *(s16*)((u8*)c->table + index * 4);
    }
    return 1;
}

/* ==================================================================== */
/* The four task constructors and the four stage dispatchers.           */
/* ==================================================================== */

/**
 * Creates a shadow task.
 *
 * The args block is a stack local rather than a caller-supplied pointer, so
 * EasyTask_CreateTask is handed `&args` as its last argument. All four of these
 * are identical apart from which TaskHandle they name -- which is the clearest
 * statement the target makes that these really are four instances of one task.
 */
s32 func_ov039_0208f40c(TaskPool* pool, s32 dataType, s32 childId) {
    OtuPinSpriteArgs args;

    args.dataType = dataType;
    args.childId  = childId;
    return EasyTask_CreateTask(pool, &data_ov039_0209930c, NULL, 0, NULL, &args);
}

/** Creates a piyo task. */
s32 func_ov039_0208f770(TaskPool* pool, s32 dataType, s32 childId) {
    OtuPinSpriteArgs args;

    args.dataType = dataType;
    args.childId  = childId;
    return EasyTask_CreateTask(pool, &data_ov039_02099354, NULL, 0, NULL, &args);
}

/** Creates a marker task. */
s32 func_ov039_0208fa5c(TaskPool* pool, s32 dataType, s32 childId) {
    OtuPinSpriteArgs args;

    args.dataType = dataType;
    args.childId  = childId;
    return EasyTask_CreateTask(pool, &data_ov039_0209939c, NULL, 0, NULL, &args);
}

/*
 * The four stage dispatchers. `const TaskStages` on the stack rather than a
 * direct `table.iter[stage]` through the global is what produces the target's
 * `ldm/stm` pair -- the same shape as NRepMenu_RunTask in NRep.c. Each copies
 * its own four-entry table out of .rodata and indexes it.
 */

/** Shadow task dispatcher. */
s32 func_ov039_0208f3c4(TaskPool* pool, Task* task, void* args, s32 stage) {
    const TaskStages stages = data_ov039_02099318;

    return stages.iter[stage](pool, task, args);
}

/** Piyo task dispatcher. */
s32 func_ov039_0208f728(TaskPool* pool, Task* task, void* args, s32 stage) {
    const TaskStages stages = data_ov039_02099360;

    return stages.iter[stage](pool, task, args);
}

/** Marker task dispatcher. */
s32 func_ov039_0208fa14(TaskPool* pool, Task* task, void* args, s32 stage) {
    const TaskStages stages = data_ov039_020993a8;

    return stages.iter[stage](pool, task, args);
}

/** Meteo task dispatcher. */
s32 func_ov039_0208fe18(TaskPool* pool, Task* task, void* args, s32 stage) {
    const TaskStages stages = data_ov039_020993f0;

    return stages.iter[stage](pool, task, args);
}

/** Loads the meteo sprite: absolute position from +0x58/+0x5C, as the shadow. */
void func_ov039_0208fb54(OtuShadowTask* self, Sprite* sprite, s32* args) {
    SpriteAnimation anim = data_ov039_02099400;

    anim.owner    = self;
    anim.dataType = *args;
    anim.posX     = F2I(*(s32*)((u8*)self + 0x58));
    anim.posY     = F2I(*(s32*)((u8*)self + 0x5C));

    _Sprite_Load(sprite, &anim);
}
s32 func_ov039_0208f0b0(void* task) {
    return *(u16*)(*(u8**)((u8*)task + 0x16C)) == 0x130;
}

/**
 * Clears a child's +0x84/+0x80 pair through func_ov039_02096270, but only when
 * +0x1A8 is non-zero.
 *
 * The +0x1A8 word is a "has this been done" latch: the target tests it, returns
 * early on zero, and never clears it, so a second call is a no-op only because
 * something else does. Worth stating because it is the one place in this band
 * where an early exit is a side-effect guard rather than a value test.
 */
void func_ov039_0208f0c8(void* task) {
    if (*(s32*)((u8*)task + 0x1A8) != 0) {
        func_ov039_02096270(EasyTask_GetTaskData(*(void**)((u8*)task + 8), *(s32*)((u8*)task + 0x230)));
    }
}

/**
 * Clears +0x1B0 when it equals the argument.
 *
 * `cmp / moveq #0 / streq` -- the store is conditional on the comparison, so
 * the source is an `if` with the constant folded into it, not a masked write.
 */
void func_ov039_0208f0f0(void* task, s32 value) {
    if (*(s32*)((u8*)task + 0x1B0) == value) {
        *(s32*)((u8*)task + 0x1B0) = 0;
    }
}

/**
 * Clears +0x114/+0x118 on two children at +0x228 and +0x22C.
 *
 * The loop reads the child id as `add r1, r5, r4, lsl #2` then `ldr r1, [r1,
 * #0x228]`, i.e. the index is scaled into a *base* and the field is a fixed
 * displacement off it -- the same shape as OTU_CHILD_ID.
 *
 * Reaching that shape needs the subscript form below. Every variant written as
 * `*(s32*)((u8*)task + i * 4 + 0x228)` makes mwcc strength-reduce the loop into
 * a walked pointer (`mov r5, r4` / `add r5, r5, #4`), which is a real
 * difference and costs several instructions; subscripting a `s32*` base keeps
 * the scaled add and the fixed displacement separate and matches exactly.
 */
void func_ov039_0208f104(void* task) {
    s32 i;

    for (i = 0; i < 2; i++) {
        func_ov039_02095ddc(EasyTask_GetTaskData(*(void**)((u8*)task + 8), ((s32*)((u8*)task + 0x228))[i]));
    }
}
