#ifndef TWEWY_BATTLE_CORE_BTLHIT_H
#define TWEWY_BATTLE_CORE_BTLHIT_H

#include "Battle/Core/BtlActor.h"
#include "Battle/Core/BtlSprite.h"
#include "Combat/Core/CombatAttack.h"

void                   BtlHit_LoadSpark(s32, BattleSprite*, s32);
void                   BtlHit_DrawSpark(BattleSprite*, s32, s32);
void                   BtlHit_LoadSparkForUnit(s32, BattleSprite*, BattleActor*);
void                   BtlHit_DrawSparkOnActor(BattleSprite*, BattleActor*);
s32                    BtlHit_CanDrawSpark(void);
s32                    BtlHit_ApplyAttack(s32 mode, BattleActor* unit, u16 attackId);
s32                    BtlHit_Stagger(s32, BattleActor*, s32);
const Ov003AttackInfo* BtlHit_GetAttackInfo(u16);
s32                    BtlHit_AttackUnit(const Ov003AttackInfo*, BattleAttackArgs*, BattleActor*, s32);
s32                    BtlHit_AttackAt(const Ov003AttackInfo*, BattleAttackArgs*, s32, s32, s32);
s32                    BtlHit_AttackAtScreen(const Ov003AttackInfo*, BattleAttackArgs*, s32, s32);
s32                    BtlHit_AttackCharacter(const Ov003AttackInfo*, BattleAttackArgs*, s32);
void                   BtlHit_ArmActor(u16 attackId, const BattleAttackArgs* args, BattleActor* actor);
s32                    BtlHit_ActorImpact(BattleActor* actor);

#endif // TWEWY_BATTLE_CORE_BTLHIT_H
