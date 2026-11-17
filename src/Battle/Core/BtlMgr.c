#include "Battle/Core/BtlMgr.h"
#include "Battle/Core/BtlActor.h"
#include "Battle/Core/BtlEnmOverlay.h"
#include "Battle/Core/BtlObsMgr.h"
#include "Battle/Core/BtlSprite.h"
#include "Battle/Core/BtlTouch.h"
#include "Combat/Core/BtlAura.h"
#include "Combat/Core/BtlBar.h"
#include "Combat/Core/BtlCmd.h"
#include "Combat/Core/BtlDbg.h"
#include "Combat/Core/BtlEff.h"
#include "Combat/Core/BtlPhase.h"
#include "Combat/Core/BtlTch.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatAttack.h"
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

static void BtlMgr_Init(BattleMgrWork* work);
static void BtlMgr_Main(BattleMgrWork* work);
static void BtlMgr_Exit(BattleMgrWork* work);
static void BtlMgr_WaitReady(BattleMgrWork* work);
static void BtlMgr_RunBattle(BattleMgrWork* work);
static void BtlMgr_WaitResults(BattleMgrWork* work);

static char Path_BtlCmd[0x18]     = "Apl_Fur/Grp_BtlCmd.bin";
static char Path_BtlObs[0x18]     = "Apl_Fur/Grp_BtlObs.bin";
static char Path_BtlMenu[0x18]    = "Apl_Fur/Bin_BtlMenu.bin";
static char Path_BtlBadge[0x1C]   = "Apl_Fur/Grp_BtlBadge.bin";
static char Path_BtlPly02[0x1C]   = "Apl_Fur/Grp_BtlPly02.bin";
static char Path_BtlPly00[0x1C]   = "Apl_Fur/Grp_BtlPly00.bin";
static char Path_BtlPly01[0x1C]   = "Apl_Fur/Grp_BtlPly01.bin";
static char Path_BtlPly03[0x1C]   = "Apl_Fur/Grp_BtlPly03.bin";
static char Path_BtlJingle[0x1C]  = "Apl_Fur/Grp_BtlJingle.bin";
static char Path_BtlEffect[0x1C]  = "Apl_Fur/Grp_BtlEffect.bin";
static char Path_BtlSpecial[0x1C] = "Apl_Fuk/Grp_BtlSpecial.bin";
static char BtlMgr_HeapTag[0x1C]  = "BtlMgr.c : BtlMgr_Init()";

/// The heap sequence name the battle work area is tagged with.
static char BtlMgr_SequenceNameBuf[16];
char*       BtlMgr_SequenceName        = BtlMgr_SequenceNameBuf;
static char BtlMgr_SequenceNameBuf[16] = "Seq_Battle()";

/// Experience percentage per difficulty.
static const s16 BtlMgr_ExpPercents[4] = {50, 100, 150, 200};

const BinIdentifier Iden_BtlPly00   = {3, Path_BtlPly00};
const BinIdentifier Iden_BtlPly01   = {4, Path_BtlPly01};
const BinIdentifier Iden_BtlPly02   = {5, Path_BtlPly02};
const BinIdentifier Iden_BtlPly03   = {6, Path_BtlPly03};
const BinIdentifier Iden_BtlEffect  = {3, Path_BtlEffect};
const BinIdentifier Iden_BtlMenu    = {3, Path_BtlMenu};
const BinIdentifier Iden_BtlCmd     = {3, Path_BtlCmd};
const BinIdentifier Iden_BtlBadge   = {3, Path_BtlBadge};
const BinIdentifier Iden_BtlJingle  = {3, Path_BtlJingle};
const BinIdentifier Iden_BtlObs     = {3, Path_BtlObs};
const BinIdentifier Iden_BtlSpecial = {3, Path_BtlSpecial};

/// The encounters played in turn by the multi-phase encounters 51, 20 and 10.
static const u16 BtlMgr_Encounter51Phases[2] = {0x33, 0x34};
static const u16 BtlMgr_Encounter20Phases[2] = {0x14, 0x15};
static const u16 BtlMgr_Encounter10Phases[6] = {0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x00};

/// Process stages of the battle overlay: init, main, exit.
static void (*const BtlMgr_ProcessStages[3])(BattleMgrWork*) = {
    BtlMgr_Init,
    BtlMgr_Main,
    BtlMgr_Exit,
};

/// The sound file of each battle track.
static const u32 BtlMgr_TrackFiles[18] = {8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 26, 22, 23, 26, 21};

/// The battle tracks each chapter picks from at random, as a mask of track indices.
static const u32 BtlMgr_ChapterTracks[22] = {1,      1,      0x1003, 0x1003, 0x1007, 0x1087, 0x287, 0xe88,
                                             0xa88,  0x1298, 0x1218, 0x438,  0x438,  0x538,  0xd40, 0xd44,
                                             0x1946, 0x18e6, 0xcf6,  0x77f,  0x37f,  0x1fff};

/// The team's maximum HP at the battle level, plus ability 3's bonus.
static s32 BtlMgr_CalcMaxHp(void) {
    Battle* g = gBattle;
    s32     partner;

    switch (g->partnerId) {
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
            partner = 0xff;
            break;
    }

    s32 maxHealth = Stats_GetMaxHealth(partner, gSaveData.battleLevel);
    if (CombatAbility_IsActive(3) != 0) {
        maxHealth = (s16)(maxHealth + (s16)CombatAbility_GetValue(3));
    }
    return maxHealth;
}

