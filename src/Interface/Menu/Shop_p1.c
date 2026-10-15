#include "ShopPart.h"

void func_ov043_020ac5d8(ShopState *arg0) {
    u32 *temp_r4;

    temp_r4 = &(arg0)->shop;
    arg0->unk_217AC = 0;
    arg0->unk_217B0 = 0;
    arg0->shop.unk_834 = 0;
    arg0->shop.unk_83A = 0;
    arg0->shop.unk_836 = 1;
    arg0->shop.unk_855 = 1;
    arg0->shop.unk_837 = 0;
    arg0->shop.unk_840 = 0;
    arg0->shop.unk_842 = 0;
    arg0->shop.unk_854 = 0;
    arg0->shop.unk_85E = 0;
    arg0->shop.unk_84C = 0xFFFF;
    arg0->shop.unk_84E = 0xFFFF;
    if (gSaveData.unk_1AB4 & 4) {
        func_ov043_020b1404(temp_r4);
    } else {
        func_ov043_020b0d1c(temp_r4);
    }
    arg0->shop.unk_844 = 0;
    arg0->shop.unk_848 = 0;
    arg0->shop.unk_850 = 0;
    arg0->shop.unk_852 = 0;
    arg0->shop.unk_868 = 0;
    arg0->shop.unk_838 = 0;
    arg0->shop.unk_839 = 0;
    arg0->shop.unk_84A = 0;
    arg0->shop.unk_864[0] = 0;
    arg0->shop.unk_864[1] = 0;
    arg0->shop.unk_864[2] = 0;
    arg0->unk_217B2 = 0;
}

void func_ov043_020ac6a0(ShopState *arg0, u16 arg1, u16 arg2, s32 arg3) {
    arg0->taskId_WindowU = Shop_windowU_CreateTask((TaskPool *) (&(arg0)->base.taskPool), arg0->base.dataType, (s16) (u16) (arg3 + ((arg1 * 0x15) + 0x303F + (arg2 * 7))), (ShopObject *) (&(arg0)->shop));
}

