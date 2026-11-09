#include "Combat/Core/BtlDbg.h"
#include "Battle/Core/BtlList.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Engine/Core/OamMgr.h"
#include "Engine/EasyTask.h"
#include <nitro/fx.h>
#include <nitro/g3.h>
#include <nitro/gx/gx_geometry.h>
#include <nitro/mi/cpumem.h>
#include <nitro/reg.h>

typedef struct BtlDbgRect {
    /* 0x00 */ s32            x;
    /* 0x04 */ s32            y;
    /* 0x08 */ s16            width;
    /* 0x0A */ s16            height;
    /* 0x0C */ s16            life; // frames left
    /* 0x0E */ u16            kind;
    /* 0x10 */ BattleListNode node;
} BtlDbgRect; // Size: 0x1C

typedef struct BtlDbg {
    /* 0x000 */ BtlDbgRect rects[0x40];
    /* 0x700 */ BattleList free;
    /* 0x708 */ BattleList shown;
} BtlDbg; // Size: 0x710

// todo: OamMgr to claim these
extern OamAffineParam data_02059ad4;
extern VtxXY          data_02059ab0[4];
extern TexCoordQuad   data_02059b14[];

static s32 BtlDbg_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

static const TaskHandle Tsk_BtlDbg = {"Tsk_BtlDbg", BtlDbg_RunTask, sizeof(BtlDbg)};

static s32 BtlDbg_TaskId;

static void BtlDbg_DrawQuad(s32 kind, s32 x, s32 y, s32 width, s32 height) {
    OamAffineParam* affine = &data_02059ad4;
    TexCoordQuad*   texCoord;
    VtxXY*          vertices;

    vertices = data_02059ab0;
    texCoord = data_02059b14;

    switch (kind) {
        case BTLDBG_KIND_ATTACK:
            REG_GFX_FIFO_VERTEX_COLOR = 0x1F;
            break;
        case BTLDBG_KIND_SCREEN:
            REG_GFX_FIFO_VERTEX_COLOR = 0x3DFF;
            break;
        case BTLDBG_KIND_HITBOX:
            REG_GFX_FIFO_VERTEX_COLOR = 0x7C00;
            break;
        case BTLDBG_KIND_FOOTPRINT:
        default:
            REG_GFX_FIFO_VERTEX_COLOR = 0x7FFF;
            break;
    }
    REG_GFX_FIFO_MATRIX_PUSH      = 0;
    REG_GFX_FIFO_MATRIX_TRANSLATE = x + ((width << 12) >> 1);
    REG_GFX_FIFO_MATRIX_TRANSLATE = y + ((height << 12) >> 1);
    REG_GFX_FIFO_MATRIX_TRANSLATE = 0x3E8000;

    s32 rotation = (affine->rotation >> 4) * 2;
    func_020370cc(FX_SinCosTable_[rotation], FX_SinCosTable_[rotation + 1]);
    G3_Scale(width * affine->scaleX, height * affine->scaleY, 0x1000);
    REG_GFX_FIFO_TEXTURE_PARAM   = 0x20000000;
    REG_GFX_FIFO_TEXTURE_PALETTE = 0;
    REG_GFX_FIFO_POLYGON_ATTR    = 0xC0;
    REG_GFX_FIFO_POLYGONS_BEGIN  = 1;
    GX_TexCoord(texCoord->vtx[0].s * width, texCoord->vtx[0].t * height);
    GX_Vtx16(vertices[0].x, vertices[0].y, 0x8000);
    GX_TexCoord(texCoord->vtx[1].s * width, texCoord->vtx[1].t * height);
    GX_Vtx16(vertices[1].x, vertices[1].y, 0x8000);
    GX_TexCoord(texCoord->vtx[2].s * width, texCoord->vtx[2].t * height);
    GX_Vtx16(vertices[2].x, vertices[2].y, 0x8000);
    GX_TexCoord(texCoord->vtx[3].s * width, texCoord->vtx[3].t * height);
    GX_Vtx16(vertices[3].x, vertices[3].y, 0x8000);
    REG_GFX_FIFO_POLYGONS_END = 0;
    REG_GFX_FIFO_MATRIX_POP   = 1;
    REG_GFX_FIFO_VERTEX_COLOR = 0x7FFF;
}

