#include "ShopPart.h"

// Shop.c is the main Shop menu dispatcher (overlay ov043).
//
// The shared context (includes, the root `ShopState` type, data externs and the
// inter-function prototypes) lives in include/ShopPart.h; the functions below
// are grouped in address order.

// ---- Shop_p1.c ----
void func_ov043_020ac5d8(ShopState *arg0) {
    typedef struct {
        char                pad_0[0x738];
        ShopItemTableEntry* unk_738[12];
        char                pad_768[0x774 - 0x768];
        u16                 unk_774;
        char                pad_776[0x788 - 0x776];
        u16                 unk_788;
        u16                 unk_78A;
        u16                 unk_78C;
        char                pad_78E[0x7B4 - 0x78E];
        u8                  unk_7B4;
        u8                  unk_7B5;
        u8                  unk_7B6;
        u8                  unk_7B7;
        char                pad_7B8[0x800 - 0x7B8];
        u16                 unk_800;
        char                pad_802[0x834 - 0x802];
        u16                 unk_834;
        u8                  unk_836;
        u8                  unk_837;
        u8                  unk_838;
        u8                  unk_839;
        u16                 unk_83A;
        s16                 unk_83C;
        u16                 unk_83E;
        s16                 unk_840;
        s16                 unk_842;
        u16                 unk_844;
        u16                 unk_846;
        u16                 unk_848;
        u16                 unk_84A;
        u16                 unk_84C;
        u16                 unk_84E;
        u16                 unk_850;
        u16                 unk_852;
        u8                  unk_854;
        u8                  unk_855;
        char                pad_856[0x858 - 0x856];
        u32                 unk_858;
        char                pad_85C[0x85E - 0x85C];
        u16                 unk_85E;
        u16                 unk_860;
        u16                 unk_862;
        u8                  unk_864[3];
        char                pad_867[0x868 - 0x867];
        u32                 unk_868;
        char                pad_86C[0x870 - 0x86C];
        u16                 unk_870;
        u16                 unk_872;
    } Shop1;

    Shop1 *shop;

    shop = (Shop1 *) &arg0->shop;
    arg0->unk_217AC = 0;
    arg0->unk_217B0 = 0;
    shop->unk_834 = 0;
    shop->unk_83A = 0;
    shop->unk_836 = 1;
    shop->unk_855 = 1;
    shop->unk_837 = 0;
    shop->unk_840 = 0;
    shop->unk_842 = 0;
    shop->unk_854 = 0;
    shop->unk_85E = 0;
    shop->unk_84C = 0xFFFF;
    shop->unk_84E = 0xFFFF;
    if (gSaveData.unk_1AB4 & 4) {
        func_ov043_020b1404((ShopObject *) shop);
    } else {
        func_ov043_020b0d1c((ShopObject *) shop);
    }
    shop->unk_844 = 0;
    shop->unk_848 = 0;
    shop->unk_850 = 0;
    shop->unk_852 = 0;
    shop->unk_868 = 0;
    shop->unk_838 = 0;
    shop->unk_839 = 0;
    shop->unk_84A = 0;
    shop->unk_864[0] = 0;
    shop->unk_864[1] = 0;
    shop->unk_864[2] = 0;
    arg0->unk_217B2 = 0;
}

void func_ov043_020ac6a0(ShopState *arg0, u16 arg1, u16 arg2, s32 arg3) {
    s32 (*fn)(TaskPool *, s32, u16, ShopObject *) = (s32 (*)(TaskPool *, s32, u16, ShopObject *)) Shop_windowU_CreateTask;

    arg0->taskId_WindowU = fn((TaskPool *) (&(arg0)->base.taskPool), arg0->base.dataType, (u16) (arg3 + ((arg1 * 0x15) + 0x303F + (arg2 * 7))), (ShopObject *) (&(arg0)->shop));
}

void func_ov043_020ac6f4(ShopState *arg0) {
    typedef struct {
        char pad_0[0x78C];
        u16  unk_78C;
        char pad_78E[0x844 - 0x78E];
        u16  unk_844;
        char pad_846[0x860 - 0x846];
        u16  unk_860;
    } Shop1f;
    ShopObject *shop;
    u16         var_r7;

    shop = &arg0->shop;
    EasyTask_CreateTask((TaskPool *) (&(arg0)->base.taskPool), &Task_EasyFade, NULL, 0, NULL, NULL);
    EasyFade_FadeBothDisplays(2, -0x10, 0x1000);
    arg0->taskId_Pointer = Shop_pointer_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, shop);
    arg0->taskId_NameD = Shop_nameD_CreateTask((TaskPool *) (&(arg0)->base.taskPool), arg0->base.dataType, shop);
    arg0->taskId_Exit = Shop_exit_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, shop);
    arg0->taskId_NumMoneyNek = Shop_numMoneyNek_CreateTask((TaskPool *) (&(arg0)->base.taskPool), arg0->base.dataType, shop);
    arg0->taskId_NumMoneyBdg = Shop_numMoneyBdg_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, shop);
    if ((u32) ((Shop1f *) shop)->unk_860 <= 0xCU) {
        arg0->taskId_Sbar = Shop_sbar2_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, shop);
    } else {
        arg0->taskId_Sbar = Shop_sbar_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, shop);
    }
    arg0->taskId_Check = Shop_check_CreateTask((TaskPool *) (&(arg0)->base.taskPool), arg0->base.dataType, shop);
    arg0->taskId_Tab = Shop_tab_CreateTask((TaskPool *) (&(arg0)->base.taskPool), arg0->base.dataType, shop);
    arg0->taskId_TextScr = Shop_textScr_CreateTask((TaskPool *) (&(arg0)->base.taskPool), arg0->base.dataType, shop);
    var_r7 = 0;
    do {
        arg0->taskId_Item[var_r7] = Shop_item_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, var_r7, shop);
        arg0->taskId_ItemPlate[var_r7] = Shop_itemPlate_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, var_r7, shop);
        arg0->taskId_ItemPrice[var_r7] = Shop_itemPrice_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, var_r7, shop);
        arg0->taskId_ItemMask[var_r7] = Shop_itemMask_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, var_r7, shop);
        arg0->taskId_ItemAbl[var_r7] = ((s32 (*)(TaskPool *, s32, u16, ShopObject *)) Shop_itemAbl_CreateTask)(&(arg0)->base.taskPool, arg0->base.dataType, var_r7, shop);
        arg0->taskId_ItemStar[var_r7] = ((s32 (*)(TaskPool *, s32, u16, ShopObject *)) Shop_itemStar_CreateTask)(&(arg0)->base.taskPool, arg0->base.dataType, var_r7, shop);
        var_r7 += 1;
    } while ((u32) var_r7 < 0xCU);
    arg0->taskId_ItemCur = Shop_itemCur_CreateTask((TaskPool *) (&(arg0)->base.taskPool), arg0->base.dataType, shop);
    arg0->taskId_ItemTotal = Shop_itemTotal_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, ((Shop1f *) shop)->unk_844, shop);
    arg0->taskId_ItemTotalNum = Shop_itemTotalNum_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, shop);
    arg0->taskId_BrdLogo = Shop_brdLogo_CreateTask((TaskPool *) (&(arg0)->base.taskPool), arg0->base.dataType, shop);
    arg0->taskId_NameU = Shop_nameU_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, shop);
    arg0->taskId_NumLvU = Shop_numLvU_CreateTask((TaskPool *) (&(arg0)->base.taskPool), arg0->base.dataType, shop);
    arg0->taskId_NumAblU = Shop_numAblU_CreateTask((TaskPool *) (&(arg0)->base.taskPool), arg0->base.dataType, shop);
    arg0->taskId_NumFsgU = Shop_numFsgU_CreateTask((TaskPool *) (&(arg0)->base.taskPool), arg0->base.dataType, shop);
    arg0->taskId_FsgGaugeU = Shop_fsgGaugeU_CreateTask((TaskPool *) (&(arg0)->base.taskPool), arg0->base.dataType, shop);
    arg0->taskId_TextScrU = Shop_textScrU_CreateTask((TaskPool *) (&(arg0)->base.taskPool), arg0->base.dataType, shop);
    arg0->taskId_BoardU = Shop_boardU_CreateTask((TaskPool *) (&(arg0)->base.taskPool), arg0->base.dataType, shop);
    arg0->taskId_CastU = Shop_castU_CreateTask((TaskPool *) (&(arg0)->base.taskPool), arg0->base.dataType, shop);
    func_ov043_020ac6a0(arg0, shop->unk_774, ((Shop1f *) shop)->unk_78C, 0);
}

void func_ov043_020acb08(ShopState *arg0) {
    ShopObject *shop;

    shop = &arg0->shop;
    EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->taskId_ItemTotal);
    EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->taskId_ItemTotalNum);
    EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->taskId_BrdLogo);
    shop->unk_85E = 0;
    shop->unk_834 |= 0x40;
    arg0->taskId_ItemTotal = Shop_itemTotal_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, shop->unk_844, shop);
    arg0->taskId_ItemTotalNum = Shop_itemTotalNum_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, shop);
    arg0->taskId_BrdLogo = Shop_brdLogo_CreateTask((TaskPool *) (&(arg0)->base.taskPool), arg0->base.dataType, shop);
}

// Nonmatching: register allocation: target keeps the running index and re-multiplies
void func_ov043_020acbd0(ShopObject *shop, u32 arg1) {
    ShopState *state;
    s16        var_r5;
    s16        var_r6;
    u32        var_r8;
    s16        var_r9;

    state = data_ov043_020cd288;
    var_r8 = arg1;
    var_r6 = 0;
    do {
        func_ov043_020b6dc8((TaskPool *) (&state->base.taskPool), state->taskId_Item[var_r6]);
        EasyTask_DeleteTask((TaskPool *) (&state->base.taskPool), state->taskId_Item[var_r6]);
        var_r6 += 1;
    } while (var_r6 < 0xC);
    var_r6 = 0;
    do {
        EasyTask_DeleteTask((TaskPool *) (&state->base.taskPool), state->taskId_ItemPlate[var_r6]);
        EasyTask_DeleteTask((TaskPool *) (&state->base.taskPool), state->taskId_ItemPrice[var_r6]);
        EasyTask_DeleteTask((TaskPool *) (&state->base.taskPool), state->taskId_ItemMask[var_r6]);
        EasyTask_DeleteTask((TaskPool *) (&state->base.taskPool), state->taskId_ItemAbl[var_r6]);
        var_r6 += 1;
    } while (var_r6 < 0xC);
    for (var_r5 = 0; var_r5 < 0xC; var_r5++) {
        shop->unk_738[var_r5] = (ShopItemTableEntry *) &shop->slots[arg1 + var_r5];
    }
    var_r9 = 0;
    do {
        u16 itemIndex = (u16) var_r9;

        state->taskId_Item[var_r9] = Shop_item_CreateTask((TaskPool *) (&state->base.taskPool), state->base.dataType, itemIndex, shop);
        state->taskId_ItemPlate[var_r9] = Shop_itemPlate_CreateTask(&(state)->base.taskPool, state->base.dataType, itemIndex, shop);
        state->taskId_ItemPrice[var_r9] = Shop_itemPrice_CreateTask(&(state)->base.taskPool, state->base.dataType, itemIndex, shop);
        state->taskId_ItemMask[var_r9] = Shop_itemMask_CreateTask(&(state)->base.taskPool, state->base.dataType, itemIndex, shop);
        state->taskId_ItemAbl[var_r9] = ((s32 (*)(TaskPool *, s32, u16, ShopObject *)) Shop_itemAbl_CreateTask)((TaskPool *) (&(state)->base.taskPool), state->base.dataType, itemIndex, shop);
        var_r9 += 1;
    } while (var_r9 < 0xC);
}

void func_ov043_020acd90(ShopState *arg0, ShopObject *arg1) {
    typedef struct {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ char pad_02[0x08 - 0x02];
        /* 0x08 */ u32 unk_08;
        /* 0x0C */ char pad_0C[0x10 - 0x0C];
        /* 0x10 */ s32 unk_10;
        /* 0x14 */ u16 unk_14;
    } ShopP1Entry;
    typedef struct {
        /* 0x000 */ char         pad_000[0x738];
        /* 0x738 */ ShopP1Entry *unk_738[12];
        /* 0x768 */ char         pad_768[0x774 - 0x768];
        /* 0x774 */ u16          unk_774;
        /* 0x776 */ u16          unk_776;
        /* 0x778 */ u16          unk_778;
        /* 0x77A */ u16          unk_77A;
        /* 0x77C */ u32          unk_77C;
        /* 0x780 */ u32          unk_780;
        /* 0x784 */ u32          unk_784;
        /* 0x788 */ u16          unk_788;
        /* 0x78A */ u16          unk_78A;
        /* 0x78C */ u16          unk_78C;
        /* 0x78E */ char         pad_78E[0x7B6 - 0x78E];
        /* 0x7B6 */ u8           unk_7B6;
        /* 0x7B7 */ char         pad_7B7[0x834 - 0x7B7];
        /* 0x834 */ u16          unk_834;
        /* 0x836 */ char         pad_836[0x83A - 0x836];
        /* 0x83A */ u16          unk_83A;
        /* 0x83C */ char         pad_83C[0x844 - 0x83C];
        /* 0x844 */ u16          unk_844;
        /* 0x846 */ char         pad_846[0x855 - 0x846];
        /* 0x855 */ u8           unk_855;
        /* 0x856 */ char         pad_856[0x858 - 0x856];
        /* 0x858 */ u32          unk_858;
    } ShopP1View;
    void func_ov043_020ad3c8(ShopState *arg0);
    ShopP1View  *shop;
    ShopP1Entry *entry;
    s32          var_r4;

    shop = (ShopP1View *) arg1;
    var_r4 = 0;
    shop->unk_858 -= shop->unk_738[shop->unk_844]->unk_08;
    if (shop->unk_738[shop->unk_844]->unk_08 >= 0x2710U) {
        Stats_AddExperience(1);
    }
    if (shop->unk_77C < shop->unk_784) {
        shop->unk_77C += shop->unk_738[shop->unk_844]->unk_08;
        if (shop->unk_77C > shop->unk_784) {
            shop->unk_77C = shop->unk_784;
        }
        if (shop->unk_77C >= shop->unk_780) {
            shop->unk_77C = shop->unk_780;
            shop->unk_778 += 1;
            if ((u32) shop->unk_778 > (u32) shop->unk_77A) {
                shop->unk_778 = shop->unk_77A;
            }
            shop->unk_780 = func_ov043_020af994(arg1);
            DebugOvlDisp_ReplaceTop((OverlayCB) func_ov043_020adbb0, arg0, 0);
            var_r4 = 1;
        }
    }
    func_ov043_020afa20(arg1);
    if (func_ov043_020b1e2c(arg1) == 1) {
        shop->unk_7B6 = 1;
    }
    entry = shop->unk_738[shop->unk_844];
    if (entry->unk_10 == 1) {
        func_02023d3c(entry->unk_14);
    }
    EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->taskId_WindowU);
    EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->taskId_ItemTotalNum);
    func_ov043_020ac6a0(arg0, shop->unk_774, shop->unk_78C, 2);
    arg0->taskId_ItemTotalNum = Shop_itemTotalNum_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, arg1);
    shop->unk_83A = 0x12C;
    shop->unk_834 |= 8;
    if ((shop->unk_834 & 4) && (var_r4 == 0)) {
        SndMgr_StartPlayingSE(0x11C);
    }
    if (var_r4 != 0) {
        return;
    }
    shop->unk_855 = 0;
    DebugOvlDisp_ReplaceTop((OverlayCB) func_ov043_020ad3c8, arg0, 0);
}

void func_ov043_020acf6c(ShopState *arg0) {
    typedef struct {
        /* 0x000 */ char pad_000[0x862];
        /* 0x862 */ u16  unk_862;
    } Shop1v;
    void       func_ov043_020ad9dc(ShopState *arg0);
    ShopObject *shop;
    u16         temp_r0;
    u16         temp_r1;
    u16         temp_r1_2;
    u16         temp_r1_3;
    u16         temp_r1_4;
    u16         temp_r1_5;
    u16         temp_r2;
    u16         temp_r2_2;
    u16         temp_r2_3;
    u16         temp_r2_4;
    u16         temp_r2_5;
    u16         temp_r3;
    u16         temp_r3_2;

    shop = &arg0->shop;
    if (!(InputStatus.buttonState.currButtons & 0xC03) && !(InputStatus.buttonState.currButtons & 0xF0)) {
        arg0->unk_217B2 = 0;
    }
    if (!(InputStatus.buttonState.pressedButtons & 0x300)) {
        temp_r2 = arg0->unk_217B2;
        if (temp_r2 != 0) {
            arg0->unk_217B2 = temp_r2 - 1;
            return;
        }
        if ((InputStatus.buttonState.pressedButtons & 0xC03) || (InputStatus.buttonState.pressedButtons & 0xF0)) {
            arg0->unk_217B2 = 0x14;
        } else if ((InputStatus.buttonState.currButtons & 0xC03) || (InputStatus.buttonState.currButtons & 0xF0)) {
            arg0->unk_217B2 = 1;
        }
        if ((InputStatus.buttonState.currButtons & 0x40) || (InputStatus.buttonState.currButtons & 0x400)) {
            temp_r2_2 = shop->unk_850;
            temp_r1 = shop->unk_848;
            if (((u32) temp_r1 >= (u32) temp_r2_2) && ((s32) temp_r1 < (s32) (temp_r2_2 + 0xC))) {
                if ((u32) temp_r1 >= 6U) {
                    SndMgr_StartPlayingSE(0x11A);
                    temp_r1_2 = shop->unk_844;
                    if ((u32) temp_r1_2 < 6U) {
                        shop->unk_850 -= 6;
                        shop->unk_848 -= 6;
                    } else {
                        shop->unk_844 = temp_r1_2 - 6;
                        shop->unk_848 -= 6;
                    }
                }
            } else {
                SndMgr_StartPlayingSE(0x11A);
                shop->unk_844 = 0;
                shop->unk_848 = shop->unk_850;
            }
            func_ov043_020acb08(arg0);
        } else if ((InputStatus.buttonState.currButtons & 0x80) || (InputStatus.buttonState.currButtons & 2)) {
            temp_r1_3 = shop->unk_850;
            temp_r2_3 = shop->unk_848;
            if (((u32) temp_r2_3 >= (u32) temp_r1_3) && ((s32) temp_r2_3 < (s32) (temp_r1_3 + 0xC))) {
                if ((s32) temp_r2_3 < (s32) (((Shop1v *) shop)->unk_862 - 6)) {
                    SndMgr_StartPlayingSE(0x11A);
                    temp_r1_4 = shop->unk_844;
                    if (((u32) temp_r1_4 >= 6U) && ((u32) temp_r1_4 < 0xCU)) {
                        shop->unk_850 += 6;
                        shop->unk_848 += 6;
                    } else {
                        shop->unk_844 += 6;
                        shop->unk_848 += 6;
                    }
                }
            } else {
                SndMgr_StartPlayingSE(0x11A);
                shop->unk_844 = 0;
                shop->unk_848 = shop->unk_850;
            }
            func_ov043_020acb08(arg0);
        } else if ((InputStatus.buttonState.currButtons & 0x20) || (InputStatus.buttonState.currButtons & 0x800)) {
            SndMgr_StartPlayingSE(0x11A);
            temp_r3 = shop->unk_850;
            temp_r2_4 = shop->unk_848;
            if (((u32) temp_r2_4 >= (u32) temp_r3) && ((s32) temp_r2_4 < (s32) (temp_r3 + 0xC))) {
                if (temp_r2_4 == 0) {
                    shop->unk_850 = shop->unk_870 * 6;
                    shop->unk_844 = 0xB;
                    shop->unk_848 = ((Shop1v *) shop)->unk_862 - 1;
                } else {
                    temp_r0 = shop->unk_844;
                    if (temp_r0 == 0) {
                        shop->unk_850 = temp_r3 - 6;
                        shop->unk_844 = 5;
                        shop->unk_848 -= 1;
                    } else {
                        shop->unk_844 = temp_r0 - 1;
                        shop->unk_848 -= 1;
                    }
                }
            } else {
                shop->unk_844 = 0;
                shop->unk_848 = shop->unk_850;
            }
            func_ov043_020acb08(arg0);
        } else if ((InputStatus.buttonState.currButtons & 0x10) || (InputStatus.buttonState.currButtons & 1)) {
            SndMgr_StartPlayingSE(0x11A);
            temp_r3_2 = shop->unk_850;
            temp_r2_5 = shop->unk_848;
            if (((u32) temp_r2_5 >= (u32) temp_r3_2) && ((s32) temp_r2_5 < (s32) (temp_r3_2 + 0xC))) {
                if (temp_r2_5 == (((Shop1v *) shop)->unk_862 - 1)) {
                    shop->unk_850 = 0;
                    shop->unk_844 = 0;
                    shop->unk_848 = 0;
                } else {
                    temp_r1_5 = shop->unk_844;
                    if (temp_r1_5 == 0xB) {
                        shop->unk_850 = temp_r3_2 + 6;
                        shop->unk_844 = 6;
                        shop->unk_848 += 1;
                    } else {
                        shop->unk_844 = temp_r1_5 + 1;
                        shop->unk_848 += 1;
                    }
                }
            } else {
                shop->unk_844 = 0;
                shop->unk_848 = shop->unk_850;
            }
            func_ov043_020acb08(arg0);
        }
        goto block_55;
    }
block_55:
    if (gSaveData.unk_1AB4 & 4) {
        return;
    }
    if (func_ov043_020afcb0(shop) != 1) {
        return;
    }
    DebugOvlDisp_ReplaceTop((OverlayCB) func_ov043_020ad9dc, arg0, 0);
}

void func_ov043_020ad354(ShopState *arg0) {
    typedef struct {
        char pad_0[0x83A];
        u16  unk_83A;
    } Shop1d;
    void     func_ov043_020ae24c(ShopState *);
    void     func_ov043_020ad738(ShopState *);
    Shop1d  *shop;

    shop = (Shop1d *) &arg0->shop;
    EasyFade_FadeBothDisplays(0, 0, 0x1000);
    if (EasyFade_IsFading() != 0) {
        return;
    }
    shop->unk_83A = 0x12C;
    if (gSaveData.unk_1AB4 & 4) {
        DebugOvlDisp_ReplaceTop((OverlayCB) func_ov043_020ae24c, arg0, 0);
        return;
    }
    DebugOvlDisp_ReplaceTop((OverlayCB) func_ov043_020ad738, arg0, 0);
}

