#include "Debug/Sugata/TinPinSlammer.h"
#include "Save.h"

/**
 * @file OtuScene.c
 * @brief Top-level scene state setup for Tin Pin Slammer (ov039).
 *
 * These are the functions MainOvlDisp calls to populate the Otosu scene object
 * from the equipped-pins save data and to tear the scene back down.
 */

/**
 * @brief Copies the equipped-pin ids from the save data into the scene's pin tray.
 *
 * Walks the six equipped-pin records in the save data (stride 0xA, id at +0x74),
 * storing each id into the next free tray slot and stopping the count at the
 * 0xFFFF "no pin" terminator, so the tray is packed to the front.  Tray slots
 * the deck did not fill are padded with the empty-pin type 0x130, and if slot 0
 * ended up empty it is forced to 0xE5 so the player always starts holding a
 * real pin.
 *
 * @param scene Scene object, viewed as a flat `u16` array; the pin tray starts
 *              at slot TIN_PIN_SLAMMER_TRAY_SLOT.
 */
void func_ov039_020824a0(u16* scene) {
    s32 i;
    s32 count = 0;
    u8* entry = (u8*)&gSaveData;

    for (i = 0; i < 6; i++) {
        u16 pinID = *(u16*)(entry + TIN_PIN_SLAMMER_PIN_ID_OFFSET);

        scene[TIN_PIN_SLAMMER_TRAY_SLOT + count] = pinID;
        if (pinID != 0xFFFF) {
            count++;
        }
        entry += TIN_PIN_SLAMMER_PIN_STRIDE;
    }

    while (count < 7) {
        scene[TIN_PIN_SLAMMER_TRAY_SLOT + count] = 0x130;
        count++;
    }

    if (scene[TIN_PIN_SLAMMER_TRAY_SLOT] == 0x130) {
        scene[TIN_PIN_SLAMMER_TRAY_SLOT] = 0xE5;
    }
}

/**
 * @brief Builds the scene's background resource, once.
 *
 * Packs a resource-handler argument block on the stack and hands it to the
 * shared resource-handler create function, then marks the scene's
 * `unk_6A4` flag so the work only happens on the first call.
 *
 * @param scene Scene object.
 */
void func_ov039_02082520(TinPinSlammer_Scene* scene) {
    TinPinSlammer_ResArgs args;

    if (scene->state.unk_6A4 != 0) {
        return;
    }

    args.unk_00   = 1;
    args.unk_04   = 0;
    args.dataType = *(s32*)((u8*)scene + TIN_PIN_SLAMMER_BASE_OFFSET + 0x588);
    args.unk_0C   = &data_0205c9b0;
    args.unk_10   = 0;
    args.unk_14   = 0;
    args.unk_18   = 0;
    args.unk_1A   = 0;
    args.unk_1C   = 0x20;
    args.unk_1E   = 0x18;

    func_02025b68((u8*)scene + TIN_PIN_SLAMMER_RES_OFFSET, &args);
    scene->state.unk_6A4 = 1;
}

/**
 * @brief Releases the scene's background resource, if one was built.
 *
 * @param scene Scene object.
 */
void func_ov039_020825b0(TinPinSlammer_Scene* scene) {
    if (scene->state.unk_6A4 == 0) {
        return;
    }

    func_02025e30((u8*)scene + TIN_PIN_SLAMMER_RES_OFFSET);
    scene->state.unk_6A4 = 0;
}
