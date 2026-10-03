#include "OtuFieldAccessShared.h"

/*
 * ov039 region 0x020950b8 - 0x020960bc. One translation unit of the
 * overlay; dsd gives each file a single contiguous `.text` claim. The
 * shared types, externs and prototypes are in OtuFieldAccessShared.h.
 */
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
OtuSpriteSlot* func_ov039_0209549c(OtuSpriteTask* t, s32 arg, s32 mode) {
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
            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + ((u16*)table)[index * 4] * 2);
            }

            slot->unk_0C   = (void*)((u8*)t + 0x1);
            slot->depthKey = func_ov039_02088400(*(s32*)((u8*)t + 0x54), 0, 0);
            return slot;
        }

        default:
            return NULL;
    }
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
OtuSpriteSlot* func_ov039_020958a8(OtuSpriteTask* t, s32 arg, s32 mode) {
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
            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + ((u16*)table)[index * 4] * 2);
            }

            slot->unk_0C   = (void*)((u8*)t + 0x1);
            slot->depthKey = func_ov039_02088400(*(s32*)((u8*)t + 0x10C), 0, 0);
            return slot;
        }

        default:
            return NULL;
    }
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
    // narrowed value on is what reproduces that. The first one must also be
    // materialised through `last` first -- the store is overwritten at `last =`
    // below and is dead, but mwcc's allocation for the first digit only matches
    // when the value goes through that slot (found by the permuter; objdiff 100%).
    last  = (count % 10) + 1;
    first = (s16)last;
    Sprite_ChangeAnimation(&self->sprite[2], self->sprite[2].animData, first, self->sprite[2].cellTable);

    last = (s16)((self->unk_11C / 10) % 10 + 1);

    if ((s16)last <= 1) {
        self->slot[3].live = 0;
        return;
    }

    Sprite_ChangeAnimation(&self->sprite[3], self->sprite[3].animData, last, self->sprite[3].cellTable);
}

/** Clears the +0x114 and +0x118 words together. */
void func_ov039_02095ddc(void* task) {
    *(s32*)((u8*)task + 0x114) = 0;
    *(s32*)((u8*)task + 0x118) = 0;
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
OtuSpriteSlot* func_ov039_02095dec(OtuSpriteTask* t, s32 arg, s32 mode) {
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
            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + ((u16*)table)[index * 4] * 2);
            }

            slot->unk_0C   = (void*)((u8*)t + 0x1);
            slot->depthKey = func_ov039_02088400(0, 0, 7);
            return slot;
        }

        default:
            return NULL;
    }
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