void func_ov043_020ad3c8(ShopState *arg0) {
    typedef struct {
        /* 0x000 */ char                pad_000[0x738];
        /* 0x738 */ ShopItemTableEntry *unk_738[12];
        /* 0x768 */ char                pad_768[0x774 - 0x768];
        /* 0x774 */ u16                 unk_774;
        /* 0x776 */ char                pad_776[0x78C - 0x776];
        /* 0x78C */ u16                 unk_78C;
        /* 0x78E */ char                pad_78E[0x834 - 0x78E];
        /* 0x834 */ u16                 unk_834;
        /* 0x836 */ char                pad_836[0x837 - 0x836];
        /* 0x837 */ u8                  unk_837;
        /* 0x838 */ u8                  unk_838;
        /* 0x839 */ u8                  unk_839;
        /* 0x83A */ u16                 unk_83A;
        /* 0x83C */ s16                 unk_83C;
        /* 0x83E */ char                pad_83E[0x840 - 0x83E];
        /* 0x840 */ s16                 unk_840;
        /* 0x842 */ s16                 unk_842;
        /* 0x844 */ u16                 unk_844;
        /* 0x846 */ char                pad_846[0x848 - 0x846];
        /* 0x848 */ u16                 unk_848;
        /* 0x84A */ char                pad_84A[0x850 - 0x84A];
        /* 0x850 */ u16                 unk_850;
        /* 0x852 */ u16                 unk_852;
        /* 0x854 */ u8                  unk_854;
        /* 0x855 */ u8                  unk_855;
        /* 0x856 */ char                pad_856[0x858 - 0x856];
        /* 0x858 */ u32                 unk_858;
    } Shop1x;
    void func_ov043_020ad9dc(ShopState *arg0);
    Shop1x     *shop;
    TouchCoord  sp0;
    s16         temp_r1;
    u16         temp_r1_2;
    u16         temp_r1_3;

    shop = (Shop1x *) &arg0->shop;
    if (shop->unk_834 & 1) {
        TouchInput_GetCoord(&sp0);
        shop->unk_840 = (s16) sp0.x;
        shop->unk_842 = (s16) sp0.y;
        if (TouchInput_WasTouchReleased() != 0) {
            shop->unk_834 &= ~1;
            EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->unk_21768);
            EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->unk_2176C);
            if (func_ov043_020b18e4((s16) sp0.x, (s16) sp0.y) == 1) {
                SndMgr_StartPlayingSE(0x129);
                if ((u32) shop->unk_858 >= (u32) shop->unk_738[shop->unk_844]->unk_08) {
                    shop->unk_855 = 1;
                    DebugOvlDisp_ReplaceTop((OverlayCB) func_ov043_020adef8, arg0, 0);
                }
            }
        }
        goto block_20;
    }
    if (TouchInput_WasTouchPressed() != 0) {
        TouchInput_GetCoord(&sp0);
        shop->unk_83C = func_ov043_020b1944((s16) sp0.x, (s16) sp0.y);
        temp_r1 = shop->unk_83C;
        if ((temp_r1 != -1) && (shop->unk_738[temp_r1]->unk_00 != 0xFFFF)) {
            SndMgr_StartPlayingSE(0x11A);
            shop->unk_844 = (u16) shop->unk_83C;
            shop->unk_848 = shop->unk_850 + shop->unk_844;
            func_ov043_020acb08(arg0);
            if (func_ov043_020afcb0((ShopObject *) shop) == 1) {
                shop->unk_855 = 1;
                DebugOvlDisp_ReplaceTop((OverlayCB) func_ov043_020ad9dc, arg0, 0);
                return;
            }
            if (func_ov043_020b1a70((ShopObject *) shop, shop->unk_83C) != 1) {
                return;
            }
            SndMgr_StartPlayingSE(0x128);
            shop->unk_840 = (s16) sp0.x;
            shop->unk_842 = (s16) sp0.y;
            shop->unk_834 |= 1;
            arg0->unk_21768 = Shop_mov_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, shop->unk_844, (ShopObject *) shop);
            arg0->unk_2176C = Shop_movShadow_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, (ShopObject *) shop);
            return;
        }
        if (func_ov043_020b17d0((s16) sp0.x, (s16) sp0.y) == 1) {
            s32 ret;

            shop->unk_838 = 1;
            shop->unk_839 = 0xC;
            shop->unk_834 |= 0x1000;
            shop->unk_855 = 1;
            SndMgr_StartPlayingSE(0x11B);
            ret = func_ov043_020b1d24(gSaveData.unk_1AB2);
            if (ret == -1) {
                shop->unk_837 = 1;
            } else {
                gSaveData.unk_1AB2 = (u8) ret;
                shop->unk_837 = 2;
            }
            DebugOvlDisp_Pop();
            return;
        }
        goto block_20;
    }
    if (TouchInput_IsTouchActive() == 0) {
        func_ov043_020acf6c(arg0);
    }
block_20:
    temp_r1_2 = shop->unk_83A;
    if (temp_r1_2 != 0) {
        shop->unk_83A = temp_r1_2 - 1;
        if (shop->unk_83A == 0) {
            EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->taskId_WindowU);
            func_ov043_020ac6a0(arg0, shop->unk_774, shop->unk_78C, 1);
        }
    }
    temp_r1_3 = shop->unk_850;
    if (shop->unk_852 == temp_r1_3) {
        return;
    }
    func_ov043_020acbd0((ShopObject *) shop, temp_r1_3);
    shop->unk_852 = shop->unk_850;
}

void func_ov043_020ad738(ShopState *arg0) {
    void        func_ov043_020ad76c(ShopState *arg0);
    ShopObject *shop;

    shop = &arg0->shop;
    func_ov043_020b1fc4(shop, (u8) shop->unk_7B7[0], 0);
    DebugOvlDisp_ReplaceTop((OverlayCB) func_ov043_020ad76c, arg0, 0);
}

void func_ov043_020ad76c(ShopState *arg0) {
    void func_ov043_020ad7a0(ShopState *arg0);

    if (SndMgr_IsSEPlaying(0x376) != 0) {
        return;
    }
    DebugOvlDisp_ReplaceTop((OverlayCB) func_ov043_020ad7a0, arg0, 0);
}

void func_ov043_020ad7a0(ShopState *arg0) {
    typedef struct {
        /* 0x000 */ char pad_000[0x774];
        /* 0x774 */ u16  unk_774;
        /* 0x776 */ char pad_776[0x78C - 0x776];
        /* 0x78C */ u16  unk_78C;
        /* 0x78E */ char pad_78E[0x7B7 - 0x78E];
        /* 0x7B7 */ u8   unk_7B7;
        /* 0x7B8 */ char pad_7B8[0x834 - 0x7B8];
        /* 0x834 */ u16  unk_834;
        /* 0x836 */ char pad_836[0x84A - 0x836];
        /* 0x84A */ u16  unk_84A;
        /* 0x84C */ u16  unk_84C;
        /* 0x84E */ char pad_84E[0x855 - 0x84E];
        /* 0x855 */ u8   unk_855;
        /* 0x856 */ char pad_856[0x85E - 0x856];
        /* 0x85E */ u16  unk_85E;
    } Shop1y;
    void func_ov043_020ad90c(ShopState *arg0);
    void func_ov043_020ad3c8(ShopState *arg0);
    Shop1y *shop;

    shop = (Shop1y *) &arg0->shop;
    if (func_ov043_020afbd4((ShopObject *) shop, shop->unk_84A) == 1) {
        if (shop->unk_84A == 0) {
            EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->taskId_WindowU);
            func_ov043_020ac6a0(arg0, shop->unk_774, shop->unk_78C, 4);
            func_ov043_020b1fc4((ShopObject *) shop, shop->unk_7B7, 2);
            g_DisplaySettings.controls[0].layers |= 4;
        } else {
            EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->unk_21774);
        }
        shop->unk_85E = 2;
        arg0->unk_21774 = Shop_window0_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, (ShopObject *) shop);
        arg0->unk_2177C = Shop_item2_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, shop->unk_84C, 1, (ShopObject *) shop);
        DebugOvlDisp_ReplaceTop((OverlayCB) func_ov043_020ad90c, arg0, 0);
        return;
    }
    if (shop->unk_84A != 0) {
        shop->unk_85E = 0;
        shop->unk_834 |= 0x40;
        g_DisplaySettings.controls[0].layers &= ~4;
        EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->unk_21774);
    }
    shop->unk_855 = 0;
    DebugOvlDisp_ReplaceTop((OverlayCB) func_ov043_020ad3c8, arg0, 0);
}

void func_ov043_020ad90c(ShopState *arg0) {
    void func_ov043_020ad958(ShopState *arg0);

    SndMgr_StartPlayingSE(0x13E);
    arg0->shop.unk_7B4 += 1;
    arg0->unk_217B0 = 0xA;
    DebugOvlDisp_ReplaceTop((OverlayCB) func_ov043_020ad958, arg0, 0);
}

void func_ov043_020ad958(ShopState *arg0) {
    void        func_ov043_020ad7a0(ShopState *arg0);
    ShopObject *shop;
    u16         temp_r0;

    shop = &arg0->shop;
    temp_r0 = arg0->unk_217B0;
    if (temp_r0 != 0) {
        arg0->unk_217B0 = temp_r0 - 1;
        return;
    }
    if (TouchInput_WasTouchPressed() == 0) {
        return;
    }
    if (SndMgr_IsSEPlaying(0x376) != 0) {
        return;
    }
    shop->unk_84A += 1;
    EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->unk_2177C);
    DebugOvlDisp_ReplaceTop((OverlayCB) func_ov043_020ad7a0, arg0, 0);
}

void func_ov043_020ad9dc(ShopState *arg0) {
    typedef struct {
        /* 0x000 */ char pad_000[0x774];
        /* 0x774 */ u16  unk_774;
        /* 0x776 */ char pad_776[0x78C - 0x776];
        /* 0x78C */ u16  unk_78C;
        /* 0x78E */ char pad_78E[0x7B7 - 0x78E];
        /* 0x7B7 */ u8   unk_7B7;
        /* 0x7B8 */ char pad_7B8[0x84C - 0x7B8];
        /* 0x84C */ u16  unk_84C;
        /* 0x84E */ char pad_84E[0x85E - 0x84E];
        /* 0x85E */ u16  unk_85E;
    } Shop1z;
    Shop1z *shop;

    shop = (Shop1z *) &arg0->shop;
    EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->taskId_WindowU);
    func_ov043_020ac6a0(arg0, shop->unk_774, shop->unk_78C, 3);
    shop->unk_85E = 3;
    arg0->unk_21774 = Shop_window0_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, (ShopObject *) shop);
    arg0->unk_2177C = Shop_item2_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, shop->unk_84C, 1, (ShopObject *) shop);
    SndMgr_StartPlayingSE(0x13E);
    func_ov043_020b1fc4((ShopObject *) shop, shop->unk_7B7, 2);
    DebugOvlDisp_ReplaceTop((OverlayCB) func_ov043_020adaa4, arg0, 0);
}
// ---- Shop_p2.c ----
void func_ov043_020adaa4(ShopState *arg0) {
    void func_ov043_020adaf4(ShopState *);

    arg0->shop.unk_7B4 += 1;
    arg0->unk_217B0 = 0xA;
    g_DisplaySettings.controls[0].layers |= 4;
    DebugOvlDisp_ReplaceTop((OverlayCB)func_ov043_020adaf4, arg0, 0);
}

void func_ov043_020adaf4(ShopState *arg0) {
    typedef struct {
        /* 0x000 */ char pad_000[0x7B4];
        /* 0x7B4 */ u8   unk_7B4;
        /* 0x7B5 */ u8   unk_7B5;
        /* 0x7B6 */ u8   unk_7B6;
        /* 0x7B7 */ u8   unk_7B7;
        /* 0x7B8 */ u8   unk_7B8;
        /* 0x7B9 */ char pad_7B9[0x800 - 0x7B9];
        /* 0x800 */ u16  unk_800;
        /* 0x802 */ char pad_802[0x834 - 0x802];
        /* 0x834 */ u16  unk_834;
        /* 0x836 */ char pad_836[0x855 - 0x836];
        /* 0x855 */ u8   unk_855;
        /* 0x856 */ char pad_856[0x85E - 0x856];
        /* 0x85E */ u16  unk_85E;
    } ShopP2;
    void  func_ov043_020ad3c8(ShopState *);
    ShopP2 *shop = (ShopP2 *) &arg0->shop;
    u16    temp_r2;

    temp_r2 = arg0->unk_217B0;
    if (temp_r2 != 0) {
        arg0->unk_217B0 = temp_r2 - 1;
        return;
    }
    if (TouchInput_WasTouchPressed() == 0) {
        return;
    }
    if (SndMgr_IsSEPlaying(0x376) != 0) {
        return;
    }
    shop->unk_85E = 0;
    shop->unk_834 |= 0x40;
    g_DisplaySettings.controls[0].layers &= ~4;
    EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->unk_21774);
    EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->unk_2177C);
    shop->unk_855 = 0;
    DebugOvlDisp_ReplaceTop((OverlayCB)func_ov043_020ad3c8, arg0, 0);
}

void func_ov043_020adbb0(ShopState *arg0) {
    typedef struct {
        /* 0x000 */ char                pad_000[0x738];
        /* 0x738 */ ShopItemTableEntry *unk_738[12];
        /* 0x768 */ char                pad_768[0x774 - 0x768];
        /* 0x774 */ u16                 unk_774;
        /* 0x776 */ u16                 unk_776;
        /* 0x778 */ u16                 unk_778;
        /* 0x77A */ char                pad_77A[0x788 - 0x77A];
        /* 0x788 */ u16                 unk_788;
        /* 0x78A */ u16                 unk_78A;
        /* 0x78C */ u16                 unk_78C;
        /* 0x78E */ char                pad_78E[0x7A2 - 0x78E];
        /* 0x7A2 */ u16                 unk_7A2;
        /* 0x7A4 */ char                pad_7A4[0x7B4 - 0x7A4];
        /* 0x7B4 */ u8                  unk_7B4;
        /* 0x7B5 */ u8                  unk_7B5;
        /* 0x7B6 */ u8                  unk_7B6;
        /* 0x7B7 */ u8                  unk_7B7;
        /* 0x7B8 */ u8                  unk_7B8;
        /* 0x7B9 */ char                pad_7B9[0x800 - 0x7B9];
        /* 0x800 */ u16                 unk_800;
        /* 0x802 */ char                pad_802[0x834 - 0x802];
        /* 0x834 */ u16                 unk_834;
        /* 0x836 */ u8                  unk_836;
        /* 0x837 */ u8                  unk_837;
        /* 0x838 */ u8                  unk_838;
        /* 0x839 */ u8                  unk_839;
        /* 0x83A */ u16                 unk_83A;
        /* 0x83C */ s16                 unk_83C;
        /* 0x83E */ u16                 unk_83E;
        /* 0x840 */ s16                 unk_840;
        /* 0x842 */ s16                 unk_842;
        /* 0x844 */ u16                 unk_844;
        /* 0x846 */ u16                 unk_846;
        /* 0x848 */ u16                 unk_848;
        /* 0x84A */ u16                 unk_84A;
        /* 0x84C */ u16                 unk_84C;
        /* 0x84E */ u16                 unk_84E;
        /* 0x850 */ u16                 unk_850;
        /* 0x852 */ u16                 unk_852;
        /* 0x854 */ u8                  unk_854;
        /* 0x855 */ u8                  unk_855;
        /* 0x856 */ char                pad_856[0x858 - 0x856];
        /* 0x858 */ u32                 unk_858;
        /* 0x85C */ char                pad_85C[0x85E - 0x85C];
        /* 0x85E */ u16                 unk_85E;
        /* 0x860 */ u16                 unk_860;
        /* 0x862 */ u16                 unk_862;
        /* 0x864 */ u8                  unk_864[3];
        /* 0x867 */ char                pad_867[0x870 - 0x867];
        /* 0x870 */ u16                 unk_870;
        /* 0x872 */ u16                 unk_872;
    } ShopP2;
    void   func_ov043_020add48(ShopState *);
    ShopP2 *shop = (ShopP2 *) &arg0->shop;
    u16    temp_r2;

    arg0->unk_217B0 = 0xA;
    if ((u32) ((ShopP2 *) ((u8 *) shop + (shop->unk_778 - 2) * 2))->unk_7A2 >= 0x3D4U) {
        shop->unk_7B8 += 1;
        shop->unk_7B5 = (u8) func_ov043_020afb14((ShopObject *) shop);
        shop->unk_84E = 0xFFFF;
    } else {
        func_ov043_020afd6c((ShopObject *) shop);
        if (shop->unk_860 == 0xD) {
            EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->taskId_Sbar);
            arg0->taskId_Sbar = Shop_sbar_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, shop);
        }
        shop->unk_862 = func_ov043_020b02ac(shop->unk_860);
        shop->unk_870 = ((shop->unk_862 / 6) + ((u16) shop->unk_862 >> 0x1F)) - 2;
        shop->unk_872 = shop->unk_870;
        func_ov043_020acbd0((s32) shop, shop->unk_850);
    }
    shop->unk_85E = 1;
    arg0->unk_21774 = Shop_window0_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, shop);
    temp_r2 = shop->unk_84E;
    if (temp_r2 != 0xFFFF) {
        arg0->unk_2177C = Shop_item2_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, temp_r2, 0, shop);
    }
    SndMgr_StartPlayingSE(0x13F);
    g_DisplaySettings.controls[0].layers |= 4;
    DebugOvlDisp_ReplaceTop((OverlayCB)func_ov043_020add48, arg0, 0);
}

void func_ov043_020add48(ShopState *arg0) {
    typedef struct {
        /* 0x000 */ char pad_000[0x834];
        /* 0x834 */ u16  unk_834;
        /* 0x836 */ char pad_836[0x84E - 0x836];
        /* 0x84E */ u16  unk_84E;
        /* 0x850 */ char pad_850[0x855 - 0x850];
        /* 0x855 */ u8   unk_855;
        /* 0x856 */ char pad_856[0x85E - 0x856];
        /* 0x85E */ u16  unk_85E;
    } ShopP2;
    void  func_ov043_020ad3c8(ShopState *);
    ShopP2 *shop = (ShopP2 *) &arg0->shop;
    u16   temp_r2;

    temp_r2 = arg0->unk_217B0;
    if (temp_r2 != 0) {
        arg0->unk_217B0 = temp_r2 - 1;
        return;
    }
    if (TouchInput_WasTouchPressed() == 0) {
        return;
    }
    shop->unk_85E = 0;
    shop->unk_834 |= 0x40;
    g_DisplaySettings.controls[0].layers &= ~4;
    EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->unk_21774);
    if (shop->unk_84E != 0xFFFF) {
        EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->unk_2177C);
    }
    shop->unk_855 = 0;
    DebugOvlDisp_ReplaceTop((OverlayCB)func_ov043_020ad3c8, arg0, 0);
}

void func_ov043_020ade08(ShopState *arg0) {
    typedef struct {
        /* 0x000 */ char pad_000[0x774];
        /* 0x774 */ u16  unk_774;
        /* 0x776 */ char pad_776[0x78C - 0x776];
        /* 0x78C */ u16  unk_78C;
        /* 0x78E */ char pad_78E[0x7B7 - 0x78E];
        /* 0x7B7 */ u8   unk_7B7;
        /* 0x7B8 */ char pad_7B8[0x834 - 0x7B8];
        /* 0x834 */ u16  unk_834;
        /* 0x836 */ char pad_836[0x83A - 0x836];
        /* 0x83A */ u16  unk_83A;
    } ShopP2;
    void  func_ov043_020ade9c(ShopState *);
    ShopP2 *shop = (ShopP2 *) &arg0->shop;

    EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->taskId_WindowU);
    if (shop->unk_834 & 8) {
        func_ov043_020ac6a0(arg0, shop->unk_774, shop->unk_78C, 6);
    } else {
        func_ov043_020ac6a0(arg0, shop->unk_774, shop->unk_78C, 5);
    }
    func_ov043_020b1fc4(shop, shop->unk_7B7, 1);
    shop->unk_83A = 0xB4;
    DebugOvlDisp_ReplaceTop((OverlayCB)func_ov043_020ade9c, arg0, 0);
}

void func_ov043_020ade9c(ShopState *arg0) {
    typedef struct {
        /* 0x000 */ char pad_000[0x83A];
        /* 0x83A */ u16  unk_83A;
    } ShopP2;
    ShopP2 *shop = (ShopP2 *) &arg0->shop;
    u16   temp_r1;

    temp_r1 = shop->unk_83A;
    if (temp_r1 != 0) {
        shop->unk_83A = temp_r1 - 1;
    }
    if (TouchInput_WasTouchPressed() != 0) {
        shop->unk_83A = 0;
    }
    if (shop->unk_83A != 0) {
        return;
    }
    if (SndMgr_IsSEPlaying(0x376) != 0) {
        return;
    }
    DebugOvlDisp_Pop();
}

void func_ov043_020adef8(ShopState *arg0) {
    typedef struct {
        /* 0x000 */ char pad_000[0x834];
        /* 0x834 */ u16  unk_834;
        /* 0x836 */ char pad_836[0x844 - 0x836];
        /* 0x844 */ u16  unk_844;
    } ShopP2;
    void   func_ov043_020ae138(ShopState *);
    void   func_ov043_020adfcc(ShopState *);
    ShopP2 *shop = (ShopP2 *) &arg0->shop;

    if (shop->unk_834 & 4) {
        if (func_ov043_020b1780((ShopObject *) shop, shop->unk_844) == 1) {
            func_ov043_020b16c4((ShopObject *) shop, shop->unk_844);
            func_ov043_020acd90(arg0, (ShopObject *) shop);
            return;
        }
        DebugOvlDisp_ReplaceTop((OverlayCB)func_ov043_020ae138, arg0, 0);
        return;
    }
    if (func_ov043_020b1780((ShopObject *) shop, shop->unk_844) == 1) {
        arg0->unk_21778 = Shop_window1_CreateTask((TaskPool *) (&(arg0)->base.taskPool), arg0->base.dataType, (ShopObject *) shop);
        g_DisplaySettings.controls[0].layers |= 4;
        DebugOvlDisp_ReplaceTop((OverlayCB)func_ov043_020adfcc, arg0, 0);
        return;
    }
    DebugOvlDisp_ReplaceTop((OverlayCB)func_ov043_020ae138, arg0, 0);
}

void func_ov043_020adfcc(ShopState *arg0) {
    typedef struct {
        /* 0x000 */ char pad_000[0x839];
        /* 0x839 */ u8   unk_839;
        /* 0x83A */ u16  unk_83A;
        /* 0x83C */ char pad_83C[0x844 - 0x83C];
        /* 0x844 */ u16  unk_844;
        /* 0x846 */ char pad_846[0x864 - 0x846];
        /* 0x864 */ u8   unk_864[3];
    } ShopP2;
    void  func_ov043_020ae0b0(ShopState *);
    ShopP2 *shop = (ShopP2 *) &arg0->shop;
    TouchCoord coord;
    s32   idx;

    if (TouchInput_WasTouchPressed() == 0) {
        return;
    }
    TouchInput_GetCoord(&coord);
    idx = func_ov043_020b1c7c((s16) coord.x, (s16) coord.y);
    if (idx == -1) {
        return;
    }
    shop->unk_864[idx] = 1;
    shop->unk_839 = 0xC;
    shop->unk_864[2] = (u8) idx;
    if (idx == 0) {
        func_ov043_020b16c4((ShopObject *) shop, shop->unk_844);
        SndMgr_StartPlayingSE(0x11C);
        shop->unk_83A = 0x14;
        DebugOvlDisp_ReplaceTop((OverlayCB)func_ov043_020ae0b0, arg0, 0);
        return;
    }
    SndMgr_StartPlayingSE(0x11B);
    shop->unk_83A = 0x14;
    DebugOvlDisp_ReplaceTop((OverlayCB)func_ov043_020ae0b0, arg0, 0);
}

