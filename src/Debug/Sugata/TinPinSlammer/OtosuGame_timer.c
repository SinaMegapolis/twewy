/**
 * @file OtosuGame_timer.c
 * @brief The `Tsk_OtosuGame_timer` task.
 */

#include "OtuFieldAccessShared.h"

typedef struct {
    s32 dataType;
    s32 seconds;
} OtuTimerArgs;

extern const SpriteAnimation data_ov039_02099b6c;
extern const TaskStages      data_ov039_02099b5c;
extern const TaskHandle      data_ov039_02099b50;

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

/* The countdown task: init, the radar task's update, and the update. */

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

/* Single words elsewhere. */

/** A single word at +0xC4. */
s32 func_ov039_02094204(OtuTimer* self) {
    return self->countdown;
}

/** A single word at +0xCC. */
s32 func_ov039_0209420c(OtuTimer* self) {
    return self->alarmed;
}
