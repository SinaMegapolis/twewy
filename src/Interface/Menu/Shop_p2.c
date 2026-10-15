#include "ShopPart.h"

void func_ov043_020adaa4(ShopState *arg0) {
    arg0->shop.unk_7B4 += 1;
    arg0->unk_217B0 = 0xA;
    g_DisplaySettings.controls[0].layers |= 4;
    DebugOvlDisp_ReplaceTop(func_ov043_020adaf4, arg0, 0);
}

void func_ov043_020adaf4(ShopState *arg0) {
    u16 temp_r2;

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
    arg0->shop.unk_85E = 0;
    arg0->shop.unk_834 |= 0x40;
    g_DisplaySettings.controls[0].layers &= ~4;
    EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->unk_21774);
    EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->unk_2177C);
    arg0->shop.unk_855 = 0;
    DebugOvlDisp_ReplaceTop(func_ov043_020ad3c8, arg0);
}

void func_ov043_020adbb0(ShopState *arg0) {
    u16 temp_r2;
    u32 *temp_r4;

    temp_r4 = &(arg0)->shop;
    arg0->unk_217B0 = 0xA;
    if ((u32) (temp_r4 + ((arg0->shop.unk_778 - 2) * 2))->unk_7A2 >= 0x3D4U) {
        arg0->shop.unk_7B8 += 1;
        func_ov043_020afb14(temp_r4);
        arg0->shop.unk_7B5 = M2C_ERROR(/* Read from unset register $r0 */);
        arg0->shop.unk_84E = 0xFFFF;
    } else {
        func_ov043_020afd6c(temp_r4);
        if (arg0->shop.unk_860 == 0xD) {
            EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->taskId_Sbar);
            arg0->taskId_Sbar = Shop_sbar_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, temp_r4);
        }
        func_ov043_020b02ac(arg0->shop.unk_860);
        arg0->shop.unk_862 = M2C_ERROR(/* Read from unset register $r0 */);
        arg0->shop.unk_870 = ((arg0->shop.unk_862 / 6) + ((u16) arg0->shop.unk_862 >> 0x1F)) - 2;
        arg0->shop.unk_872 = arg0->shop.unk_870;
        func_ov043_020acbd0((s32) temp_r4, arg0->shop.unk_850);
    }
    arg0->shop.unk_85E = 1;
    arg0->unk_21774 = Shop_window0_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, temp_r4);
    temp_r2 = arg0->shop.unk_84E;
    if (temp_r2 != 0xFFFF) {
        arg0->unk_2177C = Shop_item2_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, temp_r2, 0, temp_r4);
    }
    SndMgr_StartPlayingSE(0x13F);
    g_DisplaySettings.controls[0].layers |= 4;
    DebugOvlDisp_ReplaceTop(func_ov043_020add48, arg0, 0);
}

void func_ov043_020add48(ShopState *arg0) {
    u16 temp_r2;

    temp_r2 = arg0->unk_217B0;
    if (temp_r2 != 0) {
        arg0->unk_217B0 = temp_r2 - 1;
        return;
    }
    if (TouchInput_WasTouchPressed() == 0) {
        return;
    }
    arg0->shop.unk_85E = 0;
    arg0->shop.unk_834 |= 0x40;
    g_DisplaySettings.controls[0].layers &= ~4;
    EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->unk_21774);
    if (arg0->shop.unk_84E != 0xFFFF) {
        EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->unk_2177C);
    }
    arg0->shop.unk_855 = 0;
    DebugOvlDisp_ReplaceTop(func_ov043_020ad3c8, arg0);
}

void func_ov043_020ade08(ShopState *arg0) {
    u16 temp_r1;

    EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->taskId_WindowU);
    temp_r1 = arg0->shop.unk_774;
    if (arg0->shop.unk_834 & 8) {
        func_ov043_020ac6a0(arg0, temp_r1, arg0->shop.unk_78C, 6);
    } else {
        func_ov043_020ac6a0(arg0, temp_r1, arg0->shop.unk_78C, 5);
    }
    func_ov043_020b1fc4(&(arg0)->shop, arg0->shop.unk_7B7, 1);
    arg0->shop.unk_83A = 0xB4;
    DebugOvlDisp_ReplaceTop(func_ov043_020ade9c, arg0, 0);
}

