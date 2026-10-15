#include "Interface/Menu/Shop.h"

typedef struct {
    /* 0x000 */ Sprite      unk_000[10];
    /* 0x280 */ BOOL        unk_280[10];
    /* 0x2A8 */ ShopObject* unk_2A8;
    /* 0x2AC */ u8          unk_2AC;
    /* 0x2AD */ char        unk_2AD[0x2B0 - 0x2AD];
} Shop_numMoneyNek; // Size: 0x2B0

typedef struct {
    /* 0x0 */ s32         dataType;
    /* 0x4 */ ShopObject* shop;
    /* 0x8 */ u32         unk_8;
} Shop_numMoneyNek_Args;

extern void func_ov043_020af42c(Sprite* sprite, s16 frame);

static const Point data_ov043_020ca42c[5] = {
    {-10,  0},
    { 10, 16},
    { 22, 32},
    { 38, 44},
    {  5, 27},
};

SpriteFrameInfo* func_ov043_020b3684(Sprite* sprite, s32 arg, s32 mode);
s32              func_ov043_020b394c(TaskPool* arg0, Task* arg1, void* arg2, s32 arg3);

static const TaskHandle Tsk_Shop_numMoneyNek = {"Tsk_Shop_numMoneyNek", func_ov043_020b394c, 0x2B0};

static const SpriteAnimation data_ov043_020ca440 = {
    .bits_0_1          = 0,
    .dataType          = 0,
    .bit_6             = 0,
    .bits_7_9          = 5,
    .bits_10_11        = 0,
    .bits_12_13        = 1,
    .bits_14_15        = 0,
    .unk_02.raw        = 0x800,
    .posX              = 0x50,
    .posY              = 0x50,
    .frameInfoCallback = func_ov043_020b3684,
    .callbackArg       = 0,
    .owner             = NULL,
    .binIden           = &data_ov043_020ca1b0,
    .unk_18            = 0,
    .packIndex         = 0,
    .unk_1C            = 1,
    .unk_1E            = 0,
    .unk_20            = 4,
    .unk_22            = 3,
    .unk_24            = 0,
    .unk_26            = 2,
    .unk_28            = 3,
    .animIndex         = 1,
};

void func_ov043_020b34a0(Shop_numMoneyNek* arg0, u32 arg1, u32* arg2) {
    u32 temp = arg1;
    s32 i;

    arg2[0] = temp / 1000000;
    temp %= 1000000;
    arg2[1] = temp / 100000;
    temp %= 100000;
    arg2[2] = temp / 10000;
    temp %= 10000;
    arg2[3] = temp / 1000;
    temp %= 1000;
    arg2[4] = temp / 100;
    temp %= 100;
    arg2[5] = temp / 10;
    arg2[6] = temp % 10;

    for (i = 0; i < 10; i++) {
        arg0->unk_280[i] = TRUE;
    }

    arg0->unk_2AC = 7;
    for (i = 0; i < 6; i++) {
        if (arg2[i] != 0) {
            break;
        }
        arg0->unk_280[i + 1] = FALSE;
        arg0->unk_2AC--;
    }

    if (arg1 < 1000000) {
        arg0->unk_280[8] = FALSE;
        if (arg1 < 1000) {
            arg0->unk_280[9] = FALSE;
        }
    }
}

void func_ov043_020b3628(Shop_numMoneyNek* arg0) {
    u32 digits[7];
    u16 i;

    func_ov043_020b34a0(arg0, *(u32*)((u8*)arg0->unk_2A8 + 0x858), digits);
    for (i = 0; i < 7; i++) {
        func_ov043_020af42c(&arg0->unk_000[i + 1], (s16)(digits[i] + 0x1D));
    }
}

void func_ov043_020b3720(Shop_numMoneyNek* arg0, Sprite* arg1, Shop_numMoneyNek_Args* arg2) {
    SpriteAnimation anim = data_ov043_020ca440;
    Point           positions[5] = {
        {-10,  0},
        { 10, 16},
        { 22, 32},
        { 38, 44},
        {  5, 27},
    };
    u32             digits[7];
    u16             i;

    anim.dataType = arg2->dataType;
    func_ov043_020b34a0(arg0, arg2->unk_8, digits);

    anim.animIndex = 0x27;
    anim.posX      = positions[0].x + 0x93;
    anim.posY      = 9;
    _Sprite_Load(&arg1[0], &anim);

    for (i = 0; i < 7; i++) {
        anim.animIndex = (s16)(digits[i] + 0x1D);
        anim.posX      = ((s16*)positions)[i + 1] + 0x93;
        anim.posY      = 9;
        _Sprite_Load(&arg1[i + 1], &anim);
    }

    anim.animIndex = 0x45;
    anim.posX      = positions[4].x + 0x93;
    anim.posY      = 9;
    _Sprite_Load(&arg1[8], &anim);

    anim.animIndex = 0x45;
    anim.posX      = positions[4].y + 0x93;
    anim.posY      = 9;
    _Sprite_Load(&arg1[9], &anim);
}

SpriteFrameInfo* func_ov043_020b3684(Sprite* sprite, s32 arg, s32 mode) {
    Sprite_FrameInfoCallback(sprite, mode);
}

s32 func_ov043_020b388c(TaskPool* arg0, Task* arg1, void* arg2) {
    Shop_numMoneyNek*      temp_r0 = arg1->data;
    Shop_numMoneyNek_Args* args    = arg2;

    temp_r0->unk_2A8 = args->shop;
    func_ov043_020b3720(temp_r0, temp_r0->unk_000, args);
    return 1;
}

s32 func_ov043_020b38ac(TaskPool* arg0, Task* arg1, void* arg2) {
    Shop_numMoneyNek* var_r5 = arg1->data;
    s32               var_r4;

    func_ov043_020b3628(var_r5);
    var_r4 = 0;
    do {
        Sprite_Update(&var_r5->unk_000[var_r4]);
        var_r4 += 1;
    } while (var_r4 < 0xA);
    return 1;
}

s32 func_ov043_020b38e0(TaskPool* arg0, Task* arg1, void* arg2) {
    Shop_numMoneyNek* temp_r4 = arg1->data;
    s32               var_r5;

    var_r5 = 0;
    do {
        if (temp_r4->unk_280[var_r5] != 0) {
            Sprite_RenderFrame(&temp_r4->unk_000[var_r5]);
        }
        var_r5 += 1;
    } while (var_r5 < 0xA);
    return 1;
}

s32 func_ov043_020b3920(TaskPool* arg0, Task* arg1, void* arg2) {
    Shop_numMoneyNek* var_r5 = arg1->data;
    s32               var_r4;

    var_r4 = 0;
    do {
        Sprite_Release(&var_r5->unk_000[var_r4]);
        var_r4 += 1;
    } while (var_r4 < 0xA);
    return 1;
}

s32 func_ov043_020b394c(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = func_ov043_020b388c,
        .update     = func_ov043_020b38ac,
        .render     = func_ov043_020b38e0,
        .cleanup    = func_ov043_020b3920,
    };
    return stages.iter[stage](pool, task, args);
}

s32 Shop_numMoneyNek_CreateTask(TaskPool* pool, s32 dataType, ShopObject* shop) {
    Shop_numMoneyNek_Args args;

    args.dataType = dataType;
    args.shop     = shop;
    args.unk_8    = shop->unk_858;
    return EasyTask_CreateTask(pool, &Tsk_Shop_numMoneyNek, NULL, 0, NULL, &args);
}
