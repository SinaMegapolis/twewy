#ifndef TWEWY_BATTLE_CORE_BTLMGR_H
#define TWEWY_BATTLE_CORE_BTLMGR_H

#include "Battle/Core/BtlActor.h"
#include "Combat/Core/CombatEnemyActor.h"
#include "Engine/Core/Memory.h"
#include "Engine/EasyTask.h"
#include "Engine/Resources/ResourceMgr.h"

/// Battle scene work area, allocated from `gDebugHeap` and referenced from the overlay dispatcher
/// (`MainOvlDisp_SetCbArg`) and the debug overlay stack.
typedef struct BattleMgrWork {
    /* 0x00000 */ TaskPool        taskPool;
    /* 0x00080 */ Heap            heap;
    /* 0x0008C */ u8              heapData[0x800];
    /* 0x0088C */ ResourceManager resources;
    /* 0x11E0C */ s32             active;         // tasks are only ticked while a battle is running
    /* 0x11E10 */ u16             step;           // result/continue state machine step
    /* 0x11E12 */ u16             musicId;        // BGM currently requested
    /* 0x11E14 */ u16             encounterId;    // noise id of the battle being fought
    /* 0x11E16 */ u16             battleDuration; // frames the current chained battle lasts
    /* 0x11E18 */ s16             fadeCounter;
    /* 0x11E1A */ s16             unk_11E1A;
    /* 0x11E1C */ s32             keepBattleState; // carry the level-up gauge into the next battle
} BattleMgrWork;                                   // Size: 0x11E20

void func_ov003_0208af9c(s32 time, s32 amount);
BOOL BtlMgr_AddDrop(u32 itemId);
void BtlMgr_SetState(u32 state);
void BtlMgr_MoveByAngle(s32* x, s32* y, s32 angle, s32 scale);
void BtlMgr_WorldToScreen(s16* screenX, s16* screenY, s32 x, s32 y, s32 z);
/// True for the two encounters that are allowed to take damage while the battle is paused.
BOOL             func_ov003_0208b690(u16 encounterId);
Ov003EnemyActor* BtlMgr_FindFarthestTarget(s32 x, s32 side, s32 airborne);
void             BtlMgr_GainExp(s32 mode, s16 exp);
void             BtlMgr_GainPP(s32 mode, s16 pp);
BOOL             BtlMgr_HasAbility(s32 mode, u32 effect);
s16              BtlMgr_GetAbilityValue(s32 mode, u32 effect);
s32              func_ov003_0208ca54(s32 includePins);
s32              func_ov003_0208cae0(s32 includePins);
void             func_ov003_0208cb6c(s32 kind, s32 damage);
/// Bumps the battle's combo-length counter.
void func_ov003_0208cbac(void);
/// Extends the battle's longest combo.
void func_ov003_0208cbc8(void);

void             BtlMgr_AddExp(s32);
void             BtlMgr_AddTeamHp(s32, s16);
void             BtlMgr_AddUnitHp(s32, BattleActor*, s16);
BOOL             BtlMgr_IsTargetOnSide(Ov003EnemyActor*, s32);
Ov003EnemyActor* BtlMgr_FindNearestTarget(s32, s32, s32);
void             ProcessOverlay_Battle(BattleMgrWork*);

#endif // TWEWY_BATTLE_CORE_BTLMGR_H
