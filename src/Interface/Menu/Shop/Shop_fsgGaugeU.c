#include "Engine/Core/OamMgr.h"
#include "Interface/Menu/Shop.h"

typedef struct {
    /* 0x00 */ Sprite      sprites[3];
    /* 0xC0 */ BOOL        visible[3];
    /* 0xCC */ ShopObject* shop;
    /* 0xD0 */ u16         rotation[3];
    /* 0xD6 */ char        unk_D6[0xD8 - 0xD6];
    /* 0xD8 */ s32         scaleX[3];
    /* 0xE4 */ s32         scaleY[3];
} Shop_fsgGaugeU; // Size: 0xF0

typedef struct {
    /* 0x0 */ s32         unk_0;
    /* 0x4 */ ShopObject* shop;
} Shop_fsgGaugeU_Args;

static s32 Shop_fsgGaugeU_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

SpriteFrameInfo* Shop_fsgGaugeU_GetFrameInfo(Sprite* sprite, s32 arg, s32 mode);

static const TaskHandle Tsk_Shop_fsgGaugeU = {"Tsk_Shop_fsgGaugeU", Shop_fsgGaugeU_RunTask, 0xF0};

extern BinIdentifier data_ov043_020ca1d8;

static const SpriteAnimation data_ov043_020cac38 = {
    .bits_0_1          = 1,
    .dataType          = 0,
    .bit_6             = 0,
    .bits_7_9          = 5,
    .bits_10_11        = 0,
    .bits_12_13        = 1,
    .bits_14_15        = 0,
    .unk_02.raw        = 0x400,
    .posX              = 0,
    .posY              = 0,
    .frameInfoCallback = Shop_fsgGaugeU_GetFrameInfo,
    .callbackArg       = 0,
    .owner             = NULL,
    .binIden           = &data_ov043_020ca1d8,
    .unk_18            = 1,
    .packIndex         = 0,
    .unk_1C            = 4,
    .unk_1E            = 2,
    .unk_20            = 0,
    .unk_22            = 2,
    .unk_24            = 3,
    .unk_26            = 1,
    .unk_28            = 0,
    .animIndex         = 0,
};

static void func_ov043_020bbcc8(Shop_fsgGaugeU* gauge, s32 index, u32 value) {
    if (value <= 10) {
        gauge->scaleX[index]  = 41;
        gauge->visible[index] = FALSE;
    } else {
        gauge->scaleX[index]  = FX_Divide(value << 12, 1000 << 12);
        gauge->visible[index] = TRUE;
    }
}

SpriteFrameInfo* Shop_fsgGaugeU_GetFrameInfo(Sprite* sprite, s32 arg, s32 mode) {
    Sprite_FrameInfoCallback(sprite, mode);
}

static void Shop_fsgGaugeU_Load(Shop_fsgGaugeU* gauge, Sprite* sprite, Shop_fsgGaugeU_Args* args) {
    ShopObject*     shop = gauge->shop;
    SpriteAnimation anim = data_ov043_020cac38;

    anim.dataType = args->unk_0;
    anim.unk_02.raw |= 2;
    anim.posX      = 40;
    anim.posY      = 171;
    anim.animIndex = 25;
    _Sprite_Load(&sprite[0], &anim);

    anim.posX      = 41;
    anim.posY      = 170;
    anim.animIndex = 4;
    _Sprite_Load(&sprite[1], &anim);

    anim.animIndex = 5;
    _Sprite_Load(&sprite[2], &anim);

    for (s32 i = 0; i < 3; i++) {
        gauge->rotation[i] = 0;
        gauge->scaleX[i]   = 4096;
        gauge->scaleY[i]   = 4096;
        gauge->visible[i]  = TRUE;
    }

    func_ov043_020bbcc8(gauge, 1, shop->unk_788);
    func_ov043_020bbcc8(gauge, 2, shop->unk_78A);
}

static s32 Shop_fsgGaugeU_Init(TaskPool* pool, Task* task, void* args) {
    Shop_fsgGaugeU*      gauge     = task->data;
    Shop_fsgGaugeU_Args* gaugeArgs = args;

    gauge->shop = gaugeArgs->shop;
    Shop_fsgGaugeU_Load(gauge, gauge->sprites, gaugeArgs);
    return 1;
}

static s32 Shop_fsgGaugeU_Update(TaskPool* pool, Task* task, void* args) {
    Shop_fsgGaugeU* gauge = task->data;
    ShopObject*     shop  = gauge->shop;

    func_ov043_020bbcc8(gauge, 1, shop->unk_788);
    func_ov043_020bbcc8(gauge, 2, shop->unk_78A);

    for (s32 i = 0; i < 3; i++) {
        Sprite_Update(&gauge->sprites[i]);
    }
    return 1;
}

static s32 Shop_fsgGaugeU_Render(TaskPool* pool, Task* task, void* args) {
    Shop_fsgGaugeU* gauge = task->data;

    for (s32 i = 0; i < 3; i++) {
        Sprite* sprite = &gauge->sprites[i];
        u16     affine =
            (u16)OamMgr_AllocAffineGroup(&g_OamMgr[DISPLAY_SUB], gauge->rotation[i], gauge->scaleX[i], gauge->scaleY[i], 0);

        sprite->unk_0A.raw = (sprite->unk_0A.raw & ~1) | 1;
        sprite->unk_0A.raw = (sprite->unk_0A.raw & ~0x3E0) | ((affine & 0x1F) << 5);

        if (gauge->visible[i] != 0) {
            Sprite_RenderFrame(sprite);
        }
    }
    return 1;
}

static s32 Shop_fsgGaugeU_Destroy(TaskPool* pool, Task* task, void* args) {
    Shop_fsgGaugeU* gauge = task->data;

    for (s32 i = 0; i < 3; i++) {
        Sprite_Release(&gauge->sprites[i]);
    }
    return 1;
}

static s32 Shop_fsgGaugeU_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = Shop_fsgGaugeU_Init,
        .update     = Shop_fsgGaugeU_Update,
        .render     = Shop_fsgGaugeU_Render,
        .cleanup    = Shop_fsgGaugeU_Destroy,
    };
    return stages.iter[stage](pool, task, args);
}

s32 Shop_fsgGaugeU_CreateTask(TaskPool* pool, s32 arg1, ShopObject* shop) {
    Shop_fsgGaugeU_Args args;
    args.unk_0 = arg1;
    args.shop  = shop;
    return EasyTask_CreateTask(pool, &Tsk_Shop_fsgGaugeU, NULL, 0, NULL, &args);
}