void func_ov043_020ae0b0(ShopState *arg0) {
    typedef struct {
        /* 0x000 */ char pad_000[0x83A];
        /* 0x83A */ u16  unk_83A;
        /* 0x83C */ char pad_83C[0x855 - 0x83C];
        /* 0x855 */ u8   unk_855;
        /* 0x856 */ char pad_856[0x864 - 0x856];
        /* 0x864 */ u8   unk_864[3];
    } ShopP2;
    void  func_ov043_020ad3c8(ShopState *);
    ShopP2 *shop = (ShopP2 *) &arg0->shop;
    u16   temp_r1;

    temp_r1 = shop->unk_83A;
    if (temp_r1 != 0) {
        shop->unk_83A = temp_r1 - 1;
        return;
    }
    EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->unk_21778);
    g_DisplaySettings.controls[0].layers &= ~4;
    if (shop->unk_864[2] == 0) {
        func_ov043_020acd90(arg0, (ShopObject *) shop);
        return;
    }
    shop->unk_855 = 0;
    DebugOvlDisp_ReplaceTop((OverlayCB)func_ov043_020ad3c8, arg0, 0);
}

void func_ov043_020ae138(ShopState *arg0) {
    typedef struct {
        /* 0x000 */ char pad_000[0x85E];
        /* 0x85E */ u16  unk_85E;
    } ShopP2;
    void   func_ov043_020ae1b8(ShopState *);
    ShopP2 *shop = (ShopP2 *) &arg0->shop;

    arg0->unk_217B0 = 0xA;
    shop->unk_85E = 4;
    arg0->unk_21774 = Shop_window0_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, (ShopObject *) shop);
    SndMgr_StartPlayingSE(0x141);
    g_DisplaySettings.controls[0].layers |= 4;
    DebugOvlDisp_ReplaceTop((OverlayCB)func_ov043_020ae1b8, arg0, 0);
}

void func_ov043_020ae1b8(ShopState *arg0) {
    typedef struct {
        /* 0x000 */ char pad_000[0x834];
        /* 0x834 */ u16  unk_834;
        /* 0x836 */ char pad_836[0x855 - 0x836];
        /* 0x855 */ u8   unk_855;
        /* 0x856 */ char pad_856[0x85E - 0x856];
        /* 0x85E */ u16  unk_85E;
    } ShopP2;
    void  func_ov043_020ad3c8(ShopState *);
    ShopP2 *shop = (ShopP2 *) &arg0->shop;
    u16   temp_r2;

    temp_r2 = arg0->unk_217B0;
    if (temp_r2 != 0) {
        arg0->unk_217B0 = temp_r2 - 1;
        return;
    }
    if (TouchInput_WasTouchPressed() == 0) {
        return;
    }
    shop->unk_85E = 0;
    shop->unk_834 |= 0x40;
    g_DisplaySettings.controls[0].layers &= ~4;
    EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->unk_21774);
    shop->unk_855 = 0;
    DebugOvlDisp_ReplaceTop((OverlayCB)func_ov043_020ad3c8, arg0, 0);
}

void func_ov043_020ae24c(ShopState *arg0) {
    typedef struct {
        /* 0x000 */ char pad_000[0x7B7];
        /* 0x7B7 */ u8   unk_7B7;
        /* 0x7B8 */ char pad_7B8[0x855 - 0x7B8];
        /* 0x855 */ u8   unk_855;
    } ShopP2;
    void  func_ov043_020ae284(ShopState *);
    ShopP2 *shop = (ShopP2 *) &arg0->shop;

    shop->unk_855 = 0;
    func_ov043_020b1fc4(shop, shop->unk_7B7, 0);
    DebugOvlDisp_ReplaceTop((OverlayCB)func_ov043_020ae284, arg0, 0);
}

void func_ov043_020ae284(ShopState *arg0) {
    typedef struct {
        /* 0x000 */ char                pad_000[0x738];
        /* 0x738 */ ShopItemTableEntry *unk_738[12];
        /* 0x768 */ char                pad_768[0x774 - 0x768];
        /* 0x774 */ u16                 unk_774;
        /* 0x776 */ u16                 unk_776;
        /* 0x778 */ u16                 unk_778;
        /* 0x77A */ char                pad_77A[0x788 - 0x77A];
        /* 0x788 */ u16                 unk_788;
        /* 0x78A */ u16                 unk_78A;
        /* 0x78C */ u16                 unk_78C;
        /* 0x78E */ char                pad_78E[0x7B4 - 0x78E];
        /* 0x7B4 */ u8                  unk_7B4;
        /* 0x7B5 */ u8                  unk_7B5;
        /* 0x7B6 */ u8                  unk_7B6;
        /* 0x7B7 */ u8                  unk_7B7;
        /* 0x7B8 */ char                pad_7B8[0x834 - 0x7B8];
        /* 0x834 */ u16                 unk_834;
        /* 0x836 */ u8                  unk_836;
        /* 0x837 */ u8                  unk_837;
        /* 0x838 */ u8                  unk_838;
        /* 0x839 */ u8                  unk_839;
        /* 0x83A */ u16                 unk_83A;
        /* 0x83C */ s16                 unk_83C;
        /* 0x83E */ u16                 unk_83E;
        /* 0x840 */ s16                 unk_840;
        /* 0x842 */ s16                 unk_842;
        /* 0x844 */ u16                 unk_844;
        /* 0x846 */ u16                 unk_846;
        /* 0x848 */ u16                 unk_848;
        /* 0x84A */ u16                 unk_84A;
        /* 0x84C */ u16                 unk_84C;
        /* 0x84E */ u16                 unk_84E;
        /* 0x850 */ u16                 unk_850;
        /* 0x852 */ u16                 unk_852;
        /* 0x854 */ u8                  unk_854;
        /* 0x855 */ u8                  unk_855;
        /* 0x856 */ char                pad_856[0x858 - 0x856];
        /* 0x858 */ u32                 unk_858;
        /* 0x85C */ char                pad_85C[0x85E - 0x85C];
        /* 0x85E */ u16                 unk_85E;
        /* 0x860 */ u16                 unk_860;
        /* 0x862 */ u16                 unk_862;
        /* 0x864 */ u8                  unk_864[3];
        /* 0x867 */ char                pad_867[0x870 - 0x867];
        /* 0x870 */ u16                 unk_870;
        /* 0x872 */ u16                 unk_872;
    } ShopP2;
    void      func_ov043_020ae798(ShopState *);
    ShopP2   *shop = (ShopP2 *) &arg0->shop;
    TouchCoord coord;
    s16        temp_r3;
    u16        temp_r1;
    u16        temp_r1_2;

    if (shop->unk_834 & 1) {
        TouchInput_GetCoord(&coord);
        shop->unk_840 = (s16) coord.x;
        shop->unk_842 = (s16) coord.y;
        if (TouchInput_WasTouchReleased() != 0) {
            shop->unk_834 &= ~1;
            EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->unk_21768);
            EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->unk_2176C);
            if (func_ov043_020b18e4((s16) coord.x, (s16) coord.y) == 1) {
                SndMgr_StartPlayingSE(0x129);
                if ((u32) shop->unk_858 >= (u32) shop->unk_738[shop->unk_844]->unk_08) {
                    shop->unk_855 = 1;
                    DebugOvlDisp_ReplaceTop((OverlayCB)func_ov043_020ae798, arg0, 0);
                }
            }
        }
        goto block_15;
    }
    if (TouchInput_WasTouchPressed() != 0) {
        TouchInput_GetCoord(&coord);
        shop->unk_83C = (s16) func_ov043_020b1944((s16) coord.x, (s16) coord.y);
        temp_r3 = shop->unk_83C;
        if ((temp_r3 != -1) && (shop->unk_738[temp_r3]->unk_00 != 0xFFFF)) {
            shop->unk_844 = (u16) temp_r3;
            shop->unk_848 = shop->unk_850 + shop->unk_844;
            func_ov043_020acb08(arg0);
            if (func_ov043_020b1a70((ShopObject *) shop, shop->unk_83C) != 1) {
                return;
            }
            SndMgr_StartPlayingSE(0x128);
            shop->unk_840 = (s16) coord.x;
            shop->unk_842 = (s16) coord.y;
            shop->unk_834 |= 1;
            arg0->unk_21768 = Shop_mov_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, shop->unk_844, shop);
            arg0->unk_2176C = Shop_movShadow_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, shop);
            return;
        }
        if (func_ov043_020b17d0((s16) coord.x, (s16) coord.y) == 1) {
            shop->unk_838 = 1;
            shop->unk_839 = 0xC;
            shop->unk_834 |= 0x1000;
            shop->unk_855 = 1;
            SndMgr_StartPlayingSE(0x11B);
            shop->unk_837 = 3;
            DebugOvlDisp_Pop();
            return;
        }
        goto block_15;
    }
    if (TouchInput_IsTouchActive() == 0) {
        func_ov043_020acf6c(arg0);
    }
block_15:
    temp_r1 = shop->unk_83A;
    if (temp_r1 != 0) {
        shop->unk_83A = temp_r1 - 1;
        if (shop->unk_83A == 0) {
            EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->taskId_WindowU);
            func_ov043_020ac6a0(arg0, shop->unk_774, shop->unk_78C, 1);
        }
    }
    temp_r1_2 = shop->unk_850;
    if (shop->unk_852 == temp_r1_2) {
        return;
    }
    func_ov043_020acbd0((s32) shop, temp_r1_2);
    shop->unk_852 = shop->unk_850;
}

void func_ov043_020ae580(ShopState *arg0) {
    typedef struct {
        /* 0x000 */ char pad_000[0x774];
        /* 0x774 */ u16  unk_774;
        /* 0x776 */ char pad_776[0x78C - 0x776];
        /* 0x78C */ u16  unk_78C;
        /* 0x78E */ char pad_78E[0x7B7 - 0x78E];
        /* 0x7B7 */ u8   unk_7B7;
        /* 0x7B8 */ char pad_7B8[0x834 - 0x7B8];
        /* 0x834 */ u16  unk_834;
        /* 0x836 */ char pad_836[0x83A - 0x836];
        /* 0x83A */ u16  unk_83A;
    } ShopP2;
    void  func_ov043_020ae614(ShopState *);
    ShopP2 *shop = (ShopP2 *) &arg0->shop;

    EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->taskId_WindowU);
    if (shop->unk_834 & 8) {
        func_ov043_020ac6a0(arg0, shop->unk_774, shop->unk_78C, 6);
    } else {
        func_ov043_020ac6a0(arg0, shop->unk_774, shop->unk_78C, 5);
    }
    func_ov043_020b1fc4(shop, shop->unk_7B7, 1);
    shop->unk_83A = 0xB4;
    DebugOvlDisp_ReplaceTop((OverlayCB)func_ov043_020ae614, arg0, 0);
}

void func_ov043_020ae614(ShopState *arg0) {
    typedef struct {
        /* 0x000 */ char pad_000[0x83A];
        /* 0x83A */ u16  unk_83A;
    } ShopP2;
    ShopP2 *shop = (ShopP2 *) &arg0->shop;
    u16   temp_r1;

    temp_r1 = shop->unk_83A;
    if (temp_r1 != 0) {
        shop->unk_83A = temp_r1 - 1;
    }
    if (TouchInput_WasTouchPressed() != 0) {
        shop->unk_83A = 0;
    }
    if (shop->unk_83A != 0) {
        return;
    }
    if (SndMgr_IsSEPlaying(0x376) != 0) {
        return;
    }
    DebugOvlDisp_Pop();
}
// ---- Shop_p3.c ----
// Mirror of the ShopObject fields this part needs that Shop.h currently exposes
// as raw byte arrays (or omits entirely).  Defined locally inside each function
// so that the file also compiles function-by-function under tools/probe.py.
// Keep in sync with include/Interface/Menu/Shop.h.
//
//   0x76C u16   (CriSndMgr_PlayFile arg)
//   0x78C u16
//   0x83A u16
//   0x868 u32

void func_ov043_020ae284(ShopState *arg0);
void func_ov043_020ae580(ShopState *arg0);
void func_ov043_020ade08(ShopState *arg0);
void func_ov043_020ad354(ShopState *arg0);
void func_ov043_020ae670(ShopState *arg0, ShopObject *shop);
void func_ov043_020ae798(ShopState *arg0);
void func_ov043_020ae86c(ShopState *arg0);
void func_ov043_020ae950(ShopState *arg0);
void func_ov043_020ae9d8(ShopState *arg0);
void func_ov043_020aea58(ShopState *arg0);
void func_ov043_020aeaec(ShopState *arg0);
void func_ov043_020aeb14(ShopState *arg0);
void func_ov043_020aec98(ShopState *arg0);
void func_ov043_020aee2c(ShopState *arg0);
void func_ov043_020aeee0(ShopState *arg0);
void func_ov043_020aef20(ShopState *arg0);
void func_ov043_020af2d8(ShopState *arg0);
void func_ov043_020af3c0(ShopState *arg0);
void func_ov043_020af3dc();

void func_ov043_020ae670(ShopState *arg0, ShopObject *shop) {
    typedef struct {
        /* 0x000 */ char pad_000[0x76C];
        /* 0x76C */ u16  unk_76C;
        /* 0x76E */ char pad_76E[0x78C - 0x76E];
        /* 0x78C */ u16  unk_78C;
        /* 0x78E */ char pad_78E[0x83A - 0x78E];
        /* 0x83A */ u16  unk_83A;
        /* 0x83C */ char pad_83C[0x868 - 0x83C];
        /* 0x868 */ u32  unk_868;
    } ShopP3View;
    ShopP3View *sv;

    sv = (ShopP3View *) shop;
    shop->unk_858 -= shop->unk_738[shop->unk_844]->unk_08;
    sv->unk_868 = sv->unk_868 + shop->unk_738[shop->unk_844]->unk_08;
    if (sv->unk_868 > 0x98967FU) {
        sv->unk_868 = 0x98967FU;
    }
    if (shop->unk_738[shop->unk_844]->unk_08 >= 0x2710U) {
        Stats_AddExperience(1);
    }
    EasyTask_DeleteTask(&arg0->base.taskPool, arg0->taskId_WindowU);
    EasyTask_DeleteTask(&arg0->base.taskPool, arg0->taskId_ItemTotalNum);
    func_ov043_020ac6a0(arg0, shop->unk_774, sv->unk_78C, 2);
    arg0->taskId_ItemTotalNum = Shop_itemTotalNum_CreateTask(&arg0->base.taskPool, arg0->base.dataType, shop);
    sv->unk_83A = 0x12C;
    shop->unk_834 |= 8;
    if (shop->unk_834 & 4) {
        SndMgr_StartPlayingSE(0x11C);
    }
    shop->unk_855[0] = 0;
    DebugOvlDisp_ReplaceTop((OverlayCB) func_ov043_020ae284, arg0, 0);
}

void func_ov043_020ae798(ShopState *arg0) {
    void func_ov043_020ae9d8(ShopState *arg0);
    void func_ov043_020ae86c(ShopState *arg0);
    ShopObject *shop;

    shop = &arg0->shop;
    if (shop->unk_834 & 4) {
        if (func_ov043_020b1780((s32) shop, (s32) shop->unk_844) == 1) {
            func_ov043_020b16c4(shop, shop->unk_844);
            func_ov043_020ae670(arg0, shop);
            return;
        }
        DebugOvlDisp_ReplaceTop((OverlayCB) func_ov043_020ae9d8, arg0, 0);
        return;
    }
    if (func_ov043_020b1780((s32) shop, (s32) shop->unk_844) == 1) {
        arg0->unk_21778 = Shop_window1_CreateTask(&arg0->base.taskPool, arg0->base.dataType, shop);
        g_DisplaySettings.controls[0].layers |= 4;
        DebugOvlDisp_ReplaceTop((OverlayCB) func_ov043_020ae86c, arg0, 0);
        return;
    }
    DebugOvlDisp_ReplaceTop((OverlayCB) func_ov043_020ae9d8, arg0, 0);
}

void func_ov043_020ae86c(ShopState *arg0) {
    void func_ov043_020ae950(ShopState *arg0);
    typedef struct {
        /* 0x000 */ char pad_000[0x76C];
        /* 0x76C */ u16  unk_76C;
        /* 0x76E */ char pad_76E[0x78C - 0x76E];
        /* 0x78C */ u16  unk_78C;
        /* 0x78E */ char pad_78E[0x83A - 0x78E];
        /* 0x83A */ u16  unk_83A;
        /* 0x83C */ char pad_83C[0x868 - 0x83C];
        /* 0x868 */ u32  unk_868;
    } ShopP3View;
    ShopObject *shop;
    TouchCoord coord;
    s32 r;

    shop = &arg0->shop;
    if (TouchInput_WasTouchPressed() == 0) {
        return;
    }
    TouchInput_GetCoord(&coord);
    r = func_ov043_020b1c7c((s16) coord.x, (s16) coord.y);
    if (r == -1) {
        return;
    }
    shop->unk_864[r] = 1;
    shop->unk_839 = 0xC;
    shop->unk_864[2] = r;
    if (r == 0) {
        func_ov043_020b16c4(shop, shop->unk_844);
        SndMgr_StartPlayingSE(0x11C);
        ((ShopP3View *) shop)->unk_83A = 0x14;
        DebugOvlDisp_ReplaceTop((OverlayCB) func_ov043_020ae950, arg0, 0);
        return;
    }
    SndMgr_StartPlayingSE(0x11B);
    ((ShopP3View *) shop)->unk_83A = 0x14;
    DebugOvlDisp_ReplaceTop((OverlayCB) func_ov043_020ae950, arg0, 0);
}

void func_ov043_020ae950(ShopState *arg0) {
    typedef struct {
        /* 0x000 */ char pad_000[0x76C];
        /* 0x76C */ u16  unk_76C;
        /* 0x76E */ char pad_76E[0x78C - 0x76E];
        /* 0x78C */ u16  unk_78C;
        /* 0x78E */ char pad_78E[0x83A - 0x78E];
        /* 0x83A */ u16  unk_83A;
        /* 0x83C */ char pad_83C[0x868 - 0x83C];
        /* 0x868 */ u32  unk_868;
    } ShopP3View;
    ShopObject *shop;
    u16 temp_r1;

    shop = &arg0->shop;
    temp_r1 = ((ShopP3View *) shop)->unk_83A;
    if (temp_r1 != 0) {
        ((ShopP3View *) shop)->unk_83A = temp_r1 - 1;
        return;
    }
    EasyTask_DeleteTask(&arg0->base.taskPool, arg0->unk_21778);
    g_DisplaySettings.controls[0].layers &= ~4;
    if (shop->unk_864[2] == 0) {
        func_ov043_020ae670(arg0, shop);
        return;
    }
    shop->unk_855[0] = 0;
    DebugOvlDisp_ReplaceTop((OverlayCB) func_ov043_020ae284, arg0, 0);
}

void func_ov043_020ae9d8(ShopState *arg0) {
    void func_ov043_020aea58(ShopState *arg0);
    ShopObject *shop;

    shop = &arg0->shop;
    arg0->unk_217B0 = 0xA;
    shop->unk_85E = 4;
    arg0->unk_21774 = Shop_window0_CreateTask(&arg0->base.taskPool, arg0->base.dataType, shop);
    SndMgr_StartPlayingSE(0x141);
    g_DisplaySettings.controls[0].layers |= 4;
    DebugOvlDisp_ReplaceTop((OverlayCB) func_ov043_020aea58, arg0, 0);
}

void func_ov043_020aea58(ShopState *arg0) {
    ShopObject *shop;
    u16 temp_r2;

    shop = &arg0->shop;
    temp_r2 = arg0->unk_217B0;
    if (temp_r2 != 0) {
        arg0->unk_217B0 = temp_r2 - 1;
        return;
    }
    if (TouchInput_WasTouchPressed() == 0) {
        return;
    }
    shop->unk_85E = 0;
    shop->unk_834 |= 0x40;
    g_DisplaySettings.controls[0].layers &= ~4;
    EasyTask_DeleteTask(&arg0->base.taskPool, arg0->unk_21774);
    shop->unk_855[0] = 0;
    DebugOvlDisp_ReplaceTop((OverlayCB) func_ov043_020ae284, arg0, 0);
}

void func_ov043_020aeaec(ShopState *arg0) {
    EasyFade_FadeBothDisplays(0, 0x10, 0x1000);
    if (EasyFade_IsFading() != 0) {
        return;
    }
    DebugOvlDisp_Pop();
}

// Nonmatching: register allocation + overlay-id literal-pool entry
void func_ov043_020aeb14(ShopState *arg0) {
    void func_ov043_020aeaec(ShopState *arg0);
    void func_ov043_020ad354(ShopState *arg0);
    typedef struct {
        /* 0x000 */ char pad_000[0x76C];
        /* 0x76C */ u16  unk_76C;
    } ShopP3ViewB14;
    ShopState *var_r5;
    ShopObject *shop;
    s32 temp_r0;
    s8 *temp_r4;

    var_r5 = arg0;
    if (var_r5 == NULL) {
        temp_r4 = data_ov043_020cc5bc;
        var_r5 = Mem_AllocHeapTail(&gDebugHeap, 0x22108);
        Mem_SetSequence(&gDebugHeap, var_r5, temp_r4);
        data_ov043_020cd288 = var_r5;
        MainOvlDisp_SetCbArg(&var_r5->base.resMgr);
    }
    shop = &var_r5->shop;
    var_r5->base.spareDataType = DatMgr_AllocateSlot();
    temp_r0 = DatMgr_AllocateSlot();
    var_r5->base.dataType = temp_r0;
    func_ov043_020af3c0((ShopState *) temp_r0);
    var_r5->base.prevResMgr = ResourceMgr_ReinitManagers(&var_r5->base.resMgr);
    TouchInput_Init();
    Mem_InitializeHeap(&var_r5->base.heap, &var_r5->base.heapBuffer, 0x10000U);
    EasyTask_InitializePool(&var_r5->base.taskPool, &var_r5->base.heap, 0x100U, NULL, NULL);
    FS_LoadOverlay(0, (u32) &OVERLAY_31_ID);
    data_02066aec = 0;
    data_02066eec = 0;
    func_ov043_020ac5d8(var_r5);
    func_ov043_020b2934(shop);
    func_ov043_020ac6f4(var_r5);
    DebugOvlDisp_Init();
    DebugOvlDisp_Push((OverlayCB) func_ov043_020aeaec, var_r5, 0);
    if (gSaveData.unk_1AB4 & 4) {
        DebugOvlDisp_Push((OverlayCB) func_ov043_020ae580, var_r5, 0);
    } else {
        DebugOvlDisp_Push((OverlayCB) func_ov043_020ade08, var_r5, 0);
    }
    DebugOvlDisp_Push((OverlayCB) func_ov043_020ad354, var_r5, 0);
    MainOvlDisp_NextProcessStage();
    CriSndMgr_PlayFile(((ShopP3ViewB14 *) shop)->unk_76C);
}

