#include "OtosuMenuShared.h"

static PrcStepFn data_ov002_020932f0[] = {func_ov002_02091510, OtosuPrcStep_FadeStart_Neutral2, OtosuPrcStep_FadeWait_Neutral2,
                                          func_ov002_02091710};

static PrcStepFn data_ov002_02093300[] = {func_ov002_02091608, OtosuPrcStep_FadeStart_Neutral2, OtosuPrcStep_FadeWait_Neutral2,
                                          func_ov002_02091700};

static const Ov002_U16_10 data_ov002_020928e0 = {
    0x2A35, 0x0010, 0x0050, 0x00F0, 0x0098, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000,
}; /* const */

static const Ov002_U16_10 data_ov002_02092980 = {
    0x3384, 0x0010, 0x0050, 0x00F0, 0x0098, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000,
}; /* const */

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

PrcFrameDesc data_ov002_02093324 = {
    .enter     = func_ov002_020902a4,
    .stepTable = data_ov002_020932f0,
    .update    = func_ov002_0209034c,
    .render    = func_ov002_02090350,
    .exit      = func_ov002_02090310,
};

PrcFrameDesc data_ov002_02093460 = {
    .enter     = func_ov002_020902a4,
    .stepTable = data_ov002_02093338,
    .update    = func_ov002_0209034c,
    .render    = func_ov002_02090350,
    .exit      = func_ov002_02090310,
};
static const u16 data_ov002_020929c6[0xF] = {
    0x2A3D, 0x38, 0x51, 0xC8, 0x5B, 0x2A3E, 0x38, 0x65, 0xC8, 0x6F, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0,
}; /* const */
PrcFrameDesc data_ov002_02093310 = {
    .enter     = func_ov002_020902a4,
    .stepTable = data_ov002_02093300,
    .update    = func_ov002_0209034c,
    .render    = func_ov002_02090350,
    .exit      = func_ov002_02090310,
};
static const Ov002_U16_10 data_ov002_02092958 = {
    0x2A40, 0x0010, 0x0050, 0x00F0, 0x0098, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000,
}; /* const */
static const Ov002_U16_10 data_ov002_02092908 = {
    0x2A38, 0x0010, 0x0050, 0x00F0, 0x0098, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000,
}; /* const */
static const Ov002_U16_10 data_ov002_0209291c = {
    0x2A42, 0x0010, 0x0050, 0x00F0, 0x0098, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000,
}; /* const */
static const Ov002_U16_10 data_ov002_02092930 = {
    0x3383, 0x0010, 0x0050, 0x00F0, 0x0098, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000,
}; /* const */
static const Ov002_U16_10 data_ov002_02092944 = {
    0x2A3F, 0x0010, 0x0050, 0x00F0, 0x0098, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000,
}; /* const */
static const Ov002_U16_10 data_ov002_0209296c = {
    0x2A37, 0x0010, 0x0050, 0x00F0, 0x0098, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000,
}; /* const */
PrcFrameDesc data_ov002_0209344c = {
    .enter     = func_ov002_020902a4,
    .stepTable = data_ov002_020933a8,
    .update    = func_ov002_0209034c,
    .render    = func_ov002_02090350,
    .exit      = func_ov002_02090310,
};
static const Ov002_U16_10 data_ov002_02092994 = {
    0x2A34, 0x0010, 0x0050, 0x00F0, 0x0098, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000,
}; /* const */
PrcFrameDesc data_ov002_02093438 = {
    .enter     = func_ov002_020902a4,
    .stepTable = data_ov002_02093370,
    .update    = func_ov002_0209034c,
    .render    = func_ov002_02090350,
    .exit      = func_ov002_02090310,
};
static const Ov002_U16_10 data_ov002_020928cc = {
    0x2A36, 0x0010, 0x0050, 0x00F0, 0x0098, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000,
}; /* const */
static const u16 data_ov002_020929a8[0xF] = {
    0x2A3B, 0x38, 0x51, 0xC8, 0x5B, 0x2A3C, 0x38, 0x65, 0xC8, 0x6F, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0,
}; /* const */
static const Ov002_U16_10 data_ov002_020928f4 = {
    0x2A41, 0x0010, 0x0050, 0x00F0, 0x0098, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000,
}; /* const */
static const u16 data_ov002_020929e4[0xF] = {
    0, 0x38, 0x4C, 0xC8, 0x60, 1, 0x38, 0x61, 0xC8, 0x74, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
}; /* const */

