#include "OtosuMenuShared.h"

char      data_ov002_02092e04[] = "OtosuMenu_DaizaObj";
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

static const Ov002_U16_10 data_ov002_02091ffa = {
    0x0000, 0x0008, 0x00A0, 0x0058, 0x00B8, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
}; /* const */

static const Ov002_U16_15 data_ov002_0209202c = {
    0x23EC, 8, 8, 0xF8, 0x40, 0x23EB, 0x18, 0xA6, 0x58, 0xB6, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0,
}; /* const */

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

PrcFrameDesc data_ov002_02092e54 = {
    .enter     = func_ov002_020894f8,
    .stepTable = data_ov002_02092efc,
    .update    = func_ov002_0208950c,
    .render    = func_ov002_02089510,
    .exit      = func_ov002_020894fc,
};

static const Ov002_U16_3  data_ov002_02091fe0 = {0x0002, 0x0002, 0x009D}; /* const */
static const Ov002_U16_10 data_ov002_02091fe6 = {
    0x23EE, 0x0000, 0x0000, 0x0100, 0x00C0, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000,
}; /* const */
PrcFrameDesc data_ov002_02092e2c = {
    .enter     = func_ov002_020894f8,
    .stepTable = data_ov002_02092ec8,
    .update    = func_ov002_0208950c,
    .render    = func_ov002_02089510,
    .exit      = func_ov002_020894fc,
};
static const Ov002_U16_15 data_ov002_0209200e = {
    0x23EC, 8, 8, 0xF8, 0x40, 0x23EB, 0x18, 0xA6, 0x58, 0xB6, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0,
}; /* const */
PrcFrameDesc data_ov002_02092e40 = {
    .enter     = func_ov002_020894f8,
    .stepTable = data_ov002_02092e98,
    .update    = func_ov002_0208950c,
    .render    = func_ov002_02089510,
    .exit      = func_ov002_020894fc,
};
char                      data_ov002_02092df4[] = "OtosuMenu_Icon2";
static const Ov002_U16_15 data_ov002_0209204a   = {
    0x23EF, 8, 8, 0xF8, 0x40, 0x23EB, 0x18, 0xA6, 0x58, 0xB6, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0,
}; /* const */

void func_ov002_020894f8(void) {}

void func_ov002_020894fc(PrcCtx* ctx, void* arg1) {
    func_ov002_02085710(arg1);
}

void func_ov002_0208950c(void) {}

void func_ov002_02089510(void) {}

PrcStepResult func_ov002_02089514(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->unk_474E8);
    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->unk_476D0);
    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->unk_478B8);
    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->unk_47AA0);
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
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;
    Ov002_U16_15  layout  = data_ov002_0209200e;

    SystemStatusFlags;
    SystemStatusFlags.unk_06 = 1;
    SystemStatusFlags;
    SystemStatusFlags.unk_07 = 1;
    func_ov002_02082f18(menuObj, 0x18, 0x1B, layout.data);
    menuObj->unk_46078.posX = 0;
    menuObj->unk_46078.posY = 0xC8;
    menuObj->unk_474C8      = 0xFFFF;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_020896e4(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    Ov002_U16_10 table_sp0 = data_ov002_02091fe6;
    u16          i;

    CriSndMgr_Pause(0x19, 1);
    SystemStatusFlags;
    SystemStatusFlags.unk_06 = 1;
    SystemStatusFlags;
    SystemStatusFlags.unk_07 = 1;
    func_ov002_02085710(menuObj);
    func_ov002_0208bd40();
    func_ov002_02083484(menuObj, table_sp0.data);
    menuObj->unk_474C8 = 0xFFFF;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02089798(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;
    Ov002_U16_15  layout  = data_ov002_0209202c;

    SystemStatusFlags;
    SystemStatusFlags.unk_06 = 1;
    SystemStatusFlags;
    SystemStatusFlags.unk_07 = 1;
    func_ov002_02082f18(menuObj, 0x18, 0x1B, layout.data);
    menuObj->unk_46078.posX = 0;
    menuObj->unk_46078.posY = 0xC8;
    menuObj->unk_460C0.posX = 0;
    menuObj->unk_460C0.posY = 0xC8;
    menuObj->unk_474C8      = 0xFFFF;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02089860(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;
    Ov002_U16_15  layout  = data_ov002_0209204a;

    SystemStatusFlags;
    SystemStatusFlags.unk_06 = 1;
    SystemStatusFlags;
    SystemStatusFlags.unk_07 = 1;
    menuObj->unk_474C8       = 0xFFFF;
    func_ov002_02082f18(menuObj, 0x18, 0x1B, layout.data);
    menuObj->unk_46078.posX = 0;
    menuObj->unk_46078.posY = 0xC8;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02089920(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    Ov002_U16_10 table_sp6 = data_ov002_02091ffa;
    Ov002_U16_3  anims     = data_ov002_02091fe0;
    s32          temp_ip;
    u16          temp_r0_2;

    temp_r0_2 = func_ov002_0208597c(table_sp6.data);
    if (temp_r0_2 == 0xFFFF) {
        return 0;
    }
    if (temp_r0_2 == 0xFFFE) {
        menuObj->unk_474C8      = 0xFFFF;
        menuObj->unk_46078.posX = 0U;
        menuObj->unk_46078.posY = 0xD2U;
        return 0;
    }
    if (menuObj->unk_474C8 == temp_r0_2) {
        if (temp_r0_2 == 0) {
            menuObj->unk_46078.posX = 0U;
            menuObj->unk_46078.posY = 0xD2U;
            SndMgr_StartPlayingSE(SEIDX_SE_BAYBADGE_MENU_EXECUTE);
            PrcCtx_AdvanceStep(ctx);
            return PRC_STEP_CONTINUE;
        }
        /* Duplicate return node #11. Try simplifying control flow for better match */
        return PRC_STEP_CONTINUE;
    }
    menuObj->unk_474C8 = temp_r0_2;
    SndMgr_StartPlayingSE(SEIDX_SE_BAYBADGE_MENU_CURSOR);
    if (temp_r0_2 == 0) {
        temp_ip                 = temp_r0_2 * 3;
        menuObj->unk_46078.posX = anims.data[temp_ip + 1];
        menuObj->unk_46078.posY = anims.data[temp_ip + 2];
        Sprite_ChangeAnimation(&menuObj->unk_46078, menuObj->unk_46078.animData, (s16)anims.data[temp_ip],
                               menuObj->unk_46078.cellTable);
        return PRC_STEP_CONTINUE;
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

    menuObj->unk_46074 = 3;
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02089adc(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    menuObj->unk_46074 = 5;
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02089af0(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    func_ov002_02085710(menuObj);
    PrcCtx_ReplaceFrame(ctx, &data_ov002_02092c9c, NULL);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02089b1c(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    func_ov002_02085710(menuObj);
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}
