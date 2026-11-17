#ifndef TWEWY_BATTLE_CORE_BTLMGR_H
#define TWEWY_BATTLE_CORE_BTLMGR_H

#include "Battle/Core/BtlActor.h"
#include "Battle/Core/BtlAttackQueue.h"
#include "Battle/Core/BtlCmdDeck.h"
#include "Battle/Core/BtlCmdQueue.h"
#include "Battle/Core/BtlList.h"
#include "Battle/Core/BtlStage.h"
#include "Battle/Core/BtlTouch.h"
#include "Combat/Core/CombatAbility.h"
#include "Combat/Core/CombatEnemyActor.h"
#include "Combat/Core/CombatMic.h"
#include "Combat/Core/CombatPsych.h"
#include "Engine/Core/Memory.h"
#include "Engine/EasyTask.h"
#include "Engine/File/BinMgr.h"
#include "Engine/File/DatMgr.h"
#include "Engine/Resources/PaletteMgr.h"
#include "Engine/Resources/ResourceMgr.h"

/// The battle overlay's work area, allocated from `gDebugHeap` and passed to every overlay callback.
typedef struct BattleMgrWork {
    /* 0x00000 */ TaskPool        taskPool;
    /* 0x00080 */ Heap            heap;
    /* 0x0008C */ u8              heapData[0x800];
    /* 0x0088C */ ResourceManager resources;
    /* 0x11E0C */ s32             active;      // the work area's own tasks (the fade) run only while set
    /* 0x11E10 */ u16             step;        // how the battle ended, picks the scene it leads to
    /* 0x11E12 */ u16             musicId;     // battle track index playing
    /* 0x11E14 */ u16             encounterId; // encounter being fought
    /* 0x11E16 */ u16             phaseCount;  // extra phases of a multi-phase encounter
    /* 0x11E18 */ s16             fadeCounter;
    /* 0x11E1A */ s16             unk_11E1A;
    /* 0x11E1C */ s32             keepOverlays; // the next battle of the chain reuses the loaded overlays
} BattleMgrWork;                                // Size: 0x11E20

/// Phases of a battle, set through `BtlMgr_SetState`.
typedef enum BattleState {
    BATTLE_STATE_SETUP,        // tasks are being set up
    BATTLE_STATE_READY,        // the "Ready" intro plays
    BATTLE_STATE_FIGHT,        // the battle is running
    BATTLE_STATE_WIN,          // the goal was met, the victory plays
    BATTLE_STATE_LOSE,         // the team was defeated
    BATTLE_STATE_QUIT,         // abandoned from the pause menu
    BATTLE_STATE_SCRIPTED_END, // a story battle ended without a victory
    BATTLE_STATE_RESULTS_DONE, // the victory or defeat screens finished
} BattleState;

/// What the battle must achieve to be won.
typedef enum BattleGoal {
    BATTLE_GOAL_DEFEAT_ALL, // no enemy left on either screen
    BATTLE_GOAL_BOSS_HP,    // the shared boss HP is used up
    BATTLE_GOAL_SCRIPTED,   // an enemy script raises the scripted-goal flag
} BattleGoal;

/// The two halves of the team, each fighting on its own screen.
typedef enum BattleSide {
    BATTLE_SIDE_PLAYER,  // Neku, on the main screen
    BATTLE_SIDE_PARTNER, // the partner, on the sub screen
} BattleSide;

