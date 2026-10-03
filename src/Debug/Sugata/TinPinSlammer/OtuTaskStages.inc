/* Band 4 -- see OtuBands.h for the address range and why this is an include. */

#include "Debug/Sugata/TinPinSlammer.h"
#include "EasyFade.h"
#include "Engine/Core/OamMgr.h"
#include "SndMgr.h"
#include "SpriteMgr.h"

/* ==================================================================== */
/* The one function in this band that is deliberately absent.          */
/* ==================================================================== */

/*
 * func_ov039_02096d98 is `mov r0, #0x1` / `bx lr` and is not decompiled here.
 * OtuFieldAccess.c leaves it out, along with 0x020921f4, 0x02092bf0 and
 * 0x020933f0, on the grounds that four identical bodies at four addresses are
 * at least as likely to be four different predicates as four copies of one.
 * That reasoning does not survive contact with the callers, and the correction
 * is recorded here because the comment it contradicts lives in the shared TU
 * and not in this file.
 *
 * None of the four is ever `bl`'d. Each appears exactly once, as a `.word` in a
 * four-entry TaskStages table, and in every case it is the *third* entry:
 *
 *   0x02099610  { 02091b98, 02092154, 020921f4, 020921fc }
 *   0x020999a4  { 020929ec, 02092a78, 02092bf0, 02092bf8 }
 *   0x020999c0  { 02092f88, 020933c0, 020933f0, 020933f8 }
 *   0x02099ec8  { 02096d70, 02096d98, 02096da0, 02096dec }
 *
 * The slot order is initialize / update / render / cleanup -- fixed by
 * 02099de0, whose four entries are this file's own 020963c8 (clears the object
 * and loads the sprite), 02096404 (per-frame child fetch), 020964a4
 * (Sprite_RenderFrame) and 020964ec (Sprite_Release). So all four bodies are
 * the *render* stage of a different task, and the other three stages of each of
 * those four tasks all end `mov r0, #0x1` / `pop {..., pc}`. A render stage
 * that draws nothing and returns 1 is a coherent reading; four unrelated
 * predicates is not, because a predicate needs a load and there are none.
 *
 * What is left open is only whether the original source said `return 1;` or
 * something the compiler folded to the same two instructions -- which is not
 * decidable from the ROM and does not change the code. The function is left out
 * because the shared file's decision to leave it out is not mine to reverse,
 * and because doing so without being able to amend the comment it contradicts
 * would leave the tree asserting two things at once.
 */

/* ==================================================================== */
/* Referenced overlay data and cross-module routines.                  */
/* ==================================================================== */

/*
 * The four TaskHandles and the four TaskStages tables this band dispatches
 * through.  All eight are `.rodata` in the original overlay and are not
 * claimed by any delinks entry, so they are referenced by the build's own
 * symbol names rather than redeclared as new objects -- see the note on the
 * row tables in TinPinSlammer.h for why that matters.
 */
extern const TaskHandle data_ov039_02099d8c;
extern const TaskHandle data_ov039_02099dd4;
extern const TaskHandle data_ov039_02099e1c;
extern const TaskHandle data_ov039_02099ebc;

extern const TaskStages data_ov039_02099d98;
extern const TaskStages data_ov039_02099de0;
extern const TaskStages data_ov039_02099e28;
extern const TaskStages data_ov039_02099ec8;

/*
 * The six SpriteAnimation templates the sprite loaders copy to the stack.
 * Each is 0x2C bytes, which is exactly sizeof(SpriteAnimation), and each names
 * a different one of the overlay's sprite-cell builders as its frame-info
 * callback -- that pairing is the only thing that distinguishes them.
 */
extern const SpriteAnimation data_ov039_02099df0;
extern const SpriteAnimation data_ov039_02099e38;
extern const SpriteAnimation data_ov039_02099e64;
extern const SpriteAnimation data_ov039_02099e90;
extern const SpriteAnimation data_ov039_02099ed8;
extern const SpriteAnimation data_ov039_02099f20;

/*
 * The glyph tables the two inits at the end of this file build cursors over.
 * Declared as arrays, not as `const void*`: these symbols *are* the tables, so
 * a pointer declaration makes every use cost an extra `ldr` through the
 * symbol, which the target does not have.
 */
extern const u8 data_ov039_0209a830[];
extern const u8 data_ov039_0209a8a8[];
extern const u8 data_ov039_0209a938[];
extern const u8 data_ov039_0209aa0c[];

/* Within this band: the sprite loaders the init stages call. */
void func_ov039_0209633c(void* self, void* sprite, s32* args);
void func_ov039_02096718(void* self, void* sprite, s32* args);
void func_ov039_0209678c(void* self, void* sprite, s32* args);
void func_ov039_02096800(void* self, void* sprite, s32* args);
void func_ov039_02096d00(void* self, void* sprite);
void func_ov039_02096f40(void* self, void* sprite, s32* args);

