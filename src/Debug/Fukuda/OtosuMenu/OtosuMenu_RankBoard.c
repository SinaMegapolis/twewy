#include "OtosuMenuShared.h"

static PrcStepFn data_ov002_0209321c[] = {func_ov002_0208f6e0, func_ov002_0208f6f0, PrcStep_Continue};

PrcFrameDesc data_ov002_02093228 = {
    .enter     = func_ov002_0208f0bc,
    .stepTable = data_ov002_0209321c,
    .update    = func_ov002_0208f630,
    .render    = func_ov002_0208f688,
    .exit      = func_ov002_0208f5d8,
};

static const Ov002_U16_10 data_ov002_020924a4 = {
    0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001A, 0x001B,
}; /* const */
static const Ov002_U16_10 data_ov002_020924b8 = {
    0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F, 0x0010, 0x0011,
}; /* const */

SpriteFrameInfo* func_ov002_0208f020(Sprite* sprite, s32 arg, s32 mode) {
    Sprite_FrameInfoCallback(sprite, mode);
}

static const SpriteAnimation data_ov002_020924cc = {
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
    .frameInfoCallback = func_ov002_0208f020,
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

void func_ov002_0208f0bc(PrcCtx* ctx, void* arg1, void* arg2) {
    SpriteAnimation anim           = data_ov002_020924cc;
    const u16       table_10[4]    = {0x0013, 0x0014, 0x0015, 0x0016};
    const u16       table_20[4]    = {0x0009, 0x000A, 0x000B, 0x000C};
    const u16       table_18[4]    = {0xFFEC, 0x0014, 0x003C, 0x0064};
    const u16       table_8[4]     = {0x0009, 0x000A, 0x000B, 0x000C};
    const u16       table_unk18[4] = {0x0000, 0x0028, 0x0050, 0x0078};
    const u16       table_30[4]    = {0xFFE8, 0x0010, 0x0038, 0x0060};
    const u16       table_40[4]    = {0x0000, 0x0028, 0x0050, 0x0078};
    const u16       table_28[4]    = {0x0013, 0x0014, 0x0015, 0x0016};
    const u16       table_38[4]    = {0x0004, 0x0005, 0x0006, 0x0007};
    const u16       table_48[4]    = {0x0000, 0x0028, 0x0050, 0x0078};
    const u16       table_78[6]    = {0x0000, 0xFFFA, 0xFFF4, 0xFFEE, 0xFFE8, 0xFFE2};
    Ov002_U16_10    table_64       = data_ov002_020924b8;
    Ov002_U16_10    table_50       = data_ov002_020924a4;
    s32             temp_hi;
    s32             var_r0;
    s32             var_r2;
    s32             var_r4;
    u16             temp_r0_4;
    u16             temp_r2_3;
    u16             temp_r3_2;
    u16             var_r0_5;
    u16             var_r1;
    u16             var_r9;
    u32             temp_r5_3;
    u8              temp_r0_3;
    OVMGR_S32(arg1, 0xC)  = (s32)OVMGR_U32(arg2, 0x0);
    OVMGR_S32(arg1, 0x10) = (s32)OVMGR_U32(arg2, 0x4);
    OVMGR_S32(arg1, 0x14) = (s32)OVMGR_U32(arg2, 0x8);

    anim.bits_0_1 = 1;
    anim.unk_1C   = 0x1D;
    anim.unk_20   = 0x20;
    anim.unk_22   = 5;
    anim.unk_26   = 0x1E;
    anim.unk_28   = 0x1F;

    anim.posX       = 0x80;
    anim.posY       = (s16)table_48[OVMGR_U8(arg1, 0x10)] + 0x60;
    anim.unk_02.raw = (u16)((anim.unk_02.raw & ~0xC00) | 0x800);
    anim.animIndex  = (OVMGR_S32(arg1, 0xC) != 0) ? 2 : 1;
    if (_Sprite_Load((Sprite*)((u8*)arg1 + 0x20), &anim) == 0) {
        OS_WaitForever();
    }

    anim.posX       = 0x80;
    anim.posY       = (s16)table_40[OVMGR_U8(arg1, 0x10)] + 0x60;
    anim.unk_02.raw = (u16)((anim.unk_02.raw & ~0xC00) | 0x400);
    anim.animIndex  = (OVMGR_S32(arg1, 0xC) != 0) ? 3 : table_38[OVMGR_U8(arg1, 0x11)];
    if (_Sprite_Load((Sprite*)((u8*)arg1 + 0x60), &anim) == 0) {
        OS_WaitForever();
    }

    anim.posX       = 0x8C;
    anim.posY       = (s16)table_30[OVMGR_U8(arg1, 0x10)] + 0x60;
    anim.unk_02.raw = (u16)((anim.unk_02.raw & ~0xC00) | 0x400);
    if (OVMGR_S32(arg1, 0xC) == 0) {
        anim.animIndex = table_20[OVMGR_U8(arg1, 0x12)];
    } else {
        anim.animIndex = table_28[OVMGR_U8(arg1, 0x12)];
    }
    if (_Sprite_Load((Sprite*)((u8*)arg1 + 0xA0), &anim) == 0) {
        OS_WaitForever();
    }

    anim.posX = 0x8C;
    anim.posY = (s16)table_18[OVMGR_U8(arg1, 0x10)] + 0x60;
    if (OVMGR_S32(arg1, 0xC) == 0) {
        anim.animIndex = table_8[OVMGR_U8(arg1, 0x11)];
    } else {
        anim.animIndex = table_10[OVMGR_U8(arg1, 0x11)];
    }

    var_r0_5                   = OVMGR_U16(arg1, 0x14);
    var_r1                     = 0;
    *((s8*)((u8*)arg1 + 0x1A)) = 0;
    if (var_r0_5 != 0) {
        do {
            temp_r5_3                           = var_r0_5 >> 0x1F;
            *((s8*)((u8*)arg1 + 0x1A + var_r1)) = (s8)(var_r0_5 - (0xA * (temp_r5_3 + (var_r0_5 / 10))));
            var_r0_5                            = temp_r5_3 + (var_r0_5 / 10);
            var_r1 += 1;
        } while (var_r0_5 != 0);
    }
    if (var_r1 == 0) {
        var_r1 = 1;
    }
    OVMGR_U16(arg1, 0x18) = var_r1;
    var_r9                = 0;
    anim.unk_02.raw       = (u16)((anim.unk_02.raw & ~0xC00) | 0x400);
    if ((u32)OVMGR_U16(arg1, 0x18) <= 0U) {
        return;
    }
loop_36:
    temp_r0_3 = *((u8*)arg1 + 0x1A + var_r9);
    anim.posX = (s16)table_78[var_r9] + 0x80;
    anim.posY = (s16)table_unk18[OVMGR_U8(arg1, 0x10)] + 0x60;
    if (OVMGR_S32(arg1, 0xC) != 0) {
        anim.animIndex = table_50.data[temp_r0_3];
    } else {
        anim.animIndex = table_64.data[temp_r0_3];
    }
    if (_Sprite_Load((Sprite*)((u8*)arg1 + 0xE0 + (var_r9 << 6)), &anim) == 0) {
        OS_WaitForever();
    }
    temp_r0_4 = var_r9 + 1;
    temp_hi   = (u32)OVMGR_U16(arg1, 0x18) > (u32)temp_r0_4;
    var_r9    = temp_r0_4;
    if (!temp_hi) {
        return;
    }
    goto loop_36;
}

void func_ov002_0208f5d8(PrcCtx* ctx, void* arg1) {
    u16 temp_r0_2;
    u16 var_r5;
    u32 temp_r0;

    Sprite_Destroy(arg1 + 0x20);
    Sprite_Destroy(arg1 + 0x60);
    Sprite_Destroy(arg1 + 0xA0);
    var_r5 = 0;
    if ((u32)OVMGR_U16(arg1, 0x18) <= 0U) {
        return;
    }
    do {
        Sprite_Destroy(arg1 + 0xE0 + (var_r5 << 6));
        temp_r0_2 = var_r5 + 1;
        temp_r0   = temp_r0_2 << 0x10;
        var_r5    = temp_r0_2;
    } while ((u32)OVMGR_U16(arg1, 0x18) > (u32)(temp_r0 >> 0x10));
}

void func_ov002_0208f630(PrcCtx* ctx, void* arg1) {
    u16 temp_r0_2;
    u16 var_r5;
    u32 temp_r0;

    Sprite_UpdateAndCheck(arg1 + 0x20);
    Sprite_UpdateAndCheck(arg1 + 0x60);
    Sprite_UpdateAndCheck(arg1 + 0xA0);
    var_r5 = 0;
    if ((u32)OVMGR_U16(arg1, 0x18) <= 0U) {
        return;
    }
    do {
        Sprite_UpdateAndCheck(arg1 + 0xE0 + (var_r5 << 6));
        temp_r0_2 = var_r5 + 1;
        temp_r0   = temp_r0_2 << 0x10;
        var_r5    = temp_r0_2;
    } while ((u32)OVMGR_U16(arg1, 0x18) > (u32)(temp_r0 >> 0x10));
}

void func_ov002_0208f688(PrcCtx* ctx, void* arg1) {
    u16 temp_r0_2;
    u16 var_r5;
    u32 temp_r0;

    Sprite_Render(arg1 + 0x20);
    Sprite_Render(arg1 + 0x60);
    Sprite_Render(arg1 + 0xA0);
    var_r5 = 0;
    if ((u32)OVMGR_U16(arg1, 0x18) <= 0U) {
        return;
    }
    do {
        Sprite_Render(arg1 + 0xE0 + (var_r5 << 6));
        temp_r0_2 = var_r5 + 1;
        temp_r0   = temp_r0_2 << 0x10;
        var_r5    = temp_r0_2;
    } while ((u32)OVMGR_U16(arg1, 0x18) > (u32)(temp_r0 >> 0x10));
}

PrcStepResult func_ov002_0208f6e0(PrcCtx* ctx, void* unused) {
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_REPEAT;
}

PrcStepResult func_ov002_0208f6f0(PrcCtx* ctx, void* unused) {
    return PRC_STEP_CONTINUE;
}
