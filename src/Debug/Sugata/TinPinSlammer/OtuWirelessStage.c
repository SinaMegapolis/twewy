/**
 * @file OtuWirelessStage.c
 * @brief The wireless stage (OtuScene_WirelessStage), which reacts to the
 *        wireless link's state, and helpers the board stages and the tasks
 *        share: the scale animation, the deck copy and positional sound.
 */

#include "Engine/Core/System.h"
#include "Engine/Text.h"
#include "OtuFieldAccessShared.h"

/** Empty stage steps. */
void func_ov039_02087ac4(void) {}

void func_ov039_02087ac8(void) {}

/** Fades the scene in, and moves the stage on once the fade has finished. */
void func_ov039_02087acc(TinPinSlammer_Scene* scene) {
    EasyFade_FadeBothDisplays(0, 0x10, 0x1000);

    if (EasyFade_IsFading()) {
        return;
    }

    func_ov039_02098a50(OTU_STAGE(scene));
}

/** Reacts to the wireless link's state. */
void func_ov039_02087b04(TinPinSlammer_Scene* scene) {
    switch (func_ov040_0209cb78()) {
        case 0:
        case 10:
            func_ov039_02098a50(OTU_STAGE(scene));
            break;
        case 1:
            func_ov040_0209d6cc();
            break;
        case 8:
        case 9:
            func_ov040_0209d540();
            break;
        default:
            func_ov040_0209d588();
            break;
    }
}

/** Moves the stage on and queues no next stage, which ends the scene. */
void func_ov039_02087b74(TinPinSlammer_Scene* scene) {
    func_ov039_02098a50(OTU_STAGE(scene));
    func_ov039_02098a40(OTU_STAGE(scene), 0);
}

/** Starts `anim` at its first key and publishes that key's scales. */
void func_ov039_02087ba0(OtuScaleAnim* anim, const OtuScaleKey* keys, u16 count, OamAffineParam* affine) {
    anim->keys  = keys;
    anim->index = 0;
    anim->count = count;
    anim->hold  = anim->keys[anim->index].frames;

    affine->scaleX = anim->keys[anim->index].scaleX;
    affine->scaleY = anim->keys[anim->index].scaleY;
}

/**
 * Publishes the current key's scales, then counts its hold down and moves to
 * the next key, wrapping at `count`.
 */
void func_ov039_02087bf8(OtuScaleAnim* anim, OamAffineParam* affine) {
    affine->scaleX = anim->keys[anim->index].scaleX;
    affine->scaleY = anim->keys[anim->index].scaleY;

    if (anim->index >= anim->count - 1) {
        return;
    }

    anim->hold--;

    if (anim->hold != 0) {
        return;
    }

    anim->index++;

    if ((u32)anim->index >= (u32)anim->count) {
        anim->index = 0;
    }

    anim->hold = anim->keys[anim->index].frames;
}

/* SJIS 8b 43 90 e2 8e 9e 8a d4 */

/** Sets the wireless stages' handshake step (OtuBoardStage.step). */
void func_ov039_02087c8c(TinPinSlammer_Scene* scene, s32 step) {
    ((OtuBoardStage*)func_ov039_02098b70(OTU_STAGE(scene)))->step = step;
}

/**
 * The link's receive callback: stores player `slot`'s deck and marks that
 * player ready.
 */
void func_ov039_02087cac(TinPinSlammer_Scene* scene, s32 slot, const void* src, s32 len) {
    OtuBoardStage* stage = (OtuBoardStage*)func_ov039_02098b70(OTU_STAGE(scene));

    MI_CpuCopyU8(src, scene->decks[slot], len);
    stage->readyMask |= 1 << slot;
}

/**
 * Plays sound effect `se`, panned by the x distance from `to` to `from` and
 * attenuated by their distance.
 */
void func_ov039_02087d04(s32 se, OtuPoint* from, OtuPoint* to) {
    OtuPoint anchor;
    s32      distance;
    s32      pan;
    s32      maxVolume;

    anchor.x = 0x80000;
    anchor.y = 0x60000;

    func_ov039_02098b8c(to, &anchor, &anchor);

    distance = func_ov039_02098ca8(from, &anchor) >> 0xC;
    distance = (distance + ((u32)(distance >> 2) >> 0x1D)) >> 3;

    pan = (from->x - to->x) >> 0xC;

    if (pan > 0xFF) {
        pan = 0xFF;
    }

    if (pan < 0) {
        pan = 0;
    }

    SndMgr_StartPlayingSE(se);
    maxVolume = SndMgr_GetSeIdxVolume(se);

    if (distance > maxVolume) {
        distance = maxVolume;
    }

    if (distance < 0) {
        distance = 0;
    }

    func_02027170(se, maxVolume - distance);
    SndMgr_UpdateSEPan(se, pan);
}

/** Clears `badge` as the partner of every other badge. */
void func_ov039_02087dc0(TinPinSlammer_Scene* scene, void* badge) {
    OtuBoardStage* stage = (OtuBoardStage*)func_ov039_02098b70(OTU_STAGE(scene));
    s32            i;

    for (i = 0; i < stage->badgeCount; i++) {
        void* child = EasyTask_GetTaskData(OTU_POOL2(scene), stage->badgeIds[i]);

        if (child != badge) {
            func_ov039_0208f0f0(child, badge);
        }
    }
}
