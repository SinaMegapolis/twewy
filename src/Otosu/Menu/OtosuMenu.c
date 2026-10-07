#include "OtosuMenuShared.h"

static char                     data_ov002_02092bb0[32] = "Apl_Kit/GRP_FldDownScreen.bin";
static const OtosuMenuRectList4 data_ov002_02091b64     = {
    {
     {0x23FB, 0x0017, 0x0049, 0x0049, 0x0055},
     {0x23FC, 0x0067, 0x0049, 0x0098, 0x0055},
     {0x23FD, 0x00B7, 0x0049, 0x00E9, 0x0055},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000},
     }
};

static const OverlayProcess OvlProc_OtosuMenu_MultiplayerRanking = {
    .init = OtosuMenu_InitForMultiplayerRankings,
    .main = OtosuMenu_Update,
    .exit = OtosuMenu_Destroy,
};

static const OverlayProcess OvlProc_OtosuMenu_SinglePlayerRanking = {
    .init = OtosuMenu_InitForSinglePlayerRankings,
    .main = OtosuMenu_Update,
    .exit = OtosuMenu_Destroy,
};

static const OtosuMenuBoardEntries data_ov002_02091ac4 = {
    {0x10, 0x11, 0x12, 0x13}
};
static const SpriteAnimation data_ov002_02091b8c = {
    .bits_0_1          = 0,
    .dataType          = 0,
    .bit_6             = 0,
    .bits_7_9          = 5,
    .bits_10_11        = 0,
    .bits_12_13        = 1,
    .bits_14_15        = 0,
    .unk_02            = 0x0000,
    .posX              = -13,
    .posY              = 0x000C,
    .frameInfoCallback = func_ov002_020824ac,
    .callbackArg       = 0,
    .owner             = NULL,
    .binIden           = (BinIdentifier*)&data_ov002_02091acc,
    .unk_18            = 0x0000,
    .packIndex         = 0x0000,
    .unk_1C            = 0x0015,
    .unk_1E            = 0x0000,
    .unk_20            = 0x0018,
    .unk_22            = 0x0002,
    .unk_24            = 0x0000,
    .unk_26            = 0x0016,
    .unk_28            = 0x0017,
    .animIndex         = 0x0001,
};
static u8                          data_ov002_020934fa[0xC6];
static const OtosuMenuBoardEntries data_ov002_02091ab4 = {
    {0x04, 0x18, 0x2C, 0x40}
};

static const OtosuMenuBoardEntries data_ov002_02091abc = {
    {1, 2, 3, 4}
};

const BinIdentifier         data_ov002_02091aac                 = {0x02, "Apl_Fuk/Grp_OtosuMenu.bin"};
static const OverlayProcess OvlProc_OtosuMenu_SinglePlayerEnter = {
    .init = OtosuMenu_InitForSinglePlayerEnter,
    .main = OtosuMenu_Update,
    .exit = OtosuMenu_Destroy,
};
static const OtosuMenuBoardEntries data_ov002_02091aa4 = {
    {0xC, 0xD, 0xE, 0xF}
};

const BinIdentifier              data_ov002_02091acc = {2, "Apl_Fuk/Grp_OtosuMenuObj.bin"};
static const OtosuMenuRectList10 data_ov002_02091bb8 = {
    {
     {0x23FE, 0x0010, 0x0062, 0x0050, 0x0072},
     {0x23FF, 0x0010, 0x0082, 0x0050, 0x0092},
     {0x2400, 0x0010, 0x00A2, 0x0050, 0x00B2},
     {0x2401, 0x0060, 0x0062, 0x00A0, 0x0072},
     {0x2402, 0x0060, 0x0082, 0x00A0, 0x0092},
     {0x2403, 0x0060, 0x00A2, 0x00A0, 0x00B2},
     {0x2404, 0x00B0, 0x0062, 0x00F0, 0x0072},
     {0x2405, 0x00B0, 0x0082, 0x00F0, 0x0092},
     {0x2406, 0x00B0, 0x00A2, 0x00F0, 0x00B2},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000},
     }
};

static u16                  data_ov002_020934e4[11];
const BinIdentifier         data_ov002_02091c1c             = {0x02, "Apl_Fuk/Grp_DelMenu.bin"}; /* const */
static const OverlayProcess OvlProc_OtosuMenu_RoleSelection = {
    .init = OtosuMenu_InitForRoleSelection,
    .main = OtosuMenu_Update,
    .exit = OtosuMenu_Destroy,
};
PrcStepFn PrcSteps_FadeBrightImmediate[] = {
    OtosuPrcStep_FadeStart_BrightImmediate,
    OtosuPrcStep_FadeWait_Immediate,
    PrcStep_PopFrame,
};
PrcStepFn PrcSteps_FadeBright[] = {
    OtosuPrcStep_FadeStart_Bright,
    OtosuPrcStep_FadeWait,
    PrcStep_PopFrame,
};

u8* func_ov002_020824a0(void) {
    return data_ov002_020934fa;
}

SpriteFrameInfo* func_ov002_020824ac(Sprite* sprite, s32 arg, s32 mode) {
    Sprite_FrameInfoCallback(sprite, mode);
}

static s32 data_ov002_020934e0;

void func_ov002_02082548(OtosuMenuObj* menuObj) {
    SpriteAnimation anim = data_ov002_02091b8c;

    anim.dataType  = menuObj->unk_1158C;
    anim.animIndex = 1;
    anim.posX      = 0x80;
    anim.posY      = 0xC8;
    anim.unk_1C    = 0x15;
    anim.unk_20    = 0x18;
    anim.unk_22    = 2;
    anim.unk_26    = 0x16;
    anim.unk_28    = 0x17;

    if (_Sprite_Load(&menuObj->cursor, &anim) == 0) {
        OS_WaitForever();
    }
    menuObj->cursorActive = 1;
}

void func_ov002_02082610(OtosuMenuObj* menuObj) {
    menuObj->cursorActive = 0;
    Sprite_Destroy(&menuObj->cursor);
    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->linkLevelCtx);
}

void func_ov002_0208264c(OtosuMenuObj* menuObj) {
    menuObj->cursorActive = 0;
    Sprite_Destroy(&menuObj->cursor);
}

PrcStepResult func_ov002_0208266c(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    gSaveData.otosuGameKey = func_ov002_02082bec(menuObj);
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_REPEAT;
}

PrcStepResult func_ov002_02082698(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = object;
    u16           i;

    MI_CpuCopyU8(menuObj->players[0].bssid, gSaveData.otosuParentBssid, 6);
    MI_CpuCopyU8(menuObj->players[0].bssid, menuObj->connectedBssids, 6);
    func_ov002_02082d44(menuObj);
    gSaveData.otosuPlayerCount = menuObj->unk_4198A;
    gSaveData.otosuBoard       = menuObj->unk_4196C;
    for (i = 0; i < 4; i++) {
        MI_CpuCopyU8(menuObj->players[i].bssid, gSaveData.otosuPlayerBssids[i], 6);
    }
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_REPEAT;
}

PrcStepResult func_ov002_0208275c(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = object;
    u16           i;
    SysCode*      ownerName;

    func_ov002_020824a0();
    MI_CpuCopyU8(gSaveData.otosuParentBssid, menuObj->players[0].bssid, 6);
    menuObj->unk_4198A = gSaveData.otosuPlayerCount;
    menuObj->unk_4196C = gSaveData.otosuBoard;
    for (i = 0; i < 4; i++) {
        MI_CpuFillU16(0xFFFF, menuObj->players[i].name, 0x16);
    }
    ownerName = SysFont_GetOwnerName();
    MI_CpuCopyU8(ownerName, &menuObj->unk_4196E, 0x16);
    MI_CpuCopyU8(ownerName, menuObj->players[gSaveData.otosuGameKey].name, 0x16);
    MI_CpuCopyU8(ownerName, menuObj->ownName, 0x16);
    menuObj->ownPlayer = gSaveData.otosuGameKey;
    Mem_Free(&gDebugHeap, ownerName);
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_REPEAT;
}

