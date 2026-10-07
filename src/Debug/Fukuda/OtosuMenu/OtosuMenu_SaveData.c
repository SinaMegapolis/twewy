#include "OtosuMenuShared.h"

static PrcStepFn data_ov002_020932f0[] = {func_ov002_02091510, OtosuPrcStep_FadeStart_Neutral2, OtosuPrcStep_FadeWait_Neutral2,
                                          func_ov002_02091710};

static PrcStepFn data_ov002_02093300[] = {func_ov002_02091608, OtosuPrcStep_FadeStart_Neutral2, OtosuPrcStep_FadeWait_Neutral2,
                                          func_ov002_02091700};
static const OtosuMenuRectList1 sNoText5 = {
    {
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0},
     }
};
static const OtosuMenuRectList1 sNoText3 = {
    {
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0},
     }
};
static const OtosuMenuRectList1 sNoText2 = {
    {
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0},
     }
};
static const OtosuMenuRectList1 sNoText7 = {
    {
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0},
     }
};
static const OtosuMenuRectList1 sNoText0 = {
    {
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0},
     }
};
static const OtosuMenuRectList1 sNoText6 = {
    {
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0},
     }
};
static const OtosuMenuRectList1 sNoText1 = {
    {
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0},
     }
};
static const OtosuMenuRectList1 sNoText4 = {
    {
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0},
     }
};
static const OtosuMenuRectList2 data_ov002_0209296c = {
    {
     {0x2A37, 0x0010, 0x0050, 0x00F0, 0x0098},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000},
     }
};
static const OtosuMenuRectList2 data_ov002_02092908 = {
    {
     {0x2A38, 0x0010, 0x0050, 0x00F0, 0x0098},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000},
     }
};

static PrcStepFn data_ov002_02093338[] = {
    func_ov002_02091110,
    OtosuPrcStep_FadeStart_Neutral2,
    OtosuPrcStep_FadeWait_Neutral2,
    func_ov002_02090ee8,
    func_ov002_02090f4c,
    OtosuPrcStep_FadeStart_DarkImmediate,
    OtosuPrcStep_FadeWait_Immediate,
    func_ov002_02091300,
    OtosuPrcStep_FadeStart_Neutral,
    OtosuPrcStep_FadeWait_Neutral,
    func_ov002_020914b0,
    OtosuPrcStep_FadeStart_Dark,
    OtosuPrcStep_FadeWait,
    func_ov002_02091720,
};

static PrcStepFn data_ov002_02093370[] = {
    func_ov002_02091208,
    OtosuPrcStep_FadeStart_Neutral2,
    OtosuPrcStep_FadeWait_Neutral2,
    func_ov002_02090ee8,
    func_ov002_02090f4c,
    OtosuPrcStep_FadeStart_DarkImmediate,
    OtosuPrcStep_FadeWait_Immediate,
    func_ov002_020913d8,
    OtosuPrcStep_FadeStart_Neutral,
    OtosuPrcStep_FadeWait_Neutral,
    func_ov002_020914b0,
    OtosuPrcStep_FadeStart_Dark,
    OtosuPrcStep_FadeWait,
    func_ov002_02091720,
};

static PrcStepFn data_ov002_020933a8[] = {
    func_ov002_02090b44,
    OtosuPrcStep_FadeStart_Neutral2,
    OtosuPrcStep_FadeWait_Neutral2,
    func_ov002_0209095c,
    PrcStep_Advance,
    OtosuPrcStep_FadeStart_DarkImmediate,
    OtosuPrcStep_FadeWait_Immediate,
    func_ov002_02090c28,
    OtosuPrcStep_FadeStart_Neutral,
    OtosuPrcStep_FadeWait_Neutral,
    func_ov002_0209095c,
    PrcStep_Advance,
    OtosuPrcStep_FadeStart_DarkImmediate,
    OtosuPrcStep_FadeWait_Immediate,
    func_ov002_02090d0c,
    OtosuPrcStep_FadeStart_Neutral,
    OtosuPrcStep_FadeWait_Neutral,
    func_ov002_0209095c,
    func_ov002_02090b30,
    OtosuPrcStep_FadeStart_DarkImmediate,
    OtosuPrcStep_FadeWait_Immediate,
    func_ov002_02090ee8,
    func_ov002_02090df0,
    OtosuPrcStep_FadeStart_Neutral,
    OtosuPrcStep_FadeWait_Neutral,
    func_ov002_02090f4c,
    OtosuPrcStep_FadeStart_DarkImmediate,
    OtosuPrcStep_FadeWait_Immediate,
    func_ov002_02090fd8,
    OtosuPrcStep_FadeStart_Neutral,
    OtosuPrcStep_FadeWait_Neutral,
    func_ov002_020910b0,
    OtosuPrcStep_FadeStart_Dark,
    OtosuPrcStep_FadeWait,
    func_ov002_02091720,
    PrcStep_PopFrame,
};

