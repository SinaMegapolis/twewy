#include "Battle/Core/BtlCmdQueue.h"
#include "Battle/Core/BtlList.h"
#include "Battle/Core/BtlObsMgr.h"

void BtlCmdQueue_Push(u16 value) {
    BattleCmdQueue*      queue = gBattle->cmdQueue;
    BattleCmdQueueEntry* entry;

    queue->partnerReady = FALSE;
    entry               = BtlList_GetHead(&queue->free);
    if (entry != NULL) {
        entry->value = value;
        BtlList_Remove(&queue->free, &entry->node);
        BtlList_Append(&queue->queued, &entry->node, entry);
    } else {
        entry = BtlList_GetTail(&queue->queued);
        if (entry != NULL) {
            entry->value = value;
        }
    }
}

/// Moves a queued entry back to the free list.
static void BtlCmdQueue_Free(BattleCmdQueueEntry* entry) {
    BattleCmdQueue* queue = gBattle->cmdQueue;

    if (entry != NULL) {
        entry->value = 0;
        BtlList_Remove(&queue->queued, &entry->node);
        BtlList_Append(&queue->free, &entry->node, entry);
    }
}

void BtlCmdQueue_Clear(void) {
    BattleCmdQueueEntry* entry = BtlList_GetHead(&gBattle->cmdQueue->queued);

    while (entry != NULL) {
        BattleCmdQueueEntry* next = BtlList_GetNext(&entry->node);
        BtlCmdQueue_Free(entry);
        entry = next;
    }
}

void BtlCmdQueue_SetDirection(s32 direction) {
    gBattle->cmdQueue->direction = direction;
}

u16 BtlCmdQueue_Pop(void) {
    BattleCmdQueue*      queue = gBattle->cmdQueue;
    u16                  value = 0;
    BattleCmdQueueEntry* entry = BtlList_GetHead(&queue->queued);

    if (entry != NULL) {
        value        = entry->value;
        entry->value = 0;
        BtlList_Remove(&queue->queued, &entry->node);
        BtlList_Append(&queue->free, &entry->node, entry);
    }
    return value;
}

u16 BtlCmdQueue_TakeDirection(void) {
    BattleCmdQueue* queue     = gBattle->cmdQueue;
    u16             direction = queue->direction;

    queue->direction = 0;
    return direction;
}

BOOL BtlCmdQueue_IsPartnerReady(void) {
    return gBattle->cmdQueue->partnerReady;
}

BOOL BtlCmdQueue_IsPartnerJumping(void) {
    if (gBattle->cmdQueue->flags & BTL_CMD_QUEUE_FLAG_PARTNER_JUMPING) {
        return TRUE;
    }
    return FALSE;
}

void BtlCmdQueue_SetPartnerReady(BOOL ready) {
    gBattle->cmdQueue->partnerReady = ready;
}

void BtlCmdQueue_SetPartnerJumping(BOOL jumping) {
    BattleCmdQueue* queue = gBattle->cmdQueue;

    if (jumping) {
        queue->flags |= BTL_CMD_QUEUE_FLAG_PARTNER_JUMPING;
    } else {
        queue->flags &= ~BTL_CMD_QUEUE_FLAG_PARTNER_JUMPING;
    }
}

void func_ov003_02086af0(void) {
    gBattle->cmdQueue->flags &= ~BTL_CMD_QUEUE_FLAG_UNK_1;
}

void BtlCmdQueue_Init(BattleCmdQueue* queue) {
    s32                  i;
    BattleCmdQueueEntry* entry;

    gBattle->cmdQueue   = queue;
    queue->flags        = 0;
    queue->unk_04       = 0;
    queue->unk_06       = 0;
    queue->partnerReady = FALSE;
    queue->direction    = 0;
    BtlList_Init(&queue->free);
    BtlList_Init(&queue->queued);
    entry = queue->entries;
    for (i = 0; i < 16; i++) {
        entry->value = 0;
        BtlList_Append(&queue->free, &entry->node, entry);
        entry++;
    }
}
