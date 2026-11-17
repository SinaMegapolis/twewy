#include "Battle/Core/BtlDebug.h"
#include "Battle/Core/BtlActor.h"
#include "Battle/Core/BtlObsMgr.h"
#include "Combat/Core/BtlDbg.h"
#include "Combat/Core/BtlInf.h"
#include "Combat/Core/CombatEnemyActor.h"
#include "FixedPoint.h"
#include "Player/Stats.h"
#include <nitro/mi/cpumem.h>

/// The value BattleDebugSettings::magic holds once the settings have been initialized
#define BTL_DEBUG_MAGIC 0x6128248

static void BtlDebug_SetSetting(BattleDebugSetting setting, s32 value);

/// Clamps `setting` between `lo` and `hi`
static void BtlDebug_ClampSetting(BattleDebugSetting setting, s32 lo, s32 hi) {
    s32* values = gBattleDebugSettings.values;

    if (values[setting] < lo) {
        values[setting] = lo;
    } else if (values[setting] > hi) {
        values[setting] = hi;
    }
}

/// Loads the default settings.
static void BtlDebug_LoadDefaults(void) {
    BtlDebug_SetSetting(BTL_DEBUG_SETTING_MODE, BTL_DEBUG_MODE_NORMAL);
    BtlDebug_SetSetting(BTL_DEBUG_SETTING_DISP_AREA, BTL_DEBUG_DISP_AREA_OFF);
    BtlDebug_SetSetting(BTL_DEBUG_SETTING_DISP_PRM, FALSE);
    BtlDebug_SetSetting(BTL_DEBUG_SETTING_RG_CHARACTER, PARTNER_SHIKI);
    BtlDebug_SetSetting(BTL_DEBUG_SETTING_BADGE_SET, 0);
    BtlDebug_SetSetting(BTL_DEBUG_SETTING_ENEMY_PARTY, 0);
    BtlDebug_SetSetting(BTL_DEBUG_SETTING_FIELD_BG_NUM, 0);
    BtlDebug_SetSetting(BTL_DEBUG_SETTING_INVINCIBLE, FALSE);
    BtlDebug_SetSetting(BTL_DEBUG_SETTING_BADGE_FREE, FALSE);
    BtlDebug_SetSetting(BTL_DEBUG_SETTING_AUTOTOUCH, FALSE);
    BtlDebug_SetSetting(BTL_DEBUG_SETTING_LEVEL_MAX, 1);
    BtlDebug_SetSetting(BTL_DEBUG_SETTING_LEVEL_NOW, 1);
    BtlDebug_SetSetting(BTL_DEBUG_SETTING_SYNC, 500);
    BtlDebug_SetSetting(BTL_DEBUG_SETTING_RG_AUTO_ATTACK, 0);
    BtlDebug_SetSetting(BTL_DEBUG_SETTING_DIFFICULTY, 1);
    BtlDebug_SetSetting(BTL_DEBUG_SETTING_CHAIN_BATTLE, 1);
    BtlDebug_SetSetting(BTL_DEBUG_SETTING_UG_SIZE_X, 512);
    BtlDebug_SetSetting(BTL_DEBUG_SETTING_UG_SIZE_Y, 256);
    BtlDebug_SetSetting(BTL_DEBUG_SETTING_UG_UNDER_Y, 16);
    BtlDebug_SetSetting(BTL_DEBUG_SETTING_UG_SCROLL_Y, 128);
    BtlDebug_SetSetting(BTL_DEBUG_SETTING_UG_CAMERA_Y, -40);
    BtlDebug_SetSetting(BTL_DEBUG_SETTING_RG_UNDER_Y, 0);
    BtlDebug_SetSetting(BTL_DEBUG_SETTING_RG_SCROLL_Y, 288);
    BtlDebug_SetSetting(BTL_DEBUG_SETTING_RG_CAMERA_Y, -40);
    BtlDebug_SetSetting(BTL_DEBUG_SETTING_RG_SIZE_Y, 56);
}

