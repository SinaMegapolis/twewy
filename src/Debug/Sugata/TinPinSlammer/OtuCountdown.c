/*
 * OtuCountdown.c -- the countdown sequencer (0x02086060-0x02087E2C).
 *
 * This is the gap between OtuScoreRow.c's steppers and OtuTaskPick.c's queries,
 * and it is the thing that *drives* the steppers: the two large functions here,
 * 02086174 (1460 bytes) and 02086808 (4796 bytes), are the sequences that call
 * nearly every stepper in OtuScoreRow.c in order. The band-1 agents found
 * `Seq_Otosu()` among the overlay's strings, and these are what it resolves to.
 *
 * It is NOT a Task translation unit, which is worth saying because the rest of
 * the overlay's simulation is. There is no TaskHandle, no Init/Update/Render/
 * Destroy quartet and no CreateTask here: the functions are scene-level
 * sequencer steps and accessors, reached from the scene's own dispatch rather
 * than from EasyTask_CreateTask. Applying the Task TU template would be forcing
 * a shape the code does not have -- see the note at the bottom of this file.
 *
 * Everything here reaches the scene's state through `OTU_STAGE(scene)`, the
 * stage-dispatch container at scene+0x41AC4, and `func_ov039_02098b70`, which is
 * the one-instruction accessor for that container's `stageBlock` at +0x14.
 * `stageBlock` is the same conflated object OtuScoreRow.c walks as a table of
 * `OtuTextRow`; here it is also a bitmask (the u16 at +4) and the child-task
 * pool (count at +0x140, ids at +0x17C).
 */

#include "Debug/Sugata/TinPinSlammer.h"
#include "Engine/Core/System.h"
#include "Engine/Text.h"

/* ----------------------------------------------------------------
 * The results screens' row labels, defined in OtuMenuText.c.
 *
 * Declared here rather than in the feature header because OtuCountdown.c is
 * the only user: they are the labels func_ov039_02086174 interleaves with
 * the row builders, and nothing else in the overlay names them yet.
 *
 * They are declared in OtuMenuText.c because that is the TU whose delinks
 * entry claims the .data range they physically live in. Declaring them here
 * instead would put them in a section OtuCountdown.o does not claim, and
 * the linker would place them wherever it liked.
 *
 * The names are address-embedded rather than the `data_ov039_*` spelling the
 * target's literal-pool words use, so each pool word compares as a
 * relocation-name difference even though it resolves to the same address.
 * See the note on func_ov039_02086174 for what that costs.
 * ---------------------------------------------------------------- */
extern const char Otu_str_0209a14c[12]; /* "Seq_Otosu()" */
extern const char Otu_str_0209a178[16]; /* "SINGLE MENU 1" */
extern const char Otu_str_0209a188[12]; /* SJIS 83 58 83 5e 81 5b 83 67 */
extern const char Otu_str_0209a194[12]; /* SJIS 83 89 83 45 83 93 83 68 */
extern const char Otu_str_0209a1a0[8];  /* SJIS 96 80 8e 43 */
extern const char Otu_str_0209a1a8[8];  /* SJIS 83 6f 83 62 83 57 31 */
extern const char Otu_str_0209a1b0[8];  /* SJIS 83 6f 83 62 83 57 32 */
extern const char Otu_str_0209a1b8[8];  /* SJIS 83 6f 83 62 83 57 33 */
extern const char Otu_str_0209a1c0[8];  /* SJIS 83 6f 83 62 83 57 34 */
extern const char Otu_str_0209a1c8[8];  /* SJIS 8f 64 82 b3 31 */
extern const char Otu_str_0209a1d0[8];  /* SJIS 8f 64 82 b3 32 */
extern const char Otu_str_0209a1d8[8];  /* SJIS 8f 64 82 b3 33 */
extern const char Otu_str_0209a1e0[8];  /* SJIS 8f 64 82 b3 34 */
extern const char Otu_str_0209a1e8[8];  /* SJIS 8f 64 82 b3 35 */
extern const char Otu_str_0209a1f0[8];  /* SJIS 8f 64 82 b3 36 */
extern const char Otu_str_0209a1f8[8];  /* SJIS 8f 64 82 b3 37 */
extern const char Otu_str_0209a200[4];  /* SJIS 8f 64 82 b3 */
extern const char Otu_str_0209a204[4];  /* "8" */
extern const char Otu_str_0209a208[8];  /* SJIS 8f 64 82 b3 39 */
extern const char Otu_str_0209a210[8];  /* SJIS 8f 64 82 b3 31 30 */
extern const char Otu_str_0209a218[12]; /* SJIS 8f 64 97 cd 89 c1 91 ac 93 78 */
extern const char Otu_str_0209a224[8];  /* SJIS 83 5e 81 5b 83 93 */
extern const char Otu_str_0209a22c[8];  /* SJIS 83 70 83 6c 83 8b */
extern const char Otu_str_0209a234[8];  /* SJIS 8b 43 90 e2 */
extern const char Otu_str_0209a23c[16]; /* "SINGLE MENU 2" */
extern const char Otu_str_0209a24c[12]; /* SJIS 8f d5 93 cb 89 89 8f 6f 31 */
extern const char Otu_str_0209a258[12]; /* SJIS 8f d5 93 cb 89 89 8f 6f 32 */
extern const char Otu_str_0209a264[8];  /* SJIS 95 4b 8e 45 8b 5a */
extern const char Otu_str_0209a26c[12]; /* SJIS 83 6f 83 62 83 57 89 f1 93 5d */
extern const char Otu_str_0209a278[16]; /* "SINGLE MENU 3" */
extern const char Otu_str_0209a288[12]; /* SJIS 83 6f 83 62 83 57 49 44 */
extern const char Otu_str_0209a294[12]; /* SJIS 95 4b 8e 45 8b 5a 8f 8a 8e 9d */
extern const char Otu_str_0209a2a0[8];  /* SJIS 8f 64 82 b3 */
extern const char Otu_str_0209a2a8[12]; /* SJIS 82 dc 82 aa 82 e9 97 cd */
extern const char Otu_str_0209a2b4[12]; /* SJIS 83 6a 81 5b 83 68 83 8b */
extern const char Otu_str_0209a2c0[8];  /* SJIS 83 81 83 65 83 49 */
extern const char Otu_str_0209a2c8[12]; /* SJIS 83 6e 83 93 83 7d 81 5b */
extern const char Otu_str_0209a2d4[12]; /* SJIS 8b 43 90 e2 8e 9e 8a d4 */

/**
 * The head of the stage block, in the two views this band uses.
 *
 * `OtuTextRow` is already the union of a base-owned view and a 16-byte-stride
 * element view, so widening it to cover the bitmask would move members and
 * disturb the functions in OtuScoreRow.c that already match. A separate
 * two-field view of the same bytes is the cheaper option, and it is why these
 * two fields are reached through this type and the rows are not.
 *
 * The first word looked like a row pointer for a long while -- `02087c8c` stores
 * an argument into it and `02086728` zeroes it -- but `02086174` switches on it
 * against 0, 1 and 2 and picks one of the three result menus, so it is a menu
 * index. Both readings emit the same 32-bit store, which is why it took reading
 * the layout function to settle.
 */
