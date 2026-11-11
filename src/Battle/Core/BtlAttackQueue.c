#include "Battle/Core/BtlAttackQueue.h"
#include "Battle/Core/BtlList.h"
#include "Combat/Core/Combat.h"

/// Pushes a battle actor towards `x`/`y`/`z` through the shared action-effect pool.
void func_ov003_02087250(Ov003EffectState* state, BattleActor* target, s32 angle) {
    BattleAttackQueue*  pool = &g_CombatGlobal->attackQueue;
    BattleQueuedAttack* effect;

    if (target == NULL) {
        return;
    }
    effect = BtlList_GetHead(&pool->free);
    if (effect == NULL) {
        return;
    }
    effect->state  = state;
    effect->target = target;
    effect->angle  = angle;
    BtlList_Remove(&pool->free, &effect->node);
    BtlList_Append(&pool->active, &effect->node, effect);
}

void func_ov003_020872b4(BattleQueuedAttack* effect) {
    BattleAttackQueue* pool = &g_CombatGlobal->attackQueue;

    if (effect == NULL || effect->target == NULL) {
        return;
    }
    effect->unk_00 = 0;
    effect->state  = NULL;
    effect->target = NULL;
    effect->angle  = 0;
    BtlList_Remove(&pool->active, &effect->node);
    BtlList_Append(&pool->free, &effect->node, effect);
}

void func_ov003_02087310(void) {
    BattleQueuedAttack* effect = BtlList_GetHead(&g_CombatGlobal->attackQueue.active);

    if (effect == NULL) {
        return;
    }
    do {
        BattleQueuedAttack* next = BtlList_GetNext(&effect->node);
        func_ov003_020872b4(effect);
        effect = next;
    } while (effect != NULL);
}

BattleQueuedAttack* func_ov003_02087358(void) {
    return BtlList_GetHead(&g_CombatGlobal->attackQueue.active);
}

void func_ov003_02087378(void) {
    BattleAttackQueue*  pool = &g_CombatGlobal->attackQueue;
    s32                 i;
    BattleQueuedAttack* effect;

    BtlList_Init(&pool->free);
    BtlList_Init(&pool->active);
    effect = pool->entries;
    for (i = 0; i < 8; i++) {
        effect->unk_00 = 0;
        effect->state  = NULL;
        effect->target = NULL;
        effect->angle  = 0;
        BtlList_Append(&pool->free, &effect->node, effect);
        effect++;
    }
}

/// Retires the action effects whose target actor is gone or defeated.
void func_ov003_020873e0(void) {
    BattleQueuedAttack* effect = BtlList_GetHead(&g_CombatGlobal->attackQueue.active);

    if (effect == NULL) {
        return;
    }
    do {
        s32 flags = effect->target->flags;

        if ((flags & ACTOR_FLAG_DEFEATED) || (flags & ACTOR_FLAG_INTANGIBLE) || (flags & ACTOR_FLAG_LAUNCHED)) {
            BattleQueuedAttack* next = BtlList_GetNext(&effect->node);
            func_ov003_020872b4(effect);
            effect = next;
        } else {
            effect = BtlList_GetNext(&effect->node);
        }
    } while (effect != NULL);
}
