#include "Battle/Core/BtlActor.h"
#include "Battle/Core/BtlSprite.h"
#include "Battle/Core/BtlTouch.h"
#include "Combat/Core/BtlAura.h"
#include "Combat/Core/BtlBar.h"
#include "Combat/Core/BtlCmd.h"
#include "Combat/Core/BtlDbg.h"
#include "Combat/Core/BtlEff.h"
#include "Combat/Core/BtlPhase.h"
#include "Combat/Core/BtlTch.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatAttack.h"
#include "Combat/Core/CombatEnmOverlay.h"
#include "Combat/Fusion/BtlFusion.h"
#include "Combat/Tutorial/BtlTutorial.h"
#include "CriSndMgr.h"
#include "EasyFade.h"
#include "Engine/Color.h"
#include "Engine/Core/DMA.h"
#include "Engine/Core/HBlank.h"
#include "Engine/Core/Interrupts.h"
#include "Engine/Core/Memory.h"
#include "Engine/Core/OamMgr.h"
#include "Engine/Core/System.h"
#include "Engine/EasyTask.h"
#include "Engine/File/BinMgr.h"
#include "Engine/File/DatMgr.h"
#include "Engine/IO/Input.h"
#include "Engine/IO/TouchInput.h"
#include "Engine/Math/Random.h"
#include "Engine/Overlay/OverlayDispatcher.h"
#include "Engine/Overlay/OverlayManager.h"
#include "Engine/Resources/PaletteMgr.h"
#include "Engine/Resources/ResourceMgr.h"
#include "Engine/Resources/ScreenMapper.h"
#include "Field/Scene/FieldSeq.h"
#include "Player/Inventory.h"
#include "Save.h"
#include "SndMgr.h"
#include "common_data.h"

#include "Combat/Core/BtlInf.h"
#include "Combat/Core/BtlPop.h"

#include <nitro/fx.h>
#include <nitro/g3.h>
#include <nitro/gx.h>
#include <nitro/math.h>
#include <nitro/mi/cpumem.h>

/// A single link of the intrusive unit lists in `Ov003Global` (0x3D8A4..0x3D8C0). The link always
/// lives at offset 0x178 inside a unit and its `self` pointer points back at the unit.

/// Common prefix shared by every combat unit (players, partners, noise). Only the fields the battle
/// manager itself touches are modelled here; concrete units extend this layout.

/// A battle icon whose animation is driven by the shared state at `Ov003Global.touchWork.gesture`.

/// Overlay 4 id per noise id.

extern const TaskHandle Tsk_BtlPlayer01;
extern const TaskHandle Tsk_BtlCard_Esp;
extern const TaskHandle data_ov004_020f0120;
extern const TaskHandle data_ov005_020f0394;
extern const TaskHandle data_ov005_020f07a0;
extern const TaskHandle data_ov005_020f0a94;
extern const TaskHandle Tsk_BtlPlayer00;
extern const TaskHandle Tsk_BtlTarget;
extern const TaskHandle data_ov003_020dc614;
extern const TaskHandle data_ov003_020d4f74;

// Overlay ids are absolute linker symbols whose "address" is the overlay index.
extern u32 OVERLAY_4_ID;
extern u32 OVERLAY_5_ID;
extern u32 OVERLAY_6_ID;
extern u32 OVERLAY_7_ID;
extern u32 OVERLAY_8_ID;
extern u32 OVERLAY_9_ID;
extern u32 OVERLAY_10_ID;
extern u32 OVERLAY_11_ID;
extern u32 OVERLAY_12_ID;
extern u32 OVERLAY_13_ID;
extern u32 OVERLAY_14_ID;
extern u32 OVERLAY_15_ID;
extern u32 OVERLAY_16_ID;
extern u32 OVERLAY_17_ID;
extern u32 OVERLAY_18_ID;
extern u32 OVERLAY_19_ID;
extern u32 OVERLAY_20_ID;
extern u32 OVERLAY_21_ID;
extern u32 OVERLAY_22_ID;
extern u32 OVERLAY_23_ID;
extern u32 OVERLAY_24_ID;
extern u32 OVERLAY_25_ID;

static void BtlMgr_Init(BtlMgrWork*);
static void BtlMgr_Main(BtlMgrWork*);
static void BtlMgr_Exit(BtlMgrWork*);
static void func_ov003_0208e5f8(BtlMgrWork*);
static void func_ov003_0208e668(BtlMgrWork*);
static void func_ov003_0208e7c0(BtlMgrWork*);

// ---------------------------------------------------------------------------------------------
// Data
// ---------------------------------------------------------------------------------------------

static char data_ov003_020e397c[0x18] = "Apl_Fur/Grp_BtlCmd.bin";
static char data_ov003_020e3994[0x18] = "Apl_Fur/Grp_BtlObs.bin";
static char data_ov003_020e39ac[0x18] = "Apl_Fur/Bin_BtlMenu.bin";
static char data_ov003_020e39c4[0x1C] = "Apl_Fur/Grp_BtlBadge.bin";
static char data_ov003_020e39e0[0x1C] = "Apl_Fur/Grp_BtlPly02.bin";
static char data_ov003_020e39fc[0x1C] = "Apl_Fur/Grp_BtlPly00.bin";
static char data_ov003_020e3a18[0x1C] = "Apl_Fur/Grp_BtlPly01.bin";
static char data_ov003_020e3a34[0x1C] = "Apl_Fur/Grp_BtlPly03.bin";
static char data_ov003_020e3a50[0x1C] = "Apl_Fur/Grp_BtlJingle.bin";
static char data_ov003_020e3a6c[0x1C] = "Apl_Fur/Grp_BtlEffect.bin";
static char data_ov003_020e3a88[0x1C] = "Apl_Fuk/Grp_BtlSpecial.bin";
static char data_ov003_020e3aa4[0x1C] = "BtlMgr.c : BtlMgr_Init()";

/// The heap sequence name the battle work area is tagged with.
static char data_ov003_020e3ac4[16];
char*       data_ov003_020e3ac0     = data_ov003_020e3ac4;
static char data_ov003_020e3ac4[16] = "Seq_Battle()";

/// Experience gained is scaled by a percentage that depends on the battle difficulty.
static const s16 data_ov003_020d77d0[4] = {50, 100, 150, 200};

const BinIdentifier data_ov003_020d77d8 = {3, data_ov003_020e39fc};
const BinIdentifier data_ov003_020d77e0 = {4, data_ov003_020e3a18};
const BinIdentifier data_ov003_020d77e8 = {5, data_ov003_020e39e0};
const BinIdentifier data_ov003_020d77f0 = {6, data_ov003_020e3a34};
const BinIdentifier data_ov003_020d77f8 = {3, data_ov003_020e3a6c};
const BinIdentifier data_ov003_020d7800 = {3, data_ov003_020e39ac};
const BinIdentifier data_ov003_020d7808 = {3, data_ov003_020e397c};
const BinIdentifier data_ov003_020d7810 = {3, data_ov003_020e39c4};
const BinIdentifier data_ov003_020d7818 = {3, data_ov003_020e3a50};
const BinIdentifier data_ov003_020d7820 = {3, data_ov003_020e3994};
const BinIdentifier data_ov003_020d7828 = {3, data_ov003_020e3a88};

/// Encounter id per boss phase, indexed by the chained-battle timer.
static const u16 data_ov003_020d7830[2] = {0x33, 0x34};
static const u16 data_ov003_020d7834[2] = {0x14, 0x15};
static const u16 data_ov003_020d7838[6] = {0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x00};

/// Process stages of the battle overlay: init, main, exit.
static void (*const BtlMgr_ProcessStages[3])(BtlMgrWork*) = {
    BtlMgr_Init,
    BtlMgr_Main,
    BtlMgr_Exit,
};

/// Per-encounter battle track ids.
static const u32 data_ov003_020d7850[18] = {8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 26, 22, 23, 26, 21};

/// Per-chapter set of characters that may join the battle, as a 16-bit mask.
static const u32 data_ov003_020d7898[22] = {1,      1,      0x1003, 0x1003, 0x1007, 0x1087, 0x287, 0xe88,
                                            0xa88,  0x1298, 0x1218, 0x438,  0x438,  0x538,  0xd40, 0xd44,
                                            0x1946, 0x18e6, 0xcf6,  0x77f,  0x37f,  0x1fff};

// ---------------------------------------------------------------------------------------------
// Functions
// ---------------------------------------------------------------------------------------------

/// Maximum HP of the player (or partner) at the battle's current level.
static s32 BtlMgr_CalcMaxHp(void) {
    Ov003Global* g = g_CombatGlobal;
    s32          partner;

    switch (g->unk3D88D) {
        case 0:
            partner = 0;
            break;
        case 1:
            partner = 1;
            break;
        case 2:
            partner = 2;
            break;
        default:
            partner = 0xff;
            break;
    }

    s32 maxHealth = Stats_GetMaxHealth(partner, gSaveData.battleLevel);
    if (CombatAbility_IsActive(3) != 0) {
        maxHealth = (s16)(maxHealth + (s16)CombatAbility_GetValue(3));
    }
    return maxHealth;
}

/// Loads the player/partner/badge overlays and picks the battle track for the encounter.
static void BtlMgr_LoadOverlays(u32 partner, u16 encounterId) {
    s32 overlay4 = CombatEnmOverlay_Table[encounterId];

    switch (partner) {
        case 0:
            OvlMgr_LoadOverlay(3, &OVERLAY_4_ID);
            break;
        case 1:
            OvlMgr_LoadOverlay(3, &OVERLAY_5_ID);
            break;
        case 2:
            OvlMgr_LoadOverlay(3, &OVERLAY_6_ID);
            break;
        case 4:
            OvlMgr_LoadOverlay(3, &OVERLAY_7_ID);
            break;
        default:
            OvlMgr_LoadOverlay(3, &OVERLAY_8_ID);
            break;
    }

    switch (encounterId) {
        case 7:
        case 8:
            OvlMgr_LoadOverlay(4, &OVERLAY_18_ID);
            OvlMgr_LoadOverlay(5, &OVERLAY_10_ID);
            return;

        case 9:
            OvlMgr_LoadOverlay(4, &OVERLAY_9_ID);
            OvlMgr_LoadOverlay(5, &OVERLAY_16_ID);
            return;

        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 32:
        case 33:
            OvlMgr_LoadOverlay(4, &OVERLAY_9_ID);
            OvlMgr_LoadOverlay(5, &OVERLAY_17_ID);
            return;

        case 19:
            OvlMgr_LoadOverlay(4, &OVERLAY_19_ID);
            OvlMgr_LoadOverlay(5, &OVERLAY_10_ID);
            return;

        case 20:
        case 21:
            OvlMgr_LoadOverlay(4, &OVERLAY_20_ID);
            OvlMgr_LoadOverlay(5, &OVERLAY_10_ID);
            return;

        case 22:
            OvlMgr_LoadOverlay(4, &OVERLAY_21_ID);
            OvlMgr_LoadOverlay(5, &OVERLAY_10_ID);
            return;

        case 23:
        case 24:
        case 25:
        case 26:
            OvlMgr_LoadOverlay(4, &OVERLAY_22_ID);
            OvlMgr_LoadOverlay(5, &OVERLAY_10_ID);
            return;

        case 27:
            OvlMgr_LoadOverlay(4, &OVERLAY_23_ID);
            OvlMgr_LoadOverlay(5, &OVERLAY_10_ID);
            return;

        case 28:
        case 29:
        case 30:
        case 31:
            OvlMgr_LoadOverlay(4, &OVERLAY_24_ID);
            OvlMgr_LoadOverlay(5, &OVERLAY_10_ID);
            return;

        default:
            if (encounterId == 0) {
                overlay4 = BtlDebug_GetCurrent();
            }
            switch (overlay4) {
                case 0:
                    OvlMgr_LoadOverlay(4, &OVERLAY_9_ID);
                    OvlMgr_LoadOverlay(5, &OVERLAY_10_ID);
                    return;
                case 1:
                    OvlMgr_LoadOverlay(4, &OVERLAY_9_ID);
                    OvlMgr_LoadOverlay(5, &OVERLAY_11_ID);
                    return;
                case 2:
                    OvlMgr_LoadOverlay(4, &OVERLAY_9_ID);
                    OvlMgr_LoadOverlay(5, &OVERLAY_12_ID);
                    return;
                case 3:
                    OvlMgr_LoadOverlay(4, &OVERLAY_9_ID);
                    OvlMgr_LoadOverlay(5, &OVERLAY_13_ID);
                    return;
                case 4:
                    OvlMgr_LoadOverlay(4, &OVERLAY_9_ID);
                    OvlMgr_LoadOverlay(5, &OVERLAY_14_ID);
                    return;
                case 5:
                    OvlMgr_LoadOverlay(4, &OVERLAY_9_ID);
                    OvlMgr_LoadOverlay(5, &OVERLAY_15_ID);
                    return;
                default:
                    OvlMgr_LoadOverlay(4, &OVERLAY_9_ID);
                    OvlMgr_LoadOverlay(5, &OVERLAY_10_ID);
                    return;
            }
    }
}

