#ifndef TWEWY_BATTLE_CORE_BTLHIT_H
#define TWEWY_BATTLE_CORE_BTLHIT_H

#include "Battle/Core/BtlActor.h"
#include "Battle/Core/BtlAttackInfo.h"
#include "Battle/Core/BtlMgr.h"
#include "Battle/Core/BtlSprite.h"

typedef enum BattleHitResult {
    BTL_HIT_RESULT_MISSED,  // nothing was hit or the battle is not running
    BTL_HIT_RESULT_LANDED,  // at least one target was hit
    BTL_HIT_RESULT_GUARDED, // every reached target guarded against it
} BattleHitResult;

/**
 * @brief Loads a hit spark sprite
 *
 * @param engine The screen to load it for (DISPLAY_MAIN or DISPLAY_SUB)
 * @param sprite The sprite to load into
 * @param size Spark size, 0..5
 */
void BtlHit_LoadSpark(DisplayEngine engine, BattleSprite* sprite, s32 size);

/**
 * @brief Draws a hit spark at a field position, unless sparks are hidden (see BtlHit_CanDrawSpark)
 *
 * @param sprite The spark sprite
 * @param x Field x position, 20.12 fixed point
 * @param y Field y position, 20.12 fixed point
 */
void BtlHit_DrawSpark(BattleSprite* sprite, s32 x, s32 y);

/**
 * @brief Loads a hit spark sized to an actor's hit box
 *
 * @param engine The screen to load it for (DISPLAY_MAIN or DISPLAY_SUB)
 * @param sprite The sprite to load into
 * @param actor The actor whose radius picks the spark size
 */
void BtlHit_LoadSparkForActor(DisplayEngine engine, BattleSprite* sprite, BattleActor* actor);

/**
 * @brief Draws a hit spark on an actor, unless the actor is hidden
 *
 * @param sprite The spark sprite
 * @param actor The actor to draw it on
 */
void BtlHit_DrawSparkOnActor(BattleSprite* sprite, BattleActor* actor);

/**
 * @brief Whether hit sparks may be drawn; some special attacks hide them while they run
 *
 * @return TRUE if sparks may be drawn
 */
BOOL BtlHit_CanDrawSpark(void);

/**
 * @brief Makes an actor flinch and rolls a status effect on it
 *
 * @param side The side whose abilities apply and whose screen shows the effect
 * @param actor The actor to hit
 * @param statusEffect The status effect to roll (BattleAttackFlags.statusEffect), 0 = none
 * @return TRUE if the actor flinched, FALSE if it is defeated, intangible, launched or knocked down
 */
BOOL BtlHit_FlinchWithStatus(BattleSide side, BattleActor* actor, u16 statusEffect);

/**
 * @brief Makes an actor flinch, breaking its guard, and optionally stuns it
 *
 * @param side The side the actor is on
 * @param actor The actor to stagger
 * @param stunFrames How long to stun it, in frames; 0 for no stun
 * @return TRUE if the actor was staggered, FALSE if it is defeated, intangible, launched or knocked down
 */
BOOL BtlHit_Stagger(BattleSide side, BattleActor* actor, s16 stunFrames);

/**
 * @brief Returns an attack's info
 *
 * @param id The attack id, 0..400
 * @return The attack's entry in BtlAttackInfo_Table
 */
const BattleAttackInfo* BtlHit_GetAttackInfo(u16 id);

/**
 * @brief Attacks a given actor
 *
 * @param info The attack
 * @param args The attacker's damage arguments
 * @param actor The actor to attack
 * @param originX The attack's origin x, 20.12 fixed point; the actor faces and is knocked away from it
 * @return BTL_HIT_RESULT_MISSED also when the battle is not running
 */
BattleHitResult BtlHit_AttackActor(const BattleAttackInfo* info, BattleAttackArgs* args, BattleActor* actor, s32 originX);

/**
 * @brief Attacks with the attack's hit box centred on a field position
 *
 * A player attack hits every enemy on its screen the box overlaps; an enemy attack hits the screen's player character.
 *
 * @param info The attack
 * @param args The attacker's damage arguments
 * @param x Field x position, 20.12 fixed point
 * @param y Field y position, 20.12 fixed point
 * @param z Height, 20.12 fixed point
 * @return BTL_HIT_RESULT_LANDED if anything was hit, BTL_HIT_RESULT_GUARDED if everything hit guarded
 */
BattleHitResult BtlHit_AttackAt(const BattleAttackInfo* info, BattleAttackArgs* args, s32 x, s32 y, s32 z);

/**
 * @brief Attacks with the attack's hit box centred on a screen position, like BtlHit_AttackAt
 *
 * @param info The attack
 * @param args The attacker's damage arguments
 * @param x Screen x position, pixels
 * @param y Screen y position, pixels
 * @return BTL_HIT_RESULT_LANDED if anything was hit, BTL_HIT_RESULT_GUARDED if everything hit guarded
 */
BattleHitResult BtlHit_AttackAtScreen(const BattleAttackInfo* info, BattleAttackArgs* args, s32 x, s32 y);

/**
 * @brief Attacks the player or the partner directly, picked by the attack's screen
 *
 * @param info The attack
 * @param args The attacker's damage arguments
 * @param originX The attack's origin x, 20.12 fixed point
 * @return BTL_HIT_RESULT_MISSED also when the battle is not running
 */
BattleHitResult BtlHit_AttackCharacter(const BattleAttackInfo* info, BattleAttackArgs* args, s32 originX);

/**
 * @brief Arms an actor's impact attack, dealt when it is thrown or knocked into others (see BtlHit_ActorImpact)
 *
 * @param attackId The attack the impact deals
 * @param args The damage arguments of whoever threw or knocked the actor
 * @param actor The actor to arm
 */
void BtlHit_ArmActor(u16 attackId, const BattleAttackArgs* args, BattleActor* actor);

/**
 * @brief Deals a moving actor's impact attack around it
 *
 * The attack's hit box is the actor's own and its power scales with BattleActor.impactPercent. A field object loses
 * one HP per impact that lands and breaks at 0.
 *
 * @param actor The moving actor
 * @return The BtlHit_AttackAt result, or BTL_HIT_RESULT_MISSED if the actor is not moving fast enough
 */
BattleHitResult BtlHit_ActorImpact(BattleActor* actor);

#endif // TWEWY_BATTLE_CORE_BTLHIT_H
