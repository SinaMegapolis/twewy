#include "OtosuMenuShared.h"

char data_ov002_02092be4[] = "OtosuMenuObj";

static PrcStepFn data_ov002_02092c08[] = {
    OtosuPrcStep_FadeStart_BrightImmediate,
    OtosuPrcStep_FadeWait_Immediate,
    func_ov002_020870c8,
    OtosuPrcStep_FadeStart_NeutralSlow,
    OtosuPrcStep_FadeWait_NeutralSlow,
    func_ov002_0208749c,
    OtosuPrcStep_FadeStart_BrightImmediate,
    OtosuPrcStep_FadeWait_Immediate,
    func_ov002_02087508,
    PrcStep_PopFrame,
};

static PrcStepFn data_ov002_02092c30[] = {
    func_ov002_02086cec,
    OtosuPrcStep_FadeStart_Neutral2,
    OtosuPrcStep_FadeWait_Neutral2,
    func_ov002_02086c64,
    func_ov002_02086c84,
    func_ov002_02086e4c,
    func_ov002_02086eb4,
    func_ov002_02086ee8,
    func_ov002_020870ac,
    PrcStep_PopFrame,
};

PrcFrameDesc data_ov002_02092c58 = {
    .enter     = func_ov002_02086b8c,
    .stepTable = data_ov002_02092c30,
    .update    = func_ov002_02086c5c,
    .render    = func_ov002_02086c60,
    .exit      = func_ov002_02086bb0,
};

static PrcFrameDesc data_ov002_02092bf4 = {
    .enter     = func_ov002_02086bac,
    .stepTable = data_ov002_02092c08,
    .update    = func_ov002_02086c5c,
    .render    = func_ov002_02086c60,
    .exit      = func_ov002_02086bc4,
};

void func_ov002_02086b8c(s32 arg0, OtosuMenuObj* menuObj) {
    data_ov002_02093660 = 0;
    func_ov002_02085710(menuObj);
}

void func_ov002_02086bac(void) {
    return;
}

void func_ov002_02086bb0(void) {
    SndMgr_StopPlayingSE(SEIDX_MC_01);
}

void func_ov002_02086bc4(s32 arg0, OtosuMenuObj* menuObj) {
    SndMgr_StopPlayingSE(SEIDX_MC_01);
    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->unk_474E8);
    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->unk_476D0);
    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->unk_478B8);
    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->unk_47AA0);
    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->unk_47C88);
    func_ov002_0208bd40();
    func_ov002_02085710(menuObj);
}

void func_ov002_02086c5c(void) {
    return;
}

void func_ov002_02086c60(void) {
    return;
}

PrcStepResult func_ov002_02086c64(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;
    menuObj->unk_474CC    = 30;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_REPEAT;
}

PrcStepResult func_ov002_02086c84(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;
    if (TouchInput_WasTouchPressed() != 0) {
        PrcCtx_ReplaceFrame(ctx, &data_ov002_02092bf4, NULL);
        return 0;
    }
    if (menuObj->unk_474CC == 0) {
        PrcCtx_AdvanceStep(ctx);
    }
    menuObj->unk_474CC--;
    return 0;
}

