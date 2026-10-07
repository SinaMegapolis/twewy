#include "OtosuMenuShared.h"

static PrcStepFn data_ov002_02092f30[] = {func_ov002_0208b570, PrcStep_PopFrame};

static PrcStepFn data_ov002_02092f98[] = {
    func_ov002_02089bb0,
    func_ov002_0208aa4c,
    func_ov002_0208aabc,
    OtosuPrcStep_FadeStart_Neutral,
    OtosuPrcStep_FadeWait_Neutral,
    func_ov002_0208ab58,
    OtosuPrcStep_FadeStart_BrightImmediate,
    OtosuPrcStep_FadeWait_Immediate,
    func_ov002_0208af78,
    func_ov002_0208b0a4,
    func_ov002_0208b154,
    OtosuPrcStep_FadeStart_Neutral,
    OtosuPrcStep_FadeWait_Neutral,
    func_ov002_0208b240,
    func_ov002_0208b270,
    func_ov002_0208266c,
    func_ov002_02082698,
    func_ov002_0208b5c4,
    OtosuPrcStep_FadeStart_BrightImmediate,
    OtosuPrcStep_FadeWait_Immediate,
    func_ov002_0208a764,
    PrcStep_PopFrame,
};

static PrcStepFn data_ov002_02092f38[] = {func_ov002_0208b610, func_ov002_0208b6d0, PrcStep_PopFrame};

PrcFrameDesc data_ov002_02092f44 = {
    .enter     = func_ov002_02089b3c,
    .stepTable = data_ov002_02092f98,
    .update    = func_ov002_02089ba8,
    .render    = func_ov002_02089bac,
    .exit      = func_ov002_02089b80,
};

static PrcStepFn data_ov002_02092f58[] = {
    func_ov002_02089bb0,
    func_ov002_02089d40,
    func_ov002_02089e3c,
    func_ov002_0208a070,
    func_ov002_0208a050,
    OtosuPrcStep_FadeStart_Neutral,
    OtosuPrcStep_FadeWait_Neutral,
    func_ov002_0208a250,
    OtosuPrcStep_FadeStart_BrightImmediate,
    OtosuPrcStep_FadeWait_Immediate,
    func_ov002_0208266c,
    func_ov002_02082698,
    func_ov002_0208b670,
    func_ov002_0208b734,
    func_ov002_0208a748,
    PrcStep_PopFrame,
};

static const Ov002_U16_6 data_ov002_02092074 = {0x0002, 0x0002, 0x009D, 0x0002, 0x00A4, 0x009D}; /* const */
BOOL                     OtosuMenu_TitleSkipped;
static s16               data_ov002_02093664[0xE];
static const Ov002_U16_6 data_ov002_02092068 = {0x0002, 0x0002, 0x009D, 0x0002, 0x002A, 0x0054}; /* const */
PrcFrameDesc             data_ov002_02092ff0 = {
                .enter     = func_ov002_02089b3c,
                .stepTable = data_ov002_02092f58,
                .update    = func_ov002_02089ba8,
                .render    = func_ov002_02089bac,
                .exit      = func_ov002_02089b80,
};
static const OtosuMenuRectList2 data_ov002_02092080 = {
    {
     {0x0000, 0x0008, 0x00A0, 0x0058, 0x00B8},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF},
     }
};
static const OtosuMenuRectList5 data_ov002_0209212e = {
    {
     {0, 8, 0xA0, 0x58, 0xB8},
     {1, 0x48, 0x40, 0xB6, 0x58},
     {2, 0x48, 0x60, 0xB6, 0x78},
     {3, 0x48, 0x80, 0xB6, 0x98},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF},
     }
};
static const Ov002_U16_12       data_ov002_02092094 = {0x0002, 0x0002, 0x009D, 0x0004, 0x0040, 0x003E,
                                                       0x0004, 0x0040, 0x005E, 0x0004, 0x0040, 0x007E}; /* const */
static const OtosuMenuRectList3 data_ov002_020920ac = {
    {
     {0x23F4, 8, 8, 0xF8, 0x40},
     {0x23EB, 0x18, 0xA6, 0x58, 0xB6},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0},
     }
};
static const OtosuMenuRectList3 data_ov002_020920e8 = {
    {
     {0, 8, 0xA0, 0x58, 0xB8},
     {1, 0xA5, 0xA0, 0xF5, 0xB8},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF},
     }
};
static const OtosuMenuRectList4 data_ov002_02092106 = {
    {
     {0x23F6, 0x0008, 0x0008, 0x00F8, 0x0040},
     {0x23F7, 0x00C5, 0x00A6, 0x00F5, 0x00B6},
     {0x23EB, 0x0018, 0x00A6, 0x0058, 0x00B6},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000},
     }
};
static const OtosuMenuRectList3 data_ov002_020920ca = {
    {
     {0x23F5, 8, 8, 0xF8, 0x40},
     {0x23EB, 0x18, 0xA6, 0x58, 0xB6},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0},
     }
};

void func_ov002_02089b3c(PrcCtx* ctx, OtosuMenuObj* menuObj) {
    menuObj->cursor.posX = 0;
    menuObj->cursor.posY = 0xC8;
}

void func_ov002_02089b54(void) {
    g_DisplaySettings.engineState[0].window0              = 31;
    g_DisplaySettings.engineState[0].window0Effects       = TRUE;
    g_DisplaySettings.engineState[0].windowOutside        = 31;
    g_DisplaySettings.engineState[0].windowOutsideEffects = TRUE;
    Display_Commit();
}

void func_ov002_02089b80(void* arg0, OtosuMenuObj* menuObj) {
    func_ov002_02085710(menuObj);
    menuObj->cursor.posX = 0;
    menuObj->cursor.posY = 0xC8;
}

void func_ov002_02089ba8(void) {}

void func_ov002_02089bac(void) {}

