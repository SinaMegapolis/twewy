#include "OtosuMenuShared.h"

const Ov002_U16_4 data_ov002_02092160 = {0xFFFF, 0xFFFF, 0xFFFF, 0x0000}; /* const */

static PrcStepFn data_ov002_02093004[1] = {func_ov002_0208c8b0};

PrcFrameDesc data_ov002_02093008 = {
    .enter     = func_ov002_0208c794,
    .stepTable = data_ov002_02093004,
    .update    = func_ov002_0208c878,
    .render    = func_ov002_0208c88c,
    .exit      = func_ov002_0208c864,
};

SpriteFrameInfo* func_ov002_0208c6f8(Sprite* sprite, s32 arg, s32 mode) {
    Sprite_FrameInfoCallback(sprite, mode);
}

static const SpriteAnimation data_ov002_02092170 = {
    .bits_0_1          = 1,
    .dataType          = 0,
    .bit_6             = 0,
    .bits_7_9          = 5,
    .bits_10_11        = 0,
    .bits_12_13        = 1,
    .bits_14_15        = 0,
    .unk_02            = 0x0000,
    .posX              = -13,
    .posY              = 0x000C,
    .frameInfoCallback = func_ov002_0208c6f8,
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

void func_ov002_0208c794(PrcCtx* ctx, OtosuMenuObj* menuObj, s32 arg2) {
    SpriteAnimation anim = data_ov002_02092170;

    anim.dataType  = (u16)menuObj->unk_1158C;
    anim.animIndex = 1;
    anim.bits_0_1  = 1;
    anim.posX      = 0x80;
    anim.posY      = 0x60;
    anim.unk_1C    = 0x11;
    anim.unk_20    = 0x14;
    anim.unk_22    = 2;
    anim.unk_26    = 0x12;
    anim.unk_28    = 0x13;
    if (_Sprite_Load(&menuObj->unk_460C0, &anim) != 0) {
        return;
    }
    OS_WaitForever();
}

void func_ov002_0208c864(PrcCtx* ctx, OtosuMenuObj* menuObj) {
    Sprite_Destroy(&menuObj->unk_460C0);
}

void func_ov002_0208c878(PrcCtx* ctx, OtosuMenuObj* menuObj) {
    Sprite_UpdateAndCheck(&menuObj->unk_460C0);
}

void func_ov002_0208c88c(PrcCtx* ctx, OtosuMenuObj* menuObj) {
    if (menuObj->unk_462E8 == 0) {
        return;
    }
    Sprite_Render(&menuObj->unk_460C0);
}

static const u16 data_ov002_02092168[4] = {5, 4, 3, 2};

PrcStepResult func_ov002_0208c8b0(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;

    u16 options[4];
    options = data_ov002_02092168;

    s32 index = func_ov040_0209cb5c();

    if ((index < 0) || (index > 3)) {
        OS_WaitForever();
    }
    Sprite_ChangeAnimation(&menuObj->unk_460C0, menuObj->unk_460C0.animData, options[index], menuObj->unk_460C0.cellTable);
    return PRC_STEP_CONTINUE;
}
