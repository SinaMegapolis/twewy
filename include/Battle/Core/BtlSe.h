#ifndef TWEWY_BATTLE_CORE_BTLSE_H
#define TWEWY_BATTLE_CORE_BTLSE_H

#include "SndMgrSeIdx.h"

/**
 * @brief Plays a sound effect unless the battle is muted
 *
 * @param seIdx The sound effect to play
 */
void BtlSe_Play(SndMgrSeIdx seIdx);

/**
 * @brief Plays a panned sound effect unless the battle is muted
 *
 * @param seIdx The sound effect to play
 * @param pan Stereo pan, as screen pixels (0 = left edge, 255 = right edge)
 */
void BtlSe_PlayPanned(SndMgrSeIdx seIdx, s32 pan);

/**
 * @brief Plays a sound effect panned to a world position on the main screen, unless the battle is muted
 *
 * @param seIdx The sound effect to play
 * @param x World x position to pan to, 0x1000 = 1 pixel
 */
void BtlSe_PlayAtMainX(SndMgrSeIdx seIdx, s32 x);

/**
 * @brief Plays a sound effect panned to a world position on the sub screen, unless the battle is muted
 *
 * @param seIdx The sound effect to play
 * @param x World x position to pan to, 0x1000 = 1 pixel
 */
void BtlSe_PlayAtSubX(SndMgrSeIdx seIdx, s32 x);

#endif // TWEWY_BATTLE_CORE_BTLSE_H
