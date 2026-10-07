#include "OtosuMenuShared.h"

static PrcStepFn data_ov002_02092d24[] = {
    func_ov002_020880a0,
    func_ov002_0208275c,
    (PrcStepFn)func_ov002_02088dc0,
    OtosuPrcStep_FadeStart_Neutral,
    OtosuPrcStep_FadeWait_Neutral,
    (PrcStepFn)func_ov002_02088e9c,
    (PrcStepFn)func_ov002_02088fa0,
    func_ov002_020879f8,
    (PrcStepFn)func_ov002_02088fd8,
    PrcStep_StartDelay,
    PrcStep_TickDelay,
    PrcStep_StartDelay,
    PrcStep_TickDelay,
    OtosuPrcStep_FadeStart_BrightImmediate,
    OtosuPrcStep_FadeWait_Immediate,
    func_ov002_020890f8,
    OtosuPrcStep_FadeStart_Neutral,
    OtosuPrcStep_FadeWait_Neutral,
    func_ov002_0208920c,
    OtosuPrcStep_FadeStart_BrightImmediate,
    OtosuPrcStep_FadeWait_Immediate,
    func_ov002_02089364,
    func_ov002_02082698,
    func_ov002_02088230,
    PrcStep_PopFrame,
};

const Ov002_U16_2 data_ov002_02091f18[0x19] = {
    {0x0000, 0x0010},
    {0x0058, 0x0050},
    {0x0070, 0x0001},
    {0x0010, 0x0078},
    {0x0050, 0x0090},
    {0x0002, 0x0010},
    {0x0098, 0x0050},
    {0x00B0, 0x0003},
    {0x0060, 0x0058},
    {0x00A0, 0x0070},
    {0x0004, 0x0060},
    {0x0078, 0x00A0},
    {0x0090, 0x0005},
    {0x0060, 0x0098},
    {0x00A0, 0x00B0},
    {0x0006, 0x00B0},
    {0x0058, 0x00F0},
    {0x0070, 0x0007},
    {0x00B0, 0x0078},
    {0x00F0, 0x0090},
    {0x0008, 0x00B0},
    {0x0098, 0x00F0},
    {0x00B0, 0xFFFF},
    {0xFFFF, 0xFFFF},
    {0xFFFF, 0xFFFF}
}; /* const */

static PrcStepFn data_ov002_02092d88[] = {
    func_ov002_020880a0,
    func_ov002_0208275c,
    func_ov002_0208847c,
    func_ov002_02088524,
    func_ov002_02088610,
    OtosuPrcStep_FadeStart_Neutral,
    func_ov002_020879f8,
    OtosuPrcStep_FadeWait_Neutral,
    func_ov002_020886ec,
    PrcStep_StartDelay,
    PrcStep_TickDelay,
    PrcStep_StartDelay,
    PrcStep_TickDelay,
    OtosuPrcStep_FadeStart_BrightImmediate,
    OtosuPrcStep_FadeWait_Immediate,
    func_ov002_02088998,
    OtosuPrcStep_FadeStart_Neutral,
    OtosuPrcStep_FadeWait_Neutral,
    func_ov002_02088b28,
    OtosuPrcStep_FadeStart_BrightImmediate,
    OtosuPrcStep_FadeWait_Immediate,
    func_ov002_020893b8,
    func_ov002_020893d8,
    func_ov002_02089498,
    func_ov002_02082698,
    func_ov002_02088230,
    PrcStep_PopFrame,
};

static const s32 data_ov002_02091d4c[0xA] = {
    0x1398, 0x2730, 0x3AC8, 0x4E78, 0x6220, 0x7600, 0x8998, 0x9D30, 0xB0C8, 0xC460,
}; /* const */

static const s32 data_ov002_02091d74[0xA] = {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10}; /* const */
PrcFrameDesc     data_ov002_02092ce0      = {
             .enter     = func_ov002_02087a18,
             .stepTable = data_ov002_02092d24,
             .update    = func_ov002_02087c80,
             .render    = func_ov002_02087ca0,
             .exit      = func_ov002_02087a40,
};
static const s32 data_ov002_02091d9c[0xA] = {
    0x1388, 0x1388, 0x1388, 0x1388, 0x1388, 0x1388, 0x1388, 0x1388, 0x1388, 0x1388,
}; /* const */
const Ov002_U16_2 data_ov002_02091f7c[0x19] = {
    {0x0000, 0x0010},
    {0x0058, 0x0050},
    {0x0070, 0x0001},
    {0x0010, 0x0078},
    {0x0050, 0x0090},
    {0x0002, 0x0010},
    {0x0098, 0x0050},
    {0x00B0, 0x0003},
    {0x0060, 0x0058},
    {0x00A0, 0x0070},
    {0x0004, 0x0060},
    {0x0078, 0x00A0},
    {0x0090, 0x0005},
    {0x0060, 0x0098},
    {0x00A0, 0x00B0},
    {0x0006, 0x00B0},
    {0x0058, 0x00F0},
    {0x0070, 0x0007},
    {0x00B0, 0x0078},
    {0x00F0, 0x0090},
    {0x0008, 0x00B0},
    {0x0098, 0x00F0},
    {0x00B0, 0xFFFF},
    {0xFFFF, 0xFFFF},
    {0xFFFF, 0xFFFF}
}; /* const */
u16              data_ov002_020935c0[0x10];
static const s32 data_ov002_02091dec[0xA] = {
    0x10, 0x13A8, 0x2740, 0x3AD8, 0x4E88, 0x6268, 0x7610, 0x89A8, 0x9D40, 0xB0D8,
}; /* const */
static const s32               data_ov002_02091e14[0xA] = {0, 0, 0, 0x18, 0x10, 0x10, 0, 0, 0, 0}; /* const */
static const Ov002_Config91cc4 data_ov002_02091cc4      = {
    0x0001, 0x0002, 0x0003, 0x0004, 0x0027, 0x0000, "data/BeBadge_MapData.bin"};              /* const */