PrcStepResult func_ov002_02086cec(PrcCtx* ctx, void* object) {
    OtosuMenuObj*      menuObj = (OtosuMenuObj*)object;
    PrcCtx*            enemies[4];
    Ov002_TitleBossArg bossArg;
    Ov002_TitleEnmArg  enmArg;
    PrcCtx*            enemy;
    u16                i;

    enemies[0] = &menuObj->unk_474E8;
    enemies[1] = &menuObj->unk_476D0;
    enemies[2] = &menuObj->unk_478B8;
    enemies[3] = &menuObj->unk_47AA0;
    func_ov002_02083a74(menuObj);
    for (i = 0; i < 4; i++) {
        enemy        = enemies[i];
        enmArg.index = 3 - i;
        enmArg.posX  = 0x78;
        PrcCtx_Init(enemy, "OtosuMenu_TitleEnmObj", 0x114);
        PrcCtx_ReplaceFrame(enemy, &data_ov002_020931e8, &enmArg);
        PrcMaster_RegisterContext(&menuObj->prcMaster, enemy);
    }
    bossArg.upper = (Ov002_BgRef*)&menuObj->unk_47398;
    bossArg.lower = (Ov002_BgRef*)&menuObj->unk_47438;
    bossArg.posX  = 0x78;
    PrcCtx_Init(&menuObj->unk_47C88, "OtosuMenu_TitleBossObj", 0x118);
    PrcCtx_ReplaceFrame(&menuObj->unk_47C88, &data_ov002_02093208, &bossArg);
    PrcMaster_RegisterContext(&menuObj->prcMaster, &menuObj->unk_47C88);
    menuObj->unk_474C8 = 120;
    menuObj->unk_474D8 = -0x200000;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02086e4c(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;
    if (TouchInput_WasTouchPressed() != 0) {
        PrcCtx_ReplaceFrame(ctx, &data_ov002_02092bf4, NULL);
        return PRC_STEP_CONTINUE;
    }
    if (menuObj->unk_474C8 == 0) {
        PrcCtx_AdvanceStep(ctx);
    }
    menuObj->unk_474C8--;
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02086eb4(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;
    menuObj->unk_474C8    = 20;
    menuObj->unk_474D8    = 0x100000;
    menuObj->unk_474DC    = -0x100000;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02086ee8(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    if ((TouchInput_WasTouchPressed() != 0) || (menuObj->unk_474C8 == 0)) {
        PrcCtx_AdvanceStep(ctx);
        return PRC_STEP_CONTINUE;
    }
    func_020265d4(&menuObj->unk_474D8, 0);
    func_020265d4(&menuObj->unk_474DC, 0, menuObj->unk_474C8);

    Display_SetBGOffset(menuObj->unk_47348, menuObj->unk_4734C, menuObj->unk_474D8, 0);
    Display_SetBGOffset(menuObj->unk_473E8, menuObj->unk_473EC, menuObj->unk_474D8, 0);
    Display_SetBGOffset(menuObj->unk_47410, menuObj->unk_47414, menuObj->unk_474DC, 0);

    menuObj->unk_474C8--;
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_020870ac(PrcCtx* ctx, void* unused) {
    PrcCtx_ReplaceFrame(ctx, &data_ov002_02092bf4, NULL);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_020870c8(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    Display_SetBGOffset(menuObj->unk_47398, menuObj->unk_4739C, 0, 0);
    Display_SetBGOffset(menuObj->unk_47438, menuObj->unk_4743C, 0, 0x100000);
    Display_SetBGOffset(menuObj->unk_47348, menuObj->unk_4734C, 0, 0);
    Display_SetBGOffset(menuObj->unk_473E8, menuObj->unk_473EC, 0, 0);
    Display_SetBGOffset(menuObj->unk_47410, menuObj->unk_47414, 0, 0);
    menuObj->unk_473A0 |= 2;
    menuObj->unk_47440 |= 2;
    menuObj->unk_47350 |= 2;
    menuObj->unk_473F0 |= 2;
    menuObj->unk_47418 |= 2;
    Display_SetBGOffset(menuObj->unk_47370, menuObj->unk_47374, 0, 0);
    menuObj->unk_47378 |= 2;
    data_ov002_02093660 = 1;
    Display_SetBGOffset(menuObj->unk_47398, menuObj->unk_4739C, 0, 0);
    Display_SetBGOffset(menuObj->unk_47438, menuObj->unk_4743C, 0, 0x100000);
    menuObj->unk_474C8 = 240;
    SndMgr_StartPlayingSE(SEIDX_MC_01);
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_0208749c(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    menuObj->unk_474C8--;
    if (TouchInput_WasTouchPressed() != 0) {
        SndMgr_StartPlayingSE(SEIDX_SE_BAYBADGE_MENU_EXECUTE);
    }
    if ((TouchInput_WasTouchPressed() != 0) || (menuObj->unk_474C8 == 0)) {
        PrcCtx_AdvanceStep(ctx);
        return PRC_STEP_CONTINUE;
    }
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02087508(PrcCtx* ctx, void* unused) {
    PrcCtx_ReplaceFrame(ctx, &data_ov002_02092c9c, NULL);
    return PRC_STEP_CONTINUE;
}
