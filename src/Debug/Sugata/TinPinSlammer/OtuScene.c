#include "Debug/Sugata/TinPinSlammer.h"
#include "EasyFade.h"
#include "Engine/Core/System.h"
#include "Engine/EasyTask.h"
#include "Save.h"
#include "common_data.h"

extern const BinIdentifier data_ov039_0209a10c;
extern const BinIdentifier data_ov039_0209a11c;
extern const BinIdentifier data_ov039_0209a124;

/**
 * @file OtuScene.c
 * @brief Top-level scene state setup for Tin Pin Slammer (ov039).
 *
 * These are the functions MainOvlDisp calls to populate the Otosu scene object
 * from the equipped-pins save data and to tear the scene back down.
 */

/**
 * @brief Fills the local player's deck from the equipped pins.
 *
 * Packs the six equipped pins to the front of deck 0, pads the rest with
 * OTU_NO_PIN, and gives the player pin 0xE5 if nothing is equipped.
 */
void func_ov039_020824a0(TinPinSlammer_Scene* scene) {
    s32 i;
    s32 count = 0;

    for (i = 0; i < 6; i++) {
        u16 pinID = gSaveData.equippedPins[i].pinID;

        scene->decks[0][count] = pinID;
        if (pinID != 0xFFFF) {
            count++;
        }
    }

    while (count < 7) {
        scene->decks[0][count] = OTU_NO_PIN;
        count++;
    }

    if (scene->decks[0][0] == OTU_NO_PIN) {
        scene->decks[0][0] = 0xE5;
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

    if (scene->textReady != 0) {
        return;
    }

    args.unk_00   = 1;
    args.unk_04   = 0;
    args.dataType = scene->dataType;
    args.unk_0C   = &data_0205c9b0;
    args.unk_10   = 0;
    args.unk_14   = 0;
    args.unk_18   = 0;
    args.unk_1A   = 0;
    args.unk_1C   = 0x20;
    args.unk_1E   = 0x18;

    func_02025b68(scene->textRes, &args);
    scene->textReady = 1;
}

/**
 * @brief Releases the scene's background resource, if one was built.
 *
 * @param scene Scene object.
 */
void func_ov039_020825b0(TinPinSlammer_Scene* scene) {
    if (scene->textReady == 0) {
        return;
    }

    func_02025e30(scene->textRes);
    scene->textReady = 0;
}

/**
 * @brief Loads the three board data files and resets the scene for a fresh
 *        game.
 *
 * Loads "BeBadge_Parm.bin" (0x215C bytes) into the block that ends right at
 * the pin tray, "BeBadge_Single.bin" (0x8F0) and the badge AI script (0xEE),
 * releasing each `Data` handle immediately because the loads copy into the
 * scene buffer outright.  Fills the four 7-wide pin trays with the empty-pin
 * sentinel 0x130, starts the fade task from pool 1, and seeds the result
 * screen's stage container and its `unk_1148C` cursor.
 *
 * @param scene Scene object.
 */
void func_ov039_020825e8(TinPinSlammer_Scene* scene) {
    s32 j;
    s32 i;

    scene->done      = 0;
    scene->linkError = 0;
    scene->unk_41AF4 = 1;
    scene->playing   = 1;

    DatMgr_ReleaseData(
        DatMgr_LoadRawDataWithOffset(1, scene->badgeParams, sizeof(scene->badgeParams), &data_ov039_0209a10c, 0));
    DatMgr_ReleaseData(DatMgr_LoadRawDataWithOffset(1, scene->matches, sizeof(scene->matches), &data_ov039_0209a11c, 0));
    DatMgr_ReleaseData(DatMgr_LoadRawDataWithOffset(1, scene->ai, sizeof(scene->ai), &data_ov039_0209a124, 0));

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 7; j++) {
            scene->decks[i][j] = OTU_NO_PIN;
        }
    }

    scene->fadeTask = EasyTask_CreateTask(OTU_POOL1(scene), &Task_EasyFade, NULL, 0, NULL, NULL);
    EasyFade_FadeBothDisplays(2, 0x10, 0x1000);
    func_ov039_02098a20(OTU_STAGE(scene), OTU_HEAP(scene));
}

