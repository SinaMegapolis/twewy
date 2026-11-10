#ifndef TWEWY_COMBAT_CORE_BTLFIELD_H
#define TWEWY_COMBAT_CORE_BTLFIELD_H

#include "Battle/Core/BtlStage.h"
#include "Display.h"
#include "Engine/EasyTask.h"
#include "Engine/Resources/ScreenMapper.h"

/// The sub-screen field background: the camera-driven BG tilemap and its scrolling floor strips.
extern const TaskHandle Tsk_BtlField_RG;

/// The main-screen field background: the camera-driven BG tilemap and its scrolling floor rows.
extern const TaskHandle Tsk_BtlField_UG;

/// A field obstacle: an object on the field that can be pushed around, and blinks out when it is destroyed.
extern const TaskHandle Tsk_BtlObs;

/// Spawns the foreground task when background `bgIndex` has any bands.
void func_ov003_0209409c(s32 bgIndex);

/// Scrolls the BG of `map` to the camera's position, shifted by `offsetX`/`offsetY`.
static inline void BtlField_ScrollRow(ScreenMapEntry* map, BattleStage* camera, s32 offsetX, s32 offsetY) {
    s32                 layer  = map->bgLayer;
    DisplayEngineState* engine = &g_DisplaySettings.engineState[map->engineId];
    s32                 posY   = camera->viewY - offsetY;
    s32                 posX   = camera->viewX - offsetX;

    Display_SetEngineBGOffset(engine, layer, posX, posY);
}

#endif // TWEWY_COMBAT_CORE_BTLFIELD_H