SpriteFrameInfo* func_ov002_020901a0(Sprite* sprite, s32 arg, s32 mode) {
    Sprite_FrameInfoCallback(sprite, mode);
}

static const u16 data_ov002_02092a02[0xF] = {
    0x2A39, 0x38, 0x51, 0xC8, 0x5B, 0x2A3A, 0x38, 0x65, 0xC8, 0x6F, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0,
}; /* const */

void func_ov002_0209023c(OtosuMenuObj* menuObj) {
    SpriteAnimation anim = data_ov002_02092a20;

    _Sprite_Load(&menuObj->unk_46078, &anim);
    Sprite_ChangeAnimation(&menuObj->unk_46078, menuObj->unk_46078.animData, 3, menuObj->unk_46078.cellTable);
}

void func_ov002_020902a4(s32 arg0, OtosuMenuObj* menuObj) {
    PrcCtx_Init(&menuObj->unk_474E8, "DelData_WinObj", 0x100);
    PrcCtx_ReplaceFrame(&menuObj->unk_474E8, &data_ov002_02093498, NULL);
    PrcMaster_RegisterContext(&menuObj->prcMaster, &menuObj->unk_474E8);
    Sprite_Destroy(&menuObj->unk_46078);
    func_ov002_0209023c(menuObj);
}

void func_ov002_02090310(s32 arg0, OtosuMenuObj* menuObj) {
    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->unk_474E8);
    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->unk_476D0);
}

void func_ov002_0209034c(void) {
    return;
}

void func_ov002_02090350(void) {
    return;
}

void func_ov002_02090354(OtosuMenuObj* menuObj) {
    u16   var_r1;
    u16   var_r2;
    u16   var_r3;
    void* temp_r0;
    void* temp_r1;
    void* temp_r7;
    void* var_r4;

    if (menuObj->unk_47330 != NULL) {
        BgResMgr_ReleaseChar(g_BgResourceManagers[0], menuObj->unk_47330);
        menuObj->unk_47330 = NULL;
    }
    if (menuObj->unk_462F0 != NULL) {
        DatMgr_ReleaseData(menuObj->unk_462F0);
        menuObj->unk_462F0 = NULL;
    }
    temp_r0            = DatMgr_LoadPackEntry(1, 0, 0, &data_ov002_02091aac, 1, 0);
    menuObj->unk_462F0 = temp_r0;
    var_r4             = Data_GetPackEntryData(temp_r0, 7);
    var_r3             = 0;
    var_r2             = 0;
    do {
        var_r1 = 0;
    loop_9:
        temp_r7 = (u8*)menuObj + (var_r2 << 7) + (var_r1 * 2);
        var_r1 += 1;
        *(s16*)(temp_r7 + 0x46320) = (s16)(var_r3 | 0x1000);
        var_r3 += 1;
        if ((u32)var_r1 < 0x40U) {
            goto loop_9;
        }
        var_r2 += 1;
    } while ((u32)var_r2 < 0x20U);
    DC_PurgeAll();
    menuObj->unk_474A8 = &menuObj->unk_46320;
    menuObj->unk_47330 = BgResMgr_AllocChar32(g_BgResourceManagers[0], var_r4,
                                              g_DisplaySettings.engineState[0].bgSettings[0].charBase, 0, 0x6000);
    func_0200d1d8(&menuObj->unk_473E8, 0, 0, 0, &menuObj->unk_474A8, 1, 1);
    Display_Commit();
}

