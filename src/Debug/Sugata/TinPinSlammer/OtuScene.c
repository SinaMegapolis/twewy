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

    scene->state.unk_ADC = 0;
    scene->state.unk_AE0 = 0;
    scene->state.unk_AF4 = 1;
    scene->state.unk_698 = 1;

    DatMgr_ReleaseData(DatMgr_LoadRawDataWithOffset(1, (u8*)scene + 0x41EF0, 0x215C, &data_ov039_0209a10c, 0));
    DatMgr_ReleaseData(
        DatMgr_LoadRawDataWithOffset(1, (u8*)scene + 0x84 + TIN_PIN_SLAMMER_TABLE_OFFSET, 0x8F0, &data_ov039_0209a11c, 0));
    DatMgr_ReleaseData(DatMgr_LoadRawDataWithOffset(1, (u8*)scene + 0x44974, 0xEE, &data_ov039_0209a124, 0));

    u8* row = (u8*)scene;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 7; j++) {
            ((u16*)(row + TIN_PIN_SLAMMER_TABLE_OFFSET + 0x4C))[j] = 0x130;
        }
        row += 0xE;
    }

    scene->state.unk_69C = EasyTask_CreateTask(OTU_POOL1(scene), &Task_EasyFade, NULL, 0, NULL, NULL);
    EasyFade_FadeBothDisplays(2, 0x10, 0x1000);
    func_ov039_02098a20(OTU_STAGE(scene), (u8*)scene + TIN_PIN_SLAMMER_HEAP_OFFSET + 0x18C);
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
    scene->state.unk_AF0 = 0;
    scene->state.unk_EE0 = 4;
}

/**
 * @brief Selects the alternate-Otosu board layout.
 *
 * The counterpart to func_ov039_02082724: same two fields, opposite values.
 *
 * @param scene Scene object.
 */
void func_ov039_0208273c(TinPinSlammer_Scene* scene) {
    scene->state.unk_AF0 = 1;
    scene->state.unk_EE0 = 0;
}

/**
 * @brief Tears down the fade task the scene created on entry.
 *
 * @param scene Scene object.
 */
void func_ov039_02082754(TinPinSlammer_Scene* scene) {
    EasyTask_DeleteTask((TaskPool*)((u8*)scene + TIN_PIN_SLAMMER_POOL1_OFFSET), scene->state.unk_69C);
}

// Nonmatching: mwcc hoists the shared stage-container address into a
// callee-saved register; the target re-loads the 0x41AC4 literal at each of
// the four uses and keeps `scene` in r4 instead.
void func_ov039_02082774(TinPinSlammer_Scene* scene) {
    func_ov039_02098a60((u8*)scene + TIN_PIN_SLAMMER_STAGE_OFFSET, scene);
    func_ov039_02098acc((u8*)scene + TIN_PIN_SLAMMER_STAGE_OFFSET, scene);
    func_ov039_02098af4((u8*)scene + TIN_PIN_SLAMMER_STAGE_OFFSET, scene);

    if (func_ov039_02098b44((u8*)scene + TIN_PIN_SLAMMER_STAGE_OFFSET) != 0) {
        scene->state.unk_ADC = 1;
    }
}

// Nonmatching: 12 bytes short. The target re-loads the shared 0x41AC4
// stage-container literal at each of its nine uses, where mwcc hoists it into a
// callee-saved register once and keeps `scene` in r5. Every offset, call order
// and branch shape is otherwise identical. Eight sweep variants (stage address
// spelled four ways, scene pointer typed or void) all land at 45 differing
// lines, and a `volatile` copy of the offset makes it worse (four stack
// round-trips instead of three register loads, +16 bytes). A permuter pass with
// a call-count guard found nothing further, so the remaining gap is the hoist
// itself.
void func_ov039_020827d0(TinPinSlammer_Scene* scene) {
    // decomp-permuter's finding: cache the scene as a byte pointer and add
    // the stage offset to that, so mwcc keeps one base in a register instead
    // of re-deriving a typed pointer at each of the nine uses below.
    u8* base = (u8*)scene;
    func_ov039_02098a60(((OtuStageDispatch*)(base + TIN_PIN_SLAMMER_STAGE_OFFSET)), scene);
    func_ov039_02098acc(((OtuStageDispatch*)(base + TIN_PIN_SLAMMER_STAGE_OFFSET)), scene);

    if (func_ov039_02098b68(((OtuStageDispatch*)(base + TIN_PIN_SLAMMER_STAGE_OFFSET))) != &OtuScene_WirelessStage &&
        func_ov039_02098b44(((OtuStageDispatch*)(base + TIN_PIN_SLAMMER_STAGE_OFFSET))) == 0)
    {
        // A switch, not an `||` chain: mwcc folds `x==8 || x==9 || x==10` into
        // a `(x-8) <= 2` range check, while the target emits all three compares.
        switch (func_ov040_0209cb78()) {
            case 8:
            case 9:
            case 10: {
                if (scene->state.unk_EEC != 0) {
                    func_ov040_0209ed30();
                    func_ov040_0209ece4();
                    func_ov040_0209ed20();
                    func_02047338();
                    func_ov040_0209d970(0);
                    func_ov003_0209d434(0, 0);
                    scene->state.unk_EEC = 0;
                }
                scene->state.unk_AE0 = 1;
                func_ov040_0209d990();
                func_ov039_02098a40(((OtuStageDispatch*)(base + TIN_PIN_SLAMMER_STAGE_OFFSET)), &OtuScene_WirelessStage);
            } break;
        }

        if (SystemStatusFlags.reset != 0) {
            func_ov040_0209d990();
            func_ov039_02098a40(((OtuStageDispatch*)(base + TIN_PIN_SLAMMER_STAGE_OFFSET)), &OtuScene_WirelessStage);
        }
    }

    if (scene->state.unk_EE4 != 0) {
        scene->state.unk_AE0 = 1;
        func_ov040_0209d990();
        func_ov039_02098a40(((OtuStageDispatch*)(base + TIN_PIN_SLAMMER_STAGE_OFFSET)), &OtuScene_WirelessStage);
        scene->state.unk_EE4 = 0;
    }

    if (scene->state.unk_EE8 != 0) {
        scene->state.unk_AE0 = 1;
        func_ov040_0209d990();
        func_ov039_02098a40(((OtuStageDispatch*)(base + TIN_PIN_SLAMMER_STAGE_OFFSET)), &OtuScene_WirelessStage);
        scene->state.unk_EE8 = 0;
    }

    func_ov039_02098af4(((OtuStageDispatch*)(base + TIN_PIN_SLAMMER_STAGE_OFFSET)), scene);

    if (func_ov039_02098b44(((OtuStageDispatch*)(base + TIN_PIN_SLAMMER_STAGE_OFFSET))) != 0) {
        scene->state.unk_ADC = 1;
    }
}
