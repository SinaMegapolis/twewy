/* Band 5 -- see OtuBands.h for the address range and why this is an include. */

#include "Debug/Sugata/TinPinSlammer.h"
#include "Engine/EasyTask.h"
#include "Engine/Math/Random.h"
#include "SpriteMgr.h"
#include <nitro/fx/fx_division.h>


/*
 * ============================================================================
 * Band 5: the four pin-sprite tasks.
 * ============================================================================
 *
 * This band is four copies of one task type. Each owns a base-game `Sprite` at
 * the head of its data block and a run of pinball state after it, and each is
 * driven through the engine's `TaskStages` quartet -- init, update, render,
 * cleanup -- held in one of four `.rodata` tables:
 *
 *   data_ov039_02099f10   02096fcc  0209702c  02097160  020971b0
 *   data_ov039_02099f58   020974e0  02097528  02097670  020976c0
 *   data_ov039_02099fa0   02097918  02097954  020979cc  02097a14
 *   data_ov039_02099fe8   02097c30  02097c90  02097dbc  02097e0c
 *
 * Every stage takes `(TaskPool*, Task*, void*)`, returns 1, and reaches its
 * object through `task->data` -- the `ldr rX, [r1, #0x18]` that opens all
 * sixteen. Which of the sixteen are here:
 *
 *   first  table   init (02096fcc) is below this band; the other three are here
 *   second table   init/update/render/cleanup all here (020974e0..020976c0)
 *   third  table   all four here (02097918..02097a14)
 *   fourth table   all four here (02097c30..02097e0c)
 *
 * All sixteen stages are here except 0x02096fcc, which is the first table's init
 * and sits below this band; it is declared (unprototyped) where it is needed.
 * The band also holds the four stage dispatchers, the four task-creation
 * wrappers, the four `_Sprite_Load` wrappers and the four public setup routines.
 *
 * What the quartet does is legible from the code: an update stage fetches
 * another task's data, asks for its +0x110/+0x114 pair through
 * func_ov039_0208e85c, and blends the sprite toward it with
 * func_ov039_02098c00, so this is a sprite that chases a pin. What the pin is,
 * and what any of the counters mean, the code does not say and neither does
 * this file.
 */

/**
 * @brief A pin-sprite task's data block: a base-game `Sprite` plus overlay state.
 *
 * The first 0x40 bytes are an ordinary `Sprite`, and that is not a guess: it is
 * forced three ways over. The block is handed straight to `Sprite_Update`,
 * `Sprite_RenderFrame`, `Sprite_Release` and `_Sprite_Load`, its +0x18 and +0x1C
 * are passed back out as `Sprite_ChangeAnimation`'s `animData` and `cellTable`
 * arguments, and its first word is tested as a bitfield. Everything from +0x40
 * on is the overlay's own.
 *
 * The words are kept as a flat run rather than grouped into points, because the
 * two helpers that take them (`func_ov039_0208e85c` writes one,
 * `func_ov039_02098c00` reads one and writes another) are handed bare addresses
 * and the layouts differ between the four objects: the second task's block is
 * the first's shifted down by 0x10 for the first six fields and not for the
 * rest. Grouping them would assert a correspondence the code does not have.
 */
typedef struct {
    /* 0x00 */ Sprite sprite; // 0x40 bytes; +0x0C/+0x0E are posX/posY
    /* 0x40 */ s32    unk_40;
    /* 0x44 */ s32    unk_44;
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
    /* 0x70 */ s32    unk_70;
    /* 0x74 */ s32    unk_74;
    /* 0x78 */ s32    unk_78;
    /* 0x7C */ s32    unk_7C;
    /* 0x80 */ s32    unk_80;
    /* 0x84 */ s32    unk_84;
} OtuTaskSprite; // Size: 0x88

/** @brief One of the four stages of a pin-sprite task. */
typedef s32 (*OtuTaskSpriteStage)(TaskPool* pool, Task* task, void* param);

/* ------------------------------------------------------------------ */
/* Data the band reads but does not define.                            */
/* ------------------------------------------------------------------ */

/*
 * The four task descriptors the `EasyTask_CreateTask` wrappers hand over, and
 * the four `.rodata` stage tables the dispatchers name. All of these live in the
 * overlay's gap-filled `.rodata`, so they are referenced by the build's own
 * names: declaring an object of one's own would land it at an address of the
 * linker choosing, not the one the target uses, which is the same reason the
 * result-screen row tables are declared that way.
 *
 * The stage tables are declared but not referenced -- the dispatchers build their
 * own copies as local initialisers, because that is the only form mwcc emits as
 * the `ldm/stm` the target has. They are declared anyway so that the addresses
 * are on the record, and so that the one-row relocation-name gap on each
 * dispatcher is legible rather than mysterious. See the dispatcher section.
 */
extern const TaskHandle data_ov039_02099f04;
extern const TaskHandle data_ov039_02099f4c;
extern const TaskHandle data_ov039_02099f94;
extern const TaskHandle data_ov039_02099fdc;

extern const TaskStages data_ov039_02099f10;
extern const TaskStages data_ov039_02099f58;
extern const TaskStages data_ov039_02099fa0;
extern const TaskStages data_ov039_02099fe8;

/**
 * @brief The bin every pin-sprite in the overlay draws its cells from.
 *
 * All four `SpriteAnimation` templates in the overlay name this one record (bin
 * id 39); it is the only thing tying the four sprites' art together. Not
 * identified further -- it is a plain identifier record in the overlay's `.data`.
 */
extern const BinIdentifier data_ov039_0209a0dc;

/*
 * Eight Q12.12 constants in the overlay's `.data`, read by the two setup
 * routines. Values recovered with `tools/ov039_table.py`:
 *
 *   0209a2fc = 0x2000    0209a300 = 0x1000    0209a304 = 0x0133
 *   0209a30c = 0x4000    0209a310 = 0x0400    0209a314 = 0x3000
 *   0209a328 = 0x9800
 *
 * None of them is used the same way twice: 0x0133 is a per-frame bleed out of a
 * count, 0x0400 is both a `FX_Divide` denominator and a per-frame spin-up, and
 * 0x2000/0x4000 are the bounds of the two random ranges the starting spin is
 * drawn from.
 *
 * They are deliberately *not* declared `const`. They live in `.data`, not
 * `.rodata`, and mwcc acts on that: told a symbol is const it will keep its
 * value in a callee-saved register across the `FX_Divide` call in the two setup
 * routines, where the target reloads it every time. The `const` costs three
 * instructions there and buys nothing.
 */
