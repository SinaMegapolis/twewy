#ifndef TWEWY_BATTLE_CORE_BTLDEBUG_H
#define TWEWY_BATTLE_CORE_BTLDEBUG_H

#include "nitro/types.h"

typedef enum BattleDebugSetting {
    BTL_DEBUG_SETTING_MODE           = 0,  // BattleDebugMode, actor visibility and behavior
    BTL_DEBUG_SETTING_DISP_AREA      = 1,  // BattleDebugDispArea, which collision overlays to show
    BTL_DEBUG_SETTING_DISP_PRM       = 2,  // Show the attack and defense values
    BTL_DEBUG_SETTING_RG_CHARACTER   = 3,  // Which partner is active (0..3)
    BTL_DEBUG_SETTING_BADGE_SET      = 4,  // The badge set (0..65)
    BTL_DEBUG_SETTING_ENEMY_PARTY    = 5,  // The enemy party (0..1274)
    BTL_DEBUG_SETTING_FIELD_BG_NUM   = 6,  // The battle background (0..40)
    BTL_DEBUG_SETTING_INVINCIBLE     = 7,  // Whether the player is invincible
    BTL_DEBUG_SETTING_BADGE_FREE     = 8,  // Whether pins never run out
    BTL_DEBUG_SETTING_AUTOTOUCH      = 9,  // Whether autotouch is enabled
    BTL_DEBUG_SETTING_LEVEL_MAX      = 10, // The maximum player level (1..100)
    BTL_DEBUG_SETTING_LEVEL_NOW      = 11, // The current player level (1..LEVEL_MAX)
    BTL_DEBUG_SETTING_SYNC           = 12, // The sync value (0..1000)
    BTL_DEBUG_SETTING_RG_AUTO_ATTACK = 13, // The partner's auto attack setting (0..3)
    BTL_DEBUG_SETTING_DIFFICULTY     = 14, // The battle difficulty (0..3)
    BTL_DEBUG_SETTING_CHAIN_BATTLE   = 15, // The number of battles in the chain (1..16)
    BTL_DEBUG_SETTING_UG_SIZE_X      = 16, // The top screen field width in pixels (256..2048)
    BTL_DEBUG_SETTING_UG_SIZE_Y      = 17, // The top screen field depth in pixels (16..256)
    BTL_DEBUG_SETTING_UG_UNDER_Y     = 18, // The top screen under Y (0..128)
    BTL_DEBUG_SETTING_UG_SCROLL_Y    = 19, // The top screen scroll Y (0..320)
    BTL_DEBUG_SETTING_UG_CAMERA_Y    = 20, // The top screen camera Y (-256..256)
    BTL_DEBUG_SETTING_RG_UNDER_Y     = 21, // The bottom screen under Y (0..128)
    BTL_DEBUG_SETTING_RG_SCROLL_Y    = 22, // The bottom screen scroll Y (0..320)
    BTL_DEBUG_SETTING_RG_CAMERA_Y    = 23, // The bottom screen camera Y (-256..256)
    BTL_DEBUG_SETTING_RG_SIZE_Y      = 24, // The bottom screen field depth in pixels (16..256)
    BTL_DEBUG_SETTING_COUNT          = 25
} BattleDebugSetting;

typedef enum BattleDebugMode {
    BTL_DEBUG_MODE_NORMAL         = 0, // all actors appear and behave normally
    BTL_DEBUG_MODE_PLAYER_ONLY    = 1, // enemies are hidden and pins never act
    BTL_DEBUG_MODE_NO_FRAME_DELAY = 2, // actors update every frame, ignoring the battle's delay counters
} BattleDebugMode;

/// Values of BTL_DEBUG_SETTING_DISP_AREA.
typedef enum BattleDebugDispArea {
    BTL_DEBUG_DISP_AREA_OFF        = 0, // no collision overlay displayed
    BTL_DEBUG_DISP_AREA_FOOTPRINTS = 1, // the footprint circle of every actor
    BTL_DEBUG_DISP_AREA_HITBOXES   = 2, // the hit box of every actor
} BattleDebugDispArea;

/// The battle debug settings, kept across battles and validated with a magic word.
typedef struct BattleDebugSettings {
    /* 0x00 */ s32 reserved00;
    /* 0x04 */ s32 values[BTL_DEBUG_SETTING_COUNT]; // indexed by BattleDebugSetting
    /* 0x68 */ s32 unk_68;
    /* 0x6C */ s32 unk_6C;
    /* 0x70 */ s32 magic;
    /* 0x74 */ s8  current; // the overlay group of the enemies picked in the debug enemy select, -1 = none
    /* 0x75 */ u8  reserved75[3];
} BattleDebugSettings;      // Size: 0x78

extern BattleDebugSettings gBattleDebugSettings;

/**
 * @brief Loads the default debug settings, does nothing when settings already have magic value set
 */
void BtlDebug_InitSettings(void);

/**
 * @brief Clamps every debug setting into its legal range
 */
void BtlDebug_ValidateSettings(void);

/**
 * @brief Reads a debug setting
 *
 * @param setting The setting to read
 * @return The setting's value; see BattleDebugSetting for each setting's range and meaning
 */
s32 BtlDebug_GetSetting(BattleDebugSetting setting);

/**
 * @brief Stores the overlay group of the enemies picked in the debug enemy select
 *
 * @param value The overlay group, -1 = none
 */
void BtlDebug_SetCurrent(s32 value);

/**
 * @brief Reads the overlay group of the enemies picked in the debug enemy select
 *
 * @return The value stored by BtlDebug_SetCurrent, -1 = none
 */
s32 BtlDebug_GetCurrent(void);

/**
 * @brief Draws the collision overlay chosen by BTL_DEBUG_SETTING_DISP_AREA
 */
void BtlDebug_DrawCollision(void);

/**
 * @brief Shows the attack and defense values of the player and partner when BTL_DEBUG_SETTING_DISP_PRM is on
 */
void BtlDebug_ShowSyncValues(void);

#endif // TWEWY_BATTLE_CORE_BTLDEBUG_H
