#include "OtosuMenuShared.h"

static PrcStepFn data_ov002_020932cc[] = {
    func_ov002_0208ffac,
    OtosuPrcStep_FadeStart_Neutral,
    OtosuPrcStep_FadeWait_Neutral,
    OtosuPrcStep_CheckButtonInput,
    OtosuPrcStep_FadeStart_BrightImmediate,
    OtosuPrcStep_FadeWait_Immediate,
    PrcStep_PopFrame,
};

PrcFrameDesc data_ov002_020932b8 = {
    .enter     = func_ov002_0208f9d0,
    .stepTable = data_ov002_020932cc,
    .update    = func_ov002_0208f9e4,
    .render    = func_ov002_0208f9e8,
    .exit      = func_ov002_0208f9d4,
};

static const u16 data_ov002_02092534[214] = {
    0x209E, 0x2093, 0x2094, 0x2095, 0x2096, 0x2097, 0x2098, 0x2099, 0x209A, 0x208E, 0x208F, 0x2090, 0x2091, 0x2092, 0xFFFF, 0,
    0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,
    0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,
    0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,
    0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,
    0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,
    0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,
    0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,
    0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,
    0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,
    0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,
    0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,
    0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,
    0,      0,      0,      0,      0,      0,
};

/* const */

void func_ov002_0208f9d0(void) {
    return;
}

void func_ov002_0208f9d4(PrcCtx* ctx, void* arg1) {
    func_ov002_02085710(arg1);
}

void func_ov002_0208f9e4(void) {
    return;
}

void func_ov002_0208f9e8(void) {
    return;
}

void func_ov002_0208f9ec(OtosuMenuObj* menuObj, void* arg1, void* arg2) {
    void* temp_r4 = menuObj->unk_48058;

    SysFont_SetMsg(temp_r4, data_ov002_02092534[menuObj->unk_474CA]);
    SysFont_SetPos(temp_r4, 0, 0);
    SysFont_SetHAlign(temp_r4, 0, 256);
    SysFont_SetVAlign(temp_r4, 0, 192);
    SysFont_DrawCurrentToScreen(temp_r4, arg2, arg1, 0);
}

void func_ov002_0208fa6c(OtosuMenuObj* menuObj, void* arg1, void* arg2) {
    u16      table_sp14[0xC8] = {0xFFFF};
    u16      table_lower[3]   = {0x20A7, 0x20A8, 0x20A9};
    u16      table_upper[3]   = {0x20A4, 0x20A3, 0x20A5};
    SysCode* lowerText;
    SysCode* upperText;
    SysCode* fmt;
    void*    temp_r7;
    u16      i;

    temp_r7   = menuObj->unk_48058;
    fmt       = SysFont_GetMsgBuf(temp_r7, SYSMSG_FONT_PAGE_FMT);
    upperText = SysFont_GetMsgBuf(temp_r7, table_upper[menuObj->unk_474C8]);
    lowerText = SysFont_GetMsgBuf(temp_r7, table_lower[menuObj->unk_474CC]);
    SysFont_Format(table_sp14, fmt, upperText, menuObj->unk_474CA + 1, 14, lowerText);
    Mem_Free(&gDebugHeap, fmt);
    Mem_Free(&gDebugHeap, upperText);
    Mem_Free(&gDebugHeap, lowerText);
    SysFont_SetMsgPtr(temp_r7, table_sp14);
    SysFont_SetPos(temp_r7, 0, 0);
    SysFont_SetHAlign(temp_r7, 0, 256);
    SysFont_SetVAlign(temp_r7, 0, 192);
    SysFont_DrawCurrentToScreen(temp_r7, arg2, arg1, 0);
}