extern s32 data_ov039_0209a2fc;
extern s32 data_ov039_0209a300;
extern s32 data_ov039_0209a304;
extern s32 data_ov039_0209a30c;
extern s32 data_ov039_0209a310;
extern s32 data_ov039_0209a314;
extern s32 data_ov039_0209a328;

/**
 * @brief The base game's random `s16` pair table, indexed off `RNG_Next`.
 *
 * Unidentified. It is a plain `s16` array in the main module, and both this band
 * and the boss code read a consecutive pair out of it at a random index. This
 * band uses it as a random direction vector; that is an inference from the two
 * values never being equal and neither being zero, not something the data says.
 */
/* data_0205e4e0 is an array of s32, declared by band 3 -- casting its *value*
 * rather than its address was worth 43 points on func_ov039_02095788. */

/**
 * @brief Reads a task's +0x110/+0x114 pair out to a point, 0x0208e85c.
 *
 * Defined in OtuFieldAccess.c, which this file is #included into *above* the
 * definition, so it has to be declared here rather than picked up from the
 * header -- the header does not carry it, and without a declaration mwcc infers
 * `int (...)` from the call below and then rejects the definition.
 */

/**
 * @brief The overlay's Q12.12 point blend, 0x02098c00.
 *
 * `out = base + (offset * scale) >> 12`, componentwise. Declared without a
 * prototype so that this file cannot disagree with whoever defines it.
 */

/** @brief The base game's 32-bit signed divide. Also unprototyped. */
s32 _s32_div_f();

/*
 * The four `_Sprite_Load` wrappers. Each fills a `SpriteAnimation` template with
 * the object's own anchor and owner and loads one sprite; only the field the
 * anchor is read from and the template's constant differ between them.
 *
 * The third argument is the task's creation block again, read as a word and
 * narrowed to a halfword: the target loads a full word and masks it with
 * `lsl #0x10; lsr #0x10` before folding it into the template's four-bit
 * `dataType` field, which a `u16*` parameter would have turned into a single
 * `ldrh` instead. The target passes nothing for this argument and the callee
 * reads whatever is in r2, which is still the block because no caller has
 * touched r2 since loading the id out of it. Passing it explicitly costs no
 * instruction -- r2 already holds it -- and keeps the declaration honest.
 */
void func_ov039_02097454(OtuTaskSprite* self, Sprite* sprite, s32* dataType);
void func_ov039_0209788c(OtuTaskSprite* self, Sprite* sprite, s32* dataType);
void func_ov039_02097ba4(OtuTaskSprite* self, Sprite* sprite, s32* dataType);

/* ------------------------------------------------------------------ */
/* The cleanup stages: four identical five-instruction bodies.         */
/* ------------------------------------------------------------------ */

/*
 * The four `Sprite_Release` wrappers. The engine's convention is that a cleanup
 * stage returns 1 to keep the task alive, and all four do so unconditionally --
 * which is what the bare `mov r0, #1` is for here, exactly as in the four
 * `mov r0, #1; bx lr` bodies OtuFieldAccess.c declines to name.
 *
 * They are written out four times rather than factored, because the build uses
 * `-inline noauto` and a `static` helper would compile to a real `bl` and
 * collapse every caller to a stub.
 */

/** Releases the first pin-sprite's sprite. */
s32 func_ov039_020971b0(TaskPool* pool, Task* task, void* param) {
    Sprite_Release((Sprite*)task->data);
    return 1;
}

/** Releases the second pin-sprite's sprite. */
s32 func_ov039_020976c0(TaskPool* pool, Task* task, void* param) {
    Sprite_Release((Sprite*)task->data);
    return 1;
}

/** Releases the third pin-sprite's sprite. */
s32 func_ov039_02097a14(TaskPool* pool, Task* task, void* param) {
    Sprite_Release((Sprite*)task->data);
    return 1;
}

/** Releases the fourth pin-sprite's sprite. */
s32 func_ov039_02097e0c(TaskPool* pool, Task* task, void* param) {
    Sprite_Release((Sprite*)task->data);
    return 1;
}

/* ------------------------------------------------------------------ */
/* The create wrappers.                                                */
/* ------------------------------------------------------------------ */

/*
 * Each pushes the two words it was given as the new task's creation parameter
 * block and calls `EasyTask_CreateTask` with the NULL data pointer, priority 0
 * and NULL parent the engine's signature asks for.
 *
 * The block is a named two-word struct rather than two scalar locals, and that
 * is load-bearing: only the block's *address* is ever taken, so with scalars mwcc
 * drops the second store as dead and the frame shrinks by four bytes. A struct
 * whose address is taken keeps both.
 *
 * Every `init` stage receives this block as its third argument and reads it as
 * two words: the first becomes the sprite's initial position, the second a task
 * id the object then chases.
 *
 * The stages take it as `void*` and the four public setup routines take
 * `OtuTaskParams*`, and that is not an inconsistency. A stage's signature is
 * fixed by `TaskStages` -- every stage shares `(TaskPool*, Task*, void*)` because
 * the engine's table is homogeneous -- and mwcc rejects the implicit conversion
 * if one stage narrows it. The public routines have no such constraint and say
 * what they mean.
 */

/** @brief The two-word parameter block `EasyTask_CreateTask` hands to a task. */
typedef struct {
    /* 0x0 */ s32 word0;
    /* 0x4 */ s32 word1;
} OtuTaskParams; // Size: 0x8

/* Forward declarations for the two functions below that this file's own
 * call sites reach first. Without them C infers `int (...)` and the real
 * definition then reads as a redeclaration. This is band 5 declaring its own
 * functions, not a dependency on another band.
 *
 * They sit here rather than at the top of the file because they name
 * OtuTaskSprite and OtuTaskParams, which are defined just above: the two
 * typedefs live in this file rather than the feature header because this is
 * their only user. */
s32  func_ov039_0209720c(TaskPool* pool, s32 word0, s32 word1);
void func_ov039_02097240(OtuTaskSprite* s, OtuTaskParams* param);