/// Loads the partner's overlay and the encounter's noise overlays.
static void BtlMgr_LoadOverlays(u32 partner, u16 encounterId) {
    s32 overlay4 = BtlEnmOverlay_Table[encounterId];

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

/// The partner that fights in an encounter: the encounter's fixed partner, else the one picked in the menu.
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

/// Sets up the team for the battle: sync, movement speed, drop rate and HP. A chained battle restores the HP carried
/// over from the previous one (healing what the maximum grew by); a fresh battle starts at full HP, less the halves
/// ability 9 costs.
static void BtlMgr_InitTeam(s32 isContinuation) {
    s32 partner;

    switch (gBattle->partnerId) {
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
            partner = 0xff;
            break;
    }

    gBattle->partnerSync = (partner == 0xff) ? 0 : gSaveData.friendStats[partner].sync;
    gBattle->moveSpeed   = (CombatAbility_IsActive(0x12) != 0) ? 30 : 100;

    if (CombatAbility_IsActive(0x40) != 0) {
        gSaveData.unk_3140 |= 2;
    }

    gBattle->dropRate = gSaveData.battleDropRate;
    if (CombatAbility_IsActive(0) != 0) {
        gBattle->dropRate += (s16)CombatAbility_GetValue(0);
    }

    gBattle->teamMaxHp = BtlMgr_CalcMaxHp();

    if (isContinuation != 0) {
        s32 merged;

        gBattle->teamHp        = gSaveData.battleHpGauge;
        gBattle->playerDamage  = gSaveData.battleHpGaugeSub0;
        gBattle->partnerDamage = gSaveData.battleHpGaugeSub1;

        merged = gBattle->teamMaxHp - (gBattle->teamHp + gBattle->playerDamage + gBattle->partnerDamage);
        if (merged > 0) {
            gBattle->teamHp += (s16)(merged * 0x10000 >> 16);
        }
        if (merged >= 0) {
            return;
        }

        gBattle->partnerDamage += (s16)(merged * 0x10000 >> 16);
        if (gBattle->partnerDamage < 0) {
            gBattle->playerDamage += gBattle->partnerDamage;
            gBattle->partnerDamage = 0;
        }
        if (gBattle->playerDamage < 0) {
            gBattle->teamHp += gBattle->playerDamage;
            gBattle->playerDamage = 0;
        }
        return;
    }

    gBattle->teamHp        = gBattle->teamMaxHp;
    gBattle->playerDamage  = 0;
    gBattle->partnerDamage = 0;

    if (CombatAbility_IsActiveForPlayer(9) != 0) {
        s32 half = (gBattle->teamMaxHp + (gBattle->teamMaxHp >> 31)) * 0x8000 >> 16;

        gBattle->teamHp -= (s16)half;
        gBattle->playerDamage = (s16)half;
    }
    if (CombatAbility_IsActiveForPartner(9) != 0) {
        s32 half = (gBattle->teamMaxHp + (gBattle->teamMaxHp >> 31)) * 0x8000 >> 16;

        gBattle->teamHp -= (s16)half;
        gBattle->partnerDamage = (s16)half;
    }

    if (gBattle->teamHp < 1) {
        gBattle->teamHp = 1;
        gBattle->partnerDamage--;
    }
}
/// Picks the encounter's badge set and spawns its badges.
static void BtlMgr_InitBadges(u16 encounterId) {
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

    gBattle->musicId = music;
    BtlBadgeMgr_QueueAbilities(music);
    BtlBadgeMgr_SpawnBadges(music);
}

/// Applies the player's and the partner's equipped threads.
static void BtlMgr_InitThreads(s32 partner) {
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

    if (slot != 0xFF) {
        func_ov003_020a303c(&gSaveData.friendStats[slot].equippedThreads[0], slot);
    }
}

/// Stops a player character and starts its level-up animation.
static void BtlMgr_StartLevelUp(BattleActor* actor) {
    actor->flags &= ~(ACTOR_FLAG_STAGGERED | ACTOR_FLAG_LAUNCHED);
    actor->flags &= ~ACTOR_FLAG_GUARDING;
    actor->xVelocity = 0;
    actor->yVelocity = 0;
    actor->flags |= ACTOR_FLAG_INTANGIBLE;
    BtlActor_SetPendingCommand(actor, ACTOR_CMD_LEVEL_UP);
}

/// Whether every enemy on both screens is defeated.
static BOOL BtlMgr_AreEnemiesDefeated(void) {
    Ov003EnemyActor* unit;
    BOOL             result = TRUE;

    for (unit = BtlList_GetHead(&gBattle->enemiesMain); unit != NULL; unit = BtlList_GetNext(&unit->node)) {
        if (unit->flags196.bits.b8 == 1 || (unit->actor.flags & ACTOR_FLAG_DEFEATED) == 0) {
            result = FALSE;
            break;
        }
    }

    for (unit = BtlList_GetHead(&gBattle->enemiesSub); unit != NULL; unit = BtlList_GetNext(&unit->node)) {
        if (unit->flags196.bits.b8 == 1 || (unit->actor.flags & ACTOR_FLAG_DEFEATED) == 0) {
            result = FALSE;
            break;
        }
    }
    return result;
}

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
    if (gBattle->flags & BATTLE_FLAG_NO_LEVEL_UP || remaining > 0) {
        return;
    }

    gSaveData.battleLevel++;
    gSaveData.battleMaxLevel++;
    gSaveData.battleExpToNextLevel = gSaveData.battleMaxLevel * 100;

    gBattle->teamMaxHp     = BtlMgr_CalcMaxHp();
    gBattle->teamHp        = gBattle->teamMaxHp;
    gBattle->partnerDamage = 0;
    gBattle->playerDamage  = 0;

    gBattle->player->maxHp      = gBattle->teamMaxHp;
    gBattle->player->currentHp  = gBattle->player->maxHp;
    gBattle->partner->maxHp     = gBattle->teamMaxHp;
    gBattle->partner->currentHp = gBattle->partner->maxHp;

    gBattle->hitStopMain     = 0;
    gBattle->hitStopMainNext = 0;
    BtlMgr_StartLevelUp(gBattle->player);

    gBattle->hitStopSub     = 0;
    gBattle->hitStopSubNext = 0;
    BtlMgr_StartLevelUp(gBattle->partner);

    EasyTask_CreateTask(&gBattle->commonPool, &Tsk_BtlEff_LvUp, NULL, 0, NULL, NULL);
}

/// Awards experience, scaled by the difficulty. Nothing is awarded in retried battles.
static void BtlMgr_AwardExp(s16 exp) {
    if (gSaveData.unk_3130 != 1) {
        BtlMgr_AddExp((s16)((exp * BtlMgr_ExpPercents[gSaveData.battleDifficulty]) / 100));
    }
}

/// Adds to the PP earned this battle.
static void BtlMgr_AddPP(s16 amount) {
    gSaveData.battleBasePP += amount;
}

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

void BtlMgr_SetState(u32 state) {
    switch (state) {
        case BATTLE_STATE_WIN:
        case BATTLE_STATE_LOSE:
        case BATTLE_STATE_QUIT:
        case BATTLE_STATE_SCRIPTED_END:
            if (gBattle->state == BATTLE_STATE_FIGHT) {
                gBattle->state = state;
            }
            break;

        case BATTLE_STATE_RESULTS_DONE:
            if (gBattle->state == BATTLE_STATE_WIN || gBattle->state == BATTLE_STATE_LOSE) {
                gBattle->state = state;
            }
            break;

        default:
            gBattle->state = state;
            break;
    }
}

void BtlMgr_MoveByAngle(s32* x, s32* y, s32 angle, s32 scale) {
    s32 index = (angle & 0xFFFF) >> 4;
    s32 sin   = FX_SinCosTable_[index * 2 + 1];
    s32 cos   = FX_SinCosTable_[index * 2];

    *x += FX_Mul(sin, scale);
    *y += FX_Mul(cos, scale);
}

void BtlMgr_WorldToScreen(s16* screenX, s16* screenY, s32 x, s32 y, s32 z) {
    BattleStage* camera = &gBattle->stages[0];

    *screenX = (s16)((camera->left + (x - camera->viewX) + 0x80000) >> 12);
    *screenY = (s16)((camera->top + ((y + z) - camera->viewY) + 0x60000) >> 12);
}

