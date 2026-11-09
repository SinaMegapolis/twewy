#include "Battle/Core/BtlActor.h"
#include "Battle/Core/BtlList.h"
#include "Combat/Core/BtlFeat.h"
#include "Combat/Core/CombatApi.h"
#include "Engine/Core/Memory.h"

/// The enemies saved for each screen.
typedef struct Ov003ActorSnapshots {
    /* 0x00 */ u16                 mainCount;
    /* 0x02 */ u16                 subCount;
    /* 0x04 */ Ov003ActorSnapshot* sub;
    /* 0x08 */ Ov003ActorSnapshot* main;
} Ov003ActorSnapshots; // Size: 0xC

static Ov003ActorSnapshots sFeatEnmSnapshots;

/// Count the number of active enemy units in the list.
static u16 BtlFeat_CountActiveEnm(BattleList* list) {
    u16              count = 0;
    Ov003EnemyActor* unit  = BtlList_GetHead(list);
    while (unit != NULL) {
        if ((unit->actor.flags & ACTOR_FLAG_DEFEATED) == 0)
            count++;
        unit = BtlList_GetNext(&unit->node);
    }
    return count;
}

/// Save the current state of all active enemy units in the list.
static u16 BtlFeat_SaveEnmList(BattleList* list, Ov003ActorSnapshot** snapshots) {
    u16                 index   = 0;
    u16                 count   = BtlFeat_CountActiveEnm(list);
    Ov003ActorSnapshot* entries = Mem_AllocHeapTail(&gDebugHeap, count * sizeof(*entries));
    Mem_SetSequence(&gDebugHeap, entries, "S_FeatEnm_SavePos");
    *snapshots            = entries;
    Ov003EnemyActor* unit = BtlList_GetHead(list);
    while (unit != NULL) {
        if (!(unit->actor.flags & ACTOR_FLAG_DEFEATED)) {
            entries[index].actor = unit->actor;
            entries[index].unit  = unit;
            index++;
        }
        unit = BtlList_GetNext(&unit->node);
    }
    return count;
}

/// Set a pending command for all active enemy units in the list.
static void BtlFeat_SetEnmListCommand(BattleList* list, BattleActorCommand command) {
    Ov003EnemyActor* unit = BtlList_GetHead(list);
    while (unit != NULL) {
        BtlActor_SetPendingCommand(&unit->actor, command);
        unit = BtlList_GetNext(&unit->node);
    }
}

/// Restore the state of enemy units from snapshots.
static void BtlFeat_RestoreEnmList(BattleList* list, Ov003ActorSnapshot* snapshots, u32 count) {
    for (u16 i = 0; i < count; i++) {
        Ov003EnemyActor* unit = BtlList_GetHead(list);
        while (unit != NULL) {
            if (snapshots[i].unit == unit) {
                unit->actor = snapshots[i].actor;
                break;
            }
            unit = BtlList_GetNext(&unit->node);
        }
    }
}

void BtlFeat_SaveEnmPos(void) {
    sFeatEnmSnapshots.mainCount = BtlFeat_SaveEnmList(&g_CombatGlobal->unk3D8AC, &sFeatEnmSnapshots.main);
    sFeatEnmSnapshots.subCount  = BtlFeat_SaveEnmList(&g_CombatGlobal->unk3D8A4, &sFeatEnmSnapshots.sub);
}

void BtlFeat_RestoreEnmPos(void) {
    if (sFeatEnmSnapshots.main != NULL) {
        BtlFeat_RestoreEnmList(&g_CombatGlobal->unk3D8AC, sFeatEnmSnapshots.main, sFeatEnmSnapshots.mainCount);
        Mem_Free(&gDebugHeap, sFeatEnmSnapshots.main);
        sFeatEnmSnapshots.main = NULL;
    }

    if (sFeatEnmSnapshots.sub != NULL) {
        BtlFeat_RestoreEnmList(&g_CombatGlobal->unk3D8A4, sFeatEnmSnapshots.sub, sFeatEnmSnapshots.subCount);
        Mem_Free(&gDebugHeap, sFeatEnmSnapshots.sub);
        sFeatEnmSnapshots.sub = NULL;
    }
}

void BtlFeat_HaltEnm(void) {
    BtlFeat_SetEnmListCommand(&g_CombatGlobal->unk3D8AC, ACTOR_CMD_FEAT);
    BtlFeat_SetEnmListCommand(&g_CombatGlobal->unk3D8A4, ACTOR_CMD_FEAT);
}

void BtlFeat_ForEachEnmSnapshot(s32 mode, Ov003SnapshotCallback callback, void* args) {
    Ov003ActorSnapshot* entries;
    u16                 count;
    if (mode == 1) {
        count   = sFeatEnmSnapshots.mainCount;
        entries = sFeatEnmSnapshots.main;
    } else {
        count   = sFeatEnmSnapshots.subCount;
        entries = sFeatEnmSnapshots.sub;
    }
    for (u16 i = 0; i < count; i++)
        callback(mode, &entries[i], args);
}