void func_ov043_020ac6f4(ShopState *arg0) {
    s32 var_r0;
    u16 var_r7;
    u32 *temp_r0;
    u32 *temp_r5;
    u32 *temp_r6;

    temp_r6 = &(arg0)->shop;
    EasyTask_CreateTask((TaskPool *) (&(arg0)->base.taskPool), &Task_EasyFade, NULL, 0, NULL, NULL);
    EasyFade_FadeBothDisplays(2, -0x10, 0x1000);
    arg0->taskId_Pointer = Shop_pointer_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, temp_r6);
    arg0->taskId_NameD = Shop_nameD_CreateTask((TaskPool *) (&(arg0)->base.taskPool), arg0->base.dataType, (ShopObject *) temp_r6);
    arg0->taskId_Exit = Shop_exit_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, temp_r6);
    arg0->taskId_NumMoneyNek = Shop_numMoneyNek_CreateTask((TaskPool *) (&(arg0)->base.taskPool), arg0->base.dataType, (ShopObject *) temp_r6);
    arg0->taskId_NumMoneyBdg = Shop_numMoneyBdg_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, temp_r6);
    temp_r0 = &(arg0)->base.taskPool;
    if ((u32) arg0->shop.unk_860 <= 0xCU) {
        var_r0 = Shop_sbar2_CreateTask(temp_r0, arg0->base.dataType, temp_r6);
    } else {
        var_r0 = Shop_sbar_CreateTask(temp_r0, arg0->base.dataType, temp_r6);
    }
    arg0->taskId_Sbar = var_r0;
    arg0->taskId_Check = Shop_check_CreateTask((TaskPool *) (&(arg0)->base.taskPool), arg0->base.dataType, (ShopObject *) temp_r6);
    arg0->taskId_Tab = Shop_tab_CreateTask((TaskPool *) (&(arg0)->base.taskPool), arg0->base.dataType, (ShopObject *) temp_r6);
    arg0->taskId_TextScr = Shop_textScr_CreateTask((TaskPool *) (&(arg0)->base.taskPool), arg0->base.dataType, (ShopObject *) temp_r6);
    var_r7 = 0;
    temp_r5 = &(arg0)->base.taskPool;
    do {
        arg0->taskId_Item[var_r7] = Shop_item_CreateTask((TaskPool *) (temp_r5), arg0->base.dataType, var_r7, (ShopObject *) temp_r6);
        arg0->taskId_ItemPlate[var_r7] = Shop_itemPlate_CreateTask(temp_r5, arg0->base.dataType, var_r7, temp_r6);
        arg0->taskId_ItemPrice[var_r7] = Shop_itemPrice_CreateTask(temp_r5, arg0->base.dataType, var_r7, temp_r6);
        arg0->taskId_ItemMask[var_r7] = Shop_itemMask_CreateTask(temp_r5, arg0->base.dataType, var_r7, temp_r6);
        arg0->taskId_ItemAbl[var_r7] = Shop_itemAbl_CreateTask((TaskPool *) (temp_r5), arg0->base.dataType, (s16) var_r7, (ShopObject *) temp_r6);
        arg0->taskId_ItemStar[var_r7] = Shop_itemStar_CreateTask((TaskPool *) (temp_r5), arg0->base.dataType, (s16) var_r7, (ShopObject *) temp_r6);
        var_r7 += 1;
    } while ((u32) var_r7 < 0xCU);
    arg0->taskId_ItemCur = Shop_itemCur_CreateTask((TaskPool *) (&(arg0)->base.taskPool), arg0->base.dataType, (ShopObject *) temp_r6);
    arg0->taskId_ItemTotal = Shop_itemTotal_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, arg0->shop.unk_844, temp_r6);
    arg0->taskId_ItemTotalNum = Shop_itemTotalNum_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, temp_r6);
    arg0->taskId_BrdLogo = Shop_brdLogo_CreateTask((TaskPool *) (&(arg0)->base.taskPool), arg0->base.dataType, (ShopObject *) temp_r6);
    arg0->taskId_NameU = Shop_nameU_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, temp_r6);
    arg0->taskId_NumLvU = Shop_numLvU_CreateTask((TaskPool *) (&(arg0)->base.taskPool), arg0->base.dataType, (ShopObject *) temp_r6);
    arg0->taskId_NumAblU = Shop_numAblU_CreateTask((TaskPool *) (&(arg0)->base.taskPool), arg0->base.dataType, (ShopObject *) temp_r6);
    arg0->taskId_NumFsgU = Shop_numFsgU_CreateTask((TaskPool *) (&(arg0)->base.taskPool), arg0->base.dataType, (ShopObject *) temp_r6);
    arg0->taskId_FsgGaugeU = Shop_fsgGaugeU_CreateTask((TaskPool *) (&(arg0)->base.taskPool), arg0->base.dataType, (ShopObject *) temp_r6);
    arg0->taskId_TextScrU = Shop_textScrU_CreateTask((TaskPool *) (&(arg0)->base.taskPool), arg0->base.dataType, (ShopObject *) temp_r6);
    arg0->taskId_BoardU = Shop_boardU_CreateTask((TaskPool *) (&(arg0)->base.taskPool), arg0->base.dataType, (ShopObject *) temp_r6);
    arg0->taskId_CastU = Shop_castU_CreateTask((TaskPool *) (&(arg0)->base.taskPool), arg0->base.dataType, (ShopObject *) temp_r6);
    func_ov043_020ac6a0(arg0, arg0->shop.unk_774, arg0->shop.unk_78C, 0);
}

void func_ov043_020acb08(ShopState *arg0) {
    u32 *temp_r5;

    temp_r5 = &(arg0)->shop;
    EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->taskId_ItemTotal);
    EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->taskId_ItemTotalNum);
    EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->taskId_BrdLogo);
    arg0->shop.unk_85E = 0;
    arg0->shop.unk_834 |= 0x40;
    arg0->taskId_ItemTotal = Shop_itemTotal_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, arg0->shop.unk_844, temp_r5);
    arg0->taskId_ItemTotalNum = Shop_itemTotalNum_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, temp_r5);
    arg0->taskId_BrdLogo = Shop_brdLogo_CreateTask((TaskPool *) (&(arg0)->base.taskPool), arg0->base.dataType, (ShopObject *) (temp_r5));
}

