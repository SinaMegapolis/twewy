#include "OtosuMenuShared.h"

static PrcStepFn data_ov002_0209323c[] = {func_ov002_0208f858};

PrcFrameDesc data_ov002_02093240 = {
    .enter     = func_ov002_0208f794,
    .stepTable = data_ov002_0209323c,
    .update    = func_ov002_0208f838,
    .render    = func_ov002_0208f848,
    .exit      = func_ov002_0208f828,
};

SpriteFrameInfo* func_ov002_0208f6f8(Sprite* sprite, s32 arg, s32 mode) {
    Sprite_FrameInfoCallback(sprite, mode);
}

static const SpriteAnimation data_ov002_020924f8 = {
    .bits_0_1          = 1,
    .dataType          = 0,
    .bit_6             = 0,
    .bits_7_9          = 5,
    .bits_10_11        = 0,
    .bits_12_13        = 1,
    .bits_14_15        = 0,
    .unk_02            = 0x0000,
    .posX              = 0x0080,
    .posY              = 0x0060,
    .frameInfoCallback = func_ov002_0208f6f8,
    .callbackArg       = 0,
    .owner             = NULL,
    .binIden           = (BinIdentifier*)&data_ov002_02091acc,
    .unk_18            = 0x0000,
    .packIndex         = 0x0000,
    .unk_1C            = 0x0015,
    .unk_1E            = 0x0000,
    .unk_20            = 0x0018,
    .unk_22            = 0x0002,
    .unk_24            = 0x0000,
    .unk_26            = 0x0016,
    .unk_28            = 0x0017,
    .animIndex         = 0x0001,
};

void func_ov002_0208f794(PrcCtx* ctx, void* arg1) {
    SpriteAnimation anim     = data_ov002_020924f8;
    u16*            anim_u16 = (u16*)&anim;

    anim_u16[0]    = (u16)((anim_u16[0] & ~3) | 1);
    anim.unk_1C    = 0x19;
    anim.unk_20    = 0x1C;
    anim.unk_26    = 0x1A;
    anim.unk_28    = 0x1B;
    anim.animIndex = 1;
    anim.unk_22    = 1;
    if (_Sprite_Load(arg1, &anim) != 0) {
        return;
    }
    OS_WaitForever();
}

void func_ov002_0208f828(PrcCtx* ctx, void* arg1) {
    Sprite_Destroy(arg1);
}

void func_ov002_0208f838(PrcCtx* ctx, void* arg1) {
    Sprite_UpdateAndCheck(arg1);
}

void func_ov002_0208f848(PrcCtx* ctx, void* arg1) {
    Sprite_Render(arg1);
}

PrcStepResult func_ov002_0208f858(PrcCtx* ctx, void* unused) {
    return PRC_STEP_CONTINUE;
}