/// Battle-wide state bits, held in `Battle::flags`.
typedef enum {
    BATTLE_FLAG_SANDSTORM         = (1 << 0),  // the sandstorm task is alive
    BATTLE_FLAG_SPECIAL           = (1 << 1),  // a special attack or boss finale is playing: enemies stop acting
    BATTLE_FLAG_FEAT              = (1 << 2),  // the special attack's Feat is playing out
    BATTLE_FLAG_LEVEL_UP          = (1 << 3),  // the level-up effect plays: the player stays down, combos hold
    BATTLE_FLAG_PARTNER_AUTO      = (1 << 4),  // the partner's card game plays itself (the "Auto" indicator shows)
    BATTLE_FLAG_SE_MUTED          = (1 << 5),  // sound effects are muted (game over, scripted end)
    BATTLE_FLAG_ACTORS_FROZEN     = (1 << 6),  // players, psychs, auras, enemy shots and pins stop updating
    BATTLE_FLAG_AURA_SPLASH       = (1 << 7),  // a finisher passed the aura: the next splash effect may spawn
    BATTLE_FLAG_FIELDS_STOPPED    = (1 << 12), // the field background tasks end until BtlStage_StartFieldTasks
    BATTLE_FLAG_SANDSTORM_STOPPED = (1 << 13), // the sandstorm task ends
    BATTLE_FLAG_UNK_14            = (1 << 14), // set once the battle is finished; never read
    BATTLE_FLAG_UNK_15            = (1 << 15), // set once the battle is finished; never read
    BATTLE_FLAG_NO_PAUSE          = (1 << 16), // the pause menu cannot be opened
    BATTLE_FLAG_SPECIAL_LOCKED    = (1 << 17), // a special attack or boss finale was started; no other may start
    BATTLE_FLAG_NO_LEVEL_UP       = (1 << 19), // experience gained cannot level the player up
    BATTLE_FLAG_SILENT_READY      = (1 << 20), // the partner says no line during the "Ready" intro
    BATTLE_FLAG_SCRIPTED_END      = (1 << 21), // a met goal ends the battle as BATTLE_STATE_SCRIPTED_END
    BATTLE_FLAG_SCRIPTED_GOAL_MET = (1 << 29), // an enemy script met BATTLE_GOAL_SCRIPTED
    BATTLE_FLAG_NO_GUARD_TURN     = (1 << 30), // guarding does not turn the player to face the attacker
    BATTLE_FLAG_CHAIN             = (1 << 31), // a later battle of a chain: the "Ready" intro shows its number
} BattleFlags;

