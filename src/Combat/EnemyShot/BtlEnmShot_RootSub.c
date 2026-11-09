#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatEnemyShot.h"
#include <nitro/mi/cpumem.h>

static const TaskHandle data_ov003_020e0c98;

s32 func_ov003_020d1228(Ov003EnemyShotSource* source, BattleList* list, u16 angle, s32 x, s32 y, s32 z, s32 scaleX,
                        s32 scaleY) {
    Ov003EnemyShotRootArgs args;
    MI_CpuSet(&args, 0, sizeof(args));
    args.source     = source;
    args.list       = list;
    args.angle      = angle;
    args.position.x = x;
    args.position.y = y;
    args.position.z = z;
    args.scaleX     = scaleX;
    args.scaleY     = scaleY;
    return EasyTask_CreateTask(&g_CombatGlobal->unk_00000, &data_ov003_020e0c98, NULL, 0, NULL, &args);
}

static s32 func_ov003_020d12b4(Ov003EnemyShotRoot* self, Ov003EnemyShotRootArgs* args) {
    const Ov003EnemyShotVisual* visual = args->source->visual;
    MI_CpuSet(self, 0, sizeof(*self));
    self->angle      = args->angle;
    self->position.x = args->position.x;
    self->position.y = args->position.y;
    self->position.z = args->position.z;
    self->active     = 1;
    self->scaleX     = args->scaleX;
    self->scaleY     = args->scaleY;
    BtlList_Append(args->list, &self->node, self);
    if (visual != NULL) {
        const SpriteAnimEntry* entry = &visual->entries[visual->entry];
        BtlSprite_LoadDirect(2, &self->sprite, visual->file, entry->charDataIndex, entry->animDataIndex, entry->cellDataIndex,
                             visual->paletteEntry, entry->animIndex);
        BtlSprite_SetAnim(&self->sprite, args->source->animation, args->source->animationMode);
    }
    func_ov003_020cccf0(0, args->source);
    return 1;
}

static s32 func_ov003_020d1398(Ov003EnemyShotRoot* self) {
    if (func_ov003_020c3c28())
        return 0;
    if (self->sprite.sprite.resourceData != NULL) {
        BtlSprite_Update(&self->sprite);
        if (SpriteMgr_IsAnimationFinished(&self->sprite.sprite)) {
            BtlSprite_Release(&self->sprite);
            self->sprite.sprite.resourceData = NULL;
        }
    }
    return self->active;
}

static s32 func_ov003_020d13ec(Ov003EnemyShotRoot* self) {
    if (self->sprite.sprite.resourceData != NULL) {
        s32 scaleX  = self->scaleX;
        s32 xOffset = 0;
        s32 yOffset = 0;
        s32 scaleY  = self->scaleY;
        if (self->angle == 0x4000 || self->angle == 0xC000) {
            scaleX = self->scaleY;
            scaleY = self->scaleX;
        }
        switch (self->angle) {
            case 0x4000:
                xOffset = -1;
                break;
            case 0x8000:
                yOffset = -1;
                break;
        }
        s16 x, y;
        BtlStage_WorldToScreen(0, &x, &y, self->position.x, self->position.y, self->position.z);
        BtlSprite_SetPosition(&self->sprite, (s16)(x + xOffset), (s16)(y + yOffset));
        BtlSprite_SetAffineTransform(&self->sprite, self->angle, scaleX, scaleY, 1);
        BtlSprite_SetSortKey(&self->sprite, 0);
        BtlSprite_Render(&self->sprite);
    }
    return 1;
}

static s32 func_ov003_020d14cc(Ov003EnemyShotRoot* self) {
    if (self->sprite.sprite.resourceData != NULL)
        BtlSprite_Release(&self->sprite);
    return 1;
}

static s32 func_ov003_020d14e8(TaskPool* pool, Task* task, void* args, s32 stage) {
    Ov003EnemyShotRoot* self = task->data;
    switch (stage) {
        case 0:
            return func_ov003_020d12b4(self, args);
        case 1:
            return func_ov003_020d1398(self);
        case 2:
            return func_ov003_020d13ec(self);
        case 3:
            return func_ov003_020d14cc(self);
        default:
            return 1;
    }
}

static char             data_ov003_020e6d14[24] = "Tsk_BtlEnmShot_RootSub";
static const TaskHandle data_ov003_020e0c98     = {data_ov003_020e6d14, func_ov003_020d14e8, sizeof(Ov003EnemyShotRoot)};