PrcStepResult OtosuPrcStep_FadeStart_BrightImmediate(PrcCtx* ctx, void* unused) {
    EasyFade_FadeBothDisplays(FADER_INSTANT, 16, 1);
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult OtosuPrcStep_FadeStart_DarkImmediate(PrcCtx* ctx, void* unused) {
    EasyFade_FadeBothDisplays(FADER_INSTANT, -16, 1);
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult OtosuPrcStep_FadeWait_Immediate(PrcCtx* ctx, void* unused) {
    if (EasyFade_IsFading() == 0) {
        PrcCtx_AdvanceStep(ctx);
    }
    return PRC_STEP_CONTINUE;
}

PrcStepResult OtosuPrcStep_FadeStart_Bright(PrcCtx* ctx, void* unused) {
    EasyFade_FadeBothDisplays(FADER_INSTANT, 16, 8);
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult OtosuPrcStep_FadeStart_Dark(PrcCtx* ctx, void* unused) {
    EasyFade_FadeBothDisplays(FADER_INSTANT, -16, 8);
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult OtosuPrcStep_FadeWait(PrcCtx* ctx, void* unused) {
    if (EasyFade_IsFading() == 0) {
        PrcCtx_AdvanceStep(ctx);
    }
    return PRC_STEP_CONTINUE;
}

PrcStepResult OtosuPrcStep_FadeStart_Neutral(PrcCtx* ctx, void* unused) {
    EasyFade_FadeBothDisplays(FADER_INSTANT, 0, 8);
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult OtosuPrcStep_FadeWait_Neutral(PrcCtx* ctx, void* unused) {
    if (EasyFade_IsFading() == 0) {
        PrcCtx_AdvanceStep(ctx);
    }
    return PRC_STEP_CONTINUE;
}

PrcStepResult OtosuPrcStep_FadeStart_Neutral2(PrcCtx* ctx, void* unused) {
    EasyFade_FadeBothDisplays(FADER_INSTANT, 0, 8);
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult OtosuPrcStep_FadeWait_Neutral2(PrcCtx* ctx, void* unused) {
    if (EasyFade_IsFading() == 0) {
        PrcCtx_AdvanceStep(ctx);
    }
    return PRC_STEP_CONTINUE;
}

PrcStepResult OtosuPrcStep_FadeStart_NeutralSlow(PrcCtx* ctx, void* unused) {
    EasyFade_FadeBothDisplays(FADER_INSTANT, 0, 40);
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult OtosuPrcStep_FadeWait_NeutralSlow(PrcCtx* ctx, void* unused) {
    if (EasyFade_IsFading() == 0) {
        PrcCtx_AdvanceStep(ctx);
    }
    return PRC_STEP_CONTINUE;
}

void func_ov002_02082a44(PrcCtx*, void*) {
    switch (func_ov040_0209cb78()) {
        case 0:
            SystemStatusFlags; // Unused volatile read?
            SystemStatusFlags.unk_07 = 1;
            break;
        case 1:
            func_ov040_0209d6cc();
            break;
        case 4:
            func_ov040_0209d588();
            break;
        case 2:
            func_ov040_0209c158();
            break;
        case 7:
            func_ov040_0209cde4();
            break;
        default:
            break;
    }
}

void func_ov002_02082ab4(OtosuMenuObj* menuObj) {
    OtosuMenuBoardEntries rowY = data_ov002_02091ab4;
    u16                   i;

    MI_CpuFillU16(0, menuObj->unk_41FF4, 0x2800);
    MI_CpuFillU16(0, data_ov002_020934e4, 22);
    for (i = 0; i < 4; i++) {
        if (menuObj->playerMask & (1 << i)) {
            SysFont_SetMsgPtr(&menuObj->font, menuObj->players[i].name);
            SysFont_SetPos(&menuObj->font, 0, rowY.entry[i]);
            SysFont_SetHAlign(&menuObj->font, 0, 256);
            SysFont_SetVAlign(&menuObj->font, 3, 80);
            SysFont_DrawCurrentToChar(&menuObj->font, menuObj->unk_41FF4, 32, 10);
        } else {
            data_ov002_020934e4[0] = 0xFFFF;
        }
    }
    func_0203abec(3, menuObj->unk_41FF4, (u8*)G2_GetBG0CharPtr() + 0x2400, 0x2800);
}

static inline BOOL OtosuMenu_IsSameBssid(const u8* a, const u8* b) {
    return a[0] == b[0] && a[1] == b[1] && a[2] == b[2] && a[3] == b[3] && a[4] == b[4] && a[5] == b[5];
}

u8 func_ov002_02082bec(OtosuMenuObj* menuObj) {
    u8 bssid[6];
    u8 player = 0xFF;
    u8 i;

    func_0203a96c(bssid);
    if (menuObj->unk_462EC != 0) {
        return 0;
    }
    for (i = 0; i < 4; i++) {
        if ((menuObj->playerMask & (1 << i)) && OtosuMenu_IsSameBssid(bssid, menuObj->players[i].bssid) == TRUE) {
            player = i;
            break;
        }
    }
    if (player == 0xFF) {
        OS_WaitForever();
    }
    return player;
}

void func_ov002_02082d44(OtosuMenuObj* menuObj) {
    u16 i;
    u16 write_index = 0;

    for (i = 0; i < 4; i++) {
        if ((menuObj->playerMask & (1 << i)) != 0) {
            MI_CpuCopyU8(menuObj->connectedBssids[i], menuObj->players[write_index].bssid, 6);
            write_index++;
        }
    }
}

void func_ov002_02082dbc(SysFont* font, const OtosuMenuRect* texts, void* chars, void* screen) {
    const OtosuMenuRect* text = texts;

    if (text->id == 0xFFFF) {
        return;
    }
    do {
        SysFont_SetMsg(font, text->id);
        SysFont_SetPos(font, text->left, text->top);
        SysFont_SetHAlign(font, 0, (u16)(text->right - text->left));
        SysFont_SetVAlign(font, 0, (u16)(text->bottom - text->top));
        SysFont_DrawCurrentToScreen(font, screen, chars, 0);
        text++;
    } while (text->id != 0xFFFF);
}

void func_ov002_02082e70(void* arg0, s32* arg1, void* arg2, void* arg3) {
    s32* entry = arg1;
    u16  index = 0;

    if (*entry == 0) {
        return;
    }
    do {
        SysFont_SetMsgPtr(arg0, *entry);
        SysFont_SetPos(arg0, 72, (u16)((index + 1) * 40));
        SysFont_SetHAlign(arg0, 0, 112);
        SysFont_SetVAlign(arg0, 0, 16);
        SysFont_DrawCurrentToScreen(arg0, arg3, arg2, 0);
        index++;
        entry++;
    } while (*entry != 0);
}

void func_ov002_02082f18(OtosuMenuObj* menuObj, s32 arg1, s32 arg2, const OtosuMenuRect* texts) {
    func_ov002_02085710(menuObj);
    g_DisplaySettings.engineState[1].bgSettings[0].priority = 0;
    g_DisplaySettings.engineState[1].bgSettings[1].priority = 1;
    g_DisplaySettings.engineState[1].bgSettings[2].priority = 2;
    g_DisplaySettings.engineState[1].bgSettings[3].priority = 3;

    menuObj->packs[0] = DatMgr_LoadPackEntry(1, 0, 0, &data_ov002_02091aac, 1, 0);

    Display_SetMainLayers(LAYER_BG0 | LAYER_BG2 | LAYER_BG3 | LAYER_OBJ);
    Display_SetSubLayers(LAYER_BG2 | LAYER_BG3 | LAYER_OBJ);

    menuObj->subPalette =
        PaletteMgr_AllocPalette(g_PaletteManagers[1], Data_GetPackEntryData(menuObj->packs[0], 4), 0, 0, 0x10);
    PaletteMgr_Flush(g_PaletteManagers[1], menuObj->subPalette);

    menuObj->subChars[3] = BgResMgr_AllocChar32(g_BgResourceManagers[1], Data_GetPackEntryData(menuObj->packs[0], 10),
                                                g_DisplaySettings.engineState[1].bgSettings[3].charBase, 0, 0x6000);

    menuObj->subScreens[3][0] = Data_GetPackEntryData(menuObj->packs[0], 13) + 4;
    func_0200d1d8(&menuObj->subMaps[3], 1, 3, 0, menuObj->subScreens[3], 1, 1);

    menuObj->subChars[2] = BgResMgr_AllocChar32(g_BgResourceManagers[1], Data_GetPackEntryData(menuObj->packs[0], 11),
                                                g_DisplaySettings.engineState[1].bgSettings[2].charBase, 0, 0x4D20);

    menuObj->subScreens[2][0] = Data_GetPackEntryData(menuObj->packs[0], 14) + 4;
    func_0200d1d8(&menuObj->subMaps[2], 1, 2, 0, menuObj->subScreens[2], 1, 1);

    menuObj->mainPalette =
        PaletteMgr_AllocPalette(g_PaletteManagers[0], Data_GetPackEntryData(menuObj->packs[0], 3), 0, 0, 0x10);
    PaletteMgr_Flush(g_PaletteManagers[0], menuObj->mainPalette);

    menuObj->mainChars[3] = BgResMgr_AllocChar32(g_BgResourceManagers[0], Data_GetPackEntryData(menuObj->packs[0], 16),
                                                 g_DisplaySettings.engineState[0].bgSettings[3].charBase, 0, 0x6000);

    menuObj->mainScreens[3][0] = Data_GetPackEntryData(menuObj->packs[0], 19) + 4;
    func_0200d1d8(&menuObj->mainMaps[3], 0, 3, 0, menuObj->mainScreens[3], 1, 1);

    menuObj->mainChars[2] = BgResMgr_AllocChar32(g_BgResourceManagers[0], Data_GetPackEntryData(menuObj->packs[0], 17),
                                                 g_DisplaySettings.engineState[0].bgSettings[2].charBase, 0, 0x8000);

    menuObj->mainScreens[2][0] = Data_GetPackEntryData(menuObj->packs[0], arg2) + 4;
    func_0200d1d8(&menuObj->mainMaps[2], 0, 2, 0, menuObj->mainScreens[2], 1, 1);

    if (arg1 != 0xFFFF) {
        menuObj->mainScreens[1][0] = Data_GetPackEntryData(menuObj->packs[0], arg1) + 4;
        func_0200d1d8(&menuObj->mainMaps[1], 0, 1, 0, menuObj->mainScreens[1], 1, 1);
        g_DisplaySettings.controls[0].layers |= 2;
    } else {
        g_DisplaySettings.controls[0].layers &= ~2;
    }

    void* ptr                  = Data_GetPackEntryData(menuObj->packs[0], 7);
    menuObj->mainScreens[0][0] = Data_GetPackEntryData(menuObj->packs[0], 8) + 4;

    SysFont_SetColor(&menuObj->font, 1);
    func_ov002_02082dbc(&menuObj->font, texts, ptr + 4, menuObj->mainScreens[0][0]);

    menuObj->mainChars[0] =
        BgResMgr_AllocChar32(g_BgResourceManagers[0], ptr, g_DisplaySettings.engineState[0].bgSettings[0].charBase, 0, 0x6000);
    func_0200d1d8(&menuObj->mainMaps[0], 0, 0, 0, menuObj->mainScreens[0], 1, 1);
    Display_Commit();
}

void func_ov002_02083484(OtosuMenuObj* menuObj, const OtosuMenuRect* texts) {
    g_DisplaySettings.engineState[1].bgSettings[0].priority = 0;
    g_DisplaySettings.engineState[1].bgSettings[1].priority = 1;
    g_DisplaySettings.engineState[1].bgSettings[2].priority = 2;
    g_DisplaySettings.engineState[1].bgSettings[3].priority = 3;

    menuObj->packs[0] = DatMgr_LoadPackEntry(1, 0, 0, &data_ov002_02091aac, 1, 0);
    menuObj->packs[1] = DatMgr_LoadPackEntry(1, 0, 0, &data_ov002_02091aac, 5, 0);

    Display_SetMainLayers(LAYER_NONE);
    Display_SetSubLayers(LAYER_BG1);

    menuObj->subPalette =
        PaletteMgr_AllocPalette(g_PaletteManagers[1], Data_GetPackEntryData(menuObj->packs[1], 6), 0, 0, 0x10);
    PaletteMgr_Flush(g_PaletteManagers[1], menuObj->subPalette);

    void* table_ptr           = Data_GetPackEntryData(menuObj->packs[0], 5);
    menuObj->subScreens[1][0] = Data_GetPackEntryData(menuObj->packs[0], 6) + 4;

    SysFont_SetColor(&menuObj->font, 1);
    SysFont_SetLineSpacing(&menuObj->font, 4);
    SysFont_SetSpacing(&menuObj->font, TRUE, 0);
    func_ov002_02082dbc(&menuObj->font, texts, table_ptr + 4, menuObj->subScreens[1][0]);
    SysFont_SetLineSpacing(&menuObj->font, 2);
    SysFont_SetSpacing(&menuObj->font, TRUE, 0);

    menuObj->subChars[1] = BgResMgr_AllocChar32(g_BgResourceManagers[1], table_ptr,
                                                g_DisplaySettings.engineState[1].bgSettings[1].charBase, 0, 0x6000);
    func_0200d1d8(&menuObj->subMaps[1], 1, 1, 0, menuObj->subScreens[1], 1, 1);
    Display_Commit();
}

void func_ov002_02083694(OtosuMenuObj* menuObj) {
    func_ov002_02085710(menuObj);

    menuObj->packs[0] = DatMgr_LoadPackEntry(1, 0, 0, &data_ov002_02091aac, 1, 0);
    menuObj->packs[1] = DatMgr_LoadPackEntry(1, 0, 0, &data_ov002_02091aac, 2, 0);

    menuObj->subPalette =
        PaletteMgr_AllocPalette(g_PaletteManagers[1], Data_GetPackEntryData(menuObj->packs[1], 1), 0, 0, 0x10);

    PaletteMgr_Flush(g_PaletteManagers[1], menuObj->subPalette);

    menuObj->subChars[3] = BgResMgr_AllocChar32(g_BgResourceManagers[1], Data_GetPackEntryData(menuObj->packs[1], 4),
                                                g_DisplaySettings.engineState[1].bgSettings[3].charBase, 0, 0x5800);

    menuObj->subScreens[3][0] = Data_GetPackEntryData(menuObj->packs[1], 5) + 4;
    func_0200d1d8(&menuObj->subMaps[3], 1, 3, 0, menuObj->subScreens[3], 1, 1);

    menuObj->subChars[2] = BgResMgr_AllocChar32(g_BgResourceManagers[1], Data_GetPackEntryData(menuObj->packs[1], 2),
                                                g_DisplaySettings.engineState[1].bgSettings[2].charBase, 0, 0x8000);

    menuObj->subScreens[2][0] = Data_GetPackEntryData(menuObj->packs[1], 3) + 4;
    func_0200d1d8(&menuObj->subMaps[2], 1, 2, 0, menuObj->subScreens[2], 1, 1);

    menuObj->mainPalette =
        PaletteMgr_AllocPalette(g_PaletteManagers[0], Data_GetPackEntryData(menuObj->packs[1], 6), 0, 0, 0x10);
    PaletteMgr_Flush(g_PaletteManagers[0], menuObj->mainPalette);

    menuObj->mainChars[3] = BgResMgr_AllocChar32(g_BgResourceManagers[0], Data_GetPackEntryData(menuObj->packs[1], 9),
                                                 g_DisplaySettings.engineState[0].bgSettings[3].charBase, 0, 0x6000);

    menuObj->mainScreens[3][0] = Data_GetPackEntryData(menuObj->packs[1], 10) + 4;
    func_0200d1d8(&menuObj->mainMaps[3], 0, 3, 0, menuObj->mainScreens[3], 1, 1);

    menuObj->mainChars[2] = BgResMgr_AllocChar32(g_BgResourceManagers[0], Data_GetPackEntryData(menuObj->packs[1], 7),
                                                 g_DisplaySettings.engineState[0].bgSettings[2].charBase, 0, 0x8000);

    menuObj->mainScreens[2][0] = Data_GetPackEntryData(menuObj->packs[1], 8) + 4;
    func_0200d1d8(&menuObj->mainMaps[2], 0, 2, 0, menuObj->mainScreens[2], 1, 1);

    Display_SetMainLayers(LAYER_BG2 | LAYER_BG3 | LAYER_OBJ);
    Display_SetSubLayers(LAYER_BG2 | LAYER_BG3 | LAYER_OBJ);
    Display_Commit();
}

void func_ov002_02083a74(OtosuMenuObj* menuObj) {
    func_ov002_02085710(menuObj);

    menuObj->packs[0] = DatMgr_LoadPackEntry(1, 0, 0, &data_ov002_02091aac, 1, 0);
    menuObj->packs[1] = DatMgr_LoadPackEntry(1, 0, 0, &data_ov002_02091aac, 3, 0);

    menuObj->subPalette =
        PaletteMgr_AllocPalette(g_PaletteManagers[1], Data_GetPackEntryData(menuObj->packs[1], 1), 0, 0, 0x10);
    PaletteMgr_Flush(g_PaletteManagers[1], menuObj->subPalette);

    menuObj->subChars[3] = BgResMgr_AllocChar32(g_BgResourceManagers[1], Data_GetPackEntryData(menuObj->packs[0], 9),
                                                g_DisplaySettings.engineState[1].bgSettings[3].charBase, 0, 0x6000);

    menuObj->subScreens[3][0] = Data_GetPackEntryData(menuObj->packs[0], 12) + 4;
    func_0200d1d8(&menuObj->subMaps[3], 1, 3, 0, menuObj->subScreens[3], 1, 1);

    menuObj->subChars[2] = BgResMgr_AllocChar32(g_BgResourceManagers[1], Data_GetPackEntryData(menuObj->packs[1], 3),
                                                g_DisplaySettings.engineState[1].bgSettings[2].charBase, 0, 0x8000);

    menuObj->subScreens[2][0] = Data_GetPackEntryData(menuObj->packs[1], 5) + 4;
    menuObj->subScreens[2][1] = Data_GetPackEntryData(menuObj->packs[1], 12) + 4;

    func_0200d1d8(&menuObj->subMaps[2], 1, 2, 0, menuObj->subScreens[2], 1, 2);
    func_0200d858(&menuObj->subMaps[2], 0, 0, 0);

    Display_SetBGOffset(menuObj->subMaps[2].engineId, menuObj->subMaps[2].bgLayer, 0x200000, 0x200000);

    menuObj->subChars[1] = BgResMgr_AllocChar32(g_BgResourceManagers[1], Data_GetPackEntryData(menuObj->packs[0], 11),
                                                g_DisplaySettings.engineState[1].bgSettings[1].charBase, 0, 0x4D20);

    menuObj->subScreens[1][0] = Data_GetPackEntryData(menuObj->packs[0], 14) + 4;
    func_0200d1d8(&menuObj->subMaps[1], 1, 1, 0, menuObj->subScreens[1], 1, 1);
    func_0200d858(&menuObj->subMaps[1], 0, 0, 0);

    Display_SetBGOffset(menuObj->subMaps[1].engineId, menuObj->subMaps[1].bgLayer, 0x200000, 0x200000);

    void* var_r1_5            = Data_GetPackEntryData(menuObj->packs[1], 2);
    menuObj->subScreens[0][0] = Data_GetPackEntryData(menuObj->packs[1], 4) + 4;
    menuObj->subChars[0]      = BgResMgr_AllocChar32(g_BgResourceManagers[1], var_r1_5,
                                                     g_DisplaySettings.engineState[1].bgSettings[0].charBase, 0, 0x11A0);
    func_0200d1d8(&menuObj->subMaps[0], 1, 0, 0, menuObj->subScreens[0], 1, 1);
    func_0200d858(&menuObj->subMaps[0], 0, 0, 0);

    Display_SetBGOffset(menuObj->subMaps[0].engineId, menuObj->subMaps[0].bgLayer, 0x200000, 0x200000);

    menuObj->mainPalette =
        PaletteMgr_AllocPalette(g_PaletteManagers[0], Data_GetPackEntryData(menuObj->packs[1], 6), 0, 0, 0x10);
    PaletteMgr_Flush(g_PaletteManagers[0], menuObj->mainPalette);

    menuObj->mainChars[3] = BgResMgr_AllocChar32(g_BgResourceManagers[0], Data_GetPackEntryData(menuObj->packs[0], 15),
                                                 g_DisplaySettings.engineState[0].bgSettings[3].charBase, 0, 0x6000);

    menuObj->mainScreens[3][0] = Data_GetPackEntryData(menuObj->packs[0], 18) + 4;
    func_0200d1d8(&menuObj->mainMaps[3], 0, 3, 0, menuObj->mainScreens[3], 1, 1);

    menuObj->mainChars[2] = BgResMgr_AllocChar32(g_BgResourceManagers[0], Data_GetPackEntryData(menuObj->packs[1], 3),
                                                 g_DisplaySettings.engineState[0].bgSettings[2].charBase, 0, 0x8000);

    menuObj->mainScreens[2][0] = Data_GetPackEntryData(menuObj->packs[1], 5) + 4;
    menuObj->mainScreens[2][1] = Data_GetPackEntryData(menuObj->packs[1], 12) + 4;
    func_0200d1d8(&menuObj->mainMaps[2], 0, 2, 0, menuObj->mainScreens[2], 1, 2);
    func_0200d858(&menuObj->mainMaps[2], 0, 0, 0);

    Display_SetBGOffset(menuObj->mainMaps[2].engineId, menuObj->mainMaps[2].bgLayer, 0x200000, 0x200000);

    menuObj->mainChars[1] = BgResMgr_AllocChar32(g_BgResourceManagers[0], Data_GetPackEntryData(menuObj->packs[1], 8),
                                                 g_DisplaySettings.engineState[0].bgSettings[1].charBase, 0, 0x3EA0);

    menuObj->mainScreens[1][0] = Data_GetPackEntryData(menuObj->packs[1], 11) + 4;
    func_0200d1d8(&menuObj->mainMaps[1], 0, 1, 0, menuObj->mainScreens[1], 1, 1);
    func_0200d858(&menuObj->mainMaps[1], 0, 0, 0);

    Display_SetBGOffset(menuObj->mainMaps[1].engineId, menuObj->mainMaps[1].bgLayer, 0x200000, 0x200000);

    void* var_r1_10 = Data_GetPackEntryData(menuObj->packs[1], 7);

    menuObj->mainScreens[0][0] = Data_GetPackEntryData(menuObj->packs[1], 9) + 4;
    menuObj->mainScreens[0][1] = Data_GetPackEntryData(menuObj->packs[1], 10) + 4;

    menuObj->mainChars[0] = BgResMgr_AllocChar32(g_BgResourceManagers[0], var_r1_10,
                                                 g_DisplaySettings.engineState[0].bgSettings[0].charBase, 0, 0x8000);
    func_0200d1d8(&menuObj->mainMaps[0], 0, 0, 0, menuObj->mainScreens[0], 2, 1);
    func_0200d858(&menuObj->mainMaps[0], 0, 0, 0);

    Display_SetBGOffset(menuObj->mainMaps[0].engineId, menuObj->mainMaps[0].bgLayer, 0x200000, 0x200000);

    Display_SetMainLayers(LAYER_BG0 | LAYER_BG1 | LAYER_BG2 | LAYER_BG3 | LAYER_OBJ);
    Display_SetSubLayers(LAYER_BG0 | LAYER_BG1 | LAYER_BG2 | LAYER_BG3 | LAYER_OBJ);

    Display_Commit();
}

void func_ov002_02084494(OtosuMenuObj* menuObj, u8 arg1, const OtosuMenuRect* texts) {
    OtosuMenuRectList4    header = data_ov002_02091b64;
    OtosuMenuBoardEntries subPalettes;
    OtosuMenuRectList10   slots = data_ov002_02091bb8;
    OtosuMenuBoardEntries subScreens;
    OtosuMenuBoardEntries mainPalettes;

    func_ov002_02085710(menuObj);

    g_DisplaySettings.engineState[1].bgSettings[0].priority = 3;
    g_DisplaySettings.engineState[1].bgSettings[1].priority = 0;
    g_DisplaySettings.engineState[1].bgSettings[2].priority = 1;
    g_DisplaySettings.engineState[1].bgSettings[3].priority = 2;

    menuObj->packs[0] = DatMgr_LoadPackEntry(1, 0, 0, &data_ov002_02091aac, 1, 0);
    menuObj->packs[1] = DatMgr_LoadPackEntry(1, 0, 0, &data_ov002_02091aac, 4, 0);

    subPalettes = data_ov002_02091abc;

    menuObj->subPalette = PaletteMgr_AllocPalette(
        g_PaletteManagers[1], Data_GetPackEntryData(menuObj->packs[1], subPalettes.entry[arg1]), 0, 0, 0x10);
    PaletteMgr_Flush(g_PaletteManagers[1], menuObj->subPalette);

    menuObj->subChars[3] = BgResMgr_AllocChar32(g_BgResourceManagers[1], Data_GetPackEntryData(menuObj->packs[1], 6),
                                                g_DisplaySettings.engineState[1].bgSettings[3].charBase, 0, 0x8000);

    menuObj->subScreens[3][0] = Data_GetPackEntryData(menuObj->packs[1], 10) + 4;
    func_0200d1d8(&menuObj->subMaps[3], 1, 3, 0, menuObj->subScreens[3], 1, 1);

    menuObj->subChars[2] = BgResMgr_AllocChar32(g_BgResourceManagers[1], Data_GetPackEntryData(menuObj->packs[1], 7),
                                                g_DisplaySettings.engineState[1].bgSettings[2].charBase, 0, 0x6000);

    menuObj->subScreens[2][0] = Data_GetPackEntryData(menuObj->packs[1], 11) + 4;
    func_0200d1d8(&menuObj->subMaps[2], 1, 2, 0, menuObj->subScreens[2], 1, 1);

    menuObj->subChars[1] = BgResMgr_AllocChar32(g_BgResourceManagers[1], Data_GetPackEntryData(menuObj->packs[1], 9),
                                                g_DisplaySettings.engineState[1].bgSettings[1].charBase, 0, 0x8000);

    subScreens = data_ov002_02091aa4;

    menuObj->subScreens[1][0] = Data_GetPackEntryData(menuObj->packs[1], subScreens.entry[arg1]) + 4;
    func_0200d1d8(&menuObj->subMaps[1], 1, 1, 0, menuObj->subScreens[1], 1, 1);

    menuObj->subChars[0] = BgResMgr_AllocChar32(g_BgResourceManagers[1], Data_GetPackEntryData(menuObj->packs[1], 8),
                                                g_DisplaySettings.engineState[1].bgSettings[0].charBase, 0, 0x200);

    Display_SetSubLayers(LAYER_BG0 | LAYER_BG1 | LAYER_BG2 | LAYER_BG3 | LAYER_OBJ);

    mainPalettes = data_ov002_02091ac4;

    menuObj->mainPalette = PaletteMgr_AllocPalette(
        g_PaletteManagers[0], Data_GetPackEntryData(menuObj->packs[1], mainPalettes.entry[arg1]), 0, 0, 0x10);
    PaletteMgr_Flush(g_PaletteManagers[0], menuObj->mainPalette);

    menuObj->mainChars[3] = BgResMgr_AllocChar32(g_BgResourceManagers[0], Data_GetPackEntryData(menuObj->packs[1], 20),
                                                 g_DisplaySettings.engineState[0].bgSettings[3].charBase, 0, 0x6000);

    menuObj->mainScreens[3][0] = Data_GetPackEntryData(menuObj->packs[1], 21) + 4;
    func_0200d1d8(&menuObj->mainMaps[3], 0, 3, 0, menuObj->mainScreens[3], 1, 1);

    menuObj->mainChars[2] = BgResMgr_AllocChar32(g_BgResourceManagers[0], Data_GetPackEntryData(menuObj->packs[1], 9),
                                                 g_DisplaySettings.engineState[0].bgSettings[2].charBase, 0, 0x8000);

    menuObj->mainScreens[2][0] = Data_GetPackEntryData(menuObj->packs[0], 28) + 4;
    func_0200d1d8(&menuObj->mainMaps[2], 0, 2, 0, menuObj->mainScreens[2], 1, 1);

    void* var_r1_9 = Data_GetPackEntryData(menuObj->packs[1], 9);

    menuObj->mainScreens[1][0] = Data_GetPackEntryData(menuObj->packs[1], 22) + 4;
    menuObj->mainChars[1]      = BgResMgr_AllocChar32(g_BgResourceManagers[0], var_r1_9,
                                                      g_DisplaySettings.engineState[0].bgSettings[1].charBase, 0, 0x8000);
    func_0200d1d8(&menuObj->mainMaps[1], 0, 1, 0, menuObj->mainScreens[1], 1, 1);

    void* var_r4 = Data_GetPackEntryData(menuObj->packs[0], 7);

    menuObj->mainScreens[0][0] = Data_GetPackEntryData(menuObj->packs[0], 8) + 4;
    SysFont_SetColor(&menuObj->font, 1U);
    func_ov002_02082dbc(&menuObj->font, slots.rects, var_r4 + 4, menuObj->mainScreens[0][0]);
    func_ov002_02082dbc(&menuObj->font, texts, var_r4 + 4, menuObj->mainScreens[0][0]);
    SysFont_SetColor(&menuObj->font, 3U);
    func_ov002_02082dbc(&menuObj->font, header.rects, var_r4 + 4, menuObj->mainScreens[0][0]);
    SysFont_SetColor(&menuObj->font, 1U);
    menuObj->mainChars[0] = BgResMgr_AllocChar32(g_BgResourceManagers[0], var_r4,
                                                 g_DisplaySettings.engineState[0].bgSettings[0].charBase, 0, 0x6000);
    func_0200d1d8(&menuObj->mainMaps[0], 0, 0, 0, menuObj->mainScreens[0], 1, 1);
    Display_SetMainLayers(LAYER_BG0 | LAYER_BG1 | LAYER_BG2 | LAYER_BG3 | LAYER_OBJ);
    Display_Commit();
}

void func_ov002_02084c84(OtosuMenuObj* menuObj, const OtosuMenuRect* texts) {
    void* textChars;

    func_ov002_02085710(menuObj);
    g_DisplaySettings.engineState[1].bgSettings[0].priority = 0;
    g_DisplaySettings.engineState[1].bgSettings[1].priority = 1;
    g_DisplaySettings.engineState[1].bgSettings[2].priority = 2;
    g_DisplaySettings.engineState[1].bgSettings[3].priority = 3;

    menuObj->packs[0] = DatMgr_LoadPackEntry(1, 0, 0, &data_ov002_02091aac, 1, 0);
    menuObj->packs[1] = DatMgr_LoadPackEntry(1, 0, 0, &data_ov002_02091aac, 5, 0);

    menuObj->subPalette =
        PaletteMgr_AllocPalette(g_PaletteManagers[1], Data_GetPackEntryData(menuObj->packs[1], 1), 0, 0, 0x10);
    PaletteMgr_Flush(g_PaletteManagers[1], menuObj->subPalette);

    menuObj->subChars[3] = BgResMgr_AllocChar32(g_BgResourceManagers[1], Data_GetPackEntryData(menuObj->packs[1], 2),
                                                g_DisplaySettings.engineState[1].bgSettings[3].charBase, 0, 0x6000);

    menuObj->subScreens[3][0] = Data_GetPackEntryData(menuObj->packs[1], 4) + 4;
    func_0200d1d8(&menuObj->subMaps[3], 1, 3, 0, menuObj->subScreens[3], 1, 1);

    Display_SetSubLayers(LAYER_BG3 | LAYER_OBJ);

    menuObj->mainPalette =
        PaletteMgr_AllocPalette(g_PaletteManagers[0], Data_GetPackEntryData(menuObj->packs[1], 6), 0, 0, 0x10);
    PaletteMgr_Flush(g_PaletteManagers[0], menuObj->mainPalette);

    menuObj->mainChars[3] = BgResMgr_AllocChar32(g_BgResourceManagers[0], Data_GetPackEntryData(menuObj->packs[1], 7),
                                                 g_DisplaySettings.engineState[0].bgSettings[3].charBase, 0, 0x6000);

    menuObj->mainScreens[3][0] = Data_GetPackEntryData(menuObj->packs[1], 8) + 4;
    func_0200d1d8(&menuObj->mainMaps[3], 0, 3, 0, menuObj->mainScreens[3], 1, 1);

    menuObj->mainChars[2] = BgResMgr_AllocChar32(g_BgResourceManagers[0], Data_GetPackEntryData(menuObj->packs[1], 3),
                                                 g_DisplaySettings.engineState[0].bgSettings[2].charBase, 0, 0x8000);

    menuObj->mainScreens[2][0] = Data_GetPackEntryData(menuObj->packs[1], 11) + 4;
    func_0200d1d8(&menuObj->mainMaps[2], 0, 2, 0, menuObj->mainScreens[2], 1, 1);

    textChars                  = Data_GetPackEntryData(menuObj->packs[0], 7);
    menuObj->mainScreens[0][0] = Data_GetPackEntryData(menuObj->packs[0], 8) + 4;
    SysFont_SetColor(&menuObj->font, 1);
    if (texts != NULL) {
        func_ov002_02082dbc(&menuObj->font, texts, textChars + 4, menuObj->mainScreens[0][0]);
    }
    menuObj->mainChars[0] = BgResMgr_AllocChar32(g_BgResourceManagers[0], textChars,
                                                 g_DisplaySettings.engineState[0].bgSettings[0].charBase, 0, 0x6000);
    func_0200d1d8(&menuObj->mainMaps[0], 0, 0, 0, menuObj->mainScreens[0], 1, 1);
    Display_SetMainLayers(LAYER_BG0 | LAYER_BG2 | LAYER_BG3 | LAYER_OBJ);
    Display_Commit();
}

void func_ov002_020850c0(OtosuMenuObj* menuObj, s32 arg1, s32 arg2, s32* arg3, const OtosuMenuRect* texts) {
    void* chars;

    func_ov002_02085710(menuObj);

    menuObj->packs[0] = DatMgr_LoadPackEntry(1, 0, 0, &data_ov002_02091aac, 1, 0);
    menuObj->packs[1] = DatMgr_LoadPackEntry(1, 0, 0, &data_ov002_02091aac, 5, 0);

    menuObj->subPalette =
        PaletteMgr_AllocPalette(g_PaletteManagers[1], Data_GetPackEntryData(menuObj->packs[1], 1), 0, 0, 0x10);
    PaletteMgr_Flush(g_PaletteManagers[1], menuObj->subPalette);

    menuObj->subChars[3] = BgResMgr_AllocChar32(g_BgResourceManagers[1], Data_GetPackEntryData(menuObj->packs[1], 2),
                                                g_DisplaySettings.engineState[1].bgSettings[3].charBase, 0, 0x6000);

    menuObj->subScreens[3][0] = Data_GetPackEntryData(menuObj->packs[1], 4) + 4;
    func_0200d1d8(&menuObj->subMaps[3], 1, 3, 0, menuObj->subScreens[3], 1, 1);

    menuObj->subChars[2] = BgResMgr_AllocChar32(g_BgResourceManagers[1], Data_GetPackEntryData(menuObj->packs[1], 3),
                                                g_DisplaySettings.engineState[1].bgSettings[2].charBase, 0, 0x8000);

    menuObj->subScreens[2][0] = Data_GetPackEntryData(menuObj->packs[1], 5) + 4;
    func_0200d1d8(&menuObj->subMaps[2], 1, 2, 0, menuObj->subScreens[2], 1, 1);

    chars                     = Data_GetPackEntryData(menuObj->packs[0], 5);
    menuObj->subScreens[1][0] = Data_GetPackEntryData(menuObj->packs[0], 6) + 4;
    SysFont_SetColor(&menuObj->font, 1);
    func_ov002_02082e70(&menuObj->font, arg3, chars + 4, menuObj->subScreens[1][0]);
    menuObj->subChars[1] = BgResMgr_AllocChar32(g_BgResourceManagers[1], chars,
                                                g_DisplaySettings.engineState[1].bgSettings[1].charBase, 0, 0x6000);
    func_0200d1d8(&menuObj->subMaps[1], 1, 1, 0, menuObj->subScreens[1], 1, 1);

    Display_SetSubLayers(LAYER_BG1 | LAYER_BG2 | LAYER_BG3 | LAYER_OBJ);
    Display_SetMainLayers(LAYER_NONE);

    menuObj->mainPalette =
        PaletteMgr_AllocPalette(g_PaletteManagers[0], Data_GetPackEntryData(menuObj->packs[1], 6), 0, 0, 0x10);
    PaletteMgr_Flush(g_PaletteManagers[0], menuObj->mainPalette);

    menuObj->mainChars[3] = BgResMgr_AllocChar32(g_BgResourceManagers[0], Data_GetPackEntryData(menuObj->packs[1], 7),
                                                 g_DisplaySettings.engineState[0].bgSettings[3].charBase, 0, 0x6000);

    menuObj->mainScreens[3][0] = Data_GetPackEntryData(menuObj->packs[1], 8) + 4;
    func_0200d1d8(&menuObj->mainMaps[3], 0, 3, 0, menuObj->mainScreens[3], 1, 1);

    menuObj->mainScreens[2][0] = Data_GetPackEntryData(menuObj->packs[1], arg2) + 4;
    func_0200d1d8(&menuObj->mainMaps[2], 0, 2, 0, menuObj->mainScreens[2], 1, 1);

    if (arg1 != 0xFFFF) {
        chars                      = Data_GetPackEntryData(menuObj->packs[1], 3);
        menuObj->mainScreens[1][0] = Data_GetPackEntryData(menuObj->packs[1], arg1) + 4;
        menuObj->mainChars[1]      = BgResMgr_AllocChar32(g_BgResourceManagers[0], chars,
                                                          g_DisplaySettings.engineState[0].bgSettings[1].charBase, 0, 0x8000);
        func_0200d1d8(&menuObj->mainMaps[1], 0, 1, 0, menuObj->mainScreens[1], 1, 1);
        g_DisplaySettings.controls[0].layers |= 2;
    } else if ((menuObj->mainScreens[1][0] == 0) && (menuObj->mainChars[1] == NULL)) {
    } else {
        OS_WaitForever();
    }

    chars                      = Data_GetPackEntryData(menuObj->packs[0], 7);
    menuObj->mainScreens[0][0] = Data_GetPackEntryData(menuObj->packs[0], 8) + 4;
    SysFont_SetColor(&menuObj->font, 1);
    func_ov002_02082dbc(&menuObj->font, texts, chars + 4, menuObj->mainScreens[0][0]);
    menuObj->mainChars[0] = BgResMgr_AllocChar32(g_BgResourceManagers[0], chars,
                                                 g_DisplaySettings.engineState[0].bgSettings[0].charBase, 0, 0x6000);
    func_0200d1d8(&menuObj->mainMaps[0], 0, 0, 0, menuObj->mainScreens[0], 1, 1);
    g_DisplaySettings.controls[0].layers |= 0x1D;
    Display_Commit();
}

void func_ov002_02085710(OtosuMenuObj* menuObj) {
    if (menuObj->mainPalette != NULL) {
        PaletteMgr_ReleaseResource(g_PaletteManagers[0], menuObj->mainPalette);
        menuObj->mainPalette = NULL;
    }
    if (menuObj->mainChars[0] != NULL) {
        BgResMgr_ReleaseChar(g_BgResourceManagers[0], menuObj->mainChars[0]);
        menuObj->mainChars[0] = NULL;
    }
    if (menuObj->mainChars[1] != NULL) {
        BgResMgr_ReleaseChar(g_BgResourceManagers[0], menuObj->mainChars[1]);
        menuObj->mainChars[1] = NULL;
    }
    if (menuObj->mainChars[2] != NULL) {
        BgResMgr_ReleaseChar(g_BgResourceManagers[0], menuObj->mainChars[2]);
        menuObj->mainChars[2] = NULL;
    }
    if (menuObj->mainChars[3] != NULL) {
        BgResMgr_ReleaseChar(g_BgResourceManagers[0], menuObj->mainChars[3]);
        menuObj->mainChars[3] = NULL;
    }

    if (menuObj->subPalette != NULL) {
        PaletteMgr_ReleaseResource(g_PaletteManagers[1], menuObj->subPalette);
        menuObj->subPalette = NULL;
    }
    if (menuObj->subChars[0] != NULL) {
        BgResMgr_ReleaseChar(g_BgResourceManagers[1], menuObj->subChars[0]);
        menuObj->subChars[0] = NULL;
    }
    if (menuObj->subChars[1] != NULL) {
        BgResMgr_ReleaseChar(g_BgResourceManagers[1], menuObj->subChars[1]);
        menuObj->subChars[1] = NULL;
    }
    if (menuObj->subChars[2] != NULL) {
        BgResMgr_ReleaseChar(g_BgResourceManagers[1], menuObj->subChars[2]);
        menuObj->subChars[2] = NULL;
    }
    if (menuObj->subChars[3] != NULL) {
        BgResMgr_ReleaseChar(g_BgResourceManagers[1], menuObj->subChars[3]);
        menuObj->subChars[3] = NULL;
    }

    func_0200d954(1, 0);
    func_0200d954(1, 1);
    func_0200d954(1, 2);
    func_0200d954(1, 3);
    func_0200d954(0, 2);
    func_0200d954(0, 3);

    if (menuObj->packs[0] != NULL) {
        DatMgr_ReleaseData(menuObj->packs[0]);
        menuObj->packs[0] = NULL;
    }
    if (menuObj->packs[1] != NULL) {
        DatMgr_ReleaseData(menuObj->packs[1]);
        menuObj->packs[1] = NULL;
    }
    if (menuObj->packs[2] != NULL) {
        DatMgr_ReleaseData(menuObj->packs[2]);
        menuObj->packs[2] = NULL;
    }
    if (menuObj->packs[3] != NULL) {
        DatMgr_ReleaseData(menuObj->packs[3]);
        menuObj->packs[3] = NULL;
    }
}

u16 func_ov002_0208597c(const OtosuMenuRect* buttons) {
    const u16* table = (const u16*)buttons; // walked as raw halfwords, five per rect
    TouchCoord touch;
    TouchCoord coords;
    u16        index  = 0;
    u16        result = 0xFFFF;

    TouchInput_GetCoord(&coords);
    touch = coords;
    if (TouchInput_WasTouchPressed() != 0) {
        result = 0xFFFE;
        if (table[0] != 0xFFFF) {
            do {
                if (touch.x >= table[index * 5 + 1] && touch.x <= table[index * 5 + 3] && touch.y >= table[index * 5 + 2] &&
                    touch.y <= table[index * 5 + 4])
                {
                    result = table[index * 5];
                    break;
                }
                index++;
            } while (table[index * 5] != 0xFFFF);
        }
    }
    return result;
}

void func_ov002_02085a44(OtosuMenuObj* menuObj) {
    func_ov002_02082548(menuObj);
    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->linkLevelCtx);
    PrcCtx_Init(&menuObj->linkLevelCtx, "OtosuMenuLinklevel", sizeof(OtosuMenuObj));
    PrcCtx_SetWorkObject(&menuObj->linkLevelCtx, menuObj);
    PrcCtx_ReplaceFrame(&menuObj->linkLevelCtx, &OtosuMenu_LinkLevel_FrameDesc, NULL);
    PrcMaster_RegisterContext(&menuObj->prcMaster, &menuObj->linkLevelCtx);
}

void func_ov002_02085ac4(OtosuMenuObj* menuObj) {
    s32 result = (s32)func_ov040_0209cb78();

    switch (result) {
        case 0:
        case 8:
        case 9:
        case 10:
            menuObj->unk_462E8 = 0;
            break;
        default:
            menuObj->unk_462E8 = 1;
            break;
    }

    if (PrcCtx_GetStepTable(&menuObj->mainCtx) != data_ov002_02092efc &&
        PrcCtx_GetStepTable(&menuObj->mainCtx) != data_ov002_02092e68 &&
        PrcCtx_GetStepTable(&menuObj->mainCtx) != data_ov002_02092ec8 &&
        PrcCtx_GetStepTable(&menuObj->mainCtx) != data_ov002_02092e98)
    {
        switch (result) {
            case 10:
                PrcCtx_ReplaceFrame(&menuObj->mainCtx, &data_ov002_02092e40, NULL);
                break;
            case 8:
            case 9:
                switch (menuObj->unk_41FE9) {
                    case 0:
                        PrcCtx_ReplaceFrame(&menuObj->mainCtx, &data_ov002_02092e54, NULL);
                        break;
                    case 1:
                        PrcCtx_ReplaceFrame(&menuObj->mainCtx, &data_ov002_02092e18, NULL);
                        break;
                    case 2:
                        PrcCtx_ReplaceFrame(&menuObj->mainCtx, &data_ov002_02092e2c, NULL);
                        break;
                }
                break;
        }
    }

    menuObj->unk_4180C = 1;
}

OtosuMenuObj* OtosuMenu_Init(void) {
    FS_LoadOverlay(0, &OVERLAY_31_ID);

    OtosuMenuObj* obj = Mem_AllocHeapTail(&gMainHeap, sizeof(OtosuMenuObj));
    Mem_SetSequence(&gMainHeap, obj, OtosuMenu_ObjName);
    MI_CpuFill(0, obj, sizeof(OtosuMenuObj));
    MainOvlDisp_SetCbArg(obj);
    Mem_InitializeHeap(&obj->heap, obj->heapBuffer, sizeof(obj->heapBuffer));
    obj->unk_11584      = DatMgr_AllocateSlot();
    obj->unk_1158C      = DatMgr_AllocateSlot();
    obj->unk_11588      = DatMgr_AllocateSlot();
    data_ov002_020934e0 = 0;
    obj->unk_11580      = ResourceMgr_ReinitManagers(&obj->unk_00000);
    SysFont_InitWithFont(&obj->font, 1, TRUE);
    SysFont_SetLineSpacing(&obj->font, 3);
    SysFont_SetSpacing(&obj->font, TRUE, 0);
    SysFont_SetColor(&obj->font, 12);
    PrcMaster_Init(&obj->prcMaster, 30);
    EasyTask_InitializePool(&obj->taskPool, &obj->heap, 0x100, NULL, NULL);
    EasyTask_CreateTask(&obj->taskPool, &Task_EasyFade, NULL, 0, NULL, NULL);
    func_0200d8f0();
    data_02066aec  = 0;
    data_02066eec  = 0;
    obj->unk_46070 = SystemStatusFlags.unk_06;
    obj->unk_41FF0 = 0;
    obj->unk_460BC = 0;
    return obj;
}

OtosuMenuObj* func_ov002_02085df8(void) {
    OvlMgr_LoadOverlay(3, &OVERLAY_40_ID);

    OtosuMenuObj* obj = OtosuMenu_Init();
    obj->unk_41FF0    = 1;
    return obj;
}

void OtosuMenu_InitForSinglePlayerEnter(OtosuMenuObj* menuObj) {
    func_ov002_0208b860();
    menuObj = func_ov002_02085df8();
    PrcCtx_Init(&menuObj->mainCtx, OtosuMenu_ObjName, sizeof(OtosuMenuObj));
    PrcCtx_SetWorkObject(&menuObj->mainCtx, menuObj);
    PrcCtx_ReplaceFrame(&menuObj->mainCtx, &OtosuMenu_Entry_SinglePlayerFrameDesc, NULL);
    menuObj->unk_41FE9 = 0;
    MainOvlDisp_NextProcessStage();
}

void OtosuMenu_InitForMultiplayerEnter(OtosuMenuObj* menuObj) {
    func_ov002_0208b860();

    menuObj = func_ov002_02085df8();

    menuObj->unk_460BC = 1;
    PrcCtx_Init(&menuObj->mainCtx, OtosuMenu_ObjName, sizeof(OtosuMenuObj));
    PrcCtx_SetWorkObject(&menuObj->mainCtx, menuObj);
    PrcCtx_ReplaceFrame(&menuObj->mainCtx, &OtosuMenu_Entry_MultiplayerFrameDesc, NULL);
    menuObj->unk_41FE9 = 0;
    MainOvlDisp_NextProcessStage();
}

void OtosuMenu_InitForMultiplayerRankings(OtosuMenuObj* menuObj) {
    Mem_ValidateSequences(&gMainHeap);
    Mem_ValidateSequences(&gDebugHeap);
    func_ov002_0208bd40();

    menuObj = OtosuMenu_Init();
    func_ov040_0209d990();
    menuObj->unk_41FF0 = 1;
    menuObj->unk_460BC = 1;
    PrcCtx_Init(&menuObj->mainCtx, OtosuMenu_ObjName, sizeof(OtosuMenuObj));
    PrcCtx_SetWorkObject(&menuObj->mainCtx, menuObj);
    menuObj->unk_462EC = 0;
    if (gSaveData.otosuGameKey == 0) {
        PrcCtx_ReplaceFrame(&menuObj->mainCtx, &data_ov002_02093034, NULL);
    } else {
        PrcCtx_ReplaceFrame(&menuObj->mainCtx, &data_ov002_0209305c, NULL);
    }
    func_ov002_02085a44(menuObj);
    CriSndMgr_PlayFile(ADX_B11);
    MainOvlDisp_NextProcessStage();
}

void OtosuMenu_InitForSinglePlayerRankings(OtosuMenuObj* menuObj) {
    func_ov002_0208bd40();
    menuObj = func_ov002_02085df8();
    PrcCtx_Init(&menuObj->mainCtx, OtosuMenu_ObjName, sizeof(OtosuMenuObj));
    PrcCtx_SetWorkObject(&menuObj->mainCtx, menuObj);
    menuObj->unk_462EC = 1;
    PrcCtx_ReplaceFrame(&menuObj->mainCtx, &data_ov002_02093048, NULL);
    func_ov002_02085a44(menuObj);
    CriSndMgr_PlayFile(ADX_B11);
    MainOvlDisp_NextProcessStage();
}

void OtosuMenu_InitForConnectionError(OtosuMenuObj* menuObj) {
    func_ov002_0208bd40();
    CriSndMgr_Pause(ADX_B11, 1);

    menuObj = func_ov002_02085df8();

    SystemStatusFlags;
    SystemStatusFlags.unk_06 = 1;
    SystemStatusFlags;
    SystemStatusFlags.unk_07 = 1;
    PrcCtx_Init(&menuObj->mainCtx, OtosuMenu_ObjName, sizeof(OtosuMenuObj));
    PrcCtx_SetWorkObject(&menuObj->mainCtx, menuObj);
    PrcCtx_ReplaceFrame(&menuObj->mainCtx, &data_ov002_02092e18, NULL);
    func_ov002_02085a44(menuObj);
    MainOvlDisp_NextProcessStage();
}

void OtosuMenu_InitForRoleSelection(OtosuMenuObj* menuObj) {
    func_ov002_0208bd40();

    menuObj = func_ov002_02085df8();

    menuObj->unk_460BC = 1;
    PrcCtx_Init(&menuObj->mainCtx, OtosuMenu_ObjName, sizeof(OtosuMenuObj));
    PrcCtx_SetWorkObject(&menuObj->mainCtx, menuObj);
    PrcCtx_ReplaceFrame(&menuObj->mainCtx, &OtosuMenu_RoleSelect_FrameDesc, NULL);
    func_ov002_02085a44(menuObj);
    MainOvlDisp_NextProcessStage();
}

void OtosuMenu_InitForFontList(OtosuMenuObj* menuObj) {
    func_ov002_0208bd40();

    menuObj = func_ov002_02085df8();

    PrcCtx_Init(&menuObj->mainCtx, OtosuMenu_ObjName, sizeof(OtosuMenuObj));
    PrcCtx_SetWorkObject(&menuObj->mainCtx, menuObj);
    PrcCtx_ReplaceFrame(&menuObj->mainCtx, &data_ov002_020932b8, NULL);
    func_ov002_02085a44(menuObj);
    MainOvlDisp_NextProcessStage();
}

void OtosuMenu_InitForDataDeletion(OtosuMenuObj* menuObj) {
    func_ov002_0208c228();

    menuObj = func_ov002_02085df8();

    func_ov002_02085a44(menuObj);
    PrcCtx_Init(&menuObj->mainCtx, OtosuMenu_ObjName, sizeof(OtosuMenuObj));
    PrcCtx_SetWorkObject(&menuObj->mainCtx, menuObj);
    PrcCtx_ReplaceFrame(&menuObj->mainCtx, &OtosuMenu_SaveData_DeleteFrameDesc, NULL);
    MainOvlDisp_NextProcessStage();
}

void OtosuMenu_InitForDataCorrupted(OtosuMenuObj* menuObj) {
    func_ov002_0208c228();

    menuObj = func_ov002_02085df8();

    func_ov002_02085a44(menuObj);
    PrcCtx_Init(&menuObj->mainCtx, OtosuMenu_ObjName, sizeof(OtosuMenuObj));
    PrcCtx_SetWorkObject(&menuObj->mainCtx, menuObj);
    PrcCtx_ReplaceFrame(&menuObj->mainCtx, &OtosuMenu_SaveData_CorruptedFrameDesc, NULL);
    MainOvlDisp_NextProcessStage();
}

void func_ov002_02086290(OtosuMenuObj* menuObj) {
    func_ov002_0208c228();

    menuObj = func_ov002_02085df8();

    func_ov002_02085a44(menuObj);
    PrcCtx_Init(&menuObj->mainCtx, OtosuMenu_ObjName, sizeof(OtosuMenuObj));
    PrcCtx_SetWorkObject(&menuObj->mainCtx, menuObj);
    PrcCtx_ReplaceFrame(&menuObj->mainCtx, &OtosuMenu_SaveData_InitializeFrameDesc, NULL);
    MainOvlDisp_NextProcessStage();
}

void OtosuMenu_InitForDataLoadFailure(OtosuMenuObj* menuObj) {
    func_ov002_0208c228();

    menuObj = func_ov002_02085df8();

    func_ov002_02085a44(menuObj);
    PrcCtx_Init(&menuObj->mainCtx, OtosuMenu_ObjName, sizeof(OtosuMenuObj));
    PrcCtx_SetWorkObject(&menuObj->mainCtx, menuObj);
    PrcCtx_ReplaceFrame(&menuObj->mainCtx, &OtosuMenu_SaveData_LoadFailureFrameDesc, NULL);
    MainOvlDisp_NextProcessStage();
}

void OtosuMenu_InitForDataSaveFailure(OtosuMenuObj* menuObj) {
    func_ov002_0208c228();

    menuObj = func_ov002_02085df8();

    func_ov002_02085a44(menuObj);
    PrcCtx_Init(&menuObj->mainCtx, OtosuMenu_ObjName, sizeof(OtosuMenuObj));
    PrcCtx_SetWorkObject(&menuObj->mainCtx, menuObj);
    PrcCtx_ReplaceFrame(&menuObj->mainCtx, &OtosuMenu_SaveData_SaveFailureFrameDesc, NULL);
    MainOvlDisp_NextProcessStage();
}

static inline BOOL OtosuMenu_IsResetting(void) {
    return SystemStatusFlags.reset != 0;
}

void OtosuMenu_Update(OtosuMenuObj* menuObj) {
    if (OtosuMenu_IsResetting() && menuObj->unk_460BC) {
        PrcCtx_ReplaceCurrentUpdateCallback(&menuObj->mainCtx, func_ov002_02082a44);
    }

    TouchInput_Update();
    OamMgr_Reset(&g_OamMgr[DISPLAY_MAIN], 0, 0);
    OamMgr_Reset(&g_OamMgr[DISPLAY_SUB], 0, 0);
    OamMgr_ResetCommandQueues(&g_OamMgr[DISPLAY_MAIN]);
    OamMgr_ResetCommandQueues(&g_OamMgr[DISPLAY_SUB]);

    if (!OtosuMenu_IsResetting() || (menuObj->unk_460BC == 0)) {
        PrcMaster_RunAllCtxSteps(&menuObj->prcMaster);
        EasyTask_ProcessPendingTasks(&menuObj->taskPool);
        if (PrcCtx_RunSteps(&menuObj->mainCtx) == 0) {
            OverlayTag tag;
            MainOvlDisp_Pop(&tag);
            return;
        }
        if (menuObj->cursorActive != 0) {
            Sprite_UpdateAndCheck(&menuObj->cursor);
        }
    }

    PrcMaster_UpdateAllContexts(&menuObj->prcMaster);
    PrcCtx_Update(&menuObj->mainCtx);

    switch (menuObj->nextScene) {

        case 1: {
            SystemStatusFlags;
            SystemStatusFlags.unk_06 = 1;
            SystemStatusFlags;
            SystemStatusFlags.unk_07 = 1;

            OverlayTag tag;
            MainOvlDisp_ReplaceTop(&tag, &OVERLAY_39_ID, (void*)0x02083240, NULL, 0);
        } break;

        case 2: {
            OverlayTag tag;
            MainOvlDisp_ReplaceTop(&tag, &OVERLAY_39_ID, (void*)0x02083280, NULL, 0);
        } break;

        case 5: {
            SystemStatusFlags;
            SystemStatusFlags.unk_06 = 1;
            SystemStatusFlags;
            SystemStatusFlags.unk_07 = 1;
            OverlayTag tag;
            MainOvlDisp_ReplaceTop(&tag, &OVERLAY_43_ID, (void*)0x02084040, NULL, 0);
        } break;

        case 4: {
            SystemStatusFlags;
            SystemStatusFlags.unk_06 = 1;
            SystemStatusFlags;
            SystemStatusFlags.unk_07 = 1;
            OverlayTag tag;
            MainOvlDisp_ReplaceTop(&tag, &OVERLAY_30_ID, func_ov030_020ae92c, NULL, 0);
            return;
        }

        case 3: {
            if (menuObj->unk_462EC != 0) {
                gSaveData.unk_1AB4 |= 0x20;
            } else {
                gSaveData.unk_1AB4 |= 0x40;
            }
            SystemStatusFlags;
            SystemStatusFlags.unk_06 = 1;
            SystemStatusFlags;
            SystemStatusFlags.unk_07 = 1;
            OverlayTag tag;
            MainOvlDisp_ReplaceTop(&tag, &OVERLAY_44_ID, (void*)0x02084A88 /* ProcessOverlay_Result */, NULL, 0);
            return;
        }

        case 6:
            SystemStatusFlags;
            SystemStatusFlags.unk_06 = 1;
            SystemStatusFlags;
            SystemStatusFlags.unk_07 = 1;
            OverlayTag tag;
            MainOvlDisp_ReplaceTop(&tag, &OVERLAY_37_ID, 0x0208370C, NULL, 0);
            return;

        default:
            OS_WaitForever();

        case 0: {
            PrcMaster_RenderAllContexts(&menuObj->prcMaster);
            EasyTask_UpdateActiveTasks(&menuObj->taskPool);
            PrcCtx_Render(&menuObj->mainCtx);
            if (menuObj->cursorActive != 0) {
                Sprite_Render(&menuObj->cursor);
            }
            OamMgr_FlushCommands(&g_OamMgr[DISPLAY_MAIN]);
            OamMgr_FlushCommands(&g_OamMgr[DISPLAY_SUB]);
            PaletteMgr_Flush(g_PaletteManagers[0], 0);
            PaletteMgr_Flush(g_PaletteManagers[1], 0);
            func_0200d90c();
            if (menuObj->unk_460BC != 0) {
                func_ov002_02085ac4(menuObj);
            }
        } break;
    }
}

void OtosuMenu_Destroy(OtosuMenuObj* menuObj) {
    CriSndMgr_Pause(ADX_B11, 1);
    if (menuObj->cursorActive != 0) {
        func_ov002_0208264c(menuObj);
    }
    EasyTask_DestroyPool(&menuObj->taskPool);
    PrcCtx_Destroy(&menuObj->mainCtx);
    PrcMaster_Destroy(&menuObj->prcMaster);
    ResourceMgr_ReinitManagers(NULL);
    DatMgr_ClearSlot(menuObj->unk_11584);
    DatMgr_ClearSlot(menuObj->unk_1158C);
    DatMgr_ClearSlot(menuObj->unk_11588);
    Interrupts_RegisterVBlankCallback(NULL, 1);
    Interrupts_RegisterHBlankCallback(0, 1);
    SysFont_Destroy(&menuObj->font);
    FS_UnloadOverlay(0, &OVERLAY_31_ID);
    if (menuObj->unk_41FF0 != 0) {
        OvlMgr_UnloadOverlay(3);
    }
    Mem_Free(&gMainHeap, menuObj);
    Mem_ValidateSequences(&gMainHeap);
    Mem_ValidateSequences(&gDebugHeap);
}

static const OverlayProcess OvlProc_OtosuMenu_MultiplayerEnter = {
    .init = OtosuMenu_InitForMultiplayerEnter,
    .main = OtosuMenu_Update,
    .exit = OtosuMenu_Destroy,
};

void ProcessOverlay_OtosuMenu_SinglePlayerEnter(void* menuObj) {
    s32 stage = MainOvlDisp_GetProcessStage();
    if (stage == PROCESS_STAGE_EXIT) {
        OtosuMenu_Destroy(menuObj);
    } else {
        OvlProc_OtosuMenu_SinglePlayerEnter.funcs[stage](menuObj);
    }
}

static const OverlayProcess OvlProc_OtosuMenu_ConnectionError = {
    .init = OtosuMenu_InitForConnectionError,
    .main = OtosuMenu_Update,
    .exit = OtosuMenu_Destroy,
};

void ProcessOverlay_OtosuMenu_MultiplayerEnter(void* arg0) {
    s32 stage = MainOvlDisp_GetProcessStage();
    if (stage == PROCESS_STAGE_EXIT) {
        OtosuMenu_Destroy(arg0);
    } else {
        OvlProc_OtosuMenu_MultiplayerEnter.funcs[stage](arg0);
    }
}

static const OverlayProcess OvlProc_OtosuMenu_DataLoadFailure = {
    .init = OtosuMenu_InitForDataLoadFailure,
    .main = OtosuMenu_Update,
    .exit = OtosuMenu_Destroy,
};

void ProcessOverlay_OtosuMenu_MultiplayerRanking(void* arg0) {
    s32 stage = MainOvlDisp_GetProcessStage();
    if (stage == PROCESS_STAGE_EXIT) {
        OtosuMenu_Destroy(arg0);
    } else {
        OvlProc_OtosuMenu_MultiplayerRanking.funcs[stage](arg0);
    }
}

static const OverlayProcess OvlProc_OtosuMenu_DataDeletion = {
    .init = OtosuMenu_InitForDataDeletion,
    .main = OtosuMenu_Update,
    .exit = OtosuMenu_Destroy,
};

void ProcessOverlay_OtosuMenu_SinglePlayerRanking(void* menuObj) {
    s32 stage = MainOvlDisp_GetProcessStage();
    if (stage == PROCESS_STAGE_EXIT) {
        OtosuMenu_Destroy(menuObj);
    } else {
        OvlProc_OtosuMenu_SinglePlayerRanking.funcs[stage](menuObj);
    }
}

static const OverlayProcess data_ov002_02091b10 = {
    .init = func_ov002_02086290,
    .main = OtosuMenu_Update,
    .exit = OtosuMenu_Destroy,
};

void ProcessOverlay_OtosuMenu_ConnectionError(void* menuObj) {
    s32 stage = MainOvlDisp_GetProcessStage();
    if (stage == PROCESS_STAGE_EXIT) {
        OtosuMenu_Destroy(menuObj);
    } else {
        OvlProc_OtosuMenu_ConnectionError.funcs[stage](menuObj);
    }
}

static const OverlayProcess OvlProc_OtosuMenu_FontList = {
    .init = OtosuMenu_InitForFontList,
    .main = OtosuMenu_Update,
    .exit = OtosuMenu_Destroy,
};

void ProcessOverlay_OtosuMenu_RoleSelection(void* menuObj) {
    s32 stage = MainOvlDisp_GetProcessStage();
    if (stage == PROCESS_STAGE_EXIT) {
        OtosuMenu_Destroy(menuObj);
    } else {
        OvlProc_OtosuMenu_RoleSelection.funcs[stage](menuObj);
    }
}

static const OverlayProcess OvlProc_OtosuMenu_DataCorrupted = {
    .init = OtosuMenu_InitForDataCorrupted,
    .main = OtosuMenu_Update,
    .exit = OtosuMenu_Destroy,
};

void ProcessOverlay_OtosuMenu_FontList(void* menuObj) {
    s32 stage = MainOvlDisp_GetProcessStage();
    if (stage == PROCESS_STAGE_EXIT) {
        OtosuMenu_Destroy(menuObj);
    } else {
        OvlProc_OtosuMenu_FontList.funcs[stage](menuObj);
    }
}

const BinIdentifier data_ov002_02091c34 = {0x1E, (char*)data_ov002_02092bb0}; /* const */

void ProcessOverlay_OtosuMenu_DataDeletion(void* menuObj) {
    s32 stage = MainOvlDisp_GetProcessStage();
    if (stage == PROCESS_STAGE_EXIT) {
        OtosuMenu_Destroy(menuObj);
    } else {
        OvlProc_OtosuMenu_DataDeletion.funcs[stage](menuObj);
    }
}

static const OverlayProcess OvlProc_OtosuMenu_DataSaveFailure = {
    .init = OtosuMenu_InitForDataSaveFailure,
    .main = OtosuMenu_Update,
    .exit = OtosuMenu_Destroy,
};

void ProcessOverlay_OtosuMenu_DataCorrupted(void* menuObj) {
    s32 stage = MainOvlDisp_GetProcessStage();
    if (stage == PROCESS_STAGE_EXIT) {
        OtosuMenu_Destroy(menuObj);
    } else {
        OvlProc_OtosuMenu_DataCorrupted.funcs[stage](menuObj);
    }
}

const BinIdentifier data_ov002_02091c24 = {0x2B, "Apl_Tak/Grp_Menu_BGD.bin"}; /* const */

void func_ov002_02086acc(void* menuObj) {
    s32 stage = MainOvlDisp_GetProcessStage();
    if (stage == PROCESS_STAGE_EXIT) {
        OtosuMenu_Destroy(menuObj);
    } else {
        data_ov002_02091b10.funcs[stage](menuObj);
    }
}

const BinIdentifier data_ov002_02091c2c = {0x2B, "Apl_Tak/Grp_Menu_BGU.bin"}; /* const */

void ProcessOverlay_OtosuMenu_DataLoadFailure(void* menuObj) {
    s32 stage = MainOvlDisp_GetProcessStage();
    if (stage == PROCESS_STAGE_EXIT) {
        OtosuMenu_Destroy(menuObj);
    } else {
        OvlProc_OtosuMenu_DataLoadFailure.funcs[stage](menuObj);
    }
}

PrcStepFn PrcSteps_FadeDark[] = {
    OtosuPrcStep_FadeStart_Dark,
    OtosuPrcStep_FadeWait,
    PrcStep_PopFrame,
};

void ProcessOverlay_OtosuMenu_DataSaveFailure(void* menuObj) {
    s32 stage = MainOvlDisp_GetProcessStage();
    if (stage == PROCESS_STAGE_EXIT) {
        OtosuMenu_Destroy(menuObj);
    } else {
        OvlProc_OtosuMenu_DataSaveFailure.funcs[stage](menuObj);
    }
}