/// The battle's shared state: task pools, the team, the enemy lists and the battle's progress. Allocated by
/// `BtlMgr` for each battle; see `gBattle`.
typedef struct Battle {
    /* 0x00000 */ TaskPool          mainPool; // the player and the main screen's actors, paused by hit stop
    /* 0x00080 */ Heap              mainHeap;
    /* 0x0008C */ u8                mainHeapData[0x8000];
    /* 0x0808C */ TaskPool          subPool; // the partner and the sub screen's actors, paused by hit stop
    /* 0x0810C */ Heap              subHeap;
    /* 0x08118 */ u8                subHeapData[0x8000];
    /* 0x10118 */ TaskPool          commonPool; // fields, the interface, auras and phase banners
    /* 0x10198 */ Heap              commonHeap;
    /* 0x101A4 */ u8                commonHeapData[0x10000];
    /* 0x201A4 */ TaskPool          psychPool; // psych effects
    /* 0x20224 */ Heap              psychHeap;
    /* 0x20230 */ u8                psychHeapData[0x14000];
    /* 0x34230 */ TaskPool          specialPool; // fusion, special attacks and boss finales
    /* 0x342B0 */ Heap              specialHeap;
    /* 0x342BC */ u8                specialHeapData[0x8000];
    /* 0x3C2BC */ TaskPool          controlPool; // touch input, the pause menu and the game over; always runs
    /* 0x3C33C */ Heap              controlHeap;
    /* 0x3C348 */ u8                controlHeapData[0x1000];
    /* 0x3D348 */ PaletteResource*  menuPalettes[2]; // battle interface palettes, per display engine
    /* 0x3D350 */ Data*             menuData;        // the `Iden_BtlMenu` pack
    /* 0x3D354 */ Data*             effectData;      // the `Iden_BtlEffect` pack
    /* 0x3D358 */ Ov003EffectSlots  effectSlots;
    /* 0x3D370 */ BattleTouch       touchWork;
    /* 0x3D3CC */ BattleAttackQueue attackQueue;
    /* 0x3D4BC */ Ov003TouchEvent   touchEvents[14];
    /* 0x3D75C */ u8                unk_3D75C[4];
    /* 0x3D760 */ BtlMicInput       microphone;
    /* 0x3D7A4 */ u8                unk_3D7A4[0x3D7C0 - 0x3D7A4];
    /* 0x3D7C0 */ BattleStage       stages[2];     // [0] main engine, [1] sub engine
    /* 0x3D870 */ s32               bgmHandle;     // sound handle of the battle music
    /* 0x3D874 */ u8                state;         // BattleState
    /* 0x3D875 */ u8                goal;          // BattleGoal
    /* 0x3D876 */ char              unk_3D876[0x2];
    /* 0x3D878 */ u32               flags;         // BattleFlags
    /* 0x3D87C */ s32               paused;        // the pause menu is open
    /* 0x3D880 */ s32               lrHeld;        // L or R was held last frame
    /* 0x3D884 */ s32               specialFreeze; // special attack or boss finale: other pools stop
    /* 0x3D888 */ s32               unk3D888;
    /* 0x3D88C */ s8                chainIndex;    // battle index within the chain, 0-based
    /* 0x3D88D */ u8                partnerId;     // PARTNER_*, 3 = none, 4 = the final battle
    /* 0x3D88E */ u16               encounterId;   // encounter of the current battle
    /* 0x3D890 */ u8                musicId;       // battle track index
    /* 0x3D892 */ s16               dropRate;
    /* 0x3D894 */ s16               moveSpeed;     // the player's movement speed, percent
    /* 0x3D896 */ s16               partnerSync;   // the partner's sync rate
    /* 0x3D898 */ BattleActor*      player;        // Neku (BtlPlayer00), on the main engine
    /* 0x3D89C */ BattleActor*      partner;       // the partner, on the sub engine
    /* 0x3D8A0 */ void*             lockOnTarget;  // the enemy the target marker follows
    /* 0x3D8A4 */ BattleList        enemiesMain;   // enemies on the main screen, attacked by the player
    /* 0x3D8AC */ BattleList        enemiesSub;    // enemies on the sub screen, attacked by the partner
    /* 0x3D8B4 */ BattleList        obsList;       // field objects (BattleObsActor)
    /* 0x3D8BC */ BattleList        unk3D8BC;
    /* 0x3D8C4 */ u16               touchActorCount;
    /* 0x3D8C6 */ u8                unk_3D8C6[0x2];
    /* 0x3D8C8 */ s32               unk3D8C8;
    /* 0x3D8CC */ s32               unk3D8CC;
    /* 0x3D8D0 */ s16               teamHp;              // HP shared by the player and the partner
    /* 0x3D8D2 */ s16               teamMaxHp;
    /* 0x3D8D4 */ s16               partnerDamage;       // team HP lost to hits on the partner
    /* 0x3D8D6 */ s16               playerDamage;        // team HP lost to hits on the player
    /* 0x3D8D8 */ s16               bossHp;              // HP shared by the parts of a boss (ACTOR_FLAG_BOSS_GAUGE_HP)
    /* 0x3D8DA */ s16               bossMaxHp;
    /* 0x3D8DC */ s16               bossDamageByPartner; // boss HP taken by the partner's attacks
    /* 0x3D8DE */ s16               bossDamageByPlayer;  // boss HP taken by the player's attacks
    /* 0x3D8E0 */ s32               unk3D8E0;
    /* 0x3D8E4 */ BattleCmdQueue*   cmdQueue;
    /* 0x3D8E8 */ BattleCmdDeck*    cmdDeck;
    /* 0x3D8EC */ u8                auraPass;      // finisher's aura pass: 1 = to the partner, 2 = to the player
    /* 0x3D8ED */ s8                auraPasses;
    /* 0x3D8EE */ s16               auraRate;      // damage multiplier of the aura, percent
    /* 0x3D8F0 */ s32               auraOnPartner; // the partner holds the aura
    /* 0x3D8F4 */ s32               auraOnPlayer;  // the player holds the aura
    /* 0x3D8F8 */ s16               comboCount;
    /* 0x3D8FA */ s16               techBonus;
    /* 0x3D8FC */ s8                popupCountMain; // live message popups, main screen
    /* 0x3D8FD */ s8                popupCountSub;  // live message popups, sub screen
    /* 0x3D8FE */ u8                unk3D8FE;
    /* 0x3D8FF */ u8                unk3D8FF;
    /* 0x3D900 */ u16               unk3D900;
    /* 0x3D902 */ u16               unk3D902;
    /* 0x3D904 */ s16               hitStopMain;     // frames the main screen tasks stay frozen
    /* 0x3D906 */ s16               hitStopMainNext; // hit stop to apply to the main screen next frame
    /* 0x3D908 */ s16               hitStopSub;      // frames the sub screen tasks stay frozen
    /* 0x3D90A */ s16               hitStopSubNext;  // hit stop to apply to the sub screen next frame
    /* 0x3D90C */ s32               unk3D90C;
} Battle;                                            // Size: 0x3D910