typedef struct {
    /* 0x00 */ s32 menu;  // which result menu: 0, 1 or 2
    /* 0x04 */ u16 flags; // one bit per pin slot, set by 02087cac
} OtuCountdownHead;       // Size: 0x8

/** The stage block, as the countdown sequencer sees it. */
#define OTU_COUNTDOWN_HEAD(scene) ((OtuCountdownHead*)func_ov039_02098b70(OTU_STAGE(scene)))

/**
 * @brief An element of the countdown's value table, 0xC bytes.
 *
 * A second, narrower view of the scrolling data than the 16-byte-stride
 * `OtuTextRow` one: 12 bytes rather than 16, holding a hold counter and the
 * value pair that a row's `value`/`drawnValue` fields also carry. `02087ba0` and
 * `02087bf8` walk this and write the pair straight into a row, which is how a
 * value arrives at the display.
 */
typedef struct {
    /* 0x0 */ u16 hold; // frames to stay on this element
    /* 0x2 */ u8  pad_02[2];
    /* 0x4 */ s32 value;
    /* 0x8 */ s32 drawnValue;
} OtuCountdownValue; // Size: 0xC

/**
 * @brief A cursor over an `OtuCountdownValue` array.
 *
 * The three words of state are the array base, the element index, and a
 * down-counting `hold`; `limit` is the element count. `02087bf8` advances the
 * index when `hold` runs out, and wraps it to zero at `limit` -- so a value
 * table longer than the count it is given simply stops early, it does not wrap
 * mid-table.
 */
typedef struct {
    /* 0x0 */ OtuCountdownValue* base;
    /* 0x4 */ u16                index;
    /* 0x6 */ u16                limit;
    /* 0x8 */ u16                hold;
    u8                           pad_0A[2];
} OtuCountdownCursor; // Size: 0xC

/* ==================================================================== */
/* Accessors.  The low-level half of the sequencer: they move rows into  */
/* the stage block, set its bitmask, and drive the dispatch.             */
/* ==================================================================== */

/** Select which result menu the stage block is showing. */
void func_ov039_02087c8c(TinPinSlammer_Scene* scene, s32 menu) {
    OTU_COUNTDOWN_HEAD(scene)->menu = menu;
}

/**
 * Copy `len` bytes into pin slot `slot`'s tray and mark the slot dirty.
 *
 * The tray address is spelled as `scene + 0x44000 + 0x4C + slot * 0xE` rather
 * than via OTU_PIN_TRAY because the target builds it with two separate adds --
 * one for 0x4C, one for 0x44000 -- where the macro's single 0x4404C immediate
 * folds them into one, which costs an instruction.
 */
void func_ov039_02087cac(TinPinSlammer_Scene* scene, s32 slot, const void* src, s32 len) {
    OtuCountdownHead* head = OTU_COUNTDOWN_HEAD(scene);

    MI_CpuCopyU8((u8*)scene + 0x44000 + 0x4C + slot * 0xE, src, len);
    head->flags |= 1 << slot;
}

/** Reset the dispatch, then run stage 0. */
// Nonmatching: 59% -- register allocation only, on a 12-instruction function.
// The target needs one callee-saved register (r4, holding `scene` across the
// first call) and reloads the 0x41AC4 pool word for the second call. mwcc
// common-subexpression-eliminates the two identical pool loads instead, keeps
// the constant in a callee-saved register across the call, and so needs three.
// Nothing in the source distinguishes the two address computations -- they are
// the same expression on the same argument -- so the reload is not reachable.
void func_ov039_02087b74(TinPinSlammer_Scene* scene) {
    func_ov039_02098a50(OTU_STAGE(scene));
    func_ov039_02098a40(OTU_STAGE(scene), 0);
}

/**
 * Fade the scene in, then arm the stage once the fade has finished.
 *
 * The early return is a conditional pop on the test, not a branch to a trailing
 * block: the target pops immediately when a fade is still running, so there is
 * no frame in which this arms the stage mid-fade.
 */
void func_ov039_02087acc(TinPinSlammer_Scene* scene) {
    EasyFade_FadeBothDisplays(0, 0x10, 0x1000);

    if (EasyFade_IsFading()) {
        return;
    }

    func_ov039_02098a50(OTU_STAGE(scene));
}

/** Fade out, after handing the scene to 020825b0. */
void func_ov039_020867d4(TinPinSlammer_Scene* scene) {
    // The target calls the stageBlock accessor here and discards the result. It
    // is a real call in the ROM so it is reproduced rather than dropped, but
    // nothing downstream depends on it -- hence no cast and no temporary.
    func_ov039_02098b70(OTU_STAGE(scene));

    func_ov039_020825b0(scene);
    EasyFade_FadeBothDisplays(2, 0x10, 0x1000);
}

/**
 * Offer `data` to every child of the stage except the one already holding it.
 *
 * The count is re-read on every iteration -- once at the top of the loop and
 * again in the compare -- so the source re-reads it too, and that is
 * load-bearing rather than incidental: hoisting it into a local drops a load
 * from every iteration and the target has one.
 */
// Nonmatching: 82% -- register allocation only. The logic, the instruction
// multiset and every load offset agree; mwcc strength-reduces `i * 4` into a
// second induction variable, which costs it two extra callee-saved registers
// (r3 and r9) and an extra add per iteration. Tried three spellings of the index
// -- the child handle access at offset +0x17C (childIds[i]), a separate `u8* entry` statement, and
// `(i << 2)` -- and all three produce the identical object, so this is mwcc's
// choice rather than the source's. The target's `add r0, r4, r5, lsl #2` keeps
// the scaled index implicit instead.
/* ==================================================================== */
/* The layout.                                                          */
/* ==================================================================== */

/**
 * Lay out a whole result screen: the menu's title, then every row.
 *
 * The shape is a three-way `switch` on the block's menu index, and each arm is
 * one long straight-line run: a row builder, then the label for the row below
 * it, at that row's y. So the builders and the labels interleave rather than
 * appearing as two groups, and the y values step by 8 through the arm. This is
 * not a loop over a table -- the target has no loop and no table, and the three
 * arms differ in both length and content, so the sequence is spelled out.
 *
 * The arms are the three result menus. Menu 1 is the round summary (start,
 * round, friction, four badges, ten weights, gravity, turns, panels), menu 2 is
 * the effects summary (two collision effects, the special move, badge rotation),
 * and menu 3 is the per-pin-type summary -- badge id, special moves held,
 * weight, bending force, then needle, meteo, hammer and stun time.
 *
 * Menu 3 is why the overlay is worth reading. Its needle/meteo/hammer rows are
 * the same three the sprite-cell builders select on (`phase` 6, 7, 8) and the same
 * three the ROM's task list names, and here they are as three labelled rows of a
 * results screen. The three constants, the three task names and the three rows
 * are the same three objects seen from three directions.
 *
 * Menu 3 also takes the slot argument the earlier arms pass explicitly: it reads
 * `block->trayCount` afresh for every row rather than hoisting it, and the target
 * re-loads it each time, so this does too.
 */
