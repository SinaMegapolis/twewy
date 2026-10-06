#include "OtosuMenuShared.h"

static PrcStepFn data_ov002_020931dc[] = {func_ov002_0208ec1c, func_ov002_0208ec34, PrcStep_Continue};

PrcFrameDesc data_ov002_020931e8 = {
    .enter     = func_ov002_0208e92c,
    .stepTable = data_ov002_020931dc,
    .update    = func_ov002_0208eb3c,
    .render    = func_ov002_0208ebe4,
    .exit      = func_ov002_0208eb04,
};
static const Ov002_U16_4x4 data_ov002_020923fc = {
    0x0001, 0x0004, 0x0002, 0x0003, 0x0005, 0x0008, 0x0006, 0x0007,
    0x0009, 0x000C, 0x000A, 0x000B, 0x000D, 0x0010, 0x000E, 0x000F,
}; /* const */

SpriteFrameInfo* func_ov002_0208e890(Sprite* sprite, s32 arg, s32 mode) {
    Sprite_FrameInfoCallback(sprite, mode);
}

static const SpriteAnimation data_ov002_0209241c = {
    .bits_0_1          = 0,
    .dataType          = 0,
    .bit_6             = 0,
    .bits_7_9          = 5,
    .bits_10_11        = 0,
    .bits_12_13        = 1,
    .bits_14_15        = 0,
    .unk_02            = 0x0000,
    .posX              = -13,
    .posY              = 0x000C,
    .frameInfoCallback = func_ov002_0208e890,
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

void func_ov002_0208e92c(PrcCtx* ctx, void* arg1, Ov002_TitleEnmArg* arg) {
    Ov002_U16_4x4   cells       = data_ov002_020923fc;
    u8              palettes[4] = {3, 3, 2, 2};
    SpriteAnimation anim        = data_ov002_0209241c;

    OVMGR_S32(arg1, 0x104) = 0x80000;
    OVMGR_S32(arg1, 0x108) = 0x1C0000;
    anim.unk_1C            = cells.data[arg->index][0];
    anim.unk_22            = 4;
    anim.unk_20            = cells.data[arg->index][1];
    anim.unk_26            = cells.data[arg->index][2];
    anim.unk_28            = cells.data[arg->index][3];
    anim.animIndex         = 1;
    anim.bits_0_1          = 0;
    anim.unk_02.unk_10     = palettes[arg->index];
    if (_Sprite_Load((Sprite*)((u8*)arg1 + 0x4), &anim) == 0) {
        OS_WaitForever();
    }
    anim.animIndex = 3;
    if (_Sprite_Load((Sprite*)((u8*)arg1 + 0x44), &anim) == 0) {
        OS_WaitForever();
    }
    anim.bits_0_1  = 1;
    anim.animIndex = 1;
    if (_Sprite_Load((Sprite*)((u8*)arg1 + 0x84), &anim) == 0) {
        OS_WaitForever();
    }
    anim.animIndex = 3;
    if (_Sprite_Load((Sprite*)((u8*)arg1 + 0xC4), &anim) == 0) {
        OS_WaitForever();
    }
    OVMGR_U16(arg1, 0x110) = arg->posX;
    OVMGR_U16(arg1, 0x112) = arg->posX;
    OVMGR_U16(arg1, 0x0)   = arg->index;
}

void func_ov002_0208eb04(PrcCtx* ctx, s32 arg1) {
    u16 var_r6;

    var_r6 = 0;
    do {
        Sprite_Destroy(arg1 + 4 + (var_r6 << 6));
        Sprite_Destroy(arg1 + 0x84 + (var_r6 << 6));
        var_r6 += 1;
    } while ((u32)var_r6 < 2U);
}

#define OVMGR_S16(base, off) (*(s16*)((u8*)(base) + (off)))

void func_ov002_0208eb3c(PrcCtx* ctx, void* arg1) {
    u16 var_r6;

    OVMGR_S16(arg1, 0x10) = (s16)((s32)OVMGR_S32(arg1, 0x104) >> 0xC);
    OVMGR_S16(arg1, 0x12) = (s16)((s32)(OVMGR_S32(arg1, 0x108) - 0x40000) >> 0xC);
    OVMGR_S16(arg1, 0x90) = (s16)((s32)OVMGR_S32(arg1, 0x104) >> 0xC);
    OVMGR_S16(arg1, 0x92) = (s16)((s32)(OVMGR_S32(arg1, 0x108) + 0x80000) >> 0xC);
    OVMGR_S16(arg1, 0x50) = (s16)((s32)OVMGR_S32(arg1, 0x104) >> 0xC);
    OVMGR_S16(arg1, 0x52) = (s16)((s32)(OVMGR_S32(arg1, 0x108) + 0x40000) >> 0xC);
    OVMGR_S16(arg1, 0xD0) = (s16)((s32)OVMGR_S32(arg1, 0x104) >> 0xC);
    OVMGR_S16(arg1, 0xD2) = (s16)((s32)(OVMGR_S32(arg1, 0x108) + 0x100000) >> 0xC);
    var_r6                = 0;
    do {
        Sprite_UpdateAndCheck(arg1 + 4 + (var_r6 << 6));
        Sprite_UpdateAndCheck(arg1 + 0x84 + (var_r6 << 6));
        var_r6 += 1;
    } while ((u32)var_r6 < 2U);
}

void func_ov002_0208ebe4(PrcCtx* ctx, s32 arg1) {
    u16 var_r6;

    var_r6 = 0;
    do {
        Sprite_Render(arg1 + 4 + (var_r6 << 6));
        Sprite_Render(arg1 + 0x84 + (var_r6 << 6));
        var_r6 += 1;
    } while ((u32)var_r6 < 2U);
}

PrcStepResult func_ov002_0208ec1c(PrcCtx* ctx, void* arg1) {
    OVMGR_S32(arg1, 0x10C) = 0;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_REPEAT;
}

PrcStepResult func_ov002_0208ec34(PrcCtx* ctx, void* arg1) {
    u16 targetX[4] = {0x2C, 0x26, 0x75, 0x6C};
    f32 var_r0;
    f32 var_r0_2;
    s32 temp_r0;
    s32 temp_r0_3;
    u16 temp_r0_2;
    u16 temp_r1;
    u16 temp_r2;

    temp_r1 = targetX[OVMGR_U16(arg1, 0x0)];
    temp_r0 = temp_r1 << 0xC;
    if (temp_r1 != 0) {
        var_r0 = 0.5f + (f32)temp_r0;
    } else {
        var_r0 = (f32)temp_r0 - 0.5f;
    }
    func_020265d4(arg1 + 0x108, (s32)var_r0, OVMGR_U16(arg1, 0x112));
    func_020265d4(arg1 + 0x10C, 0x10000, OVMGR_U16(arg1, 0x112));
    temp_r2 = OVMGR_U16(arg1, 0x112);
    if (temp_r2 == 0) {
        PrcCtx_AdvanceStep(ctx);
        return PRC_STEP_CONTINUE;
    }
    if (data_ov002_02093660 == 1) {
        temp_r0_2 = targetX[OVMGR_U16(arg1, 0x0)];
        temp_r0_3 = temp_r0_2 << 0xC;
        if (temp_r0_2 != 0) {
            var_r0_2 = 0.5f + (f32)temp_r0_3;
        } else {
            var_r0_2 = (f32)temp_r0_3 - 0.5f;
        }
        OVMGR_S32(arg1, 0x108) = (s32)var_r0_2;
        PrcCtx_AdvanceStep(ctx);
        return PRC_STEP_CONTINUE;
    }
    OVMGR_U16(arg1, 0x112) = (u16)(temp_r2 - 1);
    return PRC_STEP_CONTINUE;
}
