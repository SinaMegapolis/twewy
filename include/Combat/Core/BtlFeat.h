#ifndef TWEWY_COMBAT_CORE_BTLFEAT_H
#define TWEWY_COMBAT_CORE_BTLFEAT_H

#include "Combat/Core/CombatActor.h"
#include "Combat/Core/CombatEnemyActor.h"
#include <nitro/types.h>

/// An enemy's actor state, saved before a Feat moves it and written back afterwards.
typedef struct Ov003ActorSnapshot {
    /* 0x00 */ Ov003EnemyActor* unit;
    /* 0x04 */ CombatActor      actor;
    /* 0x80 */ u8               reserved[0x1C];
} Ov003ActorSnapshot; // Size: 0x9C

typedef void (*Ov003SnapshotCallback)(s32, Ov003ActorSnapshot*, void*);

/// Starts the splash script of enemy sprites `start` onward, `count` of them, playing `sound`.
void func_ov003_020bc690(u16 start, u16 count, s32 sound);

/// Starts the splash background of `group`/`variant` on `screen` for `duration` frames.
void func_ov003_020bce18(s32 screen, u16 group, u16 variant, u16 duration);

void BtlFeat_SaveEnmPos(void);
void BtlFeat_RestoreEnmPos(void);
void BtlFeat_HaltEnm(void);
void BtlFeat_ForEachEnmSnapshot(s32 mode, Ov003SnapshotCallback callback, void* args);

#endif // TWEWY_COMBAT_CORE_BTLFEAT_H
