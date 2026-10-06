#include "OtosuMenuShared.h"

static char data_ov002_02092bb0[32] = "Apl_Kit/GRP_FldDownScreen.bin";

static const Ov002_U16_20 data_ov002_02091b64 = {0x23FB, 0x0017, 0x0049, 0x0049, 0x0055, 0x23FC, 0x0067,
                                                 0x0049, 0x0098, 0x0055, 0x23FD, 0x00B7, 0x0049, 0x00E9,
                                                 0x0055, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000}; /* const */

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

static const Ov002_U16_4     data_ov002_02091ac4 = {0x0010, 0x0011, 0x0012, 0x0013};
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
static u8        data_ov002_020934fa[0xC6];
static const u16 data_ov002_02091ab4[4] = {0x0004, 0x0018, 0x002C, 0x0040};

static const Ov002_U16_4 data_ov002_02091abc = {0x0001, 0x0002, 0x0003, 0x0004};

const BinIdentifier         data_ov002_02091aac                 = {0x02, "Apl_Fuk/Grp_OtosuMenu.bin"};
static const OverlayProcess OvlProc_OtosuMenu_SinglePlayerEnter = {
    .init = OtosuMenu_InitForSinglePlayerEnter,
    .main = OtosuMenu_Update,
    .exit = OtosuMenu_Destroy,
};
static const Ov002_U16_4 data_ov002_02091aa4 = {0x000C, 0x000D, 0x000E, 0x000F}; /* const */

const BinIdentifier data_ov002_02091acc = {2, "Apl_Fuk/Grp_OtosuMenuObj.bin"};

static const Ov002_U16_50 data_ov002_02091bb8 = {
    0x23FE, 0x0010, 0x0062, 0x0050, 0x0072, 0x23FF, 0x0010, 0x0082, 0x0050, 0x0092, 0x2400, 0x0010, 0x00A2,
    0x0050, 0x00B2, 0x2401, 0x0060, 0x0062, 0x00A0, 0x0072, 0x2402, 0x0060, 0x0082, 0x00A0, 0x0092, 0x2403,
    0x0060, 0x00A2, 0x00A0, 0x00B2, 0x2404, 0x00B0, 0x0062, 0x00F0, 0x0072, 0x2405, 0x00B0, 0x0082, 0x00F0,
    0x0092, 0x2406, 0x00B0, 0x00A2, 0x00F0, 0x00B2, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000}; /* const */

static u8                   data_ov002_020934e4[0x16];
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

    if (_Sprite_Load(&menuObj->unk_46078, &anim) == 0) {
        OS_WaitForever();
    }
    menuObj->unk_460B8 = 1;
}

void func_ov002_02082610(OtosuMenuObj* menuObj) {
    menuObj->unk_460B8 = 0;
    Sprite_Destroy(&menuObj->unk_46078);
    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->unk_46100);
}

void func_ov002_0208264c(OtosuMenuObj* menuObj) {
    menuObj->unk_460B8 = 0;
    Sprite_Destroy(&menuObj->unk_46078);
}

PrcStepResult func_ov002_0208266c(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    ((u8*)&data_02074d10)[0x40A] = func_ov002_02082bec(menuObj);
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_REPEAT;
}

PrcStepResult func_ov002_02082698(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    MI_CpuCopyU8(&menuObj->unk_41862, &data_020750fc, 6);
    MI_CpuCopyU8(&menuObj->unk_41862, &menuObj->unk_41FB6, 6);
    func_ov002_02082d44(menuObj);

    data_02074d10.unk_40B = menuObj->unk_4198A;
    data_02074d10.unk_40C = menuObj->unk_4196C;

    for (int i = 0; i < 4; i++) {
        MI_CpuCopyU8(&menuObj->unk_41862 + i * 0x30, (u8*)&data_02075102 + i * 6, 6);
    }

    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_REPEAT;
}

PrcStepResult func_ov002_0208275c(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    func_ov002_020824a0();
    MI_CpuCopyU8(&data_020750fc, &menuObj->unk_41862, 6);

    menuObj->unk_4198A = data_02074d10.unk_40B;
    menuObj->unk_4196C = data_02074d10.unk_40C;

    for (int i = 0; i < 4; i++) {
        MI_CpuFillU16(-1, (u8*)menuObj->unk_41838 + i * 0x30, 0x16);
    }

    void* paletteData = SysFont_GetOwnerName();
    MI_CpuCopyU8(paletteData, &menuObj->unk_4196E, 0x16);
    MI_CpuCopyU8(paletteData, (u8*)menuObj->unk_41838 + (data_02074d10.unk_40A * 0x30), 0x16);
    MI_CpuCopyU8(paletteData, &menuObj->unk_4181C, 0x16);

    menuObj->unk_41832 = data_02074d10.unk_40A;
    Mem_Free(&gDebugHeap, paletteData);

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
    u16 temp_values[4];
    temp_values = data_ov002_02091ab4;

    MI_CpuFillU16(0, menuObj->unk_41FF4, 0x2800);
    MI_CpuFillU16(0, data_ov002_020934e4, 22);

    for (u16 i = 0; i < 4; i++) {
        if ((menuObj->unk_41834 & (1 << i)) == 0) {
            *(u16*)((u8*)&data_ov002_020934e0 + 4) = 0xFFFF;
            continue;
        }

        SysFont_SetMsgPtr(&menuObj->font, menuObj->unk_41838 + i * 48);
        SysFont_SetPos(&menuObj->font, 0, temp_values[i]);
        SysFont_SetHAlign(&menuObj->font, 0, 256);
        SysFont_SetVAlign(&menuObj->font, 3, 80);
        SysFont_DrawCurrentToChar(&menuObj->font, menuObj->unk_41FF4, 32, 10);
    }

    func_0203abec(3, menuObj->unk_41FF4, (u8*)G2_GetBG0CharPtr() + 0x2400, 0x2800);
}

u8 func_ov002_02082bec(OtosuMenuObj* menuObj) {
    u8 keys[6];
    u8 match_index = 0xFF;
    u8 i;

    func_0203a96c(keys);
    if (menuObj->unk_462EC != 0) {
        return 0;
    }

    for (i = 0; i < 4; i++) {
        u8* entry;

        if ((menuObj->unk_41834 & (1 << i)) == 0) {
            continue;
        }

        entry = (u8*)menuObj->unk_41862 + i * 0x30;
        if (keys[0] != entry[0] || keys[1] != entry[1] || keys[2] != entry[2] || keys[3] != entry[3] || keys[4] != entry[4] ||
            keys[5] != entry[5])
        {
            continue;
        }
        match_index = i;
        break;
    }

    if (match_index == 0xFF) {
        OS_WaitForever();
    }
    return match_index;
}

void func_ov002_02082d44(OtosuMenuObj* menuObj) {
    u16 i;
    u16 write_index = 0;

    for (i = 0; i < 4; i++) {
        if ((menuObj->unk_41834 & (1 << i)) != 0) {
            MI_CpuCopyU8(&menuObj->unk_41FB6 + (i * 6), &menuObj->unk_41862 + (write_index * 0x30), 6);
            write_index++;
        }
    }
}