// Nonmatching: 75% -- mwcc common-subexpression-eliminates the text object's
// address into a callee-saved register and holds it for all 22 draws, where the
// target re-loads `scene + 0x416B4` from the pool every time. That is one
// instruction per draw, ~22 of 328, and it shifts every register in the function
// by one (target {r3,r4,r5}, mine {r4,r5,r6}) because the hoisted value takes
// the slot the target spends on the string. The dispatch, the pool order, the
// argument setup (`mov r1, #x; mov r2, r1` reusing x for y) and the whole call
// sequence all agree; only the hoisting differs. Register pressure, not shape --
// the address is the same expression on the same argument at all 22 sites, so
// there is nothing in the source to distinguish them.
void func_ov039_02086174(TinPinSlammer_Scene* scene) {
    OtuCountdownHead* head  = OTU_COUNTDOWN_HEAD(scene);
    OtuTextRow*       block = (OtuTextRow*)head;

    func_02010b50(OTU_TEXT(scene));

    switch (head->menu) {
        case 0:
            Text_RenderToScreen(OTU_TEXT(scene), 0, 0, Otu_str_0209a178);
            func_ov039_02083bb0(scene);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x08, Otu_str_0209a188);
            func_ov039_02083ed4(scene);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x10, Otu_str_0209a194);
            func_ov039_020840c0(scene);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x18, Otu_str_0209a1a0);
            func_ov039_020842bc(scene, 0);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x20, Otu_str_0209a1a8);
            func_ov039_020842bc(scene, 1);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x28, Otu_str_0209a1b0);
            func_ov039_020842bc(scene, 2);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x30, Otu_str_0209a1b8);
            func_ov039_020842bc(scene, 3);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x38, Otu_str_0209a1c0);
            func_ov039_020844f8(scene, 0);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x40, Otu_str_0209a1c8);
            func_ov039_020844f8(scene, 1);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x48, Otu_str_0209a1d0);
            func_ov039_020844f8(scene, 2);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x50, Otu_str_0209a1d8);
            func_ov039_020844f8(scene, 3);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x58, Otu_str_0209a1e0);
            func_ov039_020844f8(scene, 4);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x60, Otu_str_0209a1e8);
            func_ov039_020844f8(scene, 5);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x68, Otu_str_0209a1f0);
            func_ov039_020844f8(scene, 6);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x70, Otu_str_0209a1f8);
            func_ov039_020844f8(scene, 7);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x78, Otu_str_0209a200);
            func_ov039_020844f8(scene, 8);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x80, Otu_str_0209a208);
            func_ov039_020844f8(scene, 9);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x88, Otu_str_0209a210);
            func_ov039_0208462c(scene);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x90, Otu_str_0209a218);
            func_ov039_02084738(scene);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x98, Otu_str_0209a224);
            func_ov039_02084874(scene);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0xA0, Otu_str_0209a22c);
            func_ov039_020849a4(scene);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0xA8, Otu_str_0209a234);
            return;

        case 1:
            Text_RenderToScreen(OTU_TEXT(scene), 0, 0, Otu_str_0209a23c);
            func_ov039_02083bb0(scene);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x08, Otu_str_0209a24c);
            func_ov039_02084ac4(scene);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x10, Otu_str_0209a258);
            func_ov039_02084c34(scene);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x18, Otu_str_0209a264);
            func_ov039_02084d80(scene);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x20, Otu_str_0209a26c);
            func_ov039_02084f08(scene);
            return;

        case 2:
            Text_RenderToScreen(OTU_TEXT(scene), 0, 0, Otu_str_0209a278);
            func_ov039_02083bb0(scene);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x08, Otu_str_0209a288);
            func_ov039_020851bc(scene);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x10, Otu_str_0209a294);
            func_ov039_02085388(scene, block->trayCount);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x18, Otu_str_0209a2a0);
            func_ov039_020855e0(scene, block->trayCount);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x20, Otu_str_0209a2a8);
            func_ov039_02085770(scene, block->trayCount);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x28, Otu_str_0209a2b4);
            func_ov039_020858e4(scene, block->trayCount);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x30, Otu_str_0209a2c0);
            func_ov039_02085b30(scene, block->trayCount);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x38, Otu_str_0209a2c8);
            func_ov039_02085e54(scene, block->trayCount);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x40, Otu_str_0209a2d4);
            func_ov039_02086060(scene, block->trayCount);
            return;

        default:
            return;
    }
}

/**
 * A row's field, addressed the way the target addresses it: the fixed field
 * offset is added to the block first, and the row index is then applied as a
 * scaled index on the load.
 *
 * Spelled `(block + offset) + index * 0x10` rather than going through
 * OTU_TEXT_ROW, because those two are the same address but not the same code.
 * The target hoists the field offset into a register and lets the scaled index
 * ride on the load, three instructions per site; going through the row pointer
 * has to materialise the pointer and add, and costs four. On
 * func_ov039_02086808 that is the entire 567-instruction deficit.
 *
 * Same trap as accessing child handles by offset, one function over: a base
 * pointer plus a large fixed offset plus a scaled index wants the offset folded in first.
 */
#define OTU_TEXT_FIELD(block, index, offset) (*(s32*)((u8*)(block) + (offset) + (index) * 0x10))

/* ==================================================================== */
/* The board/pin-logic stage: the results screen's per-frame handler.     */
/* ==================================================================== */

/*
 * func_ov039_02086808 is the largest function in this overlay -- 1197
 * instructions -- and almost all of it is mechanical. It switches on a
 * system-wide state byte and, within each state, on which result menu is
 * showing and then on which row of that menu is current. The row tables below
 * are what the switch bodies are; nothing here is a computed index.
 *
 * The eight states, from the dispatch:
 *
 *   0x40  step every row's number *up*, and while a row has no value yet walk
 *         its row index *down* to 20, i.e. the countdown runs backwards
 *   0x80  the same with the signs flipped: numbers down, row index up to 21
 *   0x20  a hold timer. Ticks `drawnValue` down and, once a row has no value,
 *         gives it that row's target. This is the state that walks the rows
 *   0x10  the same timer counting the other way, clamping back to 0 once a row
 *         has reached its target. This is the state that rewinds
 *   0x1   start the menu: zero the current row, or hand over to the stage
 *         dispatch once menu 0 row 0 is reached
 *   0x2   reset the current row, or hand back to the dispatch
 *   0x100 next menu, wrapping 2 -> 0, and redraw
 *   0x200 previous menu, wrapping 0 -> 2, and redraw
 *
 * Every arm then falls through to a common tail: draw the scrollbar, redraw
 * the current row, and bump `block->flags`, which is a frame counter rather
 * than the column-blanking flag the same field name carries elsewhere.
 */
/* ==================================================================== */
/* The board/pin-logic stage: the results screen's per-frame handler.     */
/* ==================================================================== */

