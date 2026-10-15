#include "ShopPart.h"

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