// Nonmatching: overlay-id literal-pool entry (byte-identical when linked)
void func_ov043_020aec98(ShopState *arg0) {
    ShopObject *shop;
    OverlayTag tag1;
    OverlayTag tag2;
    OverlayTag tag3;
    OverlayTag tag4;
    u8 temp_r0;
    u8 temp_r1;

    shop = &arg0->shop;
    TouchInput_Update();
    OamMgr_Reset3DState();
    OamMgr_Reset(&g_OamMgr[0], 0, 0);
    OamMgr_Reset(&g_OamMgr[1], 0, 0);
    OamMgr_SetAffineCount(&g_OamMgr[2], 0);
    OamMgr_ResetCommandQueues(&g_OamMgr[0]);
    OamMgr_ResetCommandQueues(&g_OamMgr[1]);
    ((void (*)(ShopObject *)) func_ov043_020b2af0)(shop);
    DebugOvlDisp_Run();
    EasyTask_UpdatePool(&arg0->base.taskPool);
    if (DebugOvlDisp_IsStackAtBase() == 1) {
        arg0->unk_217AC = 1;
    }
    OamMgr_Swap3DBuffers();
    OamMgr_FlushCommands(&g_OamMgr[0]);
    OamMgr_FlushCommands(&g_OamMgr[1]);
    PaletteMgr_Flush(g_PaletteManagers[0], NULL);
    PaletteMgr_Flush(g_PaletteManagers[1], NULL);
    PaletteMgr_Flush(g_PaletteManagers[2], NULL);
    if (arg0->unk_217AC != 0) {
        temp_r0 = shop->unk_837;
        switch (temp_r0) {                          /* irregular */
        case 1:
            MainOvlDisp_ReplaceTop(&tag1, (s32) &OVERLAY_30_ID, (void *) 0x020AE92C, 0, 0);
            break;
        case 2:
            MainOvlDisp_ReplaceTop(&tag2, (s32) &OVERLAY_43_ID, &func_ov043_020bd414, 0, 0);
            break;
        case 3:
            MainOvlDisp_ReplaceTop(&tag3, (s32) &OVERLAY_45_ID, (void *) 0x02091034, 0, 0);
            break;
        default:
            MainOvlDisp_Pop(&tag4);
            break;
        }
    }
    temp_r1 = shop->unk_855[0];
    if (shop->unk_836 != temp_r1) {
        shop->unk_836 = temp_r1;
    }
}

// Nonmatching: overlay-id literal-pool entry (byte-identical when linked)
void func_ov043_020aee2c(ShopState *arg0) {
    ShopObject *shop;

    shop = &arg0->shop;
    CriSndMgr_Stop(0);
    if (gSaveData.unk_1AB4 & 4) {
        gSaveData.unk_1AB4 = (u16) (gSaveData.unk_1AB4 & ~4);
        func_ov043_020b1618(shop);
    } else {
        func_ov043_020b15d0(shop);
    }
    func_ov043_020b2af4((s32) shop);
    EasyTask_DestroyPool(&arg0->base.taskPool);
    ResourceMgr_ReinitManagers(NULL);
    DatMgr_ClearSlot(arg0->base.spareDataType);
    DatMgr_ClearSlot(arg0->base.dataType);
    func_ov043_020af3dc();
    FS_UnloadOverlay(0, (u32) &OVERLAY_31_ID);
    Mem_Free(&gDebugHeap, arg0);
    Mem_ValidateSequences(&gMainHeap);
    Mem_ValidateSequences(&gDebugHeap);
}

void func_ov043_020aeee0(ShopState *arg0) {
    s32 temp_r0;

    temp_r0 = MainOvlDisp_GetProcessStage();
    if (temp_r0 == 0x7FFFFFFF) {
        func_ov043_020aee2c(arg0);
        return;
    }
    ((OverlayCB *) &data_ov043_020ca18c)[temp_r0](arg0);
}

// Nonmatching: register scheduling in the Display_Init*BG* block
void func_ov043_020aef20(ShopState *arg0) {
    Interrupts_Init();
    HBlank_Init();
    GX_Init();
    func_0202b878();
    DMA_Init(0x100);
    Display_Init();
    GX_DisableBankForLcdc();
    GX_SetBankForLcdc(GX_VRAM_ALL);
    GX_SetBankForTex(GX_VRAM_A);
    GX_SetBankForTexPltt(GX_VRAM_G);
    GX_SetBankForBg(GX_VRAM_B);
    GX_SetBankForObj(GX_VRAM_E);
    GX_SetBankForBgExtPltt(GX_VRAM_NONE);
    GX_SetBankForObjExtPltt(GX_VRAM_NONE);
    GX_SetBankForSubBg(GX_VRAM_C);
    GX_SetBankForSubObj(GX_VRAM_D);
    GX_SetBankForSubBgExtPltt(GX_VRAM_NONE);
    GX_SetBankForSubObjExtPltt(GX_VRAM_I);
    MI_CpuFill(0, 0x06800000, 0xA4000);
    MI_CpuFill(0, 0x06000000, 0x80000);
    MI_CpuFill(0, 0x06200000, 0x20000);
    MI_CpuFill(0, 0x06400000, 0x40000);
    MI_CpuFill(0, 0x06600000, 0x20000);
    REG_POWER_CNT &= ~0x8000;
    Display_CommitSynced();
    g_DisplaySettings.controls[0].dispMode = GX_DISPMODE_GRAPHICS;
    g_DisplaySettings.controls[0].bgMode = GX_BGMODE_0;
    g_DisplaySettings.controls[0].dimension = GX2D3D_MODE_3D;
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX2D3D_MODE_3D);
    Display_InitMainBG1(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_256x256, GX_BG_COLORS_16, 0, 1, 0, 4);
    Display_InitMainBG2(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_256x256, GX_BG_COLORS_16, 1, 3, 1, 0x10C);
    Display_InitMainBG3(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_256x256, GX_BG_COLORS_16, 2, 3, 1, 0x20C);
    g_DisplaySettings.engineState[0].bgSettings[0].priority = 1;
    g_DisplaySettings.engineState[0].bgSettings[1].priority = 2;
    g_DisplaySettings.engineState[0].bgSettings[2].priority = 0;
    g_DisplaySettings.engineState[0].bgSettings[3].priority = 3;
    g_DisplaySettings.engineState[0].bgSettings[0].mosaic = 0;
    g_DisplaySettings.engineState[0].bgSettings[1].mosaic = 0;
    g_DisplaySettings.engineState[0].bgSettings[2].mosaic = 0;
    g_DisplaySettings.engineState[0].bgSettings[3].mosaic = 0;
    g_DisplaySettings.controls[0].objTileMode = GX_OBJTILEMODE_1D_128K;
    g_DisplaySettings.controls[0].objBmpMode = GX_OBJBMPMODE_1D_128K;
    g_DisplaySettings.controls[1].objTileMode = GX_OBJTILEMODE_1D_128K;
    g_DisplaySettings.controls[1].objBmpMode = GX_OBJBMPMODE_1D_128K;
    data_0206aa78 = 0x300010;
    data_0206aa7c = 0x400040;
    g_DisplaySettings.controls[0].layers = 0x1B;
    g_DisplaySettings.controls[0].brightness = 0x10;
    g_DisplaySettings.controls[1].bgMode = GX_BGMODE_0;
    GXs_SetGraphicsMode(GX_BGMODE_0);
    Display_InitSubBG1(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_256x256, GX_BG_COLORS_16, 0, 1, 0, 4);
    Display_InitSubBG3(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_256x256, GX_BG_COLORS_16, 1, 3, 1, 0x10C);
    g_DisplaySettings.engineState[1].bgSettings[0].priority = 0;
    g_DisplaySettings.engineState[1].bgSettings[1].priority = 1;
    g_DisplaySettings.engineState[1].bgSettings[2].priority = 2;
    g_DisplaySettings.engineState[1].bgSettings[3].priority = 3;
    g_DisplaySettings.engineState[1].bgSettings[0].mosaic = 0;
    g_DisplaySettings.engineState[1].bgSettings[1].mosaic = 0;
    g_DisplaySettings.engineState[1].bgSettings[2].mosaic = 0;
    g_DisplaySettings.engineState[1].bgSettings[3].mosaic = 0;
    g_DisplaySettings.controls[1].objTileMode = GX_OBJTILEMODE_1D_128K;
    g_DisplaySettings.controls[1].objBmpMode = GX_OBJBMPMODE_1D_128K;
    g_DisplaySettings.controls[1].layers = 0x1A;
    g_DisplaySettings.controls[1].brightness = 0x10;
    OamMgr_Init3DSpritePipeline();
    OamMgr_Swap3DBuffers();
    g_DisplaySettings.engineState[0].blendMode = 1;
    g_DisplaySettings.engineState[0].blendLayer0 = 4;
    g_DisplaySettings.engineState[0].blendLayer1 = 0x3B;
    g_DisplaySettings.engineState[0].blendCoeff0 = 6;
    g_DisplaySettings.engineState[0].blendCoeff1 = 0xA;
    OamMgr_InitExtended();
}

void func_ov043_020af2d8(ShopState *arg0) {
    if (!SystemStatusFlags.vblank) {
        return;
    }
    Display_Commit();
    DMA_Flush();
    DC_PurgeRange(g_OamMgr[0].oam, 0x400);
    GX_LoadOam(g_OamMgr[0].oam, 0U, 0x400U);
    DC_PurgeRange(g_OamMgr[1].oam, 0x400);
    GXs_LoadOam(g_OamMgr[1].oam, 0U, 0x400U);
    DC_PurgeRange(&data_02066aec, 0x400);
    GX_LoadBgPltt(&data_02066aec, 0, 0x200);
    GX_LoadObjPltt(&data_02066cec, 0, 0x200);
    DC_PurgeRange(&data_02066eec, 0x400);
    GXs_LoadBgPltt(&data_02066eec, 0, 0x200);
    GXs_LoadObjPltt(&data_020670ec, 0, 0x200);
    func_02001b44(2, 0, &data_020672ec, 0x400);
}

void func_ov043_020af3c0(ShopState *arg0) {
    void func_ov043_020af2d8(ShopState *arg0);

    func_ov043_020aef20(arg0);
    Interrupts_RegisterVBlankCallback((IRQCallback) func_ov043_020af2d8, 1);
}

void func_ov043_020af3dc(ShopState *arg0) {
    Interrupts_RegisterVBlankCallback(NULL, 1);
}
// ---- Shop_p4.c ----
BOOL func_ov043_020af3f0(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s16 arg4, s16 arg5) {
    return arg0 >= arg2 && arg0 <= arg2 + arg4 && arg1 >= arg3 && arg1 <= arg3 + arg5;
}

void func_ov043_020af42c(Sprite *sprite, s16 animFrame) {
    Data *data;
    u8 *base;
    void *animData;
    SpriteCell *cellTable;

    data = sprite->resourceData;
    if (data == NULL) {
        animData = NULL;
    } else {
        base = (u8 *) data->buffer + 0x20;
        animData = base + *(s32 *) (base + 0x18);
    }
    if (data == NULL) {
        cellTable = NULL;
    } else {
        base = (u8 *) data->buffer + 0x20;
        cellTable = (SpriteCell *) (base + *(s32 *) (base + 0x10));
    }
    Sprite_ChangeAnimation(sprite, animData, animFrame, cellTable);
}

void func_ov043_020af478(Sprite *sprite, s16 animFrame) {
    Data *data;
    u8 *base;
    void *animData;
    SpriteCell *cellTable;

    data = sprite->resourceData;
    if (data == NULL) {
        animData = NULL;
    } else {
        base = (u8 *) data->buffer + 0x20;
        animData = base + *(s32 *) (base + 0x20);
    }
    if (data == NULL) {
        cellTable = NULL;
    } else {
        base = (u8 *) data->buffer + 0x20;
        cellTable = (SpriteCell *) (base + *(s32 *) (base + 0x18));
    }
    Sprite_ChangeAnimation(sprite, animData, animFrame, cellTable);
}

void func_ov043_020af4c4(void *arg0, s32 arg1) {
    DatMgr_ReleaseData(DatMgr_LoadRawDataWithOffset(1, arg0, 0x34, &data_ov043_020ca210, arg1 * 0x34));
}

void func_ov043_020af4f0(void *arg0, s32 arg1) {
    DatMgr_ReleaseData(DatMgr_LoadRawDataWithOffset(1, arg0, 0x18, &data_ov043_020ca218, arg1 * 0x18));
}

void func_ov043_020af51c(void *arg0, s32 arg1) {
    DatMgr_ReleaseData(DatMgr_LoadRawDataWithOffset(1, arg0, 0x14, &data_ov043_020ca220, arg1 * 0x14));
}

void func_ov043_020af548(void *arg0, s32 arg1) {
    DatMgr_ReleaseData(DatMgr_LoadRawDataWithOffset(1, arg0, 8, &data_ov043_020ca228, arg1 * 8));
}

void func_ov043_020af574(void *arg0, s32 arg1) {
    DatMgr_ReleaseData(DatMgr_LoadRawDataWithOffset(1, arg0, 0xC, &data_ov043_020ca230, arg1 * 0xC));
}

void func_ov043_020af5a0(void *arg0, s32 arg1) {
    DatMgr_ReleaseData(DatMgr_LoadRawDataWithOffset(1, arg0, 0x2C, &data_ov043_020ca238, arg1 * 0x2C));
}

void func_ov043_020af5cc(void *arg0, s32 arg1) {
    DatMgr_ReleaseData(DatMgr_LoadRawDataWithOffset(1, arg0, 0x10, &data_ov043_020ca248, arg1 * 0x10));
}

void func_ov043_020af5f8(void *arg0, s32 arg1) {
    DatMgr_ReleaseData(DatMgr_LoadRawDataWithOffset(1, arg0, 0x1C, &data_ov043_020ca250, arg1 * 0x1C));
}

void func_ov043_020af624(void *arg0, s32 arg1) {
    DatMgr_ReleaseData(DatMgr_LoadRawData(1, arg0, 0x3DC0, &data_ov043_020ca210));
}

void func_ov043_020af648(void *arg0, s32 arg1) {
    DatMgr_ReleaseData(DatMgr_LoadRawData(1, arg0, 0x1A40, &data_ov043_020ca218));
}

void func_ov043_020af66c(void *arg0, s32 arg1) {
    DatMgr_ReleaseData(DatMgr_LoadRawData(1, arg0, 0x348, &data_ov043_020ca220));
}

void func_ov043_020af690(void *arg0, s32 arg1) {
    DatMgr_ReleaseData(DatMgr_LoadRawData(1, arg0, 0x4B0, &data_ov043_020ca228));
}
// ---- Shop_p5.c ----
void func_ov043_020af6b4(void *arg0, s32 arg1) {
    DatMgr_ReleaseData(DatMgr_LoadRawData(1, arg0, 0x11D0, &data_ov043_020ca240));
}

void func_ov043_020af6dc(void *arg0, s32 arg1) {
    DatMgr_ReleaseData(DatMgr_LoadRawData(1, arg0, 0xBA0, &data_ov043_020ca248));
}

void func_ov043_020af700(void *arg0, s32 arg1) {
    DatMgr_ReleaseData(DatMgr_LoadRawData(1, arg0, 0x2140, &data_ov043_020ca250));
}

void func_ov043_020af724(ShopObject *arg0) {
    typedef struct {
        /* 0x000 */ char pad_000[0x86C];
        /* 0x86C */ s32  unk_86C;
    } ShopP5View724;
    ShopP5View724 *v = (ShopP5View724 *)arg0;
    u8  mac[8];
    u16 i;
    s32 acc;

    func_0203a96c(mac);
    i = 0;
    v->unk_86C = 0;
    do {
        acc = v->unk_86C + mac[i];
        i += 1;
        v->unk_86C = acc;
    } while ((u32)i < 6U);
}

u16 func_ov043_020af770(u16 arg0, u32 arg1) {
    u16 t;

    if (arg0 == 0xCE) {
        t = arg1 & 1;
        if (t == 0) {
            return 0xCE;
        }
        return 0xCF;
    }
    if (arg0 == 0xD0) {
        t = arg1 & 3;
        switch (t) {
        case 0:
            return 0xD0;
        case 1:
            return 0xD1;
        case 2:
            return 0xD2;
        }
        return 0xD3;
    }
    if (arg0 == 0xD4) {
        t = arg1 % 6;
        switch (t) {
        case 0:
            return 0xD4;
        case 1:
            return 0xD5;
        case 2:
            return 0xD6;
        case 3:
            return 0xD7;
        case 4:
            return 0xD8;
        }
        return 0xD9;
    }
    if (arg0 == 0xDA) {
        t = arg1 % 3;
        switch (t) {
        case 0:
            return 0xDA;
        case 1:
            return 0xDB;
        }
        return 0xDC;
    }
    return arg0;
}

u16 func_ov043_020af8a8(ShopObject *arg0, u32 arg1, u32 arg2) {
    u64 q;

    q = (u64)arg1 * 1000 / arg2;
    if (q > 1000) {
        q = 1000;
    }
    return (u16)q;
}

u16 func_ov043_020af8e4(ShopObject *arg0) {
    typedef struct {
        /* 0x000 */ char pad_000[0x788];
        /* 0x788 */ u16  unk_788;
        /* 0x78A */ char pad_78A[0x790 - 0x78A];
        /* 0x790 */ u16  unk_790[9];
    } ShopP5View8e4;
    ShopP5View8e4 *v = (ShopP5View8e4 *)arg0;
    u16 temp_lr = v->unk_788;
    u16 var_ip = 0;
    s32 temp_r3;

    do {
        temp_r3 = v->unk_790[var_ip] * 10;
        if ((s32)temp_lr < temp_r3) {
            return var_ip + 1;
        }
        if (temp_lr == temp_r3) {
            return var_ip + 2;
        }
        var_ip += 1;
    } while ((u32)var_ip < 9U);
    return 10;
}

u16 func_ov043_020af950(ShopObject *arg0) {
    typedef struct {
        /* 0x000 */ char pad_000[0x790];
        /* 0x790 */ u16  unk_790[9];
    } ShopP5View950;
    ShopP5View950 *v = (ShopP5View950 *)arg0;
    u16 var_r2 = 0;

    do {
        if (v->unk_790[var_r2] == 100) {
            return var_r2 + 2;
        }
        var_r2 += 1;
    } while ((u32)var_r2 < 9U);
    return 10;
}

u32 func_ov043_020af994(ShopObject *arg0) {
    typedef struct {
        /* 0x000 */ char pad_000[0x778];
        /* 0x778 */ u16  unk_778;
        /* 0x77A */ u16  unk_77A;
        /* 0x77C */ char pad_77C[0x784 - 0x77C];
        /* 0x784 */ u32  unk_784;
        /* 0x788 */ char pad_788[0x790 - 0x788];
        /* 0x790 */ u16  unk_790[9];
    } ShopP5View994;
    ShopP5View994 *v = (ShopP5View994 *)arg0;
    u64 var_lr;

    if ((u32)v->unk_778 >= (u32)v->unk_77A) {
        var_lr = 100;
    } else {
        var_lr = v->unk_790[v->unk_778 - 1];
    }
    return (u64)v->unk_784 * var_lr / 100;
}

u16 func_ov043_020af9e8(ShopObject *arg0) {
    typedef struct {
        /* 0x000 */ char pad_000[0x788];
        /* 0x788 */ u16  unk_788;
        /* 0x78A */ char pad_78A[0x78E - 0x78A];
        /* 0x78E */ u16  unk_78E;
    } ShopP5View9e8;
    ShopP5View9e8 *v = (ShopP5View9e8 *)arg0;
    u16 temp_r2 = v->unk_788;

    if ((u32)temp_r2 >= 0x3E8U) {
        return 2;
    }
    if ((s32)temp_r2 >= (s32)(v->unk_78E * 10)) {
        return 1;
    }
    return 0;
}

void func_ov043_020afa20(ShopObject *arg0) {
    typedef struct {
        /* 0x000 */ char pad_000[0x77C];
        /* 0x77C */ s32  unk_77C;
        /* 0x780 */ s32  unk_780;
        /* 0x784 */ u32  unk_784;
        /* 0x788 */ u16  unk_788;
        /* 0x78A */ u16  unk_78A;
        /* 0x78C */ u16  unk_78C;
    } ShopP5Viewa20;
    ShopP5Viewa20 *v = (ShopP5Viewa20 *)arg0;

    v->unk_788 = func_ov043_020af8a8(arg0, v->unk_77C, v->unk_784);
    v->unk_78A = func_ov043_020af8a8(arg0, v->unk_780, v->unk_784);
    v->unk_78C = func_ov043_020af9e8(arg0);
}

u16 func_ov043_020afa68(ShopObject *arg0) {
    typedef struct {
        /* 0x000 */ char pad_000[0x778];
        /* 0x778 */ u16  unk_778;
        /* 0x77A */ char pad_77A[0x7A2 - 0x77A];
        /* 0x7A2 */ u16  unk_7A2[9];
    } ShopP5Viewa68;
    ShopP5Viewa68 *v = (ShopP5Viewa68 *)arg0;
    u16 var_r2 = 0;
    u16 var_r3 = 0;
    s32 temp_ip = v->unk_778 - 1;

    if (temp_ip > 0) {
        do {
            if ((u32)v->unk_7A2[var_r3] >= 0x3D4U) {
                var_r2 += 1;
            }
            var_r3 += 1;
        } while (temp_ip > (s32)var_r3);
    }
    return var_r2;
}

u16 func_ov043_020afabc(ShopObject *arg0) {
    typedef struct {
        /* 0x0 */ u16 unk_0;
        /* 0x2 */ u8  pad_2;
        /* 0x3 */ u8  unk_3;
    } ShopP5Recabc;
    typedef struct {
        /* 0x000 */ char        pad_000[0x7B9];
        /* 0x7B9 */ u8          unk_7B9;
        /* 0x7BA */ char        pad_7BA[0x7BC - 0x7BA];
        /* 0x7BC */ ShopP5Recabc unk_7BC[17];
    } ShopP5Viewabc;
    ShopP5Viewabc *v = (ShopP5Viewabc *)arg0;
    u16 var_r4;
    u8  var_r5 = 0;

    var_r4 = 0;
    if ((s32)v->unk_7B9 > 0) {
        do {
            if (func_02023b7c(v->unk_7BC[var_r4].unk_0) == 1) {
                var_r5 += 1;
            }
            var_r4 += 1;
        } while ((s32)v->unk_7B9 > (s32)var_r4);
    }
    return var_r5;
}

