#include "OtosuMenuShared.h"

static const Ov002_U16_10 data_ov002_02092202;

static const OtosuMenuRectList2 data_ov002_020921ee = {
    {
     {0x0000, 0x0068, 0x005C, 0x00B0, 0x0068},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF},
     }
};
static PrcStepFn data_ov002_020930a4[] = {
    func_ov002_0208d144,
    func_ov002_0208275c,
    func_ov002_0208de64,
    OtosuPrcStep_FadeStart_Neutral,
    OtosuPrcStep_FadeWait_Neutral,
    func_ov002_0208df6c,
    func_ov002_0208e080,
    func_ov002_0208e0b8,
    PrcStep_StartDelay,
    PrcStep_TickDelay,
    PrcStep_StartDelay,
    PrcStep_TickDelay,
    OtosuPrcStep_FadeStart_BrightImmediate,
    OtosuPrcStep_FadeWait_Immediate,
    func_ov002_0208e200,
    func_ov002_0208d010,
    OtosuPrcStep_FadeStart_Neutral,
    OtosuPrcStep_FadeWait_Neutral,
    func_ov002_0208e30c,
    OtosuPrcStep_FadeStart_BrightImmediate,
    OtosuPrcStep_FadeWait_Immediate,
    func_ov002_0208e6ac,
    func_ov002_02082698,
    func_ov002_0208d09c,
    func_ov002_0208d154,
    OtosuPrcStep_FadeStart_Neutral,
    OtosuPrcStep_FadeWait_Neutral,
    func_ov002_0208e658,
    func_ov002_0208d22c,
    OtosuPrcStep_FadeStart_BrightImmediate,
    OtosuPrcStep_FadeWait_Immediate,
    func_ov002_0208d27c,
    PrcStep_PopFrame,
};

static PrcStepFn data_ov002_02093128[] = {
    func_ov002_0208d144,
    func_ov002_0208275c,
    func_ov002_0208d6bc,
    func_ov002_0208d4cc,
    OtosuPrcStep_FadeStart_Neutral,
    OtosuPrcStep_FadeWait_Neutral,
    func_ov002_0208d5c0,
    func_ov002_0208d778,
    PrcStep_StartDelay,
    PrcStep_TickDelay,
    PrcStep_StartDelay,
    PrcStep_TickDelay,
    OtosuPrcStep_FadeStart_BrightImmediate,
    OtosuPrcStep_FadeWait_Immediate,
    func_ov002_0208da24,
    func_ov002_0208d010,
    OtosuPrcStep_FadeStart_Neutral,
    OtosuPrcStep_FadeWait_Neutral,
    func_ov002_0208db58,
    OtosuPrcStep_FadeStart_BrightImmediate,
    OtosuPrcStep_FadeWait_Immediate,
    func_ov002_0208e6f8,
    func_ov002_0208e750,
    func_ov002_0208e838,
    func_ov002_02082698,
    func_ov002_0208d09c,
    func_ov002_0208d154,
    OtosuPrcStep_FadeStart_Neutral,
    OtosuPrcStep_FadeWait_Neutral,
    func_ov002_0208e7d8,
    func_ov002_0208d22c,
    OtosuPrcStep_FadeStart_BrightImmediate,
    OtosuPrcStep_FadeWait_Immediate,
    func_ov002_0208d27c,
    PrcStep_PopFrame,
};
static const OtosuMenuRectList3 data_ov002_02092252 = {
    {
     {0x2407, 0x68, 0x46, 0xB0, 0x56},
     {0x2408, 0x68, 0x6E, 0xB0, 0x7E},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0},
     }
};
static const OtosuMenuRectList2 data_ov002_020921da = {
    {
     {0x23F8, 0x0008, 0x0008, 0x00F8, 0x0040},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000},
     }
};
static const OtosuMenuRectList2 data_ov002_0209223e = {
    {
     {0x240B, 0x0008, 0x0008, 0x00F8, 0x0040},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000},
     }
};

