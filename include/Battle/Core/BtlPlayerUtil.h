#ifndef TWEWY_BATTLE_CORE_BTLPLAYERUTIL_H
#define TWEWY_BATTLE_CORE_BTLPLAYERUTIL_H

#include "nitro/types.h"

/// The idle pose a player character shows, picked by BtlPlayerUtil_UpdateIdlePose.
typedef enum BtlIdlePose {
    BTL_IDLE_POSE_UNCHANGED = 0, // returned when the pose did not change
    BTL_IDLE_POSE_NORMAL    = 1,
    BTL_IDLE_POSE_HURT      = 2, // the side has taken at least half the team's maximum HP in damage
    BTL_IDLE_POSE_BORED     = 3, // idle for 240 frames or more
} BtlIdlePose;

/**
 * @brief Ticks the regeneration ability, healing the team every 300 frames while it is active
 *
 * Each heal restores the ability's value as a percentage of the team's maximum HP (at least 1).
 *
 * @param side 0 = the player, 1 = the partner
 * @param timer The character's regeneration frame counter
 */
void BtlPlayerUtil_UpdateRegen(s32 side, s16* timer);

/**
 * @brief Keeps the character invincible while it holds the aura and has abilities 0x41..0x45 all active
 *
 * @param side 0 = the player, 1 = the partner
 */
void BtlPlayerUtil_UpdateAuraInvincibility(s32 side);

/**
 * @brief Picks the idle pose for a player character
 *
 * @param pose The character's current pose (BtlIdlePose), updated in place
 * @param damageTaken Team HP lost to hits on this side
 * @param idleFrames Frames the character has been idle, 0 on the first idle frame
 * @return The new pose (BtlIdlePose), or BTL_IDLE_POSE_UNCHANGED if it did not change after the first idle frame
 */
u16 BtlPlayerUtil_UpdateIdlePose(u16* pose, s32 damageTaken, s32 idleFrames);

/**
 * @brief Whether the player is defeated or staggered, or the level-up effect is playing
 *
 * @return TRUE if the player cannot act
 */
BOOL BtlPlayerUtil_IsPlayerDown(void);

/**
 * @brief Whether the stylus is dragging the player to a point more than 80 pixels away from them
 *
 * @return TRUE for a drag from the player that has moved over 80 pixels
 */
BOOL BtlPlayerUtil_IsLongPlayerDrag(void);

/**
 * @brief Plays a random one of the player's attack shouts
 *
 * @param finisher TRUE to pick from the 9 finishing-blow shouts, FALSE for the 8 normal attack shouts
 */
void BtlPlayerUtil_PlayAttackVoice(BOOL finisher);

/**
 * @brief Moves a character to a new position, turning it to face the way it moved, and plays the warp effect
 *
 * @param side 0 = the player, 1 = the partner
 * @param x New field x position, 20.12 fixed point
 * @param y New field y position, 20.12 fixed point
 * @param z New height, 20.12 fixed point
 */
void BtlPlayerUtil_WarpCharacter(s32 side, s32 x, s32 y, s32 z);

#endif // TWEWY_BATTLE_CORE_BTLPLAYERUTIL_H
