#ifndef TWEWY_BATTLE_CORE_BTLPLAYERUTIL_H
#define TWEWY_BATTLE_CORE_BTLPLAYERUTIL_H

#include "nitro/types.h"

void func_ov003_02087458(s32 mode, s16* timer);
void func_ov003_02087540(s32 mode);
u16  func_ov003_0208765c(u16* state, s32 value, s32 arg2);
s32  func_ov003_020876c0(void);
s32  func_ov003_02087704(void);
/// Plays one of the two hit-reaction SE sets, picked at random.
void func_ov003_02087804(s32 sub);
/// Moves the player (`mode != 1`) or the partner to a new position, facing the way it moved.
void BtlPlayerUtil_WarpCharacter(s32 mode, s32 x, s32 y, s32 z);

#endif // TWEWY_BATTLE_CORE_BTLPLAYERUTIL_H