static PrcStepFn data_ov002_02093070[] = {
    func_ov002_0208d144,
    func_ov002_0208275c,
    func_ov002_0208e348,
    func_ov002_0208e410,
    OtosuPrcStep_FadeStart_Neutral,
    OtosuPrcStep_FadeWait_Neutral,
    func_ov002_0208e514,
    OtosuPrcStep_FadeStart_BrightImmediate,
    OtosuPrcStep_FadeWait_Immediate,
    func_ov002_0208d010,
    func_ov002_02082698,
    func_ov002_0208d290,
    PrcStep_PopFrame,
};
static const OtosuMenuRectList2 data_ov002_02092216 = {
    {
     {0x240A, 0x0068, 0x005C, 0x00B0, 0x0068},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000},
     }
};
PrcFrameDesc data_ov002_02093034 = {
    .enter     = func_ov002_0208cf84,
    .stepTable = data_ov002_02093128,
    .update    = func_ov002_0208d008,
    .render    = func_ov002_0208d00c,
    .exit      = func_ov002_0208cfe0,
};
static const OtosuMenuRectList3 data_ov002_02092270 = {
    {
     {0, 0x50, 0x40, 0xB0, 0x58},
     {1, 0x50, 0x68, 0xB0, 0x80},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF},
     }
};
static const Ov002_U16_4 data_ov002_0209228e[0x2C] = {
    {0x240C, 0x2421, 0xFFFF, 0xFFFF},
    {0x240C, 0x2423, 0xFFFF, 0xFFFF},
    {0x240C, 0x240D, 0xFFFF, 0xFFFF},
    {0x240C, 0x240E, 0xFFFF, 0xFFFF},
    {0x240C, 0x241F, 0xFFFF, 0xFFFF},
    {0x240C, 0x241B, 0xFFFF, 0xFFFF},
    {0x240C, 0x2428, 0x2429, 0x242A},
    {0x240C, 0x242B, 0x242C, 0x242D},
    {0x240C, 0x2418, 0xFFFF, 0xFFFF},
    {0x240E, 0x2419, 0xFFFF, 0xFFFF},
    {0x240C, 0x2420, 0xFFFF, 0xFFFF},
    {0x240C, 0x241A, 0xFFFF, 0xFFFF},
    {0x240C, 0x241B, 0xFFFF, 0xFFFF},
    {0x240C, 0x241E, 0x241E, 0xFFFF},
    {0x2423, 0x2422, 0x242E, 0x242F},
    {0x2423, 0x241D, 0xFFFF, 0xFFFF},
    {0x2423, 0x2417, 0x2417, 0xFFFF},
    {0x240C, 0x2416, 0x2416, 0x2416},
    {0x240C, 0x241F, 0xFFFF, 0xFFFF},
    {0x2423, 0x2421, 0xFFFF, 0xFFFF},
    {0x240C, 0x241C, 0xFFFF, 0xFFFF},
    {0x2423, 0x2425, 0x2425, 0x2425},
    {0x240C, 0x2411, 0xFFFF, 0xFFFF},
    {0x2426, 0x2412, 0xFFFF, 0xFFFF},
    {0x240E, 0x2413, 0xFFFF, 0xFFFF},
    {0x240D, 0x2414, 0xFFFF, 0xFFFF},
    {0x240C, 0x2415, 0xFFFF, 0xFFFF},
    {0x2423, 0x2411, 0xFFFF, 0xFFFF},
    {0x2423, 0x2411, 0xFFFF, 0xFFFF},
    {0x240C, 0x2423, 0xFFFF, 0xFFFF},
    {0x240C, 0x2424, 0xFFFF, 0xFFFF},
    {0x240C, 0x2421, 0xFFFF, 0xFFFF},
    {0x240C, 0x2421, 0xFFFF, 0xFFFF},
    {0x240C, 0x2423, 0xFFFF, 0xFFFF},
    {0x240C, 0x2421, 0xFFFF, 0xFFFF},
    {0x240C, 0x2423, 0xFFFF, 0xFFFF},
    {0x240C, 0x2423, 0x2421, 0xFFFF},
    {0x240C, 0x2427, 0x2425, 0x241C},
    {0x240C, 0x2418, 0xFFFF, 0xFFFF},
    {0x240C, 0x2419, 0xFFFF, 0xFFFF},
    {0x240C, 0x2420, 0xFFFF, 0xFFFF},
    {0x240C, 0x241A, 0xFFFF, 0xFFFF},
    {0x240C, 0x241E, 0xFFFF, 0xFFFF},
    {0x240C, 0x242E, 0xFFFF, 0xFFFF}
}; /* const */
PrcFrameDesc data_ov002_0209305c = {
    .enter     = func_ov002_0208cf84,
    .stepTable = data_ov002_020930a4,
    .update    = func_ov002_0208d008,
    .render    = func_ov002_0208d00c,
    .exit      = func_ov002_0208cfe0,
};
static const OtosuMenuRectList2 data_ov002_0209222a = {
    {
     {0x2409, 0x0008, 0x0008, 0x00F8, 0x0040},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000},
     }
};
PrcFrameDesc data_ov002_02093048 = {
    .enter     = func_ov002_0208cf84,
    .stepTable = data_ov002_02093070,
    .update    = func_ov002_0208d008,
    .render    = func_ov002_0208d00c,
    .exit      = func_ov002_0208cfe0,
};