void BtlDebug_InitSettings(void) {
    if (gBattleDebugSettings.magic != BTL_DEBUG_MAGIC) {
        MI_CpuSet(&gBattleDebugSettings, 0, sizeof(BattleDebugSettings));
        BtlDebug_LoadDefaults();
        gBattleDebugSettings.unk_68  = 0;
        gBattleDebugSettings.unk_6C  = 0;
        gBattleDebugSettings.magic   = BTL_DEBUG_MAGIC;
        gBattleDebugSettings.current = -1;
    }
}

void BtlDebug_ValidateSettings(void) {
    BtlDebug_ClampSetting(BTL_DEBUG_SETTING_MODE, BTL_DEBUG_MODE_NORMAL, BTL_DEBUG_MODE_NO_FRAME_DELAY);
    BtlDebug_ClampSetting(BTL_DEBUG_SETTING_DISP_AREA, BTL_DEBUG_DISP_AREA_OFF, BTL_DEBUG_DISP_AREA_HITBOXES);
    BtlDebug_ClampSetting(BTL_DEBUG_SETTING_DISP_PRM, FALSE, TRUE);
    BtlDebug_ClampSetting(BTL_DEBUG_SETTING_RG_CHARACTER, 0, 3);
    BtlDebug_ClampSetting(BTL_DEBUG_SETTING_BADGE_SET, 0, 65);
    BtlDebug_ClampSetting(BTL_DEBUG_SETTING_ENEMY_PARTY, 0, 1274);
    BtlDebug_ClampSetting(BTL_DEBUG_SETTING_FIELD_BG_NUM, 0, 40);
    BtlDebug_ClampSetting(BTL_DEBUG_SETTING_INVINCIBLE, FALSE, TRUE);
    BtlDebug_ClampSetting(BTL_DEBUG_SETTING_BADGE_FREE, FALSE, TRUE);
    BtlDebug_ClampSetting(BTL_DEBUG_SETTING_AUTOTOUCH, FALSE, TRUE);
    BtlDebug_ClampSetting(BTL_DEBUG_SETTING_LEVEL_MAX, 1, 100);
    BtlDebug_ClampSetting(BTL_DEBUG_SETTING_LEVEL_NOW, 1, BtlDebug_GetSetting(BTL_DEBUG_SETTING_LEVEL_MAX));
    BtlDebug_ClampSetting(BTL_DEBUG_SETTING_SYNC, 0, 1000);
    BtlDebug_ClampSetting(BTL_DEBUG_SETTING_RG_AUTO_ATTACK, 0, 3);
    BtlDebug_ClampSetting(BTL_DEBUG_SETTING_DIFFICULTY, 0, 3);
    BtlDebug_ClampSetting(BTL_DEBUG_SETTING_CHAIN_BATTLE, 1, 16);
    BtlDebug_ClampSetting(BTL_DEBUG_SETTING_UG_SIZE_X, 256, 2048);
    BtlDebug_ClampSetting(BTL_DEBUG_SETTING_UG_SIZE_Y, 16, 256);
    BtlDebug_ClampSetting(BTL_DEBUG_SETTING_UG_UNDER_Y, 0, 128);
    BtlDebug_ClampSetting(BTL_DEBUG_SETTING_UG_SCROLL_Y, 0, 320);
    BtlDebug_ClampSetting(BTL_DEBUG_SETTING_UG_CAMERA_Y, -256, 256);
    BtlDebug_ClampSetting(BTL_DEBUG_SETTING_RG_SIZE_Y, 16, 256);
    BtlDebug_ClampSetting(BTL_DEBUG_SETTING_RG_UNDER_Y, 0, 128);
    BtlDebug_ClampSetting(BTL_DEBUG_SETTING_RG_SCROLL_Y, 0, 320);
    BtlDebug_ClampSetting(BTL_DEBUG_SETTING_RG_CAMERA_Y, -256, 256);
}

s32 BtlDebug_GetSetting(BattleDebugSetting setting) {
    return gBattleDebugSettings.values[setting];
}