PrcFrameDesc data_ov002_02092cf4 = {
    .enter     = func_ov002_02087a18,
    .stepTable = data_ov002_02092d88,
    .update    = func_ov002_02087c80,
    .render    = func_ov002_02087ca0,
    .exit      = func_ov002_02087a40,
};
static const Ov002_U16_27 data_ov002_02091e64 = {
    3,    7, 0x55, 3,    7, 0x75, 3,    7, 0x95, 3,    0x57, 0x55, 3,    0x57,
    0x75, 3, 0x57, 0x95, 3, 0xA7, 0x55, 3, 0xA7, 0x75, 3,    0xA7, 0x95,
}; /* const */
static const OtosuMenuRectList2 data_ov002_02091cd4 = {
    {
     {0x23FA, 0x0008, 0x0008, 0x00F8, 0x0040},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000},
     }
};
static const OtosuMenuRectList2 data_ov002_02091cfc = {
    {
     {0x23F8, 0x0008, 0x0008, 0x00F8, 0x0040},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000},
     }
};
static const OtosuMenuRectList2 data_ov002_02091ce8 = {
    {
     {0x23F9, 0x0008, 0x0008, 0x00F8, 0x0040},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000},
     }
};
static const OtosuMenuRectList2 data_ov002_02091d10 = {
    {
     {0x23F8, 0x0008, 0x0008, 0x00F8, 0x0040},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000},
     }
};
static const Ov002_U16_27 data_ov002_02091e9a = {
    3,    7, 0x55, 3,    7, 0x75, 3,    7, 0x95, 3,    0x57, 0x55, 3,    0x57,
    0x75, 3, 0x57, 0x95, 3, 0xA7, 0x55, 3, 0xA7, 0x75, 3,    0xA7, 0x95,
}; /* const */
static const s32 data_ov002_02091d24[0xA] = {0, 0, 0, 0, 0x38, 0, 0, 0, 0, 0};        /* const */
static const s32 data_ov002_02091e3c[0xA] = {0,      0x1398, 0x2730, 0x3AC8, 0x4E78,
                                             0x6258, 0x7600, 0x8998, 0x9D30, 0xB0C8}; /* const */
static const s32 data_ov002_02091dc4[0xA] = {
    0x1398, 0x2730, 0x3AC8, 0x4E60, 0x6210, 0x75F0, 0x8998, 0x9D30, 0xB0C8, 0xC460,
}; /* const */
u16                         data_ov002_020935e0[0x40];
static const Ov002_U16_18x2 data_ov002_02091ed0 = {
    {
     {0x0000, 0x0032},
     {0x0001, 0x0002},
     {0x0002, 0x0004},
     {0x0003, 0x0004},
     {0x0004, 0x0006},
     {0x0005, 0x0006},
     {0x0006, 0x0008},
     {0x0007, 0x0008},
     {0x0008, 0x0008},
     {0x0009, 0x0006},
     {0x0008, 0x0006},
     {0x0007, 0x0005},
     {0x0006, 0x0004},
     {0x0005, 0x0004},
     {0x0004, 0x0003},
     {0x0003, 0x0002},
     {0x0002, 0x0002},
     {0x0001, 0x0001},
     }
}; /* const */

PrcStepResult func_ov002_020879f8(PrcCtx* ctx, void* object) {
    func_ov002_02082ab4(object);
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

void func_ov002_02087a18(PrcCtx* ctx, OtosuMenuObj* menuObj) {
    menuObj->unk_41FE9   = 1;
    menuObj->unk_46300   = 0;
    menuObj->cursor.posX = 0;
    menuObj->cursor.posY = 0xC8;
}

void func_ov002_02087a40(PrcCtx* ctx, OtosuMenuObj* menuObj) {
    menuObj->unk_46300 = 0;
    func_ov002_02085710(menuObj);
    menuObj->cursor.posX = 0;
    menuObj->cursor.posY = 0xC8;
    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->objCtx[0]);
    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->objCtx[4]);
}

