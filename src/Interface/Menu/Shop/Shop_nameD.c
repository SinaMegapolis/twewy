#include "Interface/Menu/Shop.h"

extern void func_ov043_020af42c(Sprite* sprite, s16 frame);

typedef struct {
    /* 0x000 */ Sprite      sprites[8];
    /* 0x200 */ BOOL        visible[8];
    /* 0x220 */ ShopObject* shop;
} Shop_nameD; // Size: 0x224

typedef struct {
    /* 0x0 */ s32         dataType;
    /* 0x4 */ ShopObject* shop;
} Shop_nameD_Args;

SpriteFrameInfo* func_ov043_020b2e70(Sprite* sprite, s32 arg, s32 mode);
s32              func_ov043_020b31f0(TaskPool* pool, Task* task, void* args, s32 stage);
s32              Shop_nameD_CreateTask(TaskPool* pool, s32 dataType, ShopObject* shop);

static const TaskHandle Tsk_Shop_nameD = {"Tsk_Shop_nameD", func_ov043_020b31f0, sizeof(Shop_nameD)};

static const SpriteAnimation data_ov043_020ca39c = {
    .bits_0_1          = 0,
    .dataType          = 0,
    .bit_6             = 0,
    .bits_7_9          = 5,
    .bits_10_11        = 0,
    .bits_12_13        = 1,
    .bits_14_15        = 0,
    .unk_02.raw        = 0xC00,
    .posX              = 0x50,
    .posY              = 0x50,
    .frameInfoCallback = func_ov043_020b2e70,
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

void func_ov043_020b2b48(Shop_nameD* name) {
    ShopObject* shop = name->shop;

    u16 state = shop->slots[shop->unk_848].unk_04;

    if (state == 0) {
        if (shop->unk_854 == 0) {
            func_ov043_020af42c(&name->sprites[4], 15);
            func_ov043_020af42c(&name->sprites[6], 16);
            func_ov043_020af42c(&name->sprites[7], 17);
            name->visible[2] = FALSE;
            name->visible[4] = TRUE;
            name->visible[5] = FALSE;
            name->visible[6] = TRUE;
            name->visible[7] = TRUE;
        } else if (shop->unk_854 == 1) {
            func_ov043_020af42c(&name->sprites[2], 5);
            name->visible[2] = TRUE;
            name->visible[4] = FALSE;
            name->visible[5] = FALSE;
            name->visible[6] = FALSE;
            name->visible[7] = FALSE;
        } else if (shop->unk_854 == 2) {
            func_ov043_020af42c(&name->sprites[2], 6);
            name->visible[2] = TRUE;
            name->visible[4] = FALSE;
            name->visible[5] = FALSE;
            name->visible[6] = FALSE;
            name->visible[7] = FALSE;
        } else if (shop->unk_854 == 3) {
            func_ov043_020af42c(&name->sprites[2], 9);
            name->visible[2] = TRUE;
            name->visible[4] = FALSE;
            name->visible[5] = FALSE;
            name->visible[6] = FALSE;
            name->visible[7] = FALSE;
        } else {
            func_ov043_020af42c(&name->sprites[2], 10);
            name->visible[2] = TRUE;
            name->visible[4] = FALSE;
            name->visible[5] = FALSE;
            name->visible[6] = FALSE;
            name->visible[7] = FALSE;
        }
    } else if (state == 1) {
        if (shop->unk_854 == 0) {
            func_ov043_020af42c(&name->sprites[4], 64);
            func_ov043_020af42c(&name->sprites[5], 12);
            func_ov043_020af42c(&name->sprites[6], 13);
            func_ov043_020af42c(&name->sprites[7], 14);
            name->visible[2] = FALSE;
            name->visible[4] = TRUE;
            name->visible[5] = TRUE;
            name->visible[6] = TRUE;
            name->visible[7] = TRUE;
        } else if (shop->unk_854 == 1) {
            func_ov043_020af42c(&name->sprites[2], 4);
            name->visible[2] = TRUE;
            name->visible[4] = FALSE;
            name->visible[5] = FALSE;
            name->visible[6] = FALSE;
            name->visible[7] = FALSE;
        } else if (shop->unk_854 == 2) {
            func_ov043_020af42c(&name->sprites[2], 8);
            name->visible[2] = TRUE;
            name->visible[4] = FALSE;
            name->visible[5] = FALSE;
            name->visible[6] = FALSE;
            name->visible[7] = FALSE;
        } else {
            func_ov043_020af42c(&name->sprites[2], 10);
            name->visible[2] = TRUE;
            name->visible[4] = FALSE;
            name->visible[5] = FALSE;
            name->visible[6] = FALSE;
            name->visible[7] = FALSE;
        }
    } else if (state == 2) {
        if (shop->unk_854 == 0) {
            func_ov043_020af42c(&name->sprites[2], 4);
            name->visible[2] = TRUE;
            name->visible[4] = FALSE;
            name->visible[5] = FALSE;
            name->visible[6] = FALSE;
            name->visible[7] = FALSE;
        } else if (shop->unk_854 == 1) {
            func_ov043_020af42c(&name->sprites[2], 7);
            name->visible[2] = TRUE;
            name->visible[4] = FALSE;
            name->visible[5] = FALSE;
            name->visible[6] = FALSE;
            name->visible[7] = FALSE;
        } else {
            func_ov043_020af42c(&name->sprites[2], 0xA);
            name->visible[2] = TRUE;
            name->visible[4] = FALSE;
            name->visible[5] = FALSE;
            name->visible[6] = FALSE;
            name->visible[7] = FALSE;
        }
    } else if (shop->unk_854 == 0) {
        func_ov043_020af42c(&name->sprites[2], 4);
        name->visible[2] = TRUE;
        name->visible[4] = FALSE;
        name->visible[5] = FALSE;
        name->visible[6] = FALSE;
        name->visible[7] = FALSE;
    } else {
        func_ov043_020af42c(&name->sprites[2], 0xA);
        name->visible[2] = TRUE;
        name->visible[4] = FALSE;
        name->visible[5] = FALSE;
        name->visible[6] = FALSE;
        name->visible[7] = FALSE;
    }
}

SpriteFrameInfo* func_ov043_020b2e70(Sprite* sprite, s32 arg, s32 mode) {
    Sprite_FrameInfoCallback(sprite, mode);
}

void func_ov043_020b2f0c(Shop_nameD* name, Sprite* sprites, Shop_nameD_Args* args) {
    ShopObject* shop = name->shop;

    SpriteAnimation anim = data_ov043_020ca39c;
    u16 coords[8][2] = {
        { 81,  10},
        {  6, 119},
        {  6, 134},
        {235,  64},
        {120, 137},
        {120, 149},
        {120, 161},
        {120, 173},
    };
    s16 frameIndex[8];

    anim.dataType = args->dataType;

    for (s16 i = 0; i < 8; i++) {
        name->visible[i] = TRUE;
    }

    frameIndex[0] = 1;
    frameIndex[1] = 2;
    // Read in place: the store to name->visible[2] may alias shop, so mwcc re-reads the
    // slot state instead of caching it for the chain below.
    if (shop->slots[0].unk_04 <= 1) {
        frameIndex[2]    = 1;
        name->visible[2] = FALSE;
    } else {
        frameIndex[2] = 4;
    }
    frameIndex[3] = 11;

    if (shop->slots[0].unk_04 == 0) {
        frameIndex[4]    = 15;
        frameIndex[5]    = 1;
        name->visible[5] = FALSE;
        frameIndex[6]    = 16;
        frameIndex[7]    = 17;
    } else if (shop->slots[0].unk_04 == 1) {
        frameIndex[4] = 64;
        frameIndex[5] = 12;
        frameIndex[6] = 13;
        frameIndex[7] = 14;
    } else if (shop->slots[0].unk_04 == 2) {
        for (s16 i = 4; i <= 7; i++) {
            frameIndex[i]    = 1;
            name->visible[i] = FALSE;
        }
    } else {
        for (s16 i = 4; i <= 7; i++) {
            frameIndex[i]    = 1;
            name->visible[i] = FALSE;
        }
    }

    for (s16 i = 0; i < 8; i++) {
        anim.animIndex = frameIndex[i];
        anim.posX      = coords[i][0];
        anim.posY      = coords[i][1];
        _Sprite_Load(&sprites[i], &anim);
    }
}

s32 func_ov043_020b3118(TaskPool* pool, Task* task, void* args) {
    Shop_nameD*      name     = task->data;
    Shop_nameD_Args* taskArgs = args;

    name->shop = taskArgs->shop;
    func_ov043_020b2f0c(name, name->sprites, taskArgs);
    return 1;
}

s32 func_ov043_020b3138(TaskPool* pool, Task* task, void* args) {
    Shop_nameD* name = task->data;

    func_ov043_020b2b48(name);
    for (s16 i = 0; i < 8; i++) {
        Sprite_Update(&name->sprites[i]);
    }
    return 1;
}

s32 func_ov043_020b3174(TaskPool* pool, Task* task, void* args) {
    Shop_nameD* name = task->data;

    for (s16 i = 0; i < 8; i++) {
        if (name->visible[i]) {
            Sprite_RenderFrame(&name->sprites[i]);
        }
    }
    return 1;
}

s32 func_ov043_020b31bc(TaskPool* pool, Task* task, void* args) {
    Shop_nameD* name = task->data;

    for (s16 i = 0; i < 8; i++) {
        Sprite_Release(&name->sprites[i]);
    }
    return 1;
}

s32 func_ov043_020b31f0(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = func_ov043_020b3118,
        .update     = func_ov043_020b3138,
        .render     = func_ov043_020b3174,
        .cleanup    = func_ov043_020b31bc,
    };

    return stages.iter[stage](pool, task, args);
}

s32 Shop_nameD_CreateTask(TaskPool* pool, s32 dataType, ShopObject* shop) {
    Shop_nameD_Args args;

    args.dataType = dataType;
    args.shop     = shop;

    return EasyTask_CreateTask(pool, &Tsk_Shop_nameD, NULL, 0, NULL, &args);
}