void func_ov043_020acbd0(s32 arg0, u16 arg1) {
    ResourceManager *temp_r7;
    s16 var_r5;
    s16 var_r6;
    s16 var_r6_2;
    u16 temp_r8;
    u16 var_r8;
    u16 var_r9;
    u32 *temp_r5;
    u32 *temp_r5_2;
    u32 *temp_r6;
    void *temp_r2;

    temp_r7 = data_ov043_020cd288;
    var_r8 = arg1;
    var_r6 = 0;
    temp_r5 = &(temp_r7)->base.taskPool;
    do {
        func_ov043_020b6dc8(temp_r5, (temp_r7 + (var_r6 * 4))->unk_2163C);
        EasyTask_DeleteTask((TaskPool *) (temp_r5), (temp_r7 + (var_r6 * 4))->unk_2163C);
        var_r6 += 1;
    } while ((s32) var_r6 < 0xC);
    var_r6_2 = 0;
    temp_r5_2 = &(temp_r7)->base.taskPool;
    do {
        EasyTask_DeleteTask((TaskPool *) (temp_r5_2), (temp_r7 + (var_r6_2 * 4))->unk_2166C);
        EasyTask_DeleteTask((TaskPool *) (temp_r5_2), (temp_r7 + (var_r6_2 * 4))->unk_2169C);
        EasyTask_DeleteTask((TaskPool *) (temp_r5_2), (temp_r7 + (var_r6_2 * 4))->unk_216FC);
        EasyTask_DeleteTask((TaskPool *) (temp_r5_2), (temp_r7 + (var_r6_2 * 4))->unk_216CC);
        var_r6_2 += 1;
    } while ((s32) var_r6_2 < 0xC);
    var_r5 = 0;
    do {
        temp_r2 = arg0 + (var_r5 * 4);
        var_r5 += 1;
        temp_r2->unk_738 = (s32) ((var_r8 * 0x2C) + arg0);
        var_r8 += 1;
    } while ((s32) var_r5 < 0xC);
    var_r9 = 0;
    temp_r6 = &(temp_r7)->base.taskPool;
    do {
        temp_r8 = var_r9;
        (temp_r7 + (var_r9 * 4))->unk_2163C = Shop_item_CreateTask((TaskPool *) (temp_r6), temp_r7->unk_11588, temp_r8, (ShopObject *) arg0);
        (temp_r7 + (var_r9 * 4))->unk_2166C = Shop_itemPlate_CreateTask(temp_r6, temp_r7->unk_11588, temp_r8, (u32 *) arg0);
        (temp_r7 + (var_r9 * 4))->unk_2169C = Shop_itemPrice_CreateTask(temp_r6, temp_r7->unk_11588, temp_r8, (u32 *) arg0);
        (temp_r7 + (var_r9 * 4))->unk_216FC = Shop_itemMask_CreateTask(temp_r6, temp_r7->unk_11588, temp_r8, (u32 *) arg0);
        (temp_r7 + (var_r9 * 4))->unk_216CC = Shop_itemAbl_CreateTask((TaskPool *) (temp_r6), temp_r7->unk_11588, (s16) temp_r8, (ShopObject *) arg0);
        var_r9 = (u16) (s16) (var_r9 + 1);
    } while ((s32) var_r9 < 0xC);
}