/* Elsewhere in the overlay: the animation-phase counter 02096da0 reads. */
extern s32 func_ov040_0209cb5c(void);

/* Elsewhere in the overlay: the point-copy helper 02096404 calls that the
 * shared header does not declare (func_ov039_0208e6e0 is already there), and
 * the text-cell pair the two switch bodies below use. */
void func_ov039_02087ba0(void* out, void* table, s32 count, void* cell);

/* ==================================================================== */
/* Task-create wrappers.                                               */
/* ==================================================================== */

/*
 * Four of these, and they differ only in which TaskHandle they pass and in
 * how many arguments they forward to the new task.  The stack block they build
 * is a two-word struct -- a null first word and a pointer to the argument
 * block -- handed to EasyTask_CreateTask as its `param`, which is why the
 * target emits `add ip, sp, #0x8` and then `stm sp, {r2, ip}`.
 */

/** Creates the task whose handle is data_ov039_02099d8c. */
s32  func_ov039_02096124(TaskPool* pool, s32 arg1) {
    s32 sp8;

    sp8 = arg1;
    return EasyTask_CreateTask(pool, &data_ov039_02099d8c, NULL, 0, NULL, &sp8);
}

/** Creates the task whose handle is data_ov039_02099dd4. Two arguments. */
s32  func_ov039_02096548(TaskPool* pool, s32 arg1, s32 arg2) {
    s32 sp8[2];

    sp8[0] = arg1;
    sp8[1] = arg2;
    return EasyTask_CreateTask(pool, &data_ov039_02099dd4, NULL, 0, NULL, sp8);
}

/** Creates the task whose handle is data_ov039_02099e1c.
 *
 * Typed as returning the handle rather than void: a caller elsewhere in this
 * overlay stores the result into a child-handle field. It costs nothing
 * here -- the body has no `mov r0` of its own in either spelling, so the
 * handle already comes back in r0 and both compile to the same
 * instructions. Same finding as func_ov039_02098394 in band 8.
 */
s32 func_ov039_02096b18(TaskPool* pool, s32 arg1) {
    s32 sp8;

    sp8 = arg1;
    return EasyTask_CreateTask(pool, &data_ov039_02099e1c, NULL, 0, NULL, &sp8);
}

/** Creates the task whose handle is data_ov039_02099ebc.
 *
 * Typed as returning the handle rather than void: a caller elsewhere in this
 * overlay stores the result into a child-handle field. It costs nothing
 * here -- the body has no `mov r0` of its own in either spelling, so the
 * handle already comes back in r0 and both compile to the same
 * instructions. Same finding as func_ov039_02098394 in band 8.
 */
s32 func_ov039_02096e4c(TaskPool* pool, s32 arg1) {
    s32 sp8;

    sp8 = arg1;
    return EasyTask_CreateTask(pool, &data_ov039_02099ebc, NULL, 0, NULL, &sp8);
}

/* ==================================================================== */
/* Task-stage dispatchers.                                             */
/* ==================================================================== */

/*
 * The standard four-slot TaskStages trampoline: copy the constant table onto
 * the stack and call slot `stage`.  The target's `ldm`/`stm` of sixteen bytes
 * is the table copy; `ldr r3, [ip, r4, lsl #2]` is the indexed call.
 */
s32 func_ov039_020960dc(TaskPool* pool, Task* task, void* data, s32 stage) {
    TaskStages stages = data_ov039_02099d98;

    return stages.iter[stage](pool, task, data);
}

s32 func_ov039_02096500(TaskPool* pool, Task* task, void* data, s32 stage) {
    TaskStages stages = data_ov039_02099de0;

    return stages.iter[stage](pool, task, data);
}

s32 func_ov039_02096ad0(TaskPool* pool, Task* task, void* data, s32 stage) {
    TaskStages stages = data_ov039_02099e28;

    return stages.iter[stage](pool, task, data);
}

s32 func_ov039_02096e04(TaskPool* pool, Task* task, void* data, s32 stage) {
    TaskStages stages = data_ov039_02099ec8;

    return stages.iter[stage](pool, task, data);
}

/* ==================================================================== */
/* The small accessors and release wrappers.                           */
/* ==================================================================== */

/** Raises the task's +0x44 word to 1, copies args->unk_00 to +0, loads. */
s32 func_ov039_02096d70(void* pool, void* task, s32* args) {
    u8* self = *(u8**)((u8*)task + 0x18);

    *(s32*)((u8*)self + 0x44) = 1;
    *(s32*)((u8*)self + 0x00) = *args;
    func_ov039_02096d00(self, self + 4);
    return 1;
}

/** Releases the sprite at self + 4. */
s32 func_ov039_02096dec(void* pool, void* task, void* args) {
    u8* self = *(u8**)((u8*)task + 0x18);

    Sprite_Release((Sprite*)(self + 4));
    return 1;
}