/// Sets a debug setting's value.
static void BtlDebug_SetSetting(BattleDebugSetting setting, s32 value) {
    gBattleDebugSettings.values[setting] = value;
}

void BtlDebug_SetCurrent(s32 value) {
    gBattleDebugSettings.current = value;
}

s32 BtlDebug_GetCurrent(void) {
    return gBattleDebugSettings.current;
}

/// Draws the footprint circles of the player, every enemy and every object.
static void BtlDebug_DrawFootprints(void) {
    BattleActor* player = gBattle->player;
    s16          radius = player->radius;

    BtlDbg_PostRect(BTLDBG_KIND_FOOTPRINT, 0, player->position.x - I2F(radius), player->position.y - I2F(radius / 2),
                    radius * 2, radius);

    Ov003EnemyActor* unit = BtlList_GetHead(&gBattle->enemiesMain);
    while (unit != NULL) {
        radius = unit->actor.radius;
        BtlDbg_PostRect(BTLDBG_KIND_FOOTPRINT, 0, unit->actor.position.x - I2F(radius),
                        unit->actor.position.y - I2F(radius / 2), radius * 2, radius);
        unit = BtlList_GetNext(&unit->node);
    }

    BattleObsActor* obs = BtlList_GetHead(&gBattle->obsList);
    while (obs != NULL) {
        radius = obs->actor.radius;
        BtlDbg_PostRect(BTLDBG_KIND_FOOTPRINT, 0, obs->actor.position.x - I2F(radius), obs->actor.position.y - I2F(radius / 2),
                        radius * 2, radius);
        obs = BtlList_GetNext(&obs->node);
    }
}

/// Draws the hit boxes of the player, every enemy and every object.
static void BtlDebug_DrawHitboxes(void) {
    BattleActor* player = gBattle->player;
    s32          radius = player->radius;
    s32          height = player->height;

    BtlDbg_PostRect(BTLDBG_KIND_HITBOX, 0, player->position.x - I2F(radius + 24),
                    player->position.z + (player->position.y - I2F(height + 18)), (radius + 24) * 2, height + 48);

    Ov003EnemyActor* unit = BtlList_GetHead(&gBattle->enemiesMain);
    while (unit != NULL) {
        radius = unit->actor.radius;
        height = unit->actor.height;
        BtlDbg_PostRect(BTLDBG_KIND_HITBOX, 0, unit->actor.position.x - I2F(radius),
                        unit->actor.position.z + (unit->actor.position.y - I2F(height)), radius * 2, height + 12);
        unit = BtlList_GetNext(&unit->node);
    }

    BattleObsActor* obs = BtlList_GetHead(&gBattle->obsList);
    while (obs != NULL) {
        BattleActor* sub = &obs->actor;

        radius = sub->radius;
        height = sub->height;
        BtlDbg_PostRect(BTLDBG_KIND_HITBOX, 0, sub->position.x - I2F(radius),
                        sub->position.z + (sub->position.y - I2F(height)), radius * 2, height + 12);
        obs = BtlList_GetNext(&obs->node);
    }
}

void BtlDebug_DrawCollision(void) {
    switch (BtlDebug_GetSetting(BTL_DEBUG_SETTING_DISP_AREA)) {
        case BTL_DEBUG_DISP_AREA_OFF:
            return;
        case BTL_DEBUG_DISP_AREA_FOOTPRINTS:
            BtlDebug_DrawFootprints();
            break;
        case BTL_DEBUG_DISP_AREA_HITBOXES:
            BtlDebug_DrawHitboxes();
            break;
    }
}

void BtlDebug_ShowSyncValues(void) {
    if (BtlDebug_GetSetting(BTL_DEBUG_SETTING_DISP_PRM) != FALSE) {
        Battle*      global  = gBattle;
        BattleActor* player  = global->player;
        BattleActor* partner = global->partner;

        func_ov003_020a2f7c(&global->commonPool, &partner->defense, &player->defense, &partner->attack, &player->attack, 0,
                            &global->teamMaxHp, 0, 0);
    }
}
