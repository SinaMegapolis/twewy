#include "OtosuMenuShared.h"

static PrcStepFn data_ov002_0209301c[] = {func_ov002_0208caa0};

PrcFrameDesc data_ov002_02093020 = {
    .enter     = func_ov002_0208c9c8,
    .stepTable = data_ov002_0209301c,
    .update    = func_ov002_0208ca80,
    .render    = func_ov002_0208ca90,
    .exit      = func_ov002_0208ca70,
};

SpriteFrameInfo* func_ov002_0208c92c(Sprite* sprite, s32 arg, s32 mode) {
    Sprite_FrameInfoCallback(sprite, mode);
}

static const SpriteAnimation data_ov002_0209219c = {
    .bits_0_1          = 0,
    .dataType          = 0,
    .bit_6             = 0,
    .bits_7_9          = 5,
    .bits_10_11        = 0,
    .bits_12_13        = 1,
    .bits_14_15        = 0,
    .unk_02            = {.raw = 0x0400},
    .posX              = -13,
    .posY              = 0x000C,
    .frameInfoCallback = func_ov002_0208c92c,
    .callbackArg       = 0,
    .owner             = NULL,
    .binIden           = &data_ov002_02091acc,
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

void func_ov002_0208c9c8(void* arg0, void* arg1, s16* arg2) {
    SpriteAnimation anim = data_ov002_0209219c;

    anim.bits_0_1  = 0;
    anim.animIndex = 1;
    anim.posX      = 0x80;
    anim.posY      = *arg2;
    anim.unk_1C    = 0x11;
    anim.unk_20    = 0x14;
    anim.unk_22    = 2;
    anim.unk_26    = 0x12;
    anim.unk_28    = 0x13;

    if (_Sprite_Load((Sprite*)arg1, &anim) == 0) {
        OS_WaitForever();
    }
}

void func_ov002_0208ca70(PrcCtx* ctx, void* arg1) {
    Sprite_Destroy(arg1);
}

void func_ov002_0208ca80(PrcCtx* ctx, void* arg1) {
    Sprite_UpdateAndCheck(arg1);
}

void func_ov002_0208ca90(PrcCtx* ctx, void* arg1) {
    Sprite_Render(arg1);
}

PrcStepResult func_ov002_0208caa0(PrcCtx* ctx, void* unused) {
    return PRC_STEP_CONTINUE;
}

void func_ov002_0208caa8(OtosuMenuObj* menuObj) {
    u16 temp_r0_2;
    u16 var_r8;
    u32 temp_r0;

    PrcCtx* options[4];
    options[0] = &menuObj->unk_474E8;
    options[1] = &menuObj->unk_476D0;
    options[2] = &menuObj->unk_478B8;
    options[3] = &menuObj->unk_47AA0;

    var_r8 = 0;
    if ((s32)data_02074d10.unk_40B > 0) {
        do {
            PrcMaster_UnregisterContext(&menuObj->prcMaster, options[var_r8]);
            temp_r0_2 = var_r8 + 1;
            temp_r0   = temp_r0_2 << 0x10;
            var_r8    = temp_r0_2;
        } while ((s32)data_02074d10.unk_40B > (s32)(temp_r0 >> 0x10));
    }
    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->unk_47C88);
}

