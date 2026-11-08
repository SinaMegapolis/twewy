#include "Combat/Core/CombatDebug.h"
#include "Combat/Core/BtlDbg.h"
#include "Combat/Core/BtlInf.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatActor.h"
#include "Combat/Core/CombatApi.h"
#include "Save.h"
#include <nitro/mi/cpumem.h>

/// Clamps setting `index` into [`lo`, `hi`].
static void CombatDebug_ClampSetting(s32 index, s32 lo, s32 hi) {
    s32* values = gCombatSettings.values;
    s32  value  = values[index];

    if (value < lo) {
        values[index] = lo;
        return;
    }
    if (value > hi) {
        values[index] = hi;
    }
}

static void CombatDebug_SetSetting(s32 index, s32 value);

/// Loads the default settings.
static void CombatDebug_LoadDefaults(void) {
    CombatDebug_SetSetting(0, 0);
    CombatDebug_SetSetting(1, 0);
    CombatDebug_SetSetting(2, 0);
    CombatDebug_SetSetting(3, 0);
    CombatDebug_SetSetting(4, 0);
    CombatDebug_SetSetting(5, 0);
    CombatDebug_SetSetting(6, 0);
    CombatDebug_SetSetting(7, 0);
    CombatDebug_SetSetting(8, 0);
    CombatDebug_SetSetting(9, 0);
    CombatDebug_SetSetting(10, 1);
    CombatDebug_SetSetting(11, 1);
    CombatDebug_SetSetting(12, 0x1F4);
    CombatDebug_SetSetting(13, 0);
    CombatDebug_SetSetting(14, 1);
    CombatDebug_SetSetting(15, 1);
    CombatDebug_SetSetting(16, 0x200);
    CombatDebug_SetSetting(17, 0x100);
    CombatDebug_SetSetting(18, 0x10);
    CombatDebug_SetSetting(19, 0x80);
    CombatDebug_SetSetting(20, -0x28);
    CombatDebug_SetSetting(21, 0);
    CombatDebug_SetSetting(22, 0x120);
    CombatDebug_SetSetting(23, -0x28);
    CombatDebug_SetSetting(24, 0x38);
}

/// Sets the settings up on the first call.
void CombatDebug_InitSettings(void) {
    if (gCombatSettings.magic == 0x6128248) {
        return;
    }
    MI_CpuSet(&gCombatSettings, 0, sizeof(Ov003Settings));
    CombatDebug_LoadDefaults();
    gCombatSettings.unk_68  = 0;
    gCombatSettings.unk_6C  = 0;
    gCombatSettings.magic   = 0x6128248;
    gCombatSettings.current = -1;
}

/// Clamps every setting into its legal range.
void CombatDebug_ValidateSettings(void) {
    CombatDebug_ClampSetting(0, 0, 2);
    CombatDebug_ClampSetting(1, 0, 2);
    CombatDebug_ClampSetting(2, 0, 1);
    CombatDebug_ClampSetting(3, 0, 3);
    CombatDebug_ClampSetting(4, 0, 0x41);
    CombatDebug_ClampSetting(5, 0, 0x4FA);
    CombatDebug_ClampSetting(6, 0, 0x28);
    CombatDebug_ClampSetting(7, 0, 1);
    CombatDebug_ClampSetting(8, 0, 1);
    CombatDebug_ClampSetting(9, 0, 1);
    CombatDebug_ClampSetting(10, 1, 0x64);
    CombatDebug_ClampSetting(11, 1, CombatDebug_GetSetting(10));
    CombatDebug_ClampSetting(12, 0, 0x3E8);
    CombatDebug_ClampSetting(13, 0, 3);
    CombatDebug_ClampSetting(14, 0, 3);
    CombatDebug_ClampSetting(15, 1, 0x10);
    CombatDebug_ClampSetting(16, 0x100, 0x800);
    CombatDebug_ClampSetting(17, 0x10, 0x100);
    CombatDebug_ClampSetting(18, 0, 0x80);
    CombatDebug_ClampSetting(19, 0, 0x140);
    CombatDebug_ClampSetting(20, -0x100, 0x100);
    CombatDebug_ClampSetting(24, 0x10, 0x100);
    CombatDebug_ClampSetting(21, 0, 0x80);
    CombatDebug_ClampSetting(22, 0, 0x140);
    CombatDebug_ClampSetting(23, -0x100, 0x100);
}