void func_ov002_020904cc(OtosuMenuObj* menuObj, u16* arg1, u16* arg2) {
    u16   var_ip;
    u16   var_r2;
    u16   var_r3;
    void* temp_r0_2;
    void* temp_r1;
    void* temp_r1_2;
    void* temp_r1_3;
    void* temp_r1_4;
    void* temp_r1_5;
    void* temp_r1_6;
    void* temp_r1_7;
    void* temp_r2_2;
    void* temp_r2_5;
    void* temp_r9;
    void* var_r0;
    void* var_r0_2;
    void* var_r1_2;
    void* var_r1_3;
    void* var_r1_4;
    void* var_r1_5;
    void* var_r4;

    func_ov002_02085710(menuObj);
    menuObj->unk_462F0 = DatMgr_LoadPackEntry(1, 0, 0, &data_ov002_02091aac, 1, 0);
    menuObj->unk_462F4 = DatMgr_LoadPackEntry(1, 0, 0, &data_ov002_02091c1c, 1, 0);
    menuObj->unk_462F8 = DatMgr_LoadRawData(1, 0, 0, &data_ov002_02091c2c);
    menuObj->unk_462FC = DatMgr_LoadRawData(1, 0, 0, &data_ov002_02091c24);

    Display_SetMainLayers(LAYER_BG0 | LAYER_BG3 | LAYER_OBJ);
    Display_SetSubLayers(LAYER_BG2 | LAYER_BG3 | LAYER_OBJ);

    menuObj->unk_47340 =
        PaletteMgr_AllocPalette(g_PaletteManagers[1], Data_GetPackEntryData(menuObj->unk_462F4, 1), 0, 0, 0x10);
    PaletteMgr_Flush(g_PaletteManagers[1], menuObj->unk_47340);

    var_r1_2 = Data_GetPackEntryData(menuObj->unk_462F8, 1);
    var_r0   = Data_GetPackEntryData(menuObj->unk_462F8, 2);

    menuObj->unk_474A0 = (void*)(var_r0 + 4);
    menuObj->unk_4732C = BgResMgr_AllocChar32(g_BgResourceManagers[1], var_r1_2,
                                              g_DisplaySettings.engineState[1].bgSettings[3].charBase, 0, 0x6000);
    func_0200d1d8(&menuObj->unk_473C0, 1, 3, 0, &menuObj->unk_474A0, 1, 1);

    var_r4   = Data_GetPackEntryData(menuObj->unk_462F4, 2);
    var_r1_3 = Data_GetPackEntryData(menuObj->unk_462F4, 3);

    menuObj->unk_47498 = (void*)(var_r1_3 + 4);
    func_ov002_02082dbc(&menuObj->font, (const Ov002_U16_5*)arg1, var_r4 + 4, menuObj->unk_47498);
    menuObj->unk_47328 = BgResMgr_AllocChar32(g_BgResourceManagers[1], var_r4,
                                              g_DisplaySettings.engineState[1].bgSettings[2].charBase, 0, 0x4380);
    func_0200d1d8(&menuObj->unk_47398, 1, 2, 0, &menuObj->unk_47498, 1, 1);
    temp_r0_2          = menuObj->unk_462F4;
    var_r1_4           = Data_GetPackEntryData(temp_r0_2, 1);
    menuObj->unk_47344 = PaletteMgr_AllocPalette(g_PaletteManagers[0], var_r1_4, 0, 0, 0x10);
    PaletteMgr_Flush(g_PaletteManagers[0], menuObj->unk_47344);

    var_r1_5 = Data_GetPackEntryData(menuObj->unk_462FC, 1);
    var_r0_2 = Data_GetPackEntryData(menuObj->unk_462FC, 2);

    menuObj->unk_474C0 = (void*)(var_r0_2 + 4);
    menuObj->unk_4733C = BgResMgr_AllocChar32(g_BgResourceManagers[0], var_r1_5,
                                              g_DisplaySettings.engineState[0].bgSettings[3].charBase, 0, 0x6000);
    func_0200d1d8(&menuObj->unk_47460, 0, 3, 0, &menuObj->unk_474C0, 1, 1);

    void* var_r4_2 = Data_GetPackEntryData(menuObj->unk_462F0, 7);

    var_ip = 0;
    var_r3 = 0;
    do {
        var_r2 = 0;
    loop_28:
        temp_r9 = (u8*)menuObj + (var_r3 << 7) + (var_r2 * 2);
        var_r2 += 1;
        *(s16*)(temp_r9 + 0x46320) = (s16)(var_ip | 0x1000);
        var_ip += 1;
        if ((u32)var_r2 < 0x40U) {
            goto loop_28;
        }
        var_r3 += 1;
    } while ((u32)var_r3 < 0x20U);

    DC_PurgeAll();
    menuObj->unk_474A8 = &menuObj->unk_46320;
    menuObj->unk_47330 = BgResMgr_AllocChar32(g_BgResourceManagers[0], var_r4_2,
                                              g_DisplaySettings.engineState[0].bgSettings[0].charBase, 0, 0x6000);
    func_ov002_02082dbc(&menuObj->font, (const Ov002_U16_5*)arg2, var_r4_2 + 4, menuObj->unk_474A8);
    func_0200d1d8(&menuObj->unk_473E8, 0, 0, 0, &menuObj->unk_474A8, 1, 1);
    Display_Commit();
}

