#include "Interface/Menu/Shop.h"

extern void func_ov043_020af42c(Sprite* sprite, s16 frame);

typedef struct {
    /* 0x000 */ Sprite      sprites[5];
    /* 0x140 */ BOOL        visible[5];
    /* 0x154 */ ShopObject* shop;
} Shop_numAblU; // Size: 0x158

typedef struct {
    /* 0x00 */ u32         dataType;
    /* 0x04 */ ShopObject* shop;
} Shop_numAblU_Args;

SpriteFrameInfo* Shop_numAblU_GetFrameInfo(Sprite* sprite, s32 arg, s32 mode);
s32              Shop_numAblU_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

const TaskHandle Tsk_Shop_numAblU = {"Tsk_Shop_numAblU", Shop_numAblU_RunTask, sizeof(Shop_numAblU)};

static void func_ov043_020bb2f0(Shop_numAblU* num) {
    ShopObject* shop = num->shop;
    u32         digitL[2];
    u32         digitR[2];
    s32         offsetX2;
    s32         offsetX;
    u16         i;

    digitL[0] = (u32)shop->unk_7B4 / 10;
    digitL[1] = (u32)shop->unk_7B4 % 10;
    digitR[0] = (u32)shop->unk_7B5 / 10;
    digitR[1] = (u32)shop->unk_7B5 % 10;

    for (i = 0; i < 5; i++) {
        num->visible[i] = TRUE;
    }

    if (digitL[0] == 0) {
        num->visible[0] = FALSE;
        num->visible[1] = TRUE;
        offsetX         = -3;
    } else {
        num->visible[0] = TRUE;
        num->visible[1] = TRUE;
        offsetX         = 0;
    }

    if (digitR[0] == 0) {
        num->visible[3] = FALSE;
        num->visible[4] = TRUE;
        offsetX2        = -3;
    } else {
        num->visible[3] = TRUE;
        num->visible[4] = TRUE;
        offsetX2        = 0;
    }

    for (i = 0; i < 2; i++) {
        num->sprites[i].posX = offsetX + (i * 6 + 110);
        num->sprites[i].posY = 183;
        func_ov043_020af42c(&num->sprites[i], digitL[i] + 10);
    }

    num->sprites[2].posX = 122;
    num->sprites[2].posY = 183;
    func_ov043_020af42c(&num->sprites[2], 22);

    for (i = 0; i < 2; i++) {
        num->sprites[i + 3].posX = offsetX2 + ((i + 3) * 6 + 110);
        num->sprites[i + 3].posY = 183;
        func_ov043_020af42c(&num->sprites[i + 3], digitR[i] + 10);
    }
}

SpriteFrameInfo* Shop_numAblU_GetFrameInfo(Sprite* sprite, s32 arg, s32 mode) {
    Sprite_FrameInfoCallback(sprite, mode);
}

const SpriteAnimation data_ov043_020caba8 = {
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
    .frameInfoCallback = Shop_numAblU_GetFrameInfo,
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

static void Shop_numAblU_Load(Shop_numAblU* num, Sprite* sprites, Shop_numAblU_Args* numArgs) {
    ShopObject*     shop = num->shop;
    SpriteAnimation anim  = data_ov043_020caba8;
    u32             digitL[2];
    u32             digitR[2];
    s32             offsetX2;
    s32             offsetX;
    u16             i;

    anim.dataType = numArgs->dataType;

    digitL[0] = (u32)shop->unk_7B4 / 10;
    digitL[1] = (u32)shop->unk_7B4 % 10;
    digitR[0] = (u32)shop->unk_7B5 / 10;
    digitR[1] = (u32)shop->unk_7B5 % 10;

    for (i = 0; i < 5; i++) {
        num->visible[i] = TRUE;
    }

    if (digitL[0] == 0) {
        num->visible[0] = FALSE;
        num->visible[1] = TRUE;
        offsetX         = -3;
    } else {
        num->visible[0] = TRUE;
        num->visible[1] = TRUE;
        offsetX         = 0;
    }

    if (digitR[0] == 0) {
        num->visible[3] = FALSE;
        num->visible[4] = TRUE;
        offsetX2        = -3;
    } else {
        num->visible[3] = TRUE;
        num->visible[4] = TRUE;
        offsetX2        = 0;
    }

    for (i = 0; i < 2; i++) {
        anim.animIndex = digitL[i] + 10;
        anim.posX      = offsetX + (i * 6 + 110);
        anim.posY      = 183;
        _Sprite_Load(&sprites[i], &anim);
    }

    anim.animIndex = 22;
    anim.posX      = 122;
    anim.posY      = 183;
    _Sprite_Load(&sprites[2], &anim);

    for (i = 0; i < 2; i++) {
        anim.animIndex = digitR[i] + 10;
        anim.posX      = offsetX2 + ((i + 3) * 6 + 110);
        anim.posY      = 183;
        _Sprite_Load(&sprites[i + 3], &anim);
    }
}

s32 Shop_numAblU_Init(TaskPool* pool, Task* task, void* args) {
    Shop_numAblU*      num     = task->data;
    Shop_numAblU_Args* numArgs = args;

    num->shop = numArgs->shop;
    Shop_numAblU_Load(num, num->sprites, numArgs);
    return 1;
}

s32 Shop_numAblU_Update(TaskPool* pool, Task* task, void* args) {
    Shop_numAblU* num = task->data;

    func_ov043_020bb2f0(num);
    for (s32 i = 0; i < 5; i++) {
        Sprite_Update(&num->sprites[i]);
    }
    return 1;
}

s32 Shop_numAblU_Render(TaskPool* pool, Task* task, void* args) {
    Shop_numAblU* num = task->data;

    for (s32 i = 0; i < 5; i++) {
        if (num->visible[i]) {
            Sprite_RenderFrame(&num->sprites[i]);
        }
    }
    return 1;
}

s32 Shop_numAblU_Destroy(TaskPool* pool, Task* task, void* args) {
    Shop_numAblU* num = task->data;

    for (s32 i = 0; i < 5; i++) {
        Sprite_Release(&num->sprites[i]);
    }
    return 1;
}

const TaskStages data_ov043_020cab98 = {
    .initialize = Shop_numAblU_Init,
    .update     = Shop_numAblU_Update,
    .render     = Shop_numAblU_Render,
    .cleanup    = Shop_numAblU_Destroy,
};

s32 Shop_numAblU_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = data_ov043_020cab98;
    return stages.iter[stage](pool, task, args);
}

s32 Shop_numAblU_CreateTask(TaskPool* pool, s32 arg1, ShopObject* shop) {
    Shop_numAblU_Args args;
    args.dataType = arg1;
    args.shop     = shop;
    return EasyTask_CreateTask(pool, &Tsk_Shop_numAblU, NULL, 0, NULL, &args);
}