/// Returns the partner that fights alongside the player for an encounter. Most encounters force a
/// specific partner; everything else uses the one selected in the phone menu.
static u32 BtlMgr_GetPartner(u16 encounterId) {
    u32 partner;

    switch (gSaveData.playerStats.activePartner) {
        case PARTNER_SHIKI:
            partner = 0;
            break;
        case PARTNER_JOSHUA:
            partner = 1;
            break;
        case PARTNER_BEAT:
            partner = 2;
            break;
        default:
            partner = 5;
            break;
    }

    switch (encounterId) {
        case 7:
        case 9:
        case 19:
            partner = 0;
            break;

        case 8:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 27:
        case 28:
        case 29:
        case 30:
        case 31:
        case 33:
            partner = 2;
            break;

        case 15:
        case 23:
        case 24:
        case 25:
        case 26:
        case 0x88:
            partner = 1;
            break;

        case 20:
            partner = 0;
            break;

        case 21:
            partner = 2;
            break;

        case 22:
            partner = 4;
            break;

        case 50:
        case 51:
        case 52:
        case 56:
        case 57:
        case 58:
            partner = 3;
            break;

        case 59:
            partner = 0;
            break;

        case 60:
            partner = 1;
            break;

        case 61:
            partner = 2;
            break;
    }
    return partner;
}

/// Resets the level-up gauge for the battle that is about to start. When `isContinuation` is set the
/// gauge carried over from the previous chained battle is restored (the player heals back to full
/// between battles).
static void func_ov003_0208aa1c(s32 isContinuation) {
    s32 partner;

    switch (g_CombatGlobal->unk3D88D) {
        case 0:
            partner = 0;
            break;
        case 1:
            partner = 1;
            break;
        case 2:
            partner = 2;
            break;
        default:
            partner = 0xff;
            break;
    }

    g_CombatGlobal->unk3D896 = (partner == 0xff) ? 0 : gSaveData.friendStats[partner].sync;
    g_CombatGlobal->unk3D894 = (CombatAbility_IsActive(0x12) != 0) ? 30 : 100;

    if (CombatAbility_IsActive(0x40) != 0) {
        gSaveData.unk_3140 |= 2;
    }

    g_CombatGlobal->unk3D892 = gSaveData.battleDropRate;
    if (CombatAbility_IsActive(0) != 0) {
        g_CombatGlobal->unk3D892 += (s16)CombatAbility_GetValue(0);
    }

    g_CombatGlobal->unk3D8D2 = BtlMgr_CalcMaxHp();

    if (isContinuation != 0) {
        s32 merged;

        g_CombatGlobal->unk3D8D0 = gSaveData.battleHpGauge;
        g_CombatGlobal->unk3D8D6 = gSaveData.battleHpGaugeSub0;
        g_CombatGlobal->unk3D8D4 = gSaveData.battleHpGaugeSub1;

        merged = g_CombatGlobal->unk3D8D2 - (g_CombatGlobal->unk3D8D0 + g_CombatGlobal->unk3D8D6 + g_CombatGlobal->unk3D8D4);
        if (merged > 0) {
            g_CombatGlobal->unk3D8D0 += (s16)(merged * 0x10000 >> 16);
        }
        if (merged >= 0) {
            return;
        }

        g_CombatGlobal->unk3D8D4 += (s16)(merged * 0x10000 >> 16);
        if (g_CombatGlobal->unk3D8D4 < 0) {
            g_CombatGlobal->unk3D8D6 += g_CombatGlobal->unk3D8D4;
            g_CombatGlobal->unk3D8D4 = 0;
        }
        if (g_CombatGlobal->unk3D8D6 < 0) {
            g_CombatGlobal->unk3D8D0 += g_CombatGlobal->unk3D8D6;
            g_CombatGlobal->unk3D8D6 = 0;
        }
        return;
    }

    g_CombatGlobal->unk3D8D0 = g_CombatGlobal->unk3D8D2;
    g_CombatGlobal->unk3D8D6 = 0;
    g_CombatGlobal->unk3D8D4 = 0;

    if (CombatAbility_IsActiveForPlayer(9) != 0) {
        s32 half = (g_CombatGlobal->unk3D8D2 + (g_CombatGlobal->unk3D8D2 >> 31)) * 0x8000 >> 16;

        g_CombatGlobal->unk3D8D0 -= (s16)half;
        g_CombatGlobal->unk3D8D6 = (s16)half;
    }
    if (CombatAbility_IsActiveForPartner(9) != 0) {
        s32 half = (g_CombatGlobal->unk3D8D2 + (g_CombatGlobal->unk3D8D2 >> 31)) * 0x8000 >> 16;

        g_CombatGlobal->unk3D8D0 -= (s16)half;
        g_CombatGlobal->unk3D8D4 = (s16)half;
    }

    if (g_CombatGlobal->unk3D8D0 < 1) {
        g_CombatGlobal->unk3D8D0 = 1;
        g_CombatGlobal->unk3D8D4--;
    }
}
/// Selects the battle music for the given encounter id.
static void func_ov003_0208ad34(u16 encounterId) {
    u8 music = 0xD;

    BtlBadgeMgr_Clear();

    switch (encounterId) {
        case 0x32:
            music = 0;
            break;
        case 0x33:
            music = 1;
            break;
        case 0x34:
            music = 2;
            break;
        case 0x35:
            music = 3;
            break;
        case 0x36:
            music = 4;
            break;
        case 0x37:
            music = 5;
            break;
        case 0x38:
            music = 6;
            break;
        case 0x39:
            music = 7;
            break;
        case 0x3A:
            music = 8;
            break;
        case 0xD:
            music = 10;
            break;
        case 0xE:
            music = 10;
            break;
        case 0x96:
            music = 0xB;
            break;
    }

    g_CombatGlobal->unk3D890 = music;
    BtlBadgeMgr_QueueAbilities(music);
    BtlBadgeMgr_SpawnBadges(music);
}

/// Points the thread/equipment system at the player's and partner's equipped threads.
static void func_ov003_0208ae24(s32 partner) {
    s32 slot;

    func_ov003_020a300c();
    func_ov003_020a303c(&gSaveData.playerStats.equippedThreads[0], -1);

    switch (partner) {
        case 0:
            slot = 0;
            break;
        case 1:
            slot = 1;
            break;
        case 2:
            slot = 2;
            break;
        default:
            slot = 0xff;
            break;
    }

    if (slot == 0xff) {
        return;
    }
    func_ov003_020a303c(&gSaveData.friendStats[slot].equippedThreads[0], slot);
}

/// Stops a level-up gauge actor and parks it.
static void func_ov003_0208ae98(BattleActor* actor) {
    actor->flags &= ~(ACTOR_FLAG_STAGGERED | ACTOR_FLAG_LAUNCHED);
    actor->flags &= ~ACTOR_FLAG_GUARDING;
    actor->xVelocity = 0;
    actor->yVelocity = 0;
    actor->flags |= ACTOR_FLAG_INTANGIBLE;
    BtlActor_SetPendingCommand(actor, ACTOR_CMD_LEVEL_UP);
}

/// Checks that both unit lists only contain idle, on-screen units.
static BOOL func_ov003_0208aed8(void) {
    Ov003EnemyActor* unit;
    BOOL             result = TRUE;

    for (unit = BtlList_GetHead(&g_CombatGlobal->unk3D8A4); unit != NULL; unit = BtlList_GetNext(&unit->node)) {
        if (unit->flags196.bits.b8 == 1 || (unit->actor.flags & ACTOR_FLAG_DEFEATED) == 0) {
            result = FALSE;
            break;
        }
    }

    for (unit = BtlList_GetHead(&g_CombatGlobal->unk3D8AC); unit != NULL; unit = BtlList_GetNext(&unit->node)) {
        if (unit->flags196.bits.b8 == 1 || (unit->actor.flags & ACTOR_FLAG_DEFEATED) == 0) {
            result = FALSE;
            break;
        }
    }
    return result;
}

/// Adds to the battle clear time and the "damage taken" counter.
void func_ov003_0208af9c(s32 time, s32 amount) {
    gSaveData.battleParTime += time;

    s32 value = gSaveData.unk_318E + amount;
    if (value > 0xFF) {
        value = 0xFF;
    }
    gSaveData.unk_318E = (u8)value;
}

void BtlMgr_AddExp(s32 exp) {
    if (gSaveData.battleMaxLevel >= 100) {
        return;
    }

    s32 remaining = gSaveData.battleExpToNextLevel - exp;

    gSaveData.battleExpToNextLevel = remaining;
    if (g_CombatGlobal->unk3D878 & 0x80000) {
        return;
    }
    if (remaining > 0) {
        return;
    }

    gSaveData.battleLevel++;
    gSaveData.battleMaxLevel++;
    gSaveData.battleExpToNextLevel = gSaveData.battleMaxLevel * 100;

    g_CombatGlobal->unk3D8D2 = BtlMgr_CalcMaxHp();
    g_CombatGlobal->unk3D8D0 = g_CombatGlobal->unk3D8D2;
    g_CombatGlobal->unk3D8D4 = 0;
    g_CombatGlobal->unk3D8D6 = 0;

    g_CombatGlobal->player->maxHp      = g_CombatGlobal->unk3D8D2;
    g_CombatGlobal->player->currentHp  = g_CombatGlobal->player->maxHp;
    g_CombatGlobal->partner->maxHp     = g_CombatGlobal->unk3D8D2;
    g_CombatGlobal->partner->currentHp = g_CombatGlobal->partner->maxHp;

    g_CombatGlobal->unk3D904 = 0;
    g_CombatGlobal->unk3D906 = 0;
    func_ov003_0208ae98(g_CombatGlobal->player);

    g_CombatGlobal->unk3D908 = 0;
    g_CombatGlobal->unk3D90A = 0;
    func_ov003_0208ae98(g_CombatGlobal->partner);

    EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &Tsk_BtlEff_LvUp, NULL, 0, NULL, NULL);
}