void BtlMgr_AddTeamHp(s32 side, s16 delta) {
    if (gSaveData.unk_3130 == 1) {
        return;
    }
    if (delta > 0) {
        s32 room = gBattle->teamMaxHp - gBattle->teamHp;

        if (room < delta) {
            delta = (s16)room;
        }
    } else {
        if (gBattle->teamHp < -delta) {
            delta = (s16)-gBattle->teamHp;
        }
    }

    gBattle->teamHp += delta;

    if (side == 0) {
        gBattle->playerDamage -= delta;
        if (gBattle->playerDamage < 0) {
            gBattle->partnerDamage += gBattle->playerDamage;
            gBattle->playerDamage = 0;
        }
    } else {
        gBattle->partnerDamage -= delta;
        if (gBattle->partnerDamage < 0) {
            gBattle->playerDamage += gBattle->partnerDamage;
            gBattle->partnerDamage = 0;
        }
    }

    gBattle->player->currentHp  = gBattle->teamHp;
    gBattle->partner->currentHp = gBattle->teamHp;
}

void BtlMgr_AddUnitHp(s32 side, BattleActor* actor, s16 delta) {
    if (!(actor->flags & ACTOR_FLAG_BOSS_GAUGE_HP)) {
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
        s32 room = gBattle->bossMaxHp - gBattle->bossHp;

        if (room < delta) {
            delta = (s16)room;
        }
    } else {
        if (gBattle->bossHp < -delta) {
            delta = (s16)-gBattle->bossHp;
        }
    }

    gBattle->bossHp += delta;

    if (side == 0) {
        gBattle->bossDamageByPlayer -= delta;
        if (gBattle->bossDamageByPlayer < 0) {
            gBattle->bossDamageByPartner += gBattle->bossDamageByPlayer;
            gBattle->bossDamageByPlayer = 0;
        }
    } else {
        gBattle->bossDamageByPartner -= delta;
        if (gBattle->bossDamageByPartner < 0) {
            gBattle->bossDamageByPlayer += gBattle->bossDamageByPartner;
            gBattle->bossDamageByPartner = 0;
        }
    }

    actor->currentHp = gBattle->bossHp;
    if (actor->linkedActor != NULL) {
        actor->linkedActor->currentHp = actor->currentHp;
    }
}

BOOL BtlMgr_IsStageFrozen(u16 stage) {
    if ((u16)(stage + 0xFFFF) <= 1U) {
        return (gBattle->flags & (BATTLE_FLAG_SPECIAL | BATTLE_FLAG_FEAT)) != 0;
    }
    return FALSE;
}

/// Enters the ready state and starts the "Ready" intro.
static void BtlMgr_StartReady(void) {
    BtlMgr_SetState(BATTLE_STATE_READY);
    EasyTask_CreateTask(&gBattle->commonPool, &Tsk_BtlReady, NULL, 0, NULL, NULL);
}

/// Checks whether the battle's goal is met (enemies, boss HP or a scripted flag, per `goal`). Once it is, makes
/// the characters intangible and starts the victory (or the scripted end).
static void BtlMgr_CheckVictory(void) {
    BOOL             finished = FALSE;
    Ov003EnemyActor* unit;

    if (gBattle->state != BATTLE_STATE_FIGHT) {
        return;
    }

    switch (gBattle->goal) {
        case BATTLE_GOAL_DEFEAT_ALL:
            unit = BtlList_GetHead(&gBattle->enemiesMain);
            if (unit == NULL && BtlList_GetHead(&gBattle->enemiesSub) == NULL) {
                finished = TRUE;
            }
            break;

        case BATTLE_GOAL_BOSS_HP:
            if (gBattle->bossMaxHp > 0) {
                finished = gBattle->bossHp <= 0;
            }
            break;

        case BATTLE_GOAL_SCRIPTED:
            if (gBattle->flags & BATTLE_FLAG_SCRIPTED_GOAL_MET) {
                finished = TRUE;
            }
            break;
    }

    if (finished || BtlMgr_AreEnemiesDefeated()) {
        gBattle->flags |= (BATTLE_FLAG_NO_PAUSE | BATTLE_FLAG_SPECIAL_LOCKED);
        gBattle->flags |= BATTLE_FLAG_UNK_14;
        gBattle->flags |= BATTLE_FLAG_UNK_15;
        gBattle->player->flags |= ACTOR_FLAG_INTANGIBLE;
        gBattle->partner->flags |= ACTOR_FLAG_INTANGIBLE;
    }

    if (func_ov003_020bb47c() != 0) {
        return;
    }
    if (!finished) {
        return;
    }

    if (gBattle->flags & BATTLE_FLAG_SCRIPTED_END) {
        gBattle->flags |= BATTLE_FLAG_SE_MUTED;
        BtlMgr_SetState(BATTLE_STATE_SCRIPTED_END);
        gBattle->player->flags &= ~ACTOR_FLAG_INTANGIBLE;
        gBattle->partner->flags &= ~ACTOR_FLAG_INTANGIBLE;
        return;
    }

    if (EasyTask_CreateTask(&gBattle->commonPool, &Tsk_BtlWin, NULL, 0, NULL, NULL) != -1) {
        BtlMgr_SetState(BATTLE_STATE_WIN);
        gBattle->player->flags &= ~ACTOR_FLAG_INTANGIBLE;
        gBattle->partner->flags &= ~ACTOR_FLAG_INTANGIBLE;
    }
}