void func_ov002_0208cb50(OtosuMenuObj* menuObj) {
    s32 temp_gt;
    u16 temp_r11;
    u16 temp_r11_2;
    u16 temp_r7;
    u16 var_lr;
    u16 var_r4;
    u16 var_r7;
    u16 var_r5;
    u32 temp_r7_2;
    u32 var_r6;
    u32 var_r8;
    u32 var_r9;
    u8* temp_r10;

    var_r9             = 0;
    menuObj->unk_41FE4 = 0;
    var_r6             = 0xFFFF;
    var_r5             = 0;
    var_r4             = 0;
    if ((s32)data_02074d10.unk_40B <= 0) {
        return;
    }
loop_3:
    var_r8 = 0;
    var_r7 = 0xFFFF;
    var_lr = 0;
    if ((s32)data_02074d10.unk_40B > 0) {
        do {
            if (!(var_r9 & (1 << var_lr))) {
                temp_r11 = data_02071d10.unk3414[var_lr];
                if (var_r8 <= (u32)temp_r11) {
                    var_r8 = (u32)temp_r11;
                    var_r7 = var_lr;
                }
            }
            temp_r11_2 = var_lr + 1;
            temp_gt    = (s32)data_02074d10.unk_40B > (s32)temp_r11_2;
            var_lr     = temp_r11_2;
        } while (temp_gt);
    }
    if ((var_r6 != 0xFFFF) && (var_r8 != var_r6)) {
        var_r5 = (u16)(var_r5 + 1);
        if (var_r4 == 1) {
            menuObj->unk_41FE4 = 1;
        }
    }
    temp_r10                                 = (u8*)menuObj + var_r4;
    var_r6                                   = var_r8;
    *(u8*)(temp_r10 + 0x41FD9)               = (u8)var_r5;
    *(u8*)((u8*)menuObj->unk_41FDD + var_r7) = (u8)var_r5;
    menuObj->unk_41FD5                       = (s8)var_r7;
    temp_r7                                  = var_r4 + 1;
    temp_r7_2                                = temp_r7 << 0x10;
    var_r9 |= 1 << var_r7;
    var_r4 = temp_r7;
    if ((s32)data_02074d10.unk_40B <= (s32)(temp_r7_2 >> 0x10)) {
        return;
    }
    goto loop_3;
}

void func_ov002_0208cc5c(OtosuMenuObj* menuObj) {
    s32 temp_gt;
    s32 temp_gt_2;
    u16 temp_r2_2;
    u16 temp_r8;
    u16 temp_r8_2;
    u16 var_ip;
    u16 var_r3;
    u16 var_r4;
    u16 var_r9;
    u32 var_r6;
    u32 var_lr;
    u32 var_r5;
    u8  temp_r5;
    u8* temp_r2;

    menuObj->unk_41FE4 = 1;
    var_r6             = 0;
    var_ip             = 0;
    var_r3             = 0;
    var_lr             = 0xFFFF;
    if ((s32)data_02074d10.unk_40B > 0) {
    loop_1:
        var_r5 = 0;
        var_r9 = 0;
        var_r4 = 0xFFFF;
        if ((s32)data_02074d10.unk_40B > 0) {
            do {
                if (!(var_r6 & (1 << var_r9))) {
                    temp_r8 = data_02071d10.unk3414[var_r9];
                    if (var_r5 <= (u32)temp_r8) {
                        var_r5 = (u32)temp_r8;
                        var_r4 = var_r9;
                    }
                }
                temp_r8_2 = var_r9 + 1;
                temp_gt   = (s32)data_02074d10.unk_40B > (s32)temp_r8_2;
                var_r9    = temp_r8_2;
            } while (temp_gt);
        }
        if ((var_lr != 0xFFFF) && (var_r5 != var_lr)) {
            var_ip = (u16)(var_ip + 1);
        } else if ((var_r3 == 1) && (var_r4 == 0)) {
            var_ip = (u16)(var_ip + 1);
        }
        temp_r2                                  = (u8*)menuObj + var_r3;
        temp_r2_2                                = var_r3 + 1;
        *(u8*)(temp_r2 + 0x41FD9)                = (u8)var_ip;
        *(u8*)((u8*)menuObj->unk_41FDD + var_r4) = (u8)var_ip;
        *(s8*)(temp_r2 + 0x41FD5)                = (s8)var_r4;
        var_lr                                   = var_r5;
        var_r6 |= 1 << var_r4;
        temp_gt_2 = (s32)data_02074d10.unk_40B > (s32)temp_r2_2;
        var_r3    = temp_r2_2;
        if (!temp_gt_2) {
            goto block_16;
        }
        goto loop_1;
    }
block_16:
    if (menuObj->unk_41FD6 != 0) {
        return;
    }
    temp_r5 = menuObj->unk_41FD5;
    if (data_02075110.unk14 == data_02075124[temp_r5]) {
        menuObj->unk_41FD6 = temp_r5;
        menuObj->unk_41FD5 = 0U;
    }
}

s32 func_ov002_0208cdb0(OtosuMenuObj* menuObj, u16 arg1) {
    u8* name      = &menuObj->unk_41838[(&menuObj->unk_41FD5)[arg1] * 0x30];
    u32 unused[5] = {0, 0, 0, 0, 0};

    return (s32)name;
}