u16 func_ov043_020afb14(ShopObject *arg0) {
    typedef struct {
        /* 0x0 */ u16 unk_0;
        /* 0x2 */ u8  pad_2;
        /* 0x3 */ u8  unk_3;
    } ShopP5Recb14;
    typedef struct {
        /* 0x000 */ char        pad_000[0x7B8];
        /* 0x7B8 */ u8          unk_7B8;
        /* 0x7B9 */ u8          unk_7B9;
        /* 0x7BA */ char        pad_7BA[0x7BC - 0x7BA];
        /* 0x7BC */ ShopP5Recb14 unk_7BC[17];
    } ShopP5Viewb14;
    ShopP5Viewb14 *v = (ShopP5Viewb14 *)arg0;
    u8  temp_lr = v->unk_7B9;
    u8  var_r2 = 0;
    u16 var_r3 = 0;
    u8  ip;

    if ((s32)temp_lr > 0) {
        ip = v->unk_7B8;
        do {
            if (ip >= v->unk_7BC[var_r3].unk_3) {
                var_r2 += 1;
            }
            var_r3 += 1;
        } while ((s32)temp_lr > (s32)var_r3);
    }
    return var_r2;
}

void func_ov043_020afb60(ShopObject *arg0) {
    typedef struct {
        /* 0x000 */ char pad_000[0x860];
        /* 0x860 */ u16  unk_860;
    } ShopP5Viewb60;
    typedef struct {
        u8 unk_0 : 1;
    } ShopP5Flag;
    ShopP5Viewb60 *v = (ShopP5Viewb60 *)arg0;
    ShopP5Flag *flags;
    u16 var_r8;

    var_r8 = 0;
    if ((u32)v->unk_860 <= (u32)var_r8) {
        return;
    }
    flags = (ShopP5Flag *)&gSaveData.unk_1EB2;
    do {
        ShopItemSlot *slot = &arg0->slots[var_r8];
        if (slot->unk_04 == 1 && flags[Inventory_GetCategorizedIndex(slot->itemID)].unk_0 == 1) {
            slot->validFlag = 1;
        }
        var_r8 += 1;
    } while ((u32)v->unk_860 > (u32)var_r8);
}
// ---- Shop_p6.c ----
// ---------------------------------------------------------------------------
// Shop_p6: item-table construction, category strip toggles, and the save
// commit paths for the Shop menu.
//
// Each function carries its own block-scope mirror of the tail of the
// ShopObject ("ShopP6") so that the file compiles both standalone and as part
// of the combined Shop.c unit.
//
// NOTE: the shared header declares func_ov043_020af624..af700 with an extra
// unused s32 parameter; the retail callees take a single pointer, so the call
// sites below use a function-pointer cast to keep the emitted bl 1-argument.
// ---------------------------------------------------------------------------

s32 func_ov043_020afbd4(ShopObject *arg0, u16 arg1) {
    typedef struct {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ u8  unk_02;
        /* 0x03 */ u8  unk_03;
    } ShopP6Entry;
    typedef struct {
        /* 0x000 */ char         pad_000[0x7B8];
        /* 0x7B8 */ u8           unk_7B8;
        /* 0x7B9 */ u8           unk_7B9;
        /* 0x7BA */ char         pad_7BA[0x7BC - 0x7BA];
        /* 0x7BC */ ShopP6Entry  unk_7BC[30];
        /* 0x834 */ char         pad_834[0x84C - 0x834];
        /* 0x84C */ u16          unk_84C;
    } ShopP6;

    ShopP6 *shop = (ShopP6 *) arg0;
    u16 var_r9;
    u16 var_r6;
    s32 temp_r7;
    s32 temp_r8;

    for (var_r9 = arg1; (s32) var_r9 < (s32) shop->unk_7B9; var_r9++) {
        temp_r7 = shop->unk_7BC[var_r9].unk_00;
        temp_r8 = Inventory_GetCategorizedIndex(temp_r7);
        for (var_r6 = 0; (u32) var_r6 < 0x1D8U; var_r6++) {
            if ((temp_r7 == gSaveData.inventoryItems[var_r6].itemID) &&
                ((u32) shop->unk_7B8 >= (u32) shop->unk_7BC[var_r9].unk_03) &&
                (func_02023b7c(temp_r7) == 0)) {
                gSaveData.unk_1EB2[temp_r8] =
                    (u8) ((gSaveData.unk_1EB2[temp_r8] & ~1) | 1);
                func_ov043_020afb60(arg0);
                shop->unk_84C = temp_r7;
                return 1;
            }
        }
    }
    return 0;
}

s32 func_ov043_020afcb0(ShopObject *arg0) {
    typedef struct {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ char pad_02[0x2C - 0x02];
    } ShopP6Slot;
    typedef struct {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ u8  unk_02;
        /* 0x03 */ u8  unk_03;
    } ShopP6Entry;
    typedef struct {
        /* 0x000 */ ShopP6Slot  slots[42];
        /* 0x738 */ char        pad_738[0x7B8 - 0x738];
        /* 0x7B8 */ u8          unk_7B8;
        /* 0x7B9 */ u8          unk_7B9;
        /* 0x7BA */ char        pad_7BA[0x7BC - 0x7BA];
        /* 0x7BC */ ShopP6Entry unk_7BC[30];
        /* 0x834 */ char        pad_834[0x848 - 0x834];
        /* 0x848 */ u16         unk_848;
        /* 0x84A */ char        pad_84A[0x84C - 0x84A];
        /* 0x84C */ u16         unk_84C;
    } ShopP6;

    ShopP6 *shop = (ShopP6 *) arg0;
    u16 var_r4;
    s32 temp_r5;
    u16 temp_r6;

    temp_r5 = shop->slots[shop->unk_848].unk_00;
    temp_r6 = Inventory_GetCategorizedIndex(temp_r5);
    for (var_r4 = 0; (s32) shop->unk_7B9 > (s32) var_r4; var_r4++) {
        if ((temp_r5 == shop->unk_7BC[var_r4].unk_00) &&
            ((u32) shop->unk_7B8 >= (u32) shop->unk_7BC[var_r4].unk_03) &&
            (func_02023b7c(temp_r5) == 0)) {
            gSaveData.unk_1EB2[temp_r6] =
                (u8) ((gSaveData.unk_1EB2[temp_r6] & ~1) | 1);
            func_ov043_020afb60(arg0);
            shop->unk_84C = temp_r5;
            return 1;
        }
    }
    return 0;
}

// Nonmatching: register allocation: arg register and push set differ
void func_ov043_020afd6c(ShopObject *arg0) {
    typedef struct {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ u16 unk_02;
        /* 0x04 */ u16 unk_04;
        /* 0x06 */ s16 unk_06;
        /* 0x08 */ u32 unk_08;
        /* 0x0C */ u8  unk_0C;
        /* 0x0D */ u8  unk_0D;
        /* 0x0E */ u16 unk_0E;
        /* 0x10 */ s32 unk_10;
        /* 0x14 */ u16 unk_14;
        /* 0x16 */ u16 unk_16[3];
        /* 0x1C */ u8  unk_1C[3];
        /* 0x1F */ u8  unk_1F;
        /* 0x20 */ s32 unk_20;
        /* 0x24 */ u8  unk_24[7];
        /* 0x2B */ u8  unk_2B;
    } ShopP6Slot;
    typedef struct {
        /* 0x000 */ ShopP6Slot slots[42];
        /* 0x738 */ char       pad_738[0x778 - 0x738];
        /* 0x778 */ u16        unk_778;
        /* 0x77A */ char       pad_77A[0x7A2 - 0x77A];
        /* 0x7A2 */ u16        unk_7A2[9];
        /* 0x7B4 */ char       pad_7B4[0x84E - 0x7B4];
        /* 0x84E */ u16        unk_84E;
        /* 0x850 */ char       pad_850[0x860 - 0x850];
        /* 0x860 */ u16        unk_860;
        /* 0x862 */ char       pad_862[0x86C - 0x862];
        /* 0x86C */ u32        unk_86C;
    } ShopP6;
    typedef struct {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ u16 unk_02;
        /* 0x04 */ u32 unk_04;
        /* 0x08 */ u16 unk_08[9];
        /* 0x1A */ u16 unk_1A[9];
    } ShopP6Menu0;
    typedef struct {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ u8  pad_02[6];
        /* 0x08 */ u16 unk_08;
        /* 0x0A */ u8  pad_0A[0x12];
        /* 0x1C */ u16 unk_1C;
        /* 0x1E */ u8  unk_1E;
        /* 0x1F */ u8  pad_1F[0x15];
        /* 0x34 */ u8  unk_34;
        /* 0x35 */ u8  unk_35;
        /* 0x36 */ u8  unk_36;
        /* 0x37 */ u8  unk_37;
        /* 0x38 */ u8  unk_38;
        /* 0x39 */ u8  pad_39;
        /* 0x3A */ s16 unk_3A;
        /* 0x3C */ u8  pad_3C[0x10];
        /* 0x4C */ u16 unk_4C;
        /* 0x4E */ u8  pad_4E[2];
        /* 0x50 */ u16 unk_50;
        /* 0x52 */ u8  pad_52[3];
        /* 0x55 */ u8  unk_55;
        /* 0x56 */ u8  pad_56[0x84 - 0x56];
        /* 0x84 */ u16 unk_84_00;
        /* 0x86 */ u16 unk_84_02;
        /* 0x88 */ u16 unk_84_04[3];
        /* 0x8E */ u8  unk_84_0A[3];
        /* 0x91 */ u8  unk_84_0D;
    } ShopP6Frame;

    ShopP6 *shop = (ShopP6 *) arg0;
    ShopP6Frame sp;
    u16 temp_r1;
    u16 temp_r5;
    s32 temp_r4;
    u16 temp_r4_2;
    s32 temp_r9;
    s32 temp_r8;
    s16 temp_r0_2;
    s32 temp_r6;
    ShopP6Slot *slot;
    u16 var;

    temp_r1 = shop->unk_778;
    temp_r5 = shop->unk_860;
    if ((u32) temp_r1 < 2U) {
        return;
    }
    temp_r4 = temp_r1 - 2;
    if ((u32) shop->unk_7A2[temp_r4] >= 0x3D4U) {
        return;
    }
    if ((u32) shop->unk_7A2[temp_r4] < 0x320U) {
        OS_WaitForever();
    }
    temp_r4_2 = shop->unk_7A2[temp_r4] - 0x320;
    func_ov043_020af5cc(&sp.unk_84_00, (s32) temp_r4_2);
    temp_r9 = func_ov043_020af770(sp.unk_84_02, shop->unk_86C);
    shop->unk_84E = temp_r9;
    temp_r8 = Inventory_GetCategorizedIndex(shop->unk_84E);
    temp_r0_2 = Inventory_GetCategory(shop->unk_84E);
    temp_r6 = temp_r5 * 0x2C;
    slot = &shop->slots[temp_r5];
    slot->unk_00 = temp_r9;
    slot->unk_04 = temp_r0_2;
    slot->unk_08 = 0;
    switch (temp_r0_2) {
    default:
        break;
    case 0:
        func_ov043_020af4c4(&sp.unk_50, temp_r8);
        func_ov043_020af5f8(&sp.unk_34, temp_r8);
        slot->unk_02 = sp.unk_50;
        slot->unk_06 = (s16) sp.unk_55;
        slot->unk_20 = 0;
        slot->unk_24[0] = (s8) (sp.unk_38 + 1);
        slot->unk_24[1] = (s8) ((s32) sp.unk_3A / 409);
        slot->unk_24[2] = (s8) ((s32) sp.unk_4C / 60);
        slot->unk_24[3] = sp.unk_34;
        slot->unk_24[4] = sp.unk_35;
        slot->unk_24[5] = sp.unk_36;
        slot->unk_24[5] = sp.unk_37;
        break;
    case 1:
        func_ov043_020af4f0(&sp.unk_1C, temp_r8);
        slot->unk_02 = sp.unk_1C;
        slot->unk_06 = (s16) sp.unk_1E;
        if (((u32) (gSaveData.unk_1EB2[temp_r8] << 0x1F) >> 0x1F) == 1) {
            slot->unk_20 = 1;
        } else {
            slot->unk_20 = 0;
        }
        for (var = 0; (u32) var < 7U; var++) {
            slot->unk_24[var] = 0;
        }
        break;
    case 2:
        func_ov043_020af51c(&sp.unk_08, temp_r8);
        slot->unk_02 = sp.unk_08;
        slot->unk_06 = 0xE;
        slot->unk_20 = 0;
        for (var = 0; (u32) var < 7U; var++) {
            slot->unk_24[var] = 0;
        }
        break;
    case 3:
        func_ov043_020af548(&sp.unk_00, temp_r8);
        slot->unk_02 = sp.unk_00;
        slot->unk_06 = 0xE;
        slot->unk_20 = 0;
        for (var = 0; (u32) var < 7U; var++) {
            slot->unk_24[var] = 0;
        }
        break;
    }
    slot->unk_10 = 1;
    slot->unk_14 = temp_r4_2;
    slot->unk_0C = sp.unk_84_0D;
    for (var = 0; (u32) var < 3U; var++) {
        slot->unk_16[var] = sp.unk_84_04[var];
        slot->unk_1C[var] = sp.unk_84_0A[var];
    }
    shop->unk_860 = (u16) (shop->unk_860 + 1);
}

s32 func_ov043_020b0088(u16 arg0) {
    s32 cat = Inventory_GetCategory(arg0);
    u16 var_r1 = 0;
    u16 var;

    if ((u32) arg0 >= 0x308U) {
        return 0;
    }
    if (cat == 0) {
        for (var = 0; (u32) var < 0x130U; var++) {
            if (arg0 == gSaveData.masteredPins[var].pinID) {
                var_r1 += gSaveData.masteredPins[var].count;
                break;
            }
        }
    } else {
        for (var = 0; (u32) var < 0x1D8U; var++) {
            if (arg0 == gSaveData.inventoryItems[var].itemID) {
                var_r1 = gSaveData.inventoryItems[var].flags.bits.count;
                break;
            }
        }
    }
    return var_r1;
}

void func_ov043_020b0154(s32 arg0, s32 arg1) {
    gSaveData.masteredPins[arg0].count -= arg1;
    if (gSaveData.masteredPins[arg0].count != 0) {
        return;
    }
    gSaveData.masteredPins[arg0].pinID = 0xFFFF;
    gSaveData.masteredPins[arg0].flags.bits.level = 0x64;
    gSaveData.masteredPins[arg0].flags.bits.unk_07 = 1;
}

void func_ov043_020b01ac(s32 arg0, s32 arg1) {
    u8 v = gSaveData.inventoryItems[arg0].flags.raw;

    gSaveData.inventoryItems[arg0].flags.raw =
        (v & ~0xF) | ((u8) (((u32) (v << 0x1C) >> 0x1C) - arg1) & 0xF);
}

void func_ov043_020b01d8(ShopObject *arg0, s32 arg1) {
    typedef struct {
        /* 0x00 */ char pad_00[0x16];
        /* 0x16 */ u16  unk_16[3];
        /* 0x1C */ u8   unk_1C[3];
    } ShopP6Slot;
    typedef struct {
        /* 0x000 */ char        pad_000[0x738];
        /* 0x738 */ ShopP6Slot *unk_738[12];
    } ShopP6;

    ShopP6 *shop = (ShopP6 *) arg0;
    u16 var_r4;

    for (var_r4 = 0; (u32) var_r4 < 3U; var_r4++) {
        ShopP6Slot *slot = shop->unk_738[arg1];
        u16 temp_r5 = slot->unk_16[var_r4];
        u8  temp_r6 = slot->unk_1C[var_r4];
        s32 temp_r0 = Inventory_GetCategory(temp_r5);

        if (temp_r6 != 0) {
            u16 var_r0;

            if (temp_r0 == 0) {
                for (var_r0 = 0; (u32) var_r0 < 0x130U; var_r0++) {
                    if (temp_r5 == gSaveData.masteredPins[var_r0].pinID) {
                        func_ov043_020b0154(var_r0, temp_r6);
                        break;
                    }
                }
            } else {
                for (var_r0 = 0; (u32) var_r0 < 0x1D8U; var_r0++) {
                    if (temp_r5 == gSaveData.inventoryItems[var_r0].itemID) {
                        func_ov043_020b01ac(var_r0, temp_r6);
                        break;
                    }
                }
            }
        }
    }
}

u16 func_ov043_020b02ac(u16 arg0) {
    if ((u32) arg0 < 0xCU) {
        return 12;
    }
    if ((s32) arg0 % 6 == 0) {
        return arg0;
    }
    return (u16) ((((s32) arg0 / 6) + 1) * 6);
}