PrcStepResult func_ov002_02089bb0(PrcCtx* ctx, void* unused) {
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

void func_ov002_02089bc0(void* arg0, void* arg1) {
    ((OtosuMenuObj*)arg1)->unk_41950 = 0;
}

s32 func_ov002_02089bd0(void) {
    return 0;
}

s32 func_ov002_02089bd8(void* arg0, OtosuMenuObj* menuObj) {
    typedef struct {
        u8  filler10[0x10];
        u16 unk10;
    } Ov002_89bd8Data;
    Ov002_89bd8Data* msg = (Ov002_89bd8Data*)arg0;
    u16              temp_r2;

    temp_r2 = msg->unk10;
    if ((u32)temp_r2 >= 4U) {
        return 0;
    }
    MI_CpuCopyU8((s32)((u8*)msg + 0x14), menuObj->players[temp_r2].name, 0x16);
    MI_CpuCopyU8((s32)((u8*)msg + 0x0A), menuObj->players[msg->unk10].bssid, 6);

    menuObj->playerMask = (u16)(menuObj->playerMask | (1 << msg->unk10));
    menuObj->unk_41836  = (u16)(menuObj->unk_41836 | (1 << msg->unk10));

    menuObj->unk_4195A[msg->unk10] = 0;
    return 1;
}

void func_ov002_02089c88(void* arg0, OtosuMenuObj* menuObj) {
    typedef struct {
        u8  filler10[0x10];
        u16 unk10;
    } Ov002_89bd8Data;
    Ov002_89bd8Data* msg = (Ov002_89bd8Data*)arg0;
    u16              temp_r3;

    temp_r3 = msg->unk10;
    if ((u32)temp_r3 >= 4U) {
        return;
    }
    menuObj->playerMask = (u16)(menuObj->playerMask & ~(1 << temp_r3));
    menuObj->unk_41836  = (u16)(menuObj->unk_41836 | (1 << msg->unk10));
}

void func_ov002_02089cd0(void* arg0, OtosuMenuObj* menuObj) {
    menuObj->unk_4198C = 0;
}

void func_ov002_02089ce0(s32 arg0, void* arg1, void* arg2, s32 arg3) {
    typedef struct {
        u16 unk0;
        u16 unk2;
    } Ov002_89ce0Data;
    Ov002_89ce0Data* msg = (Ov002_89ce0Data*)arg1;
    void*            temp_r5;

    if (arg1 == NULL) {
        return;
    }
    if (arg2 != (void*)0xA) {
        return;
    }
    if (arg3 == 0) {
        return;
    }
    temp_r5                         = (u8*)arg3 + (arg0 * 2);
    *(u16*)((u8*)temp_r5 + 0x4195A) = (u16)msg->unk0;
    *(u16*)((u8*)temp_r5 + 0x41962) = (u16)msg->unk2;
    MI_CpuCopyU8((s32)((u8*)msg + 4), (s16*)((arg0 * 6) + ((u8*)arg3 + 0x41FB6)), 6);
}

PrcStepResult func_ov002_02089d40(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    s32 temp_r0;

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
    temp_r0             = SysFont_GetOwnerName();
    MI_CpuCopyU8(temp_r0, (u8*)menuObj->unk_4196E, 0x16);
    MI_CpuCopyU8(temp_r0, menuObj->players[0].name, 0x16);
    Mem_Free(&gDebugHeap, temp_r0);
    func_0203a96c(menuObj->players[0].bssid);
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02089e3c(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    s32 temp_r4;
    s32 temp_r5;
    u32 temp_r0;

    temp_r0 = func_ov040_0209cb78();
    switch (temp_r0) {
        default:
            break;
        case 0:
            func_ov040_0209cf20();
            func_ov040_0209caac(0x400548);
            break;
        case 1:
            func_ov040_0209cb9c();
            break;
        case 7:
            temp_r5 = func_ov040_0209cde4();
            temp_r4 = func_020442f8();
            func_ov040_0209d818(3, 0, 0x78, 0xA, 1);
            func_ov040_0209d848(0);
            func_ov040_0209cb08((u8*)menuObj + 0x4196E, 0x1E);
            func_ov040_0209d40c(func_ov002_02089bd8, menuObj);
            func_ov040_0209d420(func_ov002_02089c88, menuObj);
            func_ov003_0209d434(func_ov002_02089ce0, menuObj);
            func_ov040_0209d0a8(0, temp_r4, temp_r5);
            break;
        case 4:
            PrcCtx_AdvanceStep(ctx);
            break;
    }
    return PRC_STEP_CONTINUE;
}

void func_ov002_02089f3c(OtosuMenuObj* menuObj, s32 arg1) {
    u8* pack;
    u8* base;
    u8* var_r0;
    u8* var_r0_2;

    pack = (u8*)menuObj->packs[0];
    if (arg1 != 0) {
        var_r0                                                = Data_GetPackEntryData((Data*)pack, 23);
        menuObj->mainScreens[1][0]                            = (void*)(var_r0 + 4);
        g_DisplaySettings.engineState[0].window0              = 0x1F;
        g_DisplaySettings.engineState[0].windowOutsideEffects = 1;
        g_DisplaySettings.engineState[0].windowOutside        = 0x1F;
        g_DisplaySettings.engineState[0].window0Effects       = 1;
    } else {
        var_r0_2                                              = Data_GetPackEntryData((Data*)pack, 24);
        menuObj->mainScreens[1][0]                            = (void*)(var_r0_2 + 4);
        g_DisplaySettings.engineState[0].window0Left          = 688128;
        g_DisplaySettings.engineState[0].window0Top           = 655360;
        g_DisplaySettings.engineState[0].window0Right         = 1015808;
        g_DisplaySettings.engineState[0].window0Bottom        = 753664;
        g_DisplaySettings.engineState[0].window0              = 0x1E;
        g_DisplaySettings.engineState[0].windowOutsideEffects = 1;
        g_DisplaySettings.engineState[0].windowOutside        = 0x1F;
        g_DisplaySettings.engineState[0].window0Effects       = 1;
        g_DisplaySettings.controls[0].windows |= 1;
    }
    Display_Commit();
    func_0200d1d8((u8*)menuObj->mainMaps[1].engineId, 0, 1, 0, (u8*)menuObj->mainScreens[1][0], 1, 1);
}

PrcStepResult func_ov002_0208a050(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    func_ov002_02082ab4(menuObj);
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_0208a070(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    OtosuMenuRectList4 table_sp0 = data_ov002_02092106;
    u16                i;

    func_ov002_02082f18(menuObj, 0x18, 0x16, table_sp0.rects);
    func_ov002_02089f3c(menuObj, 0);
    menuObj->unk_474C8 = 0xFFFF;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

void func_ov002_0208a0f8(OtosuMenuObj* menuObj) {
    u8  names[4][0x30];
    s32 i;
    u32 mask;
    u32 irq;
    u8* src;

    menuObj->unk_41990 = 1;
    menuObj->unk_41954 = 1;
    irq                = OS_DisableIRQ();
    mask               = menuObj->playerMask & menuObj->unk_41836;
    menuObj->unk_41836 = 0;
    src                = (u8*)menuObj->players;
    for (i = 0; i < 4; i++) {
        if (mask & (1 << i)) {
            MI_CpuCopyU8(src, names[i], 0x16);
        }
        src += 0x30;
    }
    OS_RestoreIRQ(irq);
    func_ov002_02082ab4(menuObj);
}

void func_ov002_0208a198(OtosuMenuObj* menuObj) {
    s32 var_ip;
    u16 temp_r0;

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
    if (func_ov040_0209d788(func_ov002_02089cd0, menuObj, (u8*)menuObj->unk_4196E, 0x1E, var_ip) == 0) {
        menuObj->unk_4198C = 0;
    }
    temp_r0 = menuObj->unk_4198A;
    func_ov002_02089f3c(menuObj, (temp_r0 > 1U) ? 1 : 0);
}

PrcStepResult func_ov002_0208a250(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    OtosuMenuRectList3 layout = data_ov002_020920e8;
    Ov002_U16_6        anims  = data_ov002_02092074;
    s32                sp4;
    s32                temp_r4;
    s32                temp_r6;
    s32                var_r0;
    s32                var_r2;
    s32                var_r5;
    s32                var_r9;
    s32                var_r9_2;
    const Ov002_U16_2* var_r3;
    Ov002_U16_2*       var_r4;
    u16                temp_r0;
    u16                temp_r1;
    u16                temp_r6_2;
    u16                temp_r8;
    u16                var_r4_2;
    void*              temp_r1_2;
    void*              temp_r7;
    void*              temp_r9;

    var_r5   = 0;
    var_r4_2 = func_ov002_0208597c(layout.rects);
    if ((var_r4_2 == 1) && (menuObj->unk_4198A == 1)) {
        var_r4_2 = 0xFFFF;
    }
    if (var_r4_2 == 0xFFFE) {
        var_r4_2             = 0xFFFF;
        menuObj->unk_474C8   = 0xFFFF;
        menuObj->cursor.posX = 0U;
        menuObj->cursor.posY = 0xC8U;
    }
    if ((menuObj->unk_474C8 == 1) && (menuObj->unk_4198A == 1)) {
        menuObj->unk_474C8   = 0xFFFF;
        menuObj->cursor.posX = 0U;
        menuObj->cursor.posY = 0xC8U;
    }
    if (var_r4_2 != 0xFFFF) {
        if (menuObj->unk_474C8 == var_r4_2) {
            switch (var_r4_2) { /* irregular */
                case 0:
                    SndMgr_StartPlayingSE(SEIDX_SE_BAYBADGE_MENU_CANCEL);
                    PrcCtx_ReplaceFrame(ctx, &OtosuMenu_RoleSelect_FrameDesc, NULL);
                    menuObj->linkLevelIcon.posX = 0;
                    menuObj->linkLevelIcon.posY = 0xC8;
                    PrcCtx_PushStepTable(ctx, data_ov002_02092f38);
                    PrcCtx_PushStepTable(ctx, PrcSteps_FadeBrightImmediate);
                    return 0;
                case 1:
                    temp_r4 = OS_DisableIRQ();
                    SndMgr_StartPlayingSE(SEIDX_SE_BAYBADGE_MENU_EXECUTE);
                    if (((u32)func_02047e84(menuObj->playerMask) >= 2U) && (menuObj->unk_41950 == 0)) {
                        var_r0 = 1;
                    loop_25:
                        if (var_r0 < 4) {
                            temp_r8 = menuObj->playerMask;
                            if (!(temp_r8 & (1 << var_r0)) ||
                                ((temp_r9 = (u8*)menuObj + (var_r0 * 2), (temp_r8 == ((u16*)temp_r9)[0x4195A / 2])) &&
                                 (menuObj->unk_41958 == ((u16*)temp_r9)[0x41962 / 2])))
                            {
                                var_r0 += 1;
                                goto loop_25;
                            }
                        }
                        if (var_r0 == 4) {
                            func_ov040_0209d40c(func_ov002_02089bd0, menuObj);
                            func_ov040_0209d420(NULL, menuObj);
                            menuObj->unk_4196A = 2U;
                            menuObj->unk_41994 = 0;
                            var_r5             = 1;
                            menuObj->unk_41990 = 1;
                        }
                    }
                    OS_RestoreIRQ(temp_r4);
                    goto block_33;
            }
        } else {
            menuObj->unk_474C8 = var_r4_2;
            SndMgr_StartPlayingSE(SEIDX_SE_BAYBADGE_MENU_CURSOR);
            if ((var_r4_2 != 0) && (var_r4_2 != 1)) {

            } else {
                temp_r1_2            = &anims.data[var_r4_2 * 3];
                menuObj->cursor.posX = ((u16*)temp_r1_2)[1];
                menuObj->cursor.posY = ((u16*)temp_r1_2)[2];
                Sprite_ChangeAnimation(&menuObj->cursor, menuObj->cursor.animData, (s16)((u16*)temp_r1_2)[0],
                                       menuObj->cursor.cellTable);
            }
            goto block_33;
        }
    } else {
    block_33:
        if (menuObj->unk_41836 != 0) {
            func_ov002_0208a0f8(menuObj);
        }
        if ((menuObj->unk_41990 != 0) && (menuObj->unk_4198C == 0)) {
            func_ov002_0208a198(menuObj);
        }
        if ((menuObj->playerMask != 1) && (menuObj->unk_41950 == 0)) {
            if (menuObj->unk_41954 != 0) {
                menuObj->unk_41954 = 0;
                var_r5             = 1;
                menuObj->unk_41958 = (u16)(menuObj->unk_41958 + 1);
            } else {
                var_r9 = 1;
            loop_49:
                if (var_r9 < 4) {
                    temp_r6_2 = menuObj->playerMask;
                    if (temp_r6_2 & (1 << var_r9)) {
                        temp_r7 = (u8*)menuObj + (var_r9 * 2);
                        if (temp_r6_2 != ((u16*)temp_r7)[0x4195A / 2]) {
                            var_r5 = 1;
                        } else if (menuObj->unk_41958 != ((u16*)temp_r7)[0x41962 / 2]) {
                            var_r5 = 1;
                        } else {
                            goto block_48;
                        }
                    } else {
                    block_48:
                        var_r9 += 1;
                        goto loop_49;
                    }
                }
            }
        }
        if (var_r5 != 0) {
            sp4                    = OS_DisableIRQ();
            data_ov002_020935e0[0] = menuObj->unk_4196A;
            if (menuObj->unk_4196A == 0) {
                var_r9_2                  = 0;
                data_ov002_020935e0[1]    = menuObj->playerMask;
                data_ov002_020935e0[0x3A] = menuObj->unk_41958;
            loop_56:
                if (var_r9_2 < 4) {
                    if (menuObj->playerMask & (1 << var_r9_2)) {
                        MI_CpuCopyU8(menuObj->players[var_r9_2].name, (s16*)((var_r9_2 * 0x16) + (s32)&data_ov002_020935e0[2]),
                                     0x16);
                        MI_CpuCopyU8(menuObj->players[var_r9_2].bssid,
                                     (s16*)((var_r9_2 * 6) + (s32)&data_ov002_020935e0[0x2E]), 6);
                    }
                    var_r9_2 += 1;
                    goto loop_56;
                }
            }
            OS_RestoreIRQ(sp4);
            menuObj->unk_41950 = 1;
            if (func_ov040_0209d48c(data_ov002_020935e0, 0x78, func_ov002_02089bc0, menuObj) == 0) {
                menuObj->unk_41950 = 0;
            }
        }
        if (menuObj->unk_4196A == 2) {
            menuObj->unk_41FD0 = (s32)func_02047e84(menuObj->playerMask);
            PrcCtx_AdvanceStep(ctx);
        }
        return 0;
    }
}

PrcStepResult func_ov002_0208a748(PrcCtx* ctx, void* object) {
    PrcCtx_ReplaceFrame(ctx, &data_ov002_02092cf4, NULL);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_0208a764(PrcCtx* ctx, void* object) {
    PrcCtx_ReplaceFrame(ctx, &data_ov002_02092ce0, NULL);
    return PRC_STEP_CONTINUE;
}

void func_ov002_0208a780(OtosuMenuObj* menuObj) {
    s32 temp_r8;
    u16 var_r5;
    u16 var_r9;
    u32 temp_r1;

    MI_CpuFillU16(0, (s32)menuObj->unk_41FF4, 0x3000);
    MI_CpuFillU16(0, (s32)data_ov002_02093664, 0x16);
    var_r9 = 0;
    do {
        if (!(menuObj->unk_41998 & (1 << var_r9))) {
            *data_ov002_02093664 = 0xFFFF;
        } else {
            MI_CpuCopyU8((var_r9 * 0x1E) + (s32)((u8*)menuObj->unk_41E38), data_ov002_02093664, 0x16);
        }
        SysFont_SetMsgPtr(&menuObj->font, data_ov002_02093664);
        SysFont_SetPos(&menuObj->font, 0, 5);
        SysFont_SetHAlign(&menuObj->font, 0, 256);
        SysFont_SetVAlign(&menuObj->font, 3, 32);
        SysFont_DrawCurrentToChar(&menuObj->font, menuObj->unk_41FF4 + (var_r9 << 12), 32, 4);
        var_r9 += 1;
    } while ((u32)var_r9 < 3U);
    var_r5 = 0;
    do {
        temp_r1 = ((var_r5 << 5) + 0x48) << 8;
        temp_r8 = G2_GetBG0CharPtr() + ((s32)(temp_r1 + (temp_r1 >> 0x1F)) >> 1);
        DC_PurgeAll();
        func_0203abec(3, (s32)menuObj->unk_41FF4 + (var_r5 << 0xC), temp_r8, 0x1000);
        var_r5 += 1;
    } while ((u32)var_r5 < 3U);
}

void func_ov002_0208a8d4(void* arg0, OtosuMenuObj* menuObj) {
    s32 temp_r0;
    s32 temp_r1;
    s32 temp_r2;
    s32 temp_r3;
    s32 temp_r8_2;
    s32 var_r4;
    s32 var_r4_2;
    s32 var_r5;
    u16 temp_r8;
    u8* entry = (u8*)arg0;
    u8* slot;
    u8* var_ip;

    var_r5 = 0;
    if (*(u32*)(entry + 0x3C) == 0) {
        return;
    }
    if (*(u8*)(entry + 0x4A) != 0x70) {
        return;
    }
    var_r4 = 5;
loop_5:
    slot = (u8*)menuObj->unk_419A0 + (var_r4 * 0xC4);
    if (menuObj->unk_41998 & (1 << var_r4)) {
        if ((*(u8*)(slot + 0x4) == *(u8*)(entry + 0x4)) && (*(u8*)(slot + 0x5) == *(u8*)(entry + 0x5)) &&
            (*(u8*)(slot + 0x6) == *(u8*)(entry + 0x6)) && (*(u8*)(slot + 0x7) == *(u8*)(entry + 0x7)) &&
            (*(u8*)(slot + 0x8) == *(u8*)(entry + 0x8)) && (*(u8*)(slot + 0x9) == *(u8*)(entry + 0x9)))
        {
            if (!(*(u8*)(entry + 0x4B) & 1)) {
                menuObj->unk_41998 = (u16)(menuObj->unk_41998 & ~(1 << var_r4));
                menuObj->unk_4199C = (u16)(menuObj->unk_4199C | (1 << var_r4));
                return;
            }
            goto block_17;
        }
        goto block_17;
    }
    var_r5 = var_r4;
block_17:
    var_r4 -= 1;
    if (var_r4 < 0) {
        if (!(*(u8*)(entry + 0x4B) & 1)) {
            return;
        }
        temp_r8 = func_02047e84();
        if ((u32)func_02047e84(menuObj->unk_41998) >= 6U) {
            return;
        }
        menuObj->unk_4199A = temp_r8;
        if (var_r4 >= 0) {
            var_r5 = var_r4;
        }
        temp_r8_2 = var_r5 * 0xC4;
        var_ip    = (u8*)menuObj + temp_r8_2 + 0x419A0;
        var_r4_2  = 0xC;
        do {
            temp_r0 = *(s32*)(entry + 0x0);
            temp_r1 = *(s32*)(entry + 0x4);
            temp_r2 = *(s32*)(entry + 0x8);
            temp_r3 = *(s32*)(entry + 0xC);
            entry += 0x10;
            *(s32*)(var_ip + 0x0) = temp_r0;
            *(s32*)(var_ip + 0x4) = temp_r1;
            *(s32*)(var_ip + 0x8) = temp_r2;
            *(s32*)(var_ip + 0xC) = temp_r3;
            var_ip += 0x10;
            var_r4_2 -= 1;
        } while (var_r4_2 != 0);
        *(u16*)((u8*)menuObj + temp_r8_2 + 0x41A60) = 0xB4;
        menuObj->unk_41998                          = (u16)(menuObj->unk_41998 | (1 << var_r5));
        menuObj->unk_4199C                          = (u16)(menuObj->unk_4199C | (1 << var_r5));
        return;
    }
    goto loop_5;
}

PrcStepResult func_ov002_0208aa4c(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    switch (func_ov040_0209cb78()) {
        case 0:
            func_ov040_0209cf20();
            func_ov040_0209caac(0x400548);
            break;
        case 1:
            func_ov040_0209ba04(func_ov002_0208a8d4, menuObj, &data_ov002_02092160, 0);
            break;
        case 2:
            PrcCtx_AdvanceStep(ctx);
            break;
    }
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_0208aabc(PrcCtx* ctx, void* object) {
    OtosuMenuObj*      menuObj = (OtosuMenuObj*)object;
    OtosuMenuRectList3 layout  = data_ov002_020920ac;

    func_ov002_02082f18(menuObj, 0x1A, 0x19, layout.rects);
    menuObj->unk_474C8 = 0xFFFF;
    menuObj->unk_41EEC = 0xFFFF;
    menuObj->unk_41998 = 0;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_0208ab58(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    OtosuMenuRectList5 table_sp18 = data_ov002_0209212e;
    Ov002_U16_12       table_anim = data_ov002_02092094;
    s32                temp_r0_4;
    s32                temp_r1_3;
    s32                temp_r2;
    s32                temp_r3;
    s32                temp_r5;
    s32                temp_r5_2;
    s32                temp_r6;
    s32                temp_r7;
    s32                temp_r9;
    s32                var_r1;
    s32                var_r2;
    s32                var_r2_2;
    s32                var_r2_4;
    s32                var_r5_2;
    u16*               var_r3_3;
    u16                temp_r0_3;
    u16                temp_r7_2;
    void*              var_r2_3;
    void*              var_r5_3;
    void*              var_r7_2;
    void*              var_r8;
    u16                i;

    temp_r0_3 = func_ov002_0208597c(table_sp18.rects);
    if (temp_r0_3 == 0xFFFE) {
        menuObj->unk_474C8   = 0xFFFF;
        menuObj->cursor.posX = 0U;
        menuObj->cursor.posY = 0xC8U;
        return 0;
    }
    if ((temp_r0_3 != 0xFFFF) && (temp_r0_3 != 0)) {
        if ((menuObj->unk_41998 & (1 << (temp_r0_3 - 1))) == 0) {
            temp_r0_3 = 0xFFFF;
        }
    }
    if (temp_r0_3 != 0xFFFF) {
        if (menuObj->unk_474C8 == temp_r0_3) {
            switch (temp_r0_3) { /* switch 2 */
                default:         /* switch 2 */
                    goto block_36;
                case 0:          /* switch 2 */
                    SndMgr_StartPlayingSE(SEIDX_SE_BAYBADGE_MENU_CANCEL);
                    PrcCtx_ReplaceFrame(ctx, &OtosuMenu_RoleSelect_FrameDesc, NULL);
                    menuObj->linkLevelIcon.posX = 0;
                    menuObj->linkLevelIcon.posY = 0xC8;
                    PrcCtx_PushStepTable(ctx, data_ov002_02092f30);
                    PrcCtx_PushStepTable(ctx, PrcSteps_FadeBrightImmediate);
                    return PRC_STEP_CONTINUE;
                case 1: /* switch 2 */
                case 2: /* switch 2 */
                case 3: /* switch 2 */
                    SndMgr_StartPlayingSE(SEIDX_SE_BAYBADGE_MENU_EXECUTE);
                    if (menuObj->unk_41998 & (1 << menuObj->unk_41EEC)) {
                        temp_r9  = OS_DisableIRQ();
                        var_r8   = (u8*)menuObj->unk_419A0 + (menuObj->unk_41EEC * 0xC4);
                        var_r7_2 = (u8*)menuObj->unk_41EF0;
                        var_r5_2 = 0xC;
                        do {
                            temp_r0_4               = *(s32*)(var_r8 + 0x0);
                            temp_r1_3               = *(s32*)(var_r8 + 0x4);
                            temp_r2                 = *(s32*)(var_r8 + 0x8);
                            temp_r3                 = *(s32*)(var_r8 + 0xC);
                            var_r8                  = (u8*)var_r8 + 0x10;
                            *(s32*)(var_r7_2 + 0x0) = temp_r0_4;
                            *(s32*)(var_r7_2 + 0x4) = temp_r1_3;
                            *(s32*)(var_r7_2 + 0x8) = temp_r2;
                            *(s32*)(var_r7_2 + 0xC) = temp_r3;
                            var_r7_2                = (u8*)var_r7_2 + 0x10;
                            var_r5_2 -= 1;
                        } while (var_r5_2 != 0);
                        OS_RestoreIRQ(temp_r9);
                        menuObj->unk_41810 = 1;
                        PrcCtx_AdvanceStep(ctx);
                    }
                    goto block_36;
            }
        } else {
            SndMgr_StartPlayingSE(SEIDX_SE_BAYBADGE_MENU_CURSOR);
            menuObj->unk_474C8 = temp_r0_3;
            if (temp_r0_3 != 0) {
                menuObj->unk_41EEC = (u16)(temp_r0_3 - 1);
            }
            switch (temp_r0_3) { /* switch 3 */
                default:         /* switch 3 */
                    break;
                case 0:          /* switch 3 */
                case 1:          /* switch 3 */
                case 2:          /* switch 3 */
                case 3:          /* switch 3 */
                    temp_r6              = temp_r0_3 * 3;
                    menuObj->cursor.posX = table_anim.data[temp_r6 + 1];
                    menuObj->cursor.posY = table_anim.data[temp_r6 + 2];
                    Sprite_ChangeAnimation(&menuObj->cursor, menuObj->cursor.animData, (s16)table_anim.data[temp_r6],
                                           menuObj->cursor.cellTable);
                    break;
            }
            goto block_36;
        }
    } else {
        goto block_36;
    }
block_36:
    temp_r5  = OS_DisableIRQ();
    var_r2_3 = menuObj;
    var_r1   = 0;
    do {
        if (menuObj->unk_41998 & (1 << var_r1)) {
            temp_r7                          = *(s16*)((u8*)var_r2_3 + 0x41A60) - 1;
            *(s16*)((u8*)var_r2_3 + 0x41A60) = (s16)temp_r7;
            if (temp_r7 <= 0) {
                menuObj->unk_41998 = (u16)(menuObj->unk_41998 & ~(1 << var_r1));
                menuObj->unk_4199C = (u16)(menuObj->unk_4199C | (1 << var_r1));
            }
        }
        var_r1 += 1;
        var_r2_3 = (u8*)var_r2_3 + 0xC4;
    } while (var_r1 < 6);
    if (menuObj->unk_4199C != 0) {
        menuObj->unk_4199A = func_02047e84(menuObj->unk_41998);
    }
    OS_RestoreIRQ(temp_r5);
    if (menuObj->unk_4199C != 0) {
        u32 irqState       = OS_DisableIRQ();
        temp_r7_2          = menuObj->unk_4199C;
        var_r2_4           = 0;
        menuObj->unk_4199C = 0U;
        var_r3_3           = menuObj->unk_419F0;
        var_r5_3           = menuObj;
        do {
            if (menuObj->unk_41998 & temp_r7_2 & (1 << var_r2_4)) {
                *(Ov002_U16_15*)((u8*)var_r5_3 + 0x41E38) = *(Ov002_U16_15*)var_r3_3;
            }
            var_r2_4 += 1;
            var_r3_3 = (u16*)((u8*)var_r3_3 + 0xC4);
            var_r5_3 = (u8*)var_r5_3 + 0x1E;
        } while (var_r2_4 < 6);
        OS_RestoreIRQ(irqState);
        temp_r5_2 = menuObj->unk_41EEC;
        if ((temp_r5_2 != 0xFFFF) && !(menuObj->unk_41998 & (1 << temp_r5_2))) {
            menuObj->unk_41EEC   = 0xFFFF;
            menuObj->unk_474C8   = 0xFFFF;
            menuObj->cursor.posX = 0U;
            menuObj->cursor.posY = 0xC8U;
        }
        func_ov002_0208a780(menuObj);
    }
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_0208af78(PrcCtx* ctx, void* object) {
    u32 temp_r0;

    temp_r0 = func_ov040_0209cb78();
    switch (temp_r0) { /* irregular */
        case 0:
            PrcCtx_AdvanceStep(ctx);
            break;
        case 1:
            PrcCtx_AdvanceStep(ctx);
            break;
        case 2:
            func_ov040_0209c158();
            break;
    }
    return PRC_STEP_CONTINUE;
}

void func_ov002_0208afc4(void* arg1, void* arg2, OtosuMenuObj* menuObj) {
    typedef struct {
        u16 unk0;
        u16 unk2;
        u8  filler04[0x70];
        u16 unk74;
        u16 unk76;
    } Ov002_8afc4Data;
    Ov002_8afc4Data* msg = (Ov002_8afc4Data*)arg1;
    u8*              var_r5;
    u8*              var_r7;
    s32              var_r4;
    u16              temp_r0;
    void*            var_r6;
    void*            var_r8;

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
    temp_r0            = msg->unk0;
    switch (temp_r0) { /* irregular */
        case 2:
        case 3:
            return;
        case 0:
            var_r6 = (u8*)msg + 4;
            var_r5 = (u8*)menuObj->players[0].name;
            var_r7 = menuObj->players[0].bssid;
            var_r8 = (u8*)msg + 0x5C;
            var_r4 = 0;
            do {
                if (msg->unk2 & (1 << var_r4)) {
                    MI_CpuCopyU8((s32)var_r6, var_r5, 0x16);
                    MI_CpuCopyU8((s32)var_r8, var_r7, 6);
                }
                var_r4 += 1;
                var_r5 += 0x30;
                var_r6 += 0x16;
                var_r7 += 0x30;
                var_r8 += 6;
            } while (var_r4 < 4);
            menuObj->playerMask = msg->unk2;
            menuObj->unk_41836  = 0xF;
            menuObj->unk_41958  = msg->unk74;
            return;
    }
}

PrcStepResult func_ov002_0208b0a4(PrcCtx* ctx, void* object) {
    OtosuMenuObj*      menuObj = (OtosuMenuObj*)object;
    OtosuMenuRectList3 layout  = data_ov002_020920ca;

    s32 temp_r0_2;
    menuObj->cursor.posX = 0;
    menuObj->cursor.posY = 0xC8;
    func_ov002_02082f18(object, 0x18, 0x16, layout.rects);
    temp_r0_2 = SysFont_GetOwnerName();
    MI_CpuCopyU8(temp_r0_2, object + 0x4181C, 0x16);
    Mem_Free(&gDebugHeap, temp_r0_2);
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_0208b154(PrcCtx* ctx, void* object) {
    u32 temp_r0;

    temp_r0 = func_ov040_0209cb78();
    switch (temp_r0) { /* irregular */
        case 0:
            break;
        case 1:
            MI_CpuCopyU8(object + 0x41EF4, object + 0x41FB0, 6);
            MI_CpuCopyU8(object + 0x41EF4, object + 0x41862, 6);
            func_ov040_0209d818(3, 0, 0x78, 0xA, 1);
            func_ov040_0209d848(0);
            func_ov040_0209cabc(object + 0x4181C, 0x18);
            func_ov003_0209d434((void (*)(s32, void*, void*, s32))func_ov002_0208afc4, object);
            func_ov040_0209d290(1, object + 0x41EF0);
            break;
        case 4:
            PrcCtx_AdvanceStep(ctx);
            break;
    }
    return PRC_STEP_CONTINUE;
}

void func_ov002_0208b230(void* arg0, OtosuMenuObj* menuObj) {
    menuObj->unk_41950 = 0;
}

PrcStepResult func_ov002_0208b240(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    menuObj->unk_474C8 = 0xFFFF;
    menuObj->unk_41950 = 0;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_REPEAT;
}

PrcStepResult func_ov002_0208b270(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    u8                 sp1E[0xC0];
    u16                spA;
    u16                sp8;
    u16                sp6;
    u16                sp4;
    s32                sp0;
    s32                temp_r1_2;
    s32                temp_r5;
    s32                var_r2;
    s32                var_r8;
    u16                temp_r0_3;
    u16                temp_r4;
    u16                temp_r5_2;
    u16                var_r4_2;
    OtosuMenuRectList2 table_spA = data_ov002_02092080;
    u16                i;

    sp4      = data_ov002_02092068.unk0;
    sp6      = data_ov002_02092068.unk2;
    sp8      = data_ov002_02092068.unk4;
    var_r4_2 = func_ov002_0208597c(table_spA.rects);
    if (var_r4_2 == 0xFFFE) {
        var_r4_2             = 0xFFFF;
        menuObj->unk_474C8   = 0xFFFF;
        menuObj->cursor.posX = 0U;
        menuObj->cursor.posY = 0xC8U;
    }
    if (var_r4_2 != 0xFFFF) {
        if (menuObj->unk_474C8 == var_r4_2) {
            if (var_r4_2 == 0) {
                SndMgr_StartPlayingSE(SEIDX_SE_BAYBADGE_MENU_CANCEL);
                PrcCtx_ReplaceFrame(ctx, &data_ov002_02092f44, NULL);
                PrcCtx_PushStepTable(ctx, data_ov002_02092f30);
                PrcCtx_PushStepTable(ctx, PrcSteps_FadeBrightImmediate);
                return PRC_STEP_CONTINUE;
            }
            goto block_10;
        }
        menuObj->unk_474C8 = var_r4_2;
        if (var_r4_2 == 0) {
            SndMgr_StartPlayingSE(SEIDX_SE_BAYBADGE_MENU_CANCEL);
            temp_r5 = var_r4_2 * 6;
            (void)temp_r5;
            menuObj->cursor.posX = sp6;
            menuObj->cursor.posY = sp8;
            Sprite_ChangeAnimation(&menuObj->cursor, menuObj->cursor.animData, (s16)sp4, menuObj->cursor.cellTable);
        }
        goto block_10;
    }
block_10:
    if (menuObj->unk_41836 != 0) {
        sp0                = OS_DisableIRQ();
        temp_r4            = menuObj->playerMask;
        temp_r5_2          = menuObj->unk_41836;
        menuObj->unk_4198A = func_02047e84(temp_r4);
        var_r8             = 0;
        menuObj->unk_41836 = 0U;
    loop_15:
        if (var_r8 < 4) {
            if (temp_r4 & temp_r5_2 & (1 << var_r8)) {
                temp_r1_2 = var_r8 * 0x30;
                MI_CpuCopyU8((u8*)menuObj->players + temp_r1_2, sp1E + temp_r1_2, 0x16);
            }
            var_r8 += 1;
            goto loop_15;
        }
        OS_RestoreIRQ(sp0);
        func_ov002_02082ab4(menuObj);
    }
    if (menuObj->unk_41950 == 0) {
        u32 irqState1          = OS_DisableIRQ();
        data_ov002_020935c0[0] = menuObj->playerMask;
        data_ov002_020935c0[1] = menuObj->unk_41958;
        OS_RestoreIRQ(irqState1);
        func_0203a96c((u8*)&data_ov002_020935c0[2]);
        menuObj->unk_41950 = 1;
        if (func_ov040_0209d48c(data_ov002_020935c0, 0xA, func_ov002_0208b230, menuObj) == 0) {
            menuObj->unk_41950 = 0;
        }
    }
    temp_r0_3 = menuObj->unk_4196A;
    if (temp_r0_3 != 2) {
        if (temp_r0_3 == 3) {
            PrcCtx_ReplaceFrame(ctx, &OtosuMenu_RoleSelect_FrameDesc, NULL);
            PrcCtx_PushStepTable(ctx, data_ov002_02092ec8);
            PrcCtx_PushStepTable(ctx, data_ov002_02092f30);
        }
    } else {
        SndMgr_StartPlayingSE(SEIDX_SE_BAYBADGE_MENU_EXECUTE);
        menuObj->unk_41FD0 = (s32)func_02047e84(menuObj->playerMask);
        PrcCtx_AdvanceStep(ctx);
    }
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_0208b570(PrcCtx* ctx, void* unused) {
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
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_0208b5c4(PrcCtx* ctx, void* unused) {
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

PrcStepResult func_ov002_0208b610(PrcCtx* ctx, void* arg1) {
    if (func_ov040_0209cb68() == 1) {
        PrcCtx_AdvanceStep(ctx);
        return PRC_STEP_CONTINUE;
    }
    u32 irqState2        = OS_DisableIRQ();
    *data_ov002_020935e0 = 3;
    OS_RestoreIRQ(irqState2);
    func_ov040_0209d48c(data_ov002_020935e0, 0x78, func_ov002_02089bc0, arg1);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_0208b670(PrcCtx* ctx, void* arg1) {
    if (func_ov040_0209cb68() == 1) {
        PrcCtx_AdvanceStep(ctx);
        return PRC_STEP_CONTINUE;
    }
    u32 irqState3        = OS_DisableIRQ();
    *data_ov002_020935e0 = 2;
    OS_RestoreIRQ(irqState3);
    func_ov040_0209d48c(data_ov002_020935e0, 0x78, func_ov002_02089bc0, arg1);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_0208b6d0(PrcCtx* ctx, void* arg0) {
    u32 temp_r0;

    temp_r0 = func_ov040_0209cb78();
    switch (temp_r0) {
        default:
        case 2:
            func_ov040_0209d588();
            break;
        case 0:
            func_ov002_02089b54();
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
    }
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_0208b734(PrcCtx* ctx, void* unused) {
    u32 temp_r0;

    temp_r0 = func_ov040_0209cb78();
    switch (temp_r0) {
        case 0:
        case 3:
            break;
        case 1:
            func_ov002_02089b54();
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

void func_ov002_0208b790(void) {
    if (SystemStatusFlags.vblank) {
        Display_Commit();
        DMA_Flush();
        OamMgr_Commit();
        DC_PurgeRange(&data_02066aec, 0x400);
        GX_LoadBgPltt(&data_02066aec, 0, 0x200);
        GX_LoadObjPltt(&data_02066cec, 0, 0x200);
        DC_PurgeRange(&data_02066eec, 0x400);
        GXs_LoadBgPltt(&data_02066eec, 0, 0x200);
        GXs_LoadObjPltt(&data_020670ec, 0, 0x200);
    }
}

void func_ov002_0208b860(void) {
    Interrupts_ForceVBlank();
    g_DisplaySettings.controls[0].brightness = 16;
    g_DisplaySettings.controls[1].brightness = 16;
    Interrupts_Init();
    HBlank_Init();
    GX_Init();
    DMA_Init(0x100);
    Display_Init();
    GX_DisableBankForLcdc();
    GX_SetBankForLcdc(0x1FF);
    GX_SetBankForBg(1);
    GX_SetBankForObj(2);
    GX_SetBankForSubBg(4);
    GX_SetBankForSubObj(8);
    MI_CpuFill(0, (void*)0x06800000, 0xA4000);
    MI_CpuFill(0, (void*)0x06000000, 0x80000);
    Interrupts_ForceVBlank();
    MI_CpuFill(0, (void*)0x06200000, 0x20000);
    MI_CpuFill(0, (void*)0x06400000, 0x40000);
    MI_CpuFill(0, (void*)0x06600000, 0x20000);
    Interrupts_ForceVBlank();
    g_DisplaySettings.controls[0].objTileMode = GX_OBJTILEMODE_1D_128K;
    g_DisplaySettings.controls[0].objBmpMode  = GX_OBJBMPMODE_1D_128K;
    g_DisplaySettings.controls[1].objTileMode = GX_OBJTILEMODE_1D_128K;
    g_DisplaySettings.controls[1].objBmpMode  = GX_OBJBMPMODE_1D_128K;
    g_DisplaySettings.unk_000                 = 0;
    REG_POWER_CNT &= ~0x8000;
    g_DisplaySettings.controls[0].dispMode  = GX_DISPMODE_GRAPHICS;
    g_DisplaySettings.controls[0].bgMode    = GX_BGMODE_0;
    g_DisplaySettings.controls[0].dimension = GX2D3D_MODE_2D;
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX2D3D_MODE_2D);

    Display_InitMainBG0(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_256x256, GX_BG_COLORS_16, 0, 0, 6, 0x4018);
    Display_InitMainBG1(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_256x256, GX_BG_COLORS_16, 2, 0, 5, 0x4214);
    Display_InitMainBG2(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_256x256, GX_BG_COLORS_16, 4, 3, 1, 0x440c);
    Display_InitMainBG3(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_256x256, GX_BG_COLORS_16, 6, 1, 1, 0x4604);

    g_DisplaySettings.engineState[0].bgSettings[0].priority = 0;
    g_DisplaySettings.engineState[0].bgSettings[1].priority = 1;
    g_DisplaySettings.engineState[0].bgSettings[2].priority = 2;
    g_DisplaySettings.engineState[0].bgSettings[3].priority = 3;

    g_DisplaySettings.engineState[0].bgSettings[0].mosaic = 0;
    g_DisplaySettings.engineState[0].bgSettings[1].mosaic = 0;
    g_DisplaySettings.engineState[0].bgSettings[2].mosaic = 0;
    g_DisplaySettings.engineState[0].bgSettings[3].mosaic = 0;

    Display_SetMainLayers(LAYER_BG0 | LAYER_BG1 | LAYER_BG2 | LAYER_BG3 | LAYER_OBJ);

    g_DisplaySettings.controls[1].bgMode = GX_BGMODE_0;
    GXs_SetGraphicsMode(0);

    Display_InitSubBG0(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_512x256, GX_BG_COLORS_16, 0, 7, 0, 0x401C);
    Display_InitSubBG1(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_512x256, GX_BG_COLORS_16, 2, 5, 0, 0x4214);
    Display_InitSubBG2(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_512x256, GX_BG_COLORS_16, 4, 3, 1, 0x440c);
    Display_InitSubBG3(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_512x256, GX_BG_COLORS_16, 6, 1, 1, 0x4604);

    g_DisplaySettings.engineState[1].bgSettings[0].priority = 0;
    g_DisplaySettings.engineState[1].bgSettings[1].priority = 1;
    g_DisplaySettings.engineState[1].bgSettings[2].priority = 2;
    g_DisplaySettings.engineState[1].bgSettings[3].priority = 3;

    g_DisplaySettings.engineState[1].bgSettings[0].mosaic = 0;
    g_DisplaySettings.engineState[1].bgSettings[1].mosaic = 0;
    g_DisplaySettings.engineState[1].bgSettings[2].mosaic = 0;
    g_DisplaySettings.engineState[1].bgSettings[3].mosaic = 0;

    Display_SetSubLayers(LAYER_BG0 | LAYER_BG1 | LAYER_BG2 | LAYER_BG3 | LAYER_OBJ);

    g_DisplaySettings.engineState[0].blendMode   = 1;
    g_DisplaySettings.engineState[0].blendLayer0 = 1;
    g_DisplaySettings.engineState[0].blendLayer1 = 62;
    g_DisplaySettings.engineState[0].blendCoeff0 = 6;
    g_DisplaySettings.engineState[0].blendCoeff1 = 10;
    g_DisplaySettings.engineState[1].blendMode   = 1;
    g_DisplaySettings.engineState[1].blendLayer0 = 1;
    g_DisplaySettings.engineState[1].blendLayer1 = 62;
    g_DisplaySettings.engineState[1].blendCoeff0 = 6;
    g_DisplaySettings.engineState[1].blendCoeff1 = 10;
    g_DisplaySettings.controls[0].windows |= 4;
    g_DisplaySettings.engineState[0].windowOutside        = 31;
    g_DisplaySettings.engineState[0].windowOutsideEffects = FALSE;
    g_DisplaySettings.controls[1].windows |= 4;
    g_DisplaySettings.engineState[1].windowOutside        = 31;
    g_DisplaySettings.engineState[1].windowOutsideEffects = FALSE;
    Interrupts_RegisterVBlankCallback(func_ov002_0208b790, TRUE);
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
    Display_Commit();
    OamMgr_Init();
}

void func_ov002_0208bd40(void) {
    Interrupts_ForceVBlank();
    Interrupts_Init();
    HBlank_Init();
    GX_Init();
    DMA_Init(0x100);
    Display_Init();
    g_DisplaySettings.engineState[0].window0              = 31;
    g_DisplaySettings.engineState[0].window0Effects       = TRUE;
    g_DisplaySettings.engineState[0].windowOutside        = 31;
    g_DisplaySettings.engineState[0].windowOutsideEffects = TRUE;
    GX_DisableBankForLcdc();
    GX_SetBankForLcdc(0x1FF);
    GX_SetBankForBg(1);
    GX_SetBankForObj(2);
    GX_SetBankForSubBg(4);
    GX_SetBankForSubObj(8);
    MI_CpuFill(0, (void*)0x06800000, 0xA4000);
    MI_CpuFill(0, (void*)0x06000000, 0x80000);
    Interrupts_ForceVBlank();
    MI_CpuFill(0, (void*)0x06200000, 0x20000);
    MI_CpuFill(0, (void*)0x06400000, 0x40000);
    MI_CpuFill(0, (void*)0x06600000, 0x20000);
    Interrupts_ForceVBlank();
    g_DisplaySettings.controls[0].objTileMode = GX_OBJTILEMODE_1D_128K;
    g_DisplaySettings.controls[0].objBmpMode  = GX_OBJBMPMODE_1D_128K;
    g_DisplaySettings.controls[1].objTileMode = GX_OBJTILEMODE_1D_128K;
    g_DisplaySettings.controls[1].objBmpMode  = GX_OBJBMPMODE_1D_128K;
    g_DisplaySettings.unk_000                 = 0;
    REG_POWER_CNT &= ~0x8000;
    g_DisplaySettings.controls[0].dispMode  = GX_DISPMODE_GRAPHICS;
    g_DisplaySettings.controls[0].bgMode    = GX_BGMODE_0;
    g_DisplaySettings.controls[0].dimension = GX2D3D_MODE_2D;
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX2D3D_MODE_2D);
    g_DisplaySettings.engineState[0].bgSettings[0].priority = 0;
    g_DisplaySettings.engineState[0].bgSettings[1].priority = 1;
    g_DisplaySettings.engineState[0].bgSettings[2].priority = 2;
    g_DisplaySettings.engineState[0].bgSettings[3].priority = 3;
    g_DisplaySettings.engineState[0].bgSettings[0].mosaic   = 0;
    g_DisplaySettings.engineState[0].bgSettings[1].mosaic   = 0;
    g_DisplaySettings.engineState[0].bgSettings[2].mosaic   = 0;
    g_DisplaySettings.engineState[0].bgSettings[3].mosaic   = 0;
    Display_SetMainLayers(LAYER_BG0 | LAYER_BG1 | LAYER_BG2 | LAYER_BG3 | LAYER_OBJ);
    g_DisplaySettings.controls[1].bgMode = GX_BGMODE_0;
    GXs_SetGraphicsMode(0);

    Display_InitSubBG0(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_512x256, GX_BG_COLORS_16, 0, 7, 0, 0x401C);
    Display_InitSubBG1(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_512x256, GX_BG_COLORS_16, 2, 5, 0, 0x4214);
    Display_InitSubBG2(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_512x256, GX_BG_COLORS_16, 4, 3, 1, 0x440C);
    Display_InitSubBG3(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_512x256, GX_BG_COLORS_16, 6, 1, 1, 0x4604);

    g_DisplaySettings.engineState[1].bgSettings[0].priority = 0;
    g_DisplaySettings.engineState[1].bgSettings[1].priority = 1;
    g_DisplaySettings.engineState[1].bgSettings[2].priority = 2;
    g_DisplaySettings.engineState[1].bgSettings[3].priority = 3;

    g_DisplaySettings.engineState[1].bgSettings[0].mosaic = 0;
    g_DisplaySettings.engineState[1].bgSettings[1].mosaic = 0;
    g_DisplaySettings.engineState[1].bgSettings[2].mosaic = 0;
    g_DisplaySettings.engineState[1].bgSettings[3].mosaic = 0;

    Display_SetSubLayers(LAYER_BG0 | LAYER_BG1 | LAYER_BG2 | LAYER_BG3 | LAYER_OBJ);

    g_DisplaySettings.engineState[0].blendMode   = 1;
    g_DisplaySettings.engineState[0].blendLayer0 = 1;
    g_DisplaySettings.engineState[0].blendLayer1 = 62;
    g_DisplaySettings.engineState[0].blendCoeff0 = 6;
    g_DisplaySettings.engineState[0].blendCoeff1 = 10;

    g_DisplaySettings.engineState[1].blendMode   = 1;
    g_DisplaySettings.engineState[1].blendLayer0 = 1;
    g_DisplaySettings.engineState[1].blendLayer1 = 62;
    g_DisplaySettings.engineState[1].blendCoeff0 = 6;
    g_DisplaySettings.engineState[1].blendCoeff1 = 10;

    g_DisplaySettings.controls[0].windows |= 4;
    g_DisplaySettings.engineState[0].windowOutside        = 31;
    g_DisplaySettings.engineState[0].windowOutsideEffects = FALSE;
    g_DisplaySettings.controls[1].windows |= 4;
    g_DisplaySettings.engineState[1].windowOutside        = 31;
    g_DisplaySettings.engineState[1].windowOutsideEffects = FALSE;
    Interrupts_RegisterVBlankCallback(func_ov002_0208b790, TRUE);
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
    Display_InitMainBG3(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_256x256, GX_BG_COLORS_16, 6, 3, 1, 0x460C);

    Display_Commit();
    OamMgr_Init();
}

void func_ov002_0208c228(void) {
    Interrupts_ForceVBlank();
    Display_Init();
    Interrupts_Init();
    HBlank_Init();
    DMA_Init(0x100);
    Interrupts_RegisterHBlankCallback(0, 1);
    Interrupts_RegisterVBlankCallback(func_ov002_0208b790, 1);
    g_DisplaySettings.controls[0].brightness = -16;
    g_DisplaySettings.controls[1].brightness = -16;
    GX_DisableBankForLcdc();
    GX_SetBankForLcdc(0x1FF);
    GX_SetBankForBg(1);
    GX_SetBankForObj(2);
    GX_SetBankForSubBg(4);
    GX_SetBankForSubObj(8);
    MI_CpuFill(0, 0x06800000, 0xA4000);
    MI_CpuFill(0, 0x06000000, 0x80000);
    Interrupts_ForceVBlank();
    MI_CpuFill(0, 0x06200000, 0x20000);
    MI_CpuFill(0, 0x06400000, 0x40000);
    MI_CpuFill(0, 0x06600000, 0x20000);
    Interrupts_ForceVBlank();
    g_DisplaySettings.controls[0].objTileMode = GX_OBJTILEMODE_1D_128K;
    g_DisplaySettings.controls[0].objBmpMode  = GX_OBJBMPMODE_1D_128K;
    g_DisplaySettings.controls[1].objTileMode = GX_OBJTILEMODE_1D_128K;
    g_DisplaySettings.controls[1].objBmpMode  = GX_OBJBMPMODE_1D_128K;
    g_DisplaySettings.unk_000                 = 0;
    REG_POWER_CNT &= ~0x8000;
    g_DisplaySettings.controls[0].dispMode  = GX_DISPMODE_GRAPHICS;
    g_DisplaySettings.controls[0].bgMode    = GX_BGMODE_0;
    g_DisplaySettings.controls[0].dimension = GX2D3D_MODE_2D;
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX2D3D_MODE_2D);

    g_DisplaySettings.engineState[0].bgSettings[0].priority = 0;
    g_DisplaySettings.engineState[0].bgSettings[1].priority = 1;
    g_DisplaySettings.engineState[0].bgSettings[2].priority = 2;
    g_DisplaySettings.engineState[0].bgSettings[3].priority = 3;

    g_DisplaySettings.engineState[0].bgSettings[0].mosaic = 0;
    g_DisplaySettings.engineState[0].bgSettings[1].mosaic = 0;
    g_DisplaySettings.engineState[0].bgSettings[2].mosaic = 0;
    g_DisplaySettings.engineState[0].bgSettings[3].mosaic = 0;

    Display_SetMainLayers(LAYER_BG0 | LAYER_BG1 | LAYER_BG2 | LAYER_BG3 | LAYER_OBJ);
    g_DisplaySettings.controls[1].bgMode = GX_BGMODE_0;
    GXs_SetGraphicsMode(0);

    Display_InitSubBG0(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_512x256, GX_BG_COLORS_16, 0, 7, 0, 0x401C);
    Display_InitSubBG1(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_512x256, GX_BG_COLORS_16, 2, 5, 0, 0x4214);
    Display_InitSubBG2(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_512x256, GX_BG_COLORS_16, 4, 3, 1, 0x440c);
    Display_InitSubBG3(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_512x256, GX_BG_COLORS_16, 6, 1, 1, 0x4604);

    g_DisplaySettings.engineState[1].bgSettings[0].priority = 0;
    g_DisplaySettings.engineState[1].bgSettings[1].priority = 1;
    g_DisplaySettings.engineState[1].bgSettings[2].priority = 2;
    g_DisplaySettings.engineState[1].bgSettings[3].priority = 3;

    g_DisplaySettings.engineState[1].bgSettings[0].mosaic = 0;
    g_DisplaySettings.engineState[1].bgSettings[1].mosaic = 0;
    g_DisplaySettings.engineState[1].bgSettings[2].mosaic = 0;
    g_DisplaySettings.engineState[1].bgSettings[3].mosaic = 0;

    Display_SetSubLayers(LAYER_BG0 | LAYER_BG1 | LAYER_BG2 | LAYER_BG3 | LAYER_OBJ);
    g_DisplaySettings.controls[0].windows |= 4;
    g_DisplaySettings.controls[1].windows |= 4;

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
    OamMgr_Init();
}
