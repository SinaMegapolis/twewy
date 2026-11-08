#ifndef TWEWY_COMBAT_CORE_COMBATHIT_H
#define TWEWY_COMBAT_CORE_COMBATHIT_H

#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatActor.h"
#include "Combat/Core/CombatAttack.h"
#include "Combat/Core/CombatEnemyActor.h"
#include "Combat/Core/CombatSprite.h"

void                   CombatHit_LoadSpark(s32, CombatSprite*, s32);
void                   CombatHit_DrawSpark(CombatSprite*, s32, s32);
void                   CombatHit_LoadSparkForUnit(s32, CombatSprite*, Ov003EnemyActor*);
void                   CombatHit_DrawSparkOnActor(CombatSprite*, CombatActor*);
s32                    CombatHit_CanDrawSpark(void);
s32                    CombatHit_ApplyAttack(s32 mode, Ov003EnemyActor* unit, u16 attackId);
s32                    CombatHit_Stagger(s32, Ov003EnemyActor*, s32);
const Ov003AttackInfo* CombatHit_GetAttackInfo(u16);
s32                    CombatHit_AttackUnit(const Ov003AttackInfo*, void*, void*, s32);
s32                    CombatHit_AttackAt(const Ov003AttackInfo*, Ov003AttackArgs*, s32, s32, s32);
s32                    CombatHit_AttackAtScreen(const Ov003AttackInfo*, void*, s32, s32);
s32                    CombatHit_AttackCharacter(const Ov003AttackInfo*, void*, s32);
void                   CombatHit_ArmActor(s32 arg0, const Ov003AttackArgs* args, Ov003EnemyActor* unit);
s32                    CombatHit_ActorImpact(CombatActor* actor);

#endif // TWEWY_COMBAT_CORE_COMBATHIT_H