/* Returns the task handle. The target's last two instructions are the call
 * and the epilogue with no `mov r0` between, so r0 already holds the handle
 * and returning it costs nothing -- and a caller in band 1 assigns it. */
s32 func_ov039_0209720c(TaskPool* pool, s32 word0, s32 word1) {
    OtuTaskParams params;

    params.word0 = word0;
    params.word1 = word1;
    return EasyTask_CreateTask(pool, &data_ov039_02099f04, NULL, 0, NULL, &params);
}

s32 func_ov039_0209771c(TaskPool* pool, s32 word0, s32 word1) {
    OtuTaskParams params;

    params.word0 = word0;
    params.word1 = word1;
    return EasyTask_CreateTask(pool, &data_ov039_02099f4c, NULL, 0, NULL, &params);
}

s32  func_ov039_02097a70(TaskPool* pool, s32 word0, s32 word1) {
    OtuTaskParams params;

    params.word0 = word0;
    params.word1 = word1;
    return EasyTask_CreateTask(pool, &data_ov039_02099f94, NULL, 0, NULL, &params);
}

s32 func_ov039_02097e68(TaskPool* pool, s32 word0, s32 word1) {
    OtuTaskParams params;

    params.word0 = word0;
    params.word1 = word1;
    return EasyTask_CreateTask(pool, &data_ov039_02099fdc, NULL, 0, NULL, &params);
}

/* ------------------------------------------------------------------ */
/* The render stages.                                                  */
/* ------------------------------------------------------------------ */
/*
 * (The four stage dispatchers are at the end of this file, once all sixteen
 * stages they name are in scope -- mwcc will not take a function's address
 * before it is declared.)
 */

/*
 * A render stage sets the sprite's `posX`/`posY` from two Q12.12 differences and
 * renders -- but only if the flag the update stage raised on the previous frame
 * is still set. Three of the four add a third term to `posY`; the fourth has
 * none, which is the only real difference between them.
 */

/** The first pin-sprite's render stage. */
s32 func_ov039_02097160(TaskPool* pool, Task* task, void* param) {
    OtuTaskSprite* s = (OtuTaskSprite*)task->data;

    if (s->unk_7C != 0) {
        s->sprite.posX = (s16)((s->unk_58 - s->unk_50) >> 0xC);
        s->sprite.posY = (s16)((s->unk_5C - s->unk_54 + s->unk_60) >> 0xC);
        Sprite_RenderFrame((Sprite*)s);
    }

    return 1;
}

/**
 * @brief The second pin-sprite's render stage.
 *
 * The same shape as the first's on the first task's layout, which the second
 * task reuses shifted down by 0x10 for these six fields.
 */
s32 func_ov039_02097670(TaskPool* pool, Task* task, void* param) {
    OtuTaskSprite* s = (OtuTaskSprite*)task->data;

    if (s->unk_6C != 0) {
        s->sprite.posX = (s16)((s->unk_48 - s->unk_40) >> 0xC);
        s->sprite.posY = (s16)((s->unk_4C - s->unk_44 + s->unk_50) >> 0xC);
        Sprite_RenderFrame((Sprite*)s);
    }

    return 1;
}

/** The fourth pin-sprite's render stage -- the first's, unchanged. */
s32 func_ov039_02097dbc(TaskPool* pool, Task* task, void* param) {
    OtuTaskSprite* s = (OtuTaskSprite*)task->data;

    if (s->unk_7C != 0) {
        s->sprite.posX = (s16)((s->unk_58 - s->unk_50) >> 0xC);
        s->sprite.posY = (s16)((s->unk_5C - s->unk_54 + s->unk_60) >> 0xC);
        Sprite_RenderFrame((Sprite*)s);
    }

    return 1;
}

/**
 * @brief The third pin-sprite's render stage.
 *
 * The odd one out. It watches +0x58 rather than +0x7C, and its `posY` has no
 * third term -- which follows from the third task being the short one: it never
 * keeps a value at the offset that term would come from.
 */
s32 func_ov039_020979cc(TaskPool* pool, Task* task, void* param) {
    OtuTaskSprite* s = (OtuTaskSprite*)task->data;

    if (s->unk_58 != 0) {
        s->sprite.posX = (s16)((s->unk_48 - s->unk_40) >> 0xC);
        s->sprite.posY = (s16)((s->unk_4C - s->unk_44) >> 0xC);
        Sprite_RenderFrame((Sprite*)s);
    }

    return 1;
}

/* ------------------------------------------------------------------ */
/* The init stages that live in this band.                             */
/* ------------------------------------------------------------------ */

/**
 * @brief The second pin-sprite's init stage.
 *
 * Clears the position block and both flags, keeps the creation block's second
 * word as the task id to chase at +0x68, and loads the sprite.
 */
s32 func_ov039_020974e0(TaskPool* pool, Task* task, void* param) {
    OtuTaskSprite* s = (OtuTaskSprite*)task->data;

    s->unk_6C = 0;
    s->unk_70 = 0;
    s->unk_68 = ((s32*)param)[1];
    s->unk_40 = 0;
    s->unk_44 = 0;
    s->unk_48 = 0;
    s->unk_4C = 0;
    s->unk_50 = 0;
    s->unk_54 = 0;
    s->unk_58 = 0;
    func_ov039_02097454(s, (Sprite*)s, (s32*)param);

    return 1;
}

/**
 * @brief The third pin-sprite's init stage.
 *
 * The same shape as the second's over the shorter field set: it keeps the
 * creation block's second word at +0x50 and clears +0x40 through +0x4C.
 */
s32 func_ov039_02097918(TaskPool* pool, Task* task, void* param) {
    OtuTaskSprite* s = (OtuTaskSprite*)task->data;

    s->unk_54 = 0;
    s->unk_58 = 0;
    s->unk_50 = ((s32*)param)[1];
    s->unk_40 = 0;
    s->unk_44 = 0;
    s->unk_48 = 0;
    s->unk_4C = 0;
    func_ov039_0209788c(s, (Sprite*)s, (s32*)param);

    return 1;
}

/**
 * @brief The fourth pin-sprite's init stage.
 *
 * The first task's shape, with +0x44 and +0x48 preset to 0x1000 rather than
 * 0x1800 and with the last two clears written as halfwords -- the only place in
 * the band where a field is stored as two `s16`s instead of one word, so they
 * are written that way here.
 */