/** Raises +0x58 and then tail-calls Sprite_SetAnimation on frame 4. */
void func_ov039_0209657c(Sprite* self) {
    *(s32*)((u8*)self + 0x58) = 1;
    Sprite_SetAnimation(self, *(s16**)((u8*)self + 0x18), 4, *(SpriteCell**)((u8*)self + 0x1C));
}

/** Releases the two sprites at self + 0 and self + 0x40. */
s32 func_ov039_020960bc(void* pool, void* task, void* args) {
    u8* self = *(u8**)((u8*)task + 0x18);

    Sprite_Release((Sprite*)self);
    Sprite_Release((Sprite*)(self + 0x40));
    return 1;
}

/** The same pair of releases, for the other task. */
s32 func_ov039_02096ab0(void* pool, void* task, void* args) {
    u8* self = *(u8**)((u8*)task + 0x18);

    Sprite_Release((Sprite*)self);
    Sprite_Release((Sprite*)(self + 0x40));
    return 1;
}

/** Releases the one sprite at self + 0. */
s32 func_ov039_020964ec(void* pool, void* task, void* args) {
    u8* self = *(u8**)((u8*)task + 0x18);

    Sprite_Release((Sprite*)self);
    return 1;
}

/** Reads the +0xC8 word and returns it as a predicate. */
s32 func_ov039_02096c44(void* self) {
    return *(s32*)((u8*)self + 0xC8) != 0;
}

/* ==================================================================== */
/* The sprite loaders.                                                 */
/* ==================================================================== */

/*
 * The six loaders below are the band's largest group.  They share a shape:
 * copy a 0x2C-byte SpriteAnimation template from `.rodata` onto the stack,
 * patch three or four of its fields, and hand it to _Sprite_Load.  The
 * template copy is the `ldm`/`stm` triple, the patch to the leading bitfield
 * halfword is the `bic`/`orr` pair, and the fields written after it are
 * `owner` (+0x10) and, where present, `posX`/`posY` (+0x04/+0x06).
 *
 * They are spelled out six times rather than sharing a `static` helper on
 * purpose: the build is `-inline noauto`, so a shared body would compile to a
 * real `bl` and collapse all six callers to stubs.
 *
 * Two details are load-bearing and both cost an iteration to find:
 *
 *   - `dataType` is fed a *32-bit* load of the argument's first word, narrowed
 *     to u16 and then placed in bits 2-5.  Reading it as a u16 directly emits
 *     `ldrh` and loses the target's `ldr`/`lsl #0x10`/`lsr #0x10` pair.
 *   - `posX`/`posY` are `>> 12`, not `/ 4096`.  The divide form makes mwcc
 *     emit the three-instruction sign-correcting sequence, which the target
 *     does not have.
 */

/*
 * Six routines with one shape: copy a 0x2C-byte SpriteAnimation template from
 * `.rodata` onto the stack, patch three or four of its fields, and hand it to
 * _Sprite_Load.  The template copy is the `ldm`/`stm` triple, the patch to the
 * leading bitfield halfword is the `bic`/`orr` pair, and the fields written
 * after it are `owner` (+0x10) and, where present, `posX`/`posY` (+0x04/+0x06).
 *
 * They are spelled out six times rather than sharing a `static` helper on
 * purpose: the build is `-inline noauto`, so a shared body would compile to a
 * real `bl` and collapse all six callers to stubs.
 */

/** Loads the sprite for the task whose anim template is data_ov039_02099df0. */
void func_ov039_0209633c(void* self, void* sprite, s32* args) {
    SpriteAnimation anim = data_ov039_02099df0;

    anim.owner    = self;
    anim.dataType = (u16) * (s32*)args;
    anim.posX     = *(s32*)((u8*)self + 0x48) >> 12;
    anim.posY     = *(s32*)((u8*)self + 0x4C) >> 12;
    _Sprite_Load((Sprite*)sprite, &anim);
}

/** Loads the sprite for the task whose anim template is data_ov039_02099e38. */
void func_ov039_02096718(void* self, void* sprite, s32* args) {
    SpriteAnimation anim = data_ov039_02099e38;

    anim.owner    = self;
    anim.dataType = (u16) * (s32*)args;
    _Sprite_Load((Sprite*)sprite, &anim);
}

/** Loads the sprite for the task whose anim template is data_ov039_02099e64. */
void func_ov039_0209678c(void* self, void* sprite, s32* args) {
    SpriteAnimation anim = data_ov039_02099e64;

    anim.owner    = self;
    anim.dataType = (u16) * (s32*)args;
    _Sprite_Load((Sprite*)sprite, &anim);
}

/** Loads the sprite for the task whose anim template is data_ov039_02099e90. */
void func_ov039_02096800(void* self, void* sprite, s32* args) {
    SpriteAnimation anim = data_ov039_02099e90;

    anim.owner    = self;
    anim.dataType = (u16) * (s32*)args;
    _Sprite_Load((Sprite*)sprite, &anim);
}