s32 CombatDebug_GetSetting(s32 index) {
    return gCombatSettings.values[index];
}

static void CombatDebug_SetSetting(s32 index, s32 value) {
    gCombatSettings.values[index] = value;
}

void CombatDebug_SetCurrent(s32 value) {
    gCombatSettings.current = value;
}

s32 CombatDebug_GetCurrent(void) {
    return gCombatSettings.current;
}

/// Draws the hit circles of the player and of every enemy.
static void CombatDebug_DrawFootprints(void) {
    CombatActor* player = g_CombatGlobal->unk3D898;
    s16          radius = player->unk_70;

    BtlDbg_PostRect(BTLDBG_KIND_FOOTPRINT, 0, player->position.x - I2F(radius), player->position.y - I2F(radius / 2),
                    radius * 2, radius);

    CombatActor* actor = CombatList_GetHead(&g_CombatGlobal->unk3D8A4);
    if (actor != NULL) {
        do {
            radius = actor->unk_70;
            BtlDbg_PostRect(BTLDBG_KIND_FOOTPRINT, 0, actor->position.x - (radius << 12),
                            actor->position.y - ((radius / 2) << 12), radius * 2, radius);

            actor = CombatList_GetNext(&((Ov003EnemyActor*)actor)->node);
        } while (actor != NULL);
    }

    u8* item = CombatList_GetHead(&g_CombatGlobal->unk3D8B4);
    if (item != NULL) {
        do {
            radius = ((CombatActor*)(item + 4))->unk_70;
            BtlDbg_PostRect(BTLDBG_KIND_FOOTPRINT, 0, ((CombatActor*)(item + 4))->position.x - (radius << 12),
                            ((CombatActor*)(item + 4))->position.y - ((radius / 2) << 12), radius * 2, radius);
            item = CombatList_GetNext((CombatListNode*)(item + 0xB8));
        } while (item != NULL);
    }
}

/// Draws the hit boxes of the player and of every enemy.
static void CombatDebug_DrawHitboxes(void) {
    CombatActor* player = g_CombatGlobal->unk3D898;
    CombatActor* actor;
    u8*          item;
    s32          radius = player->unk_70;
    s32          height = player->unk_72;

    BtlDbg_PostRect(BTLDBG_KIND_HITBOX, 0, player->position.x - ((radius + 0x18) << 12),
                    player->position.z + (player->position.y - ((height + 0x12) << 12)), (radius + 0x18) * 2, height + 0x30);
    actor = CombatList_GetHead(&g_CombatGlobal->unk3D8A4);
    if (actor != NULL) {
        do {
            radius = actor->unk_70;
            height = actor->unk_72;
            BtlDbg_PostRect(BTLDBG_KIND_HITBOX, 0, actor->position.x - (radius << 12),
                            actor->position.z + (actor->position.y - (height << 12)), radius * 2, height + 0xC);
            actor = CombatList_GetNext(&((Ov003EnemyActor*)actor)->node);
        } while (actor != NULL);
    }
    item = CombatList_GetHead(&g_CombatGlobal->unk3D8B4);
    if (item == NULL) {
        return;
    }
    do {
        CombatActor* sub = (CombatActor*)(item + 4);

        radius = sub->unk_70;
        height = sub->unk_72;
        BtlDbg_PostRect(BTLDBG_KIND_HITBOX, 0, sub->position.x - (radius << 12),
                        sub->position.z + (sub->position.y - (height << 12)), radius * 2, height + 0xC);
        item = CombatList_GetNext((CombatListNode*)(item + 0xB8));
    } while (item != NULL);
}

void CombatDebug_DrawCollision(void) {
    switch (CombatDebug_GetSetting(1)) {
        case 0:
            return;
        case 1:
            CombatDebug_DrawFootprints();
            break;
        case 2:
            CombatDebug_DrawHitboxes();
            break;
    }
}

void CombatDebug_ShowSyncValues(void) {
    Ov003Global* global;
    CombatActor* player;
    CombatActor* partner;

    if (CombatDebug_GetSetting(2) == 0) {
        return;
    }
    global  = g_CombatGlobal;
    player  = global->unk3D898;
    partner = global->unk3D89C;
    func_ov003_020a2f7c(&global->unk_10118, &partner->unk_68, &player->unk_68, &partner->unk_66, &player->unk_66, 0,
                        &global->unk3D8D2, 0, 0);
}
