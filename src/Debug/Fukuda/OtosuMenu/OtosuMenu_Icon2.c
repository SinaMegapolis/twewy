#include "OtosuMenuShared.h"

static PrcStepFn data_ov002_020934ac[] = {func_ov002_02091a9c};

PrcFrameDesc data_ov002_020934b0 = {
    .enter     = func_ov002_02091a14,
    .stepTable = data_ov002_020934ac,
    .update    = func_ov002_02091a7c,
    .render    = func_ov002_02091a8c,
    .exit      = func_ov002_02091a6c,
};

SpriteFrameInfo* func_ov002_02091978(Sprite* sprite, s32 arg, s32 mode) {
    Sprite_FrameInfoCallback(sprite, mode);
}

static const SpriteAnimation data_ov002_02092ab8 = {
    .bits_0_1          = 0,
    .dataType          = 0,
    .bit_6             = 0,
    .bits_7_9          = 5,
    .bits_10_11        = 0,
    .bits_12_13        = 1,
    .bits_14_15        = 0,
    .unk_02            = {.raw = 0x0800},
    .posX              = 0x0080,
    .posY              = 0x0060,
    .frameInfoCallback = func_ov002_02091978,
    .callbackArg       = 0,
    .owner             = NULL,
    .binIden           = &data_ov002_02091acc,
    .unk_18            = 0x0000,
    .packIndex         = 0,
    .unk_1C            = 0x0021,
    .unk_1E            = 0x0000,
    .unk_20            = 0x0024,
    .unk_22            = 0x0001,
    .unk_24            = 0x0000,
    .unk_26            = 0x0022,
    .unk_28            = 0x0023,
    .animIndex         = 0x0001,
};

void func_ov002_02091a14(s32 arg0, void* arg1) {
    SpriteAnimation anim = data_ov002_02092ab8;

    if (_Sprite_Load(arg1, &anim) == 0) {
        OS_WaitForever();
    }
}

void func_ov002_02091a6c(s32 arg0, void* arg1) {
    Sprite_Destroy(arg1);
}

void func_ov002_02091a7c(s32 arg0, void* arg1) {
    Sprite_UpdateAndCheck(arg1);
}

void func_ov002_02091a8c(s32 arg0, void* arg1) {
    Sprite_Render(arg1);
}

PrcStepResult func_ov002_02091a9c(PrcCtx* ctx, void* arg1) {
    return PRC_STEP_CONTINUE;
}