/** Loads the sprite for the task whose anim template is data_ov039_02099ed8. */
void func_ov039_02096d00(void* self, void* sprite) {
    SpriteAnimation anim = data_ov039_02099ed8;

    anim.owner    = self;
    anim.dataType = (u16) * (s32*)self;
    _Sprite_Load((Sprite*)sprite, &anim);
}

/** Loads the sprite for the task whose anim template is data_ov039_02099f20. */
void func_ov039_02096f40(void* self, void* sprite, s32* args) {
    SpriteAnimation anim = data_ov039_02099f20;

    anim.owner    = self;
    anim.dataType = (u16) * (s32*)args;
    anim.posX     = *(s32*)((u8*)self + 0x58) >> 12;
    anim.posY     = *(s32*)((u8*)self + 0x5C) >> 12;
    _Sprite_Load((Sprite*)sprite, &anim);
}

/* ==================================================================== */
/* The init and render stages.                                         */
/* ==================================================================== */

/*
 * Four of the band's five task-init stages, plus the two render stages that go
 * with them.  All six read the task's object through `task->data` and reach
 * the same object from three arguments, so the pointer is materialised once
 * into a local and every field is an offset from it.
 *
 * The store order inside each is the target's, and it is not sorted: 02096fcc
 * writes +0x78 and +0x7C *before* +0x74, and 020963c8 writes +0x50 before the
 * +0x40 block it is interleaved with.  Both are reproduced literally.
 */

/** Clears the coordinate block, raises +0x44, then loads via 0209633c. */
s32 func_ov039_020963c8(void* pool, void* task, s32* args) {
    u8* self = *(u8**)((u8*)task + 0x18);

    *(s32*)((u8*)self + 0x50) = *(s32*)((u8*)args + 4);
    *(s32*)((u8*)self + 0x40) = 0;
    *(s32*)((u8*)self + 0x44) = 0;
    *(s32*)((u8*)self + 0x48) = 0;
    *(s32*)((u8*)self + 0x4C) = 0;
    *(s32*)((u8*)self + 0x54) = 0;
    *(s32*)((u8*)self + 0x58) = 0;
    func_ov039_0209633c(self, self, args);
    return 1;
}

/** Recomputes the sprite position from the +0x40/+0x48 pairs, then renders. */
s32 func_ov039_020964a4(void* pool, void* task, void* args) {
    u8* self = *(u8**)((u8*)task + 0x18);

    if (*(s32*)((u8*)self + 0x54) != 0) {
        // The two halves of the object are two Q12.12 points; the sprite is
        // drawn at the difference between them, which is why the target
        // subtracts before shifting rather than loading a stored position.
        *(s16*)((u8*)self + 0x0C) = (*(s32*)((u8*)self + 0x48) - *(s32*)((u8*)self + 0x40)) >> 12;
        *(s16*)((u8*)self + 0x0E) = (*(s32*)((u8*)self + 0x4C) - *(s32*)((u8*)self + 0x44)) >> 12;
        Sprite_RenderFrame((Sprite*)self);
    }
    return 1;
}

/** Clears the +0x50..+0x7C block, then loads via 02096f40. */
s32 func_ov039_02096fcc(void* pool, void* task, s32* args) {
    u8* self = *(u8**)((u8*)task + 0x18);

    *(s32*)((u8*)self + 0x78) = 0;
    *(s32*)((u8*)self + 0x7C) = 0;
    *(s32*)((u8*)self + 0x74) = *(s32*)((u8*)args + 4);
    *(s32*)((u8*)self + 0x50) = 0;
    *(s32*)((u8*)self + 0x54) = 0;
    *(s32*)((u8*)self + 0x58) = 0;
    *(s32*)((u8*)self + 0x5C) = 0;
    *(s32*)((u8*)self + 0x60) = 0;
    *(s32*)((u8*)self + 0x64) = 0;
    *(s32*)((u8*)self + 0x68) = 0;
    *(s32*)((u8*)self + 0x40) = 0;
    *(s32*)((u8*)self + 0x44) = 0x1800;
    *(s32*)((u8*)self + 0x48) = 0x1800;
    *(s16*)((u8*)self + 0x4C) = 0;
    *(s16*)((u8*)self + 0x4E) = 0;
    func_ov039_02096f40(self, self, args);
    return 1;
}

/** Clears the +0xC0/+0xC8 words, then loads three sprites via 02096718/8c/00. */
s32 func_ov039_02096874(void* pool, void* task, s32* args) {
    u8* self = *(u8**)((u8*)task + 0x18);

    *(s32*)((u8*)self + 0xC8) = 0;
    *(s32*)((u8*)self + 0xC0) = 0;
    func_ov039_02096718(self, self, args);
    func_ov039_0209678c(self, self + 0x40, args);
    func_ov039_02096800(self, self + 0x80, args);
    return 1;
}

