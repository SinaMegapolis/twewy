#include "Engine/Core/System.h"
#include "Engine/Text.h"
#include "OtuFieldAccessShared.h"

/**
 * @file OtuCountdown.c
 * @brief The debug editor stage (OtuScene_FirstStage) -- its enter, layout,
 *        per-frame input and exit -- and helpers of the wireless stages.
 *
 * Named before the editor was identified; see OtuEditor and OtuScoreRow.c.
 */

/**
 * The editor's titles and row labels, defined in OtuMenuText.c with the rest
 * of that `.data` range. The SJIS ones are listed by their bytes.
 */
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

/** Sets the wireless stages' handshake step (OtuBoardStage.step). */
void func_ov039_02087c8c(TinPinSlammer_Scene* scene, s32 step) {
    ((OtuBoardStage*)func_ov039_02098b70(OTU_STAGE(scene)))->step = step;
}

/**
 * The link's receive callback: stores player `slot`'s deck and marks that
 * player ready.
 */
void func_ov039_02087cac(TinPinSlammer_Scene* scene, s32 slot, const void* src, s32 len) {
    OtuBoardStage* stage = (OtuBoardStage*)func_ov039_02098b70(OTU_STAGE(scene));

    MI_CpuCopyU8(src, scene->decks[slot], len);
    stage->readyMask |= 1 << slot;
}

/** Moves the stage on and queues no next stage, which ends the scene. */
void func_ov039_02087b74(TinPinSlammer_Scene* scene) {
    func_ov039_02098a50(OTU_STAGE(scene));
    func_ov039_02098a40(OTU_STAGE(scene), 0);
}

/** Fades the scene in, and moves the stage on once the fade has finished. */
void func_ov039_02087acc(TinPinSlammer_Scene* scene) {
    EasyFade_FadeBothDisplays(0, 0x10, 0x1000);

    if (EasyFade_IsFading()) {
        return;
    }

    func_ov039_02098a50(OTU_STAGE(scene));
}

/** The editor stage's exit: releases its text layer and fades out. */
void func_ov039_020867d4(TinPinSlammer_Scene* scene) {
    func_ov039_02098b70(OTU_STAGE(scene));

    func_ov039_020825b0(scene);
    EasyFade_FadeBothDisplays(2, 0x10, 0x1000);
}

/** Redraws the editor's current page: its title, then each row's label and value. */
void func_ov039_02086174(TinPinSlammer_Scene* scene) {
    OtuEditor* block = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));

    func_02010b50(OTU_TEXT(scene));

    switch (block->page) {
        case 0:
            Text_RenderToScreen(OTU_TEXT(scene), 0, 0, Otu_str_0209a178);
            func_ov039_02083bb0(scene);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x08, Otu_str_0209a188);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x10, Otu_str_0209a194);
            func_ov039_02083ed4(scene);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x18, Otu_str_0209a1a0);
            func_ov039_020840c0(scene);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x20, Otu_str_0209a1a8);
            func_ov039_020842bc(scene, 0);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x28, Otu_str_0209a1b0);
            func_ov039_020842bc(scene, 1);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x30, Otu_str_0209a1b8);
            func_ov039_020842bc(scene, 2);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x38, Otu_str_0209a1c0);
            func_ov039_020842bc(scene, 3);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x40, Otu_str_0209a1c8);
            func_ov039_020844f8(scene, 0);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x48, Otu_str_0209a1d0);
            func_ov039_020844f8(scene, 1);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x50, Otu_str_0209a1d8);
            func_ov039_020844f8(scene, 2);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x58, Otu_str_0209a1e0);
            func_ov039_020844f8(scene, 3);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x60, Otu_str_0209a1e8);
            func_ov039_020844f8(scene, 4);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x68, Otu_str_0209a1f0);
            func_ov039_020844f8(scene, 5);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x70, Otu_str_0209a1f8);
            func_ov039_020844f8(scene, 6);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x78, Otu_str_0209a200);
            func_ov039_020844f8(scene, 7);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x80, Otu_str_0209a208);
            func_ov039_020844f8(scene, 8);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x88, Otu_str_0209a210);
            func_ov039_020844f8(scene, 9);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x90, Otu_str_0209a218);
            func_ov039_0208462c(scene);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x98, Otu_str_0209a224);
            func_ov039_02084738(scene);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0xA0, Otu_str_0209a22c);
            func_ov039_02084874(scene);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0xA8, Otu_str_0209a234);
            func_ov039_020849a4(scene);
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
            func_ov039_02085388(scene, block->pinId);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x18, Otu_str_0209a2a0);
            func_ov039_020855e0(scene, block->pinId);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x20, Otu_str_0209a2a8);
            func_ov039_02085770(scene, block->pinId);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x28, Otu_str_0209a2b4);
            func_ov039_020858e4(scene, block->pinId);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x30, Otu_str_0209a2c0);
            func_ov039_02085b30(scene, block->pinId);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x38, Otu_str_0209a2c8);
            func_ov039_02085e54(scene, block->pinId);
            Text_RenderToScreen(OTU_TEXT(scene), 8, 0x40, Otu_str_0209a2d4);
            func_ov039_02086060(scene, block->pinId);
            return;

        default:
            return;
    }
}

