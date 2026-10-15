#include "ShopPart.h"

void func_ov043_020ac5d8(ShopState *arg0) {
    ShopObject *shop;

    shop = &arg0->shop;
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
        func_ov043_020b1404(shop);
    } else {
        func_ov043_020b0d1c(shop);
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

    arg0->taskId_WindowU = fn((TaskPool *) (&(arg0)->base.taskPool), arg0->base.dataType, (u16) (arg3 + ((arg1 * 0x15) + 0x303F + (arg2 * 7))), &arg0->shop);
}

void func_ov043_020ac6f4(ShopState *arg0) {
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
    if ((u32) shop->unk_860 <= 0xCU) {
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
    arg0->taskId_ItemTotal = Shop_itemTotal_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, shop->unk_844, shop);
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
    func_ov043_020ac6a0(arg0, shop->unk_774, shop->unk_78C, 0);
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
    void func_ov043_020ad3c8(ShopState *arg0);
    ShopObject   *shop;
    ShopItemSlot *entry;
    s32           var_r4;

    shop = arg1;
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
    entry = (ShopItemSlot *) shop->unk_738[shop->unk_844];
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
                if ((s32) temp_r2_3 < (s32) (shop->unk_862 - 6)) {
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
                    shop->unk_848 = shop->unk_862 - 1;
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
                if (temp_r2_5 == (shop->unk_862 - 1)) {
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
    void     func_ov043_020ae24c(ShopState *);
    void     func_ov043_020ad738(ShopState *);
    ShopObject *shop;

    shop = &arg0->shop;
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
    void func_ov043_020ad9dc(ShopState *arg0);
    ShopObject *shop;
    TouchCoord  sp0;
    s16         temp_r1;
    u16         temp_r1_2;
    u16         temp_r1_3;

    shop = &arg0->shop;
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
            if (func_ov043_020afcb0(shop) == 1) {
                shop->unk_855 = 1;
                DebugOvlDisp_ReplaceTop((OverlayCB) func_ov043_020ad9dc, arg0, 0);
                return;
            }
            if (func_ov043_020b1a70(shop, shop->unk_83C) != 1) {
                return;
            }
            SndMgr_StartPlayingSE(0x128);
            shop->unk_840 = (s16) sp0.x;
            shop->unk_842 = (s16) sp0.y;
            shop->unk_834 |= 1;
            arg0->unk_21768 = Shop_mov_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, shop->unk_844, shop);
            arg0->unk_2176C = Shop_movShadow_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, shop);
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
    func_ov043_020acbd0(shop, temp_r1_3);
    shop->unk_852 = shop->unk_850;
}

void func_ov043_020ad738(ShopState *arg0) {
    void        func_ov043_020ad76c(ShopState *arg0);
    ShopObject *shop;

    shop = &arg0->shop;
    func_ov043_020b1fc4(shop, (u8) shop->unk_7B7, 0);
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
    void func_ov043_020ad90c(ShopState *arg0);
    void func_ov043_020ad3c8(ShopState *arg0);
    ShopObject *shop;

    shop = &arg0->shop;
    if (func_ov043_020afbd4(shop, shop->unk_84A) == 1) {
        if (shop->unk_84A == 0) {
            EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->taskId_WindowU);
            func_ov043_020ac6a0(arg0, shop->unk_774, shop->unk_78C, 4);
            func_ov043_020b1fc4(shop, shop->unk_7B7, 2);
            g_DisplaySettings.controls[0].layers |= 4;
        } else {
            EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->unk_21774);
        }
        shop->unk_85E = 2;
        arg0->unk_21774 = Shop_window0_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, shop);
        arg0->unk_2177C = Shop_item2_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, shop->unk_84C, 1, shop);
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
    ShopObject *shop;

    shop = &arg0->shop;
    EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->taskId_WindowU);
    func_ov043_020ac6a0(arg0, shop->unk_774, shop->unk_78C, 3);
    shop->unk_85E = 3;
    arg0->unk_21774 = Shop_window0_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, shop);
    arg0->unk_2177C = Shop_item2_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, shop->unk_84C, 1, shop);
    SndMgr_StartPlayingSE(0x13E);
    func_ov043_020b1fc4(shop, shop->unk_7B7, 2);
    DebugOvlDisp_ReplaceTop((OverlayCB) func_ov043_020adaa4, arg0, 0);
}
