#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatPsychEffects.h"
#include <nitro/mi/cpumem.h>

static const TaskHandle data_ov003_020dc978;

void func_ov003_020b3050(CombatList* list, Ov003TargetEntry* entry) {
    if (entry == NULL)
        return;
    func_ov003_020a6060(entry->taskId);
    CombatList_Remove(list, &entry->node);
}

void func_ov003_020b3078(CombatList* list) {
    Ov003TargetEntry* entry = func_ov003_020b3094(list);
    func_ov003_020b3050(list, entry);
}

Ov003TargetEntry* func_ov003_020b3094(CombatList* list) {
    return CombatList_GetHead(list);
}

Ov003TargetEntry* func_ov003_020b30a0(CombatList* list, CombatActor* actor) {
    Ov003TargetEntry* entry = CombatList_GetHead(list);
    while (entry != NULL) {
        if (entry->actor == actor)
            return entry;
        entry = CombatList_GetNext(&entry->node);
    }
    return NULL;
}

s32 func_ov003_020b30d8(CombatList* list) {
    return func_ov003_020b3094(list) == NULL;
}

s32 func_ov003_020b30f0(CombatList* list, CombatActor* actor) {
    return func_ov003_020b30a0(list, actor) != NULL;
}

s32 func_ov003_020b3108(Ov003EffectState* effect, CombatList* targets, s32* finished) {
    Ov003ThunderArgs args;
    args.effect   = effect;
    args.targets  = targets;
    args.finished = finished;
    return func_ov003_020a6000(&data_ov003_020dc978, &args);
}

static s32 func_ov003_020b3134(TaskPool* pool, Task* task, void* args, s32 stage) {
    Ov003ThunderArgs* arg = args;
    Ov003Thunder*     self;
    switch (stage) {
        default:
        case 0:
            self = task->data;
            MI_CpuSet(self, 0, sizeof(*self));
            self->args.effect   = arg->effect;
            self->args.targets  = arg->targets;
            self->args.finished = arg->finished;
            self->speed         = self->args.effect->params[3];
            func_ov003_020b33a4(self);
            return 1;
        case 1:
            self = task->data;
            switch (self->phase) {
                case 4:
                    return 0;
                case 0:
                    func_ov003_020b3400(self);
                    break;
                case 3:
                    func_ov003_020b371c(self);
                    break;
                case 1:
                    func_ov003_020b34c0(self);
                    break;
                case 2:
                    func_ov003_020b362c(self);
                    break;
            }
            CombatSprite_Update(&self->sprite);
            return 1;
        case 2:
            self = task->data;
            CombatSprite_SetPosition(&self->sprite, (s16)(self->position.x >> 12), (s16)(self->position.y >> 12));
            CombatSprite_SetAffineTransform(&self->sprite, (u16)(self->angle - 0x4000), 0x1000, 0x1000, 0);
            CombatSprite_Render(&self->sprite);
            return 1;
        case 3:
            self = task->data;
            CombatSprite_Release(&self->sprite);
            *self->args.finished = 1;
            return 1;
    }
}

static char             data_ov003_020e4dc8[24] = "SubTsk_BtlPsy_Thunder";
static const TaskHandle data_ov003_020dc978     = {data_ov003_020e4dc8, func_ov003_020b3134, sizeof(Ov003Thunder)};
