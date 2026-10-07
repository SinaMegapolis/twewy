#include "OtosuMenuShared.h"
static const OtosuMenuCursor    data_ov002_02091fe0 = {2, 0x02, 0x9D};
static const OtosuMenuRectList2 data_ov002_02091ffa = {
    {
     {0x0000, 0x0008, 0x00A0, 0x0058, 0x00B8},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF},
     }
};
static const OtosuMenuRectList2 data_ov002_02091fe6 = {
    {
     {0x23EE, 0x0000, 0x0000, 0x0100, 0x00C0},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000},
     }
};
static const OtosuMenuRectList3 data_ov002_0209204a = {
    {
     {0x23EF, 8, 8, 0xF8, 0x40},
     {0x23EB, 0x18, 0xA6, 0x58, 0xB6},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0},
     }
};
static const OtosuMenuRectList3 data_ov002_0209202c = {
    {
     {0x23EC, 8, 8, 0xF8, 0x40},
     {0x23EB, 0x18, 0xA6, 0x58, 0xB6},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0},
     }
};
static const OtosuMenuRectList3 data_ov002_0209200e = {
    {
     {0x23EC, 8, 8, 0xF8, 0x40},
     {0x23EB, 0x18, 0xA6, 0x58, 0xB6},
     {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0},
     }
};

PrcFrameDesc data_ov002_02092e54 = {
    .enter     = func_ov002_020894f8,
    .stepTable = data_ov002_02092efc,
    .update    = func_ov002_0208950c,
    .render    = func_ov002_02089510,
    .exit      = func_ov002_020894fc,
};

PrcStepFn data_ov002_02092e68[] = {
    OtosuPrcStep_FadeStart_BrightImmediate,
    OtosuPrcStep_FadeWait_Immediate,
    func_ov002_02089514,
    func_ov002_0208958c,
    func_ov002_020895c8,
    func_ov002_020896e4,
    OtosuPrcStep_FadeStart_Neutral,
    OtosuPrcStep_FadeWait_Neutral,
    func_ov002_02089a94,
    OtosuPrcStep_FadeStart_BrightImmediate,
    OtosuPrcStep_FadeWait_Immediate,
    func_ov002_02089ac8,
};

PrcStepFn data_ov002_02092ec8[] = {
    OtosuPrcStep_FadeStart_BrightImmediate,
    OtosuPrcStep_FadeWait_Immediate,
    func_ov002_02089514,
    func_ov002_0208958c,
    func_ov002_020895c8,
    func_ov002_02089798,
    OtosuPrcStep_FadeStart_Neutral,
    OtosuPrcStep_FadeWait_Neutral,
    func_ov002_02089920,
    OtosuPrcStep_FadeStart_BrightImmediate,
    OtosuPrcStep_FadeWait_Immediate,
    func_ov002_02089b1c,
    PrcStep_PopFrame,
};

PrcFrameDesc data_ov002_02092e2c = {
    .enter     = func_ov002_020894f8,
    .stepTable = data_ov002_02092ec8,
    .update    = func_ov002_0208950c,
    .render    = func_ov002_02089510,
    .exit      = func_ov002_020894fc,
};

PrcFrameDesc data_ov002_02092e40 = {
    .enter     = func_ov002_020894f8,
    .stepTable = data_ov002_02092e98,
    .update    = func_ov002_0208950c,
    .render    = func_ov002_02089510,
    .exit      = func_ov002_020894fc,
};

PrcStepFn data_ov002_02092e98[] = {
    OtosuPrcStep_FadeStart_BrightImmediate,
    OtosuPrcStep_FadeWait_Immediate,
    func_ov002_02089514,
    func_ov002_0208958c,
    func_ov002_02089860,
    OtosuPrcStep_FadeStart_Neutral,
    OtosuPrcStep_FadeWait_Neutral,
    func_ov002_02089920,
    OtosuPrcStep_FadeStart_BrightImmediate,
    OtosuPrcStep_FadeWait_Immediate,
    func_ov002_02089adc,
    PrcStep_PopFrame,
};

PrcFrameDesc data_ov002_02092e18 = {
    .enter     = func_ov002_020894f8,
    .stepTable = data_ov002_02092e68,
    .update    = func_ov002_0208950c,
    .render    = func_ov002_02089510,
    .exit      = func_ov002_020894fc,
};

PrcStepFn data_ov002_02092efc[] = {
    OtosuPrcStep_FadeStart_BrightImmediate,
    OtosuPrcStep_FadeWait_Immediate,
    func_ov002_02089514,
    func_ov002_0208958c,
    func_ov002_020895c8,
    func_ov002_02089624,
    OtosuPrcStep_FadeStart_Neutral,
    OtosuPrcStep_FadeWait_Neutral,
    func_ov002_02089920,
    OtosuPrcStep_FadeStart_BrightImmediate,
    OtosuPrcStep_FadeWait_Immediate,
    func_ov002_02089af0,
    PrcStep_PopFrame,
};

void func_ov002_020894f8(void) {}

void func_ov002_020894fc(PrcCtx* ctx, void* arg1) {
    func_ov002_02085710(arg1);
}

void func_ov002_0208950c(void) {}

void func_ov002_02089510(void) {}

PrcStepResult func_ov002_02089514(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->objCtx[0]);
    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->objCtx[1]);
    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->objCtx[2]);
    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->objCtx[3]);
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_REPEAT;
}