void func_ov002_02087aa8(OtosuMenuObj* menuObj) {
    Ov002_U16_18x2 frames = data_ov002_02091ed0;
    u16            select_values[4];
    u16            temp_r3;
    void*          pack;
    void*          var_r4_2;
    void*          var_ip;

    if (menuObj->unk_474D2 == 0) {
        menuObj->unk_474D0++;
        menuObj->unk_474D0 %= 0x12U;
        menuObj->unk_474D2 = frames.data[menuObj->unk_474D0][1];
    } else {
        menuObj->unk_474D2--;
        return;
    }
    if (menuObj->unk_46300 == 0) {
        return;
    }
    pack             = menuObj->packs[1];
    select_values[0] = data_ov002_02091cc4.unk0;
    select_values[1] = data_ov002_02091cc4.unk2;
    select_values[2] = data_ov002_02091cc4.unk4;
    select_values[3] = data_ov002_02091cc4.unk6;
    temp_r3          = select_values[gSaveData.otosuGameKey];
    if ((pack == NULL) || ((s32)temp_r3 <= 0)) {
        var_r4_2 = NULL;
    } else {
        var_r4_2 = Data_GetPackEntryData((Data*)pack, temp_r3);
    }
    if (pack == NULL) {
        var_ip = NULL;
    } else {
        var_ip = Data_GetPackEntryData((Data*)pack, 5);
    }
    MI_CpuCopyU16((s32)(var_ip + (frames.data[menuObj->unk_474D0][0] << 5)), var_r4_2 + 0x20, 0x20);
    if (menuObj->subPalette != NULL) {
        PaletteMgr_ReleaseResource(g_PaletteManagers[1], menuObj->subPalette);
    }
    menuObj->subPalette = PaletteMgr_AllocPalette(g_PaletteManagers[1], var_r4_2, 0, 0, 0x10);
    PaletteMgr_Flush(g_PaletteManagers[1], menuObj->subPalette);
}

void func_ov002_02087c80(PrcCtx* ctx, OtosuMenuObj* menuObj) {
    if (menuObj->unk_46300 == 0) {
        return;
    }
    func_ov002_02087aa8(menuObj);
}

void func_ov002_02087ca0(void) {}

void func_ov002_02087ca4(OtosuMenuObj* menuObj, s32 arg1) {
    BinIdentifier binId;
    s32           sp8;
    s32           sp4;
    s32           temp_r6;
    s32           temp_r6_2;
    s32           temp_r6_3;
    s32           temp_r6_4;
    Data*         data;

    sp4        = data_ov002_02091cc4.unk8;
    sp8        = data_ov002_02091cc4.unkC;
    binId.id   = (u32)sp4;
    binId.path = (char*)sp8;
    data = DatMgr_LoadRawDataWithOffset(1, &menuObj->mapHeader, data_ov002_02091d74[arg1], &binId, data_ov002_02091e3c[arg1]);
    DatMgr_ReleaseData(data);
    if (data_ov002_02091d9c[arg1] != 0) {
        if (menuObj->mapCells != NULL) {
            Mem_Free(&menuObj->heap, menuObj->mapCells);
        }
        temp_r6           = data_ov002_02091d9c[arg1];
        menuObj->mapCells = Mem_AllocHeapTail(&menuObj->heap, temp_r6);
        temp_r6_2         = DatMgr_LoadRawDataWithOffset(1, menuObj->mapCells, temp_r6, &binId, data_ov002_02091dec[arg1]);
        if (menuObj->mapCells == NULL) {
            OS_WaitForever();
        }
        DatMgr_ReleaseData(temp_r6_2);
    } else {
        menuObj->mapCells = NULL;
    }
    if (data_ov002_02091e14[arg1] != 0) {
        if (menuObj->mapExtra1 != NULL) {
            Mem_Free(&menuObj->heap, menuObj->mapExtra1);
        }
        temp_r6_3          = data_ov002_02091e14[arg1];
        menuObj->mapExtra1 = Mem_AllocHeapTail(&menuObj->heap, temp_r6_3);
        data               = DatMgr_LoadRawDataWithOffset(1, menuObj->mapExtra1, temp_r6_3, &binId, data_ov002_02091dc4[arg1]);
        DatMgr_ReleaseData(data);
    } else {
        menuObj->mapExtra1 = NULL;
    }
    if (data_ov002_02091d24[arg1] != 0) {
        if (menuObj->mapExtra2 != NULL) {
            Mem_Free(&menuObj->heap, menuObj->mapExtra2);
        }
        temp_r6_4          = data_ov002_02091d24[arg1];
        menuObj->mapExtra2 = Mem_AllocHeapTail(&menuObj->heap, temp_r6_4);
        data               = DatMgr_LoadRawDataWithOffset(1, menuObj->mapExtra2, temp_r6_4, &binId, data_ov002_02091d4c[arg1]);
        DatMgr_ReleaseData(data);
        return;
    }
    menuObj->mapExtra2 = NULL;
}