/// Awards experience, scaled by the battle difficulty.
static void BtlMgr_AwardExp(s16 exp) {
    if (gSaveData.unk_3130 == 1) {
        return;
    }
    BtlMgr_AddExp((s16)((exp * data_ov003_020d77d0[gSaveData.battleDifficulty]) / 100));
}

/// Adds to the PP earned this battle.
static void BtlMgr_AddPP(s16 amount) {
    gSaveData.battleBasePP += amount;
}

/// Adds an item to the battle drop list, stacking with an existing entry.
BOOL BtlMgr_AddDrop(u32 itemId) {
    BOOL result = FALSE;
    s32  i;

    for (i = 0; i < 32; i++) {
        if (gSaveData.battleDrops[i].itemId == itemId) {
            if (gSaveData.battleDrops[i].count >= 99) {
                break;
            }
            gSaveData.battleDrops[i].count++;
            result = TRUE;
            break;
        }
        if (gSaveData.battleDrops[i].itemId == 0xFFFF) {
            gSaveData.battleDrops[i].itemId = itemId;
            gSaveData.battleDrops[i].count  = 1;
            result                          = TRUE;
            break;
        }
    }
    return result;
}

/// Bumps the battle state, gated on the state we are leaving.
void BtlMgr_SetState(u32 state) {
    // The original reloads the global on every use; keeping a pointer in a local changes the
    // register allocation of the whole function.

    switch (state) {
        case 3:
        case 4:
        case 5:
        case 6:
            if (g_CombatGlobal->unk3D874 == 2) {
                g_CombatGlobal->unk3D874 = state;
            }
            break;

        case 7:
            if (g_CombatGlobal->unk3D874 == 3 || g_CombatGlobal->unk3D874 == 4) {
                g_CombatGlobal->unk3D874 = state;
            }
            break;

        default:
            g_CombatGlobal->unk3D874 = state;
            break;
    }
}

/// Rotates a position by `angle` and adds it.
void BtlMgr_MoveByAngle(s32* x, s32* y, s32 angle, s32 scale) {
    s32 index = (angle & 0xFFFF) >> 4;
    s32 sin   = FX_SinCosTable_[index * 2 + 1];
    s32 cos   = FX_SinCosTable_[index * 2];

    *x += FX_Mul(sin, scale);
    *y += FX_Mul(cos, scale);
}

/// Projects a world position onto the screen.
void BtlMgr_WorldToScreen(s16* screenX, s16* screenY, s32 x, s32 y, s32 z) {
    BattleCamera* camera = &g_CombatGlobal->unk3D7C0[0];

    *screenX = (s16)((camera->unk_04 + (x - camera->unk_28) + 0x80000) >> 12);
    *screenY = (s16)((camera->unk_08 + ((y + z) - camera->unk_40) + 0x60000) >> 12);
}

/// Adds (or removes) HP on the shared player/partner gauge. `bank` selects which of the two banked gauges the
/// delta is taken from when the gauge overflows.
void BtlMgr_AddTeamHp(s32 bank, s16 delta) {
    if (gSaveData.unk_3130 == 1) {
        return;
    }

    if (delta > 0) {
        s32 room = g_CombatGlobal->unk3D8D2 - g_CombatGlobal->unk3D8D0;

        if (room < delta) {
            delta = (s16)room;
        }
    } else {
        if (g_CombatGlobal->unk3D8D0 < -delta) {
            delta = (s16)-g_CombatGlobal->unk3D8D0;
        }
    }

    g_CombatGlobal->unk3D8D0 += delta;

    if (bank == 0) {
        g_CombatGlobal->unk3D8D6 -= delta;
        if (g_CombatGlobal->unk3D8D6 < 0) {
            g_CombatGlobal->unk3D8D4 += g_CombatGlobal->unk3D8D6;
            g_CombatGlobal->unk3D8D6 = 0;
        }
    } else {
        g_CombatGlobal->unk3D8D4 -= delta;
        if (g_CombatGlobal->unk3D8D4 < 0) {
            g_CombatGlobal->unk3D8D6 += g_CombatGlobal->unk3D8D4;
            g_CombatGlobal->unk3D8D4 = 0;
        }
    }

    g_CombatGlobal->player->currentHp  = g_CombatGlobal->unk3D8D0;
    g_CombatGlobal->partner->currentHp = g_CombatGlobal->unk3D8D0;
}

/// Adds (or removes) HP on a unit. Units flagged ACTOR_FLAG_BOSS_GAUGE_HP use the boss gauge instead of their own
/// HP field.
void BtlMgr_AddUnitHp(s32 bank, BattleActor* actor, s16 delta) {
    // The original reloads the global on every use; keeping a pointer in a local changes the
    // register allocation of the whole function.

    if ((actor->flags & ACTOR_FLAG_BOSS_GAUGE_HP) == 0) {
        if (delta > 0) {
            s32 room = actor->maxHp - actor->currentHp;

            if (room < delta) {
                delta = (s16)room;
            }
        }
        actor->currentHp += delta;
        if (actor->linkedActor != NULL) {
            actor->linkedActor->currentHp = actor->currentHp;
        }
        return;
    }

    if (delta > 0) {
        s32 room = g_CombatGlobal->unk3D8DA - g_CombatGlobal->unk3D8D8;

        if (room < delta) {
            delta = (s16)room;
        }
    } else {
        if (g_CombatGlobal->unk3D8D8 < -delta) {
            delta = (s16)-g_CombatGlobal->unk3D8D8;
        }
    }

    g_CombatGlobal->unk3D8D8 += delta;

    if (bank == 0) {
        g_CombatGlobal->unk3D8DE -= delta;
        if (g_CombatGlobal->unk3D8DE < 0) {
            g_CombatGlobal->unk3D8DC += g_CombatGlobal->unk3D8DE;
            g_CombatGlobal->unk3D8DE = 0;
        }
    } else {
        g_CombatGlobal->unk3D8DC -= delta;
        if (g_CombatGlobal->unk3D8DC < 0) {
            g_CombatGlobal->unk3D8DE += g_CombatGlobal->unk3D8DC;
            g_CombatGlobal->unk3D8DC = 0;
        }
    }

    actor->currentHp = g_CombatGlobal->unk3D8D8;
    if (actor->linkedActor != NULL) {
        actor->linkedActor->currentHp = actor->currentHp;
    }
}

/// True for the two encounters that are allowed to take damage while the battle is paused.
BOOL func_ov003_0208b690(u16 encounterId) {
    if ((u16)(encounterId + 0xFFFF) <= 1U) {
        return (g_CombatGlobal->unk3D878 & 6) != 0;
    }
    return FALSE;
}

static void func_ov003_0208b6d4(void) {
    BtlMgr_SetState(1);
    EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &Tsk_BtlReady, NULL, 0, NULL, NULL);
}

/// Advances the battle state machine. Runs every frame from the battle task; decides when the battle
/// is won and switches to the result screen.
static void func_ov003_0208b71c(void) {
    BOOL             finished = FALSE;
    Ov003EnemyActor* unit;

    if (g_CombatGlobal->unk3D874 != 2) {
        return;
    }

    switch (g_CombatGlobal->unk3D875) {
        case 0:
            unit = BtlList_GetHead(&g_CombatGlobal->unk3D8A4);
            if (unit == NULL && BtlList_GetHead(&g_CombatGlobal->unk3D8AC) == NULL) {
                finished = TRUE;
            }
            break;

        case 1:
            if (g_CombatGlobal->unk3D8DA > 0) {
                finished = g_CombatGlobal->unk3D8D8 <= 0;
            }
            break;

        case 2:
            if (g_CombatGlobal->unk3D878 & 0x20000000) {
                finished = TRUE;
            }
            break;
    }

    if (finished || func_ov003_0208aed8()) {
        g_CombatGlobal->unk3D878 |= 0x30000;
        g_CombatGlobal->unk3D878 |= 0x4000;
        g_CombatGlobal->unk3D878 |= 0x8000;
        g_CombatGlobal->player->flags |= ACTOR_FLAG_INTANGIBLE;
        g_CombatGlobal->partner->flags |= ACTOR_FLAG_INTANGIBLE;
    }

    if (func_ov003_020bb47c() != 0) {
        return;
    }
    if (!finished) {
        return;
    }

    if (g_CombatGlobal->unk3D878 & 0x200000) {
        g_CombatGlobal->unk3D878 |= 0x20;
        BtlMgr_SetState(6);
        g_CombatGlobal->player->flags &= ~ACTOR_FLAG_INTANGIBLE;
        g_CombatGlobal->partner->flags &= ~ACTOR_FLAG_INTANGIBLE;
        return;
    }

    if (EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &Tsk_BtlWin, NULL, 0, NULL, NULL) != -1) {
        BtlMgr_SetState(3);
        g_CombatGlobal->player->flags &= ~ACTOR_FLAG_INTANGIBLE;
        g_CombatGlobal->partner->flags &= ~ACTOR_FLAG_INTANGIBLE;
    }
}

