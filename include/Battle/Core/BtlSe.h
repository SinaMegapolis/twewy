#ifndef TWEWY_BATTLE_CORE_BTLSE_H
#define TWEWY_BATTLE_CORE_BTLSE_H

#include "SndMgrSeIdx.h"

void BtlSe_Play(SndMgrSeIdx);
void BtlSe_PlayPanned(SndMgrSeIdx seIdx, s32 sePan);
void BtlSe_PlayAtMainX(SndMgrSeIdx, s32);
void BtlSe_PlayAtSubX(SndMgrSeIdx, s32);

#endif // TWEWY_BATTLE_CORE_BTLSE_H