void func_ov043_020acd90(ShopState *arg0, void *arg1) {
    s32 var_r4;
    u16 temp_r1_5;
    u16 temp_r2;
    u32 temp_r0;
    u32 temp_r1;
    u32 temp_r1_2;
    u32 temp_r1_3;
    void *temp_r1_4;

    var_r4 = 0;
    arg1->unk_858 = (s32) (arg1->unk_858 - (arg1 + (arg1->unk_844 * 4))->unk_738->unk_8);
    if ((u32) (arg1 + (arg1->unk_844 * 4))->unk_738->unk_8 >= 0x2710U) {
        Stats_AddExperience(1);
    }
    temp_r1 = arg1->unk_77C;
    if (temp_r1 < (u32) arg1->unk_784) {
        temp_r1_2 = temp_r1 + (arg1 + (arg1->unk_844 * 4))->unk_738->unk_8;
        arg1->unk_77C = temp_r1_2;
        temp_r0 = arg1->unk_784;
        if (temp_r1_2 > temp_r0) {
            arg1->unk_77C = temp_r0;
        }
        temp_r1_3 = arg1->unk_780;
        if ((u32) arg1->unk_77C >= temp_r1_3) {
            arg1->unk_77C = temp_r1_3;
            arg1->unk_778 = (u16) (arg1->unk_778 + 1);
            temp_r2 = arg1->unk_77A;
            if ((u32) arg1->unk_778 > (u32) temp_r2) {
                arg1->unk_778 = temp_r2;
            }
            func_ov043_020af994(arg1);
            arg1->unk_780 = (u32) M2C_ERROR(/* Read from unset register $r0 */);
            DebugOvlDisp_ReplaceTop(func_ov043_020adbb0, arg0, 0);
            var_r4 = 1;
        }
    }
    func_ov043_020afa20(arg1);
    func_ov043_020b1e2c(arg1);
    if (M2C_ERROR(/* Read from unset register $r0 */) == 1) {
        arg1->unk_7B6 = 1;
    }
    temp_r1_4 = (arg1 + (arg1->unk_844 * 4))->unk_738;
    if (temp_r1_4->unk_10 == 1) {
        func_02023d3c(temp_r1_4->unk_14);
    }
    EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->taskId_WindowU);
    EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->taskId_ItemTotalNum);
    func_ov043_020ac6a0(arg0, arg1->unk_774, arg1->unk_78C, 2);
    arg0->taskId_ItemTotalNum = Shop_itemTotalNum_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, (u32 *) arg1);
    arg1->unk_83A = 0x12C;
    temp_r1_5 = arg1->unk_834 | 8;
    arg1->unk_834 = temp_r1_5;
    if ((temp_r1_5 & 4) && (var_r4 == 0)) {
        SndMgr_StartPlayingSE(0x11C);
    }
    if (var_r4 != 0) {
        return;
    }
    arg1->unk_855 = 0;
    DebugOvlDisp_ReplaceTop(func_ov043_020ad3c8, arg0);
}