/// Allocates and initialises the battle globals, loads the battle graphics and spawns every battle
/// task.
static void func_ov003_0208b944(u16 encounterId, s32 isContinuation) {
    void* paletteSrc;
    void* alloc;

    alloc = Mem_AllocHeapTail(&gDebugHeap, 0x3D910);
    Mem_SetSequence(&gDebugHeap, alloc, data_ov003_020e3aa4);
    g_CombatGlobal = alloc;
    MI_CpuSet(alloc, 0, 0x3D910);

    g_CombatGlobal->unk3D350 = DatMgr_LoadRawData(1, NULL, 0, (BinIdentifier*)&data_ov003_020d7800);
    g_CombatGlobal->unk3D354 = DatMgr_LoadRawData(1, NULL, 0, (BinIdentifier*)&data_ov003_020d77f8);

    paletteSrc                  = Data_GetPackEntryData(g_CombatGlobal->unk3D350, 65);
    g_CombatGlobal->unk3D348[0] = PaletteMgr_AcquireContiguous(g_PaletteManagers[0], paletteSrc, 5, TRUE);
    g_CombatGlobal->unk3D348[1] = PaletteMgr_AcquireContiguous(g_PaletteManagers[1], paletteSrc, 5, TRUE);

    Mem_InitializeHeap(&g_CombatGlobal->heap_00080, g_CombatGlobal->heapData_00080, 0x8000);
    Mem_InitializeHeap(&g_CombatGlobal->heap_0810C, g_CombatGlobal->heapData_0810C, 0x8000);
    Mem_InitializeHeap(&g_CombatGlobal->heap_10198, g_CombatGlobal->heapData_10198, 0x10000);
    Mem_InitializeHeap(&g_CombatGlobal->heap_20224, g_CombatGlobal->heapData_20224, 0x14000);
    Mem_InitializeHeap(&g_CombatGlobal->heap_342B0, g_CombatGlobal->heapData_342B0, 0x8000);
    Mem_InitializeHeap(&g_CombatGlobal->heap_3C33C, g_CombatGlobal->heapData_3C33C, 0x1000);

    EasyTask_InitializePool(&g_CombatGlobal->unk_00000, &g_CombatGlobal->heap_00080, 0x80, NULL, NULL);
    EasyTask_InitializePool(&g_CombatGlobal->taskPool, &g_CombatGlobal->heap_0810C, 0x80, NULL, NULL);
    EasyTask_InitializePool(&g_CombatGlobal->unk_10118, &g_CombatGlobal->heap_10198, 0x80, NULL, NULL);
    EasyTask_InitializePool(&g_CombatGlobal->unk_201A4, &g_CombatGlobal->heap_20224, 0x100, NULL, NULL);
    EasyTask_InitializePool(&g_CombatGlobal->unk_34230, &g_CombatGlobal->heap_342B0, 0x80, NULL, NULL);
    EasyTask_InitializePool(&g_CombatGlobal->unk_3C2BC, &g_CombatGlobal->heap_3C33C, 0x10, NULL, NULL);

    g_CombatGlobal->unk3D90C       = 0;
    g_CombatGlobal->unk3D875       = 0;
    g_CombatGlobal->unk3D878       = 0;
    g_CombatGlobal->unk3D87C       = 0;
    g_CombatGlobal->unk3D880       = 0;
    g_CombatGlobal->unk3D884       = 0;
    g_CombatGlobal->unk3D888       = 0;
    g_CombatGlobal->unk3D8D8       = 0;
    g_CombatGlobal->unk3D8DA       = 0;
    g_CombatGlobal->unk3D8DC       = 0;
    g_CombatGlobal->unk3D8DE       = 0;
    g_CombatGlobal->unk3D8F8       = 0;
    g_CombatGlobal->unk3D8FA       = 0;
    g_CombatGlobal->popupCountMain = 0;
    g_CombatGlobal->popupCountSub  = 0;
    g_CombatGlobal->unk3D8FE       = 0;
    g_CombatGlobal->unk3D8FF       = 0;
    g_CombatGlobal->unk3D900       = 0;
    g_CombatGlobal->unk3D902       = 0;
    g_CombatGlobal->unk3D8A0       = NULL;
    g_CombatGlobal->unk3D88C       = gSaveData.unk_3142 - 1;

    g_CombatGlobal->unk3D88D = BtlMgr_GetPartner(encounterId);
    g_CombatGlobal->unk3D88E = encounterId;
    g_CombatGlobal->unk3D870 = gSaveData.unk_313C;

    if (gSaveData.battleCount > 1) {
        g_CombatGlobal->unk3D878 |= 0x80000000;
    }

    BtlMgr_LoadOverlays(g_CombatGlobal->unk3D88D, g_CombatGlobal->unk3D88E);

    BtlTouch_Init();
    BtlMic_Init();
    BtlStage_SetupCameras();
    BtlCollision_Init();
    func_ov003_02087378();
    CombatSys_ResetObsList();

    g_CombatGlobal->chargeState = NULL;

    func_ov003_0208ae24(g_CombatGlobal->unk3D88D);
    func_ov003_0208ad34(g_CombatGlobal->unk3D88E);
    func_ov003_0208aa1c(isContinuation);

    BtlList_Init(&g_CombatGlobal->unk3D8A4);
    BtlList_Init(&g_CombatGlobal->unk3D8AC);

    EasyTask_CreateTask(&g_CombatGlobal->unk_00000, &Tsk_BtlPlayer00, NULL, 0, NULL, NULL);

    switch (g_CombatGlobal->unk3D88D) {
        case 0:
            EasyTask_CreateTask(&g_CombatGlobal->taskPool, &Tsk_BtlPlayer01, NULL, 0, NULL, NULL);
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &Tsk_BtlCard_Esp, NULL, 0, NULL, NULL);
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &Tsk_BtlAura, NULL, 0, NULL, NULL);
            break;

        case 1:
            EasyTask_CreateTask(&g_CombatGlobal->taskPool, &data_ov004_020f0120, NULL, 0, NULL, NULL);
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov005_020f0394, NULL, 0, NULL, NULL);
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &Tsk_BtlAura, NULL, 0, NULL, NULL);
            break;

        case 2:
            EasyTask_CreateTask(&g_CombatGlobal->taskPool, &data_ov005_020f07a0, NULL, 0, NULL, NULL);
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov005_020f0a94, NULL, 0, NULL, NULL);
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &Tsk_BtlAura, NULL, 0, NULL, NULL);
            break;

        case 4:
            EasyTask_CreateTask(&g_CombatGlobal->taskPool, &Tsk_BtlPlayerLast, NULL, 0, NULL, NULL);
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &Tsk_BtlAuraLast, NULL, 0, NULL, NULL);
            break;

        default:
            EasyTask_CreateTask(&g_CombatGlobal->taskPool, &Tsk_BtlPlayerNone, NULL, 0, NULL, NULL);
            break;
    }

    func_ov003_020c2a34(g_CombatGlobal->unk3D88E);
    BtlStage_StartFieldTasks(0);
    CombatSys_RollObsDrop();
    BtlMic_CreateTask();

    if (g_CombatGlobal->unk3D88D < 3) {
        EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &Tsk_BtlCmd, NULL, 0, NULL, NULL);
        EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &Tsk_BtlInf_Star, NULL, 0, NULL, NULL);
    }

    EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &Tsk_BtlBarHP, NULL, 0, NULL, NULL);
    EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &Tsk_BtlTarget, NULL, 0, NULL, NULL);
    EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &Tsk_BtlPop_Combo, NULL, 0, NULL, NULL);
    EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &Tsk_BtlInf_Exp, NULL, 0, NULL, NULL);
    EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &Tsk_BtlInf_Auto, NULL, 0, NULL, NULL);
    EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov003_020dc614, NULL, 0, NULL, NULL);

    if (CombatAbility_IsActive(4) != 0) {
        EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &Tsk_BtlInf_Timer, NULL, 0, NULL, NULL);
    }

    EasyTask_CreateTask(&g_CombatGlobal->unk_3C2BC, &Task_BtlTch, NULL, 0, NULL, NULL);
    EasyTask_CreateTask(&g_CombatGlobal->unk_3C2BC, &Tsk_BtlPause, NULL, 0, NULL, NULL);

    BtlStage_StartSandstorm();
    BtlDbg_CreateTask();
    BtlDebug_ShowSyncValues();
}

/// Per-frame battle update: input, combo tracking, task pools.
static void func_ov003_0208c2a8(void) {
    // The original reloads the global on every use; keeping a pointer in a local changes the
    // register allocation of the whole function.
    s32 pressed;

    BtlTouch_Update();
    func_ov003_020873e0();
    EasyTask_ProcessPendingTasks(&g_CombatGlobal->unk_3C2BC);

    if (BtlBadgeMgr_AnyActive() != 0) {
        pressed = (InputStatus.buttonState.currButtons & (INPUT_BUTTON_L | INPUT_BUTTON_R)) != 0;
        if (g_CombatGlobal->unk3D880 != pressed) {
            CombatSe_Play(SEIDX_SE_WEAPON_SWITCH);
            g_CombatGlobal->unk3D880 = pressed;
        }
    }

    if (g_CombatGlobal->unk3D87C != 0) {
        BtlBadgeMgr_DebugDrag();
        return;
    }

    BtlBadgeActor_SeparateAll();

    if (g_CombatGlobal->unk3D874 == 2 && (g_CombatGlobal->unk3D878 & 6) == 0) {
        gSaveData.battleClearTime++;
        if (gSaveData.battleClearTime > 0x57E40) {
            gSaveData.battleClearTime = 0x57E40;
        }
    }

    EasyTask_ProcessPendingTasks(&g_CombatGlobal->unk_34230);

    if (g_CombatGlobal->unk3D884 != 0) {
        return;
    }

    if (g_CombatGlobal->unk3D888 == 0) {
        EasyTask_ProcessPendingTasks(&g_CombatGlobal->unk_10118);
        if ((g_CombatGlobal->unk3D878 & 0x40) == 0) {
            EasyTask_ProcessPendingTasks(&g_CombatGlobal->unk_201A4);
        }
    }

    if (BtlDebug_GetSetting(BTL_DEBUG_SETTING_MODE) == BTL_DEBUG_MODE_NO_FRAME_DELAY) {
        EasyTask_ProcessPendingTasks(&g_CombatGlobal->unk_00000);
        EasyTask_ProcessPendingTasks(&g_CombatGlobal->taskPool);
        BtlCollision_Update();
        return;
    }

    if (g_CombatGlobal->unk3D904 > 0) {
        g_CombatGlobal->unk3D904--;
    } else {
        g_CombatGlobal->unk3D904 = g_CombatGlobal->unk3D906;
        g_CombatGlobal->unk3D906 = 0;
        EasyTask_ProcessPendingTasks(&g_CombatGlobal->unk_00000);
        BtlCollision_Update();
    }

    if (g_CombatGlobal->unk3D908 > 0) {
        g_CombatGlobal->unk3D908--;
        return;
    }
    g_CombatGlobal->unk3D908 = g_CombatGlobal->unk3D90A;
    g_CombatGlobal->unk3D90A = 0;
    EasyTask_ProcessPendingTasks(&g_CombatGlobal->taskPool);
}

/// Per-frame task ticking.
static void func_ov003_0208c4bc(void) {
    // The original reloads the global on every use; keeping a pointer in a local changes the
    // register allocation of the whole function.

    EasyTask_UpdateActiveTasks(&g_CombatGlobal->unk_3C2BC);
    EasyTask_UpdateActiveTasks(&g_CombatGlobal->unk_34230);

    if (g_CombatGlobal->unk3D884 != 0) {
        return;
    }

    EasyTask_UpdateActiveTasks(&g_CombatGlobal->unk_10118);
    EasyTask_UpdateActiveTasks(&g_CombatGlobal->unk_201A4);
    EasyTask_UpdateActiveTasks(&g_CombatGlobal->unk_00000);
    EasyTask_UpdateActiveTasks(&g_CombatGlobal->taskPool);
    BtlDebug_DrawCollision();
}

/// Tears down the battle globals.
static void func_ov003_0208c54c(s32 keepResources) {
    // The original reloads the global on every use; keeping a pointer in a local changes the
    // register allocation of the whole function.

    gSaveData.battleHpGauge     = g_CombatGlobal->unk3D8D0;
    gSaveData.battleHpGaugeSub0 = g_CombatGlobal->unk3D8D6;
    gSaveData.battleHpGaugeSub1 = g_CombatGlobal->unk3D8D4;

    BtlBadgeMgr_PublishSlots();

    EasyTask_DestroyPool(&g_CombatGlobal->unk_00000);
    EasyTask_DestroyPool(&g_CombatGlobal->taskPool);
    EasyTask_DestroyPool(&g_CombatGlobal->unk_10118);
    EasyTask_DestroyPool(&g_CombatGlobal->unk_201A4);
    EasyTask_DestroyPool(&g_CombatGlobal->unk_34230);
    EasyTask_DestroyPool(&g_CombatGlobal->unk_3C2BC);

    if (keepResources == 0) {
        OvlMgr_UnloadOverlay(3);
        OvlMgr_UnloadOverlay(4);
        OvlMgr_UnloadOverlay(5);
    }

    PaletteMgr_ReleaseResource(g_PaletteManagers[0], g_CombatGlobal->unk3D348[0]);
    PaletteMgr_ReleaseResource(g_PaletteManagers[1], g_CombatGlobal->unk3D348[1]);
    DatMgr_ReleaseData(g_CombatGlobal->unk3D350);
    DatMgr_ReleaseData(g_CombatGlobal->unk3D354);
    Mem_Free(&gDebugHeap, g_CombatGlobal);
}

/// Number of the player's first stat pin owned.
static s32 func_ov003_0208c694(void) {
    switch (g_CombatGlobal->unk3D88D) {
        case 0:
            return (s16)Inventory_GetOwnedCount(0x2B2);
        case 1:
            return (s16)Inventory_GetOwnedCount(0x2BA);
        case 2:
            return (s16)Inventory_GetOwnedCount(0x2C2);
        default:
            return 0;
    }
}

