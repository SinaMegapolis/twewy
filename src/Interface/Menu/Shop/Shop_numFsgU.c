#include "Interface/Menu/Shop.h"

extern void func_ov043_020af42c(Sprite* sprite, s16 frame);

typedef struct {
    /* 0x000 */ Sprite      sprites[5];
    /* 0x140 */ BOOL        visible[5];
    /* 0x154 */ ShopObject* shop;
} Shop_numFsgU; // Size: 0x158

typedef struct {
    /* 0x0 */ s32         unk_0;
    /* 0x4 */ ShopObject* shop;
    /* 0x8 */ u16         unk_8;
} Shop_numFsgU_Args;

SpriteFrameInfo* func_ov043_020bb7d4(Sprite* sprite, s32 arg, s32 mode);
s32              Shop_numFsgU_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

const TaskHandle Tsk_Shop_numFsgU = {"Tsk_Shop_numFsgU", Shop_numFsgU_RunTask, 0x158};

SpriteFrameInfo* func_ov043_020bb7d4(Sprite* sprite, s32 arg, s32 mode) {
    Sprite_FrameInfoCallback(sprite, mode);
}

void func_ov043_020bb870(Shop_numFsgU* num) {
    ShopObject* shop = num->shop;
    u32         digit[3];
    s16         posX[3];
    u32         value;
    u16         i;

    for (i = 0; i < 3; i++) {
        num->visible[i] = TRUE;
    }

    value = shop->unk_788;

    if (value == 1000) {
        digit[0] = 1;
        digit[1] = 0;
        digit[2] = 0;
        posX[0]  = 14;
        posX[1]  = 20;
        posX[2]  = 26;
        num->visible[3] = FALSE;
    } else {
        digit[0] = value / 100;
        digit[1] = (value % 100) / 10;
        digit[2] = (value % 100) % 10;
        posX[0]  = 11;
        posX[1]  = 17;
        posX[2]  = 26;
        if (value / 100 == 0) {
            num->visible[0] = FALSE;
        }
    }

    for (i = 0; i < 3; i++) {
        num->sprites[i].posX = posX[i] + 100;
        num->sprites[i].posY = 171;
        func_ov043_020af42c(&num->sprites[i], digit[i] + 10);
    }
}

const SpriteAnimation data_ov043_020cabf0 = {
    .bits_0_1          = 1,
    .dataType          = 0,
    .bit_6             = 0,
    .bits_7_9          = 5,
    .bits_10_11        = 0,
    .bits_12_13        = 1,
    .bits_14_15        = 0,
    .unk_02.raw        = 0x400,
    .posX              = 0x50,
    .posY              = 0x50,
    .frameInfoCallback = func_ov043_020bb7d4,
    .callbackArg       = 0,
    .owner             = NULL,
    .binIden           = &data_ov043_020ca1d8,
    .unk_18            = 0,
    .packIndex         = 0,
    .unk_1C            = 1,
    .unk_1E            = 0,
    .unk_20            = 4,
    .unk_22            = 2,
    .unk_24            = 0,
    .unk_26            = 2,
    .unk_28            = 3,
    .animIndex         = 1,
};

void func_ov043_020bb9b0(Shop_numFsgU* num, Sprite* sprites, Shop_numFsgU_Args* args) {
    SpriteAnimation anim  = data_ov043_020cabf0;
    u32             digit[3];
    s16             posX[3];
    u32             value;
    u16             i;

    anim.dataType = args->unk_0;

    for (i = 0; i < 5; i++) {
        num->visible[i] = TRUE;
    }

    value = args->unk_8;

    if (value == 1000) {
        digit[0] = 1;
        digit[1] = 0;
        digit[2] = 0;
        posX[0]  = 14;
        posX[1]  = 20;
        posX[2]  = 26;
        num->visible[3] = FALSE;
    } else {
        digit[0] = value / 100;
        digit[1] = (value % 100) / 10;
        digit[2] = (value % 100) % 10;
        posX[0]  = 11;
        posX[1]  = 17;
        posX[2]  = 26;
        if (value / 100 == 0) {
            num->visible[0] = FALSE;
        }
    }

    for (i = 0; i < 3; i++) {
        anim.animIndex = digit[i] + 10;
        anim.posX      = posX[i] + 100;
        anim.posY      = 171;
        _Sprite_Load(&sprites[i], &anim);
    }

    anim.animIndex = 20;
    anim.posX      = 121;
    anim.posY      = 171;
    _Sprite_Load(&sprites[3], &anim);

    anim.animIndex = 21;
    anim.posX      = 134;
    anim.posY      = 171;
    _Sprite_Load(&sprites[4], &anim);
}

s32 Shop_numFsgU_Init(TaskPool* pool, Task* task, void* args) {
    Shop_numFsgU*      num     = task->data;
    Shop_numFsgU_Args* numArgs = args;

    num->shop = numArgs->shop;
    func_ov043_020bb9b0(num, num->sprites, numArgs);
    return 1;
}

s32 Shop_numFsgU_Update(TaskPool* pool, Task* task, void* args) {
    Shop_numFsgU* num = task->data;

    func_ov043_020bb870(num);
    for (s32 i = 0; i < 5; i++) {
        Sprite_Update(&num->sprites[i]);
    }
    return 1;
}

s32 Shop_numFsgU_Render(TaskPool* pool, Task* task, void* args) {
    Shop_numFsgU* num = task->data;

    for (s32 i = 0; i < 5; i++) {
        if (num->visible[i])
            Sprite_RenderFrame(&num->sprites[i]);
    }
    return 1;
}

s32 Shop_numFsgU_Destroy(TaskPool* pool, Task* task, void* args) {
    Shop_numFsgU* num = task->data;

    for (s32 i = 0; i < 5; i++) {
        Sprite_Release(&num->sprites[i]);
    }
    return 1;
}

const TaskStages data_ov043_020cabe0 = {
    .initialize = Shop_numFsgU_Init,
    .update     = Shop_numFsgU_Update,
    .render     = Shop_numFsgU_Render,
    .cleanup    = Shop_numFsgU_Destroy,
};

s32 Shop_numFsgU_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = data_ov043_020cabe0;
    return stages.iter[stage](pool, task, args);
}

s32 Shop_numFsgU_CreateTask(TaskPool* arg0, s32 arg1, ShopObject* shop) {
    Shop_numFsgU_Args args;
    args.unk_0 = arg1;
    args.shop  = shop;
    args.unk_8 = shop->unk_788;
    return EasyTask_CreateTask(arg0, &Tsk_Shop_numFsgU, NULL, 0, NULL, &args);
}
