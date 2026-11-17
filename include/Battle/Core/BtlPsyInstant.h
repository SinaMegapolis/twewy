#ifndef TWEWY_BATTLE_CORE_BTLPSYINSTANT_H
#define TWEWY_BATTLE_CORE_BTLPSYINSTANT_H

#include "Battle/Core/BtlActor.h"
#include "Combat/Core/CombatPsych.h"

/// The payload a psych's start handler allocates for its fire handler.
typedef struct BattleInstantAttack {
    /* 0x00 */ Ov003EffectState* effect; // the psych being used
    /* 0x04 */ BattleActor*      target; // the enemy to attack (not allocated by the teleport)
} BattleInstantAttack;                   // Size: 0x8

/// A started psych: the handler the battle calls to fire it, plus its payload.
typedef struct BattleInstantAttackCmd {
    /* 0x00 */ void (*fire)(BattleInstantAttack*);
    /* 0x04 */ s32                  unk_04;
    /* 0x08 */ BattleInstantAttack* data;
} BattleInstantAttackCmd; // Size: 0xC

/**
 * @brief Starts the Sword Pierce psych if an enemy was tapped
 *
 * @param cmd Receives the fire handler and its payload
 * @param effect The psych being used
 * @return 1 if the psych started, 0 if no enemy was tapped
 */
s32 BtlPsyInstant_StartPierce(BattleInstantAttackCmd* cmd, Ov003EffectState* effect);

/**
 * @brief Starts the rising (Syoryu) sword psych if an enemy was slashed with an upward stroke
 *
 * @param cmd Receives the fire handler and its payload
 * @param effect The psych being used
 * @return 1 if the psych started, 0 otherwise
 */
s32 BtlPsyInstant_StartSyoryu(BattleInstantAttackCmd* cmd, Ov003EffectState* effect);

/**
 * @brief Starts the downward (Kabuto) sword psych if an enemy was slashed with a downward stroke
 *
 * Requires the player's full movement speed.
 *
 * @param cmd Receives the fire handler and its payload
 * @param effect The psych being used
 * @return 1 if the psych started, 0 otherwise
 */
s32 BtlPsyInstant_StartKabuto(BattleInstantAttackCmd* cmd, Ov003EffectState* effect);

/**
 * @brief Starts the teleport psych if empty ground was tapped
 *
 * Requires the player's full movement speed.
 *
 * @param cmd Receives the fire handler and its payload
 * @param effect The psych being used
 * @return 1 if the psych started, 0 otherwise
 */
s32 BtlPsyInstant_StartTeleport(BattleInstantAttackCmd* cmd, Ov003EffectState* effect);

#endif // TWEWY_BATTLE_CORE_BTLPSYINSTANT_H