/**
 * The editor's per-frame input. Up and down move the cursor between rows, or
 * step the selected digit once one is selected; left and right move between
 * digits; A selects the row (or, on SINGLE MENU 1's first row, starts a match)
 * and B deselects it (or leaves); L and R turn the page. Then redraws the
 * cursor and the current row, and advances the blink.
 */
void func_ov039_02086808(TinPinSlammer_Scene* scene) {
    OtuEditor* block = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    s32        step;

    switch (SysControl.buttonState.pressedButtons) {
        case 0x40:

            if (block->cursor[block->page].digit < 0) {
                // No digit selected: move the cursor up a row, wrapping to the last.
                switch (block->page) {
                    case 0:
                        block->cursor[block->page].row -= 1;
                        if (block->cursor[block->page].row < 0) {
                            block->cursor[block->page].row = 0x14;
                        }
                        break;
                    case 1:
                        block->cursor[block->page].row -= 1;
                        if (block->cursor[block->page].row < 0) {
                            block->cursor[block->page].row = 0x3;
                        }
                        break;
                    case 2:
                        block->cursor[block->page].row -= 1;
                        if (block->cursor[block->page].row < 0) {
                            block->cursor[block->page].row = 0x7;
                        }
                        break;
                }
            } else {
                // A digit is selected: step it up.
                switch (block->page) {
                    case 0:
                        step = block->cursor[block->page].row;
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
                        step = block->cursor[block->page].row;
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
                        step = block->cursor[block->page].row;
                        switch (step) {
                            default:
                                break;
                            case 0:
                                func_ov039_02085124(scene, 1);
                                func_ov039_02085388(scene, block->pinId);
                                func_ov039_020855e0(scene, block->pinId);
                                func_ov039_02085770(scene, block->pinId);
                                func_ov039_020858e4(scene, block->pinId);
                                func_ov039_02085b30(scene, block->pinId);
                                func_ov039_02085e54(scene, block->pinId);
                                func_ov039_02086060(scene, block->pinId);
                                break;
                            case 1:
                                func_ov039_020852c0(scene, block->pinId, 1);
                                break;
                            case 2:
                                func_ov039_0208554c(scene, block->pinId, 1);
                                break;
                            case 3:
                                func_ov039_020856c8(scene, block->pinId, 1);
                                break;
                            case 4:
                                func_ov039_02085818(scene, block->pinId, 1);
                                break;
                            case 5:
                                func_ov039_02085a64(scene, block->pinId, 1);
                                break;
                            case 6:
                                func_ov039_02085cb0(scene, block->pinId, 1);
                                break;
                            case 7:
                                func_ov039_02085fb4(scene, block->pinId, 1);
                                break;
                        }
                        break;
                }
            }
            break;

        case 0x80:

            if (block->cursor[block->page].digit < 0) {
                // No digit selected: move the cursor down a row, wrapping to the first.
                switch (block->page) {
                    case 0:
                        block->cursor[block->page].row += 1;
                        if (block->cursor[block->page].row >= 21) {
                            block->cursor[block->page].row = 0;
                        }
                        break;
                    case 1:
                        block->cursor[block->page].row += 1;
                        if (block->cursor[block->page].row >= 4) {
                            block->cursor[block->page].row = 0;
                        }
                        break;
                    case 2:
                        block->cursor[block->page].row += 1;
                        if (block->cursor[block->page].row >= 8) {
                            block->cursor[block->page].row = 0;
                        }
                        break;
                }
            } else {
                // A digit is selected: step it down.
                switch (block->page) {
                    case 0:
                        step = block->cursor[block->page].row;
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
                        step = block->cursor[block->page].row;
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
                        step = block->cursor[block->page].row;
                        switch (step) {
                            default:
                                break;
                            case 0:
                                func_ov039_02085124(scene, -1);
                                func_ov039_02085388(scene, block->pinId);
                                func_ov039_020855e0(scene, block->pinId);
                                func_ov039_02085770(scene, block->pinId);
                                func_ov039_020858e4(scene, block->pinId);
                                func_ov039_02085b30(scene, block->pinId);
                                func_ov039_02085e54(scene, block->pinId);
                                func_ov039_02086060(scene, block->pinId);
                                break;
                            case 1:
                                func_ov039_020852c0(scene, block->pinId, -1);
                                break;
                            case 2:
                                func_ov039_0208554c(scene, block->pinId, -1);
                                break;
                            case 3:
                                func_ov039_020856c8(scene, block->pinId, -1);
                                break;
                            case 4:
                                func_ov039_02085818(scene, block->pinId, -1);
                                break;
                            case 5:
                                func_ov039_02085a64(scene, block->pinId, -1);
                                break;
                            case 6:
                                func_ov039_02085cb0(scene, block->pinId, -1);
                                break;
                            case 7:
                                func_ov039_02085fb4(scene, block->pinId, -1);
                                break;
                        }
                        break;
                }
            }
            break;

        case 0x20:
            if (block->cursor[block->page].digit >= 0) {
                // Move the cursor one digit left, wrapping to the row's last digit.
                switch (block->page) {
                    case 0:
                        step = block->cursor[block->page].row;
                        switch (step) {
                            default:
                                break;
                            case 1:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 8;
                                }
                                break;
                            case 2:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 15;
                                }
                                break;
                            case 3:
                            case 4:
                            case 5:
                            case 6:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 17;
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
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 15;
                                }
                                break;
                            case 17:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 3;
                                }
                                break;
                            case 18:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 7;
                                }
                                // fallthrough
                            case 19:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 11;
                                }
                                // fallthrough
                            case 20:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 3;
                                }
                                break;
                        }
                        break;
                    case 1:
                        step = block->cursor[block->page].row;
                        switch (step) {
                            default:
                                break;
                            case 0:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 11;
                                }
                                break;
                            case 1:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 15;
                                }
                                break;
                            case 2:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 3;
                                }
                                break;
                            case 3:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 9;
                                }
                                break;
                        }
                        break;
                    case 2:
                        step = block->cursor[block->page].row;
                        switch (step) {
                            default:
                                break;
                            case 0:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 2;
                                }
                                break;
                            case 1:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 7;
                                }
                                break;
                            case 2:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 1;
                                }
                                break;
                            case 3:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 3;
                                }
                                break;
                            case 4:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 5;
                                }
                                break;
                            case 5:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 5;
                                }
                                break;
                            case 6:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 11;
                                }
                                break;
                            case 7:
                                block->cursor[block->page].digit -= 1;
                                if (block->cursor[block->page].digit < 0) {
                                    block->cursor[block->page].digit = 2;
                                }
                                break;
                        }
                        break;
                }
            }
            break;

        case 0x10:
            if (block->cursor[block->page].digit >= 0) {
                // Move the cursor one digit right, wrapping to the row's first digit.
                switch (block->page) {
                    case 0:
                        step = block->cursor[block->page].row;
                        switch (step) {
                            default:
                                break;
                            case 1:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 9) {
                                    block->cursor[block->page].digit = 0;
                                }
                                break;
                            case 2:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 16) {
                                    block->cursor[block->page].digit = 0;
                                }
                                break;
                            case 3:
                            case 4:
                            case 5:
                            case 6:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 18) {
                                    block->cursor[block->page].digit = 0;
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
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 16) {
                                    block->cursor[block->page].digit = 0;
                                }
                                break;
                            case 17:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 4) {
                                    block->cursor[block->page].digit = 0;
                                }
                                break;
                            case 18:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 8) {
                                    block->cursor[block->page].digit = 0;
                                }
                                // fallthrough
                            case 19:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 12) {
                                    block->cursor[block->page].digit = 0;
                                }
                                // fallthrough
                            case 20:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 4) {
                                    block->cursor[block->page].digit = 0;
                                }
                                break;
                        }
                        break;
                    case 1:
                        step = block->cursor[block->page].row;
                        switch (step) {
                            default:
                                break;
                            case 0:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 12) {
                                    block->cursor[block->page].digit = 0;
                                }
                                break;
                            case 1:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 16) {
                                    block->cursor[block->page].digit = 0;
                                }
                                break;
                            case 2:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 4) {
                                    block->cursor[block->page].digit = 0;
                                }
                                break;
                            case 3:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 10) {
                                    block->cursor[block->page].digit = 0;
                                }
                                break;
                        }
                        break;
                    case 2:
                        step = block->cursor[block->page].row;
                        switch (step) {
                            default:
                                break;
                            case 0:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 3) {
                                    block->cursor[block->page].digit = 0;
                                }
                                break;
                            case 1:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 8) {
                                    block->cursor[block->page].digit = 0;
                                }
                                break;
                            case 2:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 2) {
                                    block->cursor[block->page].digit = 0;
                                }
                                break;
                            case 3:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 4) {
                                    block->cursor[block->page].digit = 0;
                                }
                                break;
                            case 4:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 6) {
                                    block->cursor[block->page].digit = 0;
                                }
                                break;
                            case 5:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 6) {
                                    block->cursor[block->page].digit = 0;
                                }
                                break;
                            case 6:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 12) {
                                    block->cursor[block->page].digit = 0;
                                }
                                break;
                            case 7:
                                block->cursor[block->page].digit += 1;
                                if (block->cursor[block->page].digit >= 3) {
                                    block->cursor[block->page].digit = 0;
                                }
                                break;
                        }
                        break;
                }
            }
            break;

        case 0x1:
            // Select the row's first digit; on SINGLE MENU 1's first row, start a match.

            if (block->cursor[block->page].digit < 0) {
                if (block->page == 0 && block->cursor[block->page].row == 0) {
                    func_ov039_02098a40(OTU_STAGE(scene), &OtuScene_WirelessBoard);
                } else {
                    block->cursor[block->page].digit = 0;
                }
            }
            break;

        case 0x2:
            // Deselect the digit, or leave the editor if none is selected.

            if (block->cursor[block->page].digit >= 0) {
                block->cursor[block->page].digit = -1;
            } else {
                func_ov039_02098a40(OTU_STAGE(scene), NULL);
            }
            break;

        case 0x200:
            // Previous page, fully redrawn.
            block->page -= 1;

            if (block->page < 0) {
                block->page = 2;
            }

            block->blink                          = 0;
            block->cursor[block->page].drawnDigit = -2;
            block->cursor[block->page].drawnRow   = -2;
            func_ov039_02086174(scene);
            break;

        case 0x100:
            // Next page, fully redrawn.
            block->page += 1;

            if (block->page >= 3) {
                block->page = 0;
            }

            block->blink                          = 0;
            block->cursor[block->page].drawnDigit = -2;
            block->cursor[block->page].drawnRow   = -2;
            func_ov039_02086174(scene);
            break;

        default:
            break;
    }

    func_ov039_02083bb0(scene);

    switch (block->page) {
        case 0:
            step = block->cursor[block->page].row;
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
            step = block->cursor[block->page].row;
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
            step = block->cursor[block->page].row;
            switch (step) {
                default:
                    break;
                case 0:
                    func_ov039_020851bc(scene);
                    break;
                case 1:
                    func_ov039_02085388(scene, block->pinId);
                    break;
                case 2:
                    func_ov039_020855e0(scene, block->pinId);
                    break;
                case 3:
                    func_ov039_02085770(scene, block->pinId);
                    break;
                case 4:
                    func_ov039_020858e4(scene, block->pinId);
                    break;
                case 5:
                    func_ov039_02085b30(scene, block->pinId);
                    break;
                case 6:
                    func_ov039_02085e54(scene, block->pinId);
                    break;
                case 7:
                    func_ov039_02086060(scene, block->pinId);
                    break;
            }
            break;
    }

    block->blink++;
}

