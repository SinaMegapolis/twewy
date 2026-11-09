#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatPsychEffects.h"
#include <nitro/mi/cpumem.h>

static const TaskHandle data_ov003_020dc96c;

static s32 func_ov003_020b2e30(CombatList* list, BattleActor* actor) {
    Ov003TargetArgs args;
    args.list  = list;
    args.actor = actor;
    return func_ov003_020a6000(&data_ov003_020dc96c, &args);
}

static s32 func_ov003_020b2e58(TaskPool* pool, Task* task, void* args, s32 stage) {
    Ov003TargetArgs* arg = args;
    Ov003Target*     self;
    switch (stage) {
        default:
        case 0:
            self = task->data;
            MI_CpuSet(self, 0, sizeof(*self));
            self->list         = arg->list;
            self->entry.taskId = ((u32)task->generation << 16) | (u32)task->id;
            self->entry.actor  = arg->actor;
            CombatList_Append(self->list, &self->entry.node, &self->entry);
            CombatSe_PlayAtMainX(0x14C, self->entry.actor->position.x);
            CombatPsyAnim_Load(&self->sprite, 2, 1, 0x10, 1);
            return 1;
        case 1:
            self = task->data;
            if (self->entry.actor == NULL) {
                CombatList_Remove(self->list, &self->entry.node);
                return 0;
            }
            if ((self->entry.actor->flags & ACTOR_FLAG_DEFEATED) || (self->entry.actor->flags & ACTOR_FLAG_INTANGIBLE)) {
                CombatList_Remove(self->list, &self->entry.node);
                return 0;
            }
            if (SpriteMgr_IsAnimationFinished(&self->sprite.sprite))
                CombatPsyAnim_SetAnim(&self->sprite, 0x11, 0);
            BtlSprite_Update(&self->sprite);
            return 1;
        case 2:
            self = task->data;
            BtlSprite_SetPosition(&self->sprite, self->entry.actor->screenX,
                                  (s16)(self->entry.actor->screenY - self->entry.actor->centerHeight));
            BtlSprite_Render(&self->sprite);
            return 1;
        case 3:
            self = task->data;
            BtlSprite_Release(&self->sprite);
            return 1;
    }
}

void func_ov003_020b2fd8(CombatList* list) {
    CombatList_Init(list);
}

void func_ov003_020b2fe4(CombatList* list) {
    if (func_ov003_020b30d8(list))
        return;
    do {
        func_ov003_020b3078(list);
    } while (!func_ov003_020b30d8(list));
}

void func_ov003_020b3014(CombatList* list, BattleActor* actor) {
    if (actor->flags & ACTOR_FLAG_DEFEATED)
        return;
    if (actor->flags & ACTOR_FLAG_INTANGIBLE)
        return;
    func_ov003_020b2e30(list, actor);
}

void func_ov003_020b3034(CombatList* list, BattleActor* actor) {
    Ov003TargetEntry* entry = func_ov003_020b30a0(list, actor);
    func_ov003_020b3050(list, entry);
}
static char             data_ov003_020e4db0[24] = "SubTask_BtlPsy_Target";
static const TaskHandle data_ov003_020dc96c     = {data_ov003_020e4db0, func_ov003_020b2e58, sizeof(Ov003Target)};
