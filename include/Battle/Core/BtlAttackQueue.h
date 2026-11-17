#ifndef TWEWY_BATTLE_CORE_BTLATTACKQUEUE_H
#define TWEWY_BATTLE_CORE_BTLATTACKQUEUE_H

#include "Battle/Core/BtlActor.h"
#include "Battle/Core/BtlList.h"
#include "Combat/Core/CombatPsych.h"

/// One psych attack queued for the player to play out.
typedef struct BattleQueuedAttack {
    /* 0x00 */ u16               unk_00;
    /* 0x02 */ u16               unk_02;
    /* 0x04 */ Ov003EffectState* state;   // the psych that queued the attack
    /* 0x08 */ BattleActor*      target;  // the actor to attack
    /* 0x0C */ s32               variant; // psych-specific variant, picks the player's attack animation
    /* 0x10 */ BattleListNode    node;
} BattleQueuedAttack;                     // Size: 0x1C

/// The player's queued attacks: a pool of 8 entries moving between a free list and an active list.
typedef struct BattleAttackQueue {
    /* 0x000 */ BattleList         free;
    /* 0x008 */ BattleList         active;
    /* 0x010 */ BattleQueuedAttack entries[8];
} BattleAttackQueue; // Size: 0xF0

/**
 * @brief Queues an attack for the player to play out
 *
 * Does nothing if `target` is NULL or all 8 entries are in use.
 *
 * @param state The psych that queued the attack
 * @param target The actor to attack
 * @param variant Psych-specific variant, picks the player's attack animation
 */
void BtlAttackQueue_Push(Ov003EffectState* state, BattleActor* target, s32 variant);

/**
 * @brief Removes a queued attack, returning its entry to the free list
 *
 * @param attack The attack to remove; NULL or an already-free entry is ignored
 */
void BtlAttackQueue_Remove(BattleQueuedAttack* attack);

/**
 * @brief Removes every queued attack
 */
void BtlAttackQueue_Clear(void);

/**
 * @brief Returns the oldest queued attack without removing it
 *
 * @return The oldest queued attack, or NULL if none are queued
 */
BattleQueuedAttack* BtlAttackQueue_GetHead(void);

/**
 * @brief Resets the attack queue, moving all 8 entries to the free list
 */
void BtlAttackQueue_Init(void);

/**
 * @brief Removes the queued attacks whose target is defeated, intangible or launched
 */
void BtlAttackQueue_DropInvalid(void);

#endif // TWEWY_BATTLE_CORE_BTLATTACKQUEUE_H