static void BtlDbg_DrawRect(BtlDbgRect* rect) {
    s32 x;
    s32 y;

    if (rect->kind == BTLDBG_KIND_SCREEN) {
        x = rect->x;
        y = rect->y;
    } else {
        x = BtlStage_ToScreenX(0, rect->x) << 12;
        y = BtlStage_ToScreenY(0, rect->y, 0) << 12;
    }
    BtlDbg_DrawQuad(rect->kind, x, y, rect->width, rect->height);
}

/// Moves a rectangle from the free list to the shown list.
static void BtlDbg_AddRect(BtlDbg* dbg, u16 kind, u16 life, s32 x, s32 y, s16 width, s16 height) {
    BtlDbgRect* rect = BtlList_GetHead(&dbg->free);

    if (rect != NULL) {
        rect->kind   = kind;
        rect->life   = life;
        rect->x      = x;
        rect->y      = y;
        rect->width  = width;
        rect->height = height;
        BtlList_Remove(&dbg->free, &rect->node);
        BtlList_Append(&dbg->shown, &rect->node, rect);
    }
}

static void BtlDbg_FreeRect(BtlDbg* dbg, BtlDbgRect* rect) {
    if (rect != NULL) {
        BtlList_Remove(&dbg->shown, &rect->node);
        BtlList_Append(&dbg->free, &rect->node, rect);
    }
}

static s32 BtlDbg_Init(TaskPool* pool, Task* task, void* args) {
    BtlDbg* dbg = task->data;

    MI_CpuSet(dbg, 0, sizeof(*dbg));
    BtlList_Init(&dbg->free);
    BtlList_Init(&dbg->shown);
    for (s32 i = 0; i < 0x40; i++) {
        BtlList_Append(&dbg->free, &dbg->rects[i].node, &dbg->rects[i]);
    }
    return 1;
}

static s32 BtlDbg_Update(TaskPool* pool, Task* task, void* args) {
    return 1;
}

static s32 BtlDbg_Render(TaskPool* pool, Task* task, void* args) {
    BtlDbg* dbg = task->data;

    BtlDbgRect* rect = BtlList_GetHead(&dbg->shown);
    while (rect != NULL) {
        BtlDbg_DrawRect(rect);
        if (rect->life == 0) {
            BtlDbgRect* next = BtlList_GetNext(&rect->node);
            BtlDbg_FreeRect(dbg, rect);
            rect = next;
        } else {
            rect->life--;
            rect = BtlList_GetNext(&rect->node);
        }
    }
    return 1;
}

static s32 BtlDbg_Release(TaskPool* pool, Task* task, void* args) {
    return 1;
}

static s32 BtlDbg_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlDbg_Init,
        .update     = BtlDbg_Update,
        .render     = BtlDbg_Render,
        .cleanup    = BtlDbg_Release,
    };
    return stages.iter[stage](pool, task, args);
}

void BtlDbg_CreateTask(void) {
    BtlDbg_TaskId = EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &Tsk_BtlDbg, NULL, 0, NULL, NULL);
}

void BtlDbg_PostRect(u16 kind, u16 life, s32 x, s32 y, s16 width, s16 height) {
    BtlDbg* dbg = EasyTask_GetTaskData(&g_CombatGlobal->unk_10118, BtlDbg_TaskId);

    if (BtlDebug_GetSetting(BTL_DEBUG_SETTING_DISP_AREA) != BTL_DEBUG_DISP_AREA_OFF) {
        BtlDbg_AddRect(dbg, kind, life, x, y, width, height);
    }
}
