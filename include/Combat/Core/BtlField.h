#ifndef TWEWY_COMBAT_CORE_BTLFIELD_H
#define TWEWY_COMBAT_CORE_BTLFIELD_H

#include "Engine/EasyTask.h"

/// The sub-screen field background: the camera-driven BG tilemap and its scrolling floor strips.
extern const TaskHandle Tsk_BtlField_RG;

/// The main-screen field background: the camera-driven BG tilemap and its scrolling floor rows.
extern const TaskHandle Tsk_BtlField_UG;

/// A field obstacle: an object on the field that can be pushed around, and blinks out when it is destroyed.
extern const TaskHandle Tsk_BtlObs;

/// Spawns the foreground task when background `bgIndex` has any bands.
void func_ov003_0209409c(s32 bgIndex);

#endif // TWEWY_COMBAT_CORE_BTLFIELD_H