static const SpriteAnimation data_ov002_02092a20 = {
    .bits_0_1          = 0,
    .dataType          = 0,
    .bit_6             = 0,
    .bits_7_9          = 5,
    .bits_10_11        = 0,
    .bits_12_13        = 2,
    .bits_14_15        = 0,
    .unk_02            = 0x0000,
    .posX              = 0x0000,
    .posY              = 0x00C8,
    .frameInfoCallback = func_ov002_020901a0,
    .callbackArg       = 0,
    .owner             = NULL,
    .binIden           = (BinIdentifier*)&data_ov002_02091c34,
    .unk_18            = 0x0002,
    .packIndex         = 0x000C,
    .unk_1C            = 0x0002,
    .unk_1E            = 0x0000,
    .unk_20            = 0x0001,
    .unk_22            = 0x0002,
    .unk_24            = 0x0000,
    .unk_26            = 0x0003,
    .unk_28            = 0x0004,
    .animIndex         = 0x0003,
};

static PrcStepFn data_ov002_020932e8[] = {func_ov002_02091720, PrcStep_PopFrame};

PrcFrameDesc data_ov002_02093310 = {
    .enter     = func_ov002_020902a4,
    .stepTable = data_ov002_02093300,
    .update    = func_ov002_0209034c,
    .render    = func_ov002_02090350,
    .exit      = func_ov002_02090310,
};

PrcFrameDesc data_ov002_0209344c = {
    .enter     = func_ov002_020902a4,
    .stepTable = data_ov002_020933a8,
    .update    = func_ov002_0209034c,
    .render    = func_ov002_02090350,
    .exit      = func_ov002_02090310,
};
static const OtosuMenuRectList3 data_ov002_02092a02 = {
    {
     {0x2A39, 0x38, 0x51, 0xC8, 0x5B},
     {0x2A3A, 0x38, 0x65, 0xC8, 0x6F},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0},
     }
};
PrcFrameDesc data_ov002_02093324 = {
    .enter     = func_ov002_020902a4,
    .stepTable = data_ov002_020932f0,
    .update    = func_ov002_0209034c,
    .render    = func_ov002_02090350,
    .exit      = func_ov002_02090310,
};
static const OtosuMenuRectList2 data_ov002_020928cc = {
    {
     {0x2A36, 0x0010, 0x0050, 0x00F0, 0x0098},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000},
     }
};
static const OtosuMenuRectList2 data_ov002_020928e0 = {
    {
     {0x2A35, 0x0010, 0x0050, 0x00F0, 0x0098},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000},
     }
};
static const OtosuMenuRectList2 data_ov002_020928f4 = {
    {
     {0x2A41, 0x0010, 0x0050, 0x00F0, 0x0098},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000},
     }
};
static const OtosuMenuRectList2 data_ov002_0209291c = {
    {
     {0x2A42, 0x0010, 0x0050, 0x00F0, 0x0098},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000},
     }
};
static const OtosuMenuRectList2 data_ov002_02092944 = {
    {
     {0x2A3F, 0x0010, 0x0050, 0x00F0, 0x0098},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000},
     }
};
static const OtosuMenuRectList2 data_ov002_02092958 = {
    {
     {0x2A40, 0x0010, 0x0050, 0x00F0, 0x0098},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000},
     }
};
PrcFrameDesc data_ov002_02093460 = {
    .enter     = func_ov002_020902a4,
    .stepTable = data_ov002_02093338,
    .update    = func_ov002_0209034c,
    .render    = func_ov002_02090350,
    .exit      = func_ov002_02090310,
};
static const OtosuMenuRectList2 data_ov002_02092980 = {
    {
     {0x3384, 0x0010, 0x0050, 0x00F0, 0x0098},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000},
     }
};
PrcFrameDesc data_ov002_02093438 = {
    .enter     = func_ov002_020902a4,
    .stepTable = data_ov002_02093370,
    .update    = func_ov002_0209034c,
    .render    = func_ov002_02090350,
    .exit      = func_ov002_02090310,
};
static const OtosuMenuRectList2 data_ov002_02092930 = {
    {
     {0x3383, 0x0010, 0x0050, 0x00F0, 0x0098},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000},
     }
};
static const OtosuMenuRectList3 data_ov002_020929c6 = {
    {
     {0x2A3D, 0x38, 0x51, 0xC8, 0x5B},
     {0x2A3E, 0x38, 0x65, 0xC8, 0x6F},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0},
     }
};
static const OtosuMenuRectList2 data_ov002_02092994 = {
    {
     {0x2A34, 0x0010, 0x0050, 0x00F0, 0x0098},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000},
     }
};
static const OtosuMenuRectList3 data_ov002_020929a8 = {
    {
     {0x2A3B, 0x38, 0x51, 0xC8, 0x5B},
     {0x2A3C, 0x38, 0x65, 0xC8, 0x6F},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0},
     }
};