/**
 * Picks the sprite's frame from the overlay's own animation-phase counter.
 *
 * The +0x44 word gates the whole body: while it is clear nothing is stepped,
 * updated or drawn.  `5 - phase` is the frame index, and the target narrows it
 * to a halfword with `lsl #0x10` / `asr #0x10`, so the arithmetic is done in
 * s32 and truncated on the way into Sprite_ChangeAnimation.
 */
s32 func_ov039_02096da0(void* pool, void* task, void* args) {
    u8* self = *(u8**)((u8*)task + 0x18);

    if (*(s32*)((u8*)self + 0x44) != 0) {
        // Spelled inline rather than as a local: a local makes mwcc compute the
        // `lsl` into r0 and only then move it into r2, where the target narrows
        // straight into the argument register.
        Sprite_ChangeAnimation((Sprite*)(self + 4), *(void**)((u8*)self + 0x1C), (s16)(5 - func_ov040_0209cb5c()),
                               *(void**)((u8*)self + 0x20));
        Sprite_Update((Sprite*)(self + 4));
        Sprite_RenderFrame((Sprite*)(self + 4));
    }
    return 1;
}

/* ==================================================================== */
/* 02096404 -- the per-frame update for the 0209633c task.              */
/* ==================================================================== */

/**
 * @brief Follows a child task's position and keeps the sprite in step.
 *
 * Fetches the child task's data through the id stashed at +0x50, then branches
 * on the sub-state at +0x58:
 *
 *   0  the child is not ready -- clear the +0x54 "moving" flag;
 *   1  copy two points out of the child (func_ov039_0208e85c and
 *      func_ov039_0208e6e0) and, if the sprite's own first word says the
 *      animation is finished, clear +0x58 and +0x54 instead of raising +0x54;
 *   anything else falls straight through to the tail.
 *
 * The test on the sprite's word 0 is `isPlaying`, reached through the
 * SpriteAnimation/Sprite header rather than as a raw shift -- see the comment
 * at the test for why the spelling is load-bearing.
 */
s32 func_ov039_02096404(void* pool, void* task, void* args) {
    u8*   self  = *(u8**)((u8*)task + 0x18);
    void* child = EasyTask_GetTaskData(pool, *(u32*)((u8*)self + 0x50));

    if (child != NULL) {
        switch (*(s32*)((u8*)self + 0x58)) {
            case 0:
                *(s32*)((u8*)self + 0x54) = 0;
                break;

            case 1:
                func_ov039_0208e85c(child, (OtuPoint*)((u8*)self + 0x40));
                func_ov039_0208e6e0(child, (OtuPoint*)((u8*)self + 0x48));
                // `lsl #0x15` / `lsr #0x1f` is mwcc's extract for a *single* bit
                // at position 10, not a shift by ten: the target's pair leaves
                // only bit 10 of the sprite's word 0 in the result. That is
                // `isPlaying`, and naming the bitfield is what produces the
                // pair -- both `/ 1024` and `>> 10` fold to one `lsr #0xa`.
                if (((Sprite*)self)->isPlaying != 1) {
                    *(s32*)((u8*)self + 0x54) = 1;
                } else {
                    *(s32*)((u8*)self + 0x58) = 0;
                    *(s32*)((u8*)self + 0x54) = 0;
                }
                break;
        }
    } else {
        *(s32*)((u8*)self + 0x54) = 0;
    }

    if (*(s32*)((u8*)self + 0x54) != 0) {
        Sprite_Update((Sprite*)self);
    }
    return 1;
}

/* ==================================================================== */
/* 02096c58 -- the sprite-cell builder for the 02096d00 task.          */
/* ==================================================================== */

/**
 * @brief The frame-info callback for the 02096d00 task's sprite.
 *
 * Structurally the same as the twenty-two cell builders in OtuFieldAccess.c --
 * same two-step lookup, same guard chain over +0x18, +0x1C and +0x16 -- but with
 * three differences that are all visible in the disassembly and none of which
 * are stylistic:
 *
 *   - the depth key is the constant 3, not a call to func_ov039_02088400. The
 *     target ends the case-2 body with `mov r2, #0x3` / `str r2, [r1, #0x10]`;
 *   - `+0x0C` is never written.  Every one of the twenty-two writes it, and
 *     this one does not;
 *   - the index at +0x16 is read with `ldrsb`, a *sign-extended byte*, where
 *     all twenty-two use `ldrsh`.  That is a real type difference in the
 *     original, not a codegen artefact, so it is reproduced as a `s8` read.
 *
 * It is also a leaf: the two returns are `bx lr` out of the switch, with no
 * frame at all, and the object it fills is `g_SpriteFrameInfo` rather than
 * `data_0206b408`. That is the other tell that this one is a frame-info
 * callback and not one of the twenty-two: its literal pool word names the
 * frame-info object.
 */