/**
 * @brief Selects the plain-Otosu board layout.
 *
 * Sets the scene's board-mode selector and its companion layout id.  Called
 * when the player picks the standard pin-throwing board.
 *
 * @param scene Scene object.
 */
void func_ov039_02082724(TinPinSlammer_Scene* scene) {
    scene->multiplayer = 0;
    scene->playerCount = 4;
}

/**
 * @brief Selects the alternate-Otosu board layout.
 *
 * The counterpart to func_ov039_02082724: same two fields, opposite values.
 *
 * @param scene Scene object.
 */
void func_ov039_0208273c(TinPinSlammer_Scene* scene) {
    scene->multiplayer = 1;
    scene->playerCount = 0;
}

/**
 * @brief Tears down the fade task the scene created on entry.
 *
 * @param scene Scene object.
 */
void func_ov039_02082754(TinPinSlammer_Scene* scene) {
    EasyTask_DeleteTask(OTU_POOL1(scene), scene->fadeTask);
}

void func_ov039_02082774(TinPinSlammer_Scene* scene) {
    func_ov039_02098a60(OTU_STAGE(scene), scene);
    func_ov039_02098acc(OTU_STAGE(scene), scene);
    func_ov039_02098af4(OTU_STAGE(scene), scene);

    if (func_ov039_02098b44(OTU_STAGE(scene)) != 0) {
        scene->done = 1;
    }
}

void func_ov039_020827d0(TinPinSlammer_Scene* scene) {
    // decomp-permuter's finding: cache the scene as a byte pointer and add
    // the stage offset to that, so mwcc keeps one base in a register instead
    // of re-deriving a typed pointer at each of the nine uses below.
    u8* base = (u8*)scene;
    s32 reset;
    func_ov039_02098a60(&scene->stage, scene);
    func_ov039_02098acc(&scene->stage, scene);

    if (func_ov039_02098b68(&scene->stage) != &OtuScene_WirelessStage && func_ov039_02098b44(&scene->stage) == 0) {
        // A switch, not an `||` chain: mwcc folds `x==8 || x==9 || x==10` into
        // a `(x-8) <= 2` range check, while the target emits all three compares.
        switch (func_ov040_0209cb78()) {
            case 8:
            case 9:
            case 10: {
                if (scene->linkOpen != 0) {
                    func_ov040_0209ed30();
                    func_ov040_0209ece4();
                    func_ov040_0209ed20();
                    func_02047338();
                    func_ov040_0209d970(0);
                    func_ov003_0209d434(0, 0);
                    scene->linkOpen = 0;
                }
                scene->linkError = 1;
                func_ov040_0209d990();
                func_ov039_02098a40(&scene->stage, &OtuScene_WirelessStage);
            } break;
        }

        // A named temp, not `if (SystemStatusFlags.reset != 0)`: the target
        // materialises the flag in a register first. (Permuter finding; objdiff 100%.)
        reset = (SystemStatusFlags.reset != 0);
        if (reset) {
            func_ov040_0209d990();
            func_ov039_02098a40(&scene->stage, &OtuScene_WirelessStage);
        }
    }

    if (scene->linkLost != 0) {
        scene->linkError = 1;
        func_ov040_0209d990();
        func_ov039_02098a40(&scene->stage, &OtuScene_WirelessStage);
        scene->linkLost = 0;
    }

    if (scene->linkTimeout != 0) {
        scene->linkError = 1;
        func_ov040_0209d990();
        func_ov039_02098a40(&scene->stage, &OtuScene_WirelessStage);
        scene->linkTimeout = 0;
    }

    func_ov039_02098af4(&scene->stage, scene);

    if (func_ov039_02098b44(&scene->stage) != 0) {
        scene->done = 1;
    }
}