// Nonmatching: register/spill allocation across the 2.5 KB body
void func_ov043_020b02fc(ShopObject *arg0) {
    typedef struct {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ u16 unk_02;
        /* 0x04 */ u16 unk_04;
        /* 0x06 */ s16 unk_06;
        /* 0x08 */ u32 unk_08;
        /* 0x0C */ u8  unk_0C;
        /* 0x0D */ u8  unk_0D;
        /* 0x0E */ u16 unk_0E;
        /* 0x10 */ s32 unk_10;
        /* 0x14 */ u16 unk_14;
        /* 0x16 */ u16 unk_16[3];
        /* 0x1C */ u8  unk_1C[3];
        /* 0x1F */ u8  unk_1F;
        /* 0x20 */ s32 unk_20;
        /* 0x24 */ u8  unk_24[7];
        /* 0x2B */ u8  unk_2B;
    } ShopP6Slot;
    typedef struct {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ u8  unk_02;
        /* 0x03 */ u8  unk_03;
    } ShopP6Entry;
    typedef struct {
        /* 0x000 */ ShopP6Slot  slots[42];
        /* 0x738 */ ShopP6Slot *unk_738[12];
        /* 0x768 */ u16         unk_768;
        /* 0x76A */ s16         unk_76A;
        /* 0x76C */ s16         unk_76C;
        /* 0x76E */ u16         unk_76E;
        /* 0x770 */ u16         unk_770;
        /* 0x772 */ u16         unk_772;
        /* 0x774 */ u16         unk_774;
        /* 0x776 */ u16         unk_776;
        /* 0x778 */ u16         unk_778;
        /* 0x77A */ u16         unk_77A;
        /* 0x77C */ u32         unk_77C;
        /* 0x780 */ s32         unk_780;
        /* 0x784 */ u32         unk_784;
        /* 0x788 */ s16         unk_788;
        /* 0x78A */ s16         unk_78A;
        /* 0x78C */ s16         unk_78C;
        /* 0x78E */ u16         unk_78E;
        /* 0x790 */ u16         unk_790[9];
        /* 0x7A2 */ u16         unk_7A2[9];
        /* 0x7B4 */ u8          unk_7B4;
        /* 0x7B5 */ u8          unk_7B5;
        /* 0x7B6 */ u8          unk_7B6;
        /* 0x7B7 */ u8          unk_7B7;
        /* 0x7B8 */ u8          unk_7B8;
        /* 0x7B9 */ u8          unk_7B9;
        /* 0x7BA */ u8          unk_7BA;
        /* 0x7BB */ u8          unk_7BB;
        /* 0x7BC */ ShopP6Entry unk_7BC[30];
        /* 0x834 */ u16         unk_834;
        /* 0x836 */ u8          unk_836;
        /* 0x837 */ u8          unk_837;
        /* 0x838 */ u8          unk_838;
        /* 0x839 */ u8          unk_839;
        /* 0x83A */ u16         unk_83A;
        /* 0x83C */ s16         unk_83C;
        /* 0x83E */ u16         unk_83E;
        /* 0x840 */ s16         unk_840;
        /* 0x842 */ s16         unk_842;
        /* 0x844 */ u16         unk_844;
        /* 0x846 */ u16         unk_846;
        /* 0x848 */ u16         unk_848;
        /* 0x84A */ u16         unk_84A;
        /* 0x84C */ u16         unk_84C;
        /* 0x84E */ u16         unk_84E;
        /* 0x850 */ u16         unk_850;
        /* 0x852 */ u16         unk_852;
        /* 0x854 */ u8          unk_854;
        /* 0x855 */ u8          unk_855;
        /* 0x856 */ u16         unk_856;
        /* 0x858 */ u32         unk_858;
        /* 0x85C */ u16         unk_85C;
        /* 0x85E */ u16         unk_85E;
        /* 0x860 */ u16         unk_860;
        /* 0x862 */ u16         unk_862;
        /* 0x864 */ u8          unk_864[3];
        /* 0x867 */ u8          unk_867;
        /* 0x868 */ u32         unk_868;
        /* 0x86C */ u32         unk_86C;
        /* 0x870 */ u16         unk_870;
        /* 0x872 */ u16         unk_872;
    } ShopP6;
    typedef struct {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ u16 unk_02;
        /* 0x04 */ u16 unk_04;
        /* 0x06 */ u16 unk_06;
    } ShopP6Sale;
    typedef struct {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ u16 unk_02;
        /* 0x04 */ u16 unk_04[3];
        /* 0x0A */ u8  unk_0A[3];
        /* 0x0D */ u8  unk_0D;
        /* 0x0E */ u8  pad_0E[2];
    } ShopP6ShopItem;
    typedef struct {
        /* 0x00 */ u8  unk_00;
        /* 0x01 */ u8  unk_01;
        /* 0x02 */ u8  unk_02;
        /* 0x03 */ u8  unk_03;
        /* 0x04 */ u8  unk_04;
        /* 0x05 */ u8  unk_05;
        /* 0x06 */ s16 unk_06;
        /* 0x08 */ u8  pad_08[0x10];
        /* 0x18 */ u16 unk_18;
        /* 0x1A */ u8  pad_1A[2];
    } ShopP6PinInfo;

    ShopP6 *shop = (ShopP6 *) arg0;
    RawPinData      *pinData;
    RawItemData     *itemData;
    RawFoodData     *foodData;
    RawTreasureData *treasureData;
    ShopP6Sale      *sale;
    ShopP6ShopItem  *shopItem;
    ShopP6PinInfo   *pinInfo;
    u16              var_r4;
    u16              var_r7;
    u16              var_r8;
    u16              var_r5;
    u16              var;
    s32              temp_r10;
    u16              temp_r4_2;
    ShopP6Slot      *slot;
    s32              catIdx;
    s32              category;
    u32              item;

    pinData = Mem_AllocHeapTail(&gDebugHeap, 0x3DC0);
    Mem_SetSequence(&gDebugHeap, pinData, &data_ov043_020cc82c);
    itemData = Mem_AllocHeapTail(&gDebugHeap, 0x1A40);
    Mem_SetSequence(&gDebugHeap, itemData, &data_ov043_020cc838);
    foodData = Mem_AllocHeapTail(&gDebugHeap, 0x348);
    Mem_SetSequence(&gDebugHeap, foodData, &data_ov043_020cc844);
    treasureData = Mem_AllocHeapTail(&gDebugHeap, 0x4B0);
    Mem_SetSequence(&gDebugHeap, treasureData, &data_ov043_020cc850);
    sale = Mem_AllocHeapTail(&gDebugHeap, 0x11D0);
    Mem_SetSequence(&gDebugHeap, sale, &data_ov043_020cc860);
    shopItem = Mem_AllocHeapTail(&gDebugHeap, 0xBA0);
    Mem_SetSequence(&gDebugHeap, shopItem, &data_ov043_020cc870);
    pinInfo = Mem_AllocHeapTail(&gDebugHeap, 0x2140);
    Mem_SetSequence(&gDebugHeap, pinInfo, &data_ov043_020cc880);
    ((void (*)(void *)) func_ov043_020af624)(pinData);
    ((void (*)(void *)) func_ov043_020af648)(itemData);
    ((void (*)(void *)) func_ov043_020af66c)(foodData);
    ((void (*)(void *)) func_ov043_020af690)(treasureData);
    ((void (*)(void *)) func_ov043_020af6b4)(sale);
    ((void (*)(void *)) func_ov043_020af6dc)(shopItem);
    ((void (*)(void *)) func_ov043_020af700)(pinInfo);

    var_r4 = 0;
    var_r7 = 0;
    do {
        slot = &shop->slots[var_r4];
        slot->unk_00 = 0xFFFF;
        slot->unk_02 = 0;
        slot->unk_04 = 0;
        slot->unk_06 = 0xE;
        slot->unk_08 = 0;
        slot->unk_0C = 1;
        slot->unk_10 = 0;
        slot->unk_14 = 0xFFFF;
        slot->unk_20 = 0;
        for (var = 0; (u32) var < 7U; var++) {
            slot->unk_24[var] = 0;
        }
        for (var = 0; (u32) var < 3U; var++) {
            slot->unk_16[var] = 0xFFFF;
            slot->unk_1C[var] = 0;
        }
        var_r4 += 1;
    } while ((u32) var_r4 < 0x2AU);

    var_r8 = 0;
    do {
        if (shop->unk_768 == sale[var_r8].unk_00) {
            item = func_ov043_020af770(sale[var_r8].unk_02, shop->unk_86C);
            temp_r10 = sale[var_r8].unk_04;
            if ((Inventory_GetOwnedCount(0x2AE) != 0) || ((s32) (gSaveData.chapter + 1) >= (s32) sale[var_r8].unk_06)) {
                if (item < 0x320) {
                    catIdx = Inventory_GetCategorizedIndex((u16) item);
                    category = Inventory_GetCategory((u16) item);
                    slot = &shop->slots[var_r7];
                    slot->unk_00 = (u16) item;
                    slot->unk_04 = category;
                    switch (category) {
                    default:
                        break;
                    case 0:
                        slot->unk_02 = pinData[catIdx].iconIndex;
                        slot->unk_06 = (s16) pinData[catIdx].brand;
                        slot->unk_08 = (u32) (temp_r10 * pinData[catIdx].price) / 100;
                        slot->unk_20 = 0;
                        slot->unk_24[0] = (s8) (pinInfo[catIdx].unk_04 + 1);
                        slot->unk_24[1] = (s8) ((s32) pinInfo[catIdx].unk_06 / 409);
                        slot->unk_24[2] = (s8) ((s32) pinInfo[catIdx].unk_18 / 60);
                        slot->unk_24[3] = pinInfo[catIdx].unk_00;
                        slot->unk_24[4] = pinInfo[catIdx].unk_01;
                        slot->unk_24[5] = pinInfo[catIdx].unk_02;
                        slot->unk_24[6] = pinInfo[catIdx].unk_03;
                        break;
                    case 1:
                        slot->unk_02 = itemData[catIdx].unk_00;
                        slot->unk_06 = (s16) itemData[catIdx].brand;
                        slot->unk_08 = (u32) (temp_r10 * itemData[catIdx].unk_04) / 100;
                        if (((u32) (gSaveData.unk_1EB2[catIdx] << 0x1F) >> 0x1F) == 1) {
                            slot->unk_20 = 1;
                        } else {
                            slot->unk_20 = 0;
                        }
                        break;
                    case 2:
                        slot->unk_02 = *(u16 *) (foodData + catIdx);
                        slot->unk_06 = 0xE;
                        slot->unk_08 = (u32) (temp_r10 * foodData[catIdx].unk_04) / 100;
                        slot->unk_20 = 0;
                        break;
                    case 3:
                        slot->unk_02 = *(u16 *) (treasureData + catIdx);
                        slot->unk_06 = 0xE;
                        slot->unk_08 = (u32) (temp_r10 * treasureData[catIdx].unk_04) / 100;
                        slot->unk_20 = 0;
                        break;
                    }
                    slot->unk_0C = 1;
                    slot->unk_10 = 0;
                    slot->unk_14 = 0xFFFF;
                } else {
                    temp_r4_2 = (u16) (item - 0x320);
                    item = shopItem[temp_r4_2].unk_02;
                    catIdx = Inventory_GetCategorizedIndex((u16) item);
                    category = Inventory_GetCategory((u16) item);
                    slot = &shop->slots[var_r7];
                    slot->unk_00 = (u16) item;
                    slot->unk_04 = category;
                    slot->unk_08 = 0;
                    switch (category) {
                    default:
                        break;
                    case 0:
                        slot->unk_02 = pinData[catIdx].iconIndex;
                        slot->unk_06 = (s16) pinData[catIdx].brand;
                        slot->unk_20 = 0;
                        slot->unk_24[0] = (s8) (pinInfo[catIdx].unk_04 + 1);
                        slot->unk_24[1] = (s8) ((s32) pinInfo[catIdx].unk_06 / 409);
                        slot->unk_24[2] = (s8) ((s32) pinInfo[catIdx].unk_18 / 60);
                        slot->unk_24[3] = pinInfo[catIdx].unk_00;
                        slot->unk_24[4] = pinInfo[catIdx].unk_01;
                        slot->unk_24[5] = pinInfo[catIdx].unk_02;
                        slot->unk_24[6] = pinInfo[catIdx].unk_03;
                        break;
                    case 1:
                        slot->unk_02 = itemData[catIdx].unk_00;
                        slot->unk_06 = (s16) itemData[catIdx].brand;
                        if (((u32) (gSaveData.unk_1EB2[catIdx] << 0x1F) >> 0x1F) == 1) {
                            slot->unk_20 = 1;
                        } else {
                            slot->unk_20 = 0;
                        }
                        break;
                    case 2:
                        slot->unk_02 = *(u16 *) (foodData + catIdx);
                        slot->unk_06 = 0xE;
                        slot->unk_20 = 0;
                        break;
                    case 3:
                        slot->unk_02 = *(u16 *) (treasureData + catIdx);
                        slot->unk_06 = 0xE;
                        slot->unk_20 = 0;
                        break;
                    }
                    slot->unk_10 = 1;
                    slot->unk_14 = temp_r4_2;
                    slot->unk_0C = shopItem[temp_r4_2].unk_0D;
                    for (var = 0; (u32) var < 3U; var++) {
                        slot->unk_16[var] = shopItem[temp_r4_2].unk_04[var];
                        slot->unk_1C[var] = shopItem[temp_r4_2].unk_0A[var];
                    }
                }
                var_r7 += 1;
                if ((u32) var_r7 >= 0x2AU) {
                    OS_WaitForever();
                }
            }
        }
        var_r8 += 1;
    } while ((u32) var_r8 < 0x23AU);

    var_r5 = 0;
    if ((s32) (shop->unk_778 - 1) > 0) {
        do {
            if ((u32) shop->unk_7A2[var_r5] < 0x3D4U) {
                if ((u32) shop->unk_7A2[var_r5] < 0x320U) {
                    OS_WaitForever();
                }
                temp_r4_2 = (u16) (shop->unk_7A2[var_r5] - 0x320);
                item = func_ov043_020af770(shopItem[temp_r4_2].unk_02, shop->unk_86C);
                catIdx = Inventory_GetCategorizedIndex((u16) item);
                category = Inventory_GetCategory((u16) item);
                slot = &shop->slots[var_r7];
                slot->unk_00 = (u16) item;
                slot->unk_04 = category;
                slot->unk_08 = 0;
                switch (category) {
                default:
                    break;
                case 0:
                    slot->unk_02 = pinData[catIdx].iconIndex;
                    slot->unk_06 = (s16) pinData[catIdx].brand;
                    slot->unk_20 = 0;
                    slot->unk_24[0] = (s8) (pinInfo[catIdx].unk_04 + 1);
                    slot->unk_24[1] = (s8) ((s32) pinInfo[catIdx].unk_06 / 409);
                    slot->unk_24[2] = (s8) ((s32) pinInfo[catIdx].unk_18 / 60);
                    slot->unk_24[3] = pinInfo[catIdx].unk_00;
                    slot->unk_24[4] = pinInfo[catIdx].unk_01;
                    slot->unk_24[5] = pinInfo[catIdx].unk_02;
                    slot->unk_24[6] = pinInfo[catIdx].unk_03;
                    break;
                case 1:
                    slot->unk_02 = itemData[catIdx].unk_00;
                    slot->unk_06 = (s16) itemData[catIdx].brand;
                    if (((u32) (gSaveData.unk_1EB2[catIdx] << 0x1F) >> 0x1F) == 1) {
                        slot->unk_20 = 1;
                    } else {
                        slot->unk_20 = 0;
                    }
                    break;
                case 2:
                    slot->unk_02 = *(u16 *) (foodData + catIdx);
                    slot->unk_06 = 0xE;
                    slot->unk_20 = 0;
                    break;
                case 3:
                    slot->unk_02 = *(u16 *) (treasureData + catIdx);
                    slot->unk_06 = 0xE;
                    slot->unk_20 = 0;
                    break;
                }
                slot->unk_10 = 1;
                slot->unk_14 = temp_r4_2;
                slot->unk_0C = shopItem[temp_r4_2].unk_0D;
                for (var = 0; (u32) var < 3U; var++) {
                    slot->unk_16[var] = shopItem[temp_r4_2].unk_04[var];
                    slot->unk_1C[var] = shopItem[temp_r4_2].unk_0A[var];
                }
                var_r7 += 1;
            }
            var_r5 += 1;
        } while ((s32) (shop->unk_778 - 1) > (s32) var_r5);
    }
    shop->unk_860 = var_r7;
    func_ov043_020b02ac(shop->unk_860);
    shop->unk_862 = (u16) func_ov043_020b02ac(shop->unk_860);
    shop->unk_870 = (u16) ((shop->unk_862 / 6) - 2);
    shop->unk_872 = shop->unk_870;
    var_r7 = 0;
    do {
        shop->unk_7BC[var_r7].unk_00 = 0xFFFF;
        shop->unk_7BC[var_r7].unk_02 = 0;
        shop->unk_7BC[var_r7].unk_03 = 0xA;
        var_r7 += 1;
    } while ((u32) var_r7 < 0x1EU);
    var_r7 = 0;
    var_r8 = 0;
    do {
        if (shop->unk_774 == itemData[var_r7].unk_12) {
            shop->unk_7BC[var_r8].unk_00 = (s16) (var_r7 + 0x130);
            shop->unk_7BC[var_r8].unk_02 = itemData[var_r7].unk_10;
            shop->unk_7BC[var_r8].unk_03 = itemData[var_r7].unk_14;
            var_r8 += 1;
        }
        var_r7 += 1;
    } while ((u32) var_r7 < 0x118U);
    shop->unk_7B9 = (s8) var_r8;
    Mem_Free(&gDebugHeap, pinData);
    Mem_Free(&gDebugHeap, itemData);
    Mem_Free(&gDebugHeap, foodData);
    Mem_Free(&gDebugHeap, treasureData);
    Mem_Free(&gDebugHeap, sale);
    Mem_Free(&gDebugHeap, shopItem);
    Mem_Free(&gDebugHeap, pinInfo);
}

void func_ov043_020b0d18(void) {
}

// Nonmatching: if-conversion vs branch on the unk_834 flag update
void func_ov043_020b0d1c(ShopObject *arg0) {
    typedef struct {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ u16 unk_02;
        /* 0x04 */ u16 unk_04;
        /* 0x06 */ s16 unk_06;
        /* 0x08 */ u32 unk_08;
        /* 0x0C */ u8  unk_0C;
        /* 0x0D */ u8  unk_0D;
        /* 0x0E */ u16 unk_0E;
        /* 0x10 */ s32 unk_10;
        /* 0x14 */ u16 unk_14;
        /* 0x16 */ u16 unk_16[3];
        /* 0x1C */ u8  unk_1C[3];
        /* 0x1F */ u8  unk_1F;
        /* 0x20 */ s32 unk_20;
        /* 0x24 */ u8  unk_24[7];
        /* 0x2B */ u8  unk_2B;
    } ShopP6Slot;
    typedef struct {
        /* 0x000 */ ShopP6Slot slots[42];
        /* 0x738 */ ShopP6Slot *unk_738[12];
        /* 0x768 */ u16         unk_768;
        /* 0x76A */ s16         unk_76A;
        /* 0x76C */ s16         unk_76C;
        /* 0x76E */ u16         unk_76E;
        /* 0x770 */ u16         unk_770;
        /* 0x772 */ u16         unk_772;
        /* 0x774 */ u16         unk_774;
        /* 0x776 */ u16         unk_776;
        /* 0x778 */ u16         unk_778;
        /* 0x77A */ u16         unk_77A;
        /* 0x77C */ u32         unk_77C;
        /* 0x780 */ s32         unk_780;
        /* 0x784 */ u32         unk_784;
        /* 0x788 */ s16         unk_788;
        /* 0x78A */ s16         unk_78A;
        /* 0x78C */ s16         unk_78C;
        /* 0x78E */ u16         unk_78E;
        /* 0x790 */ u16         unk_790[9];
        /* 0x7A2 */ u16         unk_7A2[9];
        /* 0x7B4 */ u8          unk_7B4;
        /* 0x7B5 */ u8          unk_7B5;
        /* 0x7B6 */ u8          unk_7B6;
        /* 0x7B7 */ u8          unk_7B7;
        /* 0x7B8 */ u8          unk_7B8;
        /* 0x7B9 */ u8          unk_7B9;
        /* 0x7BA */ u8          unk_7BA;
        /* 0x7BB */ u8          unk_7BB;
        /* 0x7BC */ char        pad_7BC[0x834 - 0x7BC];
        /* 0x834 */ u16         unk_834;
        /* 0x836 */ u8          unk_836;
        /* 0x837 */ u8          unk_837;
        /* 0x838 */ u8          unk_838;
        /* 0x839 */ u8          unk_839;
        /* 0x83A */ u16         unk_83A;
        /* 0x83C */ s16         unk_83C;
        /* 0x83E */ u16         unk_83E;
        /* 0x840 */ s16         unk_840;
        /* 0x842 */ s16         unk_842;
        /* 0x844 */ u16         unk_844;
        /* 0x846 */ u16         unk_846;
        /* 0x848 */ u16         unk_848;
        /* 0x84A */ u16         unk_84A;
        /* 0x84C */ u16         unk_84C;
        /* 0x84E */ u16         unk_84E;
        /* 0x850 */ u16         unk_850;
        /* 0x852 */ u16         unk_852;
        /* 0x854 */ u8          unk_854;
        /* 0x855 */ u8          unk_855;
        /* 0x856 */ u16         unk_856;
        /* 0x858 */ u32         unk_858;
        /* 0x85C */ u16         unk_85C;
        /* 0x85E */ u16         unk_85E;
        /* 0x860 */ u16         unk_860;
        /* 0x862 */ u16         unk_862;
        /* 0x864 */ u8          unk_864[3];
        /* 0x867 */ u8          unk_867;
        /* 0x868 */ u32         unk_868;
        /* 0x86C */ u32         unk_86C;
        /* 0x870 */ u16         unk_870;
        /* 0x872 */ u16         unk_872;
    } ShopP6;
    typedef struct {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ u16 unk_02;
        /* 0x04 */ u32 unk_04;
        /* 0x08 */ u16 unk_08[9];
        /* 0x1A */ u16 unk_1A[9];
    } ShopP6Menu0;
    typedef struct {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ u16 unk_02;
        /* 0x04 */ u16 unk_04;
        /* 0x06 */ u16 unk_06;
        /* 0x08 */ u16 unk_08;
        /* 0x0A */ u16 unk_0A;
    } ShopP6Menu2C;
    typedef struct {
        /* 0x00 */ ShopP6Menu0  menu0;
        /* 0x2C */ ShopP6Menu2C menu2C;
    } ShopP6Scratch;

    ShopP6 *shop = (ShopP6 *) arg0;
    ShopP6Scratch sp;
    s32          var_ip;
    s32          var_r2;
    s16          var_r1;
    ShopP6Slot  *var_r1_2;

    func_ov043_020af724(arg0);
    shop->unk_858 = gSaveData.playerStats.money;
    if (gSaveData.playerStats.unk_19_6 == 1) {
        var_r1 = shop->unk_834 | 4;
    } else {
        var_r1 = 0;
    }
    shop->unk_834 = var_r1;
    shop->unk_768 = gSaveData.unk_1AB2;
    func_ov043_020af574(&sp.menu2C, (s32) shop->unk_768);
    shop->unk_76A = (s16) (sp.menu2C.unk_02 + 0x3F);
    shop->unk_76C = (s16) (sp.menu2C.unk_04 + 0x1F);
    shop->unk_76E = sp.menu2C.unk_06;
    shop->unk_770 = sp.menu2C.unk_0A;
    shop->unk_774 = sp.menu2C.unk_08;
    func_ov043_020af5a0(&sp.menu0, (s32) sp.menu2C.unk_08);
    shop->unk_776 = sp.menu0.unk_00;
    shop->unk_77C = gSaveData.unk_1A18[shop->unk_774];
    shop->unk_784 = sp.menu0.unk_04;
    shop->unk_78E = sp.menu0.unk_02;
    if (shop->unk_77C > shop->unk_784) {
        shop->unk_77C = shop->unk_784;
    }
    for (var_ip = 0; var_ip < 9; var_ip++) {
        shop->unk_790[var_ip] = sp.menu0.unk_08[var_ip];
        shop->unk_7A2[var_ip] = sp.menu0.unk_1A[var_ip];
    }
    shop->unk_788 = func_ov043_020af8a8(arg0, shop->unk_77C, shop->unk_784);
    shop->unk_778 = func_ov043_020af8e4(arg0);
    shop->unk_77A = func_ov043_020af950(arg0);
    func_ov043_020af994(arg0);
    shop->unk_780 = func_ov043_020af994(arg0);
    shop->unk_78C = func_ov043_020af9e8(arg0);
    shop->unk_78A = func_ov043_020af8a8(arg0, shop->unk_780, shop->unk_784);
    if (func_ov043_020b1e2c(arg0) == 0) {
        shop->unk_7B6 = 0;
    } else {
        shop->unk_7B6 = 1;
    }
    shop->unk_7B7 = (s8) func_ov043_020b1ecc(arg0, shop->unk_774);
    func_ov043_020b02fc(arg0);
    shop->unk_7B8 = (s8) func_ov043_020afa68(arg0);
    shop->unk_7B4 = (s8) func_ov043_020afabc(arg0);
    shop->unk_7B5 = (s8) func_ov043_020afb14(arg0);
    var_r1_2 = shop->slots;
    var_r2 = 0;
    do {
        shop->unk_738[var_r2] = var_r1_2;
        var_r2 += 1;
        var_r1_2 = (ShopP6Slot *) ((u8 *) var_r1_2 + 0x2C);
    } while (var_r2 < 12);
    func_ov043_020b0d18();
}

