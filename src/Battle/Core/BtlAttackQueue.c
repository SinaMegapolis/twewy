#include "Battle/Core/BtlAttackQueue.h"
#include "Battle/Core/BtlList.h"
#include "Battle/Core/BtlObsMgr.h"

void BtlAttackQueue_Push(Ov003EffectState* state, BattleActor* target, s32 variant) {
    BattleAttackQueue* queue = &gBattle->attackQueue;

    if (target != NULL) {
        BattleQueuedAttack* attack = BtlList_GetHead(&queue->free);

        if (attack != NULL) {
            attack->state   = state;
            attack->target  = target;
            attack->variant = variant;
            BtlList_Remove(&queue->free, &attack->node);
            BtlList_Append(&queue->active, &attack->node, attack);
        }
    }
}

void BtlAttackQueue_Remove(BattleQueuedAttack* attack) {
    BattleAttackQueue* queue = &gBattle->attackQueue;

    if (attack != NULL && attack->target != NULL) {
        attack->unk_00  = 0;
        attack->state   = NULL;
        attack->target  = NULL;
        attack->variant = 0;
        BtlList_Remove(&queue->active, &attack->node);
        BtlList_Append(&queue->free, &attack->node, attack);
    }
}

void BtlAttackQueue_Clear(void) {
    BattleQueuedAttack* attack = BtlList_GetHead(&gBattle->attackQueue.active);

    while (attack != NULL) {
        BattleQueuedAttack* next = BtlList_GetNext(&attack->node);
        BtlAttackQueue_Remove(attack);
        attack = next;
    }
}

BattleQueuedAttack* BtlAttackQueue_GetHead(void) {
    return BtlList_GetHead(&gBattle->attackQueue.active);
}

void BtlAttackQueue_Init(void) {
    BattleAttackQueue*  queue = &gBattle->attackQueue;
    s32                 i;
    BattleQueuedAttack* attack;

    BtlList_Init(&queue->free);
    BtlList_Init(&queue->active);
    attack = queue->entries;
    for (i = 0; i < 8; i++) {
        attack->unk_00  = 0;
        attack->state   = NULL;
        attack->target  = NULL;
        attack->variant = 0;
        BtlList_Append(&queue->free, &attack->node, attack);
        attack++;
    }
}

void BtlAttackQueue_DropInvalid(void) {
    BattleQueuedAttack* attack = BtlList_GetHead(&gBattle->attackQueue.active);

    while (attack != NULL) {
        s32 flags = attack->target->flags;

        if ((flags & ACTOR_FLAG_DEFEATED) || (flags & ACTOR_FLAG_INTANGIBLE) || (flags & ACTOR_FLAG_LAUNCHED)) {
            BattleQueuedAttack* next = BtlList_GetNext(&attack->node);
            BtlAttackQueue_Remove(attack);
            attack = next;
        } else {
            attack = BtlList_GetNext(&attack->node);
        }
    }
}