s32 func_ov039_02097c30(TaskPool* pool, Task* task, void* param) {
    OtuTaskSprite* s = (OtuTaskSprite*)task->data;

    s->unk_78              = 0;
    s->unk_7C              = 0;
    s->unk_74              = ((s32*)param)[1];
    s->unk_50              = 0;
    s->unk_54              = 0;
    s->unk_58              = 0;
    s->unk_5C              = 0;
    s->unk_60              = 0;
    s->unk_64              = 0;
    s->unk_68              = 0;
    s->unk_40              = 0;
    s->unk_44              = 0x1000;
    s->unk_48              = 0x1000;
    *(s16*)&s->unk_4C      = 0;
    *(s16*)((u8*)s + 0x4E) = 0;
    func_ov039_02097ba4(s, (Sprite*)s, (s32*)param);

    return 1;
}

/* ------------------------------------------------------------------ */
/* The two 300-byte update siblings.  This is the band.                */
/* ------------------------------------------------------------------ */

/*
 * 0x0209702c (the first task) and 0x02097c90 (the fourth) are the same function
 * with one constant changed. Both walk the same path -- fetch the chased pin,
 * count one tick off a budget, read the pin's position, scale the budget into a
 * Q12.12 anchor, bleed a count down, spin the object up, blend its position
 * toward the anchor, and raise the flag the render stage watches -- and their
 * instructions line up one for one apart from two differences:
 *
 *   1. the multiplier. The first computes `budget * 0x1800`, which mwcc emits as
 *      an explicit `mov r0, #0x1800; mul`; the fourth computes `budget * 0x1000`,
 *      which it folds to the `lsl r0, r0, #0xc` the target wants. So the two
 *      constants are genuinely different -- 0x1800 is 1.5 in Q12.12 and 0x1000 is
 *      1.0 -- and it is the *value*, not the spelling, that separates them: the
 *      target itself spells the fourth one as a shift because the constant makes
 *      a shift legal. This is not the divide-form gap from the brief; both sides
 *      are multiplies and both fall out of a plain `*`.
 *
 *   2. where the position read sits. The first calls the position getter and
 *      only then reloads the budget for the multiply; the fourth reads the
 *      position first and decrements after, so the decremented value is still in
 *      a register at the multiply. That is a source-order difference, not a
 *      codegen one: in the first the reload is forced because the call has
 *      already clobbered r0.
 *
 * They are written out twice rather than factored, per the `-inline noauto`
 * rule in the brief.
 */

/** The first pin-sprite's update stage. */
s32 func_ov039_0209702c(TaskPool* pool, Task* task, void* param) {
    OtuTaskSprite* s   = (OtuTaskSprite*)task->data;
    void*          pin = EasyTask_GetTaskData(pool, s->unk_74);

    if (pin != NULL) {
        if (s->unk_78 != 0) {
            s->unk_84 = s->unk_84 - 1;

            if (s->unk_84 <= 0) {
                s->unk_78 = 0;
                s->unk_7C = 0;
            } else {
                func_ov039_0208e85c(pin, (OtuPoint*)&s->unk_50);
                s->unk_48 = _s32_div_f(s->unk_84 * 0x1800, s->unk_80);
                s->unk_44 = s->unk_48;

                s->unk_6C = s->unk_6C - data_ov039_0209a304;

                if (s->unk_6C < 0) {
                    s->unk_6C = 0;
                }

                s->unk_70 = s->unk_70 + data_ov039_0209a310;
                func_ov039_02098c00(s->unk_6C, (OtuPoint*)&s->unk_64, (OtuPoint*)&s->unk_58, (OtuPoint*)&s->unk_58);

                s->unk_60 = s->unk_60 + s->unk_70;

                // Wrap the accumulated offset back to zero, and rescale the spin
                // by the same signed factor when it is still positive.
                //
                // The rescale is a 64-bit multiply-and-reshift with a +0x800
                // rounding bias, not a divide: that is what produces the target's
                // smull / adds #0x800 / lsr #12 / orr sequence, and writing it as
                // `/ 0x1000` instead makes mwcc emit a call to __ll_sdiv.
                if (s->unk_60 > 0) {
                    s->unk_60 = 0;

                    if (s->unk_70 > 0) {
                        s->unk_70 = (s32)(((s64)s->unk_70 * -data_ov039_0209a300 + 0x800) >> 0xC);
                    }
                }

                Sprite_Update((Sprite*)s);
                s->unk_7C = 1;
            }
        }
    } else {
        // The pin is gone: stop, and do not draw this frame.
        s->unk_78 = 0;
        s->unk_7C = 0;
    }

    return 1;
}

/**
 * @brief The fourth pin-sprite's update stage.
 *
 * 0x02097c90. The same body as func_ov039_0209702c with 0x1800 replaced by
 * 0x1000 and the position read hoisted above the decrement; see the note above
 * the two together for why they are not one body.
 */
s32 func_ov039_02097c90(TaskPool* pool, Task* task, void* param) {
    OtuTaskSprite* s = (OtuTaskSprite*)task->data;
    s32            budget;
    void*          pin = EasyTask_GetTaskData(pool, s->unk_74);

    if (pin != NULL) {
        if (s->unk_78 != 0) {
            func_ov039_0208e85c(pin, (OtuPoint*)&s->unk_50);
            budget    = s->unk_84 - 1;
            s->unk_84 = budget;

            if (budget <= 0) {
                s->unk_78 = 0;
                s->unk_7C = 0;
            } else {
                s->unk_48 = _s32_div_f(budget * 0x1000, s->unk_80);
                s->unk_44 = s->unk_48;

                s->unk_6C = s->unk_6C - data_ov039_0209a304;

                if (s->unk_6C < 0) {
                    s->unk_6C = 0;
                }

                s->unk_70 = s->unk_70 + data_ov039_0209a310;
                func_ov039_02098c00(s->unk_6C, (OtuPoint*)&s->unk_64, (OtuPoint*)&s->unk_58, (OtuPoint*)&s->unk_58);

                s->unk_60 = s->unk_60 + s->unk_70;

                // Same rescale as the first task's: a 64-bit multiply-and-reshift
                // with a rounding bias, which is what the smull sequence is.
                if (s->unk_60 > 0) {
                    s->unk_60 = 0;

                    if (s->unk_70 > 0) {
                        s->unk_70 = (s32)(((s64)s->unk_70 * -data_ov039_0209a300 + 0x800) >> 0xC);
                    }
                }

                Sprite_Update((Sprite*)s);
                s->unk_7C = 1;
            }
        }
    } else {
        s->unk_78 = 0;
        s->unk_7C = 0;
    }

    return 1;
}