SpriteFrameInfo* func_ov002_020901a0(Sprite* sprite, s32 arg, s32 mode) {
    Sprite_FrameInfoCallback(sprite, mode);
}

static const OtosuMenuRectList3 data_ov002_020929e4 = {
    {
     {0, 0x38, 0x4C, 0xC8, 0x60},
     {1, 0x38, 0x61, 0xC8, 0x74},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF},
     }
};

void func_ov002_0209023c(OtosuMenuObj* menuObj) {
    SpriteAnimation anim = data_ov002_02092a20;

    _Sprite_Load(&menuObj->cursor, &anim);
    Sprite_ChangeAnimation(&menuObj->cursor, menuObj->cursor.animData, 3, menuObj->cursor.cellTable);
}

void func_ov002_020902a4(s32 arg0, OtosuMenuObj* menuObj) {
    PrcCtx_Init(&menuObj->objCtx[0], "DelData_WinObj", 0x100);
    PrcCtx_ReplaceFrame(&menuObj->objCtx[0], &OtosuMenu_DelDataWin_FrameDesc, NULL);
    PrcMaster_RegisterContext(&menuObj->prcMaster, &menuObj->objCtx[0]);
    Sprite_Destroy(&menuObj->cursor);
    func_ov002_0209023c(menuObj);
}

void func_ov002_02090310(s32 arg0, OtosuMenuObj* menuObj) {
    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->objCtx[0]);
    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->objCtx[1]);
}

void func_ov002_0209034c(void) {
    return;
}

void func_ov002_02090350(void) {
    return;
}

void func_ov002_02090354(OtosuMenuObj* menuObj) {
    u16   col;
    u16   row;
    u16   tile;
    void* textChars;

    if (menuObj->mainChars[0] != NULL) {
        BgResMgr_ReleaseChar(g_BgResourceManagers[0], menuObj->mainChars[0]);
        menuObj->mainChars[0] = NULL;
    }
    if (menuObj->packs[0] != NULL) {
        DatMgr_ReleaseData(menuObj->packs[0]);
        menuObj->packs[0] = NULL;
    }
    menuObj->packs[0] = DatMgr_LoadPackEntry(1, 0, 0, &data_ov002_02091aac, 1, 0);
    textChars         = Data_GetPackEntryData(menuObj->packs[0], 7);
    tile              = 0;
    for (row = 0; row < 32; row++) {
        for (col = 0; col < 64; col++) {
            menuObj->tilemap[row][col] = tile | 0x1000;
            tile++;
        }
    }
    DC_PurgeAll();
    menuObj->mainScreens[0][0] = menuObj->tilemap;
    menuObj->mainChars[0]      = BgResMgr_AllocChar32(g_BgResourceManagers[0], textChars,
                                                      g_DisplaySettings.engineState[0].bgSettings[0].charBase, 0, 0x6000);
    func_0200d1d8(&menuObj->mainMaps[0], 0, 0, 0, menuObj->mainScreens[0], 1, 1);
    Display_Commit();
}

