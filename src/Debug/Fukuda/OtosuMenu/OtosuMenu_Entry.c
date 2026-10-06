#include "OtosuMenuShared.h"

static PrcStepFn data_ov002_0209327c[] = {
    func_ov002_0208f87c, OtosuPrcStep_FadeStart_Neutral2, OtosuPrcStep_FadeWait_Neutral2,
    func_ov002_0208f92c, OtosuPrcStep_FadeStart_Bright,   OtosuPrcStep_FadeWait,
    PrcStep_PopFrame,
};

PrcFrameDesc data_ov002_02093254 = {
    .enter     = func_ov002_0208f860,
    .stepTable = data_ov002_0209327c,
    .update    = func_ov002_0208f874,
    .render    = func_ov002_0208f878,
    .exit      = func_ov002_0208f864,
};

static PrcStepFn data_ov002_02093298[] = {
    func_ov002_0208f87c,
    OtosuPrcStep_FadeStart_Neutral2,
    OtosuPrcStep_FadeWait_Neutral2,
    func_ov002_0208f89c,
    OtosuPrcStep_FadeStart_Bright,
    OtosuPrcStep_FadeWait,
    func_ov002_0208f918,
    PrcStep_PopFrame,
};

PrcFrameDesc data_ov002_02093268 = {
    .enter     = func_ov002_0208f860,
    .stepTable = data_ov002_02093298,
    .update    = func_ov002_0208f874,
    .render    = func_ov002_0208f878,
    .exit      = func_ov002_0208f864,
};

void func_ov002_0208f860(void) {}

void func_ov002_0208f864(PrcCtx* ctx, void* arg1) {
    func_ov002_02085710(arg1);
}

void func_ov002_0208f874(void) {}

void func_ov002_0208f878(void) {}

PrcStepResult func_ov002_0208f87c(PrcCtx* ctx, void* arg1) {
    func_ov002_02083694(arg1);
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_0208f89c(PrcCtx* ctx, void* arg0) {
    if ((TouchInput_WasTouchPressed() != 0) || (SysControl.buttonState.pressedButtons & 1) ||
        (SysControl.buttonState.pressedButtons & 2) || (SysControl.buttonState.pressedButtons & 0x400) ||
        (SysControl.buttonState.pressedButtons & 0x800) || (SysControl.buttonState.pressedButtons & 0x40) ||
        (SysControl.buttonState.pressedButtons & 0x80) || (SysControl.buttonState.pressedButtons & 0x20) ||
        (SysControl.buttonState.pressedButtons & 0x10))
    {
        SndMgr_StartPlayingSE(SEIDX_SE_BAYBADGE_MENU_EXECUTE);
        PrcCtx_AdvanceStep(ctx);
    }
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_0208f918(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    menuObj->unk_46074 = 1;
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_0208f92c(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    if ((TouchInput_WasTouchPressed() != 0) || (SysControl.buttonState.pressedButtons & 1) ||
        (SysControl.buttonState.pressedButtons & 2) || (SysControl.buttonState.pressedButtons & 0x400) ||
        (SysControl.buttonState.pressedButtons & 0x800) || (SysControl.buttonState.pressedButtons & 0x40) ||
        (SysControl.buttonState.pressedButtons & 0x80) || (SysControl.buttonState.pressedButtons & 0x20) ||
        (SysControl.buttonState.pressedButtons & 0x10))
    {
        SndMgr_StartPlayingSE(SEIDX_SE_BAYBADGE_MENU_EXECUTE);
        PrcCtx_ReplaceFrame(&menuObj->unk_4161C, &data_ov002_02092c58, NULL);
        PrcCtx_PushStepTable(ctx, PrcSteps_FadeBright);
    }
    return PRC_STEP_CONTINUE;
}
