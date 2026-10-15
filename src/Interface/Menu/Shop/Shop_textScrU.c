#include "Interface/Menu/Shop.h"
#include "Util/SysFont.h"
#include "common_data.h"

typedef struct {
    /* 0x000 */ ShopObject* shop;
    /* 0x004 */ SysFont     fonts[3];
} Shop_textScrU; // Size: 0x178

typedef struct {
    /* 0x0 */ s32         dataType;
    /* 0x4 */ ShopObject* shop;
} Shop_textScrU_Args;

s32 Shop_textScrU_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

static const TaskHandle Tsk_Shop_textScrU;

void func_ov043_020bc098(Shop_textScrU* textScr) {
    for (s32 i = 0; i < 3; i++) {
        SysFont_Init(&textScr->fonts[i]);
        SysFont_SetSpacing(&textScr->fonts[i], TRUE, 0);
    }
}

void func_ov043_020bc0d8(Shop_textScrU* textScr) {
    s32         i;
    ShopObject* shop         = textScr->shop;
    Point       positions[3] = {
        {36,   8},
        {36,  21},
        {36, 153},
    };

    u16* map      = shop->resources[1].screenMap;
    u16* charData = shop->resources[1].charData;

    if (map == NULL || charData == NULL) {
        OS_WaitForever();
    }

    SysFont_SetMsg(&textScr->fonts[0], shop->unk_768 + 0x2FE7);
    SysFont_SetMsg(&textScr->fonts[1], shop->unk_768 + 0x2FB8);
    SysFont_SetMsg(&textScr->fonts[2], shop->unk_774 + 0x3016);

    SysFont_SetHAlign(&textScr->fonts[0], 0, 108);
    SysFont_SetHAlign(&textScr->fonts[1], 0, 108);
    SysFont_SetHAlign(&textScr->fonts[2], 0, 107);

    for (i = 0; i < 3; i++) {
        SysFont_SetColor(&textScr->fonts[i], 14);
        SysFont_SetPos(&textScr->fonts[i], positions[i].x, positions[i].y);
        SysFont_DrawCurrentToScreen(&textScr->fonts[i], map + 2, charData + 2, 0);
    }
}

s32 Shop_textScrU_Init(TaskPool* pool, Task* task, void* args) {
    Shop_textScrU*      textScr     = task->data;
    Shop_textScrU_Args* textScrArgs = args;

    textScr->shop = textScrArgs->shop;
    func_ov043_020bc098(textScr);
    func_ov043_020bc0d8(textScr);
    return 1;
}

s32 Shop_textScrU_Update(TaskPool* pool, Task* task, void* args) {
    return 1;
}

s32 Shop_textScrU_Render(TaskPool* pool, Task* task, void* args) {
    return 1;
}

s32 Shop_textScrU_Destroy(TaskPool* pool, Task* task, void* args) {
    Shop_textScrU* textScr = task->data;

    for (s32 i = 0; i < 3; i++) {
        SysFont_Destroy(&textScr->fonts[i]);
    }
    return 1;
}

s32 Shop_textScrU_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = Shop_textScrU_Init,
        .update     = Shop_textScrU_Update,
        .render     = Shop_textScrU_Render,
        .cleanup    = Shop_textScrU_Destroy,
    };
    return stages.iter[stage](pool, task, args);
}

s32 Shop_textScrU_CreateTask(TaskPool* pool, s32 dataType, ShopObject* shop) {
    Shop_textScrU_Args args;
    args.dataType = dataType;
    args.shop     = shop;
    return EasyTask_CreateTask(pool, &Tsk_Shop_textScrU, NULL, 0, NULL, &args);
}

static const TaskHandle Tsk_Shop_textScrU = {"Tsk_Shop_textScrU", Shop_textScrU_RunTask, sizeof(Shop_textScrU)};