/// Number of the player's second stat pin owned.
static s32 func_ov003_0208c718(void) {
    switch (g_CombatGlobal->unk3D88D) {
        case 0:
            return (s16)Inventory_GetOwnedCount(0x2B3);
        case 1:
            return (s16)Inventory_GetOwnedCount(0x2BB);
        case 2:
            return (s16)Inventory_GetOwnedCount(0x2C3);
        default:
            return 0;
    }
}

/// True when `unit` is a valid target sitting on the requested side of the screen.
BOOL BtlMgr_IsTargetOnSide(Ov003EnemyActor* unit, s32 side) {
    if ((unit->actor.flags & ACTOR_FLAG_DEFEATED) || (unit->actor.flags & ACTOR_FLAG_INTANGIBLE)) {
        return FALSE;
    }
    if (side != 0) {
        if (g_CombatGlobal->unk3D7C0[1].unk_20 <= unit->actor.position.x) {
            return FALSE;
        }
        return TRUE;
    }
    return unit->actor.position.x > g_CombatGlobal->unk3D7C0[1].unk_20;
}

/// Finds the closest valid target to `x` on the given side, preferring airborne or grounded units as
/// requested.
Ov003EnemyActor* BtlMgr_FindNearestTarget(s32 x, s32 side, s32 airborne) {
    Ov003Global*     g        = g_CombatGlobal;
    Ov003EnemyActor* best     = NULL;
    s32              bestDist = 0x100000;
    Ov003EnemyActor* unit;

    for (unit = BtlList_GetHead(&g->unk3D8AC); unit != NULL; unit = BtlList_GetNext(&unit->node)) {
        s32 dist;
        s32 absDist;

        if (!BtlMgr_IsTargetOnSide(unit, side)) {
            continue;
        }
        if ((unit->actor.position.z <= -0x40000) != airborne) {
            continue;
        }
        dist    = unit->actor.position.x - x;
        absDist = ABS(dist);
        if (bestDist <= absDist) {
            continue;
        }
        bestDist = absDist;
        best     = unit;
    }
    return best;
}

/// Finds the farthest valid target to `x` on the given side.
Ov003EnemyActor* BtlMgr_FindFarthestTarget(s32 x, s32 side, s32 airborne) {
    Ov003Global*     g        = g_CombatGlobal;
    Ov003EnemyActor* best     = NULL;
    s32              bestDist = 0;
    Ov003EnemyActor* unit;

    for (unit = BtlList_GetHead(&g->unk3D8AC); unit != NULL; unit = BtlList_GetNext(&unit->node)) {
        s32 dist;
        s32 absDist;

        if (!BtlMgr_IsTargetOnSide(unit, side)) {
            continue;
        }
        if ((unit->actor.position.z <= -0x40000) != airborne) {
            continue;
        }
        dist    = unit->actor.position.x - x;
        absDist = ABS(dist);
        if (bestDist >= absDist) {
            continue;
        }
        bestDist = absDist;
        best     = unit;
    }
    return best;
}

/// Awards experience; the "stat pin" bonus increases the amount.
void BtlMgr_GainExp(s32 mode, s16 exp) {
    if (CombatAbility_IsActive(0x1F) != 0) {
        return;
    }
    if (BtlMgr_HasAbility(mode, 0x1C) != 0) {
        s16 bonus = BtlMgr_GetAbilityValue(mode, 0x1C);

        exp += (s16)((exp * bonus) / 100);
    }
    BtlMgr_AwardExp(exp);
}

/// Awards PP; the "stat pin" bonus increases the amount.
void BtlMgr_GainPP(s32 mode, s16 pp) {
    if (CombatAbility_IsActive(0x1E) != 0) {
        return;
    }
    BtlMgr_AddPP(pp);
}

BOOL BtlMgr_HasAbility(s32 mode, u32 effect) {
    if (mode != 1) {
        return CombatAbility_IsActiveForPlayer(effect);
    }
    return CombatAbility_IsActiveForPartner(effect);
}

s16 BtlMgr_GetAbilityValue(s32 mode, u32 effect) {
    if (mode != 1) {
        return (s16)CombatAbility_GetPlayerValue(effect);
    }
    return (s16)CombatAbility_GetPartnerValue(effect);
}

/// Attack value used by the battle HUD, including the pin bonus when requested.
s32 func_ov003_0208ca54(s32 includePins) {
    u32 partner;

    switch (g_CombatGlobal->unk3D88D) {
        case 0:
            partner = 0;
            break;
        case 1:
            partner = 1;
            break;
        case 2:
            partner = 2;
            break;
        default:
            partner = 0xff;
            break;
    }

    if (includePins != 1) {
        return Stats_GetEffectiveValue(partner, 0);
    }
    return (s16)(func_ov003_0208c694() + Stats_GetEffectiveFriendValue(partner, 0));
}

/// Defense value used by the battle HUD, including the pin bonus when requested.
s32 func_ov003_0208cae0(s32 includePins) {
    u32 partner;

    switch (g_CombatGlobal->unk3D88D) {
        case 0:
            partner = 0;
            break;
        case 1:
            partner = 1;
            break;
        case 2:
            partner = 2;
            break;
        default:
            partner = 0xff;
            break;
    }

    if (includePins != 1) {
        return Stats_GetEffectiveValue(partner, 1);
    }
    return (s16)(func_ov003_0208c718() + Stats_GetEffectiveFriendValue(partner, 1));
}

/// Records that the battle saw a hit of the given kind and remembers the largest one.
void func_ov003_0208cb6c(s32 kind, s32 damage) {
    if (kind == 1) {
        gSaveData.unk_317D |= 2;
    } else {
        gSaveData.unk_317D |= 1;
    }

    if (gSaveData.unk_317E < damage) {
        gSaveData.unk_317E = damage;
    }
}

/// Bumps the battle's combo-length counter.
void func_ov003_0208cbac(void) {
    if (gSaveData.unk_318F != 0xFF) {
        gSaveData.unk_318F++;
    }
}

/// Extends the battle's longest combo.
void func_ov003_0208cbc8(void) {
    // The original reloads the global on every use; keeping a pointer in a local changes the
    // register allocation of the whole function.

    g_CombatGlobal->unk3D8ED++;
    if (gSaveData.unk_317C < g_CombatGlobal->unk3D8ED) {
        gSaveData.unk_317C = g_CombatGlobal->unk3D8ED;
    }
}

/// Fills in the battle bonus flags from the damage/combo records.
static void func_ov003_0208cc14(void) {
    Ov003Global* g = g_CombatGlobal;

    if (CombatAbility_IsActive(0x4B) != 0) {
        s32 pp = gSaveData.battleBasePP * CombatAbility_GetValue(0x4B);

        gSaveData.battleBasePP = (s16)((pp / 100) - (pp >> 31));
    }

    if (g->unk3D880 == 0) {
        return;
    }

    if (gSaveData.unk_318F == 0) {
        gSaveData.battleBonusFlags |= 1;
    }
    if (gSaveData.battleMaxHits >= 1000) {
        gSaveData.battleBonusFlags |= 0x80;
    }
    if (gSaveData.battleMaxHits >= 500) {
        gSaveData.battleBonusFlags |= 0x40;
    }
    if (gSaveData.battleMaxHits >= 200) {
        gSaveData.battleBonusFlags |= 0x20;
    }
    if (gSaveData.battleMaxHits >= 100) {
        gSaveData.battleBonusFlags |= 0x10;
    }
    if (gSaveData.battleMaxHits >= 50) {
        gSaveData.battleBonusFlags |= 8;
    }
    if (gSaveData.battleMaxHits >= 30) {
        gSaveData.battleBonusFlags |= 4;
    }
    if (gSaveData.battleMaxHits >= 10) {
        gSaveData.battleBonusFlags |= 2;
    }
    if (gSaveData.unk_317C >= 100) {
        gSaveData.battleBonusFlags |= 0x800;
    }
    if (gSaveData.unk_317C >= 50) {
        gSaveData.battleBonusFlags |= 0x400;
    }
    if (gSaveData.unk_317C >= 30) {
        gSaveData.battleBonusFlags |= 0x200;
    }
    if (gSaveData.unk_317C >= 10) {
        gSaveData.battleBonusFlags |= 0x100;
    }
    if ((gSaveData.unk_317D & 2) == 0) {
        gSaveData.battleBonusFlags |= 0x2000;
    } else if ((gSaveData.unk_317D & 1) == 0) {
        gSaveData.battleBonusFlags |= 0x1000;
    }
    if (gSaveData.battleDrops[0].itemId == 0xFFFF) {
        gSaveData.battleBonusFlags |= 0x4000;
    }
    if (gSaveData.playerStats.difficulty == 0) {
        gSaveData.battleBonusFlags |= 0x8000;
    }

    u32 partner;

    switch (g->unk3D88D) {
        case 0:
            partner = 0;
            break;
        case 1:
            partner = 1;
            break;
        case 2:
            partner = 2;
            break;
        default:
            partner = 0xff;
            break;
    }
    if (func_02023dd4(partner) != 0) {
        gSaveData.battleBonusFlags |= 0x10000;
    }

    if (gSaveData.unk_317E >= 9999) {
        gSaveData.battleBonusFlags |= 0x800000;
    }
    if (gSaveData.unk_317E >= 5000) {
        gSaveData.battleBonusFlags |= 0x400000;
    }
    if (gSaveData.unk_317E >= 3000) {
        gSaveData.battleBonusFlags |= 0x200000;
    }
    if (gSaveData.unk_317E >= 2000) {
        gSaveData.battleBonusFlags |= 0x100000;
    }
    if (gSaveData.unk_317E >= 1000) {
        gSaveData.battleBonusFlags |= 0x80000;
    }
    if (gSaveData.unk_317E >= 500) {
        gSaveData.battleBonusFlags |= 0x40000;
    }
}

/// Saves progress when the battle was won normally.
static void func_ov003_0208cf94(u32 unused) {
    if (gSaveData.unk_3130 != 2) {
        return;
    }
    if (gSaveData.battleDifficulty < 2) {
        return;
    }
    if (gSaveData.unk_3134 == 0x31) {
        gSaveData.battleParTime = 0x11490;
        if (gSaveData.experience.unk_8 != 0 && gSaveData.experience.unk_8 <= gSaveData.battleClearTime) {
            return;
        }
        gSaveData.experience.unk_8 = gSaveData.battleClearTime;
        return;
    }

    func_ov003_020bb190(unused, gSaveData.battleClearTime, gSaveData.unk_3134);
}