void func_ov002_02087eb4(void* arg0, s32 arg1) {
    typedef struct {
        u8  unk0;
        u8  unk1;
        u8  unk2;
        u8  unk3;
        u8  pad_4[0x10 - 4];
        u8* unk10;
    } Ov002_MapBlock;

    s32             sp10;
    s32             spC;
    s32             sp8;
    s32             sp4;
    s32             sp0;
    s16             var_r4;
    s32             temp_r0;
    s32             temp_r11;
    s32             temp_r2_2;
    s32             var_ip;
    s32             var_r2;
    s32             var_r3;
    s32             var_r6;
    s32             var_r7;
    s32             var_r8;
    s32             var_r9;
    u16             var_r5;
    u32             temp_r2;
    u32             temp_r3_2;
    u32             temp_r4;
    u32             temp_r4_2;
    u8              temp_r0_2;
    u8              temp_r3;
    u8              temp_r5;
    Ov002_MapBlock* temp_lr;
    u8*             temp_r0_3;

    temp_lr   = (Ov002_MapBlock*)((u8*)arg0 + 0x46304);
    temp_r5   = temp_lr->unk2;
    temp_r3   = temp_lr->unk3;
    sp8       = 0;
    temp_r4   = 0x28 - (temp_r5 - 0xA);
    temp_r2   = 0x28 - (temp_r3 - 0xA);
    temp_r3_2 = temp_r3 - 9;
    temp_r0   = (s32)(temp_r3_2 + (temp_r3_2 >> 0x1F)) >> 1;
    temp_r4_2 = temp_r5 - 9;
    sp4       = temp_r0;
    sp0       = ((s32)(temp_r4 + (temp_r4 >> 0x1F)) >> 1) + 8;
    spC       = ((s32)(temp_r2 + (temp_r2 >> 0x1F)) >> 1) + 2;
    temp_r11  = (s32)(temp_r4_2 + (temp_r4_2 >> 0x1F)) >> 1;
    if (temp_r0 <= 0) {
        return;
    }
    var_r6 = 5;
    var_r7 = 7;
    do {
        var_ip = 0;
        if (temp_r11 > 0) {
            var_r8 = 5;
            var_r9 = 7;
            sp10   = arg1 + (sp0 * 2) + (((spC + sp8) << 5) * 2);
            do {
                var_r4 = 0x2000;
                var_r5 = 1;
                var_r3 = var_r6;
                if (var_r6 < var_r7) {
                    do {
                        var_r2 = var_r8;
                        if (var_r8 < var_r9) {
                            do {
                                temp_r0_2 = *(temp_lr->unk10 + (var_r3 * temp_lr->unk2 * 2) + (var_r2 * 2));
                                var_r2 += 1;
                                if (temp_r0_2 != 0) {
                                    var_r4 |= var_r5;
                                }
                                var_r5 *= 2;
                            } while (var_r2 < var_r9);
                        }
                        var_r3 += 1;
                    } while (var_r3 < var_r7);
                }
                var_r8 += 2;
                temp_r0_3 = (u8*)(sp10 + (var_ip * 2));
                var_ip += 1;
                *(u16*)(temp_r0_3 + 4) = var_r4;
                var_r9 += 2;
            } while (var_ip < temp_r11);
        }
        var_r6 += 2;
        temp_r2_2 = sp8 + 1;
        sp8       = temp_r2_2;
        var_r7 += 2;
    } while (temp_r2_2 < sp4);
}

void func_ov002_0208800c(OtosuMenuObj* menuObj) {
    u16 index = menuObj->unk_4196C;
    if (index == 0xFFFF) {
        func_0203aafc(3, G2S_GetBG0ScrPtr(), 0, 0x1000);
        return;
    }
    func_ov002_02087ca4(menuObj, index);
    if (menuObj->mapCells == NULL) {
        OS_WaitForever();
    }
    menuObj->subScreens[0][0] = menuObj->tilemap;
    func_ov002_02087eb4(menuObj, menuObj->subScreens[0][0]);
    DC_PurgeAll();
    func_0203ab7c(3, menuObj->tilemap, G2S_GetBG0ScrPtr(), 0x1000);
}