void func_ov043_020acf6c(ShopState *arg0) {
    u16 temp_r0;
    u16 temp_r1;
    u16 temp_r1_2;
    u16 temp_r1_3;
    u16 temp_r1_4;
    u16 temp_r1_5;
    u16 temp_r2;
    u16 temp_r2_2;
    u16 temp_r2_3;
    u16 temp_r2_4;
    u16 temp_r2_5;
    u16 temp_r3;
    u16 temp_r3_2;

    if (!(InputStatus.unk_4 & 0xC03) && !(InputStatus.unk_4 & 0xF0)) {
        arg0->unk_217B2 = 0;
    }
    if (!(InputStatus.unk_6 & 0x300)) {
        temp_r2 = arg0->unk_217B2;
        if (temp_r2 != 0) {
            arg0->unk_217B2 = temp_r2 - 1;
            return;
        }
        if ((InputStatus.unk_6 & 0xC03) || (InputStatus.unk_6 & 0xF0)) {
            arg0->unk_217B2 = 0x14;
        } else if ((InputStatus.unk_4 & 0xC03) || (InputStatus.unk_4 & 0xF0)) {
            arg0->unk_217B2 = 1;
        }
        if ((InputStatus.unk_4 & 0x40) || (InputStatus.unk_4 & 0x400)) {
            temp_r2_2 = arg0->shop.unk_850;
            temp_r1 = arg0->shop.unk_848;
            if (((u32) temp_r1 >= (u32) temp_r2_2) && ((s32) temp_r1 < (s32) (temp_r2_2 + 0xC))) {
                if ((u32) temp_r1 >= 6U) {
                    SndMgr_StartPlayingSE(0x11A);
                    temp_r1_2 = arg0->shop.unk_844;
                    if ((u32) temp_r1_2 < 6U) {
                        arg0->shop.unk_850 -= 6;
                        arg0->shop.unk_848 -= 6;
                    } else {
                        arg0->shop.unk_844 = temp_r1_2 - 6;
                        arg0->shop.unk_848 -= 6;
                    }
                }
            } else {
                SndMgr_StartPlayingSE(0x11A);
                arg0->shop.unk_844 = 0;
                arg0->shop.unk_848 = arg0->shop.unk_850;
            }
            func_ov043_020acb08(arg0);
        } else if ((InputStatus.unk_4 & 0x80) || (InputStatus.unk_4 & 2)) {
            temp_r1_3 = arg0->shop.unk_850;
            temp_r2_3 = arg0->shop.unk_848;
            if (((u32) temp_r2_3 >= (u32) temp_r1_3) && ((s32) temp_r2_3 < (s32) (temp_r1_3 + 0xC))) {
                if ((s32) temp_r2_3 < (s32) (arg0->shop.unk_862 - 6)) {
                    SndMgr_StartPlayingSE(0x11A);
                    temp_r1_4 = arg0->shop.unk_844;
                    if (((u32) temp_r1_4 >= 6U) && ((u32) temp_r1_4 < 0xCU)) {
                        arg0->shop.unk_850 += 6;
                        arg0->shop.unk_848 += 6;
                    } else {
                        arg0->shop.unk_844 += 6;
                        arg0->shop.unk_848 += 6;
                    }
                }
            } else {
                SndMgr_StartPlayingSE(0x11A);
                arg0->shop.unk_844 = 0;
                arg0->shop.unk_848 = arg0->shop.unk_850;
            }
            func_ov043_020acb08(arg0);
        } else if ((InputStatus.unk_4 & 0x20) || (InputStatus.unk_4 & 0x800)) {
            SndMgr_StartPlayingSE(0x11A);
            temp_r3 = arg0->shop.unk_850;
            temp_r2_4 = arg0->shop.unk_848;
            if (((u32) temp_r2_4 >= (u32) temp_r3) && ((s32) temp_r2_4 < (s32) (temp_r3 + 0xC))) {
                if (temp_r2_4 == 0) {
                    arg0->shop.unk_850 = arg0->shop.unk_870 * 6;
                    arg0->shop.unk_844 = 0xB;
                    arg0->shop.unk_848 = arg0->shop.unk_862 - 1;
                } else {
                    temp_r0 = arg0->shop.unk_844;
                    if (temp_r0 == 0) {
                        arg0->shop.unk_850 = temp_r3 - 6;
                        arg0->shop.unk_844 = 5;
                        arg0->shop.unk_848 -= 1;
                    } else {
                        arg0->shop.unk_844 = temp_r0 - 1;
                        arg0->shop.unk_848 -= 1;
                    }
                }
            } else {
                arg0->shop.unk_844 = 0;
                arg0->shop.unk_848 = arg0->shop.unk_850;
            }
            func_ov043_020acb08(arg0);
        } else if ((InputStatus.unk_4 & 0x10) || (InputStatus.unk_4 & 1)) {
            SndMgr_StartPlayingSE(0x11A);
            temp_r3_2 = arg0->shop.unk_850;
            temp_r2_5 = arg0->shop.unk_848;
            if (((u32) temp_r2_5 >= (u32) temp_r3_2) && ((s32) temp_r2_5 < (s32) (temp_r3_2 + 0xC))) {
                if (temp_r2_5 == (arg0->shop.unk_862 - 1)) {
                    arg0->shop.unk_850 = 0;
                    arg0->shop.unk_844 = 0;
                    arg0->shop.unk_848 = 0;
                } else {
                    temp_r1_5 = arg0->shop.unk_844;
                    if (temp_r1_5 == 0xB) {
                        arg0->shop.unk_850 = temp_r3_2 + 6;
                        arg0->shop.unk_844 = 6;
                        arg0->shop.unk_848 += 1;
                    } else {
                        arg0->shop.unk_844 = temp_r1_5 + 1;
                        arg0->shop.unk_848 += 1;
                    }
                }
            } else {
                arg0->shop.unk_844 = 0;
                arg0->shop.unk_848 = arg0->shop.unk_850;
            }
            func_ov043_020acb08(arg0);
        }
        goto block_55;
    }
block_55:
    if (gSaveData.unk_1AB4 & 4) {
        return;
    }
    func_ov043_020afcb0(&(arg0)->shop);
    if (M2C_ERROR(/* Read from unset register $r0 */) != 1) {
        return;
    }
    DebugOvlDisp_ReplaceTop(func_ov043_020ad9dc, arg0, 0);
}