PrcStepResult func_ov002_0208958c(PrcCtx* ctx, void* unused) {
    g_DisplaySettings.engineState[0].window0              = 31;
    g_DisplaySettings.engineState[0].window0Effects       = TRUE;
    g_DisplaySettings.engineState[0].windowOutside        = 31;
    g_DisplaySettings.engineState[0].windowOutsideEffects = TRUE;
    Display_Commit();
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_020895c8(PrcCtx* ctx, void* unused) {
    s32 temp_r0;

    temp_r0 = (s32)func_ov040_0209cb78();
    switch (temp_r0) { /* irregular */
        case 3:
            break;
        case 10:
        case 0:
            PrcCtx_AdvanceStep(ctx);
            break;
        case 1:
            func_ov040_0209d6cc();
            break;
        default:
            func_ov040_0209d540();
            break;
    }
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02089624(PrcCtx* ctx, void* object) {
    OtosuMenuObj*      menuObj = (OtosuMenuObj*)object;
    OtosuMenuRectList3 layout  = data_ov002_0209202c;

    SystemStatusFlags;
    SystemStatusFlags.unk_06 = 1;
    SystemStatusFlags;
    SystemStatusFlags.unk_07 = 1;
    func_ov002_02082f18(menuObj, 0x18, 0x1B, layout.rects);
    menuObj->cursor.posX = 0;
    menuObj->cursor.posY = 0xC8;
    menuObj->unk_474C8   = 0xFFFF;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_020896e4(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    OtosuMenuRectList2 table_sp0 = data_ov002_02091fe6;
    u16                i;

    CriSndMgr_Pause(0x19, 1);
    SystemStatusFlags;
    SystemStatusFlags.unk_06 = 1;
    SystemStatusFlags;
    SystemStatusFlags.unk_07 = 1;
    func_ov002_02085710(menuObj);
    func_ov002_0208bd40();
    func_ov002_02083484(menuObj, table_sp0.rects);
    menuObj->unk_474C8 = 0xFFFF;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02089798(PrcCtx* ctx, void* object) {
    OtosuMenuObj*      menuObj = (OtosuMenuObj*)object;
    OtosuMenuRectList3 layout  = data_ov002_0209200e;

    SystemStatusFlags;
    SystemStatusFlags.unk_06 = 1;
    SystemStatusFlags;
    SystemStatusFlags.unk_07 = 1;
    func_ov002_02082f18(menuObj, 0x18, 0x1B, layout.rects);
    menuObj->cursor.posX        = 0;
    menuObj->cursor.posY        = 0xC8;
    menuObj->linkLevelIcon.posX = 0;
    menuObj->linkLevelIcon.posY = 0xC8;
    menuObj->unk_474C8          = 0xFFFF;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02089860(PrcCtx* ctx, void* object) {
    OtosuMenuObj*      menuObj = (OtosuMenuObj*)object;
    OtosuMenuRectList3 layout  = data_ov002_0209204a;

    SystemStatusFlags;
    SystemStatusFlags.unk_06 = 1;
    SystemStatusFlags;
    SystemStatusFlags.unk_07 = 1;
    menuObj->unk_474C8       = 0xFFFF;
    func_ov002_02082f18(menuObj, 0x18, 0x1B, layout.rects);
    menuObj->cursor.posX = 0;
    menuObj->cursor.posY = 0xC8;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02089920(PrcCtx* ctx, void* object) {
    OtosuMenuObj*      menuObj   = (OtosuMenuObj*)object;
    OtosuMenuRectList2 table_sp6 = data_ov002_02091ffa;
    OtosuMenuCursor    anims[1]  = {data_ov002_02091fe0};
    u16                selected  = func_ov002_0208597c(table_sp6.rects);

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
        if (selected == 0) {
            menuObj->cursor.posX = 0;
            menuObj->cursor.posY = 210;
            SndMgr_StartPlayingSE(SEIDX_SE_BAYBADGE_MENU_EXECUTE);
            PrcCtx_AdvanceStep(ctx);
            return PRC_STEP_CONTINUE;
        }
    } else {
        menuObj->unk_474C8 = selected;
        SndMgr_StartPlayingSE(SEIDX_SE_BAYBADGE_MENU_CURSOR);
        if (selected == 0) {
            menuObj->cursor.posX = anims[selected].x;
            menuObj->cursor.posY = anims[selected].y;
            Sprite_ChangeAnimation(&menuObj->cursor, menuObj->cursor.animData, anims[selected].anim,
                                   menuObj->cursor.cellTable);
            return PRC_STEP_CONTINUE;
        }
    }
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02089a94(PrcCtx* ctx, void* unused) {
    if (SysControl.buttonState.pressedButtons & 1) {
        SndMgr_StartPlayingSE(SEIDX_SE_BAYBADGE_MENU_CANCEL);
        PrcCtx_AdvanceStep(ctx);
    }
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02089ac8(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    menuObj->nextScene = 3;
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02089adc(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    menuObj->nextScene = 5;
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02089af0(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    func_ov002_02085710(menuObj);
    PrcCtx_ReplaceFrame(ctx, &OtosuMenu_RoleSelect_FrameDesc, NULL);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02089b1c(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    func_ov002_02085710(menuObj);
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}