PrcStepResult func_ov002_0209095c(PrcCtx* ctx, void* arg1) {
    OtosuMenuObj*      menuObj      = (OtosuMenuObj*)arg1;
    Ov002_U16_15       table_sub    = *(const Ov002_U16_15*)data_ov002_020929e4;
    u16                table_pos[6] = {0x0003, 0x0080, 0x0056, 0x0003, 0x0080, 0x006A};
    s32                temp_ip;
    s32                var_r2;
    Ov002_U16_2*       var_r3;
    const Ov002_U16_2* var_r4;
    u16                temp_r0;
    u16                temp_r0_2;
    u16                temp_r1;

    temp_r0_2 = func_ov002_0208597c(table_sub.data);
    if (temp_r0_2 == 0xFFFF) {
        return 0;
    }
    if (temp_r0_2 == 0xFFFE) {
        menuObj->unk_474C8      = 0xFFFFU;
        menuObj->unk_46078.posX = 0U;
        menuObj->unk_46078.posY = 0xD2U;
        return 0;
    }
    if (menuObj->unk_474C8 == temp_r0_2) {
        SndMgr_StartPlayingSE(SEIDX_MENU_MSYSTEM_EXECUTE);
        switch (temp_r0_2) { /* irregular */
            case 0:
                PrcCtx_AdvanceStep(ctx);
                return PRC_STEP_CONTINUE;
            case 1:
                PrcCtx_ReplaceStepTable(ctx, data_ov002_020932e8);
                func_ov002_02090354(menuObj);
                menuObj->unk_46078.posX = 0U;
                menuObj->unk_46078.posY = 0xD2U;
                PrcCtx_PushStepTable(ctx, PrcSteps_FadeDark);
                return PRC_STEP_CONTINUE;
        }
    } else {
        menuObj->unk_474C8 = temp_r0_2;
        SndMgr_StartPlayingSE(SEIDX_MENU_MSYSTEM_CURSOR);
        if ((temp_r0_2 != 0) && (temp_r0_2 != 1)) {
            return 2;
        }
        temp_ip                 = temp_r0_2 * 6;
        menuObj->unk_46078.posX = table_pos[temp_ip + 1];
        menuObj->unk_46078.posY = table_pos[temp_ip + 2];
        Sprite_SetAnimation(&menuObj->unk_46078, menuObj->unk_46078.animData, table_pos[temp_ip],
                            menuObj->unk_46078.cellTable);
        return PRC_STEP_CONTINUE;
    }
}