/*
 * func_ov039_02086808 is the largest function in this overlay -- 1197
 * instructions -- and almost all of it is mechanical. It switches on a
 * system-wide state byte and, within each state, on which result menu is
 * showing and then on which row of that menu is current. The row tables below
 * are what the switch bodies are; nothing here is a computed index.
 *
 * The eight states, from the dispatch:
 *
 *   0x40  step every row's number *up*, and while a row has no value yet walk
 *         its row index *down* to 20, i.e. the countdown runs backwards
 *   0x80  the same with the signs flipped: numbers down, row index up to 21
 *   0x20  a hold timer. Ticks `drawnValue` down and, once a row has no value,
 *         gives it that row's target. This is the state that walks the rows
 *   0x10  the same timer counting the other way, clamping back to 0 once a row
 *         has reached its target. This is the state that rewinds
 *   0x1   start the menu: zero the current row, or hand over to the stage
 *         dispatch once menu 0 row 0 is reached
 *   0x2   reset the current row, or hand back to the dispatch
 *   0x100 next menu, wrapping 2 -> 0, and redraw
 *   0x200 previous menu, wrapping 0 -> 2, and redraw
 *
 * Every arm then falls through to a common tail: draw the scrollbar, redraw
 * the current row, and bump `block->flags`, which is a frame counter rather
 * than the column-blanking flag the same field name carries elsewhere.
 */
/* ==================================================================== */
/* The board/pin-logic stage: the results screen's per-frame handler.     */
/* ==================================================================== */

/*
 * func_ov039_02086808 is the largest function in this overlay -- 1197
 * instructions -- and almost all of it is mechanical. It switches on a
 * system-wide state byte and, within each state, on which result menu is
 * showing and then on which row of that menu is current. The row tables below
 * are what the switch bodies are; nothing here is a computed index.
 *
 * The eight states, from the dispatch:
 *
 *   0x40  step every row's number *up*, and while a row has no value yet walk
 *         its row index *down* to 20, i.e. the countdown runs backwards
 *   0x80  the same with the signs flipped: numbers down, row index up to 21
 *   0x20  a hold timer. Ticks `drawnValue` down and, once a row has no value,
 *         gives it that row's target. This is the state that walks the rows
 *   0x10  the same timer counting the other way, clamping back to 0 once a row
 *         has reached its target. This is the state that rewinds
 *   0x1   start the menu: zero the current row, or hand over to the stage
 *         dispatch once menu 0 row 0 is reached
 *   0x2   reset the current row, or hand back to the dispatch
 *   0x100 next menu, wrapping 2 -> 0, and redraw
 *   0x200 previous menu, wrapping 0 -> 2, and redraw
 *
 * Every arm then falls through to a common tail: draw the scrollbar, redraw
 * the current row, and bump `block->flags`, which is a frame counter rather
 * than the column-blanking flag the same field name carries elsewhere.
 */