void func_ov043_020ade9c(ShopState *arg0) {
    u16 temp_r1;

    temp_r1 = arg0->shop.unk_83A;
    if (temp_r1 != 0) {
        arg0->shop.unk_83A = temp_r1 - 1;
    }
    if (TouchInput_WasTouchPressed() != 0) {
        arg0->shop.unk_83A = 0;
    }
    if (arg0->shop.unk_83A != 0) {
        return;
    }
    if (SndMgr_IsSEPlaying(0x376) != 0) {
        return;
    }
    DebugOvlDisp_Pop();
}

void func_ov043_020adef8(ShopState *arg0) {
    u16 temp_r1;
    u32 *temp_r0;
    u32 *temp_r5;

    temp_r5 = &(arg0)->shop;
    temp_r1 = arg0->shop.unk_844;
    temp_r0 = temp_r5;
    if (arg0->shop.unk_834 & 4) {
        func_ov043_020b1780((s32) temp_r0, (s32) temp_r1);
        if (M2C_ERROR(/* Read from unset register $r0 */) == 1) {
            func_ov043_020b16c4(temp_r5, arg0->shop.unk_844);
            func_ov043_020acd90(arg0, temp_r5);
            return;
        }
        DebugOvlDisp_ReplaceTop(func_ov043_020ae138, arg0, 0);
        return;
    }
    func_ov043_020b1780((s32) temp_r0, (s32) temp_r1);
    if (M2C_ERROR(/* Read from unset register $r0 */) == 1) {
        arg0->unk_21778 = Shop_window1_CreateTask((TaskPool *) (&(arg0)->base.taskPool), arg0->base.dataType, (ShopObject *) (temp_r5));
        g_DisplaySettings.controls[0].layers |= 4;
        DebugOvlDisp_ReplaceTop(func_ov043_020adfcc, arg0, 0);
        return;
    }
    DebugOvlDisp_ReplaceTop(func_ov043_020ae138, arg0, 0);
}

void func_ov043_020adfcc(ShopState *arg0) {
    s32 sp0;
    u32 *temp_r4;

    temp_r4 = &(arg0)->shop;
    if (TouchInput_WasTouchPressed() == 0) {
        return;
    }
    TouchInput_GetCoord(&sp0);
    func_ov043_020b1c7c((s16) sp0, (s16) sp4);
    if (M2C_ERROR(/* Read from unset register $r0 */) == -1) {
        return;
    }
    (temp_r4 + M2C_ERROR(/* Read from unset register $r0 */))->unk_864 = 1;
    arg0->shop.unk_839 = 0xC;
    arg0->shop.unk_864[2] = M2C_ERROR(/* Read from unset register $r0 */);
    if (M2C_ERROR(/* Read from unset register $r0 */) == 0) {
        func_ov043_020b16c4(temp_r4, arg0->shop.unk_844);
        SndMgr_StartPlayingSE(0x11C);
        arg0->shop.unk_83A = 0x14;
        DebugOvlDisp_ReplaceTop(func_ov043_020ae0b0, arg0, 0);
        return;
    }
    SndMgr_StartPlayingSE(0x11B);
    arg0->shop.unk_83A = 0x14;
    DebugOvlDisp_ReplaceTop(func_ov043_020ae0b0, arg0, 0);
}

void func_ov043_020ae0b0(ShopState *arg0) {
    u16 temp_r1;

    temp_r1 = arg0->shop.unk_83A;
    if (temp_r1 != 0) {
        arg0->shop.unk_83A = temp_r1 - 1;
        return;
    }
    EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->unk_21778);
    g_DisplaySettings.controls[0].layers &= ~4;
    if (arg0->shop.unk_864[2] == 0) {
        func_ov043_020acd90(arg0, &(arg0)->shop);
        return;
    }
    arg0->shop.unk_855 = 0;
    DebugOvlDisp_ReplaceTop(func_ov043_020ad3c8, arg0);
}

void func_ov043_020ae138(ShopState *arg0) {
    arg0->unk_217B0 = 0xA;
    arg0->shop.unk_85E = 4;
    arg0->unk_21774 = Shop_window0_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, &(arg0)->shop);
    SndMgr_StartPlayingSE(0x141);
    g_DisplaySettings.controls[0].layers |= 4;
    DebugOvlDisp_ReplaceTop(func_ov043_020ae1b8, arg0, 0);
}