/* ------------------------------------------------------------------ */
/* The second pin-sprite's update stage: a four-state machine.         */
/* ------------------------------------------------------------------ */

/**
 * @brief The second pin-sprite's update stage, 0x02097528.
 *
 * The third member of the cluster -- it calls both func_ov039_0208e85c and
 * func_ov039_02098c00 like the two 300-byte siblings -- but it is a different
 * shape, not a fourth copy of them. Its blend runs only while the state word is
 * non-zero, and it blends in the *other* direction: the offset is a countdown
 * that is decremented by a fixed step rather than a scale that is bled down by a
 * constant, so this sprite closes on its target rather than easing toward it.
 *
 * The state machine over +0x70 is a genuine `switch` and not an if-chain: the
 * target tests against 3, jumps through a table, and has the four bodies out of
 * line in source order. That matters beyond the layout -- it is also what
 * suppresses if-conversion, which is why case 1 and case 3 test the same
 * bitfield and yet one branches to an out-of-line `bne` and the other writes its
 * result predicated with `moveq`/`streq`.
 *
 * The bitfield tested at +0x00 in cases 1 and 3 is the base game's `Sprite`'s
 * own first word; the target reaches it as `(u32)(w << 0x15) >> 0x1F`, i.e. bit
 * 10, which is `Sprite.isPlaying` in the header's layout.
 *
 * Both tests are written `== TRUE` rather than as bare conditions, and that is
 * not cosmetic: a one-bit field tested bare lets mwcc use the flags the
 * extraction's `lsr` already set and drop the compare altogether, and the target
 * keeps `cmp r0, #1`. Comparing against TRUE is what preserves it.
 */
s32 func_ov039_02097528(TaskPool* pool, Task* task, void* param) {
    OtuTaskSprite* s   = (OtuTaskSprite*)task->data;
    void*          pin = EasyTask_GetTaskData(pool, s->unk_68);

    if (pin != NULL) {
        if (s->unk_70 != 0) {
            func_ov039_0208e85c(pin, (OtuPoint*)&s->unk_40);

            if (s->unk_5C > 0) {
                s->unk_5C = s->unk_5C - s->unk_60;

                if (s->unk_5C < 0) {
                    s->unk_5C = 0;
                }
            }

            func_ov039_02098c00(s->unk_5C, (OtuPoint*)&s->unk_54, (OtuPoint*)&s->unk_48, (OtuPoint*)&s->unk_48);
            s->unk_6C = 1;
        }

        switch (s->unk_70) {
            case 0:
                s->unk_6C = 0;
                break;

            case 1:
                // Wait for the intro animation to end, then move to the hold.
                if (s->sprite.isPlaying == TRUE) {
                    Sprite_ChangeAnimation((Sprite*)s, s->sprite.animData, 8, s->sprite.cellTable);
                    s->unk_70 = 2;
                }
                Sprite_Update((Sprite*)s);
                break;

            case 2:
                // Hold for +0x74 frames, then move to the outro.
                s->unk_74 = s->unk_74 - 1;

                if (s->unk_74 <= 0) {
                    Sprite_ChangeAnimation((Sprite*)s, s->sprite.animData, 9, s->sprite.cellTable);
                    s->unk_70 = 3;
                }
                Sprite_Update((Sprite*)s);
                break;

            case 3:
                // Wait out the outro, then retire the sprite.
                if (s->sprite.isPlaying == TRUE) {
                    s->unk_70 = 0;
                    s->unk_6C = 0;
                }
                Sprite_Update((Sprite*)s);
                break;

            default:
                break;
        }
    } else {
        s->unk_6C = 0;
        s->unk_70 = 0;
    }

    return 1;
}

/**
 * @brief The third pin-sprite's update stage, 0x02097954.
 *
 * The shortest of the four updates. It runs the blend only while the sprite's
 * animation is still playing, and when the animation finishes it drops both the
 * "running" and "drawn" flags -- the sprite plays once, tracks its target for as
 * long as it lasts, and then stops without any of the countdown machinery the
 * other three carry.
 */
s32 func_ov039_02097954(TaskPool* pool, Task* task, void* param) {
    OtuTaskSprite* s   = (OtuTaskSprite*)task->data;
    void*          pin = EasyTask_GetTaskData(pool, s->unk_50);

    // Positively nested on purpose. Spelled as `if (pin == NULL) {...} else if`,
    // mwcc if-converts both clears into the branches above them and tail-merges
    // them into one block; the target emits the same four instructions twice,
    // out of line at the bottom, which is what the nested form produces.
    if (pin != NULL) {
        if (s->unk_54 != 0) {
            if (SpriteMgr_IsAnimationFinished((Sprite*)s) == 0) {
                func_ov039_0208e85c(pin, (OtuPoint*)&s->unk_40);
                Sprite_Update((Sprite*)s);
                s->unk_58 = 1;
            } else {
                s->unk_54 = 0;
                s->unk_58 = 0;
            }
        }
    } else {
        s->unk_54 = 0;
        s->unk_58 = 0;
    }

    return 1;
}

/* ------------------------------------------------------------------ */
/* The two 344-byte setup routines: the fourth pair of siblings.       */
/* ------------------------------------------------------------------ */

/*
 * 0x02097240 and 0x02097e9c are one body with four constants varied, and they
 * are the largest pair in the band. Both:
 *
 *   - raise the "running" flag;
 *   - preset the anchor words;
 *   - copy the creation block's two words into the sprite's start position;
 *   - draw two random values out of the base game's s16 pair table;
 *   - derive a speed from the starting spin and store it twice;
 *   - pick a starting animation at random out of a range.
 *
 * What varies:
 *
 *              02097240   02097e9c
 *   anchor        0x1800      0x1000     (1.5 vs 1.0 in Q12.12)
 *   anim range    0x0A ..     0x03 ..
 *                 + RNG(7)    + RNG(0xB)
 *
 * i.e. the same sprite, one covering 1.5x the ground and starting on a different
 * animation band.
 */

