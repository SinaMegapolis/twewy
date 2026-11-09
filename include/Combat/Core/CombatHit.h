#ifndef TWEWY_COMBAT_CORE_COMBATHIT_H
#define TWEWY_COMBAT_CORE_COMBATHIT_H

#include "Battle/Core/BtlActor.h"
#include "Battle/Core/BtlSprite.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatAttack.h"
#include "Combat/Core/CombatEnemyActor.h"

void                   CombatHit_LoadSpark(s32, BattleSprite*, s32);
void                   CombatHit_DrawSpark(BattleSprite*, s32, s32);
void                   CombatHit_LoadSparkForUnit(s32, BattleSprite*, BattleActor*);
void                   CombatHit_DrawSparkOnActor(BattleSprite*, BattleActor*);
s32                    CombatHit_CanDrawSpark(void);
s32                    CombatHit_ApplyAttack(s32 mode, BattleActor* unit, u16 attackId);
s32                    CombatHit_Stagger(s32, BattleActor*, s32);
const Ov003AttackInfo* CombatHit_GetAttackInfo(u16);
s32                    CombatHit_AttackUnit(const Ov003AttackInfo*, BattleAttackArgs*, BattleActor*, s32);
s32                    CombatHit_AttackAt(const Ov003AttackInfo*, BattleAttackArgs*, s32, s32, s32);
s32                    CombatHit_AttackAtScreen(const Ov003AttackInfo*, BattleAttackArgs*, s32, s32);
s32                    CombatHit_AttackCharacter(const Ov003AttackInfo*, BattleAttackArgs*, s32);
void                   CombatHit_ArmActor(u16 attackId, const BattleAttackArgs* args, BattleActor* actor);
s32                    CombatHit_ActorImpact(BattleActor* actor);

#endif // TWEWY_COMBAT_CORE_COMBATHIT_H
