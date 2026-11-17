#ifndef COMBAT_COMBATATTACK_H
#define COMBAT_COMBATATTACK_H

#include "Battle/Core/BtlActor.h"
#include "Battle/Core/BtlAttackInfo.h"
#include "Battle/Core/BtlList.h"

/// Hit counter, payload and configured maximum consecutive hits (0xFF = unlimited).
typedef struct Ov003AttackStreak {
    /* 0x00 */ u8               unk_00[0x8];
    /* 0x08 */ s16              hits;
    /* 0x0A */ u8               unk_0A[0x2];
    /* 0x0C */ BattleAttackArgs args;
    /* 0x14 */ u8               maxHits;
    /* 0x15 */ u8               unk_15[3];
} Ov003AttackStreak; // Size: 0x18

// Applies the staged attack and returns its processing result after restoring X velocity.
struct Ov003EnemyActor;

s32 func_ov003_020a6b84(Ov003AttackStreak* streak, u16 attackId, struct Ov003EnemyActor* unit);

#endif
