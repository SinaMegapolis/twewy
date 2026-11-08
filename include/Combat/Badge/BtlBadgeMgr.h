#ifndef TWEWY_COMBAT_BADGE_BTLBADGEMGR_H
#define TWEWY_COMBAT_BADGE_BTLBADGEMGR_H

#include "Combat/Core/Combat.h"

/// Drops every badge of the battle.
void BtlBadgeMgr_Clear(void);
/// Publishes the actor positions to the 3D layer's slot table.
void BtlBadgeMgr_PublishSlots(void);
/// Queues one queued ability for every equipped pin of the current deck (`arg0` selects the deck
/// of the active player).
void BtlBadgeMgr_QueueAbilities(s32 arg0);
/// Spawns the touch icon task and queues one action per equipped pin of the current deck.
void BtlBadgeMgr_SpawnBadges(s32 arg0);
/// TRUE while at least one non-player actor is alive.
s32  BtlBadgeMgr_AnyActive(void);
void BtlBadgeMgr_ClearDrag(void);
/// Picks the actor under the touch/stylus and moves it there; SELECT resets the positions.
void BtlBadgeMgr_DebugDrag(void);

#endif // TWEWY_COMBAT_BADGE_BTLBADGEMGR_H
