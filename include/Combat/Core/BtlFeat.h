#ifndef TWEWY_COMBAT_CORE_BTLFEAT_H
#define TWEWY_COMBAT_CORE_BTLFEAT_H

#include <nitro/types.h>

/// Starts the splash script of enemy sprites `start` onward, `count` of them, playing `sound`.
void func_ov003_020bc690(u16 start, u16 count, s32 sound);

/// Starts the splash background of `group`/`variant` on `screen` for `duration` frames.
void func_ov003_020bce18(s32 screen, u16 group, u16 variant, u16 duration);

#endif // TWEWY_COMBAT_CORE_BTLFEAT_H