void func_ov043_020ad354(ShopState *arg0) {
    EasyFade_FadeBothDisplays(0, 0, 0x1000);
    if (EasyFade_IsFading() != 0) {
        return;
    }
    arg0->shop.unk_83A = 0x12C;
    if (gSaveData.unk_1AB4 & 4) {
        DebugOvlDisp_ReplaceTop(func_ov043_020ae24c, arg0);
        return;
    }
    DebugOvlDisp_ReplaceTop(func_ov043_020ad738, arg0);
}

void func_ov043_020ad3c8(ShopState *arg0) {
    s32 sp0;
    s16 temp_r1;
    u16 temp_r1_2;
    u16 temp_r1_3;
    u32 *temp_r4;

    temp_r4 = &(arg0)->shop;
    if (arg0->shop.unk_834 & 1) {
        TouchInput_GetCoord(&sp0);
        arg0->shop.unk_840 = (s16) sp0;
        arg0->shop.unk_842 = (s16) sp4;
        if (TouchInput_WasTouchReleased() != 0) {
            arg0->shop.unk_834 &= ~1;
            EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->unk_21768);
            EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->unk_2176C);
            func_ov043_020b18e4((s16) sp0, (s16) sp4);
            if (M2C_ERROR(/* Read from unset register $r0 */) == 1) {
                SndMgr_StartPlayingSE(0x129);
                if ((u32) arg0->shop.unk_858 >= (u32) temp_r4[arg0->shop.unk_844].unk_738->unk_8) {
                    arg0->shop.unk_855 = 1;
                    DebugOvlDisp_ReplaceTop(func_ov043_020adef8, arg0, 0);
                }
            }
        }
        goto block_20;
    }
    if (TouchInput_WasTouchPressed() != 0) {
        TouchInput_GetCoord(&sp0);
        func_ov043_020b1944((s16) sp0, (s16) sp4);
        arg0->shop.unk_83C = M2C_ERROR(/* Read from unset register $r0 */);
        temp_r1 = arg0->shop.unk_83C;
        if ((temp_r1 != -1) && (*temp_r4[temp_r1].unk_738 != 0xFFFF)) {
            SndMgr_StartPlayingSE(0x11A);
            arg0->shop.unk_844 = (u16) arg0->shop.unk_83C;
            arg0->shop.unk_848 = arg0->shop.unk_850 + arg0->shop.unk_844;
            func_ov043_020acb08(arg0);
            func_ov043_020afcb0(temp_r4);
            if (M2C_ERROR(/* Read from unset register $r0 */) == 1) {
                arg0->shop.unk_855 = 1;
                DebugOvlDisp_ReplaceTop(func_ov043_020ad9dc, arg0, 0);
                return;
            }
            func_ov043_020b1a70(temp_r4, arg0->shop.unk_83C);
            if (M2C_ERROR(/* Read from unset register $r0 */) != 1) {
                return;
            }
            SndMgr_StartPlayingSE(0x128);
            arg0->shop.unk_840 = (s16) sp0;
            arg0->shop.unk_842 = (s16) sp4;
            arg0->shop.unk_834 |= 1;
            arg0->unk_21768 = Shop_mov_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, arg0->shop.unk_844, temp_r4);
            arg0->unk_2176C = Shop_movShadow_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, temp_r4);
            return;
        }
        func_ov043_020b17d0((s16) sp0, (s16) sp4);
        if (M2C_ERROR(/* Read from unset register $r0 */) == 1) {
            arg0->shop.unk_838 = 1;
            arg0->shop.unk_839 = 0xC;
            arg0->shop.unk_834 |= 0x1000;
            arg0->shop.unk_855 = 1;
            SndMgr_StartPlayingSE(0x11B);
            func_ov043_020b1d24(gSaveData.unk_1AB2);
            if (M2C_ERROR(/* Read from unset register $r0 */) == -1) {
                arg0->shop.unk_837 = 1;
            } else {
                gSaveData.unk_1AB2 = (u8) M2C_ERROR(/* Read from unset register $r0 */);
                arg0->shop.unk_837 = 2;
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
    temp_r1_2 = arg0->shop.unk_83A;
    if (temp_r1_2 != 0) {
        arg0->shop.unk_83A = temp_r1_2 - 1;
        if (arg0->shop.unk_83A == 0) {
            EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->taskId_WindowU);
            func_ov043_020ac6a0(arg0, arg0->shop.unk_774, arg0->shop.unk_78C, 1);
        }
    }
    temp_r1_3 = arg0->shop.unk_850;
    if (arg0->shop.unk_852 == temp_r1_3) {
        return;
    }
    func_ov043_020acbd0((s32) temp_r4, temp_r1_3);
    arg0->shop.unk_852 = arg0->shop.unk_850;
}

