#include "Battle/Core/BtlSe.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "SndMgr.h"

/// Plays an SE unless the battle is muted.
void BtlSe_Play(SndMgrSeIdx seIdx) {
    if (g_CombatGlobal->unk3D878 & 0x20) {
        return;
    }
    SndMgr_StartPlayingSE(seIdx);
}

/// Plays an SE at a pan position unless the battle is muted.
void BtlSe_PlayPanned(SndMgrSeIdx seIdx, s32 sePan) {
    if (g_CombatGlobal->unk3D878 & 0x20) {
        return;
    }
    SndMgr_PlaySEWithPan(seIdx, sePan);
}

void BtlSe_PlayAtMainX(SndMgrSeIdx seIdx, s32 pan) {
    if (g_CombatGlobal->unk3D878 & 0x20) {
        return;
    }
    SndMgr_PlaySEWithPan(seIdx, BtlStage_ToScreenX(0, pan));
}

void BtlSe_PlayAtSubX(SndMgrSeIdx seIdx, s32 pan) {
    if (g_CombatGlobal->unk3D878 & 0x20) {
        return;
    }
    SndMgr_PlaySEWithPan(seIdx, BtlStage_ToScreenX(1, pan));
}