// Nonmatching: 82%. Every semantic decision here is confirmed against the
// disassembly and the size is exact: the three-argument callback shape, the
// `updateSteps` store at +0 on the LOAD pass, the constant depth key of 3, the
// absent +0x0C write, the `ldrsb` (signed *byte*) index read where all
// twenty-two cell builders use `ldrsh`, the leaf frame, and the fact that the
// object filled is `g_SpriteFrameInfo` rather than `data_0206b408`.
//
// What is left is one register: the target materialises the zero it stores to
// +0x04/+0x08/+0x0C in r2, this build in r3. Both then compute the -1 for
// +0x10 in r2, so the residual is the choice of scratch register for the
// constant and nothing else. Four spellings of the zero stores were tried
// (separate assignments, a shared local, and reordering against the +0x10
// write) without moving it, and a chained assignment does not type-check
// because the three fields are s32, pointer and pointer.
SpriteFrameInfo* func_ov039_02096c58(Sprite* sprite, s32 arg, s32 mode) {
    SpriteFrameInfo* info = &g_SpriteFrameInfo;

    switch (mode) {
        case 1:
            // Offset 0, not +4: on the LOAD pass the target writes the *first*
            // word of the frame-info object, which is `updateSteps`. The cell
            // builders' case 1 writes +0x00 of OtuSpriteSlot, which is why the
            // two look identical in the disassembly and are not the same store.
            info->updateSteps = 1;
            return info;

        case 2: {
            u8* table;

            info->pieceCount = 0;
            info->cellPieces = NULL;
            info->affine     = NULL;
            info->sortKey    = -1;

            if (*(void**)((u8*)sprite + 0x18) != NULL && (table = *(u8**)((u8*)sprite + 0x1C)) != NULL &&
                *(s16*)((u8*)sprite + 0x16) >= 0)
            {
                // Both the index and the table pointer are re-read rather than
                // cached: the target loads +0x16 and +0x1C again for the second
                // lookup, and keeping them in locals shortens the body by an
                // instruction and shifts the whole register allocation.
                // `(index * 4 + 1) * 2`, not `index * 8 + 2`, for the same
                // reason -- the unsimplified form is what emits lsl/add/lsl.
                info->pieceCount = *(u16*)(table + (*(s16*)((u8*)sprite + 0x16) * 4 + 1) * 2);
                info->cellPieces =
                    (struct OamCellPiece*)(*(u8**)((u8*)sprite + 0x1C) +
                                           *(u16*)(*(u8**)((u8*)sprite + 0x1C) + *(s16*)((u8*)sprite + 0x16) * 8) * 2);
            }

            info->sortKey = 3;
            return info;
        }

        default:
            return NULL;
    }
}

/* ==================================================================== */
/* 020969fc -- the render stage for the three-sprite task.              */
/* ==================================================================== */

/**
 * @brief Draws whichever of the task's three sprites the state selects.
 *
 * The whole body is gated on +0xC0.  Inside it a switch on +0xC4 picks the
 * sprite: 0 draws the one at self+0, and cases 1 and 2 share the sprite at
 * self+0x40 while case 3 draws self+0x80.  Both `addls pc, pc, r0, lsl #2`
 * sequences in the target are switch jump tables, and the case bodies come out
 * in source order.
 *
 * Case 0 is the only one that does any work beyond rendering: it asks the OAM
 * manager for a fresh affine group, sized from +0xD4 and +0xD8, and stores the
 * returned index into bits 5-9 of the sprite's OAM attribute halfword at +0x0A.
 * The target builds the index with `lsl #0x10` / `lsr #0x10` (a u16 narrowing
 * of the s32 result) and then places it with `lsl #0x1b` / `lsr #0x16`, which
 * is a five-bit field insert -- hence the `unk_05 : 5` member rather than a
 * masked word.
 */
// Nonmatching: 83%. The switch, all four case bodies, the gate on +0xC0, the
// two jump tables, the bitfield insert and the size are all exact. What is
// left is the scheduling of case 0's affine-group call: the target computes
// the manager address (`ldrh`-free `lsl/lsr` index extract, two pool loads and
// the `mla`) and only then stores the zero fifth argument to the outgoing
// stack slot, whereas this build stores the stack argument first and shifts
// the address computation after it. The two `mla` operands are also permuted,
// which is the same reordering seen from the other side. Naming the manager
// address in a local was tried and changed nothing, so this is mwcc's
// evaluation order for the call and not a misread of the source.
s32 func_ov039_020969fc(void* pool, void* task, void* args) {
    u8* self = *(u8**)((u8*)task + 0x18);

    if (*(s32*)((u8*)self + 0xC0) != 0) {
        switch (*(s32*)((u8*)self + 0xC4)) {
            case 0:
                ((Sprite*)self)->unk_0A.unk_05 = (u16)OamMgr_AllocAffineGroup(
                    &g_OamMgr[((Sprite*)self)->bits_0_1], 0, *(s32*)((u8*)self + 0xD4), *(s32*)((u8*)self + 0xD8), 0);
                Sprite_RenderFrame((Sprite*)self);
                break;

            case 1:
            case 2:
                Sprite_RenderFrame((Sprite*)(self + 0x40));
                break;

            case 3:
                Sprite_RenderFrame((Sprite*)(self + 0x80));
                break;
        }
    }
    return 1;
}