// Two of the eight arms below -- 0x20 and 0x10, the hold timers -- were emitted
// empty for several iterations of this file, as a bare
// `switch (row) { default: break; }`. All 32 of their case bodies were missing
// while the case labels were still present, so nothing in the source looked
// wrong, and both arms compiled to nothing: 566 of the target's 1197 instructions
// simply absent. 51.4% -> 80.8% once they were there.
//
// The cause was in the generator that wrote this function, not in the
// decompilation -- one emitter helper wrote to stdout while every other one wrote
// to the output buffer. The fix is an assertion there that counts the emitted
// bodies rather than trusting the emitter, because the failure mode was exactly
// an emitter that reported success and produced nothing.
//
// What let it run so long is that objdiff's diff-kind census said
// `DIFF_DELETE 567` while the instruction listing looked like nothing worse than
// register noise, and 51% read like a register-allocation gap. The census is the
// instrument for "my build is missing code"; the listing is not.
void func_ov039_02086808(TinPinSlammer_Scene* scene) {
    OtuTextRow* block = (OtuTextRow*)func_ov039_02098b70(OTU_STAGE(scene));
    OtuTextRow* row;
    s32         menu;
    s32         step;

    switch (SysControl.buttonState.pressedButtons) {
        case 0x40:
            row = OTU_TEXT_ROW(block, block->index);

            if (row->value < 0) {
                // No value yet: walk the row index so the next row to fill is
                // the one that comes next in this menu's countdown.
                switch (block->index) {
                    case 0:
                        OTU_TEXT_FIELD(block, block->index, 0x0C) -= 1;
                        row = OTU_TEXT_ROW(block, block->index);
                        if (row->row < 0) {
                            row->row = 0x14;
                        }
                        break;
                    case 1:
                        OTU_TEXT_FIELD(block, block->index, 0x0C) -= 1;
                        row = OTU_TEXT_ROW(block, block->index);
                        if (row->row < 0) {
                            row->row = 0x3;
                        }
                        break;
                    case 2:
                        OTU_TEXT_FIELD(block, block->index, 0x0C) -= 1;
                        row = OTU_TEXT_ROW(block, block->index);
                        if (row->row < 0) {
                            row->row = 0x7;
                        }
                        break;
                }
            } else {
                // The row has a value, so step its number in `sign`'s direction.
                switch (block->index) {
                    case 0:
                        step = row->row;
                        switch (step) {
                            default:
                                break;
                            case 1:
                                func_ov039_02083cbc(scene, 1);
                                break;
                            case 2:
                                func_ov039_02084020(scene, 1);
                                break;
                            case 3:
                            case 4:
                            case 5:
                            case 6:
                                func_ov039_020841a0(scene, step - 3, 1);
                                break;
                            case 7:
                            case 8:
                            case 9:
                            case 10:
                            case 11:
                            case 12:
                            case 13:
                            case 14:
                            case 15:
                            case 16:
                                func_ov039_02084458(scene, step - 7, 1);
                                break;
                            case 17:
                                func_ov039_020845dc(scene, 1);
                                break;
                            case 18:
                                func_ov039_020846c4(scene, 1);
                                break;
                            case 19:
                                func_ov039_020847ec(scene, 1);
                                break;
                            case 20:
                                func_ov039_02084944(scene, 1);
                                break;
                        }
                        break;
                    case 1:
                        step = row->row;
                        switch (step) {
                            default:
                                break;
                            case 0:
                                func_ov039_02084a3c(scene, 1);
                                break;
                            case 1:
                                func_ov039_02084b94(scene, 1);
                                break;
                            case 2:
                                func_ov039_02084d20(scene, 1);
                                break;
                            case 3:
                                func_ov039_02084e1c(scene, 1);
                                break;
                        }
                        break;
                    case 2:
                        step = row->row;
                        switch (step) {
                            default:
                                break;
                            case 0:
                                func_ov039_02085124(scene, 1);
                                func_ov039_02085388(scene, block->trayCount);
                                func_ov039_020855e0(scene, block->trayCount);
                                func_ov039_02085770(scene, block->trayCount);
                                func_ov039_020858e4(scene, block->trayCount);
                                func_ov039_02085b30(scene, block->trayCount);
                                func_ov039_02085e54(scene, block->trayCount);
                                func_ov039_02086060(scene, block->trayCount);
                                break;
                            case 1:
                                func_ov039_020852c0(scene, block->trayCount, 1);
                                break;
                            case 2:
                                func_ov039_0208554c(scene, block->trayCount, 1);
                                break;
                            case 3:
                                func_ov039_020856c8(scene, block->trayCount, 1);
                                break;
                            case 4:
                                func_ov039_02085818(scene, block->trayCount, 1);
                                break;
                            case 5:
                                func_ov039_02085a64(scene, block->trayCount, 1);
                                break;
                            case 6:
                                func_ov039_02085cb0(scene, block->trayCount, 1);
                                break;
                            case 7:
                                func_ov039_02085fb4(scene, block->trayCount, 1);
                                break;
                        }
                        break;
                }
            }
            break;

        case 0x80:
            row = OTU_TEXT_ROW(block, block->index);

            if (row->value < 0) {
                // No value yet: walk the row index so the next row to fill is
                // the one that comes next in this menu's countdown.
                switch (block->index) {
                    case 0:
                        OTU_TEXT_FIELD(block, block->index, 0x0C) -= 1;
                        row = OTU_TEXT_ROW(block, block->index);
                        if (row->row < 0) {
                            row->row = 21;
                        }
                        break;
                    case 1:
                        OTU_TEXT_FIELD(block, block->index, 0x0C) -= 1;
                        row = OTU_TEXT_ROW(block, block->index);
                        if (row->row < 0) {
                            row->row = 4;
                        }
                        break;
                    case 2:
                        OTU_TEXT_FIELD(block, block->index, 0x0C) -= 1;
                        row = OTU_TEXT_ROW(block, block->index);
                        if (row->row < 0) {
                            row->row = 8;
                        }
                        break;
                }
            } else {
                // The row has a value, so step its number in `sign`'s direction.
                switch (block->index) {
                    case 0:
                        step = row->row;
                        switch (step) {
                            default:
                                break;
                            case 1:
                                func_ov039_02083cbc(scene, -1);
                                break;
                            case 2:
                                func_ov039_02084020(scene, -1);
                                break;
                            case 3:
                            case 4:
                            case 5:
                            case 6:
                                func_ov039_020841a0(scene, step - 3, -1);
                                break;
                            case 7:
                            case 8:
                            case 9:
                            case 10:
                            case 11:
                            case 12:
                            case 13:
                            case 14:
                            case 15:
                            case 16:
                                func_ov039_02084458(scene, step - 7, -1);
                                break;
                            case 17:
                                func_ov039_020845dc(scene, -1);
                                break;
                            case 18:
                                func_ov039_020846c4(scene, -1);
                                break;
                            case 19:
                                func_ov039_020847ec(scene, -1);
                                break;
                            case 20:
                                func_ov039_02084944(scene, -1);
                                break;
                        }
                        break;
                    case 1:
                        step = row->row;
                        switch (step) {
                            default:
                                break;
                            case 0:
                                func_ov039_02084a3c(scene, -1);
                                break;
                            case 1:
                                func_ov039_02084b94(scene, -1);
                                break;
                            case 2:
                                func_ov039_02084d20(scene, -1);
                                break;
                            case 3:
                                func_ov039_02084e1c(scene, -1);
                                break;
                        }
                        break;
                    case 2:
                        step = row->row;
                        switch (step) {
                            default:
                                break;
                            case 0:
                                func_ov039_02085124(scene, -1);
                                func_ov039_02085388(scene, block->trayCount);
                                func_ov039_020855e0(scene, block->trayCount);
                                func_ov039_02085770(scene, block->trayCount);
                                func_ov039_020858e4(scene, block->trayCount);
                                func_ov039_02085b30(scene, block->trayCount);
                                func_ov039_02085e54(scene, block->trayCount);
                                func_ov039_02086060(scene, block->trayCount);
                                break;
                            case 1:
                                func_ov039_020852c0(scene, block->trayCount, -1);
                                break;
                            case 2:
                                func_ov039_0208554c(scene, block->trayCount, -1);
                                break;
                            case 3:
                                func_ov039_020856c8(scene, block->trayCount, -1);
                                break;
                            case 4:
                                func_ov039_02085818(scene, block->trayCount, -1);
                                break;
                            case 5:
                                func_ov039_02085a64(scene, block->trayCount, -1);
                                break;
                            case 6:
                                func_ov039_02085cb0(scene, block->trayCount, -1);
                                break;
                            case 7:
                                func_ov039_02085fb4(scene, block->trayCount, -1);
                                break;
                        }
                        break;
                }
            }
            break;

        case 0x20:
            row = OTU_TEXT_ROW(block, block->index);

            if (row->value >= 0) {
                // Ticking down, and giving a row that has no value its target.
                // The inner clamp cannot fire: the guard above already
                // excluded a negative value. It is in the original and it is
                // kept, because dropping it is a mismatch.
                switch (block->index) {
                    case 0:
                        step = row->row;
                        switch (step) {
                            default:
                                break;
                            case 1:
                                OTU_TEXT_FIELD(block, block->index, 0x04) -= 1;
                                row = OTU_TEXT_ROW(block, block->index);
                                if (row->value < 0) {
                                    row->value = 9;
                                }
                                break;
                            case 2:
                                OTU_TEXT_FIELD(block, block->index, 0x04) -= 1;
                                row = OTU_TEXT_ROW(block, block->index);
                                if (row->value < 0) {
                                    row->value = 16;
                                }
                                break;
                            case 3:
                            case 4:
                            case 5:
                            case 6:
                                OTU_TEXT_FIELD(block, block->index, 0x04) -= 1;
                                row = OTU_TEXT_ROW(block, block->index);
                                if (row->value < 0) {
                                    row->value = 18;
                                }
                                break;
                            case 7:
                            case 8:
                            case 9:
                            case 10:
                            case 11:
                            case 12:
                            case 13:
                            case 14:
                            case 15:
                            case 16:
                                OTU_TEXT_FIELD(block, block->index, 0x04) -= 1;
                                row = OTU_TEXT_ROW(block, block->index);
                                if (row->value < 0) {
                                    row->value = 16;
                                }
                                break;
                            case 17:
                                OTU_TEXT_FIELD(block, block->index, 0x04) -= 1;
                                row = OTU_TEXT_ROW(block, block->index);
                                if (row->value < 0) {
                                    row->value = 4;
                                }
                                break;
                            case 18:
                                OTU_TEXT_FIELD(block, block->index, 0x04) -= 1;
                                row = OTU_TEXT_ROW(block, block->index);
                                if (row->value < 0) {
                                    row->value = 8;
                                }
                                break;
                            case 19:
                                OTU_TEXT_FIELD(block, block->index, 0x04) -= 1;
                                row = OTU_TEXT_ROW(block, block->index);
                                if (row->value < 0) {
                                    row->value = 12;
                                }
                                break;
                            case 20:
                                OTU_TEXT_FIELD(block, block->index, 0x04) -= 1;
                                row = OTU_TEXT_ROW(block, block->index);
                                if (row->value < 0) {
                                    row->value = 4;
                                }
                                break;
                        }
                        break;
                    case 1:
                        step = row->row;
                        switch (step) {
                            default:
                                break;
                            case 0:
                                OTU_TEXT_FIELD(block, block->index, 0x04) -= 1;
                                row = OTU_TEXT_ROW(block, block->index);
                                if (row->value < 0) {
                                    row->value = 12;
                                }
                                break;
                            case 1:
                                OTU_TEXT_FIELD(block, block->index, 0x04) -= 1;
                                row = OTU_TEXT_ROW(block, block->index);
                                if (row->value < 0) {
                                    row->value = 16;
                                }
                                break;
                            case 2:
                                OTU_TEXT_FIELD(block, block->index, 0x04) -= 1;
                                row = OTU_TEXT_ROW(block, block->index);
                                if (row->value < 0) {
                                    row->value = 4;
                                }
                                break;
                            case 3:
                                OTU_TEXT_FIELD(block, block->index, 0x04) -= 1;
                                row = OTU_TEXT_ROW(block, block->index);
                                if (row->value < 0) {
                                    row->value = 10;
                                }
                                break;
                        }
                        break;
                    case 2:
                        step = row->row;
                        switch (step) {
                            default:
                                break;
                            case 0:
                                OTU_TEXT_FIELD(block, block->index, 0x04) -= 1;
                                row = OTU_TEXT_ROW(block, block->index);
                                if (row->value < 0) {
                                    row->value = 3;
                                }
                                break;
                            case 1:
                                OTU_TEXT_FIELD(block, block->index, 0x04) -= 1;
                                row = OTU_TEXT_ROW(block, block->index);
                                if (row->value < 0) {
                                    row->value = 8;
                                }
                                break;
                            case 2:
                                OTU_TEXT_FIELD(block, block->index, 0x04) -= 1;
                                row = OTU_TEXT_ROW(block, block->index);
                                if (row->value < 0) {
                                    row->value = 2;
                                }
                                break;
                            case 3:
                                OTU_TEXT_FIELD(block, block->index, 0x04) -= 1;
                                row = OTU_TEXT_ROW(block, block->index);
                                if (row->value < 0) {
                                    row->value = 4;
                                }
                                break;
                            case 4:
                            case 5:
                                OTU_TEXT_FIELD(block, block->index, 0x04) -= 1;
                                row = OTU_TEXT_ROW(block, block->index);
                                if (row->value < 0) {
                                    row->value = 6;
                                }
                                break;
                            case 6:
                                OTU_TEXT_FIELD(block, block->index, 0x04) -= 1;
                                row = OTU_TEXT_ROW(block, block->index);
                                if (row->value < 0) {
                                    row->value = 12;
                                }
                                break;
                            case 7:
                                OTU_TEXT_FIELD(block, block->index, 0x04) -= 1;
                                row = OTU_TEXT_ROW(block, block->index);
                                if (row->value < 0) {
                                    row->value = 3;
                                }
                                break;
                        }
                        break;
                }
            }
            break;

        case 0x10:
            row = OTU_TEXT_ROW(block, block->index);

            if (row->value >= 0) {
                // Ticking up, and clearing a row that has reached its target.
                switch (block->index) {
                    case 0:
                        step = row->row;
                        switch (step) {
                            default:
                                break;
                            case 1:
                                OTU_TEXT_FIELD(block, block->index, 0x04) += 1;
                                if (OTU_TEXT_FIELD(block, block->index, 0x04) < 0) {
                                    OTU_TEXT_FIELD(block, block->index, 0x04) = 0x8;
                                }
                                break;
                            case 2:
                                OTU_TEXT_FIELD(block, block->index, 0x04) += 1;
                                if (OTU_TEXT_FIELD(block, block->index, 0x04) < 0) {
                                    OTU_TEXT_FIELD(block, block->index, 0x04) = 0xF;
                                }
                                break;
                            case 3:
                            case 4:
                            case 5:
                            case 6:
                                OTU_TEXT_FIELD(block, block->index, 0x04) += 1;
                                if (OTU_TEXT_FIELD(block, block->index, 0x04) < 0) {
                                    OTU_TEXT_FIELD(block, block->index, 0x04) = 0x11;
                                }
                                break;
                            case 7:
                            case 8:
                            case 9:
                            case 10:
                            case 11:
                            case 12:
                            case 13:
                            case 14:
                            case 15:
                            case 16:
                                OTU_TEXT_FIELD(block, block->index, 0x04) += 1;
                                if (OTU_TEXT_FIELD(block, block->index, 0x04) < 0) {
                                    OTU_TEXT_FIELD(block, block->index, 0x04) = 0xF;
                                }
                                break;
                            case 17:
                                OTU_TEXT_FIELD(block, block->index, 0x04) += 1;
                                if (OTU_TEXT_FIELD(block, block->index, 0x04) < 0) {
                                    OTU_TEXT_FIELD(block, block->index, 0x04) = 0x3;
                                }
                                break;
                            case 18:
                                OTU_TEXT_FIELD(block, block->index, 0x04) += 1;
                                if (OTU_TEXT_FIELD(block, block->index, 0x04) < 0) {
                                    OTU_TEXT_FIELD(block, block->index, 0x04) = 0x7;
                                }
                                break;
                            case 19:
                                OTU_TEXT_FIELD(block, block->index, 0x04) += 1;
                                if (OTU_TEXT_FIELD(block, block->index, 0x04) < 0) {
                                    OTU_TEXT_FIELD(block, block->index, 0x04) = 0xB;
                                }
                                break;
                            case 20:
                                OTU_TEXT_FIELD(block, block->index, 0x04) += 1;
                                if (OTU_TEXT_FIELD(block, block->index, 0x04) < 0) {
                                    OTU_TEXT_FIELD(block, block->index, 0x04) = 0x3;
                                }
                                break;
                        }
                        break;
                    case 1:
                        step = row->row;
                        switch (step) {
                            default:
                                break;
                            case 0:
                                OTU_TEXT_FIELD(block, block->index, 0x04) += 1;
                                if (OTU_TEXT_FIELD(block, block->index, 0x04) < 0) {
                                    OTU_TEXT_FIELD(block, block->index, 0x04) = 0xB;
                                }
                                break;
                            case 1:
                                OTU_TEXT_FIELD(block, block->index, 0x04) += 1;
                                if (OTU_TEXT_FIELD(block, block->index, 0x04) < 0) {
                                    OTU_TEXT_FIELD(block, block->index, 0x04) = 0xF;
                                }
                                break;
                            case 2:
                                OTU_TEXT_FIELD(block, block->index, 0x04) += 1;
                                if (OTU_TEXT_FIELD(block, block->index, 0x04) < 0) {
                                    OTU_TEXT_FIELD(block, block->index, 0x04) = 0x3;
                                }
                                break;
                            case 3:
                                OTU_TEXT_FIELD(block, block->index, 0x04) += 1;
                                if (OTU_TEXT_FIELD(block, block->index, 0x04) < 0) {
                                    OTU_TEXT_FIELD(block, block->index, 0x04) = 0x9;
                                }
                                break;
                        }
                        break;
                    case 2:
                        step = row->row;
                        switch (step) {
                            default:
                                break;
                            case 0:
                                OTU_TEXT_FIELD(block, block->index, 0x04) += 1;
                                if (OTU_TEXT_FIELD(block, block->index, 0x04) < 0) {
                                    OTU_TEXT_FIELD(block, block->index, 0x04) = 0x2;
                                }
                                break;
                            case 1:
                                OTU_TEXT_FIELD(block, block->index, 0x04) += 1;
                                if (OTU_TEXT_FIELD(block, block->index, 0x04) < 0) {
                                    OTU_TEXT_FIELD(block, block->index, 0x04) = 0x7;
                                }
                                break;
                            case 2:
                                OTU_TEXT_FIELD(block, block->index, 0x04) += 1;
                                if (OTU_TEXT_FIELD(block, block->index, 0x04) < 0) {
                                    OTU_TEXT_FIELD(block, block->index, 0x04) = 0x1;
                                }
                                break;
                            case 3:
                                OTU_TEXT_FIELD(block, block->index, 0x04) += 1;
                                if (OTU_TEXT_FIELD(block, block->index, 0x04) < 0) {
                                    OTU_TEXT_FIELD(block, block->index, 0x04) = 0x3;
                                }
                                break;
                            case 4:
                            case 5:
                                OTU_TEXT_FIELD(block, block->index, 0x04) += 1;
                                if (OTU_TEXT_FIELD(block, block->index, 0x04) < 0) {
                                    OTU_TEXT_FIELD(block, block->index, 0x04) = 0x5;
                                }
                                break;
                            case 6:
                                OTU_TEXT_FIELD(block, block->index, 0x04) += 1;
                                if (OTU_TEXT_FIELD(block, block->index, 0x04) < 0) {
                                    OTU_TEXT_FIELD(block, block->index, 0x04) = 0xB;
                                }
                                break;
                            case 7:
                                OTU_TEXT_FIELD(block, block->index, 0x04) += 1;
                                if (OTU_TEXT_FIELD(block, block->index, 0x04) < 0) {
                                    OTU_TEXT_FIELD(block, block->index, 0x04) = 0x2;
                                }
                                break;
                        }
                        break;
                }
            }
            break;

        case 0x1:
            // Start the menu. The very first row of the very first menu hands
            // over to the stage dispatch instead, which is how the sequence
            // leaves this screen.
            row = OTU_TEXT_ROW(block, block->index);

            if (row->value < 0) {
                if (block->index == 0 && row->row == 0) {
                    func_ov039_02098a40(OTU_STAGE(scene), &OtuScene_WirelessBoard);
                } else {
                    row->value = 0;
                }
            }
            break;

        case 0x2:
            // Reset the current row, or hand back to the dispatch if it was
            // already clear.
            row = OTU_TEXT_ROW(block, block->index);

            if (row->value >= 0) {
                row->value = -1;
            } else {
                func_ov039_02098a40(OTU_STAGE(scene), NULL);
            }
            break;

        case 0x200:
            // Step to the previous menu and force a full redraw of it.
            block->index -= 1;

            if (block->index < 0) {
                block->index = 2;
            }

            block->flags    = 0;
            row             = OTU_TEXT_ROW(block, block->index);
            row->drawnValue = -2;
            row->drawnRow   = -2;
            func_ov039_02086174(scene);
            break;

        case 0x100:
            // Step to the next menu and force a full redraw of it.
            block->index += 1;

            if (block->index >= 3) {
                block->index = 0;
            }

            block->flags    = 0;
            row             = OTU_TEXT_ROW(block, block->index);
            row->drawnValue = -2;
            row->drawnRow   = -2;
            func_ov039_02086174(scene);
            break;

        default:
            break;
    }

    func_ov039_02083bb0(scene);

    menu = block->index;
    step = OTU_TEXT_ROW(block, menu)->row;

    switch (menu) {
        case 0:
            switch (step) {
                default:
                    break;
                case 1:
                    func_ov039_02083ed4(scene);
                    break;
                case 2:
                    func_ov039_020840c0(scene);
                    break;
                case 3:
                case 4:
                case 5:
                case 6:
                    func_ov039_020842bc(scene, step - 3);
                    break;
                case 7:
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                    func_ov039_020844f8(scene, step - 7);
                    break;
                case 17:
                    func_ov039_0208462c(scene);
                    break;
                case 18:
                    func_ov039_02084738(scene);
                    break;
                case 19:
                    func_ov039_02084874(scene);
                    break;
                case 20:
                    func_ov039_020849a4(scene);
                    break;
            }
            break;
        case 1:
            switch (step) {
                default:
                    break;
                case 0:
                    func_ov039_02084ac4(scene);
                    break;
                case 1:
                    func_ov039_02084c34(scene);
                    break;
                case 2:
                    func_ov039_02084d80(scene);
                    break;
                case 3:
                    func_ov039_02084f08(scene);
                    break;
            }
            break;
        case 2:
            switch (step) {
                default:
                    break;
                case 0:
                    func_ov039_020851bc(scene);
                    break;
                case 1:
                    func_ov039_02085388(scene, block->trayCount);
                    break;
                case 2:
                    func_ov039_020855e0(scene, block->trayCount);
                    break;
                case 3:
                    func_ov039_02085770(scene, block->trayCount);
                    break;
                case 4:
                    func_ov039_020858e4(scene, block->trayCount);
                    break;
                case 5:
                    func_ov039_02085b30(scene, block->trayCount);
                    break;
                case 6:
                    func_ov039_02085e54(scene, block->trayCount);
                    break;
                case 7:
                    func_ov039_02086060(scene, block->trayCount);
                    break;
            }
            break;
    }

    block->flags++;
}