// Nonmatching: register/spill allocation through the item-table build
void func_ov043_020b0f20(ShopObject *arg0) {
    typedef struct {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ u16 unk_02;
        /* 0x04 */ u16 unk_04;
        /* 0x06 */ s16 unk_06;
        /* 0x08 */ u32 unk_08;
        /* 0x0C */ u8  unk_0C;
        /* 0x0D */ u8  unk_0D;
        /* 0x0E */ u16 unk_0E;
        /* 0x10 */ s32 unk_10;
        /* 0x14 */ u16 unk_14;
        /* 0x16 */ u16 unk_16[3];
        /* 0x1C */ u8  unk_1C[3];
        /* 0x1F */ u8  unk_1F;
        /* 0x20 */ s32 unk_20;
        /* 0x24 */ u8  unk_24[7];
        /* 0x2B */ u8  unk_2B;
    } ShopP6Slot;
    typedef struct {
        /* 0x000 */ ShopP6Slot slots[42];
        /* 0x738 */ ShopP6Slot *unk_738[12];
        /* 0x768 */ u16         unk_768;
        /* 0x76A */ s16         unk_76A;
        /* 0x76C */ s16         unk_76C;
        /* 0x76E */ u16         unk_76E;
        /* 0x770 */ u16         unk_770;
        /* 0x772 */ u16         unk_772;
        /* 0x774 */ u16         unk_774;
        /* 0x776 */ u16         unk_776;
        /* 0x778 */ u16         unk_778;
        /* 0x77A */ u16         unk_77A;
        /* 0x77C */ u32         unk_77C;
        /* 0x780 */ s32         unk_780;
        /* 0x784 */ u32         unk_784;
        /* 0x788 */ s16         unk_788;
        /* 0x78A */ s16         unk_78A;
        /* 0x78C */ s16         unk_78C;
        /* 0x78E */ u16         unk_78E;
        /* 0x790 */ u16         unk_790[9];
        /* 0x7A2 */ u16         unk_7A2[9];
        /* 0x7B4 */ u8          unk_7B4;
        /* 0x7B5 */ u8          unk_7B5;
        /* 0x7B6 */ u8          unk_7B6;
        /* 0x7B7 */ u8          unk_7B7;
        /* 0x7B8 */ u8          unk_7B8;
        /* 0x7B9 */ u8          unk_7B9;
        /* 0x7BA */ u8          unk_7BA;
        /* 0x7BB */ u8          unk_7BB;
        /* 0x7BC */ char        pad_7BC[0x834 - 0x7BC];
        /* 0x834 */ u16         unk_834;
        /* 0x836 */ u8          unk_836;
        /* 0x837 */ u8          unk_837;
        /* 0x838 */ u8          unk_838;
        /* 0x839 */ u8          unk_839;
        /* 0x83A */ u16         unk_83A;
        /* 0x83C */ s16         unk_83C;
        /* 0x83E */ u16         unk_83E;
        /* 0x840 */ s16         unk_840;
        /* 0x842 */ s16         unk_842;
        /* 0x844 */ u16         unk_844;
        /* 0x846 */ u16         unk_846;
        /* 0x848 */ u16         unk_848;
        /* 0x84A */ u16         unk_84A;
        /* 0x84C */ u16         unk_84C;
        /* 0x84E */ u16         unk_84E;
        /* 0x850 */ u16         unk_850;
        /* 0x852 */ u16         unk_852;
        /* 0x854 */ u8          unk_854;
        /* 0x855 */ u8          unk_855;
        /* 0x856 */ u16         unk_856;
        /* 0x858 */ u32         unk_858;
        /* 0x85C */ u16         unk_85C;
        /* 0x85E */ u16         unk_85E;
        /* 0x860 */ u16         unk_860;
        /* 0x862 */ u16         unk_862;
        /* 0x864 */ u8          unk_864[3];
        /* 0x867 */ u8          unk_867;
        /* 0x868 */ u32         unk_868;
        /* 0x86C */ u32         unk_86C;
        /* 0x870 */ u16         unk_870;
        /* 0x872 */ u16         unk_872;
    } ShopP6;
    typedef struct {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ u16 unk_02;
        /* 0x04 */ u16 unk_04;
        /* 0x06 */ u16 unk_06;
    } ShopP6Sale;
    typedef struct {
        /* 0x00 */ u8  unk_00;
        /* 0x01 */ u8  unk_01;
        /* 0x02 */ u8  unk_02;
        /* 0x03 */ u8  unk_03;
        /* 0x04 */ u8  unk_04;
        /* 0x05 */ u8  unk_05;
        /* 0x06 */ s16 unk_06;
        /* 0x08 */ u8  pad_08[0x10];
        /* 0x18 */ u16 unk_18;
        /* 0x1A */ u8  pad_1A[2];
    } ShopP6PinInfo;

    ShopP6 *shop = (ShopP6 *) arg0;
    RawPinData      *pinData;
    RawItemData     *itemData;
    RawFoodData     *foodData;
    RawTreasureData *treasureData;
    ShopP6Sale      *sale;
    ShopP6PinInfo   *pinInfo;
    u16              var_r8;
    u16              var_r5;
    u16              var_r9;
    u16              var_ip;
    s32              catIdx;
    s32              item;
    s32              category;
    s32              cost;
    ShopP6Slot      *slot;
    ShopP6PinInfo   *info;

    pinData = Mem_AllocHeapTail(&gDebugHeap, 0x3DC0);
    Mem_SetSequence(&gDebugHeap, pinData, &data_ov043_020cc82c);
    itemData = Mem_AllocHeapTail(&gDebugHeap, 0x1A40);
    Mem_SetSequence(&gDebugHeap, itemData, &data_ov043_020cc838);
    foodData = Mem_AllocHeapTail(&gDebugHeap, 0x348);
    Mem_SetSequence(&gDebugHeap, foodData, &data_ov043_020cc844);
    treasureData = Mem_AllocHeapTail(&gDebugHeap, 0x4B0);
    Mem_SetSequence(&gDebugHeap, treasureData, &data_ov043_020cc850);
    sale = Mem_AllocHeapTail(&gDebugHeap, 0x11D0);
    Mem_SetSequence(&gDebugHeap, sale, &data_ov043_020cc860);
    pinInfo = Mem_AllocHeapTail(&gDebugHeap, 0x2140);
    Mem_SetSequence(&gDebugHeap, pinInfo, &data_ov043_020cc880);
    ((void (*)(void *)) func_ov043_020af624)(pinData);
    ((void (*)(void *)) func_ov043_020af648)(itemData);
    ((void (*)(void *)) func_ov043_020af66c)(foodData);
    ((void (*)(void *)) func_ov043_020af690)(treasureData);
    ((void (*)(void *)) func_ov043_020af6b4)(sale);
    ((void (*)(void *)) func_ov043_020af700)(pinInfo);

    var_r8 = 0;
    var_r5 = 0;
    do {
        slot = &shop->slots[var_r8];
        slot->unk_00 = 0xFFFF;
        slot->unk_02 = 0;
        slot->unk_04 = 0;
        slot->unk_06 = 0xE;
        slot->unk_08 = 0;
        slot->unk_0C = 1;
        slot->unk_10 = 0;
        for (var_ip = 0; (u32) var_ip < 7U; var_ip++) {
            slot->unk_24[var_ip] = 0;
        }
        for (var_ip = 0; (u32) var_ip < 3U; var_ip++) {
            slot->unk_16[var_ip] = 0xFFFF;
            slot->unk_1C[var_ip] = 0;
        }
        var_r8 += 1;
    } while ((u32) var_r8 < 0x2AU);

    var_r9 = 0;
    do {
        item = *(u16 *) ((u8 *) &gSaveData + 0x1AF6 + var_r9 * 2);
        catIdx = Inventory_GetCategorizedIndex((u16) item);
        if (item != 0xFFFF) {
            cost = pinData[catIdx].price;
            if ((cost != 0) && (cost != 0x989680)) {
                info = &pinInfo[catIdx];
                slot = &shop->slots[var_r5];
                slot->unk_00 = (u16) item;
                slot->unk_02 = pinData[catIdx].iconIndex;
                slot->unk_04 = 0;
                slot->unk_06 = (s16) pinData[catIdx].brand;
                slot->unk_08 = pinData[catIdx].price;
                slot->unk_20 = 0;
                slot->unk_24[0] = (s8) (info->unk_04 + 1);
                slot->unk_24[1] = (s8) ((s32) info->unk_06 / 409);
                slot->unk_24[2] = (s8) ((s32) info->unk_18 / 60);
                slot->unk_24[3] = info->unk_00;
                slot->unk_24[4] = info->unk_01;
                slot->unk_24[5] = info->unk_02;
                slot->unk_24[6] = info->unk_03;
                var_r5 += 1;
            }
        }
        var_r9 += 1;
    } while ((u32) var_r9 < 6U);

    var_r8 = 0;
    do {
        item = *(u16 *) ((u8 *) &gSaveData + 0x1B02 + var_r8 * 2);
        catIdx = Inventory_GetCategorizedIndex((u16) item);
        category = Inventory_GetCategory((u16) item);
        if (item != 0xFFFF) {
            cost = itemData[catIdx].unk_04;
            if ((cost != 0) && (cost != 0x989680)) {
                switch (category) {
                case 1:
                    slot = &shop->slots[var_r5];
                    slot->unk_00 = (u16) item;
                    slot->unk_02 = itemData[catIdx].unk_00;
                    slot->unk_04 = 1;
                    slot->unk_06 = (s16) itemData[catIdx].brand;
                    slot->unk_08 = itemData[catIdx].unk_04;
                    if (((u32) (gSaveData.unk_1EB2[catIdx] << 0x1F) >> 0x1F) == 1) {
                        slot->unk_20 = 1;
                    } else {
                        slot->unk_20 = 0;
                    }
                    break;
                case 2:
                    slot = &shop->slots[var_r5];
                    slot->unk_00 = (u16) item;
                    slot->unk_02 = *(u16 *) (foodData + catIdx);
                    slot->unk_04 = 2;
                    slot->unk_06 = 0xE;
                    slot->unk_08 = foodData[catIdx].unk_04;
                    slot->unk_20 = 0;
                    break;
                case 3:
                    slot = &shop->slots[var_r5];
                    slot->unk_00 = (u16) item;
                    slot->unk_02 = *(u16 *) (treasureData + catIdx);
                    slot->unk_04 = 3;
                    slot->unk_06 = 0xE;
                    slot->unk_08 = treasureData[catIdx].unk_04;
                    slot->unk_20 = 0;
                    break;
                default:
                    OS_WaitForever();
                    break;
                }
                var_r5 += 1;
            }
        }
        var_r8 += 1;
    } while ((u32) var_r8 < 0x10U);

    shop->unk_860 = var_r5;
    func_ov043_020b02ac(shop->unk_860);
    shop->unk_862 = (u16) func_ov043_020b02ac(shop->unk_860);
    shop->unk_870 = (u16) ((shop->unk_862 / 6) - 2);
    shop->unk_872 = shop->unk_870;
    Mem_Free(&gDebugHeap, pinData);
    Mem_Free(&gDebugHeap, itemData);
    Mem_Free(&gDebugHeap, foodData);
    Mem_Free(&gDebugHeap, treasureData);
    Mem_Free(&gDebugHeap, sale);
    Mem_Free(&gDebugHeap, pinInfo);
}

// Nonmatching: if-conversion vs branch on the unk_834 flag update
void func_ov043_020b1404(ShopObject *arg0) {
    typedef struct {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ u16 unk_02;
        /* 0x04 */ u16 unk_04;
        /* 0x06 */ s16 unk_06;
        /* 0x08 */ u32 unk_08;
        /* 0x0C */ u8  unk_0C;
        /* 0x0D */ u8  unk_0D;
        /* 0x0E */ u16 unk_0E;
        /* 0x10 */ s32 unk_10;
        /* 0x14 */ u16 unk_14;
        /* 0x16 */ u16 unk_16[3];
        /* 0x1C */ u8  unk_1C[3];
        /* 0x1F */ u8  unk_1F;
        /* 0x20 */ s32 unk_20;
        /* 0x24 */ u8  unk_24[7];
        /* 0x2B */ u8  unk_2B;
    } ShopP6Slot;
    typedef struct {
        /* 0x000 */ ShopP6Slot slots[42];
        /* 0x738 */ ShopP6Slot *unk_738[12];
        /* 0x768 */ u16         unk_768;
        /* 0x76A */ s16         unk_76A;
        /* 0x76C */ s16         unk_76C;
        /* 0x76E */ u16         unk_76E;
        /* 0x770 */ u16         unk_770;
        /* 0x772 */ u16         unk_772;
        /* 0x774 */ u16         unk_774;
        /* 0x776 */ u16         unk_776;
        /* 0x778 */ u16         unk_778;
        /* 0x77A */ u16         unk_77A;
        /* 0x77C */ u32         unk_77C;
        /* 0x780 */ s32         unk_780;
        /* 0x784 */ u32         unk_784;
        /* 0x788 */ s16         unk_788;
        /* 0x78A */ s16         unk_78A;
        /* 0x78C */ s16         unk_78C;
        /* 0x78E */ u16         unk_78E;
        /* 0x790 */ u16         unk_790[9];
        /* 0x7A2 */ u16         unk_7A2[9];
        /* 0x7B4 */ u8          unk_7B4;
        /* 0x7B5 */ u8          unk_7B5;
        /* 0x7B6 */ u8          unk_7B6;
        /* 0x7B7 */ u8          unk_7B7;
        /* 0x7B8 */ u8          unk_7B8;
        /* 0x7B9 */ u8          unk_7B9;
        /* 0x7BA */ u8          unk_7BA;
        /* 0x7BB */ u8          unk_7BB;
        /* 0x7BC */ char        pad_7BC[0x834 - 0x7BC];
        /* 0x834 */ u16         unk_834;
        /* 0x836 */ u8          unk_836;
        /* 0x837 */ u8          unk_837;
        /* 0x838 */ u8          unk_838;
        /* 0x839 */ u8          unk_839;
        /* 0x83A */ u16         unk_83A;
        /* 0x83C */ s16         unk_83C;
        /* 0x83E */ u16         unk_83E;
        /* 0x840 */ s16         unk_840;
        /* 0x842 */ s16         unk_842;
        /* 0x844 */ u16         unk_844;
        /* 0x846 */ u16         unk_846;
        /* 0x848 */ u16         unk_848;
        /* 0x84A */ u16         unk_84A;
        /* 0x84C */ u16         unk_84C;
        /* 0x84E */ u16         unk_84E;
        /* 0x850 */ u16         unk_850;
        /* 0x852 */ u16         unk_852;
        /* 0x854 */ u8          unk_854;
        /* 0x855 */ u8          unk_855;
        /* 0x856 */ u16         unk_856;
        /* 0x858 */ u32         unk_858;
        /* 0x85C */ u16         unk_85C;
        /* 0x85E */ u16         unk_85E;
        /* 0x860 */ u16         unk_860;
        /* 0x862 */ u16         unk_862;
        /* 0x864 */ u8          unk_864[3];
        /* 0x867 */ u8          unk_867;
        /* 0x868 */ u32         unk_868;
        /* 0x86C */ u32         unk_86C;
        /* 0x870 */ u16         unk_870;
        /* 0x872 */ u16         unk_872;
    } ShopP6;
    typedef struct {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ u16 unk_02;
        /* 0x04 */ u32 unk_04;
        /* 0x08 */ u16 unk_08[9];
        /* 0x1A */ u16 unk_1A[9];
    } ShopP6Menu0;
    typedef struct {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ u16 unk_02;
        /* 0x04 */ u16 unk_04;
        /* 0x06 */ u16 unk_06;
        /* 0x08 */ u16 unk_08;
        /* 0x0A */ u16 unk_0A;
    } ShopP6Menu2C;
    typedef struct {
        /* 0x00 */ ShopP6Menu0  menu0;
        /* 0x2C */ ShopP6Menu2C menu2C;
    } ShopP6Scratch;

    ShopP6 *shop = (ShopP6 *) arg0;
    ShopP6Scratch sp;
    s32          var_ip;
    s32          var_r2;
    s16          var_r1;
    ShopP6Slot  *var_r1_2;

    shop->unk_858 = gSaveData.playerStats.money;
    if (gSaveData.playerStats.unk_19_6 == 1) {
        var_r1 = shop->unk_834 | 4;
    } else {
        var_r1 = 0;
    }
    shop->unk_834 = var_r1;
    shop->unk_768 = (u16) (((u32) gSaveData.unk_1AF2.shopId));
    func_ov043_020af574(&sp.menu2C, (s32) shop->unk_768);
    shop->unk_76A = (s16) (sp.menu2C.unk_02 + 0x3F);
    shop->unk_76C = (s16) (gSaveData.unk_1AF2.musicId + 0x1F);
    shop->unk_76E = gSaveData.unk_1AF2.giftItemId;
    shop->unk_770 = sp.menu2C.unk_0A;
    shop->unk_774 = (u16) gSaveData.unk_1AF2.clerkId;
    func_ov043_020af5a0(&sp.menu0, (s32) shop->unk_774);
    shop->unk_776 = sp.menu0.unk_00;
    shop->unk_77C = sp.menu0.unk_04;
    shop->unk_784 = sp.menu0.unk_04;
    shop->unk_78E = sp.menu0.unk_02;
    for (var_ip = 0; var_ip < 9; var_ip++) {
        shop->unk_790[var_ip] = sp.menu0.unk_08[var_ip];
        shop->unk_7A2[var_ip] = sp.menu0.unk_1A[var_ip];
    }
    shop->unk_788 = func_ov043_020af8a8(arg0, shop->unk_77C, shop->unk_784);
    shop->unk_778 = func_ov043_020af8e4(arg0);
    shop->unk_77A = func_ov043_020af950(arg0);
    func_ov043_020af994(arg0);
    shop->unk_780 = func_ov043_020af994(arg0);
    shop->unk_78C = func_ov043_020af9e8(arg0);
    shop->unk_78A = func_ov043_020af8a8(arg0, shop->unk_780, shop->unk_784);
    shop->unk_7B4 = 0;
    shop->unk_7B5 = 0;
    shop->unk_7B6 = 0;
    shop->unk_7B7 = (s8) func_ov043_020b1ecc(arg0, shop->unk_774);
    func_ov043_020b0f20(arg0);
    var_r1_2 = shop->slots;
    var_r2 = 0;
    do {
        shop->unk_738[var_r2] = var_r1_2;
        var_r2 += 1;
        var_r1_2 = (ShopP6Slot *) ((u8 *) var_r1_2 + 0x2C);
    } while (var_r2 < 12);
}

void func_ov043_020b15d0(ShopObject *arg0) {
    typedef struct {
        /* 0x000 */ char pad_000[0x774];
        /* 0x774 */ u16  unk_774;
        /* 0x776 */ char pad_776[0x77C - 0x776];
        /* 0x77C */ u32  unk_77C;
        /* 0x780 */ char pad_780[0x834 - 0x780];
        /* 0x834 */ u16  unk_834;
        /* 0x836 */ char pad_836[0x858 - 0x836];
        /* 0x858 */ u32  unk_858;
    } ShopP6;

    ShopP6 *shop = (ShopP6 *) arg0;

    gSaveData.playerStats.money = shop->unk_858;
    gSaveData.unk_1A18[shop->unk_774] = shop->unk_77C;
    if (shop->unk_834 & 4) {
        ((u8 *) &gSaveData)[0x19] |= 0x40;
    } else {
        ((u8 *) &gSaveData)[0x19] &= ~0x40;
    }
}

// Nonmatching: pure register permutation
void func_ov043_020b1618(ShopObject *arg0) {
    typedef struct {
        /* 0x000 */ char pad_000[0x834];
        /* 0x834 */ u16  unk_834;
        /* 0x836 */ char pad_836[0x858 - 0x836];
        /* 0x858 */ u32  unk_858;
        /* 0x85C */ char pad_85C[0x868 - 0x85C];
        /* 0x868 */ u32  unk_868;
    } ShopP6;

    ShopP6 *shop = (ShopP6 *) arg0;
    u8 idx;
    u32 *p;
    u32 temp_r4;
    u32 temp_r2;
    u32 var_r3;
    u32 var_r2;

    idx = gSaveData.unk_1AB9;
    gSaveData.playerStats.money = shop->unk_858;
    temp_r4 = shop->unk_868;
    if (temp_r4 != 0) {
        p = &gSaveData.mingleFriends[idx].unk_8;
        temp_r2 = *p;
        var_r3 = (temp_r2 & ~0xFF000000) + temp_r4;
        if (var_r3 > 0x98967FU) {
            var_r3 = 0x98967F;
        }
        var_r2 = ((u32) (temp_r2 & 0xFF000000) >> 0x18) + 1;
        if (var_r2 > 0xFFU) {
            var_r2 = 0;
        }
        *p = (var_r3 & ~0xFF000000) + (var_r2 << 0x18);
    }
    if (shop->unk_834 & 4) {
        ((u8 *) &gSaveData)[0x19] |= 0x40;
        return;
    }
    ((u8 *) &gSaveData)[0x19] &= ~0x40;
}
// ---- Shop_p7.c ----
// NOTE: mirrors for the data_ov043_020ca260 layout table are declared locally in
// each function that reads it (the header only exposes it as a bare `int`).

s32  func_02023d88(u32 arg0);

s32 func_ov043_020b16c4(ShopObject *arg0, s32 arg1) {
    ShopItemSlot *slot = (ShopItemSlot *) (arg0->unk_738[arg1]);
    u16           itemID;
    s32           unk_10;

    itemID = slot->itemID;
    unk_10 = slot->unk_10;
    if (slot->unk_04 == 0) {
        if (Inventory_AddItem(itemID, 1) == 1) {
            if (unk_10 == 1) {
                func_ov043_020b01d8(arg0, arg1);
                arg0->unk_834 = arg0->unk_834 | 0x40;
            }
            return 1;
        }
    } else if ((Inventory_GetOwnedCount(itemID) < 9U) && (Inventory_AddItem(itemID, 0) == 1)) {
        if (unk_10 == 1) {
            func_ov043_020b01d8(arg0, arg1);
            arg0->unk_834 = arg0->unk_834 | 0x40;
        }
        return 1;
    }
    return 0;
}

s32 func_ov043_020b1780(ShopObject *arg0, s32 arg1) {
    ShopItemSlot *slot = (ShopItemSlot *) (arg0->unk_738[arg1]);
    u16           itemID;

    itemID = slot->itemID;
    if (slot->unk_04 == 0) {
        if (Inventory_CanAddPin(itemID, 1) == 1) {
            return 1;
        }
    } else if (Inventory_CanAddPin(itemID, 0) == 1) {
        return 1;
    }
    return 0;
}

// Nonmatching: the four loaded values are scheduled w,x,y,h (target) vs w,x,h,y,
// with a register rotation (base/last load lands in r2 in the target).
s32 func_ov043_020b17d0(s16 arg0, s16 arg1) {
    typedef struct {
        /* 0x00 */ char pad_00[0x08];
        /* 0x08 */ u16  unk_8;
        /* 0x0A */ u16  unk_A;
        /* 0x0C */ char pad_0C[0x10 - 0x0C];
        /* 0x10 */ u16  unk_10;
        /* 0x12 */ u16  unk_12;
    } ShopP7_Layout;
    volatile s16   spE;
    volatile s16   spC;
    volatile s16   spA;
    volatile s16   sp8;
    s16            a;
    s16            b;
    s16            c;
    s16            d;

    sp8 = ((ShopP7_Layout *) &data_ov043_020ca260)->unk_10;
    a = sp8;
    spC = ((ShopP7_Layout *) &data_ov043_020ca260)->unk_8;
    spA = ((ShopP7_Layout *) &data_ov043_020ca260)->unk_12;
    spE = ((ShopP7_Layout *) &data_ov043_020ca260)->unk_A;
    b = spA;
    c = spC;
    d = spE;
    return func_ov043_020af3f0(arg0, arg1, c, d, a, b) == 1;
}

// Nonmatching: array-fill load/store pairing and register allocation differ.
s32 func_ov043_020b1830(s16 arg0, s16 arg1) {
    typedef struct {
        /* 0x00 */ char pad_00[0x04];
        /* 0x04 */ u16  unk_4;
        /* 0x06 */ u16  unk_6;
        /* 0x08 */ char pad_08[0x3C - 0x08];
        /* 0x3C */ u16  unk_3C;
        /* 0x3E */ u16  unk_3E;
        /* 0x40 */ u16  unk_40;
        /* 0x42 */ u16  unk_42;
        /* 0x44 */ u16  unk_44;
    } ShopP7_Layout;
    ShopP7_Layout *t = (ShopP7_Layout *) &data_ov043_020ca260;
    u16            sp8;
    u16            spA;
    s16            spC[5];
    s16            i;

    spA = t->unk_6;
    sp8 = t->unk_4;
    spC[4] = t->unk_44;
    spC[3] = t->unk_42;
    spC[0] = t->unk_3C;
    spC[1] = t->unk_3E;
    spC[2] = t->unk_40;
    for (i = 0; i < 5; i++) {
        if (func_ov043_020af3f0(arg0, arg1, spC[i], 0xB0, (s16) sp8, (s16) spA) == 1) {
            return i;
        }
    }
    return -1;
}

// Nonmatching: see func_ov043_020b17d0 (load scheduling + register rotation).
s32 func_ov043_020b18e4(s16 arg0, s16 arg1) {
    typedef struct {
        /* 0x00 */ char pad_00[0x24];
        /* 0x24 */ u16  unk_24;
        /* 0x26 */ u16  unk_26;
        /* 0x28 */ u16  unk_28;
        /* 0x2A */ u16  unk_2A;
    } ShopP7_Layout;
    volatile s16   spE;
    volatile s16   spC;
    volatile s16   spA;
    volatile s16   sp8;
    s16            a;
    s16            b;
    s16            c;
    s16            d;

    sp8 = ((ShopP7_Layout *) &data_ov043_020ca260)->unk_24;
    a = sp8;
    spC = ((ShopP7_Layout *) &data_ov043_020ca260)->unk_28;
    spA = ((ShopP7_Layout *) &data_ov043_020ca260)->unk_26;
    spE = ((ShopP7_Layout *) &data_ov043_020ca260)->unk_2A;
    b = spA;
    c = spC;
    d = spE;
    return func_ov043_020af3f0(arg0, arg1, c, d, a, b) == 1;
}

