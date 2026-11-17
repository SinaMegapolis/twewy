#ifndef TWEWY_BATTLE_CORE_BTLOBSMGR_H
#define TWEWY_BATTLE_CORE_BTLOBSMGR_H

#include "Battle/Core/BtlActor.h"
#include "Battle/Core/BtlCollision.h"
#include "Battle/Core/BtlList.h"
#include "Battle/Core/BtlMgr.h"
#include "Battle/Core/BtlSprite.h"

/// What BtlObsMgr_SpawnTask hands the field-object task it creates.
typedef struct BattleObsSpawn {
    /* 0x00 */ u16 kind;     // the config's task kind
    /* 0x02 */ u16 configId; // index into the field-object configs
    /* 0x04 */ s32 x;        // field position, 20.12 fixed point
    /* 0x08 */ s32 y;
} BattleObsSpawn;            // Size: 0xC

/// A field object's collision body.
typedef struct BattleObsBody {
    /* 0x00 */ BtlCollider collider;
    /* 0x34 */ u16         flags;
    /* 0x36 */ u16         unk_36;
} BattleObsBody; // Size: 0x38

/// A field object (bin, sign, bus, ...) that can be picked up, thrown and broken; kept in `Battle.obsList`.
typedef struct BattleObsActor {
    /* 0x000 */ u16            kind;     // the config's task kind
    /* 0x002 */ u16            configId; // index into the field-object configs
    /* 0x004 */ BattleActor    actor;
    /* 0x080 */ BattleObsBody  body;
    /* 0x0B8 */ BattleListNode node;
} BattleObsActor; // Size: 0xC4

/// The current battle's shared state, allocated by `BtlMgr`.
extern Battle* gBattle;

/**
 * @brief Spawns a field object at a random point of the main screen's field
 *
 * @param configId The field-object config to spawn
 */
void BtlObsMgr_DropAtRandom(u16 configId);

/**
 * @brief Spawns several field objects of a group at random points of the main screen's field
 *
 * Groups 0, 1, 3, 4 and 5 pick one of their configs at random for the whole drop.
 *
 * @param group The field-object group, 0..13 (BattleObsSpawnCmd.kind)
 * @param count How many objects to spawn
 */
void BtlObsMgr_DropGroup(s32 group, s32 count);

/**
 * @brief Loads a field object's sprite
 *
 * @param engine The screen to load it for (DISPLAY_MAIN or DISPLAY_SUB)
 * @param sprite The sprite to load into
 * @param configId The field-object config, 0..23
 * @return The BtlSprite_Load result, or 0 if `configId` is out of range
 */
s32 BtlObsMgr_LoadSprite(DisplayEngine engine, BattleSprite* sprite, u32 configId);

/**
 * @brief Loads the hit spark a field object shows when it is hit
 *
 * @param engine The screen to load it for (DISPLAY_MAIN or DISPLAY_SUB)
 * @param sprite The sprite to load into
 * @param configId The field-object config
 */
void BtlObsMgr_LoadHitEffect(DisplayEngine engine, BattleSprite* sprite, s32 configId);

/**
 * @brief Sets up a battle actor as a field object: hit box, physics and HP from its config
 *
 * @param actor The actor to set up
 * @param configId The field-object config
 */
void BtlObsMgr_InitActor(BattleActor* actor, u16 configId);

/**
 * @brief Plays a field object's landing sound, panned to its position
 *
 * @param engine The screen the object is on (DISPLAY_MAIN or DISPLAY_SUB)
 * @param configId The field-object config
 * @param x World x position to pan to, 20.12 fixed point
 */
void BtlObsMgr_PlaySe(DisplayEngine engine, s32 configId, s32 x);

/**
 * @brief Whether the sub-screen field objects should be drawn
 *
 * @return FALSE while battle flag 0x4 hides them
 */
BOOL func_ov003_0208656c(void);

/**
 * @brief Spawns a field object
 *
 * @param configId The field-object config
 * @param x Field position, 20.12 fixed point
 * @param y Field position, 20.12 fixed point
 */
void BtlObsMgr_Spawn(u16 configId, s32 x, s32 y);

/**
 * @brief Spawns a field object's task
 *
 * @param configId The field-object config
 * @param x Field position, 20.12 fixed point
 * @param y Field position, 20.12 fixed point
 * @param taskId Receives the created task's id
 * @return The last object in the field-object list, i.e. the new one if its task set itself up
 */
BattleObsActor* BtlObsMgr_SpawnTask(u16 configId, s32 x, s32 y, s32* taskId);

/**
 * @brief Breaks a field object
 *
 * @param obs The object to break
 */
void BtlObsMgr_Defeat(BattleObsActor* obs);

/**
 * @brief Sets up a newly created field object from its spawn request and adds it to the field-object list
 *
 * @param obs The object to set up
 * @param spawn The spawn request its task was created with
 */
void BtlObsMgr_Setup(BattleObsActor* obs, BattleObsSpawn* spawn);

/**
 * @brief Moves a field object, keeping it inside the field unless it is held, and updates its collider
 *
 * @param obs The object to update
 */
void BtlObsMgr_Update(BattleObsActor* obs);

/**
 * @brief Removes a field object's collider and takes it off the field-object list
 *
 * @param obs The object to remove
 */
void BtlObsMgr_Remove(BattleObsActor* obs);

/**
 * @brief Empties the field-object list
 */
void BtlObsMgr_ResetList(void);

/**
 * @brief Rolls the encounter's field-object drop and spawns the objects of the list picked for the main screen's field
 *
 * Nothing drops while ability 5 is active; encounters 7 and 8 always drop object 24.
 */
void BtlObsMgr_RollDrop(void);

#endif // TWEWY_BATTLE_CORE_BTLOBSMGR_H