/// Allocates and sets up the battle's shared state, loads the battle graphics and spawns every battle task.
static void BtlMgr_StartBattle(u16 encounterId, s32 isContinuation) {
    void* paletteSrc;
    void* alloc;

    alloc = Mem_AllocHeapTail(&gDebugHeap, sizeof(Battle));
    Mem_SetSequence(&gDebugHeap, alloc, BtlMgr_HeapTag);
    gBattle = alloc;
    MI_CpuSet(alloc, 0, sizeof(Battle));

    gBattle->menuData   = DatMgr_LoadRawData(1, NULL, 0, (BinIdentifier*)&Iden_BtlMenu);
    gBattle->effectData = DatMgr_LoadRawData(1, NULL, 0, (BinIdentifier*)&Iden_BtlEffect);

    paletteSrc               = Data_GetPackEntryData(gBattle->menuData, 65);
    gBattle->menuPalettes[0] = PaletteMgr_AcquireContiguous(g_PaletteManagers[0], paletteSrc, 5, TRUE);
    gBattle->menuPalettes[1] = PaletteMgr_AcquireContiguous(g_PaletteManagers[1], paletteSrc, 5, TRUE);

    Mem_InitializeHeap(&gBattle->mainHeap, gBattle->mainHeapData, 0x8000);
    Mem_InitializeHeap(&gBattle->subHeap, gBattle->subHeapData, 0x8000);
    Mem_InitializeHeap(&gBattle->commonHeap, gBattle->commonHeapData, 0x10000);
    Mem_InitializeHeap(&gBattle->psychHeap, gBattle->psychHeapData, 0x14000);
    Mem_InitializeHeap(&gBattle->specialHeap, gBattle->specialHeapData, 0x8000);
    Mem_InitializeHeap(&gBattle->controlHeap, gBattle->controlHeapData, 0x1000);

    EasyTask_InitializePool(&gBattle->mainPool, &gBattle->mainHeap, 0x80, NULL, NULL);
    EasyTask_InitializePool(&gBattle->subPool, &gBattle->subHeap, 0x80, NULL, NULL);
    EasyTask_InitializePool(&gBattle->commonPool, &gBattle->commonHeap, 0x80, NULL, NULL);
    EasyTask_InitializePool(&gBattle->psychPool, &gBattle->psychHeap, 0x100, NULL, NULL);
    EasyTask_InitializePool(&gBattle->specialPool, &gBattle->specialHeap, 0x80, NULL, NULL);
    EasyTask_InitializePool(&gBattle->controlPool, &gBattle->controlHeap, 0x10, NULL, NULL);

    gBattle->unk3D90C            = 0;
    gBattle->goal                = BATTLE_GOAL_DEFEAT_ALL;
    gBattle->flags               = 0;
    gBattle->paused              = 0;
    gBattle->lrHeld              = 0;
    gBattle->specialFreeze       = 0;
    gBattle->unk3D888            = 0;
    gBattle->bossHp              = 0;
    gBattle->bossMaxHp           = 0;
    gBattle->bossDamageByPartner = 0;
    gBattle->bossDamageByPlayer  = 0;
    gBattle->comboCount          = 0;
    gBattle->techBonus           = 0;
    gBattle->popupCountMain      = 0;
    gBattle->popupCountSub       = 0;
    gBattle->unk3D8FE            = 0;
    gBattle->unk3D8FF            = 0;
    gBattle->unk3D900            = 0;
    gBattle->unk3D902            = 0;
    gBattle->lockOnTarget        = NULL;
    gBattle->chainIndex          = gSaveData.unk_3142 - 1;

    gBattle->partnerId   = BtlMgr_GetPartner(encounterId);
    gBattle->encounterId = encounterId;
    gBattle->bgmHandle   = gSaveData.unk_313C;

    if (gSaveData.battleCount > 1) {
        gBattle->flags |= BATTLE_FLAG_CHAIN;
    }

    BtlMgr_LoadOverlays(gBattle->partnerId, gBattle->encounterId);

    BtlTouch_Init();
    BtlMic_Init();
    BtlStage_Init();
    BtlCollision_Init();
    BtlAttackQueue_Init();
    BtlObsMgr_ResetList();

    gBattle->cmdQueue = NULL;

    BtlMgr_InitThreads(gBattle->partnerId);
    BtlMgr_InitBadges(gBattle->encounterId);
    BtlMgr_InitTeam(isContinuation);

    BtlList_Init(&gBattle->enemiesMain);
    BtlList_Init(&gBattle->enemiesSub);

    EasyTask_CreateTask(&gBattle->mainPool, &Tsk_BtlPlayer00, NULL, 0, NULL, NULL);

    switch (gBattle->partnerId) {
        case PARTNER_SHIKI:
            EasyTask_CreateTask(&gBattle->subPool, &Tsk_BtlPlayer01, NULL, 0, NULL, NULL);
            EasyTask_CreateTask(&gBattle->commonPool, &Tsk_BtlCard_Esp, NULL, 0, NULL, NULL);
            EasyTask_CreateTask(&gBattle->commonPool, &Tsk_BtlAura, NULL, 0, NULL, NULL);
            break;

        case PARTNER_JOSHUA:
            EasyTask_CreateTask(&gBattle->subPool, &data_ov004_020f0120, NULL, 0, NULL, NULL);
            EasyTask_CreateTask(&gBattle->commonPool, &data_ov005_020f0394, NULL, 0, NULL, NULL);
            EasyTask_CreateTask(&gBattle->commonPool, &Tsk_BtlAura, NULL, 0, NULL, NULL);
            break;

        case PARTNER_BEAT:
            EasyTask_CreateTask(&gBattle->subPool, &data_ov005_020f07a0, NULL, 0, NULL, NULL);
            EasyTask_CreateTask(&gBattle->commonPool, &data_ov005_020f0a94, NULL, 0, NULL, NULL);
            EasyTask_CreateTask(&gBattle->commonPool, &Tsk_BtlAura, NULL, 0, NULL, NULL);
            break;

        case 4:
            EasyTask_CreateTask(&gBattle->subPool, &Tsk_BtlPlayerLast, NULL, 0, NULL, NULL);
            EasyTask_CreateTask(&gBattle->commonPool, &Tsk_BtlAuraLast, NULL, 0, NULL, NULL);
            break;

        default:
            EasyTask_CreateTask(&gBattle->subPool, &Tsk_BtlPlayerNone, NULL, 0, NULL, NULL);
            break;
    }

    func_ov003_020c2a34(gBattle->encounterId);
    BtlStage_StartFieldTasks(0);
    BtlObsMgr_RollDrop();
    BtlMic_CreateTask();

    if (gBattle->partnerId < 3) {
        EasyTask_CreateTask(&gBattle->commonPool, &Tsk_BtlCmd, NULL, 0, NULL, NULL);
        EasyTask_CreateTask(&gBattle->commonPool, &Tsk_BtlInf_Star, NULL, 0, NULL, NULL);
    }

    EasyTask_CreateTask(&gBattle->commonPool, &Tsk_BtlBarHP, NULL, 0, NULL, NULL);
    EasyTask_CreateTask(&gBattle->commonPool, &Tsk_BtlTarget, NULL, 0, NULL, NULL);
    EasyTask_CreateTask(&gBattle->commonPool, &Tsk_BtlPop_Combo, NULL, 0, NULL, NULL);
    EasyTask_CreateTask(&gBattle->commonPool, &Tsk_BtlInf_Exp, NULL, 0, NULL, NULL);
    EasyTask_CreateTask(&gBattle->commonPool, &Tsk_BtlInf_Auto, NULL, 0, NULL, NULL);
    EasyTask_CreateTask(&gBattle->commonPool, &data_ov003_020dc614, NULL, 0, NULL, NULL);

    if (CombatAbility_IsActive(4) != 0) {
        EasyTask_CreateTask(&gBattle->commonPool, &Tsk_BtlInf_Timer, NULL, 0, NULL, NULL);
    }

    EasyTask_CreateTask(&gBattle->controlPool, &Task_BtlTch, NULL, 0, NULL, NULL);
    EasyTask_CreateTask(&gBattle->controlPool, &Tsk_BtlPause, NULL, 0, NULL, NULL);

    BtlStage_StartSandstorm();
    BtlDbg_CreateTask();
    BtlDebug_ShowSyncValues();
}

