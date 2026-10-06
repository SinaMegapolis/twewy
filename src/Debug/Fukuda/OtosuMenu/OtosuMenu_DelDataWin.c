#include "OtosuMenuShared.h"

static PrcStepFn data_ov002_02093494[] = {func_ov002_02091970};

PrcFrameDesc data_ov002_02093498 = {
    .enter     = func_ov002_020917fc,
    .stepTable = data_ov002_02093494,
    .update    = func_ov002_02091918,
    .render    = func_ov002_02091944,
    .exit      = func_ov002_020918ec,
};

static const Ov002_S16_4x8 data_ov002_02092a78 = {
    {
     {0x000C, 0x0002, 0x0001, 0x0003, 0x0004, 0x0000, 0xFFF6, 0x0001},
     {0x000D, 0x0002, 0x0001, 0x0003, 0x0004, 0x0000, 0xFFF6, 0x0001},
     {0x000D, 0x0002, 0x0001, 0x0003, 0x0004, 0x0000, 0x000A, 0x0001},
     {0x000C, 0x0002, 0x0001, 0x0003, 0x0004, 0x0000, 0x000A, 0x0002},
     }
}; /* const */

SpriteFrameInfo* func_ov002_02091760(Sprite* sprite, s32 arg, s32 mode) {
    Sprite_FrameInfoCallback(sprite, mode);
}

static const SpriteAnimation data_ov002_02092a4c = {
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
    .frameInfoCallback = func_ov002_02091760,
    .callbackArg       = 0,
    .owner             = NULL,
    .binIden           = (BinIdentifier*)&data_ov002_02091c34,
    .unk_18            = 0x0002,
    .packIndex         = 0x000D,
    .unk_1C            = 0x0002,
    .unk_1E            = 0x0000,
    .unk_20            = 0x0001,
    .unk_22            = 0x0002,
    .unk_24            = 0x0000,
    .unk_26            = 0x0003,
    .unk_28            = 0x0004,
    .animIndex         = 0x0001,
};

void func_ov002_020917fc(PrcCtx* ctx, Sprite* sprites) {
    SpriteAnimation anim = data_ov002_02092a4c;
    Ov002_S16_4x8   rows = data_ov002_02092a78;
    u16             i;

    for (i = 0; i < 4; i++) {
        anim.unk_1C    = rows.data[i][0];
        anim.unk_20    = rows.data[i][1];
        anim.unk_22    = rows.data[i][2];
        anim.unk_26    = rows.data[i][3];
        anim.unk_28    = rows.data[i][4];
        anim.posX      = rows.data[i][5] + 0x80;
        anim.posY      = rows.data[i][6] + 0x60;
        anim.animIndex = rows.data[i][7];
        if (_Sprite_Load(&sprites[i], &anim) == 0) {
            OS_WaitForever();
        }
    }
}

void func_ov002_020918ec(s32 arg0, s32 arg1) {
    for (u16 var_r4 = 0; var_r4 < 4; var_r4++) {
        Sprite_Destroy(arg1 + (var_r4 << 6));
    }
}

void func_ov002_02091918(s32 arg0, s32 arg1) {
    for (u16 var_r4 = 0; var_r4 < 4; var_r4++) {
        Sprite_UpdateAndCheck(arg1 + (var_r4 << 6));
    }
}

void func_ov002_02091944(s32 arg0, s32 arg1) {
    for (u16 var_r4 = 0; var_r4 < 4; var_r4++) {
        Sprite_Render(arg1 + (var_r4 << 6));
    }
}

PrcStepResult func_ov002_02091970(PrcCtx* ctx, void* arg1) {
    return PRC_STEP_CONTINUE;
}