/// Initialises the battle's graphics hardware and display settings.
static void func_ov003_0208d010(void) {
    Interrupts_Init();
    HBlank_Init();
    GX_Init();
    G3X_Init();
    G3X_InitMtxStack();
    DMA_Init(0x100);
    Display_Init();

    g_DisplaySettings.controls[DISPLAY_MAIN].hBlank = 0;
    g_DisplaySettings.controls[DISPLAY_SUB].hBlank  = 0;

    GX_DisableBankForLcdc();
    GX_SetBankForLcdc(0x1FF);
    GX_SetBankForTex(1);
    GX_SetBankForObj(2);
    GX_SetBankForBg(0x30);
    GX_SetBankForTexPltt(0x40);
    GX_SetBankForSubBg(4);
    GX_SetBankForSubObj(8);
    GX_SetBankForSubObjExtPltt(0x100);

    OamMgr_Init3DSpritePipeline();

    g_DisplaySettings.controls[DISPLAY_MAIN].objTileMode = 0x200010;
    g_DisplaySettings.controls[DISPLAY_MAIN].objBmpMode  = 0x40;
    g_DisplaySettings.controls[DISPLAY_SUB].objTileMode  = 0x200010;
    g_DisplaySettings.controls[DISPLAY_SUB].objBmpMode   = 0x40;
    data_0206aa78                                        = 0x200010;
    data_0206aa7c                                        = 0x40;

    OamMgr_InitEngine(0, DISPLAY_MAIN);
    OamMgr_InitEngine(0, DISPLAY_SUB);
    OamMgr_InitEngine(0, DISPLAY_EXTENDED);

    MI_CpuFill(0, (void*)0x06000000, 0x80000);
    MI_CpuFill(0, (void*)0x06800000, 0xA4000);

    OamMgr_ResetAndCommit();

    OamMgr_SetAffineCount(&g_OamMgr[DISPLAY_EXTENDED], 0);

    Color_CopyRange(g_PaletteBuffers[DISPLAY_MAIN].bg, data_0205a128, 0, 0x200, TRUE);
    Color_CopyRange(g_PaletteBuffers[DISPLAY_SUB].bg, data_0205a128, 0, 0x200, TRUE);
}

/// Configures the battle's background layers and blend settings.
static void func_ov003_0208d1d0(void) {
    DisplayBGSettings*      bg;
    DisplayControlSettings* main = Display_GetMainControls();
    DisplayControlSettings* sub  = Display_GetSubControls();

    main->brightness = 16;
    sub->brightness  = 16;
    Display_CommitSynced();

    g_DisplaySettings.unk_000 = 0;
    REG_DISPCNT_SUB           = REG_DISPCNT_SUB & 0x7FFF;

    main->dispMode  = 1;
    main->bgMode    = 0;
    main->dimension = 1;
    GX_SetGraphicsMode(main->dispMode, main->bgMode, main->dimension);

    bg                 = Display_GetBGSettings(DISPLAY_MAIN, DISPLAY_BG1);
    bg->bgMode         = 0;
    bg->screenSizeText = 1;
    bg->colorMode      = 0;
    bg->screenBase     = 0;
    bg->charBase       = 1;
    bg->extPlttSlot    = 0;
    REG_BG0CNT         = (REG_BG0CNT & 0x43) | 0x4004;

    bg                 = Display_GetBGSettings(DISPLAY_MAIN, DISPLAY_BG2);
    bg->bgMode         = 0;
    bg->screenSizeText = 1;
    bg->colorMode      = 0;
    bg->screenBase     = 2;
    bg->charBase       = 3;
    bg->extPlttSlot    = 1;
    REG_BG1CNT         = (REG_BG1CNT & 0x43) | 0x420C;

    bg                 = Display_GetBGSettings(DISPLAY_MAIN, DISPLAY_BG3);
    bg->bgMode         = 0;
    bg->screenSizeText = 0;
    bg->colorMode      = 0;
    bg->screenBase     = 4;
    bg->charBase       = 1;
    bg->extPlttSlot    = 1;
    REG_BG2CNT         = (REG_BG2CNT & 0x43) | 0x404;

    Display_GetBGSettings(DISPLAY_MAIN, DISPLAY_BG0)->priority = 1;
    Display_GetBGSettings(DISPLAY_MAIN, DISPLAY_BG1)->priority = 2;
    Display_GetBGSettings(DISPLAY_MAIN, DISPLAY_BG2)->priority = 2;
    Display_GetBGSettings(DISPLAY_MAIN, DISPLAY_BG3)->priority = 3;
    Display_GetBGSettings(DISPLAY_MAIN, DISPLAY_BG0)->mosaic   = 0;
    Display_GetBGSettings(DISPLAY_MAIN, DISPLAY_BG1)->mosaic   = 0;
    Display_GetBGSettings(DISPLAY_MAIN, DISPLAY_BG2)->mosaic   = 0;
    Display_GetBGSettings(DISPLAY_MAIN, DISPLAY_BG3)->mosaic   = 0;
    main->layers                                               = 0x1F;
    main->charBase                                             = 0;
    GXs_SetGraphicsMode(0);

    bg                 = Display_GetBGSettings(DISPLAY_SUB, DISPLAY_BG0);
    bg->bgMode         = 0;
    bg->screenSizeText = 1;
    bg->colorMode      = 1;
    bg->screenBase     = 6;
    bg->charBase       = 5;
    bg->extPlttSlot    = 0;
    REG_BG0CNT_SUB     = (REG_BG0CNT_SUB & 0x43) | 0x4694;

    bg                 = Display_GetBGSettings(DISPLAY_SUB, DISPLAY_BG1);
    bg->bgMode         = 0;
    bg->screenSizeText = 1;
    bg->colorMode      = 1;
    bg->screenBase     = 0;
    bg->charBase       = 1;
    bg->extPlttSlot    = 0;
    REG_BG1CNT_SUB     = (REG_BG1CNT_SUB & 0x43) | 0x4084;

    bg                 = Display_GetBGSettings(DISPLAY_SUB, DISPLAY_BG2);
    bg->bgMode         = 0;
    bg->screenSizeText = 1;
    bg->colorMode      = 1;
    bg->screenBase     = 2;
    bg->charBase       = 3;
    bg->extPlttSlot    = 1;
    REG_BG2CNT_SUB     = (REG_BG2CNT_SUB & 0x43) | 0x428C;

    bg                 = Display_GetBGSettings(DISPLAY_SUB, DISPLAY_BG3);
    bg->charBase       = 5;
    bg->bgMode         = 0;
    bg->screenSizeText = 0;
    bg->colorMode      = 1;
    bg->screenBase     = 4;
    bg->extPlttSlot    = 1;
    REG_BG3CNT_SUB     = (REG_BG3CNT_SUB & 0x43) | 0x494;

    Display_GetBGSettings(DISPLAY_SUB, DISPLAY_BG0)->priority = 1;
    Display_GetBGSettings(DISPLAY_SUB, DISPLAY_BG1)->priority = 2;
    Display_GetBGSettings(DISPLAY_SUB, DISPLAY_BG2)->priority = 2;
    Display_GetBGSettings(DISPLAY_SUB, DISPLAY_BG3)->priority = 3;
    Display_GetBGSettings(DISPLAY_SUB, DISPLAY_BG0)->mosaic   = 0;
    Display_GetBGSettings(DISPLAY_SUB, DISPLAY_BG1)->mosaic   = 0;
    Display_GetBGSettings(DISPLAY_SUB, DISPLAY_BG2)->mosaic   = 0;
    Display_GetBGSettings(DISPLAY_SUB, DISPLAY_BG3)->mosaic   = 0;
    sub->layers                                               = 0x1E;

    g_DisplaySettings.engineState[DISPLAY_MAIN].blendMode = 1;
    Display_SetBlendLayers(DISPLAY_MAIN, 1, 0x3E);
    Display_SetBlendCoeffs(DISPLAY_MAIN, 6, 0xA);
    g_DisplaySettings.engineState[DISPLAY_SUB].blendMode = 1;
    Display_SetBlendLayers(DISPLAY_SUB, 1, 0x3E);
    Display_SetBlendCoeffs(DISPLAY_SUB, 6, 0xA);

    main->windows |= 4;
    g_DisplaySettings.engineState[DISPLAY_MAIN].windowObj            = 8;
    g_DisplaySettings.engineState[DISPLAY_MAIN].windowObjEffects     = 0;
    g_DisplaySettings.engineState[DISPLAY_MAIN].windowOutside        = 0x17;
    g_DisplaySettings.engineState[DISPLAY_MAIN].windowOutsideEffects = 0;

    sub->windows |= 4;
    g_DisplaySettings.engineState[DISPLAY_SUB].windowObj            = 8;
    g_DisplaySettings.engineState[DISPLAY_SUB].windowObjEffects     = 0;
    g_DisplaySettings.engineState[DISPLAY_SUB].windowOutside        = 0x17;
    g_DisplaySettings.engineState[DISPLAY_SUB].windowOutsideEffects = 0;
}

/// VBlank callback: flushes the display lists and uploads OAM/palettes.
static void func_ov003_0208d49c(void) {
    if (SystemStatusFlags.vblank == 0) {
        return;
    }

    Display_Commit();
    DMA_Flush();

    OamMgr_Commit();

    DC_PurgeRange(g_PaletteBuffers[DISPLAY_MAIN].bg, 0x400);
    GX_LoadBgPltt(g_PaletteBuffers[DISPLAY_MAIN].bg, 0, 0x200);
    GX_LoadObjPltt(g_PaletteBuffers[DISPLAY_MAIN].obj, 0, 0x200);
    DC_PurgeRange(g_PaletteBuffers[DISPLAY_SUB].bg, 0x400);
    GXs_LoadBgPltt(g_PaletteBuffers[DISPLAY_SUB].bg, 0, 0x200);
    GXs_LoadObjPltt(g_PaletteBuffers[DISPLAY_SUB].obj, 0, 0x200);
}

static void func_ov003_0208d56c(void) {
    func_ov003_0208d010();
    func_ov003_0208d1d0();
    Interrupts_RegisterVBlankCallback(func_ov003_0208d49c, 1);
}

static void func_ov003_0208d58c(void) {
    Interrupts_ForceVBlank();
    Interrupts_RegisterVBlankCallback(NULL, 1);
}

/// Starts a battle (or a chain of battles) with the given encounter id.
static void func_ov003_0208d5a4(u16 encounterId, s32 duration) {
    gSaveData.battleCount         = 1;
    gSaveData.battleEncounters[0] = encounterId;
    if (duration != 0) {
        gSaveData.unk_3138 = duration;
    }
}

/// Number of chapter characters that may still join the current battle.
static s32 func_ov003_0208d5cc(u32 mask) {
    s32 i;
    s32 count     = 0;
    u32 available = gSaveData.unk_3234;

    for (i = 0; i < 18; i++) {
        if ((mask & (1 << i)) && (available & (1 << i)) == 0) {
            count++;
        }
    }
    return count;
}

/// Picks the character that joins the battle for this chapter and tracks that it was used.
static u16 func_ov003_0208d608(void) {
    u32 mask  = data_ov003_020d7898[gSaveData.chapter];
    s32 count = func_ov003_0208d5cc(mask);
    s32 rng;
    s32 i;

    if (count <= 0) {
        gSaveData.unk_3234 = 0;
        count              = func_ov003_0208d5cc(mask);
    }

    rng = RNG_Next(count);

    for (i = 0; i < 18; i++) {
        if ((mask & (1 << i)) == 0) {
            continue;
        }
        if (gSaveData.unk_3234 & (1 << i)) {
            continue;
        }
        if (rng == 0) {
            return (u16)i;
        }
        rng--;
    }
    return 0;
}