/// The battle's graphics packs.
extern const BinIdentifier Iden_BtlPly00;   // the player
extern const BinIdentifier Iden_BtlPly01;   // Shiki
extern const BinIdentifier Iden_BtlPly02;   // Joshua
extern const BinIdentifier Iden_BtlPly03;   // Beat
extern const BinIdentifier Iden_BtlEffect;  // shared effects
extern const BinIdentifier Iden_BtlMenu;    // hit sparks, gauges and other battle interface sprites
extern const BinIdentifier Iden_BtlCmd;     // the partner card game
extern const BinIdentifier Iden_BtlBadge;   // badges (pins)
extern const BinIdentifier Iden_BtlJingle;  // the ready / victory / defeat banners
extern const BinIdentifier Iden_BtlObs;     // field objects
extern const BinIdentifier Iden_BtlSpecial; // special-attack cut-ins

/**
 * @brief Adds to the battle's par time and to `SaveData.unk_318E` (capped at 255)
 *
 * @param time Par time to add
 * @param amount Amount to add to `unk_318E`
 */
void func_ov003_0208af9c(s32 time, s32 amount);

/**
 * @brief Adds an item to the battle's drops, stacking it with an earlier drop of the same item
 *
 * @param itemId The item dropped
 * @return TRUE if it was added, FALSE if the drop list is full or the stack holds 99 already
 */
BOOL BtlMgr_AddDrop(u32 itemId);

/**
 * @brief Changes the battle state
 *
 * The ending states (`BATTLE_STATE_WIN` to `BATTLE_STATE_SCRIPTED_END`) are only entered from `BATTLE_STATE_FIGHT`,
 * and `BATTLE_STATE_RESULTS_DONE` only from `BATTLE_STATE_WIN` or `BATTLE_STATE_LOSE`.
 *
 * @param state The new state (BattleState)
 */
void BtlMgr_SetState(u32 state);

/**
 * @brief Moves a position along an angle
 *
 * @param x Position to move, x adds sin(angle) * scale
 * @param y Position to move, y adds cos(angle) * scale
 * @param angle Direction, 0x10000 = a full turn
 * @param scale Distance, 20.12 fixed point
 */
void BtlMgr_MoveByAngle(s32* x, s32* y, s32 angle, s32 scale);

/**
 * @brief Projects a field position onto the main screen
 *
 * @param screenX Receives the screen x position, pixels
 * @param screenY Receives the screen y position, pixels
 * @param x Field x position, 20.12 fixed point
 * @param y Field y position, 20.12 fixed point
 * @param z Height, 20.12 fixed point
 */
void BtlMgr_WorldToScreen(s16* screenX, s16* screenY, s32 x, s32 y, s32 z);

/**
 * @brief Whether a battle task should skip a stage because the battle is frozen
 *
 * @param stage The TaskStages index being run
 * @return TRUE for the update and render stages while battle flag 0x2 or 0x4 freezes the battle
 */
BOOL BtlMgr_IsStageFrozen(u16 stage);

/**
 * @brief Finds the sub-screen enemy farthest from a position on one side of the partner's field
 *
 * @param x Field x position to measure from, 20.12 fixed point
 * @param side 0 = right of the sub screen's centre, otherwise left of it
 * @param airborne TRUE to look only at enemies at least 64 pixels up, FALSE only at the others
 * @return The enemy, or NULL if there is none
 */
Ov003EnemyActor* BtlMgr_FindFarthestTarget(s32 x, s32 side, s32 airborne);