// Nonmatching: loop-counter/point registers rotated (i/h/w land in r8/r6/r7
// instead of the target's r6/r7/r8); otherwise identical.
s32 func_ov043_020b1944(s16 arg0, s16 arg1) {
    typedef struct {
        /* 0x00 */ char pad_00[0x20];
        /* 0x20 */ u16  unk_20;
        /* 0x22 */ u16  unk_22;
    } ShopP7_Layout;
    volatile u16   spA;
    volatile u16   sp8;
    s16            h;
    s16            w;
    s16            i;
    s16            x;
    s16            y;
    s16            w2;

    sp8 = ((ShopP7_Layout *) &data_ov043_020ca260)->unk_20;
    spA = ((ShopP7_Layout *) &data_ov043_020ca260)->unk_22;
    w = sp8;
    h = spA;
    for (i = 0; i < 12; i++) {
        x = (i % 6) * 33 + 6;
        y = (i / 6) * 41 + 23;
        w2 = (s16) w - ((x == 0xAB) ? 2 : 0);
        if (func_ov043_020af3f0(arg0, arg1, x, y, w2, (s16) h) == 1) {
            return i;
        }
    }
    return -1;
}

s32 func_ov043_020b1a1c(ShopObject *arg0, s32 arg1) {
    u16 i;
    u8  count;
    u16 itemID;

    for (i = 0; i < 3; i++) {
        count = ((ShopItemSlot *) (arg0->unk_738[arg1]))->tradeInCounts[i];
        itemID = ((ShopItemSlot *) (arg0->unk_738[arg1]))->tradeInItemIDs[i];
        if (count != 0) {
            if ((s32) func_ov043_020b0088(itemID) < (s32) count) {
                return 0;
            }
        }
    }
    return 1;
}

s32 func_ov043_020b1a70(ShopObject *arg0, s16 arg1) {
    ShopItemSlot *slot;

    if (arg1 == -1) {
        return 0;
    }
    slot = (ShopItemSlot *) (arg0->unk_738[arg1]);
    if (slot->unk_0C[0] == 1) {
        if (slot->unk_10 == 0) {
            if (slot->cost <= arg0->unk_858) {
                return 1;
            }
        } else {
            if (func_ov043_020b1a1c(arg0, arg1) == 1) {
                return 1;
            }
        }
    } else if ((slot->unk_10 == 1) && (func_02023d88(slot->unk_14) == 0)) {
        if (func_ov043_020b1a1c(arg0, arg1) == 1) {
            return 1;
        }
    }
    return 0;
}

// Nonmatching: see func_ov043_020b17d0 (load scheduling + register rotation).
s32 func_ov043_020b1b14(s16 arg0, s16 arg1) {
    typedef struct {
        /* 0x00 */ char pad_00[0x18];
        /* 0x18 */ u16  unk_18;
        /* 0x1A */ u16  unk_1A;
        /* 0x1C */ u16  unk_1C;
        /* 0x1E */ u16  unk_1E;
    } ShopP7_Layout;
    volatile s16   spE;
    volatile s16   spC;
    volatile s16   spA;
    volatile s16   sp8;
    s16            a;
    s16            b;
    s16            c;
    s16            d;

    sp8 = ((ShopP7_Layout *) &data_ov043_020ca260)->unk_18;
    a = sp8;
    spC = ((ShopP7_Layout *) &data_ov043_020ca260)->unk_1C;
    spA = ((ShopP7_Layout *) &data_ov043_020ca260)->unk_1A;
    spE = ((ShopP7_Layout *) &data_ov043_020ca260)->unk_1E;
    b = spA;
    c = spC;
    d = spE;
    return func_ov043_020af3f0(arg0, arg1, c, d, a, b) == 1;
}

s32 func_ov043_020b1b74(s16 arg0, s16 arg1, s16 arg2, s16 arg3) {
    typedef struct {
        /* 0x00 */ u16 unk_0;
        /* 0x02 */ u16 unk_2;
    } ShopP7_Layout;
    ShopP7_Layout *t = (ShopP7_Layout *) &data_ov043_020ca260;
    volatile u16   spA;
    volatile u16   sp8;
    s16            a;
    s16            b;

    sp8 = t->unk_0;
    a = sp8;
    spA = t->unk_2;
    b = spA;
    return func_ov043_020af3f0(arg0, arg1, (s16) (arg2 - 8), (s16) (arg3 - 21), a, b) == 1;
}

// Nonmatching: array-fill load pairing and register allocation differ.
s32 func_ov043_020b1bd4(s16 arg0, s16 arg1) {
    typedef struct {
        /* 0x00 */ char pad_00[0x0C];
        /* 0x0C */ u16  unk_C;
        /* 0x0E */ u16  unk_E;
        /* 0x10 */ char pad_10[0x2C - 0x10];
        /* 0x2C */ u16  unk_2C;
        /* 0x2E */ u16  unk_2E;
        /* 0x30 */ u16  unk_30;
        /* 0x32 */ u16  unk_32;
    } ShopP7_Layout;
    ShopP7_Layout *t = (ShopP7_Layout *) &data_ov043_020ca260;
    volatile u16   spA;
    volatile u16   sp8;
    s16            w;
    s16            h;
    s16            spC[2][2];
    s16            i;

    spC[0][0] = t->unk_2C;
    spC[0][1] = t->unk_2E;
    spC[1][0] = t->unk_30;
    spC[1][1] = t->unk_32;
    spA = t->unk_E;
    sp8 = t->unk_C;
    w = sp8;
    h = spA;
    for (i = 0; i < 2; i++) {
        if (func_ov043_020af3f0(arg0, arg1, spC[i][0], spC[i][1], (s16) w, (s16) h) == 1) {
            return i;
        }
    }
    return -1;
}

// Nonmatching: array-fill load pairing and register allocation differ.
s32 func_ov043_020b1c7c(s16 arg0, s16 arg1) {
    typedef struct {
        /* 0x00 */ char pad_00[0x14];
        /* 0x14 */ u16  unk_14;
        /* 0x16 */ u16  unk_16;
        /* 0x18 */ char pad_18[0x34 - 0x18];
        /* 0x34 */ u16  unk_34;
        /* 0x36 */ u16  unk_36;
        /* 0x38 */ u16  unk_38;
        /* 0x3A */ u16  unk_3A;
    } ShopP7_Layout;
    ShopP7_Layout *t = (ShopP7_Layout *) &data_ov043_020ca260;
    volatile u16   spA;
    volatile u16   sp8;
    s16            w;
    s16            h;
    s16            spC[2][2];
    s16            i;

    spC[0][0] = t->unk_34;
    spC[0][1] = t->unk_36;
    spC[1][0] = t->unk_38;
    spC[1][1] = t->unk_3A;
    spA = t->unk_16;
    sp8 = t->unk_14;
    h = spA;
    w = sp8;
    for (i = 0; i < 2; i++) {
        if (func_ov043_020af3f0(arg0, arg1, spC[i][0], spC[i][1], (s16) w, (s16) h) == 1) {
            return i;
        }
    }
    return -1;
}

s32 func_ov043_020b1d24(u8 arg0) {
    switch (arg0) {
    case 1:
    case 2:
    case 3:
    case 4:
        return 0;
    case 6:
    case 7:
    case 8:
    case 9:
        return 5;
    case 14:
    case 15:
    case 16:
    case 17:
        return 13;
    case 19:
    case 20:
    case 21:
    case 22:
        return 18;
    case 26:
    case 27:
    case 28:
    case 29:
        return 25;
    case 31:
    case 32:
    case 33:
    case 34:
        return 30;
    case 43:
    case 44:
    case 45:
    case 46:
        return 42;
    default:
        return -1;
    }
}

s32 func_ov043_020b1e2c(ShopObject *arg0) {
    switch (arg0->unk_776) {
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 22:
    case 29:
        return 0;
    default:
        return arg0->unk_788 >= 0x3E8U;
    }
}

s32 func_ov043_020b1ecc(void *arg0, u32 arg1) {
    switch (arg1) {
    case 18:
    case 28:
        return 0;
    case 2:
    case 8:
    case 9:
    case 10:
    case 11:
    case 22:
    case 23:
    case 30:
    case 31:
    case 34:
        return 1;
    case 15:
    case 16:
    case 17:
    case 26:
        return 2;
    case 29:
        return 3;
    case 20:
        return 4;
    case 5:
    case 12:
    case 19:
    case 27:
        return 5;
    case 3:
    case 4:
    case 6:
    case 21:
    case 32:
        return 6;
    case 0:
    case 1:
    case 13:
    case 14:
        return 7;
    case 7:
    case 33:
        return 8;
    case 25:
        return 9;
    case 24:
        return 10;
    default:
        return 11;
    }
}
// ---- Shop_p8.c ----
void func_ov043_020b1fc4(void *arg0, u8 arg1, s32 arg2) {
    typedef struct {
        /* 0x00 */ s32 v[4];
    } ShopSeRow; // Size: 0x10

    typedef struct {
        /* 0x00 */ ShopSeRow rows[10];
    } ShopSeTable; // Size: 0xA0

    typedef struct {
        /* 0x00 */ s32 v[6];
    } ShopSeList; // Size: 0x18

    ShopSeTable table = *(ShopSeTable *)&data_ov043_020ca2c0;
    ShopSeList  list  = *(ShopSeList *)&data_ov043_020ca2a8;
    u16         idx;

    if (arg1 == 0xB) {
        return;
    }
    if (arg1 == 0xA) {
        if ((u32)arg2 > 5U) {
            OS_WaitForever();
        }
        if (arg2 == 0) {
            idx = RNG_Next(0xFFFF) % 3;
        } else if (arg2 == 2) {
            idx = (u16)((RNG_Next(0xFFFF) % 2) + 4);
        } else {
            idx = (u16)(arg2 + 2);
        }
        SndMgr_StartPlayingSE((SndMgrSeIdx)list.v[idx]);
        return;
    }
    if ((u32)arg2 > 3U) {
        OS_WaitForever();
    }
    if (arg2 == 2) {
        arg2 = (u16)(arg2 + RNG_Next(0xFFFF) % 2);
    }
    SndMgr_StartPlayingSE((SndMgrSeIdx)table.rows[arg1].v[arg2]);
}

void func_ov043_020b2100(MenuBgResource *res, s32 engine, s32 layer, s32 binIndex, s32 start, u32 count) {
    res->data        = DatMgr_LoadRawData(1, NULL, 0, (BinIdentifier *)((u8 *)&data_ov043_020ca198 + binIndex * 8));
    res->charData    = Data_GetPackEntryData(res->data, 1);
    res->screenMap   = Data_GetPackEntryData(res->data, 2);
    res->paletteData = Data_GetPackEntryData(res->data, 3);

    if (engine == 0) {
        u32 charSize = (*(u32 *)res->charData & ~0xFF) >> 8;
        if ((*(u8 *)res->charData & 0xF0) == 0) {
            charSize -= 4;
        }
        res->charResource = BgResMgr_AllocChar32(g_BgResourceManagers[0], res->charData,
                                                 g_DisplaySettings.engineState[0].bgSettings[layer].charBase, 0, charSize);
        res->screenResource =
            BgResMgr_AllocScreen(g_BgResourceManagers[0], res->screenMap,
                                 g_DisplaySettings.engineState[0].bgSettings[layer].screenBase,
                                 g_DisplaySettings.engineState[0].bgSettings[layer].screenSizeText);
        res->paletteResource = PaletteMgr_AllocPaletteNoProto(g_PaletteManagers[0], res->paletteData, 0, start, count);
        PaletteMgr_Flush(g_PaletteManagers[0], res->paletteResource);
    } else {
        u32 charSize = (*(u32 *)res->charData & ~0xFF) >> 8;
        if ((*(u8 *)res->charData & 0xF0) == 0) {
            charSize -= 4;
        }
        res->charResource = BgResMgr_AllocChar32(g_BgResourceManagers[1], res->charData,
                                                 g_DisplaySettings.engineState[1].bgSettings[layer].charBase, 0, charSize);
        res->screenResource =
            BgResMgr_AllocScreen(g_BgResourceManagers[1], res->screenMap,
                                 g_DisplaySettings.engineState[1].bgSettings[layer].screenBase,
                                 g_DisplaySettings.engineState[1].bgSettings[layer].screenSizeText);
        res->paletteResource = PaletteMgr_AllocPaletteNoProto(g_PaletteManagers[1], res->paletteData, 0, start, count);
        PaletteMgr_Flush(g_PaletteManagers[1], res->paletteResource);
    }
}

void func_ov043_020b2300(MenuBgResource *res, s32 engine, s32 layer, s32 binIndex, s32 screenIndex, s32 start, u32 count) {
    s32  index;
    u16 *screenMap;

    res->data     = DatMgr_LoadRawData(1, NULL, 0, (BinIdentifier *)((u8 *)&data_ov043_020ca198 + binIndex * 8));
    res->charData = Data_GetPackEntryData(res->data, 1);

    if (res->data == NULL || (index = screenIndex + 2) <= 0) {
        screenMap = NULL;
    } else {
        u8 *base  = (u8 *)res->data->buffer + 0x20;
        screenMap = (u16 *)(base + *(s32 *)(base + index * 8));
    }
    res->screenMap = screenMap;

    res->paletteData = Data_GetPackEntryData(res->data, 3);

    if (engine == 0) {
        u32 charSize = (*(u32 *)res->charData & ~0xFF) >> 8;
        if ((*(u8 *)res->charData & 0xF0) == 0) {
            charSize -= 4;
        }
        res->charResource = BgResMgr_AllocChar32(g_BgResourceManagers[0], res->charData,
                                                 g_DisplaySettings.engineState[0].bgSettings[layer].charBase, 0, charSize);
        res->screenResource =
            BgResMgr_AllocScreen(g_BgResourceManagers[0], res->screenMap,
                                 g_DisplaySettings.engineState[0].bgSettings[layer].screenBase,
                                 g_DisplaySettings.engineState[0].bgSettings[layer].screenSizeText);
        res->paletteResource = PaletteMgr_AllocPaletteNoProto(g_PaletteManagers[0], res->paletteData, 0, start, count);
        PaletteMgr_Flush(g_PaletteManagers[0], res->paletteResource);
    } else {
        u32 charSize = (*(u32 *)res->charData & ~0xFF) >> 8;
        if ((*(u8 *)res->charData & 0xF0) == 0) {
            charSize -= 4;
        }
        res->charResource = BgResMgr_AllocChar32(g_BgResourceManagers[1], res->charData,
                                                 g_DisplaySettings.engineState[1].bgSettings[layer].charBase, 0, charSize);
        res->screenResource =
            BgResMgr_AllocScreen(g_BgResourceManagers[1], res->screenMap,
                                 g_DisplaySettings.engineState[1].bgSettings[layer].screenBase,
                                 g_DisplaySettings.engineState[1].bgSettings[layer].screenSizeText);
        res->paletteResource = PaletteMgr_AllocPaletteNoProto(g_PaletteManagers[1], res->paletteData, 0, start, count);
        PaletteMgr_Flush(g_PaletteManagers[1], res->paletteResource);
    }
}

void func_ov043_020b2514(MenuBgResource *res, Data *data, s32 engine, s32 layer, s32 screenIndex) {
    u16 *screenMap;

    if (data == NULL || screenIndex <= 0) {
        screenMap = NULL;
    } else {
        u8 *base   = (u8 *)data->buffer + 0x20;
        screenMap = (u16 *)(base + *(s32 *)(base + screenIndex * 8));
    }
    res->screenMap = screenMap;

    if (engine == 0) {
        res->screenResource =
            BgResMgr_AllocScreen(g_BgResourceManagers[0], res->screenMap,
                                 g_DisplaySettings.engineState[0].bgSettings[layer].screenBase,
                                 g_DisplaySettings.engineState[0].bgSettings[layer].screenSizeText);
        return;
    }
    res->screenResource = BgResMgr_AllocScreen(g_BgResourceManagers[1], res->screenMap,
                                               g_DisplaySettings.engineState[1].bgSettings[layer].screenBase,
                                               g_DisplaySettings.engineState[1].bgSettings[layer].screenSizeText);
}

void func_ov043_020b25c0(MenuBgResource *res, s32 engine, s32 layer, s32 binIndex, u16 packIndex, s32 start, u32 count) {
    res->data        = DatMgr_LoadPackEntry(1, NULL, 0, (BinIdentifier *)((u8 *)&data_ov043_020ca198 + binIndex * 8), packIndex, FALSE);
    res->charData    = Data_GetPackEntryData(res->data, 1);
    res->screenMap   = Data_GetPackEntryData(res->data, 2);
    res->paletteData = Data_GetPackEntryData(res->data, 3);

    if (engine == 0) {
        u32 charSize = (*(u32 *)res->charData & ~0xFF) >> 8;
        if ((*(u8 *)res->charData & 0xF0) == 0) {
            charSize -= 4;
        }
        res->charResource = BgResMgr_AllocChar32(g_BgResourceManagers[0], res->charData,
                                                 g_DisplaySettings.engineState[0].bgSettings[layer].charBase, 0, charSize);
        res->screenResource =
            BgResMgr_AllocScreen(g_BgResourceManagers[0], res->screenMap,
                                 g_DisplaySettings.engineState[0].bgSettings[layer].screenBase,
                                 g_DisplaySettings.engineState[0].bgSettings[layer].screenSizeText);
        res->paletteResource = PaletteMgr_AllocPaletteNoProto(g_PaletteManagers[0], res->paletteData, 0, start, count);
        PaletteMgr_Flush(g_PaletteManagers[0], res->paletteResource);
    } else {
        u32 charSize = (*(u32 *)res->charData & ~0xFF) >> 8;
        if ((*(u8 *)res->charData & 0xF0) == 0) {
            charSize -= 4;
        }
        res->charResource = BgResMgr_AllocChar32(g_BgResourceManagers[1], res->charData,
                                                 g_DisplaySettings.engineState[1].bgSettings[layer].charBase, 0, charSize);
        res->screenResource =
            BgResMgr_AllocScreen(g_BgResourceManagers[1], res->screenMap,
                                 g_DisplaySettings.engineState[1].bgSettings[layer].screenBase,
                                 g_DisplaySettings.engineState[1].bgSettings[layer].screenSizeText);
        res->paletteResource = PaletteMgr_AllocPaletteNoProto(g_PaletteManagers[1], res->paletteData, 0, start, count);
        PaletteMgr_Flush(g_PaletteManagers[1], res->paletteResource);
    }
}

void func_ov043_020b27cc(MenuBgResource *res, s32 engine) {
    if (engine == 0) {
        BgResMgr_ReleaseChar(g_BgResourceManagers[0], res->charResource);
        BgResMgr_ReleaseScreen(g_BgResourceManagers[0], res->screenResource);
        PaletteMgr_ReleaseResource(g_PaletteManagers[0], res->paletteResource);
    } else {
        BgResMgr_ReleaseChar(g_BgResourceManagers[1], res->charResource);
        BgResMgr_ReleaseScreen(g_BgResourceManagers[1], res->screenResource);
        PaletteMgr_ReleaseResource(g_PaletteManagers[1], res->paletteResource);
    }
    DatMgr_ReleaseData(res->data);
}

void func_ov043_020b2854(MenuBgResource *res, s32 engine) {
    if (engine == 0) {
        BgResMgr_ReleaseScreen(g_BgResourceManagers[0], res->screenResource);
        return;
    }
    BgResMgr_ReleaseScreen(g_BgResourceManagers[1], res->screenResource);
}

void func_ov043_020b288c(MenuBgResource *res, s32 engine, s32 layer, s32 binIndex, s32 start, u32 count) {
    func_ov043_020b27cc(res, engine);
    func_ov043_020b2100(res, engine, layer, binIndex, start, count);
}

void func_ov043_020b28d4(MenuBgResource *res, Data *data, s32 engine, s32 layer, s32 screenIndex) {
    func_ov043_020b2854(res, engine);
    func_ov043_020b2514(res, data, engine, layer, screenIndex);
}

void func_ov043_020b2910(MenuBgResource *res) {
    res->data            = 0;
    res->screenResource  = 0;
    res->charResource    = 0;
    res->paletteResource = 0;
    res->screenMap       = 0;
    res->charData        = 0;
    res->paletteData     = 0;
}

void func_ov043_020b2934(ShopObject *shop) {
    s32             i;
    MenuBgResource *sub  = &shop->resources[0];
    MenuBgResource *main = &shop->resources[4];

    for (i = 0; i < 4; i++) {
        func_ov043_020b2910(sub);
        func_ov043_020b2910(main);
        sub++;
        main++;
    }
    func_ov043_020b2100(&shop->resources[5], 0, 1, 0, 0xF, 1);
    if (shop->slots[shop->unk_848].itemID == 0xFFFF) {
        func_ov043_020b2300(&shop->resources[7], 0, 3, 1, 0, 0, 6);
    } else {
        s32 category = Inventory_GetCategory(shop->slots[shop->unk_848].itemID);
        if (category == 0) {
            func_ov043_020b2300(&shop->resources[7], 0, 3, 1, 8, 0, 6);
        } else if (category == 1) {
            func_ov043_020b2300(&shop->resources[7], 0, 3, 1, 0, 0, 6);
        } else {
            func_ov043_020b2300(&shop->resources[7], 0, 3, 1, 2, 0, 6);
        }
    }
    func_ov043_020b2514(&shop->resources[6], shop->resources[7].data, 0, 2, 9);
    func_ov043_020b2100(&shop->resources[1], 1, 1, 6, 0xF, 1);
    func_ov043_020b25c0(&shop->resources[3], 1, 3, 7, *(u16 *)((u8 *)shop + 0x76A), 0, 0xE);
}

void func_ov043_020b2af0(void) {

}

void func_ov043_020b2af4(s32 arg0) {
    func_ov043_020b27cc((MenuBgResource *)(arg0 + 0x900), 0);
    func_ov043_020b2854((MenuBgResource *)(arg0 + 0x91C), 0);
    func_ov043_020b27cc((MenuBgResource *)(arg0 + 0x938), 0);
    func_ov043_020b27cc((MenuBgResource *)(arg0 + 0x890), 1);
    func_ov043_020b27cc((MenuBgResource *)(arg0 + 0x8C8), 1);
}
/* Warning: struct OamCellPiece is not defined (only forward-declared) */
/* Warning: struct OSMutex is not defined (only forward-declared) */
/* Warning: struct OamAffineParam is not defined (only forward-declared) */