/// Configures the encounter chain and background track for a battle selector.
static void func_ov003_0208d698(u32 selector) {
    switch (selector) {
        case 0:
        case 1:
            return;

        case 2:
            func_ov003_0208d5a4(0x32, 1);
            return;
        case 3:
            func_ov003_0208d5a4(0x33, 4);
            return;
        case 4:
            func_ov003_0208d5a4(0x35, 2);
            return;
        case 5:
            func_ov003_0208d5a4(0x36, 2);
            return;
        case 6:
            func_ov003_0208d5a4(0x37, 2);
            return;
        case 7:
            func_ov003_0208d5a4(0x7C, 4);
            return;
        case 8:
            func_ov003_0208d5a4(0x7D, 4);
            return;
        case 9:
            func_ov003_0208d5a4(0x3B, 0x16);
            return;
        case 10:
            func_ov003_0208d5a4(7, 0x2A);
            return;
        case 11:
            func_ov003_0208d5a4(0x81, 0xE);
            return;
        case 12:
            func_ov003_0208d5a4(0x82, 0xE);
            return;
        case 13:
            func_ov003_0208d5a4(0x84, 7);
            return;
        case 14:
            func_ov003_0208d5a4(0x13, 0x2A);
            return;
        case 15:
            func_ov003_0208d5a4(9, 0x1A);
            return;
        case 16:
            func_ov003_0208d5a4(0x88, 4);
            return;
        case 17:
            func_ov003_0208d5a4(0x89, 2);
            return;
        case 18:
            func_ov003_0208d5a4(0x3C, 1);
            return;
        case 19:
            func_ov003_0208d5a4(0x8A, 0xD);
            return;
        case 20:
            func_ov003_0208d5a4(0x17, 0xE);
            return;
        case 21:
            func_ov003_0208d5a4(0x18, 8);
            return;
        case 22:
            func_ov003_0208d5a4(0x19, 0x10);
            return;
        case 23:
            func_ov003_0208d5a4(0x90, 0x13);
            return;
        case 24:
            func_ov003_0208d5a4(0x96, 0);
            return;
        case 25:
            func_ov003_0208d5a4(0x91, 1);
            return;
        case 26:
            func_ov003_0208d5a4(0x92, 6);
            return;
        case 27:
            func_ov003_0208d5a4(0x93, 10);
            return;
        case 28:
            func_ov003_0208d5a4(0x94, 5);
            return;
        case 29:
            func_ov003_0208d5a4(0x95, 8);
            return;
        case 30:
            func_ov003_0208d5a4(0xF, 0x28);
            return;
        case 31:
            func_ov003_0208d5a4(0x97, 4);
            return;
        case 32:
            func_ov003_0208d5a4(0x98, 4);
            return;
        case 33:
            func_ov003_0208d5a4(0x3D, 1);
            return;
        case 34:
            func_ov003_0208d5a4(0x99, 0x14);
            return;
        case 35:
            func_ov003_0208d5a4(0x9A, 0x16);
            return;
        case 36:
            func_ov003_0208d5a4(8, 0x2A);
            return;
        case 37:
            func_ov003_0208d5a4(0x1D, 6);
            return;
        case 38:
            func_ov003_0208d5a4(0x1E, 0xE);
            return;
        case 39:
            func_ov003_0208d5a4(0x1C, 0xE);
            return;
        case 40:
            func_ov003_0208d5a4(0x1F, 10);
            return;
        case 41:
            func_ov003_0208d5a4(0x9B, 6);
            return;
        case 42:
            func_ov003_0208d5a4(0x9C, 8);
            return;
        case 43:
            func_ov003_0208d5a4(0x21, 0xD);
            return;
        case 44:
            func_ov003_0208d5a4(10, 0x2A);
            return;
        case 45:
            func_ov003_0208d5a4(0x1B, 0x2A);
            return;
        case 46:
            func_ov003_0208d5a4(0x14, 0x2A);
            return;
        case 47:
            func_ov003_0208d5a4(0x16, 0x2A);
            return;
        case 48:
            func_ov003_0208d5a4(0x20, 0x28);
            return;
        case 49:
            gSaveData.battleCount          = 11;
            gSaveData.battleEncounters[0]  = 9;
            gSaveData.battleEncounters[1]  = 0x13;
            gSaveData.battleEncounters[2]  = 0x1A;
            gSaveData.battleEncounters[3]  = 0xF;
            gSaveData.battleEncounters[4]  = 8;
            gSaveData.battleEncounters[5]  = 0x1D;
            gSaveData.battleEncounters[6]  = 0x1C;
            gSaveData.battleEncounters[7]  = 0x1F;
            gSaveData.battleEncounters[8]  = 10;
            gSaveData.battleEncounters[9]  = 0x1B;
            gSaveData.battleEncounters[10] = 0x14;
            gSaveData.unk_3138             = 0x2A;
            return;
        case 50:
            func_ov003_0208d5a4(0x38, 0x13);
            return;
        case 51:
            func_ov003_0208d5a4(0x39, 0x13);
            return;
        case 52:
            func_ov003_0208d5a4(0x3A, 0x13);
            return;
    }
}

/// Chooses the encounter that the battle actually fights and how long it lasts.
static void func_ov003_0208db18(BtlMgrWork* work) {
    u16 encounter = gSaveData.battleEncounters[gSaveData.unk_3142 - 1];

    switch (encounter) {
        case 10:
            work->encounterId    = data_ov003_020d7838[gSaveData.battleTimer];
            work->battleDuration = 4;
            break;

        case 0x14:
            work->encounterId    = data_ov003_020d7834[gSaveData.battleTimer];
            work->battleDuration = 1;
            break;

        case 0x33:
            work->encounterId    = data_ov003_020d7830[gSaveData.battleTimer];
            work->battleDuration = 1;
            break;

        default:
            work->encounterId    = encounter;
            work->battleDuration = 0;
            break;
    }
}

/// Sets the battle track and remembers that it was played.
static void func_ov003_0208dbf4(BtlMgrWork* work, u32 track) {
    gSaveData.unk_313C = data_ov003_020d7850[track];

    if (gSaveData.battleTimer == 0 || gSaveData.unk_3130 == 1) {
        CriSndMgr_PlayFile(gSaveData.unk_313C);
    }

    gSaveData.unk_3234 |= 1 << track;
    work->musicId = track;
}

/// Selects the battle music. Note that the field at 0x11E14 is reused as the music index once the
/// encounter id has been consumed.
static void func_ov003_0208dc64(BtlMgrWork* work, s32 isContinuation) {
    u16 music;

    if (isContinuation != 0) {
        SndMgr_DeepResetSequenceHeap();
    } else {
        SndMgr_ResetSequenceHeap();
    }

    if (gSaveData.unk_3134 == 0x31) {
        func_ov003_0208dbf4(work, 9);
        return;
    }

    if (work->encounterId >= 0x9D && work->encounterId <= 0xCE) {
        func_ov003_0208dbf4(work, 0xE);
        return;
    }

    if (gSaveData.unk_3130 == 2 && work->encounterId != 0x16 && work->encounterId != 0x20) {
        func_ov003_0208dbf4(work, 0xF);
        return;
    }

    switch (work->encounterId) {
        case 7:
        case 8:
            music = 9;
            break;
        case 9:
            music = 0xB;
            break;
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
            music = 0xC;
            break;
        case 15:
        case 16:
        case 17:
        case 18:
            music = 0xB;
            break;
        case 19:
            music = 0xA;
            break;
        case 20:
        case 21:
            music = 0xA;
            break;
        case 22:
            music = 0x11;
            break;
        case 23:
            music = 5;
            break;
        case 24:
            music = 0xA;
            break;
        case 25:
        case 26:
            music = 7;
            break;
        case 27:
            music = 7;
            break;
        case 28:
            music = 0xA;
            break;
        case 29:
            music = 7;
            break;
        case 30:
        case 31:
            music = 8;
            break;
        case 32:
            music = 9;
            break;
        case 33:
            music = 0xB;
            break;
        case 55:
            music = 1;
            break;
        case 125:
            music = 0xC;
            break;
        case 129:
            music = 2;
            break;
        case 132:
            music = 7;
            break;
        case 137:
            music = 0xC;
            break;
        case 138:
            music = 0x10;
            break;
        case 144:
            music = 8;
            break;
        case 145:
            music = 0x10;
            break;
        case 147:
            music = 8;
            break;

        default:
            music = func_ov003_0208d608();
            if (gSaveData.unk_313C == data_ov003_020d7850[music] &&
                func_ov003_0208d5cc(data_ov003_020d7898[gSaveData.chapter] > 1) != 0)
            {
                gSaveData.unk_3234 |= 1 << music;
                music = func_ov003_0208d608();
                gSaveData.unk_3234 &= ~(1 << music);
            }
            break;
    }

    func_ov003_0208dbf4(work, music);
}

/// Advances the chained-battle timer. Returns TRUE once all battles of the chain are done.
static BOOL func_ov003_0208e008(BtlMgrWork* work) {
    gSaveData.battleTimer++;
    if (gSaveData.battleTimer > work->battleDuration) {
        gSaveData.unk_3142++;
        gSaveData.battleTimer = 0;
    }
    return gSaveData.unk_3142 > gSaveData.battleCount;
}

/// Copies the battle level/experience back into the save data.
static void func_ov003_0208e064(void) {
    gSaveData.playerStats.currentLevel   = gSaveData.battleLevel;
    gSaveData.playerStats.maxLevel       = gSaveData.battleMaxLevel;
    gSaveData.playerStats.expToNextLevel = gSaveData.battleExpToNextLevel;
}

/// Battle result/continue state machine.
static void func_ov003_0208e098(BtlMgrWork* work) {
    BOOL isTrueEnd = gSaveData.unk_3130 == 3;

    switch (work->step) {
        case 3: {
            OverlayTag tag;
            OverlayTag tag2;

            func_ov003_0208e064();
            CriSndMgr_Stop(gSaveData.unk_313C);

            if (isTrueEnd) {
                MainOvlDisp_ReplaceTop(&tag, &OVERLAY_27_ID, func_ov027_020e860c, NULL, 0);
                return;
            }

            if (gSaveData.unk_3140 & 2) {
                gSaveData.unk_24B4 = 5;
            } else if (gSaveData.playerStats.activePartner != 0xFF) {
                gSaveData.unk_24B4 = 3;
            } else {
                gSaveData.unk_24B4 = 4;
            }
            MainOvlDisp_ReplaceTop(&tag2, &OVERLAY_30_ID, func_ov030_020ae92c, NULL, 0);
            return;
        }

        case 1: {
            OverlayTag tag;
            OverlayTag tag2;

            CriSndMgr_Stop(gSaveData.unk_313C);
            if (Inventory_GetOwnedCount(0x2C9) != 0) {
                MainOvlDisp_ReplaceTop(&tag, &OVERLAY_25_ID, ProcessOverlay_Continue, NULL, 0);
            } else {
                MainOvlDisp_ReplaceTop(&tag2, &OVERLAY_37_ID, (void*)0x0208370C, NULL, 0);
            }
            return;
        }

        case 4: {
            OverlayTag tag;
            OverlayTag tag2;
            s32        i;

            func_ov003_0208e064();
            func_ov003_0208cc14();
            func_ov003_0208cf94(work->encounterId);
            for (i = 0; i < 96; i++) {
                gSaveData.unk_1FCC[i] = gSaveData.unk_326A[i];
            }
            CriSndMgr_Stop(gSaveData.unk_313C);

            if (work->encounterId == 0x16) {
                gSaveData.unk_3134 = 0x31;
                MainOvlDisp_ReplaceTop(&tag, &OVERLAY_30_ID, func_ov030_020ae92c, (void*)&data_ov003_020d4f74, 0);
            } else {
                gSaveData.unk_24B4 = 5;
                MainOvlDisp_ReplaceTop(&tag2, &OVERLAY_30_ID, func_ov030_020ae92c, NULL, 0);
            }
            return;
        }

        case 0: {
            OverlayTag tag;
            OverlayTag tag2;
            s32        i;

            func_ov003_0208e064();
            func_ov003_0208cc14();
            func_ov003_0208cf94(work->encounterId);
            for (i = 0; i < 96; i++) {
                gSaveData.unk_1FCC[i] = gSaveData.unk_326A[i];
            }

            if (isTrueEnd) {
                CriSndMgr_Stop(gSaveData.unk_313C);
                MainOvlDisp_ReplaceTop(&tag, &OVERLAY_27_ID, func_ov027_020e860c, NULL, 0);
            } else {
                MainOvlDisp_ReplaceTop(&tag2, &OVERLAY_44_ID, (void*)0x02084A88, NULL, 0);
            }
            return;
        }

        case 2: {
            OverlayTag tag;

            if (gSaveData.battleTimer == 0) {
                CriSndMgr_Stop(gSaveData.unk_313C);
            }
            work->keepBattleState = 1;
            MainOvlDisp_ReplaceTop(&tag, &OVERLAY_3_ID, ProcessOverlay_Battle, NULL, 0);
            return;
        }

        case 5: {
            OverlayTag tag;

            CriSndMgr_Stop(gSaveData.unk_313C);
            MainOvlDisp_ReplaceTop(&tag, &OVERLAY_27_ID, func_ov027_020e860c, NULL, 0);
            return;
        }

        default:
            return;
    }
}

