#include "Battle/Core/BtlCmdQueue.h"
#include "Battle/Core/BtlList.h"
#include "Combat/Core/Combat.h"

void BtlCmdQueue_Push(u16 value) {
    BattleCmdQueue*      state = g_CombatGlobal->cmdQueue;
    BattleCmdQueueEntry* entry;

    state->unk_08 = 0;
    entry         = BtlList_GetHead(&state->free);
    if (entry != NULL) {
        entry->value = value;
        BtlList_Remove(&state->free, &entry->node);
        BtlList_Append(&state->queued, &entry->node, entry);
        return;
    }
    entry = BtlList_GetTail(&state->queued);
    if (entry != NULL) {
        entry->value = value;
    }
}

static void BtlCmdQueue_Free(BattleCmdQueueEntry* entry) {
    BattleCmdQueue* state = g_CombatGlobal->cmdQueue;

    if (entry == NULL) {
        return;
    }
    entry->value = 0;
    BtlList_Remove(&state->queued, &entry->node);
    BtlList_Append(&state->free, &entry->node, entry);
}

void BtlCmdQueue_Clear(void) {
    BattleCmdQueueEntry* entry = BtlList_GetHead(&g_CombatGlobal->cmdQueue->queued);

    if (entry == NULL) {
        return;
    }
    do {
        BattleCmdQueueEntry* next = BtlList_GetNext(&entry->node);
        BtlCmdQueue_Free(entry);
        entry = next;
    } while (entry != NULL);
}

void BtlCmdQueue_SetDirection(s32 value) {
    g_CombatGlobal->cmdQueue->direction = value;
}

u16 BtlCmdQueue_Pop(void) {
    BattleCmdQueue*      state = g_CombatGlobal->cmdQueue;
    u16                  value = 0;
    BattleCmdQueueEntry* entry = BtlList_GetHead(&state->queued);

    if (entry != NULL) {
        value        = entry->value;
        entry->value = 0;
        BtlList_Remove(&state->queued, &entry->node);
        BtlList_Append(&state->free, &entry->node, entry);
    }
    return value;
}

u16 BtlCmdQueue_TakeDirection(void) {
    BattleCmdQueue* state = g_CombatGlobal->cmdQueue;
    u16             value = state->direction;

    state->direction = 0;
    return value;
}

s32 func_ov003_02086a64(void) {
    return g_CombatGlobal->cmdQueue->unk_08;
}

s32 func_ov003_02086a80(void) {
    if (g_CombatGlobal->cmdQueue->flags & 4) {
        return 1;
    }
    return 0;
}

void func_ov003_02086aa8(s32 value) {
    g_CombatGlobal->cmdQueue->unk_08 = value;
}

void func_ov003_02086ac4(s32 enabled) {
    BattleCmdQueue* state = g_CombatGlobal->cmdQueue;

    if (enabled != 0) {
        state->flags |= 4;
    } else {
        state->flags &= ~4;
    }
}

void func_ov003_02086af0(void) {
    g_CombatGlobal->cmdQueue->flags &= ~2;
}

/// Resets a sync gauge state and seeds its 16 pooled entries into the free list.
void BtlCmdQueue_Init(BattleCmdQueue* state) {
    s32                  i;
    BattleCmdQueueEntry* entry;

    g_CombatGlobal->cmdQueue = state;
    state->flags             = 0;
    state->unk_04            = 0;
    state->unk_06            = 0;
    state->unk_08            = 0;
    state->direction         = 0;
    BtlList_Init(&state->free);
    BtlList_Init(&state->queued);
    entry = state->entries;
    for (i = 0; i < 16; i++) {
        entry->value = 0;
        BtlList_Append(&state->free, &entry->node, entry);
        entry++;
    }
}