void func_ov043_020ad738(ShopState *arg0) {
    func_ov043_020b1fc4(&(arg0)->shop, arg0->shop.unk_7B7, 0);
    DebugOvlDisp_ReplaceTop(func_ov043_020ad76c, arg0, 0);
}

void func_ov043_020ad76c(ShopState *arg0) {
    if (SndMgr_IsSEPlaying(0x376) != 0) {
        return;
    }
    DebugOvlDisp_ReplaceTop(func_ov043_020ad7a0, arg0, 0);
}

void func_ov043_020ad7a0(ShopState *arg0) {
    u32 *temp_r0;
    u32 *temp_r4;

    temp_r4 = &(arg0)->shop;
    func_ov043_020afbd4(temp_r4, arg0->shop.unk_84A);
    if (M2C_ERROR(/* Read from unset register $r0 */) == 1) {
        temp_r0 = &(arg0)->base.taskPool;
        if (arg0->shop.unk_84A == 0) {
            EasyTask_DeleteTask((TaskPool *) (temp_r0), (u32) arg0->taskId_WindowU);
            func_ov043_020ac6a0(arg0, arg0->shop.unk_774, arg0->shop.unk_78C, 4);
            func_ov043_020b1fc4(temp_r4, arg0->shop.unk_7B7, 2);
            g_DisplaySettings.controls[0].layers |= 4;
        } else {
            EasyTask_DeleteTask((TaskPool *) (temp_r0), (u32) arg0->unk_21774);
        }
        arg0->shop.unk_85E = 2;
        arg0->unk_21774 = Shop_window0_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, temp_r4);
        arg0->unk_2177C = Shop_item2_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, arg0->shop.unk_84C, 1, temp_r4);
        DebugOvlDisp_ReplaceTop(func_ov043_020ad90c, arg0, 0);
        return;
    }
    if (arg0->shop.unk_84A != 0) {
        arg0->shop.unk_85E = 0;
        arg0->shop.unk_834 |= 0x40;
        g_DisplaySettings.controls[0].layers &= ~4;
        EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->unk_21774);
    }
    arg0->shop.unk_855 = 0;
    DebugOvlDisp_ReplaceTop(func_ov043_020ad3c8, arg0);
}

void func_ov043_020ad90c(ShopState *arg0) {
    SndMgr_StartPlayingSE(0x13E);
    arg0->shop.unk_7B4 += 1;
    arg0->unk_217B0 = 0xA;
    DebugOvlDisp_ReplaceTop(func_ov043_020ad958, arg0, 0);
}

void func_ov043_020ad958(ShopState *arg0) {
    u16 temp_r0;

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
    arg0->shop.unk_84A += 1;
    EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->unk_2177C);
    DebugOvlDisp_ReplaceTop(func_ov043_020ad7a0, arg0, 0);
}

void func_ov043_020ad9dc(ShopState *arg0) {
    u32 *temp_r4;

    temp_r4 = &(arg0)->shop;
    EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->taskId_WindowU);
    func_ov043_020ac6a0(arg0, arg0->shop.unk_774, arg0->shop.unk_78C, 3);
    arg0->shop.unk_85E = 3;
    arg0->unk_21774 = Shop_window0_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, temp_r4);
    arg0->unk_2177C = Shop_item2_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, arg0->shop.unk_84C, 1, temp_r4);
    SndMgr_StartPlayingSE(0x13E);
    func_ov043_020b1fc4(temp_r4, arg0->shop.unk_7B7, 2);
    DebugOvlDisp_ReplaceTop(func_ov043_020adaa4, arg0, 0);
}