void func_ov002_020904cc(OtosuMenuObj* menuObj, const OtosuMenuRect* subTexts, const OtosuMenuRect* mainTexts) {
    u16   col;
    u16   row;
    u16   tile;
    void* chars;
    void* screen;
    void* textChars;

    func_ov002_02085710(menuObj);
    menuObj->packs[0] = DatMgr_LoadPackEntry(1, 0, 0, &data_ov002_02091aac, 1, 0);
    menuObj->packs[1] = DatMgr_LoadPackEntry(1, 0, 0, &data_ov002_02091c1c, 1, 0);
    menuObj->packs[2] = DatMgr_LoadRawData(1, 0, 0, &data_ov002_02091c2c);
    menuObj->packs[3] = DatMgr_LoadRawData(1, 0, 0, &data_ov002_02091c24);

    Display_SetMainLayers(LAYER_BG0 | LAYER_BG3 | LAYER_OBJ);
    Display_SetSubLayers(LAYER_BG2 | LAYER_BG3 | LAYER_OBJ);

    menuObj->subPalette =
        PaletteMgr_AllocPalette(g_PaletteManagers[1], Data_GetPackEntryData(menuObj->packs[1], 1), 0, 0, 0x10);
    PaletteMgr_Flush(g_PaletteManagers[1], menuObj->subPalette);

    chars                     = Data_GetPackEntryData(menuObj->packs[2], 1);
    screen                    = Data_GetPackEntryData(menuObj->packs[2], 2);
    menuObj->subScreens[3][0] = screen + 4;
    menuObj->subChars[3]      = BgResMgr_AllocChar32(g_BgResourceManagers[1], chars,
                                                     g_DisplaySettings.engineState[1].bgSettings[3].charBase, 0, 0x6000);
    func_0200d1d8(&menuObj->subMaps[3], 1, 3, 0, menuObj->subScreens[3], 1, 1);

    chars                     = Data_GetPackEntryData(menuObj->packs[1], 2);
    screen                    = Data_GetPackEntryData(menuObj->packs[1], 3);
    menuObj->subScreens[2][0] = screen + 4;
    func_ov002_02082dbc(&menuObj->font, subTexts, chars + 4, menuObj->subScreens[2][0]);
    menuObj->subChars[2] = BgResMgr_AllocChar32(g_BgResourceManagers[1], chars,
                                                g_DisplaySettings.engineState[1].bgSettings[2].charBase, 0, 0x4380);
    func_0200d1d8(&menuObj->subMaps[2], 1, 2, 0, menuObj->subScreens[2], 1, 1);

    menuObj->mainPalette =
        PaletteMgr_AllocPalette(g_PaletteManagers[0], Data_GetPackEntryData(menuObj->packs[1], 1), 0, 0, 0x10);
    PaletteMgr_Flush(g_PaletteManagers[0], menuObj->mainPalette);

    chars                      = Data_GetPackEntryData(menuObj->packs[3], 1);
    screen                     = Data_GetPackEntryData(menuObj->packs[3], 2);
    menuObj->mainScreens[3][0] = screen + 4;
    menuObj->mainChars[3]      = BgResMgr_AllocChar32(g_BgResourceManagers[0], chars,
                                                      g_DisplaySettings.engineState[0].bgSettings[3].charBase, 0, 0x6000);
    func_0200d1d8(&menuObj->mainMaps[3], 0, 3, 0, menuObj->mainScreens[3], 1, 1);

    textChars = Data_GetPackEntryData(menuObj->packs[0], 7);
    tile      = 0;
    for (row = 0; row < 32; row++) {
        for (col = 0; col < 64; col++) {
            menuObj->tilemap[row][col] = tile | 0x1000;
            tile++;
        }
    }
    DC_PurgeAll();
    menuObj->mainScreens[0][0] = menuObj->tilemap;
    menuObj->mainChars[0]      = BgResMgr_AllocChar32(g_BgResourceManagers[0], textChars,
                                                      g_DisplaySettings.engineState[0].bgSettings[0].charBase, 0, 0x6000);
    func_ov002_02082dbc(&menuObj->font, mainTexts, textChars + 4, menuObj->mainScreens[0][0]);
    func_0200d1d8(&menuObj->mainMaps[0], 0, 0, 0, menuObj->mainScreens[0], 1, 1);
    Display_Commit();
}