/// State machine used by the "continue" battle overlay.
static void func_ov003_0208e3f0(BtlMgrWork* work) {
    switch (work->step) {
        case 0: {
            OverlayTag tag;

            gSaveData.unk_24B4 = 2;
            CriSndMgr_Stop(gSaveData.unk_313C);
            MainOvlDisp_ReplaceTop(&tag, &OVERLAY_30_ID, func_ov030_020ae92c, NULL, 0);
            return;
        }

        case 3: {
            OverlayTag tag;

            gSaveData.unk_24B4 = 4;
            CriSndMgr_Stop(gSaveData.unk_313C);
            MainOvlDisp_ReplaceTop(&tag, &OVERLAY_30_ID, func_ov030_020ae92c, NULL, 0);
            return;
        }

        case 2: {
            OverlayTag tag;

            CriSndMgr_Stop(gSaveData.unk_313C);
            work->keepBattleState = 1;
            MainOvlDisp_ReplaceTop(&tag, &OVERLAY_3_ID, ProcessOverlay_Battle, NULL, 0);
            return;
        }

        case 1:
        case 5: {
            OverlayTag tag;

            CriSndMgr_Stop(gSaveData.unk_313C);
            MainOvlDisp_ReplaceTop(&tag, &OVERLAY_27_ID, func_ov027_020e860c, NULL, 0);
            return;
        }

        default:
            return;
    }
}

/// Fades the battle in, then switches to the battle update callback.
static void func_ov003_0208e520(BtlMgrWork* work) {
    if (work->fadeCounter == 0) {
        EasyFade_FadeBothDisplays(0, 0, 0x1000);
        work->active = 1;
        work->fadeCounter++;
    }
    if (EasyFade_IsFading() != 0) {
        return;
    }
    work->fadeCounter = 0;
    work->unk_11E1A   = 0;
    DebugOvlDisp_ReplaceTop((OverlayCB)func_ov003_0208e5f8, work, 0);
}

/// Fades the battle out, then pops the battle callback.
static void func_ov003_0208e59c(BtlMgrWork* work) {
    if (work->fadeCounter == 0) {
        EasyFade_FadeBothDisplays(0, 0x10, 0x1000);
        work->active = 1;
        work->fadeCounter++;
    }
    if (EasyFade_IsFading() != 0) {
        return;
    }
    DebugOvlDisp_Pop();
}

/// Battle update callback: switches to the fade-out callback once the battle is over.
static void func_ov003_0208e5f8(BtlMgrWork* work) {
    work->active = g_CombatGlobal->unk3D87C == 0;

    if (g_CombatGlobal->unk3D874 == 1) {
        return;
    }
    work->fadeCounter = 0;
    work->unk_11E1A   = 0;
    DebugOvlDisp_ReplaceTop((OverlayCB)func_ov003_0208e668, work, 0);
}

/// Battle update callback for the "battle is over" path.
static void func_ov003_0208e668(BtlMgrWork* work) {
    work->active = g_CombatGlobal->unk3D87C == 0;
    func_ov003_0208b71c();

    switch (g_CombatGlobal->unk3D874) {
        case 5:
            work->step        = 3;
            work->fadeCounter = 0;
            work->unk_11E1A   = 0;
            DebugOvlDisp_ReplaceTop((OverlayCB)func_ov003_0208e59c, work, 0);
            return;

        case 3:
            work->step        = func_ov003_0208e008(work) ? 0 : 2;
            work->fadeCounter = 0;
            work->unk_11E1A   = 0;
            DebugOvlDisp_ReplaceTop((OverlayCB)func_ov003_0208e7c0, work, 0);
            return;

        case 4:
            work->step        = 1;
            work->fadeCounter = 0;
            work->unk_11E1A   = 0;
            DebugOvlDisp_ReplaceTop((OverlayCB)func_ov003_0208e7c0, work, 0);
            return;

        case 6:
            work->step        = func_ov003_0208e008(work) ? 4 : 2;
            work->fadeCounter = 0;
            work->unk_11E1A   = 0;
            DebugOvlDisp_ReplaceTop((OverlayCB)func_ov003_0208e59c, work, 0);
            return;

        default:
            return;
    }
}

static void func_ov003_0208e7c0(BtlMgrWork* work) {
    work->active = g_CombatGlobal->unk3D87C == 0;

    if (g_CombatGlobal->unk3D874 != 7) {
        return;
    }
    work->fadeCounter = 0;
    work->unk_11E1A   = 0;
    DebugOvlDisp_ReplaceTop((OverlayCB)func_ov003_0208e59c, work, 0);
}

/// Overlay init stage: allocate the work area and bring up the battle scene.
static void BtlMgr_Init(BtlMgrWork* work) {
    if (work == NULL) {
        char* sequence = data_ov003_020e3ac0;

        work = Mem_AllocHeapTail(&gDebugHeap, 0x11E20);
        Mem_SetSequence(&gDebugHeap, work, sequence);
        MainOvlDisp_SetCbArg(work);
    }

    MI_CpuFill(0, work, Mem_GetBlockSize(&gDebugHeap, work));

    Mem_InitializeHeap(&work->heap, work->heapData, 0x800);
    EasyTask_InitializePool(&work->taskPool, &work->heap, 8, NULL, NULL);

    if (gSaveData.unk_3130 == 0 || gSaveData.unk_3130 == 1 || gSaveData.unk_3130 == 2 || gSaveData.unk_3130 == 4) {
        BtlDebug_InitSettings();
    }

    func_ov003_0208d56c();
    ResourceMgr_ReinitManagers(&work->resources);

    g_PaletteBuffers[DISPLAY_MAIN].bg[0] = 0;
    g_PaletteBuffers[DISPLAY_SUB].bg[0]  = 0;
    Input_Init(&InputStatus, 0x14, 0x14, 0);
    TouchInput_Init();
    func_0200d8f0();

    EasyTask_CreateTask(&work->taskPool, &Task_EasyFade, NULL, 0, NULL, NULL);

    if (gSaveData.unk_3142 == 0) {
        s32 i;

        for (i = 0; i < 96; i++) {
            gSaveData.unk_326A[i] = gSaveData.unk_1FCC[i];
        }

        gSaveData.battleLevel          = gSaveData.playerStats.currentLevel;
        gSaveData.battleMaxLevel       = gSaveData.playerStats.maxLevel;
        gSaveData.battleExpToNextLevel = gSaveData.playerStats.expToNextLevel;
        gSaveData.battleChargeProgress = 0;
        gSaveData.battleChargeStage    = 0;
        gSaveData.unk_317C             = 0;
        gSaveData.unk_317D             = 0;
        gSaveData.unk_317E             = 0;
        gSaveData.battleBasePP         = 0;
        gSaveData.battleParTime        = 0;
        gSaveData.battleClearTime      = 0;
        gSaveData.battleMaxHits        = 0;
        gSaveData.unk_318E             = 0;
        gSaveData.unk_318F             = 0;
        gSaveData.battleBonusFlags     = 0;
        gSaveData.battlePartner        = 0xFF;

        for (i = 0; i < 32; i++) {
            gSaveData.battleDrops[i].itemId = 0xFFFF;
            gSaveData.battleDrops[i].count  = 0;
        }

        gSaveData.unk_3142++;
        gSaveData.battleTimer = 0;
        gSaveData.unk_3232    = 0;

        func_ov003_0208d698(gSaveData.unk_3134);
        func_ov003_0208db18(work);
        func_ov003_0208dc64(work, 0);
        func_ov003_0208b944(work->encounterId, 0);
    } else {
        func_ov003_0208db18(work);
        func_ov003_0208dc64(work, 1);
        func_ov003_0208b944(work->encounterId, 1);
    }

    work->fadeCounter     = 0;
    work->unk_11E1A       = 0;
    work->active          = 1;
    work->step            = 5;
    work->keepBattleState = 0;

    func_ov003_0208b6d4();
    DebugOvlDisp_Init();
    DebugOvlDisp_Push((OverlayCB)func_ov003_0208e520, work, 0);
    MainOvlDisp_NextProcessStage();
    MainOvlDisp_Run();
}

/// Overlay main stage.
static void BtlMgr_Main(BtlMgrWork* work) {
    TouchInput_Update();
    OamMgr_Reset3DState();
    OamMgr_Reset(&g_OamMgr[DISPLAY_MAIN], 0, 0);
    OamMgr_Reset(&g_OamMgr[DISPLAY_SUB], 0, 0);
    OamMgr_SetAffineCount(&g_OamMgr[DISPLAY_EXTENDED], 0);
    OamMgr_ResetCommandQueues(&g_OamMgr[DISPLAY_MAIN]);
    OamMgr_ResetCommandQueues(&g_OamMgr[DISPLAY_SUB]);

    func_ov003_0208c2a8();
    func_ov003_0208c4bc();
    DebugOvlDisp_Run();

    if (DebugOvlDisp_IsStackAtBase() != 0) {
        if (gSaveData.unk_3130 == 1) {
            func_ov003_0208e3f0(work);
        } else {
            func_ov003_0208e098(work);
        }
        return;
    }

    if (work->active != 0) {
        EasyTask_UpdatePool(&work->taskPool);
    }

    OamMgr_Swap3DBuffers();
    OamMgr_FlushCommands(&g_OamMgr[DISPLAY_MAIN]);
    OamMgr_FlushCommands(&g_OamMgr[DISPLAY_SUB]);
    PaletteMgr_Flush(g_PaletteManagers[2], 0);
    func_02001b44(2, 0, &g_PaletteBuffers[DISPLAY_EXTENDED], 0x400);
    PaletteMgr_Flush(g_PaletteManagers[0], 0);
    PaletteMgr_Flush(g_PaletteManagers[1], 0);
    func_0200d90c();
}

/// Overlay exit stage.
static void BtlMgr_Exit(BtlMgrWork* work) {
    if (work == NULL) {
        return;
    }

    func_ov003_0208c54c(work->keepBattleState);
    Input_Init(&InputStatus, 8, 1, 2);
    func_ov003_0208d58c();
    EasyTask_DestroyPool(&work->taskPool);
    Mem_Free(&gDebugHeap, work);
    MainOvlDisp_SetCbArg(NULL);
}

/// Overlay callback dispatcher.
void ProcessOverlay_Battle(BtlMgrWork* work) {
    s32 stage = MainOvlDisp_GetProcessStage();

    if (stage == PROCESS_STAGE_EXIT) {
        BtlMgr_Exit(work);
        return;
    }
    BtlMgr_ProcessStages[stage](work);
}