/**
 * @brief The signed magnitude of the starting spin, in Q12.12.
 *
 * `FX_Divide` here is the DS hardware divide, so this is a 20-bit-fraction
 * quotient scaled by three and shifted back to Q12.12.
 *
 * The macro form is load-bearing: it evaluates its argument three times, which is
 * exactly what the target does -- once for the sign test and once on whichever
 * side of it is taken. Written as a call to a helper it would evaluate once, and
 * the target would be two `bl`s instead of three inline expandings.
 */
#define OTU_SPIN_SPEED(s) ((FX_Divide((s)->unk_70, data_ov039_0209a310) * 3) >> 0xC)
#define OTU_ABS_SPEED(s)  (OTU_SPIN_SPEED(s) < 0 ? -OTU_SPIN_SPEED(s) : OTU_SPIN_SPEED(s))

// Nonmatching: 87.6%, eleven instructions out of 79, and all eleven are register
// allocation or load scheduling inside a body that is otherwise settled -- every
// field, offset, constant, call and branch edge is right.
//
// What is left, in order of size:
//
//   * the second word of the creation block is loaded one instruction later than
//     the target loads it, and into r2 rather than r1;
//   * the index arithmetic for the table pair uses r1 where the target uses r3
//     (and takes the pool word into r1 where the target takes it into r3).
//
// Tried and rejected: threading the two block words through locals (86.1%, worse
// -- mwcc then sinks the second load past all three stores).
//
// One thing here is worth recording because it was worth 33 points. These eight
// constants are declared `extern s32`, NOT `extern const s32`. Told a symbol is
// const, mwcc keeps its value in a callee-saved register across the FX_Divide
// call -- here and in func_ov039_0209702c's bleed-down block alike -- where the
// target reloads it from the literal pool every time. Declaring them const cost
// 02097240 33 points (54.8% -> 87.6%) and kept 0209702c and 02097c90 off 100%.
void func_ov039_02097240(OtuTaskSprite* s, OtuTaskParams* param) {
    s32 index;

    s->unk_78 = 1;
    s->unk_44 = 0x1800;
    s->unk_48 = 0x1800;
    s->unk_58 = param->word0;
    s->unk_5C = param->word1;
    s->unk_60 = 0;

    // The starting count is jittered about data_ov039_0209a314 (0x3000) by up to
    // the difference to 0x0209a328, and the starting spin negatively about
    // 0x0200 by up to the difference to 0x4000.
    s->unk_6C = data_ov039_0209a314;
    s->unk_6C = s->unk_6C + RNG_Next(data_ov039_0209a328 - data_ov039_0209a314);
    s->unk_70 = 0 - data_ov039_0209a2fc;
    s->unk_70 = s->unk_70 - RNG_Next(data_ov039_0209a30c - data_ov039_0209a2fc);

    // The speed is the magnitude of that spin; stored twice, once as the divisor
    // the update stage divides by and once as the remaining budget.
    s->unk_80 = OTU_ABS_SPEED(s);
    s->unk_84 = s->unk_80;

    // A random adjacent pair out of the base game's table, taken as a direction.
    index     = RNG_Next(0x10000) >> 4;
    s->unk_64 = *(s16*)((u8*)&data_0205e4e0 + (index + 1) * 2);
    s->unk_68 = *(s16*)((u8*)&data_0205e4e0 + index * 2);

    Sprite_ChangeAnimation((Sprite*)s, s->sprite.animData, (s16)(RNG_Next(7) + 0xA), s->sprite.cellTable);
}

// Nonmatching: 87.6%, the same eleven instructions as func_ov039_02097240 and for
// the same reason -- both bodies are identical apart from four constants, so this
// is one mwcc behaviour reached twice, not two misreadings. See the note above.
void func_ov039_02097e9c(OtuTaskSprite* s, OtuTaskParams* param) {
    s32 index;

    s->unk_78 = 1;
    s->unk_44 = 0x1000;
    s->unk_48 = 0x1000;
    s->unk_58 = param->word0;
    s->unk_5C = param->word1;
    s->unk_60 = 0;
    s->unk_6C = data_ov039_0209a314;
    s->unk_6C = s->unk_6C + RNG_Next(data_ov039_0209a328 - data_ov039_0209a314);
    s->unk_70 = 0 - data_ov039_0209a2fc;
    s->unk_70 = s->unk_70 - RNG_Next(data_ov039_0209a30c - data_ov039_0209a2fc);
    s->unk_80 = OTU_ABS_SPEED(s);
    s->unk_84 = s->unk_80;
    index     = RNG_Next(0x10000) >> 4;
    s->unk_64 = *(s16*)((u8*)&data_0205e4e0 + (index + 1) * 2);
    s->unk_68 = *(s16*)((u8*)&data_0205e4e0 + index * 2);
    Sprite_ChangeAnimation((Sprite*)s, s->sprite.animData, (s16)(RNG_Next(0xB) + 3), s->sprite.cellTable);
}

#undef OTU_ABS_SPEED
#undef OTU_SPIN_SPEED

/* ------------------------------------------------------------------ */
/* The _Sprite_Load wrappers.                                          */
/* ------------------------------------------------------------------ */

/*
 * Three routines with one body: copy a 0x2C-byte `SpriteAnimation` template onto
 * the stack, stamp it with this object's owner, its `dataType` and its anchor,
 * and load one sprite.
 *
 * The template copy is the `ldm r5!, {r0..r3} / stm r4!, {r0..r3}` triple out of
 * a literal pool, which is mwcc's rendering of a *local array initialiser* -- not
 * of a read from a `static const` object, which it would constant-fold instead.
 * So each wrapper declares its template as a local brace initialiser, and the
 * 0x2C bytes have to be spelled out. They are transcribed from the target's own
 * templates at 0x02099f68, 0x02099fb0 and 0x02099ff8; the two that differ are
 * the `frameInfoCallback` (each wrapper's own cell builder) and six of the small
 * trailing fields.
 *
 * The templates themselves are *not* emitted as named objects. A declared
 * `static const SpriteAnimation` would be emitted into this object's `.rodata`,
 * and this translation unit has no `.rodata` claim in the overlay's delinks --
 * the range is gap-filled from the original overlay -- so the linker would place
 * it at an address of its own choosing and overwrite the target's bytes. The
 * local-initialiser form keeps the bytes in an anonymous template instead, which
 * costs nothing in the ROM because the copy is immediate.
 */

