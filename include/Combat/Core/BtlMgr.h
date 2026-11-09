#ifndef TWEWY_COMBAT_CORE_BTLMGR_H
#define TWEWY_COMBAT_CORE_BTLMGR_H

#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatEnemyActor.h"

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
void             ProcessOverlay_Battle(BtlMgrWork*);

#endif // TWEWY_COMBAT_CORE_BTLMGR_H