/// Per-frame battle logic: touch input, the clear timer, hit stop, and adding/removing tasks in every pool.
static void BtlMgr_Tick(void) {
    s32 pressed;

    BtlTouch_Update();
    BtlAttackQueue_DropInvalid();
    EasyTask_ProcessPendingTasks(&gBattle->controlPool);

    if (BtlBadgeMgr_AnyActive() != 0) {
        pressed = (InputStatus.buttonState.currButtons & (INPUT_BUTTON_L | INPUT_BUTTON_R)) != 0;
        if (gBattle->lrHeld != pressed) {
            BtlSe_Play(SEIDX_SE_WEAPON_SWITCH);
            gBattle->lrHeld = pressed;
        }
    }

    if (gBattle->paused != 0) {
        BtlBadgeMgr_DebugDrag();
        return;
    }

    BtlBadgeActor_SeparateAll();

    if (gBattle->state == BATTLE_STATE_FIGHT && (gBattle->flags & (BATTLE_FLAG_SPECIAL | BATTLE_FLAG_FEAT)) == 0) {
        gSaveData.battleClearTime++;
        if (gSaveData.battleClearTime > 0x57E40) {
            gSaveData.battleClearTime = 0x57E40;
        }
    }

    EasyTask_ProcessPendingTasks(&gBattle->specialPool);

    if (gBattle->specialFreeze != 0) {
        return;
    }

    if (gBattle->unk3D888 == 0) {
        EasyTask_ProcessPendingTasks(&gBattle->commonPool);
        if ((gBattle->flags & BATTLE_FLAG_ACTORS_FROZEN) == 0) {
            EasyTask_ProcessPendingTasks(&gBattle->psychPool);
        }
    }

    if (BtlDebug_GetSetting(BTL_DEBUG_SETTING_MODE) == BTL_DEBUG_MODE_NO_FRAME_DELAY) {
        EasyTask_ProcessPendingTasks(&gBattle->mainPool);
        EasyTask_ProcessPendingTasks(&gBattle->subPool);
        BtlCollision_Update();
        return;
    }

    if (gBattle->hitStopMain > 0) {
        gBattle->hitStopMain--;
    } else {
        gBattle->hitStopMain     = gBattle->hitStopMainNext;
        gBattle->hitStopMainNext = 0;
        EasyTask_ProcessPendingTasks(&gBattle->mainPool);
        BtlCollision_Update();
    }

    if (gBattle->hitStopSub > 0) {
        gBattle->hitStopSub--;
    } else {
        gBattle->hitStopSub     = gBattle->hitStopSubNext;
        gBattle->hitStopSubNext = 0;
        EasyTask_ProcessPendingTasks(&gBattle->subPool);
    }
}

/// Runs every task pool's tasks for the frame.
static void BtlMgr_RunTasks(void) {
    EasyTask_UpdateActiveTasks(&gBattle->controlPool);
    EasyTask_UpdateActiveTasks(&gBattle->specialPool);

    if (gBattle->specialFreeze == 0) {
        EasyTask_UpdateActiveTasks(&gBattle->commonPool);
        EasyTask_UpdateActiveTasks(&gBattle->psychPool);
        EasyTask_UpdateActiveTasks(&gBattle->mainPool);
        EasyTask_UpdateActiveTasks(&gBattle->subPool);
        BtlDebug_DrawCollision();
    }
}

/// Saves the team's HP, destroys every task pool and frees the battle's shared state; unloads the battle overlays
/// unless the next battle of the chain reuses them.
static void BtlMgr_EndBattle(s32 keepOverlays) {
    gSaveData.battleHpGauge     = gBattle->teamHp;
    gSaveData.battleHpGaugeSub0 = gBattle->playerDamage;
    gSaveData.battleHpGaugeSub1 = gBattle->partnerDamage;

    BtlBadgeMgr_PublishSlots();

    EasyTask_DestroyPool(&gBattle->mainPool);
    EasyTask_DestroyPool(&gBattle->subPool);
    EasyTask_DestroyPool(&gBattle->commonPool);
    EasyTask_DestroyPool(&gBattle->psychPool);
    EasyTask_DestroyPool(&gBattle->specialPool);
    EasyTask_DestroyPool(&gBattle->controlPool);

    if (keepOverlays == 0) {
        OvlMgr_UnloadOverlay(3);
        OvlMgr_UnloadOverlay(4);
        OvlMgr_UnloadOverlay(5);
    }

    PaletteMgr_ReleaseResource(g_PaletteManagers[0], gBattle->menuPalettes[0]);
    PaletteMgr_ReleaseResource(g_PaletteManagers[1], gBattle->menuPalettes[1]);
    DatMgr_ReleaseData(gBattle->menuData);
    DatMgr_ReleaseData(gBattle->effectData);
    Mem_Free(&gDebugHeap, gBattle);
}

/// Attack Boost stickers owned for the active partner.
static s32 BtlMgr_GetAttackBoost(void) {
    switch (gBattle->partnerId) {
        case PARTNER_SHIKI:
            return (s16)Inventory_GetOwnedCount(0x2B2);
        case PARTNER_JOSHUA:
            return (s16)Inventory_GetOwnedCount(0x2BA);
        case PARTNER_BEAT:
            return (s16)Inventory_GetOwnedCount(0x2C2);
        default:
            return 0;
    }
}

/// Defense Boost stickers owned for the active partner.
static s32 BtlMgr_GetDefenseBoost(void) {
    switch (gBattle->partnerId) {
        case PARTNER_SHIKI:
            return (s16)Inventory_GetOwnedCount(0x2B3);
        case PARTNER_JOSHUA:
            return (s16)Inventory_GetOwnedCount(0x2BB);
        case PARTNER_BEAT:
            return (s16)Inventory_GetOwnedCount(0x2C3);
        default:
            return 0;
    }
}

BOOL BtlMgr_IsTargetOnSide(Ov003EnemyActor* unit, s32 side) {
    if ((unit->actor.flags & ACTOR_FLAG_DEFEATED) || (unit->actor.flags & ACTOR_FLAG_INTANGIBLE)) {
        return FALSE;
    }
    if (side != 0) {
        if (gBattle->stages[1].centerX <= unit->actor.position.x) {
            return FALSE;
        }
        return TRUE;
    }
    return unit->actor.position.x > gBattle->stages[1].centerX;
}