// Nonmatching: 99.9%, one row -- the literal pool word. mwcc names the template
// `@NNNN` where the target's delinked object names it `data_ov039_02099f68`; the
// bytes behind the two are identical. This is the relocation-naming case the
// objdiff skill lists as ignorable, and it is the price of making the template a
// local initialiser rather than an `extern`: see the section note above.
void func_ov039_02097454(OtuTaskSprite* self, Sprite* sprite, s32* dataType) {
    SpriteAnimation anim = {
        /* 0x00 */ {2, 0, 0, 5, 0, 2, 0}, // 0x2282
        /* 0x02 */
         {0},
        /* 0x04 */
         0x50, // posX
        /* 0x06 */ 0x50, // posY
        /* 0x08 */ (SpriteFrameInfoCallback)func_ov039_02097398,
        /* 0x0C */ 0, // callbackArg
        /* 0x10 */ NULL, // owner
        /* 0x14 */ &data_ov039_0209a0dc,
        /* 0x18 */ 2,
        /* 0x1A */ 8,
        /* 0x1C */ 1,
        /* 0x1E */ 0,
        /* 0x20 */ 4,
        /* 0x22 */ 2,
        /* 0x24 */ 0,
        /* 0x26 */ 2,
        /* 0x28 */ 3,
        /* 0x2A */ 4  // animIndex
    };

    anim.owner    = self;
    anim.dataType = (u16)*dataType;
    anim.posX     = (s16)(self->unk_48 >> 0xC);
    anim.posY     = (s16)(self->unk_4C >> 0xC);
    _Sprite_Load(sprite, &anim);
}

/**
 * @brief The second pin-sprite's sprite load, 0x0209788c.
 *
 * The same body as func_ov039_02097454 over a template that differs only in its
 * cell builder (func_ov039_020977d0), four of the trailing halfwords, and its
 * final `animIndex`.
 */
// Nonmatching: 99.9%, one row -- the literal pool word. mwcc gives the template
// an anonymous name (`@NNNN`) where the target's delinked object names it
// `data_ov039_02099fb0`; the two point at identical bytes. This is the
// relocation-naming case the objdiff skill lists as ignorable, and it is why
// the template is a local initialiser rather than a referenced `extern`: see the
// section note above. Every instruction matches.
void func_ov039_0209788c(OtuTaskSprite* self, Sprite* sprite, s32* dataType) {
    SpriteAnimation anim = {
        /* 0x00 */ {2, 0, 0, 5, 0, 2, 0}, // 0x2282
        /* 0x02 */
         {0},
        /* 0x04 */
         0x50, // posX
        /* 0x06 */ 0x50, // posY
        /* 0x08 */ (SpriteFrameInfoCallback)func_ov039_020977d0,
        /* 0x0C */ 0,
        /* 0x10 */ NULL,
        /* 0x14 */ &data_ov039_0209a0dc,
        /* 0x18 */ 2,
        /* 0x1A */ 5,
        /* 0x1C */ 1,
        /* 0x1E */ 0,
        /* 0x20 */ 4,
        /* 0x22 */ 1,
        /* 0x24 */ 0,
        /* 0x26 */ 2,
        /* 0x28 */ 3,
        /* 0x2A */ 1
    };

    anim.owner    = self;
    anim.dataType = (u16)*dataType;
    anim.posX     = (s16)(self->unk_48 >> 0xC);
    anim.posY     = (s16)(self->unk_4C >> 0xC);
    _Sprite_Load(sprite, &anim);
}

/**
 * @brief The fourth pin-sprite's sprite load, 0x02097ba4.
 *
 * The same body again. Its template's first word is 0x1282 rather than 0x2282 --
 * `bits_12_13` is 1 instead of 2 -- and it reads its anchor from the other pair
 * of words, +0x58/+0x5C, matching the first and fourth tasks' position block.
 */
// Nonmatching: 99.9%, one row -- the literal pool word naming, as above.
void func_ov039_02097ba4(OtuTaskSprite* self, Sprite* sprite, s32* dataType) {
    SpriteAnimation anim = {
        /* 0x00 */ {2, 0, 0, 5, 0, 1, 0}, // 0x1282
        /* 0x02 */
         {0},
        /* 0x04 */
         0x50, // posX
        /* 0x06 */ 0x50, // posY
        /* 0x08 */ (SpriteFrameInfoCallback)func_ov039_02097ae0,
        /* 0x0C */ 0,
        /* 0x10 */ NULL,
        /* 0x14 */ &data_ov039_0209a0dc,
        /* 0x18 */ 2,
        /* 0x1A */ 7,
        /* 0x1C */ 1,
        /* 0x1E */ 0,
        /* 0x20 */ 4,
        /* 0x22 */ 1,
        /* 0x24 */ 0,
        /* 0x26 */ 2,
        /* 0x28 */ 3,
        /* 0x2A */ 3
    };

    anim.owner    = self;
    anim.dataType = (u16)*dataType;
    anim.posX     = (s16)(self->unk_58 >> 0xC);
    anim.posY     = (s16)(self->unk_5C >> 0xC);
    _Sprite_Load(sprite, &anim);
}

/* ------------------------------------------------------------------ */
/* The four stage dispatchers.                                         */
/* ------------------------------------------------------------------ */

/*
 * One body, four times: index the task's four-entry stage table and call it.
 *
 * This is the task's `TaskHandle.taskFunc`, and it is why the sixteen stages all
 * share one signature -- the table is what the engine walks. The frame is what
 * tells the story: `sub sp, #0x10` plus `add ip, sp, #0` and the
 * `ldm lr, {r0..r3} / stm ip, {r0..r3}` pair is mwcc rendering a four-pointer
 * *local array initialiser*, not a read from `extern const TaskStages`. So each
 * dispatcher writes its table out as a brace initialiser even though the same
 * bytes also exist in the overlay's gap-filled `.rodata` at
 * 0x02099f10/0x02099f58/0x02099fa0/0x02099fe8 -- the `extern` declarations for
 * those are above and are what the *relocation names* would otherwise have
 * pointed at. An anonymous template costs one relocation-name row; a referenced
 * `extern` would move the bytes.
 *
 * The first table's init entry, 0x02096fcc, lives below this band and is declared
 * here without a prototype so this file cannot disagree with whoever defines it.
 */
