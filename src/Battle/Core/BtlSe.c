#include "Battle/Core/BtlSe.h"
#include "Battle/Core/BtlObsMgr.h"
#include "SndMgr.h"

void BtlSe_Play(SndMgrSeIdx seIdx) {
    if ((gBattle->flags & BATTLE_FLAG_SE_MUTED) == FALSE) {
        SndMgr_StartPlayingSE(seIdx);
    }
}

void BtlSe_PlayPanned(SndMgrSeIdx seIdx, s32 pan) {
    if ((gBattle->flags & BATTLE_FLAG_SE_MUTED) == FALSE) {
        SndMgr_PlaySEWithPan(seIdx, pan);
    }
}

void BtlSe_PlayAtMainX(SndMgrSeIdx seIdx, s32 x) {
    if ((gBattle->flags & BATTLE_FLAG_SE_MUTED) == FALSE) {
        SndMgr_PlaySEWithPan(seIdx, BtlStage_ToScreenX(DISPLAY_MAIN, x));
    }
}

void BtlSe_PlayAtSubX(SndMgrSeIdx seIdx, s32 x) {
    if ((gBattle->flags & BATTLE_FLAG_SE_MUTED) == FALSE) {
        SndMgr_PlaySEWithPan(seIdx, BtlStage_ToScreenX(DISPLAY_SUB, x));
    }
}