PrcStepResult func_ov002_0209095c(PrcCtx* ctx, void* arg1) {
    OtosuMenuObj*      menuObj    = arg1;
    OtosuMenuRectList3 buttons    = data_ov002_020929e4;
    OtosuMenuCursor    cursors[2] = {
        {3, 0x80, 0x56},
        {3, 0x80, 0x6A},
    };
    u16 selected = func_ov002_0208597c(buttons.rects);

    if (selected == 0xFFFF) {
        return PRC_STEP_CONTINUE;
    }
    if (selected == 0xFFFE) {
        menuObj->unk_474C8   = 0xFFFF;
        menuObj->cursor.posX = 0;
        menuObj->cursor.posY = 210;
        return PRC_STEP_CONTINUE;
    }
    if (menuObj->unk_474C8 == selected) {
        SndMgr_StartPlayingSE(SEIDX_MENU_MSYSTEM_EXECUTE);
        switch (selected) {
            case 0:
                PrcCtx_AdvanceStep(ctx);
                return PRC_STEP_CONTINUE;
            case 1:
                PrcCtx_ReplaceStepTable(ctx, data_ov002_020932e8);
                func_ov002_02090354(menuObj);
                menuObj->cursor.posX = 0;
                menuObj->cursor.posY = 210;
                PrcCtx_PushStepTable(ctx, PrcSteps_FadeDark);
                return PRC_STEP_CONTINUE;
        }
    } else {
        menuObj->unk_474C8 = selected;
        SndMgr_StartPlayingSE(SEIDX_MENU_MSYSTEM_CURSOR);
        switch (selected) {
            case 0:
            case 1:
                menuObj->cursor.posX = cursors[selected].x;
                menuObj->cursor.posY = cursors[selected].y;
                Sprite_SetAnimation(&menuObj->cursor, menuObj->cursor.animData, cursors[selected].anim,
                                    menuObj->cursor.cellTable);
                return PRC_STEP_CONTINUE;
        }
    }
    return PRC_STEP_REPEAT;
}