void func_ov002_0208ce00(OtosuMenuObj* menuObj) {
    struct {
        u32 unk0;
        u8  unk4;
        u8  unk5;
        u8  unk6;
        u8  unk7;
        u16 unk8;
    } sp0;
    PrcCtx* menu_ptrs[4];
    u32     var_r0;
    s8      var_r8;
    u16     temp_r0_2;
    u32     temp_r0_3;
    u8      temp_r1;
    u8*     temp_r0;
    PrcCtx* temp_r9;

    menu_ptrs[0] = &menuObj->objCtx[0];
    menu_ptrs[1] = &menuObj->objCtx[1];
    menu_ptrs[2] = &menuObj->objCtx[2];
    menu_ptrs[3] = &menuObj->objCtx[3];
    var_r8       = 0;
    if ((s32)gSaveData.otosuPlayerCount <= 0) {
        return;
    }
loop_3:
    temp_r0 = (u8*)menuObj + var_r8;
    temp_r1 = *(u8*)(temp_r0 + 0x41FD5);
    if ((menuObj->firstPlaceUnique != 0) && (var_r8 == 0)) {
        var_r0 = 1;
    } else {
        var_r0 = 0;
    }
    temp_r9 = menu_ptrs[var_r8];
    PrcCtx_Init(temp_r9, "OtosuMenu_RankBoardObj", 0x220);
    sp0.unk0 = var_r0;
    sp0.unk4 = (u8)var_r8;
    sp0.unk5 = *(u8*)(temp_r0 + 0x41FD9);
    sp0.unk6 = temp_r1;
    sp0.unk7 = 0;
    sp0.unk8 = gSaveData.otosuScores[temp_r1];
    PrcCtx_ReplaceFrame(temp_r9, &OtosuMenu_RankBoard_FrameDesc, &sp0);
    PrcMaster_RegisterContext(&menuObj->prcMaster, temp_r9);
    if ((menuObj->firstPlaceUnique != 0) && (menuObj->playerRanks[gSaveData.otosuGameKey] == 0)) {
        menuObj->unk_41FE8 = 2;
        goto block_16;
    }
    if (menuObj->playerRanks[gSaveData.otosuGameKey] == 0) {
        if (menuObj->unk_462EC != 0) {
            menuObj->unk_41FE8 = 2;
            goto block_16;
        }
        menuObj->unk_41FE8 = 0;
        goto block_16;
    }
    menuObj->unk_41FE8 = 1;
block_16:
    temp_r0_2 = var_r8 + 1;
    temp_r0_3 = temp_r0_2 << 0x10;
    var_r8    = (s8)temp_r0_2;
    if ((s32)gSaveData.otosuPlayerCount <= (s32)(temp_r0_3 >> 0x10)) {
        return;
    }
    goto loop_3;
}

void func_ov002_0208cf84(PrcCtx* ctx, OtosuMenuObj* menuObj) {
    u16 var_ip;

    var_ip               = 0;
    menuObj->cursor.posX = 0;
    menuObj->cursor.posY = 0xC8;
    menuObj->unk_41FE9   = 1;
    do {
        u16* value = &gSaveData.otosuScores[var_ip];
        if (*value > 0x270FU) {
            *value = 0x270FU;
        }
        var_ip += 1;
    } while ((u32)var_ip < 4U);
}

void func_ov002_0208cfe0(PrcCtx* ctx, OtosuMenuObj* menuObj) {
    func_ov002_02085710(menuObj);
    menuObj->cursor.posX = 0;
    menuObj->cursor.posY = 0xC8;
}

void func_ov002_0208d008(void) {}

void func_ov002_0208d00c(void) {}

PrcStepResult func_ov002_0208d010(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;
    u32           temp_r3;

    if ((menuObj->firstPlaceUnique != 0) && (menuObj->playerRanks[gSaveData.otosuGameKey] == 0)) {
        gSaveData.unk_1D84 = (u8)(gSaveData.unk_1D84 + 1);
    }
    temp_r3              = gSaveData.mabsBasePP + gSaveData.otosuScores[gSaveData.otosuGameKey];
    gSaveData.mabsBasePP = temp_r3;
    if (temp_r3 > 0x270FU) {
        gSaveData.mabsBasePP = 0x270FU;
    }
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_REPEAT;
}