s32 func_ov039_02096fcc();

/** The first pin-sprite's stage dispatcher. */
// Nonmatching: 99.4%, one row -- the literal pool word naming the anonymous
// template (`@NNNN`) where the target names it `data_ov039_02099f10`. Same bytes,
// same instruction; see the section note. Every instruction matches.
s32 func_ov039_020971c4(TaskPool* pool, Task* task, void* param, s32 stage) {
    OtuTaskSpriteStage stages[4] = {func_ov039_02096fcc, func_ov039_0209702c, func_ov039_02097160, func_ov039_020971b0};

    return stages[stage](pool, task, param);
}

/** The second pin-sprite's stage dispatcher. */
// Nonmatching: 99.3%, one row -- the literal pool word naming, as above.
s32 func_ov039_020976d4(TaskPool* pool, Task* task, void* param, s32 stage) {
    OtuTaskSpriteStage stages[4] = {func_ov039_020974e0, func_ov039_02097528, func_ov039_02097670, func_ov039_020976c0};

    return stages[stage](pool, task, param);
}

/** The third pin-sprite's stage dispatcher. */
// Nonmatching: 99.4%, one row -- the literal pool word naming, as above.
s32 func_ov039_02097a28(TaskPool* pool, Task* task, void* param, s32 stage) {
    OtuTaskSpriteStage stages[4] = {func_ov039_02097918, func_ov039_02097954, func_ov039_020979cc, func_ov039_02097a14};

    return stages[stage](pool, task, param);
}

/** The fourth pin-sprite's stage dispatcher. */
// Nonmatching: 99.3%, one row -- the literal pool word naming, as above.
s32 func_ov039_02097e20(TaskPool* pool, Task* task, void* param, s32 stage) {
    OtuTaskSpriteStage stages[4] = {func_ov039_02097c30, func_ov039_02097c90, func_ov039_02097dbc, func_ov039_02097e0c};

    return stages[stage](pool, task, param);
}

/* ------------------------------------------------------------------ */
/* The two remaining public entry points.                              */
/* ------------------------------------------------------------------ */

/**
 * @brief The second pin-sprite's setup, 0x02097750.
 *
 * The only entry point in the band taking six arguments, and the only one that
 * takes its starting direction from a caller-supplied selector rather than from
 * the RNG: `selector >> 4` indexes the same base-game `s16` pair table the setup
 * routines pick at random, so this one is the same sprite with the direction
 * handed in rather than rolled.
 *
 * The stack arguments are load-bearing. `push {r3, r4, r5, lr}` is a sixteen-byte
 * frame, so the fifth and sixth arguments land at sp+0x10 and sp+0x14 -- a
 * prototype that dropped them would put the reads four bytes out.
 */
// Nonmatching: 46.3%. Same instruction count as the target (32 each), and the
// last eleven -- the two `ldrsh`s' *results*, the two stack-argument loads, the
// `Sprite_ChangeAnimation` call and `+0x70 = 1` -- all match. What differs is
// entirely the scheduler's ordering of the first twenty-one.
//
// The target interleaves the stores with the loads in a way that minimises live
// ranges by hand: `+0x5C` and `+0x60` are stored, *then* the first `ldrsh`, then
// `+0x64`, then the second `ldrsh`. mwcc instead sinks the two block-word loads
// to the top and hoists both `ldrsh`s above the `+0x64` store, which shifts
// everything by a few positions.
//
// Tried and rejected, all landing within a point of this: threading the two block
// words through locals, hoisting the two table addresses into `s16*` locals, and
// typing the block as `OtuTaskParams*` rather than `s32*`. Every field, offset,
// constant and argument position is settled; this is mwcc's block scheduler and
// the source cannot ask for a different one.
void func_ov039_02097750(OtuTaskSprite* s, OtuTaskParams* param, s32 selector, s32 closeBy, s32 step, s32 holdFor) {
    s32 index = selector >> 4;

    s->unk_48 = param->word0;
    s->unk_4C = param->word1;
    s->unk_50 = 0;
    s->unk_5C = closeBy;
    s->unk_60 = step;
    s->unk_64 = 0;
    s->unk_54 = *(s16*)((u8*)&data_0205e4e0 + (index + 1) * 2);
    s->unk_58 = *(s16*)((u8*)&data_0205e4e0 + index * 2);
    s->unk_74 = holdFor;
    Sprite_ChangeAnimation((Sprite*)s, s->sprite.animData, 7, s->sprite.cellTable);
    s->unk_70 = 1;
}

/**
 * @brief The third pin-sprite's setup, 0x02097aa4.
 *
 * A tail call, and the target says so: `ldr ip, .L_Sprite_SetAnimation` followed
 * by `bx ip`, with no frame at all. mwcc emits that when the call is the last
 * thing the function does.
 *
 * Note it is `Sprite_SetAnimation`, not `Sprite_ChangeAnimation` as the other
 * three setups use -- the two differ in the base game by the animation restart
 * `Sprite_ChangeAnimation` performs, and this one does not restart.
 */
// Nonmatching: 68.3%, four instructions out of 13, all scheduling.
//
// The `lsl r2, r2, #0x10` / `asr r2, r2, #0x10` pair is present in both -- that is
// the third argument truncated to `s16`, and it is why the parameter is typed
// `s32` here: declared `s16`, mwcc knows the value is already in range and drops
// the pair entirely (59.5%). Where the two halves land differs: the target keeps
// them together after the two stores, mwcc sinks the shift up above them. The
// pool word is likewise taken a slot earlier.
void func_ov039_02097aa4(OtuTaskSprite* s, OtuTaskParams* param, s32 animation) {
    s->unk_54 = 1;
    s->unk_48 = param->word0;
    s->unk_4C = param->word1;
    Sprite_SetAnimation((Sprite*)s, s->sprite.animData, (s16)animation, s->sprite.cellTable);
}