/** Clears `badge` as the partner of every other badge. */
void func_ov039_02087dc0(TinPinSlammer_Scene* scene, void* badge) {
    OtuBoardStage* stage = (OtuBoardStage*)func_ov039_02098b70(OTU_STAGE(scene));
    s32            i;

    for (i = 0; i < stage->badgeCount; i++) {
        void* child = EasyTask_GetTaskData(OTU_POOL2(scene), stage->badgeIds[i]);

        if (child != badge) {
            func_ov039_0208f0f0(child, badge);
        }
    }
}

/** Starts `anim` at its first key and publishes that key's scales. */
void func_ov039_02087ba0(OtuScaleAnim* anim, const OtuScaleKey* keys, u16 count, OamAffineParam* affine) {
    anim->keys  = keys;
    anim->index = 0;
    anim->count = count;
    anim->hold  = anim->keys[anim->index].frames;

    affine->scaleX = anim->keys[anim->index].scaleX;
    affine->scaleY = anim->keys[anim->index].scaleY;
}

/**
 * Publishes the current key's scales, then counts its hold down and moves to
 * the next key, wrapping at `count`.
 */
void func_ov039_02087bf8(OtuScaleAnim* anim, OamAffineParam* affine) {
    affine->scaleX = anim->keys[anim->index].scaleX;
    affine->scaleY = anim->keys[anim->index].scaleY;

    if (anim->index >= anim->count - 1) {
        return;
    }

    anim->hold--;

    if (anim->hold != 0) {
        return;
    }

    anim->index++;

    if ((u32)anim->index >= (u32)anim->count) {
        anim->index = 0;
    }

    anim->hold = anim->keys[anim->index].frames;
}