PrcStepResult func_ov002_02090b30(PrcCtx* ctx, void*) {
    PrcCtx_AdvanceStep(ctx);
    Savefile_ResetIOPipeline();
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02090b44(PrcCtx* ctx, void* arg1) {
    OtosuMenuObj*      menuObj    = (OtosuMenuObj*)arg1;
    OtosuMenuRectList2 table_sp1E = data_ov002_02092994;
    OtosuMenuRectList3 table_sub  = data_ov002_02092a02;
    u16                i;

    Display_SetMainLayers(LAYER_BG0 | LAYER_OBJ);
    Display_SetSubLayers(LAYER_BG2 | LAYER_OBJ);
    Display_Commit();
    func_ov002_020904cc(menuObj, table_sp1E.rects, table_sub.rects);
    menuObj->cursor.posX = 0;
    menuObj->cursor.posY = 0xD2;
    menuObj->unk_474C8   = 0xFFFF;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02090c28(PrcCtx* ctx, void* arg1) {
    OtosuMenuObj*      menuObj    = (OtosuMenuObj*)arg1;
    OtosuMenuRectList2 table_sp1E = data_ov002_020928e0;
    OtosuMenuRectList3 table_sub  = data_ov002_020929a8;
    u16                i;

    Display_SetMainLayers(LAYER_BG0 | LAYER_OBJ);
    Display_SetSubLayers(LAYER_BG2 | LAYER_OBJ);
    Display_Commit();
    func_ov002_020904cc(menuObj, table_sp1E.rects, table_sub.rects);
    menuObj->unk_474C8   = 0xFFFF;
    menuObj->cursor.posX = 0;
    menuObj->cursor.posY = 0xD2;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02090d0c(PrcCtx* ctx, void* arg1) {
    OtosuMenuObj*      menuObj    = (OtosuMenuObj*)arg1;
    OtosuMenuRectList2 table_sp1E = data_ov002_020928cc;
    OtosuMenuRectList3 table_sub  = data_ov002_020929c6;
    u16                i;

    Display_SetMainLayers(LAYER_BG0 | LAYER_OBJ);
    Display_SetSubLayers(LAYER_BG2 | LAYER_OBJ);
    Display_Commit();
    func_ov002_020904cc(menuObj, table_sp1E.rects, table_sub.rects);
    menuObj->unk_474C8   = 0xFFFF;
    menuObj->cursor.posX = 0;
    menuObj->cursor.posY = 0xD2;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02090df0(PrcCtx* ctx, void* arg1) {
    OtosuMenuObj*      menuObj   = (OtosuMenuObj*)arg1;
    OtosuMenuRectList2 table_spA = data_ov002_0209296c;
    OtosuMenuRectList1 table_sub = sNoText0;
    u16                i;

    Display_SetMainLayers(LAYER_BG0 | LAYER_OBJ);
    Display_SetSubLayers(LAYER_BG2 | LAYER_OBJ);
    Display_Commit();
    func_ov002_020904cc(menuObj, table_spA.rects, table_sub.rects);
    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->objCtx[0]);
    menuObj->cursor.posX = 0;
    menuObj->cursor.posY = 0xD2;
    menuObj->unk_474C8   = 0xFFFF;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02090ee8(PrcCtx* ctx, void* arg1) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)arg1;

    PrcCtx_Init(&menuObj->objCtx[1], "OtosuMenu_Icon2", 0x40);
    PrcCtx_ReplaceFrame(&menuObj->objCtx[1], &OtosuMenu_Icon2_FrameDesc, NULL);
    PrcMaster_RegisterContext(&menuObj->prcMaster, &menuObj->objCtx[1]);
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02090f4c(PrcCtx* ctx, void* arg1) {
    s32 temp_r4;

    if (Savefile_RunSavePipelineStep() == 1) {
        temp_r4 = Savefile_GetWriteErrorFlags();
        PrcMaster_UnregisterContext(arg1 + 0x41804, arg1 + 0x476D0);
        if (temp_r4 == 0) {
            SndMgr_StartPlayingSE(SEIDX_MENU_SAVE);
            PrcCtx_AdvanceStep(ctx);
        } else if (temp_r4 & 2) {
            PrcCtx_ReplaceFrame(ctx, &data_ov002_02093310, 0);
        } else {
            PrcCtx_ReplaceFrame(ctx, &data_ov002_02093324, 0);
        }
    }
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02090fd8(PrcCtx* ctx, void* arg1) {
    OtosuMenuObj*      menuObj   = (OtosuMenuObj*)arg1;
    OtosuMenuRectList2 table_spA = data_ov002_02092908;
    OtosuMenuRectList1 table_sub = sNoText1;
    u16                i;

    Display_SetMainLayers(LAYER_BG0 | LAYER_OBJ);
    Display_SetSubLayers(LAYER_BG2 | LAYER_OBJ);
    Display_Commit();
    func_ov002_020904cc(menuObj, table_spA.rects, table_sub.rects);
    menuObj->cursor.posX = 0;
    menuObj->cursor.posY = 0xD2;
    menuObj->unk_474C8   = 0xF0;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_020910b0(PrcCtx* ctx, void* arg1) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)arg1;
    if (TouchInput_WasTouchPressed() != 0) {
        SndMgr_StartPlayingSE(SEIDX_MENU_MSYSTEM_EXECUTE);
    }
    if ((TouchInput_WasTouchPressed() != 0) || (menuObj->unk_474C8 == 0)) {
        PrcCtx_AdvanceStep(ctx);
    }
    menuObj->unk_474C8--;
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02091110(PrcCtx* ctx, void* arg1) {
    OtosuMenuObj*      menuObj   = (OtosuMenuObj*)arg1;
    OtosuMenuRectList2 table_spA = data_ov002_02092944;
    OtosuMenuRectList1 table_sub = sNoText2;
    u16                i;

    Display_SetMainLayers(LAYER_BG0 | LAYER_OBJ);
    Display_SetSubLayers(LAYER_BG2 | LAYER_OBJ);
    Display_Commit();
    func_ov002_020904cc(menuObj, table_spA.rects, table_sub.rects);
    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->objCtx[0]);
    menuObj->cursor.posX = 0;
    menuObj->cursor.posY = 0xD2;
    menuObj->unk_474C8   = 0xFFFF;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02091208(PrcCtx* ctx, void* arg1) {
    OtosuMenuObj*      menuObj   = (OtosuMenuObj*)arg1;
    OtosuMenuRectList2 table_spA = data_ov002_02092930;
    OtosuMenuRectList1 table_sub = sNoText3;
    u16                i;

    Display_SetMainLayers(LAYER_BG0 | LAYER_OBJ);
    Display_SetSubLayers(LAYER_BG2 | LAYER_OBJ);
    Display_Commit();
    func_ov002_020904cc(menuObj, table_spA.rects, table_sub.rects);
    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->objCtx[0]);
    menuObj->cursor.posX = 0;
    menuObj->cursor.posY = 0xD2;
    menuObj->unk_474C8   = 0xFFFF;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02091300(PrcCtx* ctx, void* arg1) {
    OtosuMenuObj*      menuObj   = (OtosuMenuObj*)arg1;
    OtosuMenuRectList2 table_spA = data_ov002_02092958;
    OtosuMenuRectList1 table_sub = sNoText4;
    u16                i;

    Display_SetMainLayers(LAYER_BG0 | LAYER_OBJ);
    Display_SetSubLayers(LAYER_BG2 | LAYER_OBJ);
    Display_Commit();
    func_ov002_020904cc(menuObj, table_spA.rects, table_sub.rects);
    menuObj->cursor.posX = 0;
    menuObj->cursor.posY = 0xD2;
    menuObj->unk_474C8   = 0xF0;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_020913d8(PrcCtx* ctx, void* arg1) {
    OtosuMenuObj*      menuObj   = (OtosuMenuObj*)arg1;
    OtosuMenuRectList2 table_spA = data_ov002_02092980;
    OtosuMenuRectList1 table_sub = sNoText5;
    u16                i;

    Display_SetMainLayers(LAYER_BG0 | LAYER_OBJ);
    Display_SetSubLayers(LAYER_BG2 | LAYER_OBJ);
    Display_Commit();
    func_ov002_020904cc(menuObj, table_spA.rects, table_sub.rects);
    menuObj->cursor.posX = 0;
    menuObj->cursor.posY = 0xD2;
    menuObj->unk_474C8   = 0xF0;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_020914b0(PrcCtx* ctx, void* arg1) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)arg1;
    if (TouchInput_WasTouchPressed() != 0) {
        SndMgr_StartPlayingSE(SEIDX_MENU_MSYSTEM_EXECUTE);
    }
    if ((TouchInput_WasTouchPressed() != 0) || (menuObj->unk_474C8 == 0)) {
        PrcCtx_AdvanceStep(ctx);
    }
    menuObj->unk_474C8--;
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02091510(PrcCtx* ctx, void* arg1) {
    OtosuMenuObj*      menuObj   = (OtosuMenuObj*)arg1;
    OtosuMenuRectList2 table_spA = data_ov002_0209291c;
    OtosuMenuRectList1 table_sub = sNoText6;
    u16                i;

    Display_SetMainLayers(LAYER_BG0 | LAYER_OBJ);
    Display_SetSubLayers(LAYER_BG2 | LAYER_OBJ);
    Display_Commit();
    func_ov002_020904cc(menuObj, table_spA.rects, table_sub.rects);
    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->objCtx[0]);
    menuObj->cursor.posX = 0;
    menuObj->cursor.posY = 210;
    menuObj->unk_474C8   = 0xFFFF;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02091608(PrcCtx* ctx, void* arg1) {
    OtosuMenuObj*      menuObj   = (OtosuMenuObj*)arg1;
    OtosuMenuRectList2 table_spA = data_ov002_020928f4;
    OtosuMenuRectList1 table_sub = sNoText7;
    u16                i;

    Display_SetMainLayers(LAYER_BG0 | LAYER_OBJ);
    Display_SetSubLayers(LAYER_BG2 | LAYER_OBJ);
    Display_Commit();
    func_ov002_020904cc(menuObj, table_spA.rects, table_sub.rects);
    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->objCtx[0]);
    menuObj->cursor.posX = 0;
    menuObj->cursor.posY = 210;
    menuObj->unk_474C8   = 0xFFFF;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02091700(PrcCtx* ctx, void* arg1) {
    OS_WaitForever();
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02091710(PrcCtx* ctx, void* arg1) {
    OS_WaitForever();
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02091720(PrcCtx* ctx, void* arg1) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)arg1;
    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->objCtx[0]);
    func_ov002_02085710(menuObj);
    menuObj->nextScene = 6;
    return PRC_STEP_CONTINUE;
}