void func_ov002_0208fbf4(OtosuMenuObj* menuObj) {
    u8*   temp_r1;
    u8*   temp_r1_4;
    s32   temp_r3;
    s32   temp_r3_2;
    u16   var_ip;
    u16   var_ip_2;
    void* temp_r0;
    void* temp_r0_2;
    void* temp_r1_2;
    void* temp_r1_3;
    void* temp_r1_5;
    void* temp_r1_6;
    void* temp_r2;
    void* temp_r2_2;
    void* temp_r2_3;
    void* temp_r2_4;
    void* temp_r2_5;
    void* temp_r2_6;
    void* temp_r2_7;
    void* var_r0;
    void* var_r0_2;
    void* var_r4;
    void* var_r4_2;
    void* var_r4_3;
    void* var_r4_4;

    func_ov002_02085710(menuObj);
    menuObj->unk_462F0 = DatMgr_LoadPackEntry(1, 0, 0, &data_ov002_02091aac, 1, 0);
    Display_SetMainLayers(LAYER_BG0);
    Display_SetSubLayers(LAYER_BG3);
    temp_r1 = SysFont_GetAllocPal(0);
    var_r4  = Data_GetPackEntryData(menuObj->unk_462F0, 4);
    var_ip  = 0;
    do {
        temp_r3                         = var_ip * 2;
        temp_r2_3                       = var_r4 + (var_ip * 2);
        *(u16*)((u8*)temp_r2_3 + 0x120) = *(u16*)(temp_r1 + temp_r3);
        var_ip += 1;
    } while ((u32)var_ip < 0x10U);
    Mem_Free(&gDebugHeap, temp_r1);
    DC_PurgeAll();
    menuObj->unk_47340 = PaletteMgr_AllocPalette(g_PaletteManagers[1], var_r4, 0, 0, 0x10);
    PaletteMgr_Flush(g_PaletteManagers[1], menuObj->unk_47340);
    var_r4_2           = Data_GetPackEntryData(menuObj->unk_462F0, 5);
    menuObj->unk_4732C = BgResMgr_AllocChar32(g_BgResourceManagers[1], var_r4_2,
                                              g_DisplaySettings.engineState[1].bgSettings[3].charBase, 0, 0x6000);
    temp_r0_2          = menuObj->unk_462F0;
    var_r0             = Data_GetPackEntryData(temp_r0_2, 6);
    menuObj->unk_474A0 = (void*)(var_r0 + 4);
    func_ov002_0208f9ec(menuObj, var_r4_2 + 4, &menuObj->unk_474A0);
    func_0200d1d8(&menuObj->unk_473C0, 1, 3, 0, &menuObj->unk_474A0, 1, 1);
    temp_r2_4 = menuObj->unk_462F0;
    temp_r1_4 = SysFont_GetAllocPal(0);
    var_r4_3  = Data_GetPackEntryData(temp_r2_4, 3);
    var_ip_2  = 0;
    do {
        temp_r3_2 = var_ip_2 * 2;
        temp_r2_6 = var_r4_3 + (var_ip_2 * 2);
        var_ip_2 += 1;
        *(u16*)((u8*)temp_r2_6 + 0x120) = *(u16*)(temp_r1_4 + temp_r3_2);
    } while ((u32)var_ip_2 < 0x10U);
    Mem_Free(&gDebugHeap, temp_r1_4);
    DC_PurgeAll();
    menuObj->unk_47344 = PaletteMgr_AllocPalette(g_PaletteManagers[0], var_r4_3, 0, 0, 0x10);
    PaletteMgr_Flush(g_PaletteManagers[0], menuObj->unk_47344);
    var_r4_4 = Data_GetPackEntryData(menuObj->unk_462F0, 7);
    if (menuObj->unk_462F0 == NULL) {
        var_r0_2 = NULL;
    } else {
        var_r0_2 = Data_GetPackEntryData(menuObj->unk_462F0, 8);
    }
    menuObj->unk_474A8 = (void*)(var_r0_2 + 4);
    menuObj->unk_47330 = BgResMgr_AllocChar32(g_BgResourceManagers[0], var_r4_4,
                                              g_DisplaySettings.engineState[0].bgSettings[0].charBase, 0, 0x6000);
    func_ov002_0208fa6c(menuObj, var_r4_4 + 4, &menuObj->unk_474A8);
    func_0200d1d8(&menuObj->unk_473E8, 0, 0, 0, &menuObj->unk_474A8, 1, 1);
    Display_Commit();
}

void func_ov002_0208ff0c(void* arg0) {
    func_ov002_0208fbf4(arg0);
}

void func_ov002_0208ff18(OtosuMenuObj* menuObj) {
    u8 colors[3] = {0x0E, 0x0C, 0x0A};

    SysFont_SetColor(&menuObj->font, colors[menuObj->unk_474CC]);
}

void func_ov002_0208ff6c(OtosuMenuObj* menuObj) {
    void* fonts[3];

    fonts[0]           = &menuObj->font;
    menuObj->unk_48058 = fonts[menuObj->unk_474C8];
}

PrcStepResult func_ov002_0208ffac(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    menuObj->unk_48058 = &menuObj->font;
    SysFont_SetColor(&menuObj->font, 14);

    Display_SetMainLayers(LAYER_BG0);
    Display_SetSubLayers(LAYER_BG2);
    Display_Commit();
    func_ov002_0208ff6c(menuObj);
    func_ov002_0208fbf4(menuObj);
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult OtosuPrcStep_CheckButtonInput(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    BOOL buttonPressed = FALSE;

    if (SysControl.buttonState.pressedButtons & INPUT_BUTTON_A) {
        buttonPressed = TRUE;
        menuObj->unk_474CA++;
        menuObj->unk_474CA %= (u16)14;
    }

    if (SysControl.buttonState.pressedButtons & INPUT_BUTTON_B) {
        if (menuObj->unk_474CA == 0) {
            menuObj->unk_474CA = 13;
        } else {
            menuObj->unk_474CA--;
        }
        buttonPressed = TRUE;
    }

    if (SysControl.buttonState.pressedButtons & INPUT_BUTTON_LEFT) {
        if (menuObj->unk_474C8 == 0) {
            menuObj->unk_474C8 = 0;
        } else {
            menuObj->unk_474C8--;
        }
        buttonPressed = TRUE;
    }

    if (SysControl.buttonState.pressedButtons & INPUT_BUTTON_RIGHT) {
        if (menuObj->unk_474C8 == 0) {
            menuObj->unk_474C8 = 0;
        } else {
            menuObj->unk_474C8++;
        }
        buttonPressed = TRUE;
    }

    if (SysControl.buttonState.pressedButtons & INPUT_BUTTON_L) {
        if (menuObj->unk_474CC == 0) {
            menuObj->unk_474CC = 2;
        } else {
            menuObj->unk_474CC--;
        }
        buttonPressed = TRUE;
    }

    if (SysControl.buttonState.pressedButtons & INPUT_BUTTON_R) {
        if (menuObj->unk_474CC == 2) {
            menuObj->unk_474CC = 0;
        } else {
            menuObj->unk_474CC++;
        }
        buttonPressed = TRUE;
    }

    if (buttonPressed) {
        func_ov002_0208ff18(menuObj);
        func_ov002_0208ff6c(menuObj);
        func_ov002_0208ff0c(menuObj);
    }

    if (SysControl.buttonState.pressedButtons & INPUT_BUTTON_START) {
        PrcCtx_AdvanceStep(ctx);
    }

    return PRC_STEP_CONTINUE;
}