void func_ov043_020ae1b8(ShopState *arg0) {
    u16 temp_r2;

    temp_r2 = arg0->unk_217B0;
    if (temp_r2 != 0) {
        arg0->unk_217B0 = temp_r2 - 1;
        return;
    }
    if (TouchInput_WasTouchPressed() == 0) {
        return;
    }
    arg0->shop.unk_85E = 0;
    arg0->shop.unk_834 |= 0x40;
    g_DisplaySettings.controls[0].layers &= ~4;
    EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->unk_21774);
    arg0->shop.unk_855 = 0;
    DebugOvlDisp_ReplaceTop(func_ov043_020ad3c8, arg0);
}

void func_ov043_020ae24c(ShopState *arg0) {
    arg0->shop.unk_855 = 0;
    func_ov043_020b1fc4(&(arg0)->shop, arg0->shop.unk_7B7, 0);
    DebugOvlDisp_ReplaceTop(func_ov043_020ae284, arg0, 0);
}

void func_ov043_020ae284(ShopState *arg0) {
    s32 sp0;
    s16 temp_r3;
    u16 temp_r1;
    u16 temp_r1_2;
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
                    DebugOvlDisp_ReplaceTop(func_ov043_020ae798, arg0, 0);
                }
            }
        }
        goto block_15;
    }
    if (TouchInput_WasTouchPressed() != 0) {
        TouchInput_GetCoord(&sp0);
        func_ov043_020b1944((s16) sp0, (s16) sp4);
        arg0->shop.unk_83C = M2C_ERROR(/* Read from unset register $r0 */);
        temp_r3 = arg0->shop.unk_83C;
        if ((temp_r3 != -1) && (*temp_r4[temp_r3].unk_738 != 0xFFFF)) {
            arg0->shop.unk_844 = (u16) temp_r3;
            arg0->shop.unk_848 = arg0->shop.unk_850 + arg0->shop.unk_844;
            func_ov043_020acb08(arg0);
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
            arg0->shop.unk_837 = 3;
            DebugOvlDisp_Pop();
            return;
        }
        goto block_15;
    }
    if (TouchInput_IsTouchActive() == 0) {
        func_ov043_020acf6c(arg0);
    }
block_15:
    temp_r1 = arg0->shop.unk_83A;
    if (temp_r1 != 0) {
        arg0->shop.unk_83A = temp_r1 - 1;
        if (arg0->shop.unk_83A == 0) {
            EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->taskId_WindowU);
            func_ov043_020ac6a0(arg0, arg0->shop.unk_774, arg0->shop.unk_78C, 1);
        }
    }
    temp_r1_2 = arg0->shop.unk_850;
    if (arg0->shop.unk_852 == temp_r1_2) {
        return;
    }
    func_ov043_020acbd0((s32) temp_r4, temp_r1_2);
    arg0->shop.unk_852 = arg0->shop.unk_850;
}

void func_ov043_020ae580(ShopState *arg0) {
    u16 temp_r1;

    EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->taskId_WindowU);
    temp_r1 = arg0->shop.unk_774;
    if (arg0->shop.unk_834 & 8) {
        func_ov043_020ac6a0(arg0, temp_r1, arg0->shop.unk_78C, 6);
    } else {
        func_ov043_020ac6a0(arg0, temp_r1, arg0->shop.unk_78C, 5);
    }
    func_ov043_020b1fc4(&(arg0)->shop, arg0->shop.unk_7B7, 1);
    arg0->shop.unk_83A = 0xB4;
    DebugOvlDisp_ReplaceTop(func_ov043_020ae614, arg0, 0);
}

void func_ov043_020ae614(ShopState *arg0) {
    u16 temp_r1;

    temp_r1 = arg0->shop.unk_83A;
    if (temp_r1 != 0) {
        arg0->shop.unk_83A = temp_r1 - 1;
    }
    if (TouchInput_WasTouchPressed() != 0) {
        arg0->shop.unk_83A = 0;
    }
    if (arg0->shop.unk_83A != 0) {
        return;
    }
    if (SndMgr_IsSEPlaying(0x376) != 0) {
        return;
    }
    DebugOvlDisp_Pop();
}
