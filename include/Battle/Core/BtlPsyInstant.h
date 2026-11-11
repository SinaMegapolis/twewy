#ifndef TWEWY_BATTLE_CORE_BTLPSYINSTANT_H
#define TWEWY_BATTLE_CORE_BTLPSYINSTANT_H

#include "Battle/Core/BtlActor.h"
#include "Combat/Core/CombatPsych.h"

/// The 8-byte payload an attack entry point allocates for its on-fire handler.
typedef struct BattleInstantAttack {
    /* 0x00 */ Ov003EffectState* effect;
    /* 0x04 */ BattleActor*      target;
} BattleInstantAttack; // Size: 0x8

/// An attack entry point: the handler the battle calls when the attack lands, plus its payload.
typedef struct BattleInstantAttackCmd {
    /* 0x00 */ void (*fire)(BattleInstantAttack*);
    /* 0x04 */ s32                  unk_04;
    /* 0x08 */ BattleInstantAttack* data;
} BattleInstantAttackCmd; // Size: 0xC

s32 BtlPsyInstant_StartPierce(BattleInstantAttackCmd* cmd, Ov003EffectState* effect);
s32 BtlPsyInstant_StartSyoryu(BattleInstantAttackCmd* cmd, Ov003EffectState* effect);
s32 BtlPsyInstant_StartKabuto(BattleInstantAttackCmd* cmd, Ov003EffectState* effect);
s32 BtlPsyInstant_StartTeleport(BattleInstantAttackCmd* cmd, Ov003EffectState* effect);

#endif // TWEWY_BATTLE_CORE_BTLPSYINSTANT_H