void func_ov039_02087dc0(TinPinSlammer_Scene* scene, void* data) {
    OtuTextRow* block = func_ov039_02098b70(OTU_STAGE(scene));
    s32         i;

    /* The child count/IDs are accessed via macros here (raw offsets) because the
     * countdown stage block's full layout isn't exposed as a shared struct type
     * without pulling in conflicting local types. The offsets match the common
     * stage layout. */
    for (i = 0; i < OTU_CHILD_COUNT(block); i++) {
        void* child = EasyTask_GetTaskData(OTU_POOL2(scene), OTU_CHILD_ID(block, i));

        if (child != data) {
            func_ov039_0208f0f0(child, data);
        }
    }
}

/* ==================================================================== */
/* The value cursor.                                                      */
/* ==================================================================== */

/**
 * Point a cursor at `table` and publish its first element into `row`.
 *
 * All three element reads go back through the cursor rather than through the
 * `table` argument, and that is load-bearing: the target stores the base and the
 * zero index, then *re-loads both out of the cursor* to index the array. Reading
 * through `table` instead lets mwcc keep the argument in a register and drops
 * four loads, along with the register the target needs for the 12-byte stride.
 */
void func_ov039_02087ba0(OtuCountdownCursor* cursor, OtuCountdownValue* table, u16 limit, OtuTextRow* row) {
    cursor->base  = table;
    cursor->index = 0;
    cursor->limit = limit;
    cursor->hold  = cursor->base[cursor->index].hold;

    row->value      = cursor->base[cursor->index].value;
    row->drawnValue = cursor->base[cursor->index].drawnValue;
}

