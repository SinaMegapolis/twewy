#ifndef TWEWY_BATTLE_CORE_BTLATTACKQUEUE_H
#define TWEWY_BATTLE_CORE_BTLATTACKQUEUE_H

#include "Battle/Core/BtlActor.h"
#include "Battle/Core/BtlList.h"
#include "Combat/Core/CombatPsych.h"

/// One attack queued for the player to play out.
typedef struct BattleQueuedAttack {
    /* 0x00 */ u16               unk_00;
    /* 0x02 */ u16               unk_02;
    /* 0x04 */ Ov003EffectState* state;
    /* 0x08 */ BattleActor*      target;
    /* 0x0C */ s32               angle;
    /* 0x10 */ BattleListNode    node;
} BattleQueuedAttack; // Size: 0x1C

/// The queued attacks: two lists (free, queued) over a pool of 8 entries.
typedef struct BattleAttackQueue {
    /* 0x000 */ BattleList         free;
    /* 0x008 */ BattleList         active;
    /* 0x010 */ BattleQueuedAttack entries[8];
} BattleAttackQueue; // Size: 0xF0

void                func_ov003_02087250(Ov003EffectState* state, BattleActor* target, s32 angle);
void                func_ov003_020872b4(BattleQueuedAttack* attack);
void                func_ov003_02087310(void);
BattleQueuedAttack* func_ov003_02087358(void);
void                func_ov003_02087378(void);
/// Retires the queued attacks whose target actor is gone or defeated.
void func_ov003_020873e0(void);

#endif // TWEWY_BATTLE_CORE_BTLATTACKQUEUE_H