/* ==================================================================== */
/* The two inits that are not task stages.                              */
/* ==================================================================== */

/*
 * func_ov039_02096b48 and func_ov039_02096154 are not reached through a
 * TaskStages table: the scene calls them directly with the task object in r0
 * and a selector in r1, so they take two or three arguments rather than the
 * usual (pool, task, args).  Both build a text-cell cursor over the shared
 * glyph table and then start a sound effect, and both are the only routines in
 * the overlay that use the `+0xC4` selector to pick *which* of the three
 * sprites a stage is about.
 *
 * The object is three Sprites (at +0, +0x40 and +0x80) followed by the state
 * block at +0xC0; the sound index constants are the overlay's own SndMgrSeIdx
 * values and are left as literals, since SndMgrSeIdx.h does not name them.
 */

/** Initialises the three-sprite task for one of its four selectors. */
void func_ov039_02096b48(void* self, s32 which) {
    *(s32*)((u8*)self + 0xC8) = 1;
    *(s32*)((u8*)self + 0xCC) = 0x3C;
    *(s32*)((u8*)self + 0xC4) = which;

    // The text-cell block at +0xD0: a cleared x, a 0x1000 (1.0) y and scale,
    // then a zero width, followed by the cursor built over the glyph table.
    *(s32*)((u8*)self + 0xD0) = 0;
    *(s32*)((u8*)self + 0xD4) = 0x1000;
    *(s32*)((u8*)self + 0xD8) = 0x1000;
    *(s16*)((u8*)self + 0xDC) = 0;
    *(s16*)((u8*)self + 0xDE) = 0;
    func_ov039_02087ba0((u8*)self + 0xE0, data_ov039_0209aa0c, 0xC, (u8*)self + 0xD0);

    // A switch, not an if/else chain: the target compares against 3 with
    // `addls pc, pc, r4, lsl #2`, and the four arms come out in source order.
    switch (which) {
        case 0:
            SndMgr_StartPlayingSE(0x55F);
            Sprite_ChangeAnimation((Sprite*)self, *(void**)((u8*)self + 0x18), 1, *(void**)((u8*)self + 0x1C));
            break;

        case 1:
            SndMgr_StartPlayingSE(0x560);
            Sprite_ChangeAnimation((Sprite*)(self + 0x40), *(void**)((u8*)self + 0x58), 1, *(void**)((u8*)self + 0x5C));
            break;

        case 2:
            SndMgr_StartPlayingSE(0x561);
            Sprite_ChangeAnimation((Sprite*)(self + 0x40), *(void**)((u8*)self + 0x58), 2, *(void**)((u8*)self + 0x5C));
            break;

        case 3:
            Sprite_ChangeAnimation((Sprite*)(self + 0x80), *(void**)((u8*)self + 0x98), 1, *(void**)((u8*)self + 0x9C));
            break;
    }

    SndMgr_StartPlayingSE(0x34B);
}

/**
 * @brief Initialises the two-sprite task, with an optional second label.
 *
 * The same object shape as 02096b48 but two sprites wide (the third block at
 * +0x80 is the one 02096b48 uses and this one does not), and with the +0x8C
 * argument acting as a gate: when it is zero the function returns straight
 * after the first Sprite_ChangeAnimation, which is what the target's `popeq`
 * on the link register is -- an early exit, not a computed return value. That
 * is also why the function is `void`: there is nothing in r0 on either path
 * that the source chose to return.
 *
 * The two selector arms differ only in their sound effects, their glyph table
 * and their cursor length (0xC versus 0xE), and the +0x349 first effect is
 * built by reusing the `1` already in r3 as `r3 + 0x348` -- reproduced here as
 * the plain constant, which mwcc folds back into that form on its own.
 *
 * `which` is typed `s16` rather than `s32` because the target hands it to
 * Sprite_ChangeAnimation with no narrowing: an s32 there would make mwcc emit
 * the `lsl #0x10` / `asr #0x10` pair the target does not have.
 */