/** Draws SINGLE MENU 3's stun-time row. */
void func_ov039_02086060(TinPinSlammer_Scene* scene, s32 slot) {
    OtuEditor* block = func_ov039_02098b70(OTU_STAGE(scene));
    s32        value = scene->badgeParams[slot].stunFrames;
    char       row[0x20];
    s32        column;

    row[0] = '0' + (value / 100) % 10;
    row[1] = '0' + (value / 10) % 10;
    row[2] = '0' + value % 10;
    row[3] = ' ';

    column = block->cursor[block->page].digit;

    if (column >= 0 && (block->blink & 0x20)) {
        row[data_ov039_02098e50[column]] = ' ';
    }

    row[3] = 0;

    Text_RenderToScreen(OTU_TEXT(scene), 0x38, 0x40, row);
}

/**
 * The editor stage's enter: sets up the display and the text layer, resets
 * the cursors and draws the first page.
 */
void func_ov039_02086728(TinPinSlammer_Scene* scene) {
    // Never written; func_0203a96c is only handed its address.
    char       pad[0x8];
    OtuEditor* block = (OtuEditor*)func_ov039_02098b70(OTU_STAGE(scene));
    s32        i;

    Display_SetMainLayers(0);
    Display_SetSubLayers(0x11);

    g_DisplaySettings.engineState[1].blendMode   = 0;
    g_DisplaySettings.engineState[1].blendLayer0 = 0;
    g_DisplaySettings.engineState[1].blendLayer1 = 0x20;

    block->page = 0;

    for (i = 0; i < 3; i++) {
        block->cursor[i].digit      = -1;
        block->cursor[i].drawnDigit = -1;
        block->cursor[i].row        = 0;
        block->cursor[i].drawnRow   = 1;
    }

    block->blink = 0;
    block->pinId = 0;

    func_ov039_02082520(scene);
    func_ov039_02086174(scene);
    func_0203a96c(pad);
    EasyFade_FadeBothDisplays(1, 0, 0x1000);
}

/** Reacts to the wireless link's state. */
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

/** Empty stage steps. */
void func_ov039_02087ac4(void) {}

void func_ov039_02087ac8(void) {}

/**
 * Plays sound effect `se`, panned by the x distance from `to` to `from` and
 * attenuated by their distance.
 */
void func_ov039_02087d04(s32 se, OtuPoint* from, OtuPoint* to) {
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
