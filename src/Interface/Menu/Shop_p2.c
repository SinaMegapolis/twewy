#include "ShopPart.h"

void func_ov043_020adaa4(ShopState *arg0) {
    void func_ov043_020adaf4(ShopState *);

    arg0->shop.unk_7B4 += 1;
    arg0->unk_217B0 = 0xA;
    g_DisplaySettings.controls[0].layers |= 4;
    DebugOvlDisp_ReplaceTop((OverlayCB)func_ov043_020adaf4, arg0, 0);
}

void func_ov043_020adaf4(ShopState *arg0) {
    void  func_ov043_020ad3c8(ShopState *);
    ShopObject *shop = &arg0->shop;
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
    void   func_ov043_020add48(ShopState *);
    ShopObject *shop = &arg0->shop;
    u16    temp_r2;

    arg0->unk_217B0 = 0xA;
    if ((u32) shop->unk_7A2[shop->unk_778 - 2] >= 0x3D4U) {
        shop->unk_7B8 += 1;
        shop->unk_7B5 = (u8) func_ov043_020afb14(shop);
        shop->unk_84E = 0xFFFF;
    } else {
        func_ov043_020afd6c(shop);
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
    void  func_ov043_020ad3c8(ShopState *);
    ShopObject *shop = &arg0->shop;
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
    void  func_ov043_020ade9c(ShopState *);
    ShopObject *shop = &arg0->shop;

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
    ShopObject *shop = &arg0->shop;
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
    void   func_ov043_020ae138(ShopState *);
    void   func_ov043_020adfcc(ShopState *);
    ShopObject *shop = &arg0->shop;

    if (shop->unk_834 & 4) {
        if (func_ov043_020b1780(shop, shop->unk_844) == 1) {
            func_ov043_020b16c4(shop, shop->unk_844);
            func_ov043_020acd90(arg0, shop);
            return;
        }
        DebugOvlDisp_ReplaceTop((OverlayCB)func_ov043_020ae138, arg0, 0);
        return;
    }
    if (func_ov043_020b1780(shop, shop->unk_844) == 1) {
        arg0->unk_21778 = Shop_window1_CreateTask((TaskPool *) (&(arg0)->base.taskPool), arg0->base.dataType, shop);
        g_DisplaySettings.controls[0].layers |= 4;
        DebugOvlDisp_ReplaceTop((OverlayCB)func_ov043_020adfcc, arg0, 0);
        return;
    }
    DebugOvlDisp_ReplaceTop((OverlayCB)func_ov043_020ae138, arg0, 0);
}

void func_ov043_020adfcc(ShopState *arg0) {
    void  func_ov043_020ae0b0(ShopState *);
    ShopObject *shop = &arg0->shop;
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
        func_ov043_020b16c4(shop, shop->unk_844);
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
    void  func_ov043_020ad3c8(ShopState *);
    ShopObject *shop = &arg0->shop;
    u16   temp_r1;

    temp_r1 = shop->unk_83A;
    if (temp_r1 != 0) {
        shop->unk_83A = temp_r1 - 1;
        return;
    }
    EasyTask_DeleteTask((TaskPool *) (&(arg0)->base.taskPool), (u32) arg0->unk_21778);
    g_DisplaySettings.controls[0].layers &= ~4;
    if (shop->unk_864[2] == 0) {
        func_ov043_020acd90(arg0, shop);
        return;
    }
    shop->unk_855 = 0;
    DebugOvlDisp_ReplaceTop((OverlayCB)func_ov043_020ad3c8, arg0, 0);
}

void func_ov043_020ae138(ShopState *arg0) {
    void   func_ov043_020ae1b8(ShopState *);
    ShopObject *shop = &arg0->shop;

    arg0->unk_217B0 = 0xA;
    shop->unk_85E = 4;
    arg0->unk_21774 = Shop_window0_CreateTask(&(arg0)->base.taskPool, arg0->base.dataType, shop);
    SndMgr_StartPlayingSE(0x141);
    g_DisplaySettings.controls[0].layers |= 4;
    DebugOvlDisp_ReplaceTop((OverlayCB)func_ov043_020ae1b8, arg0, 0);
}

void func_ov043_020ae1b8(ShopState *arg0) {
    void  func_ov043_020ad3c8(ShopState *);
    ShopObject *shop = &arg0->shop;
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
    void  func_ov043_020ae284(ShopState *);
    ShopObject *shop = &arg0->shop;

    shop->unk_855 = 0;
    func_ov043_020b1fc4(shop, shop->unk_7B7, 0);
    DebugOvlDisp_ReplaceTop((OverlayCB)func_ov043_020ae284, arg0, 0);
}

void func_ov043_020ae284(ShopState *arg0) {
    void      func_ov043_020ae798(ShopState *);
    ShopObject *shop = &arg0->shop;
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
            if (func_ov043_020b1a70(shop, shop->unk_83C) != 1) {
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
    void  func_ov043_020ae614(ShopState *);
    ShopObject *shop = &arg0->shop;

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
    ShopObject *shop = &arg0->shop;
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
