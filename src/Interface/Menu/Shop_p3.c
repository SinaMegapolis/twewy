#include "ShopPart.h"

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