/**
 * @brief Awards experience to the team, raised by ability 0x1C, unless ability 0x1F blocks it
 *
 * @param side The side whose abilities apply: 0 = the player, 1 = the partner
 * @param exp Experience to award
 */
void BtlMgr_GainExp(s32 side, s16 exp);

/**
 * @brief Adds PP earned this battle, unless ability 0x1E blocks it
 *
 * @param side The side that earned it: 0 = the player, 1 = the partner
 * @param pp PP to add
 */
void BtlMgr_GainPP(s32 side, s16 pp);

/**
 * @brief Whether an ability is active for a side
 *
 * @param side 0 = the player, 1 = the partner
 * @param effect The ability
 * @return TRUE if it is active
 */
BOOL BtlMgr_HasAbility(s32 side, u32 effect);

/**
 * @brief Returns an ability's value for a side
 *
 * @param side 0 = the player, 1 = the partner
 * @param effect The ability
 * @return The ability's value
 */
s16 BtlMgr_GetAbilityValue(s32 side, u32 effect);

/**
 * @brief Returns a character's attack stat
 *
 * @param side 0 = the player, 1 = the partner (adding the partner's Attack Boost stickers)
 * @return The attack stat
 */
s32 BtlMgr_GetAttack(s32 side);

/**
 * @brief Returns a character's defense stat
 *
 * @param side 0 = the player, 1 = the partner (adding the partner's Defense Boost stickers)
 * @return The defense stat
 */
s32 BtlMgr_GetDefense(s32 side);

/**
 * @brief Records damage dealt to an enemy for the battle's bonuses: which side dealt damage, and the highest hit
 *
 * @param side 0 = the player, 1 = the partner
 * @param damage Damage dealt
 */
void BtlMgr_RecordDamageDealt(s32 side, s32 damage);

/**
 * @brief Counts a hit the team took, for the battle's no-damage bonus
 */
void BtlMgr_CountTeamHit(void);

/**
 * @brief Counts an aura pass in the current chain, and the battle's longest chain
 */
void BtlMgr_CountAuraPass(void);

/**
 * @brief Adds experience, levelling the team up (full HP, level-up animation) when it reaches the next level
 *
 * Nothing is added at level 100.
 *
 * @param exp Experience to add
 */
void BtlMgr_AddExp(s32 exp);

/**
 * @brief Heals or damages the team, keeping the damage each side took in step
 *
 * Does nothing in retried battles.
 *
 * @param side The side hit or healed: 0 = the player, 1 = the partner
 * @param delta HP to add; negative to damage
 */
void BtlMgr_AddTeamHp(s32 side, s16 delta);

/**
 * @brief Heals or damages an enemy; a boss part with ACTOR_FLAG_BOSS_GAUGE_HP uses the shared boss HP instead
 *
 * @param side The attacking side: 0 = the player, 1 = the partner
 * @param actor The enemy
 * @param delta HP to add; negative to damage
 */
void BtlMgr_AddUnitHp(s32 side, BattleActor* actor, s16 delta);

/**
 * @brief Whether an enemy can be targeted and is on one side of the partner's field
 *
 * @param unit The enemy
 * @param side 0 = right of the sub screen's centre, otherwise left of it
 * @return TRUE if it is not defeated or intangible and is on that side
 */
BOOL BtlMgr_IsTargetOnSide(Ov003EnemyActor* unit, s32 side);

/**
 * @brief Finds the sub-screen enemy nearest a position on one side of the partner's field, within 256 pixels
 *
 * @param x Field x position to measure from, 20.12 fixed point
 * @param side 0 = right of the sub screen's centre, otherwise left of it
 * @param airborne TRUE to look only at enemies at least 64 pixels up, FALSE only at the others
 * @return The enemy, or NULL if there is none
 */
Ov003EnemyActor* BtlMgr_FindNearestTarget(s32 x, s32 side, s32 airborne);

/**
 * @brief The battle overlay's process callback (Init, Main, Exit)
 *
 * @param work The battle's work area, NULL on the first call
 */
void ProcessOverlay_Battle(BattleMgrWork* work);

#endif // TWEWY_BATTLE_CORE_BTLMGR_H