PrcStepResult func_ov002_02090b30(PrcCtx* ctx, void*) {
    PrcCtx_AdvanceStep(ctx);
    Savefile_ResetIOPipeline();
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02090b44(PrcCtx* ctx, void* arg1) {
    OtosuMenuObj* menuObj    = (OtosuMenuObj*)arg1;
    Ov002_U16_10  table_sp1E = data_ov002_02092994;
    Ov002_U16_15  table_sub  = *(const Ov002_U16_15*)data_ov002_02092a02;
    u16           i;

    Display_SetMainLayers(LAYER_BG0 | LAYER_OBJ);
    Display_SetSubLayers(LAYER_BG2 | LAYER_OBJ);
    Display_Commit();
    func_ov002_020904cc(menuObj, table_sp1E.data, table_sub.data);
    menuObj->unk_46078.posX = 0;
    menuObj->unk_46078.posY = 0xD2;
    menuObj->unk_474C8      = 0xFFFF;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02090c28(PrcCtx* ctx, void* arg1) {
    OtosuMenuObj* menuObj    = (OtosuMenuObj*)arg1;
    Ov002_U16_10  table_sp1E = data_ov002_020928e0;
    Ov002_U16_15  table_sub  = *(const Ov002_U16_15*)data_ov002_020929a8;
    u16           i;

    Display_SetMainLayers(LAYER_BG0 | LAYER_OBJ);
    Display_SetSubLayers(LAYER_BG2 | LAYER_OBJ);
    Display_Commit();
    func_ov002_020904cc(menuObj, table_sp1E.data, table_sub.data);
    menuObj->unk_474C8      = 0xFFFF;
    menuObj->unk_46078.posX = 0;
    menuObj->unk_46078.posY = 0xD2;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02090d0c(PrcCtx* ctx, void* arg1) {
    OtosuMenuObj* menuObj    = (OtosuMenuObj*)arg1;
    Ov002_U16_10  table_sp1E = data_ov002_020928cc;
    Ov002_U16_15  table_sub  = *(const Ov002_U16_15*)data_ov002_020929c6;
    u16           i;

    Display_SetMainLayers(LAYER_BG0 | LAYER_OBJ);
    Display_SetSubLayers(LAYER_BG2 | LAYER_OBJ);
    Display_Commit();
    func_ov002_020904cc(menuObj, table_sp1E.data, table_sub.data);
    menuObj->unk_474C8      = 0xFFFF;
    menuObj->unk_46078.posX = 0;
    menuObj->unk_46078.posY = 0xD2;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02090df0(PrcCtx* ctx, void* arg1) {
    OtosuMenuObj* menuObj      = (OtosuMenuObj*)arg1;
    Ov002_U16_10  table_spA    = data_ov002_0209296c;
    u16           table_sub[5] = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0};
    u16           i;

    Display_SetMainLayers(LAYER_BG0 | LAYER_OBJ);
    Display_SetSubLayers(LAYER_BG2 | LAYER_OBJ);
    Display_Commit();
    func_ov002_020904cc(menuObj, table_spA.data, table_sub);
    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->unk_474E8);
    menuObj->unk_46078.posX = 0;
    menuObj->unk_46078.posY = 0xD2;
    menuObj->unk_474C8      = 0xFFFF;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02090ee8(PrcCtx* ctx, void* arg1) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)arg1;

    PrcCtx_Init(&menuObj->unk_476D0, "OtosuMenu_Icon2", 0x40);
    PrcCtx_ReplaceFrame(&menuObj->unk_476D0, &data_ov002_020934b0, NULL);
    PrcMaster_RegisterContext(&menuObj->prcMaster, &menuObj->unk_476D0);
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
    OtosuMenuObj* menuObj      = (OtosuMenuObj*)arg1;
    Ov002_U16_10  table_spA    = data_ov002_02092908;
    u16           table_sub[5] = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0};
    u16           i;

    Display_SetMainLayers(LAYER_BG0 | LAYER_OBJ);
    Display_SetSubLayers(LAYER_BG2 | LAYER_OBJ);
    Display_Commit();
    func_ov002_020904cc(menuObj, table_spA.data, table_sub);
    menuObj->unk_46078.posX = 0;
    menuObj->unk_46078.posY = 0xD2;
    menuObj->unk_474C8      = 0xF0;
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
    OtosuMenuObj* menuObj      = (OtosuMenuObj*)arg1;
    Ov002_U16_10  table_spA    = data_ov002_02092944;
    u16           table_sub[5] = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0};
    u16           i;

    Display_SetMainLayers(LAYER_BG0 | LAYER_OBJ);
    Display_SetSubLayers(LAYER_BG2 | LAYER_OBJ);
    Display_Commit();
    func_ov002_020904cc(menuObj, table_spA.data, table_sub);
    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->unk_474E8);
    menuObj->unk_46078.posX = 0;
    menuObj->unk_46078.posY = 0xD2;
    menuObj->unk_474C8      = 0xFFFF;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02091208(PrcCtx* ctx, void* arg1) {
    OtosuMenuObj* menuObj      = (OtosuMenuObj*)arg1;
    Ov002_U16_10  table_spA    = data_ov002_02092930;
    u16           table_sub[5] = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0};
    u16           i;

    Display_SetMainLayers(LAYER_BG0 | LAYER_OBJ);
    Display_SetSubLayers(LAYER_BG2 | LAYER_OBJ);
    Display_Commit();
    func_ov002_020904cc(menuObj, table_spA.data, table_sub);
    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->unk_474E8);
    menuObj->unk_46078.posX = 0;
    menuObj->unk_46078.posY = 0xD2;
    menuObj->unk_474C8      = 0xFFFF;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02091300(PrcCtx* ctx, void* arg1) {
    OtosuMenuObj* menuObj      = (OtosuMenuObj*)arg1;
    Ov002_U16_10  table_spA    = data_ov002_02092958;
    u16           table_sub[5] = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0};
    u16           i;

    Display_SetMainLayers(LAYER_BG0 | LAYER_OBJ);
    Display_SetSubLayers(LAYER_BG2 | LAYER_OBJ);
    Display_Commit();
    func_ov002_020904cc(menuObj, table_spA.data, table_sub);
    menuObj->unk_46078.posX = 0;
    menuObj->unk_46078.posY = 0xD2;
    menuObj->unk_474C8      = 0xF0;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_020913d8(PrcCtx* ctx, void* arg1) {
    OtosuMenuObj* menuObj      = (OtosuMenuObj*)arg1;
    Ov002_U16_10  table_spA    = data_ov002_02092980;
    u16           table_sub[5] = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0};
    u16           i;

    Display_SetMainLayers(LAYER_BG0 | LAYER_OBJ);
    Display_SetSubLayers(LAYER_BG2 | LAYER_OBJ);
    Display_Commit();
    func_ov002_020904cc(menuObj, table_spA.data, table_sub);
    menuObj->unk_46078.posX = 0;
    menuObj->unk_46078.posY = 0xD2;
    menuObj->unk_474C8      = 0xF0;
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
    OtosuMenuObj* menuObj      = (OtosuMenuObj*)arg1;
    Ov002_U16_10  table_spA    = data_ov002_0209291c;
    u16           table_sub[5] = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0};
    u16           i;

    Display_SetMainLayers(LAYER_BG0 | LAYER_OBJ);
    Display_SetSubLayers(LAYER_BG2 | LAYER_OBJ);
    Display_Commit();
    func_ov002_020904cc(menuObj, table_spA.data, table_sub);
    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->unk_474E8);
    menuObj->unk_46078.posX = 0;
    menuObj->unk_46078.posY = 210;
    menuObj->unk_474C8      = 0xFFFF;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02091608(PrcCtx* ctx, void* arg1) {
    OtosuMenuObj* menuObj      = (OtosuMenuObj*)arg1;
    Ov002_U16_10  table_spA    = data_ov002_020928f4;
    u16           table_sub[5] = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0};
    u16           i;

    Display_SetMainLayers(LAYER_BG0 | LAYER_OBJ);
    Display_SetSubLayers(LAYER_BG2 | LAYER_OBJ);
    Display_Commit();
    func_ov002_020904cc(menuObj, table_spA.data, table_sub);
    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->unk_474E8);
    menuObj->unk_46078.posX = 0;
    menuObj->unk_46078.posY = 210;
    menuObj->unk_474C8      = 0xFFFF;
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
    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->unk_474E8);
    func_ov002_02085710(menuObj);
    menuObj->unk_46074 = 6;
    return PRC_STEP_CONTINUE;
}