/**
 * Publish the cursor's current element into `row`, then advance if it is spent.
 *
 * The advance is a countdown rather than a table walk: `hold` falls by one each
 * call, and only when it reaches zero does the index move on and take a fresh
 * `hold` from the element it lands on. The wrap test at the end is unsigned, so
 * an element count of zero wraps immediately and a count larger than the table
 * runs off the end -- both are the target's behaviour, not guards.
 */
void func_ov039_02087bf8(OtuCountdownCursor* cursor, OtuTextRow* row) {
    row->value      = cursor->base[cursor->index].value;
    row->drawnValue = cursor->base[cursor->index].drawnValue;

    if (cursor->index >= cursor->limit - 1) {
        return;
    }

    cursor->hold--;

    if (cursor->hold != 0) {
        return;
    }

    cursor->index++;

    if ((u32)cursor->index >= (u32)cursor->limit) {
        cursor->index = 0;
    }

    cursor->hold = cursor->base[cursor->index].hold;
}

/* ==================================================================== */
/* Row rendering.                                                         */
/* ==================================================================== */

/**
 * Draw the pin-count row's three-digit count for `slot`.
 *
 * This is the same "over-declared buffer" trick the OtuScoreRow.c builders use:
 * only four bytes are written, but the target's stack frame is 0x20 and sizing
 * the buffer to four shrinks the frame and costs the prologue an instruction.
 * The store order matters too -- the two leading digits go in after the
 * trailing space, and the space is overwritten with the terminator only after
 * the blanking check, so a blanked column never lands on the terminator.
 */