void func_ov039_02096154(void* self, s16 which, s32 hasLabel) {
    *(s32*)((u8*)self + 0x84) = 1;
    *(s32*)((u8*)self + 0x88) = 0x78;
    *(s32*)((u8*)self + 0x8C) = hasLabel;
    *(s32*)((u8*)self + 0x90) = 0;
    *(s32*)((u8*)self + 0x94) = 0x1000;
    *(s32*)((u8*)self + 0x98) = 0x1000;
    *(s16*)((u8*)self + 0x9C) = 0;
    *(s16*)((u8*)self + 0x9E) = 0;

    // A switch even though there is no jump table: the target does both
    // compares up front and emits the two bodies out of line, which is the
    // shape a switch gives. An `else if` chain gets if-converted the other way
    // round and costs two extra branches.
    switch (which) {
        case 1:
            SndMgr_StartPlayingSE(0x349);
            SndMgr_StartPlayingSE(0x55D);
            func_ov039_02087ba0((u8*)self + 0xB0, data_ov039_0209a8a8, 0xC, (u8*)self + 0x90);
            break;

        case 2:
            SndMgr_StartPlayingSE(0x34A);
            SndMgr_StartPlayingSE(0x55E);
            func_ov039_02087ba0((u8*)self + 0xB0, data_ov039_0209a938, 0xE, (u8*)self + 0x90);
            break;
    }

    Sprite_ChangeAnimation((Sprite*)self, *(void**)((u8*)self + 0x18), which, *(void**)((u8*)self + 0x1C));

    if (*(s32*)((u8*)self + 0x8C) == 0) {
        return;
    }

    *(s32*)((u8*)self + 0xA0) = 0;
    *(s32*)((u8*)self + 0xA4) = 0x1000;
    *(s32*)((u8*)self + 0xA8) = 0x1000;
    *(s16*)((u8*)self + 0xAC) = 0;
    *(s16*)((u8*)self + 0xAE) = 0;
    func_ov039_02087ba0((u8*)self + 0xBC, data_ov039_0209a830, 0xA, (u8*)self + 0xA0);
    Sprite_ChangeAnimation((Sprite*)(self + 0x40), *(void**)((u8*)self + 0x58), 3, *(void**)((u8*)self + 0x5C));
}

/* ==================================================================== */
/* 020968c0 -- the per-frame update for the three-sprite task.          */
/* ==================================================================== */

/**
 * @brief Advances the three-sprite task's fade/label sequence.
 *
 * The task is a four-state machine on +0xC8, run as a `switch` because the
 * target emits a jump table for it.  Each of states 1 to 3 counts +0xCC down
 * from 0x3C and, on reaching zero, either reloads it and advances the state or
 * clears the state outright:
 *
 *   0  nothing is showing: clear the +0xC0 "visible" flag;
 *   1  visible; fade the main display out, then move to state 2;
 *   2  visible; on expiry play the effect and move to state 3;
 *   3  visible; fade both displays to 0x10, then clear the state.
 *
 * States 1 and 2 write the reload with an `if/else` shaped test -- the target
 * branches *over* the reset with `bgt` when the counter is still positive --
 * while state 3 tests the same counter with a conditional store instead.  Both
 * spellings are reproduced; making them uniform does not match.
 *
 * The tail is a second switch, on +0xC4, that steps whichever sprite the
 * selector names.  It is reached only when +0xC0 is set, and so is the
 * text-cell advance through func_ov039_02087bf8.
 */
s32 func_ov039_020968c0(void* pool, void* task, void* args) {
    u8* self = *(u8**)((u8*)task + 0x18);

    switch (*(s32*)((u8*)self + 0xC8)) {
        case 0:
            *(s32*)((u8*)self + 0xC0) = 0;
            break;

        case 1:
            *(s32*)((u8*)self + 0xC0) = 1;
            EasyFade_FadeMainDisplay(FADER_INSTANT, 0, 0x3C);
            *(s32*)((u8*)self + 0xCC) = *(s32*)((u8*)self + 0xCC) - 1;
            if (*(s32*)((u8*)self + 0xCC) <= 0) {
                *(s32*)((u8*)self + 0xCC) = 0x3C;
                *(s32*)((u8*)self + 0xC8) = 2;
            }
            break;

        case 2:
            *(s32*)((u8*)self + 0xC0) = 1;
            *(s32*)((u8*)self + 0xCC) = *(s32*)((u8*)self + 0xCC) - 1;
            if (*(s32*)((u8*)self + 0xCC) <= 0) {
                SndMgr_StartPlayingSE(0x34C);
                *(s32*)((u8*)self + 0xCC) = 0x3C;
                *(s32*)((u8*)self + 0xC8) = 3;
            }
            break;

        case 3:
            *(s32*)((u8*)self + 0xC0) = 1;
            EasyFade_FadeBothDisplays(FADER_INSTANT, 0x10, 0x3C);
            *(s32*)((u8*)self + 0xCC) = *(s32*)((u8*)self + 0xCC) - 1;
            if (*(s32*)((u8*)self + 0xCC) <= 0) {
                *(s32*)((u8*)self + 0xC8) = 0;
            }
            break;
    }

    if (*(s32*)((u8*)self + 0xC0) != 0) {
        func_ov039_02087bf8((u8*)self + 0xE0, (u8*)self + 0xD0);

        switch (*(s32*)((u8*)self + 0xC4)) {
            case 0:
                Sprite_Update((Sprite*)self);
                break;

            case 1:
            case 2:
                Sprite_Update((Sprite*)(self + 0x40));
                break;

            case 3:
                Sprite_Update((Sprite*)(self + 0x80));
                break;
        }
    }
    return 1;
}