PrcStepResult func_ov002_0208d09c(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;
    u16           temp_r1;

    temp_r1 = menuObj->unk_4196A;
    switch (temp_r1) { /* irregular */
        case 7:
            func_ov002_0208caa8(menuObj);
            func_ov002_02082610(menuObj);
            func_ov002_02085710(menuObj);
            func_ov002_0208bd40();
            func_ov002_02085a44(menuObj);
            if (gSaveData.otosuGameKey == 0) {
                PrcCtx_ReplaceFrame(&menuObj->mainCtx, &data_ov002_02092cf4, NULL);
            } else {
                PrcCtx_ReplaceFrame(&menuObj->mainCtx, &data_ov002_02092ce0, NULL);
            }
            break;
        case 8:
            PrcCtx_AdvanceStep(ctx);
            break;
        default:
            OS_WaitForever();
            break;
    }
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_0208d144(PrcCtx* ctx, void* unused) {
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_0208d154(PrcCtx* ctx, void* object) {
    OtosuMenuObj*      menuObj   = (OtosuMenuObj*)object;
    OtosuMenuRectList2 table_sp0 = data_ov002_0209223e;
    u16                i;

    menuObj->cursor.posX = 0;
    menuObj->cursor.posY = 0xC8;
    func_ov002_0208caa8(menuObj);
    func_ov002_02084c84(menuObj, table_sp0.rects);
    PrcCtx_Init(&menuObj->objCtx[4], "OtosuMenu_Icon2", 0x40);
    PrcCtx_ReplaceFrame(&menuObj->objCtx[4], &OtosuMenu_Icon2_FrameDesc, NULL);
    PrcMaster_RegisterContext(&menuObj->prcMaster, &menuObj->objCtx[4]);
    menuObj->unk_474C8 = 60;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_0208d22c(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;
    menuObj->unk_474C8--;
    if (menuObj->unk_474C8 == 0) {
        PrcMaster_UnregisterContextOrPanic(&menuObj->prcMaster, &menuObj->objCtx[4]);
        PrcCtx_AdvanceStep(ctx);
    }
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_0208d27c(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;
    menuObj->nextScene    = 3;
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_0208d290(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;
    menuObj->nextScene    = 4;
    return PRC_STEP_CONTINUE;
}

void func_ov002_0208d2a4(void* arg0, OtosuMenuObj* menuObj) {
    menuObj->unk_41950 = 0;
}

s32 func_ov002_0208d2b4(void* arg0, OtosuMenuObj* menuObj) {
    u8* msg       = (u8*)arg0;
    u16 msg_unk10 = *(u16*)(msg + 0x10);
    u16 msg_unk2a = *(u16*)(msg + 0x2A);

    if ((u32)msg_unk10 >= 4U) {
        return 0;
    }
    MI_CpuCopyU8((s32)(msg + 0x14), menuObj->players[msg_unk2a].name, 0x16);
    MI_CpuCopyU8((s32)(msg + 0xA), menuObj->players[msg_unk10].bssid, 6);

    menuObj->playerMask |= (1 << msg_unk10);
    menuObj->unk_41836 |= (1 << msg_unk10);

    menuObj->unk_4195A[msg_unk10] = 0;
    return 1;
}

void func_ov002_0208d368(void* arg0, OtosuMenuObj* menuObj) {
    u16 temp_r3;

    temp_r3 = *(u16*)((u8*)arg0 + 0x10);
    if ((u32)temp_r3 >= 4U) {
        return;
    }
    menuObj->playerMask = (u16)(menuObj->playerMask & ~(1 << temp_r3));
    menuObj->unk_41836  = (u16)(menuObj->unk_41836 | (1 << temp_r3));
}

void func_ov002_0208d3b0(void* arg0, OtosuMenuObj* menuObj) {
    menuObj->unk_4198C = 0;
}

void func_ov002_0208d3c0(s32 arg0, void* arg1, void* arg2, OtosuMenuObj* menuObj) {
    if (arg1 == NULL) {
        return;
    }
    if (arg2 != (void*)0xA) {
        return;
    }
    if (menuObj == 0) {
        return;
    }
    menuObj->unk_4195A[arg0] = *(u16*)arg1;
    menuObj->unk_41962[arg0] = *(u16*)((u8*)arg1 + 2);
}

void func_ov002_0208d3fc(OtosuMenuObj* menuObj) {
    menuObj->unk_41990 = 1;
    menuObj->unk_41954 = 1;
    u32 irqState       = OS_DisableIRQ();
    menuObj->unk_41836 = 0;
    OS_RestoreIRQ(irqState);
}

void func_ov002_0208d430(OtosuMenuObj* menuObj) {
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
    if (func_ov040_0209d788(func_ov002_0208d3b0, menuObj, (u8*)menuObj->unk_4196E, 0x1E, var_ip) == 0) {
        menuObj->unk_4198C = 0;
    }
}

PrcStepResult func_ov002_0208d4cc(PrcCtx* ctx, void* object) {
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
    PrcCtx_Init(&menuObj->objCtx[4], "OtosuMenu_Icon2", 0x40);
    PrcCtx_ReplaceFrame(&menuObj->objCtx[4], &OtosuMenu_Icon2_FrameDesc, NULL);
    PrcMaster_RegisterContext(&menuObj->prcMaster, &menuObj->objCtx[4]);
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_0208d5c0(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;
    s32           temp_r4;
    s32           temp_r5;
    u32           temp_r0;

    temp_r0 = func_ov040_0209cb78();
    switch (temp_r0) {
        default:
            break;
        case 0:
            func_ov040_0209caac(0x400548);
            break;
        case 1:
            func_ov040_0209cb9c();
            break;
        case 7:
            temp_r5 = func_ov040_0209cde4();
            temp_r4 = func_020442f8();
            func_ov040_0209d818(3, 0, 0x78, 0xA, 1);
            func_ov040_0209d848(2);
            func_ov040_0209cb08(menuObj->unk_4196E, 0x1E);
            func_ov040_0209d40c(func_ov002_0208d2b4, menuObj);
            func_ov040_0209d420(func_ov002_0208d368, menuObj);
            func_ov003_0209d434(func_ov002_0208d3c0, menuObj);
            func_ov040_0209d0a8(0, temp_r4, temp_r5);
            break;
        case 4:
            PrcCtx_AdvanceStep(ctx);
            break;
    }
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_0208d6bc(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj   = (OtosuMenuObj*)object;
    Ov002_U16_10  table_sp0 = data_ov002_02092202;
    u16           i;
    u32           temp_r3;

    menuObj->cursor.posX = 0;
    menuObj->cursor.posY = 0xC8;
    func_ov002_02084c84(menuObj, (const OtosuMenuRect*)table_sp0.data);
    temp_r3                  = SystemStatusFlags.unk_06;
    SystemStatusFlags.unk_06 = 0;
    menuObj->unk_46070       = temp_r3;
    SystemStatusFlags;
    SystemStatusFlags.unk_07 = 0;
    menuObj->unk_474C8       = 600;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_0208d778(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;
    s32           sp4;
    void*         sp0;
    s32           var_r4;
    s32           var_r9;
    s32           var_r9_2;
    u16           temp_r6;
    u32           var_r0;

    sp0    = ctx;
    var_r4 = 0;
    func_ov002_020824a0();
    if (menuObj->unk_41836 != 0) {
        func_ov002_0208d3fc(menuObj);
    }
    if ((menuObj->unk_41990 != 0) && (menuObj->unk_4198C == 0)) {
        func_ov002_0208d430(menuObj);
    }
    if ((menuObj->playerMask != 1) && (menuObj->unk_41950 == 0)) {
        if (menuObj->unk_41954 != 0) {
            menuObj->unk_41954 = 0;
            var_r4             = 1;
            menuObj->unk_41958 = (u16)(menuObj->unk_41958 + 1);
        } else {
            var_r9 = 1;
        loop_16:
            if (var_r9 < 4) {
                temp_r6 = menuObj->playerMask;
                if (temp_r6 & (1 << var_r9)) {
                    if (temp_r6 != menuObj->unk_4195A[var_r9]) {
                        var_r4 = 1;
                    } else if (menuObj->unk_41958 != menuObj->unk_41962[var_r9]) {
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
        if (func_ov040_0209d48c(data_ov002_020935e0, 0x78, func_ov002_0208d2a4, menuObj) == 0) {
            menuObj->unk_41950 = 0;
        }
    }
    menuObj->unk_474C8--;
    if (menuObj->unk_474C8 == 0) {
        PrcCtx_ReplaceFrame(&menuObj->mainCtx, &data_ov002_02092e18, NULL);
        return PRC_STEP_CONTINUE;
    }
    if (menuObj->unk_4196A == 4) {
        PrcCtx_AdvanceStep(sp0);
    }
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_0208da24(PrcCtx* ctx, void* object) {
    OtosuMenuObj*      menuObj  = (OtosuMenuObj*)object;
    OtosuMenuRectList3 layout   = data_ov002_02092252;
    s32                names[5] = {0, 0, 0, 0, 0};
    u16                i;

    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->objCtx[4]);
    func_ov002_0208cb50(menuObj);
    for (i = 0; i < gSaveData.otosuPlayerCount; i++) {
        names[i] = func_ov002_0208cdb0(menuObj, i);
    }
    names[i] = 0;
    func_ov002_020850c0(menuObj, 0xA, 0xC, names, layout.rects);
    func_ov002_0208ce00(menuObj);
    menuObj->unk_4196C = 0xFFFF;
    menuObj->unk_474CA = menuObj->playerMask;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_0208db58(PrcCtx* ctx, void* object) {
    OtosuMenuObj*      menuObj  = (OtosuMenuObj*)object;
    u16                anims[6] = {5, 0x47, 0x3C, 5, 0x47, 0x64};
    OtosuMenuRectList3 layout   = data_ov002_02092270;
    s32                temp_ip;
    s32                temp_r5;
    s32                var_r2;
    const Ov002_U16_2* var_r3;
    Ov002_U16_2*       var_r4;
    u16                temp_r0;
    u16                temp_r0_2;
    u16                temp_r1;
    u16                temp_r4;
    u16*               temp_r1_2;

    temp_r4 = func_ov002_0208597c(layout.rects);
    if (menuObj->unk_474CA != func_ov040_0209cb68()) {
        PrcCtx_ReplaceFrame(&menuObj->mainCtx, &data_ov002_02092e18, NULL);
        return PRC_STEP_CONTINUE;
    }
    if (temp_r4 == 0xFFFF) {
        return PRC_STEP_CONTINUE;
    }
    if (temp_r4 == 0xFFFE) {
        menuObj->unk_4196C   = 0xFFFFU;
        menuObj->cursor.posX = 0U;
        menuObj->cursor.posY = 0xD2U;
        return PRC_STEP_CONTINUE;
    }
    if (menuObj->unk_4196C == temp_r4) {
        switch (temp_r4) { /* switch 1; irregular */
            case 0:        /* switch 1 */
                SndMgr_StartPlayingSE(SEIDX_SE_BAYBADGE_MENU_EXECUTE);
                menuObj->unk_4196A = 7U;
                break;
            case 1: /* switch 1 */
                SndMgr_StartPlayingSE(SEIDX_SE_BAYBADGE_MENU_EXECUTE);
                menuObj->unk_4196A = 8U;
                break;
        }
    } else {
        SndMgr_StartPlayingSE(SEIDX_SE_BAYBADGE_MENU_CURSOR);
        menuObj->unk_4196C   = temp_r4;
        temp_r1_2            = &anims[temp_r4 * 3];
        menuObj->cursor.posX = temp_r1_2[1];
        menuObj->cursor.posY = temp_r1_2[2];
        Sprite_ChangeAnimation(&menuObj->cursor, menuObj->cursor.animData, (s16)temp_r1_2[0], menuObj->cursor.cellTable);
    }
    temp_r5                = OS_DisableIRQ();
    data_ov002_020935e0[0] = menuObj->unk_4196A;
    temp_r0_2              = menuObj->unk_4196A;
    switch (temp_r0_2) { /* switch 2; irregular */
        case 7:          /* switch 2 */
            data_ov002_020935e0[0x3B] = menuObj->unk_4196C;
            PrcCtx_AdvanceStep(ctx);
            break;
        case 8: /* switch 2 */
            data_ov002_020935e0[0x3B] = menuObj->unk_4196C;
            PrcCtx_AdvanceStep(ctx);
            break;
    }
    OS_RestoreIRQ(temp_r5);
    func_ov040_0209d48c(data_ov002_020935e0, 0x78, func_ov002_0208d2a4, menuObj);
    return PRC_STEP_CONTINUE;
}

static const Ov002_U16_10 data_ov002_02092202 = {0x23F8, 0x0008, 0x0008, 0x00F8, 0x0040,
                                                 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000}; /* const */
void                      func_ov002_0208dda4(void* arg1, void* arg2, OtosuMenuObj* menuObj) {
    typedef struct {
        u16 unk0;
        u16 unk2;
        u8  filler04[0x70];
        u16 unk74;
        u16 unk76;
    } Ov002_8dda4Data;
    Ov002_8dda4Data* msg = (Ov002_8dda4Data*)arg1;
    u8*              src;
    u8*              dst;
    s32              i;

    if (arg1 == NULL) {
        return;
    }
    if (arg2 != (void*)0x78) {
        return;
    }
    if (menuObj == NULL) {
        return;
    }
    menuObj->unk_4196A = msg->unk0;
    switch (msg->unk0) {
        case 3:
        case 4:
            break;
        case 0:
            src = (u8*)msg + 4;
            dst = (u8*)menuObj->players;
            for (i = 0; i < 4; i++) {
                if (msg->unk2 & (1 << i)) {
                    MI_CpuCopyU8(src, dst, 0x16);
                }
                dst += 0x30;
                src += 0x16;
            }
            menuObj->playerMask = msg->unk2;
            menuObj->unk_41836  = 0xF;
            menuObj->unk_41958  = msg->unk74;
            break;
    }
}

PrcStepResult func_ov002_0208de64(PrcCtx* ctx, void* object) {
    OtosuMenuObj*      menuObj   = (OtosuMenuObj*)object;
    OtosuMenuRectList2 table_sp0 = data_ov002_020921da;
    u16                i;
    u32                temp_r1_2;

    menuObj->cursor.posX = 0;
    menuObj->cursor.posY = 0xC8;
    func_ov002_02084c84(menuObj, table_sp0.rects);
    temp_r1_2                = SystemStatusFlags.unk_06;
    SystemStatusFlags.unk_06 = 0;
    menuObj->unk_46070       = temp_r1_2;
    SystemStatusFlags;
    SystemStatusFlags.unk_07 = 0;
    PrcCtx_Init(&menuObj->objCtx[4], "OtosuMenu_Icon2", 0x40);
    PrcCtx_ReplaceFrame(&menuObj->objCtx[4], &OtosuMenu_Icon2_FrameDesc, NULL);
    PrcMaster_RegisterContext(&menuObj->prcMaster, &menuObj->objCtx[4]);
    menuObj->unk_474C8 = 600;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_0208df6c(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;
    u32           temp_r0;

    temp_r0 = func_ov040_0209cb78();
    switch (temp_r0) {
        case 2:
        case 3:
            break;
        default:
            break;
        case 0:
            func_ov040_0209caac(0x400548);
            break;
        case 1:
            func_ov040_0209d818(3, 0, 0x78, 0xA, 1);
            func_ov040_0209d848(2);
            func_ov040_0209cabc(menuObj->ownName, 0x18);
            func_ov040_0209b8b8(1, menuObj->players[0].bssid, 0);
            func_ov003_0209d434((void (*)(s32, void*, void*, s32))func_ov002_0208dda4, menuObj);
            break;
        case 4:
            PrcCtx_AdvanceStep(ctx);
            break;
    }
    menuObj->unk_474C8--;
    if (menuObj->unk_474C8 != 0) {
        return 0;
    }
    PrcCtx_ReplaceFrame(&menuObj->mainCtx, &data_ov002_02092e18, NULL);
    return 0;
}

void func_ov002_0208e070(void* arg0, OtosuMenuObj* menuObj) {
    menuObj->unk_41950 = 0;
}

PrcStepResult func_ov002_0208e080(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    menuObj->unk_474C8 = 0xFFFF;
    menuObj->unk_474CA = 0;
    menuObj->unk_474CC = 0;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_0208e0b8(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    if (menuObj->unk_41836 != 0) {
        menuObj->unk_474CC = 1U;
    }
    if ((menuObj->unk_41950 == 0) && (menuObj->unk_474CC != 0)) {
        u32 irqState1          = OS_DisableIRQ();
        data_ov002_020935c0[0] = menuObj->playerMask;
        data_ov002_020935c0[1] = menuObj->unk_41958;
        OS_RestoreIRQ(irqState1);
        menuObj->unk_474CC = 0U;
        menuObj->unk_41950 = 1;
        if (func_ov040_0209d48c(data_ov002_020935c0, 0xA, func_ov002_0208e070, menuObj) == 0) {
            menuObj->unk_41950 = 0;
        }
    }
    if (menuObj->unk_4196A == 4) {
        PrcCtx_AdvanceStep(ctx);
    }
    return PRC_STEP_CONTINUE;
}

void func_ov002_0208e194(s32 arg0, void* arg1, void* arg2, OtosuMenuObj* menuObj) {
    u16 temp_r2;

    if (arg1 == NULL) {
        return;
    }
    if (arg2 != (void*)0x78) {
        return;
    }
    if (menuObj == NULL) {
        return;
    }
    temp_r2 = OVMGR_U16(arg1, 0x0);
    switch (temp_r2) { /* irregular */
        case 3:
        case 8:
            menuObj->unk_4196A = 8U;
            menuObj->unk_4196C = (u16)OVMGR_U16(arg1, 0x76);
            return;
        case 7:
            menuObj->unk_4196A = temp_r2;
            menuObj->unk_4196C = (u16)OVMGR_U16(arg1, 0x76);
            return;
    }
}

PrcStepResult func_ov002_0208e200(PrcCtx* ctx, void* object) {
    OtosuMenuObj*      menuObj  = (OtosuMenuObj*)object;
    OtosuMenuRectList2 layout   = data_ov002_0209222a;
    s32                names[5] = {0, 0, 0, 0, 0};
    u16                i;

    func_ov002_0208cb50(menuObj);
    for (i = 0; i < gSaveData.otosuPlayerCount; i++) {
        names[i] = func_ov002_0208cdb0(menuObj, i);
    }
    names[i] = 0;
    func_ov002_020850c0(menuObj, 0xFFFF, 0xB, names, layout.rects);
    func_ov002_0208ce00(menuObj);
    menuObj->unk_474C8 = 0xFFFF;
    func_ov003_0209d434((void (*)(s32, void*, void*, s32))func_ov002_0208e194, menuObj);
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_0208e30c(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;
    u16           temp_r1;

    temp_r1 = menuObj->unk_4196A;
    switch (temp_r1) { /* irregular */
        case 3:
        case 8:
            PrcCtx_AdvanceStep(ctx);
            break;
        case 7:
            PrcCtx_AdvanceStep(ctx);
            break;
    }
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_0208e348(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;
    const u16*    msgIds;
    SysCode*      name;
    u16           i;

    menuObj->unk_41FD0 = 0;
    for (i = 0; i < 4; i++) {
        msgIds = (const u16*)&data_ov002_0209228e[(u8)gSaveData.unk_341C];
        if (msgIds[i] == 0xFFFF) {
            break;
        }
        name = SysFont_GetMsgBuf(&menuObj->font, msgIds[i]);
        MI_CpuCopyU16(name, menuObj->players[i].name, 0x2A);
        Mem_Free(&gDebugHeap, name);
        menuObj->unk_41FD0++;
    }
    gSaveData.otosuPlayerCount = i;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_0208e410(PrcCtx* ctx, void* object) {
    OtosuMenuObj*      menuObj  = (OtosuMenuObj*)object;
    OtosuMenuRectList2 layout   = data_ov002_02092216;
    s32                names[5] = {0, 0, 0, 0, 0};
    u16                i;

    func_ov002_0208cc5c(menuObj);
    for (i = 0; i < menuObj->unk_41FD0; i++) {
        names[i] = func_ov002_0208cdb0(menuObj, i);
    }
    names[i] = 0;
    func_ov002_020850c0(menuObj, 0xE, 0xD, names, layout.rects);
    func_ov002_0208ce00(menuObj);
    menuObj->unk_474C8 = 0xFFFF;
    menuObj->unk_4196C = 0xFFFF;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_0208e514(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    u16                anims[3]  = {5, 0x48, 0x52};
    OtosuMenuRectList2 table_sp6 = data_ov002_020921ee;
    s32                temp_ip;
    u16                temp_r0_2;
    u16                i;

    temp_r0_2 = func_ov002_0208597c(table_sp6.rects);
    if (temp_r0_2 == 0xFFFF) {
        return 0;
    }
    if (temp_r0_2 == 0xFFFE) {
        menuObj->unk_4196C   = 0xFFFFU;
        menuObj->cursor.posX = 0U;
        menuObj->cursor.posY = 0xD2U;
        return PRC_STEP_CONTINUE;
    }
    if (menuObj->unk_4196C == temp_r0_2) {
        if (temp_r0_2 == 0) {
            SndMgr_StartPlayingSE(SEIDX_SE_BAYBADGE_MENU_EXECUTE);
            PrcCtx_AdvanceStep(ctx);
        }
    } else {
        SndMgr_StartPlayingSE(SEIDX_SE_BAYBADGE_MENU_CURSOR);
        temp_ip              = temp_r0_2 * 3;
        menuObj->unk_4196C   = temp_r0_2;
        menuObj->cursor.posX = anims[temp_ip + 1];
        menuObj->cursor.posY = anims[temp_ip + 2];
        Sprite_ChangeAnimation(&menuObj->cursor, menuObj->cursor.animData, (s16)anims[temp_ip], menuObj->cursor.cellTable);
    }
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_0208e658(PrcCtx* ctx, void* object) {
    u32 temp_r0;

    temp_r0 = func_ov040_0209cb78();
    switch (temp_r0) {
        default:
            break;
        case 0:
            PrcCtx_AdvanceStep(ctx);
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
    }
    return 0;
}

PrcStepResult func_ov002_0208e6ac(PrcCtx* ctx, void* object) {
    u32 temp_r0;

    temp_r0 = func_ov040_0209cb78();
    switch (temp_r0) {
        case 0:
        case 3:
            break;
        default:
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

PrcStepResult func_ov002_0208e6f8(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    if (menuObj->unk_474CA != func_ov040_0209cb68()) {
        PrcCtx_ReplaceFrame(&menuObj->mainCtx, &data_ov002_02092e18, NULL);
        return 0;
    }
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_REPEAT;
}

PrcStepResult func_ov002_0208e750(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    if (func_ov040_0209cb68() == 1) {
        PrcCtx_AdvanceStep(ctx);
        return PRC_STEP_CONTINUE;
    }
    u32 irqState2        = OS_DisableIRQ();
    *data_ov002_020935e0 = 3;
    OS_RestoreIRQ(irqState2);
    if (func_ov040_0209d48c(data_ov002_020935e0, 0xA, func_ov002_0208e070, menuObj) != 0) {
        return PRC_STEP_CONTINUE;
    }
    PrcCtx_ReplaceFrame(&menuObj->mainCtx, &data_ov002_02092e18, NULL);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_0208e7d8(PrcCtx* ctx, void* object) {
    u32 temp_r0;

    temp_r0 = func_ov040_0209cb78();
    switch (temp_r0) {
        case 0:
            PrcCtx_AdvanceStep(ctx);
            break;
        case 1:
            func_ov040_0209d6cc();
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

PrcStepResult func_ov002_0208e838(PrcCtx* ctx, void* object) {
    u32 temp_r0;

    temp_r0 = func_ov040_0209cb78();
    switch (temp_r0) {
        case 0:
        case 3:
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