void func_ov039_02086060(TinPinSlammer_Scene* scene, s32 slot) {
    OtuTextRow* block = func_ov039_02098b70(OTU_STAGE(scene));
    s32         value = OTU_DIGIT_COLUMN(scene, slot)->unk_18;
    char        row[0x20];
    s32         column;

    row[0] = '0' + (value / 100) % 10;
    row[1] = '0' + (value / 10) % 10;
    row[2] = '0' + value % 10;
    row[3] = ' ';

    column = OTU_TEXT_ROW(block, block->index)->value;

    if (column >= 0 && (block->flags & 0x20)) {
        row[data_ov039_02098e50[column]] = ' ';
    }

    row[3] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, 0x40, row);
}

/* ==================================================================== */
/* Scene setup and the wireless branch.                                   */
/* ==================================================================== */

/**
 * Clear the three result rows and start the countdown, fading the scene in.
 *
 * The row table is reset to the "nothing drawn yet" pattern -- value and
 * drawnValue both -1, row 0, and a drawn row of 1 -- which is what makes the
 * first frame after this differ from every later one. The two base fields at
 * +0x34 and +0x38 are then zeroed, clearing the column-blanking flag and the
 * tray counter, so no column starts out hidden.
 */
void func_ov039_02086728(TinPinSlammer_Scene* scene) {
    // 8 bytes, never written, handed to func_0203a96c by address. The frame is
    // the only reason it exists; the target allocates it and passes a pointer
    // into it without ever storing to it.
    char              pad[0x8];
    OtuCountdownHead* head  = OTU_COUNTDOWN_HEAD(scene);
    OtuTextRow*       block = (OtuTextRow*)head;
    s32               i;

    Display_SetMainLayers(0);
    Display_SetSubLayers(0x11);

    g_DisplaySettings.engineState[1].blendMode   = 0;
    g_DisplaySettings.engineState[1].blendLayer0 = 0;
    g_DisplaySettings.engineState[1].blendLayer1 = 0x20;

    head->menu = 0;

    for (i = 0; i < 3; i++) {
        OtuTextRow* row = OTU_TEXT_ROW(block, i);

        row->value      = -1;
        row->drawnValue = -1;
        row->row        = 0;
        row->drawnRow   = 1;
    }

    block->flags     = 0;
    block->trayCount = 0;

    func_ov039_02082520(scene);
    func_ov039_02086174(scene);
    func_0203a96c(pad);
    EasyFade_FadeBothDisplays(1, 0, 0x1000);
}

/**
 * Branch on the wireless link state.
 *
 * A `switch`, not an if/else chain: the target does all six compares up front
 * and keeps the bodies out of line, and the case values are the wireless status
 * codes. Cases 0 and 10 share a body, as do 2/4/5/6/7 and 8/9 -- so this is one
 * function that turns a link status into "do nothing", "arm the stage" or
 * "hand over to the wireless stack", and the sharing is the point.
 */
void func_ov039_02087b04(TinPinSlammer_Scene* scene) {
    switch (func_ov040_0209cb78()) {
        case 0:
        case 10:
            func_ov039_02098a50(OTU_STAGE(scene));
            break;
        case 1:
            func_ov040_0209d6cc();
            break;
        case 8:
        case 9:
            func_ov040_0209d540();
            break;
        default:
            func_ov040_0209d588();
            break;
    }
}

/* Two empty stage slots.  Both are bare `bx lr`, so r0 is whatever the caller
 * left there -- there is no return statement to reproduce, and adding one would
 * be the mismatch. */
void func_ov039_02087ac4(void) {}

void func_ov039_02087ac8(void) {}

/* ==================================================================== */
/* Audio.                                                                */
/* ==================================================================== */

/**
 * Play a sound effect panned and attenuated by how far `from` is from `to`.
 *
 * The pan comes straight from the x difference and the volume from the vector
 * unit's distance score, both clamped into 0..255 -- and clamped in opposite
 * orders, which is why they are two separate if/else chains rather than a
 * shared clamp helper. The two anchors on the stack are the shared constants
 * the overlay's distance helpers take; both are written here rather than being
 * passed in because the target builds them on the stack.
 */
void func_ov039_02087d04(s32 se, OtuPoint* from, OtuPoint* to) {
    // `anchor.y` is stored and never read -- the target passes the same address
    // as both trailing arguments. Kept, because the store is in the ROM and
    // dropping it is the mismatch.
    OtuPoint anchor;
    s32      distance;
    s32      pan;
    s32      maxVolume;

    anchor.x = 0x80000;
    anchor.y = 0x60000;

    func_ov039_02098b8c(to, &anchor, &anchor);

    distance = func_ov039_02098ca8(from, &anchor) >> 0xC;
    distance = (distance + ((u32)(distance >> 2) >> 0x1D)) >> 3;

    pan = (from->x - to->x) >> 0xC;

    if (pan > 0xFF) {
        pan = 0xFF;
    }

    if (pan < 0) {
        pan = 0;
    }

    SndMgr_StartPlayingSE(se);
    maxVolume = SndMgr_GetSeIdxVolume(se);

    if (distance > maxVolume) {
        distance = maxVolume;
    }

    if (distance < 0) {
        distance = 0;
    }

    func_02027170(se, maxVolume - distance);
    SndMgr_UpdateSEPan(se, pan);
}

/* ==================================================================== */
/* A note on the Task TU template, since it was considered and rejected.  */
/* ==================================================================== */
/*
 * The Task TU shape in this repo -- two local structs, a TaskHandle, four
 * lifecycle callbacks, a RunTask dispatching through a local `TaskStages`, and a
 * CreateTask calling EasyTask_CreateTask once -- describes the overlay's
 * *simulation* tasks: the pin sprite tasks in OtuPinSprites, the sprite tasks in
 * bands 4 and 5.
 *
 * This band has none of that. There is no TaskHandle, so nothing is created
 * through EasyTask_CreateTask; the four functions that look like stages
 * (02087ac4, 02087ac8, and the two entries in 02086728's sequence) are called
 * from a plain sequential list inside 02086174 and 02086808, which is a
 * different thing with a different shape. Refactoring these into the template
 * would mean inventing a Task that does not exist and inventing four lifecycle
 * callbacks the code has no use for.
 *
 * The template is still the right tool for the rest of the overlay's
 * simulation, and it is worth using there.
 */