void func_ov002_02082dbc(void* arg0, const Ov002_U16_5* arg1, void* arg2, void* arg3) {
    const Ov002_U16_5* entry = arg1;

    if (entry->unk0 == 0xFFFF) {
        return;
    }
    do {
        SysFont_SetMsg(arg0, entry->unk0);
        SysFont_SetPos(arg0, entry->unk2, entry->unk4);
        SysFont_SetHAlign(arg0, 0, (u16)(entry->unk6 - entry->unk2));
        SysFont_SetVAlign(arg0, 0, (u16)(entry->unk8 - entry->unk4));
        SysFont_DrawCurrentToScreen(arg0, arg3, arg2, 0);
        entry++;
    } while (entry->unk0 != 0xFFFF);
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

void func_ov002_02082f18(OtosuMenuObj* menuObj, s32 arg1, s32 arg2, void* arg3) {
    func_ov002_02085710(menuObj);
    g_DisplaySettings.engineState[1].bgSettings[0].priority = 0;
    g_DisplaySettings.engineState[1].bgSettings[1].priority = 1;
    g_DisplaySettings.engineState[1].bgSettings[2].priority = 2;
    g_DisplaySettings.engineState[1].bgSettings[3].priority = 3;

    menuObj->unk_462F0 = DatMgr_LoadPackEntry(1, 0, 0, &data_ov002_02091aac, 1, 0);

    Display_SetMainLayers(LAYER_BG0 | LAYER_BG2 | LAYER_BG3 | LAYER_OBJ);
    Display_SetSubLayers(LAYER_BG2 | LAYER_BG3 | LAYER_OBJ);

    menuObj->unk_47340 =
        PaletteMgr_AllocPalette(g_PaletteManagers[1], Data_GetPackEntryData(menuObj->unk_462F0, 4), 0, 0, 0x10);
    PaletteMgr_Flush(g_PaletteManagers[1], menuObj->unk_47340);

    menuObj->unk_4732C = BgResMgr_AllocChar32(g_BgResourceManagers[1], Data_GetPackEntryData(menuObj->unk_462F0, 10),
                                              g_DisplaySettings.engineState[1].bgSettings[3].charBase, 0, 0x6000);

    menuObj->unk_474A0 = Data_GetPackEntryData(menuObj->unk_462F0, 13) + 4;
    func_0200d1d8(&menuObj->unk_473C0, 1, 3, 0, &menuObj->unk_474A0, 1, 1);

    menuObj->unk_47328 = BgResMgr_AllocChar32(g_BgResourceManagers[1], Data_GetPackEntryData(menuObj->unk_462F0, 11),
                                              g_DisplaySettings.engineState[1].bgSettings[2].charBase, 0, 0x4D20);

    menuObj->unk_47498 = Data_GetPackEntryData(menuObj->unk_462F0, 14) + 4;
    func_0200d1d8(&menuObj->unk_47398, 1, 2, 0, &menuObj->unk_47498, 1, 1);

    menuObj->unk_47344 =
        PaletteMgr_AllocPalette(g_PaletteManagers[0], Data_GetPackEntryData(menuObj->unk_462F0, 3), 0, 0, 0x10);
    PaletteMgr_Flush(g_PaletteManagers[0], menuObj->unk_47344);

    menuObj->unk_4733C = BgResMgr_AllocChar32(g_BgResourceManagers[0], Data_GetPackEntryData(menuObj->unk_462F0, 16),
                                              g_DisplaySettings.engineState[0].bgSettings[3].charBase, 0, 0x6000);

    menuObj->unk_474C0 = Data_GetPackEntryData(menuObj->unk_462F0, 19) + 4;
    func_0200d1d8(&menuObj->unk_47460, 0, 3, 0, &menuObj->unk_474C0, 1, 1);

    menuObj->unk_47338 = BgResMgr_AllocChar32(g_BgResourceManagers[0], Data_GetPackEntryData(menuObj->unk_462F0, 17),
                                              g_DisplaySettings.engineState[0].bgSettings[2].charBase, 0, 0x8000);

    menuObj->unk_474B8 = Data_GetPackEntryData(menuObj->unk_462F0, arg2) + 4;
    func_0200d1d8(&menuObj->unk_47438, 0, 2, 0, &menuObj->unk_474B8, 1, 1);

    if (arg1 != 0xFFFF) {
        menuObj->unk_474B0 = Data_GetPackEntryData(menuObj->unk_462F0, arg1) + 4;
        func_0200d1d8(&menuObj->unk_47410, 0, 1, 0, &menuObj->unk_474B0, 1, 1);
        g_DisplaySettings.controls[0].layers |= 2;
    } else {
        g_DisplaySettings.controls[0].layers &= ~2;
    }

    void* ptr          = Data_GetPackEntryData(menuObj->unk_462F0, 7);
    menuObj->unk_474A8 = Data_GetPackEntryData(menuObj->unk_462F0, 8) + 4;

    SysFont_SetColor(&menuObj->font, 1);
    func_ov002_02082dbc(&menuObj->font, (const Ov002_U16_5*)arg3, ptr + 4, menuObj->unk_474A8);

    menuObj->unk_47330 =
        BgResMgr_AllocChar32(g_BgResourceManagers[0], ptr, g_DisplaySettings.engineState[0].bgSettings[0].charBase, 0, 0x6000);
    func_0200d1d8(&menuObj->unk_473E8, 0, 0, 0, &menuObj->unk_474A8, 1, 1);
    Display_Commit();
}

void func_ov002_02083484(OtosuMenuObj* menuObj, u16* arg1) {
    g_DisplaySettings.engineState[1].bgSettings[0].priority = 0;
    g_DisplaySettings.engineState[1].bgSettings[1].priority = 1;
    g_DisplaySettings.engineState[1].bgSettings[2].priority = 2;
    g_DisplaySettings.engineState[1].bgSettings[3].priority = 3;

    menuObj->unk_462F0 = DatMgr_LoadPackEntry(1, 0, 0, &data_ov002_02091aac, 1, 0);
    menuObj->unk_462F4 = DatMgr_LoadPackEntry(1, 0, 0, &data_ov002_02091aac, 5, 0);

    Display_SetMainLayers(LAYER_NONE);
    Display_SetSubLayers(LAYER_BG1);

    menuObj->unk_47340 =
        PaletteMgr_AllocPalette(g_PaletteManagers[1], Data_GetPackEntryData(menuObj->unk_462F4, 6), 0, 0, 0x10);
    PaletteMgr_Flush(g_PaletteManagers[1], menuObj->unk_47340);

    void* table_ptr    = Data_GetPackEntryData(menuObj->unk_462F0, 5);
    menuObj->unk_47490 = Data_GetPackEntryData(menuObj->unk_462F0, 6) + 4;

    SysFont_SetColor(&menuObj->font, 1);
    SysFont_SetLineSpacing(&menuObj->font, 4);
    SysFont_SetSpacing(&menuObj->font, TRUE, 0);
    func_ov002_02082dbc(&menuObj->font, (const Ov002_U16_5*)arg1, table_ptr + 4, menuObj->unk_47490);
    SysFont_SetLineSpacing(&menuObj->font, 2);
    SysFont_SetSpacing(&menuObj->font, TRUE, 0);

    menuObj->unk_47324 = BgResMgr_AllocChar32(g_BgResourceManagers[1], table_ptr,
                                              g_DisplaySettings.engineState[1].bgSettings[1].charBase, 0, 0x6000);
    func_0200d1d8(&menuObj->unk_47370, 1, 1, 0, &menuObj->unk_47490, 1, 1);
    Display_Commit();
}

void func_ov002_02083694(OtosuMenuObj* menuObj) {
    func_ov002_02085710(menuObj);

    menuObj->unk_462F0 = DatMgr_LoadPackEntry(1, 0, 0, &data_ov002_02091aac, 1, 0);
    menuObj->unk_462F4 = DatMgr_LoadPackEntry(1, 0, 0, &data_ov002_02091aac, 2, 0);

    menuObj->unk_47340 =
        PaletteMgr_AllocPalette(g_PaletteManagers[1], Data_GetPackEntryData(menuObj->unk_462F4, 1), 0, 0, 0x10);

    PaletteMgr_Flush(g_PaletteManagers[1], menuObj->unk_47340);

    menuObj->unk_4732C = BgResMgr_AllocChar32(g_BgResourceManagers[1], Data_GetPackEntryData(menuObj->unk_462F4, 4),
                                              g_DisplaySettings.engineState[1].bgSettings[3].charBase, 0, 0x5800);

    menuObj->unk_474A0 = Data_GetPackEntryData(menuObj->unk_462F4, 5) + 4;
    func_0200d1d8(&menuObj->unk_473C0, 1, 3, 0, &menuObj->unk_474A0, 1, 1);

    menuObj->unk_47328 = BgResMgr_AllocChar32(g_BgResourceManagers[1], Data_GetPackEntryData(menuObj->unk_462F4, 2),
                                              g_DisplaySettings.engineState[1].bgSettings[2].charBase, 0, 0x8000);

    menuObj->unk_47498 = Data_GetPackEntryData(menuObj->unk_462F4, 3) + 4;
    func_0200d1d8(&menuObj->unk_47398, 1, 2, 0, &menuObj->unk_47498, 1, 1);

    menuObj->unk_47344 =
        PaletteMgr_AllocPalette(g_PaletteManagers[0], Data_GetPackEntryData(menuObj->unk_462F4, 6), 0, 0, 0x10);
    PaletteMgr_Flush(g_PaletteManagers[0], menuObj->unk_47344);

    menuObj->unk_4733C = BgResMgr_AllocChar32(g_BgResourceManagers[0], Data_GetPackEntryData(menuObj->unk_462F4, 9),
                                              g_DisplaySettings.engineState[0].bgSettings[3].charBase, 0, 0x6000);

    menuObj->unk_474C0 = Data_GetPackEntryData(menuObj->unk_462F4, 10) + 4;
    func_0200d1d8(&menuObj->unk_47460, 0, 3, 0, &menuObj->unk_474C0, 1, 1);

    menuObj->unk_47338 = BgResMgr_AllocChar32(g_BgResourceManagers[0], Data_GetPackEntryData(menuObj->unk_462F4, 7),
                                              g_DisplaySettings.engineState[0].bgSettings[2].charBase, 0, 0x8000);

    menuObj->unk_474B8 = Data_GetPackEntryData(menuObj->unk_462F4, 8) + 4;
    func_0200d1d8(&menuObj->unk_47438, 0, 2, 0, &menuObj->unk_474B8, 1, 1);

    Display_SetMainLayers(LAYER_BG2 | LAYER_BG3 | LAYER_OBJ);
    Display_SetSubLayers(LAYER_BG2 | LAYER_BG3 | LAYER_OBJ);
    Display_Commit();
}

void func_ov002_02083a74(OtosuMenuObj* menuObj) {
    func_ov002_02085710(menuObj);

    menuObj->unk_462F0 = DatMgr_LoadPackEntry(1, 0, 0, &data_ov002_02091aac, 1, 0);
    menuObj->unk_462F4 = DatMgr_LoadPackEntry(1, 0, 0, &data_ov002_02091aac, 3, 0);

    menuObj->unk_47340 =
        PaletteMgr_AllocPalette(g_PaletteManagers[1], Data_GetPackEntryData(menuObj->unk_462F4, 1), 0, 0, 0x10);
    PaletteMgr_Flush(g_PaletteManagers[1], menuObj->unk_47340);

    menuObj->unk_4732C = BgResMgr_AllocChar32(g_BgResourceManagers[1], Data_GetPackEntryData(menuObj->unk_462F0, 9),
                                              g_DisplaySettings.engineState[1].bgSettings[3].charBase, 0, 0x6000);

    menuObj->unk_474A0 = Data_GetPackEntryData(menuObj->unk_462F0, 12) + 4;
    func_0200d1d8(&menuObj->unk_473C0, 1, 3, 0, &menuObj->unk_474A0, 1, 1);

    menuObj->unk_47328 = BgResMgr_AllocChar32(g_BgResourceManagers[1], Data_GetPackEntryData(menuObj->unk_462F4, 3),
                                              g_DisplaySettings.engineState[1].bgSettings[2].charBase, 0, 0x8000);

    menuObj->unk_47498 = Data_GetPackEntryData(menuObj->unk_462F4, 5) + 4;
    menuObj->unk_4749C = Data_GetPackEntryData(menuObj->unk_462F4, 12) + 4;

    func_0200d1d8(&menuObj->unk_47398, 1, 2, 0, &menuObj->unk_47498, 1, 2);
    func_0200d858(&menuObj->unk_47398, 0, 0, 0);

    Display_SetBGOffset(menuObj->unk_47398, menuObj->unk_4739C, 0x200000, 0x200000);

    menuObj->unk_47324 = BgResMgr_AllocChar32(g_BgResourceManagers[1], Data_GetPackEntryData(menuObj->unk_462F0, 11),
                                              g_DisplaySettings.engineState[1].bgSettings[1].charBase, 0, 0x4D20);

    menuObj->unk_47490 = Data_GetPackEntryData(menuObj->unk_462F0, 14) + 4;
    func_0200d1d8(&menuObj->unk_47370, 1, 1, 0, &menuObj->unk_47490, 1, 1);
    func_0200d858(&menuObj->unk_47370, 0, 0, 0);

    Display_SetBGOffset(menuObj->unk_47370, menuObj->unk_47374, 0x200000, 0x200000);

    void* var_r1_5     = Data_GetPackEntryData(menuObj->unk_462F4, 2);
    menuObj->unk_47488 = Data_GetPackEntryData(menuObj->unk_462F4, 4) + 4;
    menuObj->unk_47320 = BgResMgr_AllocChar32(g_BgResourceManagers[1], var_r1_5,
                                              g_DisplaySettings.engineState[1].bgSettings[0].charBase, 0, 0x11A0);
    func_0200d1d8(&menuObj->unk_47348, 1, 0, 0, &menuObj->unk_47488, 1, 1);
    func_0200d858(&menuObj->unk_47348, 0, 0, 0);

    Display_SetBGOffset(menuObj->unk_47348, menuObj->unk_4734C, 0x200000, 0x200000);

    menuObj->unk_47344 =
        PaletteMgr_AllocPalette(g_PaletteManagers[0], Data_GetPackEntryData(menuObj->unk_462F4, 6), 0, 0, 0x10);
    PaletteMgr_Flush(g_PaletteManagers[0], menuObj->unk_47344);

    menuObj->unk_4733C = BgResMgr_AllocChar32(g_BgResourceManagers[0], Data_GetPackEntryData(menuObj->unk_462F0, 15),
                                              g_DisplaySettings.engineState[0].bgSettings[3].charBase, 0, 0x6000);

    menuObj->unk_474C0 = Data_GetPackEntryData(menuObj->unk_462F0, 18) + 4;
    func_0200d1d8(&menuObj->unk_47460, 0, 3, 0, &menuObj->unk_474C0, 1, 1);

    menuObj->unk_47338 = BgResMgr_AllocChar32(g_BgResourceManagers[0], Data_GetPackEntryData(menuObj->unk_462F4, 3),
                                              g_DisplaySettings.engineState[0].bgSettings[2].charBase, 0, 0x8000);

    menuObj->unk_474B8 = Data_GetPackEntryData(menuObj->unk_462F4, 5) + 4;
    menuObj->unk_474BC = Data_GetPackEntryData(menuObj->unk_462F4, 12) + 4;
    func_0200d1d8(&menuObj->unk_47438, 0, 2, 0, &menuObj->unk_474B8, 1, 2);
    func_0200d858(&menuObj->unk_47438, 0, 0, 0);

    Display_SetBGOffset(menuObj->unk_47438, menuObj->unk_4743C, 0x200000, 0x200000);

    menuObj->unk_47334 = BgResMgr_AllocChar32(g_BgResourceManagers[0], Data_GetPackEntryData(menuObj->unk_462F4, 8),
                                              g_DisplaySettings.engineState[0].bgSettings[1].charBase, 0, 0x3EA0);

    menuObj->unk_474B0 = Data_GetPackEntryData(menuObj->unk_462F4, 11) + 4;
    func_0200d1d8(&menuObj->unk_47410, 0, 1, 0, &menuObj->unk_474B0, 1, 1);
    func_0200d858(&menuObj->unk_47410, 0, 0, 0);

    Display_SetBGOffset(menuObj->unk_47410, menuObj->unk_47414, 0x200000, 0x200000);

    void* var_r1_10 = Data_GetPackEntryData(menuObj->unk_462F4, 7);

    menuObj->unk_474A8 = Data_GetPackEntryData(menuObj->unk_462F4, 9) + 4;
    menuObj->unk_474AC = Data_GetPackEntryData(menuObj->unk_462F4, 10) + 4;

    menuObj->unk_47330 = BgResMgr_AllocChar32(g_BgResourceManagers[0], var_r1_10,
                                              g_DisplaySettings.engineState[0].bgSettings[0].charBase, 0, 0x8000);
    func_0200d1d8(&menuObj->unk_473E8, 0, 0, 0, &menuObj->unk_474A8, 2, 1);
    func_0200d858(&menuObj->unk_473E8, 0, 0, 0);

    Display_SetBGOffset(menuObj->unk_473E8, menuObj->unk_473EC, 0x200000, 0x200000);

    Display_SetMainLayers(LAYER_BG0 | LAYER_BG1 | LAYER_BG2 | LAYER_BG3 | LAYER_OBJ);
    Display_SetSubLayers(LAYER_BG0 | LAYER_BG1 | LAYER_BG2 | LAYER_BG3 | LAYER_OBJ);

    Display_Commit();
}

void func_ov002_02084494(OtosuMenuObj* menuObj, u8 arg1, u16* arg2) {
    Ov002_U16_20 header = data_ov002_02091b64;
    u16          subPal[4];
    Ov002_U16_50 slots = data_ov002_02091bb8;
    u16          subBg1[4];
    u16          mainPal[4];

    func_ov002_02085710(menuObj);

    g_DisplaySettings.engineState[1].bgSettings[0].priority = 3;
    g_DisplaySettings.engineState[1].bgSettings[1].priority = 0;
    g_DisplaySettings.engineState[1].bgSettings[2].priority = 1;
    g_DisplaySettings.engineState[1].bgSettings[3].priority = 2;

    menuObj->unk_462F0 = DatMgr_LoadPackEntry(1, 0, 0, &data_ov002_02091aac, 1, 0);
    menuObj->unk_462F4 = DatMgr_LoadPackEntry(1, 0, 0, &data_ov002_02091aac, 4, 0);

    subPal[0]   = data_ov002_02091abc.data[0];
    subPal[1]   = data_ov002_02091abc.data[1];
    subPal[3]   = data_ov002_02091abc.data[3];
    subPal[2]   = data_ov002_02091abc.data[2];
    u16 temp_r2 = subPal[arg1];

    menuObj->unk_47340 =
        PaletteMgr_AllocPalette(g_PaletteManagers[1], Data_GetPackEntryData(menuObj->unk_462F4, temp_r2), 0, 0, 0x10);
    PaletteMgr_Flush(g_PaletteManagers[1], menuObj->unk_47340);

    menuObj->unk_4732C = BgResMgr_AllocChar32(g_BgResourceManagers[1], Data_GetPackEntryData(menuObj->unk_462F4, 6),
                                              g_DisplaySettings.engineState[1].bgSettings[3].charBase, 0, 0x8000);

    menuObj->unk_474A0 = Data_GetPackEntryData(menuObj->unk_462F4, 10) + 4;
    func_0200d1d8(&menuObj->unk_473C0, 1, 3, 0, &menuObj->unk_474A0, 1, 1);

    menuObj->unk_47328 = BgResMgr_AllocChar32(g_BgResourceManagers[1], Data_GetPackEntryData(menuObj->unk_462F4, 7),
                                              g_DisplaySettings.engineState[1].bgSettings[2].charBase, 0, 0x6000);

    menuObj->unk_47498 = Data_GetPackEntryData(menuObj->unk_462F4, 11) + 4;
    func_0200d1d8(&menuObj->unk_47398, 1, 2, 0, &menuObj->unk_47498, 1, 1);

    menuObj->unk_47324 = BgResMgr_AllocChar32(g_BgResourceManagers[1], Data_GetPackEntryData(menuObj->unk_462F4, 9),
                                              g_DisplaySettings.engineState[1].bgSettings[1].charBase, 0, 0x8000);

    subBg1[3]     = data_ov002_02091aa4.data[3];
    subBg1[0]     = data_ov002_02091aa4.data[0];
    subBg1[1]     = data_ov002_02091aa4.data[1];
    subBg1[2]     = data_ov002_02091aa4.data[2];
    u16 temp_r2_2 = subBg1[arg1];

    menuObj->unk_47490 = Data_GetPackEntryData(menuObj->unk_462F4, temp_r2_2) + 4;
    func_0200d1d8(&menuObj->unk_47370, 1, 1, 0, &menuObj->unk_47490, 1, 1);

    menuObj->unk_47320 = BgResMgr_AllocChar32(g_BgResourceManagers[1], Data_GetPackEntryData(menuObj->unk_462F4, 8),
                                              g_DisplaySettings.engineState[1].bgSettings[0].charBase, 0, 0x200);

    Display_SetSubLayers(LAYER_BG0 | LAYER_BG1 | LAYER_BG2 | LAYER_BG3 | LAYER_OBJ);

    mainPal[3]    = data_ov002_02091ac4.data[3];
    mainPal[0]    = data_ov002_02091ac4.data[0];
    mainPal[1]    = data_ov002_02091ac4.data[1];
    mainPal[2]    = data_ov002_02091ac4.data[2];
    u16 temp_r2_3 = mainPal[arg1];

    menuObj->unk_47344 =
        PaletteMgr_AllocPalette(g_PaletteManagers[0], Data_GetPackEntryData(menuObj->unk_462F4, temp_r2_3), 0, 0, 0x10);
    PaletteMgr_Flush(g_PaletteManagers[0], menuObj->unk_47344);

    menuObj->unk_4733C = BgResMgr_AllocChar32(g_BgResourceManagers[0], Data_GetPackEntryData(menuObj->unk_462F4, 20),
                                              g_DisplaySettings.engineState[0].bgSettings[3].charBase, 0, 0x6000);

    menuObj->unk_474C0 = Data_GetPackEntryData(menuObj->unk_462F4, 21) + 4;
    func_0200d1d8(&menuObj->unk_47460, 0, 3, 0, &menuObj->unk_474C0, 1, 1);

    menuObj->unk_47338 = BgResMgr_AllocChar32(g_BgResourceManagers[0], Data_GetPackEntryData(menuObj->unk_462F4, 9),
                                              g_DisplaySettings.engineState[0].bgSettings[2].charBase, 0, 0x8000);

    menuObj->unk_474B8 = Data_GetPackEntryData(menuObj->unk_462F0, 28) + 4;
    func_0200d1d8(&menuObj->unk_47438, 0, 2, 0, &menuObj->unk_474B8, 1, 1);

    void* var_r1_9 = Data_GetPackEntryData(menuObj->unk_462F4, 9);

    menuObj->unk_474B0 = Data_GetPackEntryData(menuObj->unk_462F4, 22) + 4;
    menuObj->unk_47334 = BgResMgr_AllocChar32(g_BgResourceManagers[0], var_r1_9,
                                              g_DisplaySettings.engineState[0].bgSettings[1].charBase, 0, 0x8000);
    func_0200d1d8(&menuObj->unk_47410, 0, 1, 0, &menuObj->unk_474B0, 1, 1);

    void* var_r4 = Data_GetPackEntryData(menuObj->unk_462F0, 7);

    menuObj->unk_474A8 = Data_GetPackEntryData(menuObj->unk_462F0, 8) + 4;
    SysFont_SetColor(&menuObj->font, 1U);
    func_ov002_02082dbc(&menuObj->font, (const Ov002_U16_5*)slots.data, var_r4 + 4, menuObj->unk_474A8);
    func_ov002_02082dbc(&menuObj->font, (const Ov002_U16_5*)arg2, var_r4 + 4, menuObj->unk_474A8);
    SysFont_SetColor(&menuObj->font, 3U);
    func_ov002_02082dbc(&menuObj->font, (const Ov002_U16_5*)header.data, var_r4 + 4, menuObj->unk_474A8);
    SysFont_SetColor(&menuObj->font, 1U);
    menuObj->unk_47330 = BgResMgr_AllocChar32(g_BgResourceManagers[0], var_r4,
                                              g_DisplaySettings.engineState[0].bgSettings[0].charBase, 0, 0x6000);
    func_0200d1d8(&menuObj->unk_473E8, 0, 0, 0, &menuObj->unk_474A8, 1, 1);
    Display_SetMainLayers(LAYER_BG0 | LAYER_BG1 | LAYER_BG2 | LAYER_BG3 | LAYER_OBJ);
    Display_Commit();
}

void func_ov002_02084c84(OtosuMenuObj* menuObj, u16* arg1) {
    void* temp_r0;
    void* temp_r0_2;
    void* temp_r0_3;
    void* temp_r0_4;
    void* temp_r0_5;
    void* temp_r0_6;
    void* temp_r0_7;
    void* temp_r0_8;
    void* temp_r1;
    void* temp_r1_10;
    void* temp_r1_2;
    void* temp_r1_3;
    void* temp_r1_4;
    void* temp_r1_5;
    void* temp_r1_6;
    void* temp_r1_7;
    void* temp_r1_8;
    void* temp_r1_9;
    void* temp_r2;
    void* var_r0;
    void* var_r0_2;
    void* var_r0_3;
    void* var_r1;
    void* var_r1_2;
    void* var_r1_3;
    void* var_r1_4;
    void* var_r1_5;
    void* var_r1_6;
    void* var_r4;

    func_ov002_02085710(menuObj);
    g_DisplaySettings.engineState[1].bgSettings[0].priority = 0;
    g_DisplaySettings.engineState[1].bgSettings[1].priority = 1;
    g_DisplaySettings.engineState[1].bgSettings[2].priority = 2;
    g_DisplaySettings.engineState[1].bgSettings[3].priority = 3;
    menuObj->unk_462F0                                      = DatMgr_LoadPackEntry(1, 0, 0, &data_ov002_02091aac, 1, 0);
    temp_r0                                                 = DatMgr_LoadPackEntry(1, 0, 0, &data_ov002_02091aac, 5, 0);
    menuObj->unk_462F4                                      = temp_r0;
    var_r1                                                  = Data_GetPackEntryData(temp_r0, 1);
    menuObj->unk_47340 = PaletteMgr_AllocPalette(g_PaletteManagers[1], var_r1, 0, 0, 0x10);
    PaletteMgr_Flush(g_PaletteManagers[1], menuObj->unk_47340);
    temp_r0_2          = menuObj->unk_462F4;
    var_r1_2           = Data_GetPackEntryData(temp_r0_2, 2);
    menuObj->unk_4732C = BgResMgr_AllocChar32(g_BgResourceManagers[1], var_r1_2,
                                              g_DisplaySettings.engineState[1].bgSettings[3].charBase, 0, 0x6000);
    temp_r0_3          = menuObj->unk_462F4;
    var_r0             = Data_GetPackEntryData(temp_r0_3, 4);
    menuObj->unk_474A0 = (void*)(var_r0 + 4);
    func_0200d1d8(&menuObj->unk_473C0, 1, 3, 0, &menuObj->unk_474A0, 1, 1);
    var_r1_3 = NULL;
    Display_SetSubLayers(LAYER_BG3 | LAYER_OBJ);
    temp_r0_4 = menuObj->unk_462F4;
    if (temp_r0_4 != NULL) {
        var_r1_3 = Data_GetPackEntryData(temp_r0_4, 6);
    }
    menuObj->unk_47344 = PaletteMgr_AllocPalette(g_PaletteManagers[0], var_r1_3, 0, 0, 0x10);
    PaletteMgr_Flush(g_PaletteManagers[0], menuObj->unk_47344);
    temp_r0_5          = menuObj->unk_462F4;
    var_r1_4           = Data_GetPackEntryData(temp_r0_5, 7);
    menuObj->unk_4733C = BgResMgr_AllocChar32(g_BgResourceManagers[0], var_r1_4,
                                              g_DisplaySettings.engineState[0].bgSettings[3].charBase, 0, 0x6000);
    temp_r0_6          = menuObj->unk_462F4;
    var_r0_2           = Data_GetPackEntryData(temp_r0_6, 8);
    menuObj->unk_474C0 = (void*)(var_r0_2 + 4);
    func_0200d1d8(&menuObj->unk_47460, 0, 3, 0, &menuObj->unk_474C0, 1, 1);
    temp_r0_7          = menuObj->unk_462F4;
    var_r1_5           = Data_GetPackEntryData(temp_r0_7, 3);
    menuObj->unk_47338 = BgResMgr_AllocChar32(g_BgResourceManagers[0], var_r1_5,
                                              g_DisplaySettings.engineState[0].bgSettings[2].charBase, 0, 0x8000);
    temp_r0_8          = menuObj->unk_462F4;
    var_r0_3           = Data_GetPackEntryData(temp_r0_8, 11);
    menuObj->unk_474B8 = (void*)(var_r0_3 + 4);
    func_0200d1d8(&menuObj->unk_47438, 0, 2, 0, &menuObj->unk_474B8, 1, 1);
    temp_r2 = menuObj->unk_462F0;
    var_r4  = Data_GetPackEntryData(temp_r2, 7);
    if (temp_r2 == NULL) {
        var_r1_6 = NULL;
    } else {
        var_r1_6 = Data_GetPackEntryData(temp_r2, 8);
    }
    menuObj->unk_474A8 = (void*)(var_r1_6 + 4);
    SysFont_SetColor(&menuObj->font, 1U);
    if (arg1 != NULL) {
        func_ov002_02082dbc(&menuObj->font, (const Ov002_U16_5*)arg1, var_r4 + 4, menuObj->unk_474A8);
    }
    menuObj->unk_47330 = BgResMgr_AllocChar32(g_BgResourceManagers[0], var_r4,
                                              g_DisplaySettings.engineState[0].bgSettings[0].charBase, 0, 0x6000);
    func_0200d1d8(&menuObj->unk_473E8, 0, 0, 0, &menuObj->unk_474A8, 1, 1);
    Display_SetMainLayers(LAYER_BG0 | LAYER_BG2 | LAYER_BG3 | LAYER_OBJ);
    Display_Commit();
}

void func_ov002_020850c0(OtosuMenuObj* menuObj, s32 arg1, s32 arg2, s32* arg3, u16* arg4) {
    func_ov002_02085710(menuObj);

    menuObj->unk_462F0 = DatMgr_LoadPackEntry(1, 0, 0, &data_ov002_02091aac, 1, 0);
    menuObj->unk_462F4 = DatMgr_LoadPackEntry(1, 0, 0, &data_ov002_02091aac, 5, 0);

    menuObj->unk_47340 =
        PaletteMgr_AllocPalette(g_PaletteManagers[1], Data_GetPackEntryData(menuObj->unk_462F4, 1), 0, 0, 0x10);
    PaletteMgr_Flush(g_PaletteManagers[1], menuObj->unk_47340);

    menuObj->unk_4732C = BgResMgr_AllocChar32(g_BgResourceManagers[1], Data_GetPackEntryData(menuObj->unk_462F4, 2),
                                              g_DisplaySettings.engineState[1].bgSettings[3].charBase, 0, 0x6000);

    menuObj->unk_474A0 = Data_GetPackEntryData(menuObj->unk_462F4, 4) + 4;
    func_0200d1d8(&menuObj->unk_473C0, 1, 3, 0, &menuObj->unk_474A0, 1, 1);

    menuObj->unk_47328 = BgResMgr_AllocChar32(g_BgResourceManagers[1], Data_GetPackEntryData(menuObj->unk_462F4, 3),
                                              g_DisplaySettings.engineState[1].bgSettings[2].charBase, 0, 0x8000);

    menuObj->unk_47498 = Data_GetPackEntryData(menuObj->unk_462F4, 5) + 4;
    func_0200d1d8(&menuObj->unk_47398, 1, 2, 0, &menuObj->unk_47498, 1, 1);

    menuObj->unk_47324 = BgResMgr_AllocChar32(g_BgResourceManagers[1], Data_GetPackEntryData(menuObj->unk_462F0, 5),
                                              g_DisplaySettings.engineState[1].bgSettings[1].charBase, 0, 0x6000);

    menuObj->unk_47490 = Data_GetPackEntryData(menuObj->unk_462F0, 6) + 4;
    SysFont_SetColor(&menuObj->font, 1);
    func_ov002_02082e70(&menuObj->font, arg3, menuObj->unk_47490 + 4, menuObj->unk_47490);
    func_0200d1d8(&menuObj->unk_47370, 1, 1, 0, &menuObj->unk_47490, 1, 1);

    Display_SetSubLayers(LAYER_BG1 | LAYER_BG2 | LAYER_BG3 | LAYER_OBJ);
    Display_SetMainLayers(LAYER_NONE);

    menuObj->unk_47344 =
        PaletteMgr_AllocPalette(g_PaletteManagers[0], Data_GetPackEntryData(menuObj->unk_462F4, 6), 0, 0, 0x10);
    PaletteMgr_Flush(g_PaletteManagers[0], menuObj->unk_47344);

    menuObj->unk_4733C = BgResMgr_AllocChar32(g_BgResourceManagers[0], Data_GetPackEntryData(menuObj->unk_462F4, 7),
                                              g_DisplaySettings.engineState[0].bgSettings[3].charBase, 0, 0x6000);

    menuObj->unk_474C0 = Data_GetPackEntryData(menuObj->unk_462F4, 8) + 4;
    func_0200d1d8(&menuObj->unk_47460, 0, 3, 0, &menuObj->unk_474C0, 1, 1);

    menuObj->unk_474B8 = Data_GetPackEntryData(menuObj->unk_462F4, arg2) + 4;
    func_0200d1d8(&menuObj->unk_47438, 0, 2, 0, &menuObj->unk_474B8, 1, 1);

    if (arg1 != 0xFFFF) {
        menuObj->unk_47334 = BgResMgr_AllocChar32(g_BgResourceManagers[0], Data_GetPackEntryData(menuObj->unk_462F4, 3),
                                                  g_DisplaySettings.engineState[0].bgSettings[1].charBase, 0, 0x8000);

        menuObj->unk_474B0 = Data_GetPackEntryData(menuObj->unk_462F4, arg1) + 4;
        func_0200d1d8(&menuObj->unk_47410, 0, 1, 0, &menuObj->unk_474B0, 1, 1);
        g_DisplaySettings.controls[0].layers |= 2;
    } else if ((menuObj->unk_474B0 == 0) && (menuObj->unk_47334 == NULL)) {
    } else {
        OS_WaitForever();
    }

    void* ptr          = Data_GetPackEntryData(menuObj->unk_462F0, 7);
    menuObj->unk_474A8 = Data_GetPackEntryData(menuObj->unk_462F0, 8);
    menuObj->unk_474A8 = menuObj->unk_474A8 + 4;
    SysFont_SetColor(&menuObj->font, 1);
    func_ov002_02082dbc(&menuObj->font, (const Ov002_U16_5*)arg4, ptr + 4, menuObj->unk_474A8);

    menuObj->unk_47330 =
        BgResMgr_AllocChar32(g_BgResourceManagers[0], ptr, g_DisplaySettings.engineState[0].bgSettings[0].charBase, 0, 0x6000);
    func_0200d1d8(&menuObj->unk_473E8, 0, 0, 0, &menuObj->unk_474A8, 1, 1);
    g_DisplaySettings.controls[0].layers |= 0x1D;
    Display_Commit();
}

void func_ov002_02085710(OtosuMenuObj* menuObj) {
    if (menuObj->unk_47344 != NULL) {
        PaletteMgr_ReleaseResource(g_PaletteManagers[0], menuObj->unk_47344);
        menuObj->unk_47344 = NULL;
    }
    if (menuObj->unk_47330 != NULL) {
        BgResMgr_ReleaseChar(g_BgResourceManagers[0], menuObj->unk_47330);
        menuObj->unk_47330 = NULL;
    }
    if (menuObj->unk_47334 != NULL) {
        BgResMgr_ReleaseChar(g_BgResourceManagers[0], menuObj->unk_47334);
        menuObj->unk_47334 = NULL;
    }
    if (menuObj->unk_47338 != NULL) {
        BgResMgr_ReleaseChar(g_BgResourceManagers[0], menuObj->unk_47338);
        menuObj->unk_47338 = NULL;
    }
    if (menuObj->unk_4733C != NULL) {
        BgResMgr_ReleaseChar(g_BgResourceManagers[0], menuObj->unk_4733C);
        menuObj->unk_4733C = NULL;
    }

    if (menuObj->unk_47340 != NULL) {
        PaletteMgr_ReleaseResource(g_PaletteManagers[1], menuObj->unk_47340);
        menuObj->unk_47340 = NULL;
    }
    if (menuObj->unk_47320 != NULL) {
        BgResMgr_ReleaseChar(g_BgResourceManagers[1], menuObj->unk_47320);
        menuObj->unk_47320 = NULL;
    }
    if (menuObj->unk_47324 != NULL) {
        BgResMgr_ReleaseChar(g_BgResourceManagers[1], menuObj->unk_47324);
        menuObj->unk_47324 = NULL;
    }
    if (menuObj->unk_47328 != NULL) {
        BgResMgr_ReleaseChar(g_BgResourceManagers[1], menuObj->unk_47328);
        menuObj->unk_47328 = NULL;
    }
    if (menuObj->unk_4732C != NULL) {
        BgResMgr_ReleaseChar(g_BgResourceManagers[1], menuObj->unk_4732C);
        menuObj->unk_4732C = NULL;
    }

    func_0200d954(1, 0);
    func_0200d954(1, 1);
    func_0200d954(1, 2);
    func_0200d954(1, 3);
    func_0200d954(0, 2);
    func_0200d954(0, 3);

    if (menuObj->unk_462F0 != NULL) {
        DatMgr_ReleaseData(menuObj->unk_462F0);
        menuObj->unk_462F0 = NULL;
    }
    if (menuObj->unk_462F4 != NULL) {
        DatMgr_ReleaseData(menuObj->unk_462F4);
        menuObj->unk_462F4 = NULL;
    }
    if (menuObj->unk_462F8 != NULL) {
        DatMgr_ReleaseData(menuObj->unk_462F8);
        menuObj->unk_462F8 = NULL;
    }
    if (menuObj->unk_462FC != NULL) {
        DatMgr_ReleaseData(menuObj->unk_462FC);
        menuObj->unk_462FC = NULL;
    }
}

u16 func_ov002_0208597c(u16* arg0) {
    s32 x;
    s32 y;
    u16 index  = 0;
    u16 result = 0xFFFF;

    TouchCoord coords;

    TouchInput_GetCoord(&coords);

    if (TouchInput_WasTouchPressed() != 0) {
        result = 0xFFFE;
        if (arg0[0] != 0xFFFF) {
            do {
                u16* entry = arg0 + index * 5;

                if ((coords.x >= (s32)entry[1]) && (coords.x <= (s32)entry[3]) && (coords.y >= (s32)entry[2]) &&
                    (coords.y <= (s32)entry[4]))
                {
                    result = entry[0];
                    break;
                }
                index++;
            } while (arg0[index * 5] != 0xFFFF);
        }
    }
    return result;
}

void func_ov002_02085a44(OtosuMenuObj* menuObj) {
    func_ov002_02082548(menuObj);
    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->unk_46100);
    PrcCtx_Init(&menuObj->unk_46100, "OtosuMenuLinklevel", sizeof(OtosuMenuObj));
    PrcCtx_SetWorkObject(&menuObj->unk_46100, menuObj);
    PrcCtx_ReplaceFrame(&menuObj->unk_46100, &data_ov002_02093008, NULL);
    PrcMaster_RegisterContext(&menuObj->prcMaster, &menuObj->unk_46100);
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

    if (PrcCtx_GetStepTable(&menuObj->unk_4161C) != data_ov002_02092efc &&
        PrcCtx_GetStepTable(&menuObj->unk_4161C) != data_ov002_02092e68 &&
        PrcCtx_GetStepTable(&menuObj->unk_4161C) != data_ov002_02092ec8 &&
        PrcCtx_GetStepTable(&menuObj->unk_4161C) != data_ov002_02092e98)
    {
        switch (result) {
            case 10:
                PrcCtx_ReplaceFrame(&menuObj->unk_4161C, &data_ov002_02092e40, NULL);
                break;
            case 8:
            case 9:
                switch (menuObj->unk_41FE9) {
                    case 0:
                        PrcCtx_ReplaceFrame(&menuObj->unk_4161C, &data_ov002_02092e54, NULL);
                        break;
                    case 1:
                        PrcCtx_ReplaceFrame(&menuObj->unk_4161C, &data_ov002_02092e18, NULL);
                        break;
                    case 2:
                        PrcCtx_ReplaceFrame(&menuObj->unk_4161C, &data_ov002_02092e2c, NULL);
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
    Mem_SetSequence(&gMainHeap, obj, data_ov002_02092be4);
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
    PrcCtx_Init(&menuObj->unk_4161C, data_ov002_02092be4, sizeof(OtosuMenuObj));
    PrcCtx_SetWorkObject(&menuObj->unk_4161C, menuObj);
    PrcCtx_ReplaceFrame(&menuObj->unk_4161C, &data_ov002_02093268, NULL);
    menuObj->unk_41FE9 = 0;
    MainOvlDisp_NextProcessStage();
}

void OtosuMenu_InitForMultiplayerEnter(OtosuMenuObj* menuObj) {
    func_ov002_0208b860();

    menuObj = func_ov002_02085df8();

    menuObj->unk_460BC = 1;
    PrcCtx_Init(&menuObj->unk_4161C, data_ov002_02092be4, sizeof(OtosuMenuObj));
    PrcCtx_SetWorkObject(&menuObj->unk_4161C, menuObj);
    PrcCtx_ReplaceFrame(&menuObj->unk_4161C, &data_ov002_02093254, NULL);
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
    PrcCtx_Init(&menuObj->unk_4161C, data_ov002_02092be4, sizeof(OtosuMenuObj));
    PrcCtx_SetWorkObject(&menuObj->unk_4161C, menuObj);
    menuObj->unk_462EC = 0;
    if (data_02074d10.unk_40A == 0) {
        PrcCtx_ReplaceFrame(&menuObj->unk_4161C, &data_ov002_02093034, NULL);
    } else {
        PrcCtx_ReplaceFrame(&menuObj->unk_4161C, &data_ov002_0209305c, NULL);
    }
    func_ov002_02085a44(menuObj);
    CriSndMgr_PlayFile(ADX_B11);
    MainOvlDisp_NextProcessStage();
}

void OtosuMenu_InitForSinglePlayerRankings(OtosuMenuObj* menuObj) {
    func_ov002_0208bd40();
    menuObj = func_ov002_02085df8();
    PrcCtx_Init(&menuObj->unk_4161C, data_ov002_02092be4, sizeof(OtosuMenuObj));
    PrcCtx_SetWorkObject(&menuObj->unk_4161C, menuObj);
    menuObj->unk_462EC = 1;
    PrcCtx_ReplaceFrame(&menuObj->unk_4161C, &data_ov002_02093048, NULL);
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
    PrcCtx_Init(&menuObj->unk_4161C, data_ov002_02092be4, sizeof(OtosuMenuObj));
    PrcCtx_SetWorkObject(&menuObj->unk_4161C, menuObj);
    PrcCtx_ReplaceFrame(&menuObj->unk_4161C, &data_ov002_02092e18, NULL);
    func_ov002_02085a44(menuObj);
    MainOvlDisp_NextProcessStage();
}

void OtosuMenu_InitForRoleSelection(OtosuMenuObj* menuObj) {
    func_ov002_0208bd40();

    menuObj = func_ov002_02085df8();

    menuObj->unk_460BC = 1;
    PrcCtx_Init(&menuObj->unk_4161C, data_ov002_02092be4, sizeof(OtosuMenuObj));
    PrcCtx_SetWorkObject(&menuObj->unk_4161C, menuObj);
    PrcCtx_ReplaceFrame(&menuObj->unk_4161C, &data_ov002_02092c9c, NULL);
    func_ov002_02085a44(menuObj);
    MainOvlDisp_NextProcessStage();
}

void OtosuMenu_InitForFontList(OtosuMenuObj* menuObj) {
    func_ov002_0208bd40();

    menuObj = func_ov002_02085df8();

    PrcCtx_Init(&menuObj->unk_4161C, data_ov002_02092be4, sizeof(OtosuMenuObj));
    PrcCtx_SetWorkObject(&menuObj->unk_4161C, menuObj);
    PrcCtx_ReplaceFrame(&menuObj->unk_4161C, &data_ov002_020932b8, NULL);
    func_ov002_02085a44(menuObj);
    MainOvlDisp_NextProcessStage();
}

void OtosuMenu_InitForDataDeletion(OtosuMenuObj* menuObj) {
    func_ov002_0208c228();

    menuObj = func_ov002_02085df8();

    func_ov002_02085a44(menuObj);
    PrcCtx_Init(&menuObj->unk_4161C, data_ov002_02092be4, sizeof(OtosuMenuObj));
    PrcCtx_SetWorkObject(&menuObj->unk_4161C, menuObj);
    PrcCtx_ReplaceFrame(&menuObj->unk_4161C, &data_ov002_0209344c, NULL);
    MainOvlDisp_NextProcessStage();
}

void OtosuMenu_InitForDataCorrupted(OtosuMenuObj* menuObj) {
    func_ov002_0208c228();

    menuObj = func_ov002_02085df8();

    func_ov002_02085a44(menuObj);
    PrcCtx_Init(&menuObj->unk_4161C, data_ov002_02092be4, sizeof(OtosuMenuObj));
    PrcCtx_SetWorkObject(&menuObj->unk_4161C, menuObj);
    PrcCtx_ReplaceFrame(&menuObj->unk_4161C, &data_ov002_02093460, NULL);
    MainOvlDisp_NextProcessStage();
}

void func_ov002_02086290(OtosuMenuObj* menuObj) {
    func_ov002_0208c228();

    menuObj = func_ov002_02085df8();

    func_ov002_02085a44(menuObj);
    PrcCtx_Init(&menuObj->unk_4161C, data_ov002_02092be4, sizeof(OtosuMenuObj));
    PrcCtx_SetWorkObject(&menuObj->unk_4161C, menuObj);
    PrcCtx_ReplaceFrame(&menuObj->unk_4161C, &data_ov002_02093438, NULL);
    MainOvlDisp_NextProcessStage();
}

void OtosuMenu_InitForDataLoadFailure(OtosuMenuObj* menuObj) {
    func_ov002_0208c228();

    menuObj = func_ov002_02085df8();

    func_ov002_02085a44(menuObj);
    PrcCtx_Init(&menuObj->unk_4161C, data_ov002_02092be4, sizeof(OtosuMenuObj));
    PrcCtx_SetWorkObject(&menuObj->unk_4161C, menuObj);
    PrcCtx_ReplaceFrame(&menuObj->unk_4161C, &data_ov002_02093310, NULL);
    MainOvlDisp_NextProcessStage();
}

void OtosuMenu_InitForDataSaveFailure(OtosuMenuObj* menuObj) {
    func_ov002_0208c228();

    menuObj = func_ov002_02085df8();

    func_ov002_02085a44(menuObj);
    PrcCtx_Init(&menuObj->unk_4161C, data_ov002_02092be4, sizeof(OtosuMenuObj));
    PrcCtx_SetWorkObject(&menuObj->unk_4161C, menuObj);
    PrcCtx_ReplaceFrame(&menuObj->unk_4161C, &data_ov002_02093324, NULL);
    MainOvlDisp_NextProcessStage();
}

void OtosuMenu_Update(OtosuMenuObj* menuObj) {
    if (SystemStatusFlags.reset && menuObj->unk_460BC) {
        PrcCtx_ReplaceCurrentUpdateCallback(&menuObj->unk_4161C, func_ov002_02082a44);
    }

    TouchInput_Update();
    OamMgr_Reset(&g_OamMgr[DISPLAY_MAIN], 0, 0);
    OamMgr_Reset(&g_OamMgr[DISPLAY_SUB], 0, 0);
    OamMgr_ResetCommandQueues(&g_OamMgr[DISPLAY_MAIN]);
    OamMgr_ResetCommandQueues(&g_OamMgr[DISPLAY_SUB]);

    if (SystemStatusFlags.reset || (menuObj->unk_460BC == 0)) {
        PrcMaster_RunAllCtxSteps(&menuObj->prcMaster);
        EasyTask_ProcessPendingTasks(&menuObj->taskPool);
        if (PrcCtx_RunSteps(&menuObj->unk_4161C) == 0) {
            OverlayTag tag;
            MainOvlDisp_Pop(&tag);
            return;
        }
        if (menuObj->unk_460B8 != 0) {
            Sprite_UpdateAndCheck(&menuObj->unk_46078);
        }
    }

    PrcMaster_UpdateAllContexts(&menuObj->prcMaster);
    PrcCtx_Update(&menuObj->unk_4161C);

    switch (menuObj->unk_46074) {

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
            PrcCtx_Render(&menuObj->unk_4161C);
            if (menuObj->unk_460B8 != 0) {
                Sprite_Render(&menuObj->unk_46078);
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
    if (menuObj->unk_460B8 != 0) {
        func_ov002_0208264c(menuObj);
    }
    EasyTask_DestroyPool(&menuObj->taskPool);
    PrcCtx_Destroy(&menuObj->unk_4161C);
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
