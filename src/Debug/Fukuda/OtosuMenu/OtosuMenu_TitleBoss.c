#include "OtosuMenuShared.h"

static PrcStepFn data_ov002_020931fc[] = {func_ov002_0208ee9c, func_ov002_0208eeac, PrcStep_Continue};

PrcFrameDesc data_ov002_02093208 = {
    .enter     = func_ov002_0208ed78,
    .stepTable = data_ov002_020931fc,
    .update    = func_ov002_0208edb0,
    .render    = func_ov002_0208ee98,
    .exit      = func_ov002_0208edac,
};

void func_ov002_0208ed78(PrcCtx* ctx, void* arg1, void* arg2) {
    OVMGR_S32(arg1, 0x104) = (s32)OVMGR_U32(arg2, 0x4);
    OVMGR_S32(arg1, 0x108) = (s32)OVMGR_U32(arg2, 0x8);
    OVMGR_U16(arg1, 0x114) = (u16)OVMGR_U16(arg2, 0x0);
    OVMGR_U16(arg1, 0x116) = (u16)OVMGR_U16(arg2, 0x0);
    OVMGR_S32(arg1, 0x110) = -0x200000;
}

void func_ov002_0208edac(void) {}

void func_ov002_0208edb0(PrcCtx* ctx, void* arg1) {
    Ov002_BgRef* upper = *(Ov002_BgRef**)((u8*)arg1 + 0x104);
    Ov002_BgRef* lower;

    Display_SetBGOffset(upper->engine, upper->layer, 0, OVMGR_S32(arg1, 0x110));
    lower = *(Ov002_BgRef**)((u8*)arg1 + 0x108);
    Display_SetBGOffset(lower->engine, lower->layer, 0, OVMGR_S32(arg1, 0x110) + 0x100000);
}

void func_ov002_0208ee98(void) {}

PrcStepResult func_ov002_0208ee9c(PrcCtx* ctx, void* unused) {
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_REPEAT;
}

PrcStepResult func_ov002_0208eeac(PrcCtx* ctx, void* arg1) {
    Ov002_BgRef* ref;
    u16          timer;

    func_020265d4(arg1 + 0x110, 0, OVMGR_U16(arg1, 0x116));
    timer = OVMGR_U16(arg1, 0x116);
    if (timer == 0) {
        PrcCtx_AdvanceStep(ctx);
        return PRC_STEP_CONTINUE;
    }
    if (data_ov002_02093660 == 1) {
        OVMGR_S32(arg1, 0x110) = 0;
        ref                    = *(Ov002_BgRef**)((u8*)arg1 + 0x104);
        Display_SetBGOffset(ref->engine, ref->layer, 0, 0);
        ref = *(Ov002_BgRef**)((u8*)arg1 + 0x108);
        Display_SetBGOffset(ref->engine, ref->layer, 0, 0x100000);
        (*(Ov002_BgRef**)((u8*)arg1 + 0x104))->flags |= 2;
        (*(Ov002_BgRef**)((u8*)arg1 + 0x108))->flags |= 2;
        PrcCtx_AdvanceStep(ctx);
        return PRC_STEP_CONTINUE;
    }
    OVMGR_U16(arg1, 0x116) = (u16)(timer - 1);
    return PRC_STEP_CONTINUE;
}