PrcStepResult func_ov002_020880a0(PrcCtx* ctx, void* unused) {
    g_DisplaySettings.controls[0].windows |= 4;
    g_DisplaySettings.engineState[0].windowObj            = 12;
    g_DisplaySettings.engineState[0].windowObjEffects     = TRUE;
    g_DisplaySettings.engineState[0].windowOutside        = 31;
    g_DisplaySettings.engineState[0].windowOutsideEffects = TRUE;
    g_DisplaySettings.engineState[0].blendMode            = 1;
    g_DisplaySettings.engineState[0].blendLayer0          = 4;
    g_DisplaySettings.engineState[0].blendLayer1          = 8;
    g_DisplaySettings.engineState[0].blendCoeff0          = 12;
    g_DisplaySettings.engineState[0].blendCoeff1          = 4;
    g_DisplaySettings.engineState[1].blendMode            = 1;
    g_DisplaySettings.engineState[1].windowObj            = 12;
    g_DisplaySettings.engineState[1].windowObjEffects     = TRUE;
    g_DisplaySettings.engineState[1].blendLayer0          = 4;
    g_DisplaySettings.engineState[1].blendLayer1          = 8;
    g_DisplaySettings.engineState[1].windowOutside        = 31;
    g_DisplaySettings.engineState[1].windowOutsideEffects = FALSE;
    g_DisplaySettings.engineState[1].blendCoeff0          = 12;
    g_DisplaySettings.engineState[1].blendCoeff1          = 4;

    Display_InitMainBG0(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_256x256, GX_BG_COLORS_16, 0, 5, 0, 0x4014);
    Display_InitMainBG1(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_256x256, GX_BG_COLORS_16, 2, 1, 0, 0x4204);
    Display_InitMainBG2(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_256x256, GX_BG_COLORS_16, 4, 1, 1, 0x4404);
    Display_InitMainBG3(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_256x256, GX_BG_COLORS_16, 6, 3, 1, 0x460c);

    Display_Commit();
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02088230(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    menuObj->nextScene = 2;
    menuObj->unk_41FF0 = 0;
}

void func_ov002_0208824c(void* arg0, OtosuMenuObj* menuObj) {
    menuObj->unk_41950 = 0;
}

s32 func_ov002_0208825c(OtosuMenuChildInfo* child, OtosuMenuObj* menuObj) {
    if (child->aid >= 4) {
        return FALSE;
    }
    MI_CpuCopyU8(child->name, menuObj->players[child->unk_2A].name, 0x16);
    MI_CpuCopyU8(child->bssid, menuObj->players[child->unk_2A].bssid, 6);
    menuObj->playerMask |= 1 << child->aid;
    menuObj->unk_41836 |= 1 << child->aid;
    menuObj->unk_4195A[child->aid] = 0;
    // No return: falls through with r0 still holding the bit that was just shifted in.
}

void func_ov002_02088310(OtosuMenuChildInfo* child, OtosuMenuObj* menuObj) {
    if (child->aid < 4) {
        menuObj->playerMask &= ~(1 << child->aid);
        menuObj->unk_41836 |= 1 << child->aid;
    }
}

void func_ov002_02088358(void* arg0, OtosuMenuObj* menuObj) {
    menuObj->unk_4198C = 0;
}

void func_ov002_02088368(s32 arg0, void* arg1, void* arg2, OtosuMenuObj* menuObj) {
    u16 value0;
    u16 value2;

    if (arg1 == NULL) {
        return;
    }
    if (arg2 != (void*)0xA) {
        return;
    }
    if (menuObj == 0) {
        return;
    }
    value0                   = *(u16*)arg1;
    value2                   = *(u16*)((u8*)arg1 + 2);
    menuObj->unk_4195A[arg0] = value0;
    menuObj->unk_41962[arg0] = value2;
}

void func_ov002_020883a4(OtosuMenuObj* menuObj) {
    menuObj->unk_41990 = 1;
    menuObj->unk_41954 = 1;
    u32 irqState       = OS_DisableIRQ();
    menuObj->unk_41836 = 0;
    OS_RestoreIRQ(irqState);
}

void func_ov002_020883d8(OtosuMenuObj* menuObj) {
    s32 var_ip;

    menuObj->unk_41990 = 0;
    menuObj->unk_4198A = func_02047e84(menuObj->playerMask);
    var_ip             = 1;
    menuObj->unk_4198C = 1;
    if (menuObj->unk_41994 != 0) {
        if ((u32)menuObj->unk_4198A >= 4U) {
            var_ip = 0;
        }
    } else {
        var_ip = 0;
    }
    if (func_ov040_0209d788(func_ov002_02088358, menuObj, menuObj->unk_4196E, 0x1E, var_ip) == 0) {
        menuObj->unk_4198C = 0;
    }
    func_ov002_02082ab4(menuObj);
}

PrcStepResult func_ov002_0208847c(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    menuObj->playerMask = 0;
    menuObj->unk_41836  = 0;
    menuObj->unk_4198C  = 0;
    menuObj->unk_41990  = 0;
    menuObj->unk_41994  = 1;
    menuObj->unk_41950  = 0;
    menuObj->unk_41954  = 0;
    menuObj->unk_41958  = 0;
    MI_CpuSet(menuObj->unk_4195A, 0, 8);
    MI_CpuSet(menuObj->unk_41962, 0, 8);
    menuObj->unk_4196A  = 0;
    menuObj->unk_4198A  = 1;
    menuObj->playerMask = 1;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02088524(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    s32 temp_r4;
    s32 temp_r5;
    u32 temp_r0;

    temp_r0 = func_ov040_0209cb78();
    switch (temp_r0) {
        case 0:
        case 2:
        case 3:
        case 5:
        case 6:
            break;
        default:
            break;
        case 1:
            func_ov040_0209cb9c();
            break;
        case 7:
            temp_r5 = func_ov040_0209cde4();
            temp_r4 = func_020442f8();
            func_ov040_0209d818(3, 0, 0x78, 0xA, 1);
            func_ov040_0209d848(1);
            func_ov040_0209cb08(menuObj->unk_4196E, 0x1E);
            func_ov040_0209d40c(func_ov002_0208825c, menuObj);
            func_ov040_0209d420(func_ov002_02088310, menuObj);
            func_ov003_0209d434(func_ov002_02088368, menuObj);
            func_ov040_0209d0a8(0, temp_r4, temp_r5);
            break;
        case 4:
            PrcCtx_AdvanceStep(ctx);
            break;
    }
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02088610(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    OtosuMenuRectList2 table_sp0 = data_ov002_02091cfc;
    u16                i;

    PrcCtx_Init(&menuObj->objCtx[4], "OtosuMenu_Icon2", 0x40);
    PrcCtx_ReplaceFrame(&menuObj->objCtx[4], &OtosuMenu_Icon2_FrameDesc, NULL);
    PrcMaster_RegisterContext(&menuObj->prcMaster, &menuObj->objCtx[4]);
    menuObj->cursor.posX = 0;
    menuObj->cursor.posY = 0xC8;
    func_ov002_02082f18(menuObj, 0xFFFF, 0x16, table_sp0.rects);
    menuObj->unk_474C8 = 600;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_020886ec(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    s32   sp4;
    s32   var_r4;
    s32   var_r9;
    s32   var_r9_2;
    u16   temp_r6;
    u32   var_r0;
    void* temp_r7;

    var_r4 = 0;
    func_ov002_020824a0();

    if (menuObj->unk_41836 != 0) {
        func_ov002_020883a4(menuObj);
    }
    if ((menuObj->unk_41990 != 0) && (menuObj->unk_4198C == 0)) {
        func_ov002_020883d8(menuObj);
    }
    if ((menuObj->playerMask != 1) && (menuObj->unk_41950 == 0)) {
        if (menuObj->unk_41954 != 0) {
            menuObj->unk_41954 = 0;
            var_r4             = 1;
            menuObj->unk_41958++;
        } else {
            var_r9 = 1;
        loop_16:
            if (var_r9 < 4) {
                temp_r6 = menuObj->playerMask;
                if (temp_r6 & (1 << var_r9)) {
                    temp_r7 = menuObj + (var_r9 * 2);
                    if (temp_r6 != *menuObj->unk_4195A) {
                        var_r4 = 1;
                    } else if (menuObj->unk_41958 != *menuObj->unk_41962) {
                        var_r4 = 1;
                    } else {
                        goto block_15;
                    }
                } else {
                block_15:
                    var_r9 += 1;
                    goto loop_16;
                }
            }
            if ((var_r4 == 0) && (menuObj->unk_4198A == gSaveData.otosuPlayerCount)) {
                menuObj->unk_4196A = 4U;
                var_r4             = 1;
            }
        }
    }
    if (var_r4 != 0) {
        sp4                    = OS_DisableIRQ();
        data_ov002_020935e0[0] = menuObj->unk_4196A;
        if (menuObj->unk_4196A != 0) {

        } else {
            var_r9_2                  = 0;
            data_ov002_020935e0[1]    = menuObj->playerMask;
            data_ov002_020935e0[0x3A] = menuObj->unk_41958;
        loop_31:
            if (var_r9_2 < 4) {
                if (menuObj->playerMask & (1 << var_r9_2)) {
                    if (var_r9_2 == 0) {
                        var_r0 = 0;
                    loop_28:
                        if (var_r0 < 0xBU) {
                            var_r0 = (u32)(u16)(var_r0 + 1);
                            goto loop_28;
                        }
                    }
                    MI_CpuCopyU8(menuObj->players[var_r9_2].name, (s16*)((var_r9_2 * 0x16) + (s32)&data_ov002_020935e0[2]),
                                 0x16);
                }
                var_r9_2 += 1;
                goto loop_31;
            }
        }
        OS_RestoreIRQ(sp4);
        menuObj->unk_41950 = 1;
        if (func_ov040_0209d48c(data_ov002_020935e0, 0x78, func_ov002_0208824c, menuObj) == 0) {
            menuObj->unk_41950 = 0;
        }
    }
    menuObj->unk_474C8--;
    if (menuObj->unk_474C8 == 0) {
        PrcCtx_ReplaceFrame(&menuObj->mainCtx, &data_ov002_02092e18, NULL);
        return 0;
    }
    if (menuObj->unk_4196A == 4) {
        PrcCtx_AdvanceStep(ctx);
    }
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02088998(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    OtosuMenuRectList2 table_sp0 = data_ov002_02091ce8;
    u16                i;

    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->objCtx[4]);
    func_ov002_02084494(menuObj, gSaveData.otosuGameKey, table_sp0.rects);
    menuObj->unk_46300 = 1;
    PrcCtx_Init(&menuObj->objCtx[0], "OtosuMenu_DaizaObj", 0x40);
    PrcCtx_ReplaceFrame(&menuObj->objCtx[0], &OtosuMenu_Daiza_FrameDesc, NULL);
    PrcMaster_RegisterContext(&menuObj->prcMaster, &menuObj->objCtx[0]);
    menuObj->unk_4196C = 0xFFFF;
    menuObj->unk_474C8 = 0xFFFF;
    menuObj->unk_474CA = menuObj->playerMask;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

void func_ov002_02088aa4(void* arg0, OtosuMenuObj* menuObj) {
    s32 temp_r5;
    u16 temp_r0;

    temp_r5                = OS_DisableIRQ();
    data_ov002_020935e0[0] = menuObj->unk_4196A;
    temp_r0                = menuObj->unk_4196A;
    switch (temp_r0) { /* irregular */
        case 5:
            data_ov002_020935e0[0x3B] = menuObj->unk_4196C;
            break;
        case 6:
            PrcCtx_AdvanceStep(arg0);
            break;
    }
    OS_RestoreIRQ(temp_r5);
    func_ov040_0209d48c(data_ov002_020935e0, 0x78, func_ov002_0208824c, menuObj);
}

PrcStepResult func_ov002_02088b28(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    u16          table_sp36[0x32];
    Ov002_U16_27 table_sp0 = data_ov002_02091e9a;
    u16          temp_r4;
    u16          i;

    for (i = 0; i < 0x19 * 2; i++) {
        table_sp36[i] = ((const u16*)data_ov002_02091f7c)[i];
    }

    temp_r4 = func_ov002_0208597c((const OtosuMenuRect*)table_sp36);
    if (menuObj->unk_474CA != func_ov040_0209cb68()) {
        PrcCtx_ReplaceFrame(&menuObj->mainCtx, &data_ov002_02092e18, NULL);
        return 0;
    }
    if (temp_r4 == 0xFFFF) {
        return 0;
    }
    if (temp_r4 == 0xFFFE) {
        menuObj->unk_474C8   = 0xFFFF;
        menuObj->cursor.posX = 0U;
        menuObj->cursor.posY = 0xD2U;
        menuObj->unk_4196A   = 5;
        menuObj->unk_4196C   = 0xFFFFU;
        func_ov002_0208800c(menuObj);
    } else if (menuObj->unk_4196C == temp_r4) {
        menuObj->unk_4196A = 6;
        SndMgr_StartPlayingSE(SEIDX_SE_BAYBADGE_MENU_EXECUTE);
    } else {
        u16 anim_index = (u16)(temp_r4 * 3);

        SndMgr_StartPlayingSE(SEIDX_SE_BAYBADGE_MENU_CURSOR);
        menuObj->unk_4196C   = temp_r4;
        menuObj->cursor.posX = table_sp0.data[anim_index + 1];
        menuObj->cursor.posY = table_sp0.data[anim_index + 2];
        Sprite_ChangeAnimation(&menuObj->cursor, menuObj->cursor.animData, (s16)table_sp0.data[anim_index],
                               menuObj->cursor.cellTable);
        menuObj->unk_4196A = 5;
        func_ov002_0208800c(menuObj);
    }
    func_ov002_02088aa4(ctx, menuObj);
    return PRC_STEP_CONTINUE;
}

void func_ov002_02088d00(s32 arg0, void* arg1, void* arg2, OtosuMenuObj* menuObj) {
    typedef struct {
        u16 unk0;
        u16 unk2;
        u8  filler04[0x70];
        u16 unk74;
        u16 unk76;
    } Ov002_88d00Data;
    Ov002_88d00Data* msg       = (Ov002_88d00Data*)arg1;
    u8*              msg_bytes = (u8*)arg1;
    u8*              var_r5;
    s32              var_r4;
    u16              temp_r0;
    void*            var_r6;

    if (arg1 == NULL) {
        return;
    }
    if (arg2 != (void*)0x78) {
        return;
    }
    if (menuObj == NULL) {
        return;
    }
    menuObj->unk_4196A = (u16)msg->unk0;
    temp_r0            = msg->unk0;
    switch (temp_r0) { /* irregular */
        case 3:
        case 4:
            return;
        case 0:
            var_r6 = msg_bytes + 4;
            var_r5 = (u8*)menuObj->players[0].name;
            var_r4 = 0;
            do {
                if (msg->unk2 & (1 << var_r4)) {
                    MI_CpuCopyU8((s32)var_r6, var_r5, 0x16);
                }
                var_r4 += 1;
                var_r5 += 0x30;
                var_r6 += 0x16;
            } while (var_r4 < 4);
            menuObj->playerMask = (u16)msg->unk2;
            menuObj->unk_41836  = 0xF;
            menuObj->unk_41958  = (u16)msg->unk74;
            return;
    }
}

s32 func_ov002_02088dc0(void* arg0, OtosuMenuObj* menuObj) {
    OtosuMenuRectList2 table_sp0 = data_ov002_02091d10;
    u16                i;

    PrcCtx_Init(&menuObj->objCtx[4], "OtosuMenu_Icon2", 0x40);
    PrcCtx_ReplaceFrame(&menuObj->objCtx[4], &OtosuMenu_Icon2_FrameDesc, NULL);
    PrcMaster_RegisterContext(&menuObj->prcMaster, &menuObj->objCtx[4]);
    menuObj->cursor.posX = 0;
    menuObj->cursor.posY = 0xC8;
    func_ov002_02082f18(menuObj, 0xFFFF, 0x16, table_sp0.rects);
    menuObj->unk_474C8 = 600;
    PrcCtx_AdvanceStep(arg0);
    return 0;
}

s32 func_ov002_02088e9c(void* arg0, OtosuMenuObj* menuObj) {
    u32 temp_r0 = func_ov040_0209cb78();
    switch (temp_r0) {
        case 0:
        case 2:
        case 3:
            break;
        default:
            break;
        case 1:
            func_ov040_0209d818(3, 0, 0x78, 0xA, 1);
            func_ov040_0209d848(1);
            func_ov040_0209cabc(menuObj->ownName, 0x18);
            func_ov040_0209b8b8(1, menuObj->players[0].bssid, 0);
            func_ov003_0209d434((void (*)(s32, void*, void*, s32))func_ov002_02088d00, menuObj);
            break;
        case 4:
            PrcCtx_AdvanceStep(arg0);
            break;
    }
    menuObj->unk_474C8--;
    if (menuObj->unk_474C8 != 0) {
        return 0;
    }
    PrcCtx_ReplaceFrame(&menuObj->mainCtx, &data_ov002_02092e18, NULL);
    return 0;
}

void func_ov002_02088f90(void* arg0, OtosuMenuObj* menuObj) {
    menuObj->unk_41950 = 0;
}

s32 func_ov002_02088fa0(PrcCtx* ctx, OtosuMenuObj* menuObj) {
    menuObj->unk_474C8 = 0xFFFF;
    menuObj->unk_474CA = 0;
    menuObj->unk_474CC = 0;
    PrcCtx_AdvanceStep(ctx);
    return 0;
}

s32 func_ov002_02088fd8(void* arg0, OtosuMenuObj* menuObj) {
    if (menuObj->unk_41836 != 0) {
        func_ov002_02082ab4(menuObj);
        menuObj->unk_474CC = 1U;
    }
    if ((menuObj->unk_41950 == 0) && (menuObj->unk_474CC != 0)) {
        u32 irqState1          = OS_DisableIRQ();
        data_ov002_020935c0[0] = menuObj->playerMask;
        data_ov002_020935c0[1] = menuObj->unk_41958;
        OS_RestoreIRQ(irqState1);
        menuObj->unk_474CC = 0U;
        menuObj->unk_41950 = 1;
        if (func_ov040_0209d48c(data_ov002_020935c0, 0xA, func_ov002_02088f90, menuObj) == 0) {
            menuObj->unk_41950 = 0;
        }
    }
    if (menuObj->unk_4196A == 4) {
        PrcCtx_AdvanceStep(arg0);
    }
    return 0;
}

void func_ov002_020890c0(s32 arg0, void* arg1, void* arg2, OtosuMenuObj* menuObj) {
    typedef struct {
        u16 unk0;
        u16 unk2;
        u8  filler04[0x70];
        u16 unk74;
        u16 unk76;
    } Ov002_88d00Data;
    Ov002_88d00Data* msg = (Ov002_88d00Data*)arg1;

    if (arg1 == NULL) {
        return;
    }
    if (arg2 != (void*)0x78) {
        return;
    }
    if (menuObj == NULL) {
        return;
    }
    menuObj->unk_4196A = (u16)msg->unk0;
    menuObj->unk_4196C = (u16)msg->unk76;
}

PrcStepResult func_ov002_020890f8(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    OtosuMenuRectList2 table_sp0 = data_ov002_02091cd4;
    u16                i;

    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->objCtx[4]);
    PrcCtx_Init(&menuObj->objCtx[0], "OtosuMenu_DaizaObj", 0x40);
    PrcCtx_ReplaceFrame(&menuObj->objCtx[0], &OtosuMenu_Daiza_FrameDesc, NULL);
    PrcMaster_RegisterContext(&menuObj->prcMaster, &menuObj->objCtx[0]);
    menuObj->unk_46300 = 1;
    menuObj->unk_474C8 = 0xFFFF;
    func_ov002_02084494(menuObj, gSaveData.otosuGameKey, table_sp0.rects);
    menuObj->unk_474CA = menuObj->unk_4196C;
    func_ov003_0209d434((void (*)(s32, void*, void*, s32))func_ov002_020890c0, menuObj);
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_REPEAT;
}

PrcStepResult func_ov002_0208920c(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    Ov002_U16_27 table_sp0 = data_ov002_02091e64;
    u16          temp_r7;
    u16          temp_r3;
    u16          i;

    if (menuObj->unk_4196A == 5) {
        temp_r3 = menuObj->unk_4196C;
        if (temp_r3 == 0xFFFF) {
            menuObj->unk_474CA   = temp_r3;
            menuObj->cursor.posX = 0U;
            menuObj->cursor.posY = 0xD2U;
            menuObj->unk_4196A   = 5U;
            func_ov002_0208800c(menuObj);
        } else if (menuObj->unk_474CA != temp_r3) {
            SndMgr_StartPlayingSE(SEIDX_SE_BAYBADGE_MENU_CURSOR);
            menuObj->unk_474CA   = menuObj->unk_4196C;
            temp_r7              = (u16)(menuObj->unk_4196C * 3);
            menuObj->cursor.posX = table_sp0.data[temp_r7 + 1];
            menuObj->cursor.posY = table_sp0.data[temp_r7 + 2];
            Sprite_ChangeAnimation(&menuObj->cursor, menuObj->cursor.animData, (s16)table_sp0.data[temp_r7],
                                   menuObj->cursor.cellTable);
            func_ov002_0208800c(menuObj);
        }
    }
    if (menuObj->unk_4196A == 6) {
        SndMgr_StartPlayingSE(SEIDX_SE_BAYBADGE_MENU_EXECUTE);
        PrcCtx_AdvanceStep(ctx);
    }
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02089364(PrcCtx* ctx, void* unused) {
    switch (func_ov040_0209cb78()) {
        default:
            break;
        case 0:
            OS_WaitForever();
            break;
        case 1:
            PrcCtx_AdvanceStep(ctx);
            break;
        case 4:
            func_ov040_0209d588();
            break;
        case 2:
            func_ov040_0209c158();
            break;
    }
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_020893b8(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    menuObj->unk_474C8 = 0;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_020893d8(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    if (func_ov040_0209cb68() == 1) {
        PrcCtx_AdvanceStep(ctx);
        return PRC_STEP_CONTINUE;
    }
    u32 irqState2        = OS_DisableIRQ();
    *data_ov002_020935e0 = 6;
    OS_RestoreIRQ(irqState2);
    if (func_ov040_0209d48c(data_ov002_020935e0, 0x78, func_ov002_0208824c, menuObj) == 0) {
        PrcCtx_ReplaceFrame(&menuObj->mainCtx, &data_ov002_02092e18, NULL);
        return PRC_STEP_CONTINUE;
    }
    menuObj->unk_474C8++;
    if (menuObj->unk_474C8 == 180) {
        PrcCtx_ReplaceFrame(&menuObj->mainCtx, &data_ov002_02092e18, NULL);
    }
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02089498(PrcCtx* ctx, void* object) {
    u32 temp_r0;

    temp_r0 = func_ov040_0209cb78();
    switch (temp_r0) {
        case 0:
            OS_WaitForever();
            break;
        case 1:
            PrcCtx_AdvanceStep(ctx);
            break;
        case 4:
            if (func_ov040_0209cb68() == 1) {
                func_ov040_0209d588();
            }
            break;
        case 2:
        default:
            func_ov040_0209d588();
            break;
    }
    return PRC_STEP_CONTINUE;
}