Ov003EnemyActor* BtlMgr_FindNearestTarget(s32 x, s32 side, s32 airborne) {
    Battle*          g        = gBattle;
    Ov003EnemyActor* best     = NULL;
    s32              bestDist = 0x100000;
    Ov003EnemyActor* unit;

    for (unit = BtlList_GetHead(&g->enemiesSub); unit != NULL; unit = BtlList_GetNext(&unit->node)) {
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

Ov003EnemyActor* BtlMgr_FindFarthestTarget(s32 x, s32 side, s32 airborne) {
    Battle*          g        = gBattle;
    Ov003EnemyActor* best     = NULL;
    s32              bestDist = 0;
    Ov003EnemyActor* unit;

    for (unit = BtlList_GetHead(&g->enemiesSub); unit != NULL; unit = BtlList_GetNext(&unit->node)) {
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

void BtlMgr_GainExp(s32 side, s16 exp) {
    if (CombatAbility_IsActive(0x1F) != 0) {
        return;
    }
    if (BtlMgr_HasAbility(side, 0x1C) != 0) {
        s16 bonus = BtlMgr_GetAbilityValue(side, 0x1C);

        exp += (s16)((exp * bonus) / 100);
    }
    BtlMgr_AwardExp(exp);
}

void BtlMgr_GainPP(s32 side, s16 pp) {
    if (CombatAbility_IsActive(0x1E) != 0) {
        return;
    }
    BtlMgr_AddPP(pp);
}

BOOL BtlMgr_HasAbility(s32 side, u32 effect) {
    if (side != 1) {
        return CombatAbility_IsActiveForPlayer(effect);
    }
    return CombatAbility_IsActiveForPartner(effect);
}

s16 BtlMgr_GetAbilityValue(s32 side, u32 effect) {
    if (side != 1) {
        return (s16)CombatAbility_GetPlayerValue(effect);
    }
    return (s16)CombatAbility_GetPartnerValue(effect);
}

s32 BtlMgr_GetAttack(s32 side) {
    u32 partner;

    switch (gBattle->partnerId) {
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
            partner = 0xff;
            break;
    }

    if (side != 1) {
        return Stats_GetEffectiveValue(partner, 0);
    }
    return (s16)(BtlMgr_GetAttackBoost() + Stats_GetEffectiveFriendValue(partner, 0));
}

s32 BtlMgr_GetDefense(s32 side) {
    u32 partner;

    switch (gBattle->partnerId) {
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
            partner = 0xff;
            break;
    }

    if (side != 1) {
        return Stats_GetEffectiveValue(partner, 1);
    }
    return (s16)(BtlMgr_GetDefenseBoost() + Stats_GetEffectiveFriendValue(partner, 1));
}

void BtlMgr_RecordDamageDealt(s32 side, s32 damage) {
    if (side == 1) {
        gSaveData.unk_317D |= 2;
    } else {
        gSaveData.unk_317D |= 1;
    }

    if (gSaveData.unk_317E < damage) {
        gSaveData.unk_317E = damage;
    }
}

void BtlMgr_CountTeamHit(void) {
    if (gSaveData.unk_318F != 0xFF) {
        gSaveData.unk_318F++;
    }
}

void BtlMgr_CountAuraPass(void) {
    gBattle->auraPasses++;
    if (gSaveData.unk_317C < gBattle->auraPasses) {
        gSaveData.unk_317C = gBattle->auraPasses;
    }
}

/// Applies ability 0x4B to the PP earned, then sets the battle's bonus flags from its records.
static void BtlMgr_CalcBonuses(void) {
    Battle* g = gBattle;

    if (CombatAbility_IsActive(0x4B) != 0) {
        s32 pp = gSaveData.battleBasePP * CombatAbility_GetValue(0x4B);

        gSaveData.battleBasePP = (s16)((pp / 100) - (pp >> 31));
    }

    if (g->lrHeld == 0) {
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

    switch (g->partnerId) {
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

/// Records a hard-mode clear time of a scripted battle.
static void BtlMgr_RecordClearTime(u32 unused) {
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

/// Initialises the graphics hardware.
static void BtlMgr_InitHardware(void) {
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

/// Configures both screens' display modes, layers and blending.
static void BtlMgr_InitDisplay(void) {
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

/// VBlank callback: commits the display settings, DMA, OAM and palettes.
static void BtlMgr_VBlank(void) {
    if (SystemStatusFlags.vblank != 0) {
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
}

/// Sets up the graphics hardware and the VBlank callback.
static void BtlMgr_InitGraphics(void) {
    BtlMgr_InitHardware();
    BtlMgr_InitDisplay();
    Interrupts_RegisterVBlankCallback(BtlMgr_VBlank, 1);
}

/// Removes the VBlank callback.
static void BtlMgr_ExitGraphics(void) {
    Interrupts_ForceVBlank();
    Interrupts_RegisterVBlankCallback(NULL, 1);
}

/// Sets up a single-battle chain fighting `encounterId`, on field `fieldId` unless it is 0.
static void BtlMgr_SetEncounter(u16 encounterId, s32 fieldId) {
    gSaveData.battleCount         = 1;
    gSaveData.battleEncounters[0] = encounterId;
    if (fieldId != 0) {
        gSaveData.unk_3138 = fieldId;
    }
}

/// Number of tracks in `mask` not played yet.
static s32 BtlMgr_CountUnplayedTracks(u32 mask) {
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

/// Picks a random track of the chapter not played yet, starting over once all were played.
static u16 BtlMgr_PickTrack(void) {
    u32 mask  = BtlMgr_ChapterTracks[gSaveData.chapter];
    s32 count = BtlMgr_CountUnplayedTracks(mask);
    s32 rng;
    s32 i;

    if (count <= 0) {
        gSaveData.unk_3234 = 0;
        count              = BtlMgr_CountUnplayedTracks(mask);
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

/// Sets up the encounter chain and field of a scripted battle.
static void BtlMgr_SetupScriptedBattle(u32 selector) {
    switch (selector) {
        case 0:
        case 1:
            return;

        case 2:
            BtlMgr_SetEncounter(0x32, 1);
            return;
        case 3:
            BtlMgr_SetEncounter(0x33, 4);
            return;
        case 4:
            BtlMgr_SetEncounter(0x35, 2);
            return;
        case 5:
            BtlMgr_SetEncounter(0x36, 2);
            return;
        case 6:
            BtlMgr_SetEncounter(0x37, 2);
            return;
        case 7:
            BtlMgr_SetEncounter(0x7C, 4);
            return;
        case 8:
            BtlMgr_SetEncounter(0x7D, 4);
            return;
        case 9:
            BtlMgr_SetEncounter(0x3B, 0x16);
            return;
        case 10:
            BtlMgr_SetEncounter(7, 0x2A);
            return;
        case 11:
            BtlMgr_SetEncounter(0x81, 0xE);
            return;
        case 12:
            BtlMgr_SetEncounter(0x82, 0xE);
            return;
        case 13:
            BtlMgr_SetEncounter(0x84, 7);
            return;
        case 14:
            BtlMgr_SetEncounter(0x13, 0x2A);
            return;
        case 15:
            BtlMgr_SetEncounter(9, 0x1A);
            return;
        case 16:
            BtlMgr_SetEncounter(0x88, 4);
            return;
        case 17:
            BtlMgr_SetEncounter(0x89, 2);
            return;
        case 18:
            BtlMgr_SetEncounter(0x3C, 1);
            return;
        case 19:
            BtlMgr_SetEncounter(0x8A, 0xD);
            return;
        case 20:
            BtlMgr_SetEncounter(0x17, 0xE);
            return;
        case 21:
            BtlMgr_SetEncounter(0x18, 8);
            return;
        case 22:
            BtlMgr_SetEncounter(0x19, 0x10);
            return;
        case 23:
            BtlMgr_SetEncounter(0x90, 0x13);
            return;
        case 24:
            BtlMgr_SetEncounter(0x96, 0);
            return;
        case 25:
            BtlMgr_SetEncounter(0x91, 1);
            return;
        case 26:
            BtlMgr_SetEncounter(0x92, 6);
            return;
        case 27:
            BtlMgr_SetEncounter(0x93, 10);
            return;
        case 28:
            BtlMgr_SetEncounter(0x94, 5);
            return;
        case 29:
            BtlMgr_SetEncounter(0x95, 8);
            return;
        case 30:
            BtlMgr_SetEncounter(0xF, 0x28);
            return;
        case 31:
            BtlMgr_SetEncounter(0x97, 4);
            return;
        case 32:
            BtlMgr_SetEncounter(0x98, 4);
            return;
        case 33:
            BtlMgr_SetEncounter(0x3D, 1);
            return;
        case 34:
            BtlMgr_SetEncounter(0x99, 0x14);
            return;
        case 35:
            BtlMgr_SetEncounter(0x9A, 0x16);
            return;
        case 36:
            BtlMgr_SetEncounter(8, 0x2A);
            return;
        case 37:
            BtlMgr_SetEncounter(0x1D, 6);
            return;
        case 38:
            BtlMgr_SetEncounter(0x1E, 0xE);
            return;
        case 39:
            BtlMgr_SetEncounter(0x1C, 0xE);
            return;
        case 40:
            BtlMgr_SetEncounter(0x1F, 10);
            return;
        case 41:
            BtlMgr_SetEncounter(0x9B, 6);
            return;
        case 42:
            BtlMgr_SetEncounter(0x9C, 8);
            return;
        case 43:
            BtlMgr_SetEncounter(0x21, 0xD);
            return;
        case 44:
            BtlMgr_SetEncounter(10, 0x2A);
            return;
        case 45:
            BtlMgr_SetEncounter(0x1B, 0x2A);
            return;
        case 46:
            BtlMgr_SetEncounter(0x14, 0x2A);
            return;
        case 47:
            BtlMgr_SetEncounter(0x16, 0x2A);
            return;
        case 48:
            BtlMgr_SetEncounter(0x20, 0x28);
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
            BtlMgr_SetEncounter(0x38, 0x13);
            return;
        case 51:
            BtlMgr_SetEncounter(0x39, 0x13);
            return;
        case 52:
            BtlMgr_SetEncounter(0x3A, 0x13);
            return;
    }
}

/// Picks the encounter of the chain's current battle, stepping through the phases of multi-phase encounters.
static void BtlMgr_PickEncounter(BattleMgrWork* work) {
    u16 encounter = gSaveData.battleEncounters[gSaveData.unk_3142 - 1];

    switch (encounter) {
        case 10:
            work->encounterId = BtlMgr_Encounter10Phases[gSaveData.battleTimer];
            work->phaseCount  = 4;
            break;

        case 0x14:
            work->encounterId = BtlMgr_Encounter20Phases[gSaveData.battleTimer];
            work->phaseCount  = 1;
            break;

        case 0x33:
            work->encounterId = BtlMgr_Encounter51Phases[gSaveData.battleTimer];
            work->phaseCount  = 1;
            break;

        default:
            work->encounterId = encounter;
            work->phaseCount  = 0;
            break;
    }
}

/// Plays a battle track (unless the chain is already playing it) and marks it played.
static void BtlMgr_PlayTrack(BattleMgrWork* work, u32 track) {
    gSaveData.unk_313C = BtlMgr_TrackFiles[track];

    if (gSaveData.battleTimer == 0 || gSaveData.unk_3130 == 1) {
        CriSndMgr_PlayFile(gSaveData.unk_313C);
    }

    gSaveData.unk_3234 |= 1 << track;
    work->musicId = track;
}

/// Resets the sound heap and picks the battle track: fixed for scripted and boss encounters, random otherwise.
static void BtlMgr_PickMusic(BattleMgrWork* work, s32 isContinuation) {
    u16 music;

    if (isContinuation != 0) {
        SndMgr_DeepResetSequenceHeap();
    } else {
        SndMgr_ResetSequenceHeap();
    }

    if (gSaveData.unk_3134 == 0x31) {
        BtlMgr_PlayTrack(work, 9);
        return;
    }

    if (work->encounterId >= 0x9D && work->encounterId <= 0xCE) {
        BtlMgr_PlayTrack(work, 0xE);
        return;
    }

    if (gSaveData.unk_3130 == 2 && work->encounterId != 0x16 && work->encounterId != 0x20) {
        BtlMgr_PlayTrack(work, 0xF);
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
            music = BtlMgr_PickTrack();
            if (gSaveData.unk_313C == BtlMgr_TrackFiles[music] &&
                BtlMgr_CountUnplayedTracks(BtlMgr_ChapterTracks[gSaveData.chapter] > 1) != 0)
            {
                gSaveData.unk_3234 |= 1 << music;
                music = BtlMgr_PickTrack();
                gSaveData.unk_3234 &= ~(1 << music);
            }
            break;
    }

    BtlMgr_PlayTrack(work, music);
}

/// Advances to the next phase or battle of the chain. Returns TRUE once the chain is over.
static BOOL BtlMgr_AdvanceChain(BattleMgrWork* work) {
    gSaveData.battleTimer++;
    if (gSaveData.battleTimer > work->phaseCount) {
        gSaveData.unk_3142++;
        gSaveData.battleTimer = 0;
    }
    return gSaveData.unk_3142 > gSaveData.battleCount;
}

/// Copies the battle's level and experience back to the player's stats.
static void BtlMgr_SaveLevel(void) {
    gSaveData.playerStats.currentLevel   = gSaveData.battleLevel;
    gSaveData.playerStats.maxLevel       = gSaveData.battleMaxLevel;
    gSaveData.playerStats.expToNextLevel = gSaveData.battleExpToNextLevel;
}

/// Leaves the battle for the scene its outcome (`work->step`) leads to.
static void BtlMgr_LeaveBattle(BattleMgrWork* work) {
    BOOL isTrueEnd = gSaveData.unk_3130 == 3;

    switch (work->step) {
        case 3: {
            OverlayTag tag;
            OverlayTag tag2;

            BtlMgr_SaveLevel();
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

            BtlMgr_SaveLevel();
            BtlMgr_CalcBonuses();
            BtlMgr_RecordClearTime(work->encounterId);
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

            BtlMgr_SaveLevel();
            BtlMgr_CalcBonuses();
            BtlMgr_RecordClearTime(work->encounterId);
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
            work->keepOverlays = 1;
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

/// Leaves a retried battle for the scene its outcome (`work->step`) leads to.
static void BtlMgr_LeaveRetry(BattleMgrWork* work) {
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
            work->keepOverlays = 1;
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

/// Fades the battle in, then waits for the intro.
static void BtlMgr_FadeIn(BattleMgrWork* work) {
    if (work->fadeCounter == 0) {
        EasyFade_FadeBothDisplays(0, 0, 0x1000);
        work->active = 1;
        work->fadeCounter++;
    }
    if (!EasyFade_IsFading()) {
        work->fadeCounter = 0;
        work->unk_11E1A   = 0;
        DebugOvlDisp_ReplaceTop((OverlayCB)BtlMgr_WaitReady, work, 0);
    }
}

/// Fades the battle out, then ends the battle callbacks.
static void BtlMgr_FadeOut(BattleMgrWork* work) {
    if (work->fadeCounter == 0) {
        EasyFade_FadeBothDisplays(0, 0x10, 0x1000);
        work->active = 1;
        work->fadeCounter++;
    }
    if (!EasyFade_IsFading()) {
        DebugOvlDisp_Pop();
    }
}

/// Waits for the intro to end, then runs the battle.
static void BtlMgr_WaitReady(BattleMgrWork* work) {
    work->active = gBattle->paused == 0;

    if (gBattle->state != BATTLE_STATE_READY) {
        work->fadeCounter = 0;
        work->unk_11E1A   = 0;
        DebugOvlDisp_ReplaceTop((OverlayCB)BtlMgr_RunBattle, work, 0);
    }
}

/// Runs the battle until it is won, lost or ended, then picks where it leads.
static void BtlMgr_RunBattle(BattleMgrWork* work) {
    work->active = gBattle->paused == 0;
    BtlMgr_CheckVictory();

    switch (gBattle->state) {
        case BATTLE_STATE_QUIT:
            work->step        = 3;
            work->fadeCounter = 0;
            work->unk_11E1A   = 0;
            DebugOvlDisp_ReplaceTop((OverlayCB)BtlMgr_FadeOut, work, 0);
            return;

        case BATTLE_STATE_WIN:
            work->step        = BtlMgr_AdvanceChain(work) ? 0 : 2;
            work->fadeCounter = 0;
            work->unk_11E1A   = 0;
            DebugOvlDisp_ReplaceTop((OverlayCB)BtlMgr_WaitResults, work, 0);
            return;

        case BATTLE_STATE_LOSE:
            work->step        = 1;
            work->fadeCounter = 0;
            work->unk_11E1A   = 0;
            DebugOvlDisp_ReplaceTop((OverlayCB)BtlMgr_WaitResults, work, 0);
            return;

        case BATTLE_STATE_SCRIPTED_END:
            work->step        = BtlMgr_AdvanceChain(work) ? 4 : 2;
            work->fadeCounter = 0;
            work->unk_11E1A   = 0;
            DebugOvlDisp_ReplaceTop((OverlayCB)BtlMgr_FadeOut, work, 0);
            return;

        default:
            return;
    }
}

/// Waits for the result screens to finish, then fades out.
static void BtlMgr_WaitResults(BattleMgrWork* work) {
    work->active = gBattle->paused == 0;

    if (gBattle->state == BATTLE_STATE_RESULTS_DONE) {
        work->fadeCounter = 0;
        work->unk_11E1A   = 0;
        DebugOvlDisp_ReplaceTop((OverlayCB)BtlMgr_FadeOut, work, 0);
    }
}

/// Overlay init stage: allocates the work area and starts the chain's next battle.
static void BtlMgr_Init(BattleMgrWork* work) {
    if (work == NULL) {
        char* sequence = BtlMgr_SequenceName;

        work = Mem_AllocHeapTail(&gDebugHeap, sizeof(BattleMgrWork));
        Mem_SetSequence(&gDebugHeap, work, sequence);
        MainOvlDisp_SetCbArg(work);
    }

    MI_CpuFill(0, work, Mem_GetBlockSize(&gDebugHeap, work));

    Mem_InitializeHeap(&work->heap, work->heapData, 0x800);
    EasyTask_InitializePool(&work->taskPool, &work->heap, 8, NULL, NULL);

    if (gSaveData.unk_3130 == 0 || gSaveData.unk_3130 == 1 || gSaveData.unk_3130 == 2 || gSaveData.unk_3130 == 4) {
        BtlDebug_InitSettings();
    }

    BtlMgr_InitGraphics();
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

        BtlMgr_SetupScriptedBattle(gSaveData.unk_3134);
        BtlMgr_PickEncounter(work);
        BtlMgr_PickMusic(work, 0);
        BtlMgr_StartBattle(work->encounterId, 0);
    } else {
        BtlMgr_PickEncounter(work);
        BtlMgr_PickMusic(work, 1);
        BtlMgr_StartBattle(work->encounterId, 1);
    }

    work->fadeCounter  = 0;
    work->unk_11E1A    = 0;
    work->active       = 1;
    work->step         = 5;
    work->keepOverlays = 0;

    BtlMgr_StartReady();
    DebugOvlDisp_Init();
    DebugOvlDisp_Push((OverlayCB)BtlMgr_FadeIn, work, 0);
    MainOvlDisp_NextProcessStage();
    MainOvlDisp_Run();
}

/// Overlay main stage: runs one frame of the battle.
static void BtlMgr_Main(BattleMgrWork* work) {
    TouchInput_Update();
    OamMgr_Reset3DState();
    OamMgr_Reset(&g_OamMgr[DISPLAY_MAIN], 0, 0);
    OamMgr_Reset(&g_OamMgr[DISPLAY_SUB], 0, 0);
    OamMgr_SetAffineCount(&g_OamMgr[DISPLAY_EXTENDED], 0);
    OamMgr_ResetCommandQueues(&g_OamMgr[DISPLAY_MAIN]);
    OamMgr_ResetCommandQueues(&g_OamMgr[DISPLAY_SUB]);

    BtlMgr_Tick();
    BtlMgr_RunTasks();
    DebugOvlDisp_Run();

    if (DebugOvlDisp_IsStackAtBase() != 0) {
        if (gSaveData.unk_3130 == 1) {
            BtlMgr_LeaveRetry(work);
        } else {
            BtlMgr_LeaveBattle(work);
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

/// Overlay exit stage: ends the battle and frees the work area.
static void BtlMgr_Exit(BattleMgrWork* work) {
    if (work != NULL) {
        BtlMgr_EndBattle(work->keepOverlays);
        Input_Init(&InputStatus, 8, 1, 2);
        BtlMgr_ExitGraphics();
        EasyTask_DestroyPool(&work->taskPool);
        Mem_Free(&gDebugHeap, work);
        MainOvlDisp_SetCbArg(NULL);
    }
}

void ProcessOverlay_Battle(BattleMgrWork* work) {
    s32 stage = MainOvlDisp_GetProcessStage();

    if (stage == PROCESS_STAGE_EXIT) {
        BtlMgr_Exit(work);
    } else {
        BtlMgr_ProcessStages[stage](work);
    }
}
