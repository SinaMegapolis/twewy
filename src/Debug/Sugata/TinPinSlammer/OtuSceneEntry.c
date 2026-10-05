#include "Debug/Sugata/TinPinSlammer.h"
#include "Display.h"
#include "Engine/Core/Memory.h"
#include "Engine/Core/OamMgr.h"
#include "Engine/Core/System.h"
#include "Engine/EasyTask.h"
#include "Engine/IO/TouchInput.h"
#include "Engine/Overlay/OverlayDispatcher.h"
#include "Engine/Resources/PaletteMgr.h"
#include "Engine/Resources/ResourceMgr.h"
#include "OtosuMenu.h"
#include "Save.h"

/**
 * @brief The wireless entry point.
 *
 * The same opening as the other two entry points, then: latch and clear the
 * two `SystemStatusFlags` bits (func_ov039_020831d8 restores them), take the
 * board and player count from the save, swap the two text-block templates, and
 * enter the wireless menu -- or, when the save holds a game key, rejoin that
 * game's parent through the result stage.
 */
// Nonmatching: 91.7%. The target loads both task-pool offsets (0x41598,
// 0x41618) from the literal pool and adds the scene base; mwcc folds each into
// an immediate add, which shifts the pool and the registers after it.
void func_ov039_02082c50(void) {
    const char*          sequenceName = *((const char* const*)OtuScene_SequenceNames + 1);
    TinPinSlammer_Scene* scene        = Mem_AllocHeapTail(&gDebugHeap, sizeof(TinPinSlammer_Scene));
    OtuPinTune           swap;

    Mem_SetSequence(&gDebugHeap, scene, sequenceName);

    MainOvlDisp_SetCbArg(scene);
    Mem_InitializeHeap(OTU_HEAP(scene), OTU_HEAP_BUFFER(scene), 0x30000);

    scene->spareDataType = DatMgr_AllocateSlot();
    scene->dataType      = DatMgr_AllocateSlot();

    OtuScene_SlotState.count = 0;
    OtuScene_SlotState.flag  = 0;
    scene->linkLost          = 0;
    scene->linkTimeout       = 0;
    scene->linkOpen          = 0;

    func_ov039_02083944();
    scene->prevResMgr = ResourceMgr_ReinitManagers(&scene->resMgr);
    EasyTask_InitializePool(OTU_POOL1(scene), OTU_HEAP(scene), 8, NULL, NULL);
    EasyTask_InitializePool(OTU_POOL2(scene), OTU_HEAP(scene), 0x180, NULL, NULL);
    func_0200d8f0();

    data_02066aec                            = 0;
    data_02066eec                            = 0;
    g_DisplaySettings.controls[0].brightness = 0x10;
    g_DisplaySettings.controls[1].brightness = 0x10;

    func_ov039_020825e8(scene);
    func_ov039_0208273c(scene);

    scene->linkStatus        = SystemStatusFlags.unk_06;
    scene->wirelessStatus    = SystemStatusFlags.unk_07 != 0;
    SystemStatusFlags.unk_06 = 0;
    SystemStatusFlags.unk_07 = 0;

    func_ov039_020824a0(scene);

    scene->boardIndex  = gSaveData.otosuBoard;
    scene->playerCount = gSaveData.otosuPlayerCount;

    swap                = data_ov039_0209a47c;
    data_ov039_0209a47c = data_ov039_0209a48c;
    data_ov039_0209a48c = swap;

    if (gSaveData.otosuGameKey == 0) {
        func_ov039_02098a40(OTU_STAGE(scene), &OtuScene_MenuStage);
    } else {
        MI_CpuCopyU8(gSaveData.otosuParentBssid, scene->parentBssid, sizeof(scene->parentBssid));
        func_ov039_02098a40(OTU_STAGE(scene), &OtuScene_ResultStage);
    }

    MainOvlDisp_NextProcessStage();
}

/**
 * @file OtuSceneEntry.c
 * @brief Scene entry points for Tin Pin Slammer (ov039).
 *
 * Each entry point allocates the 0x44A6C-byte scene block out of the debug
 * heap, brings up the resource managers, two task pools and the display, runs
 * the shared scene setup, then hands control to the first stage.  The variants
 * differ in which board layout they select and which VBlank/display pair they
 * install.
 */

/**
 * @brief The scene's per-frame update.
 *
 * Drains the OAM and palette managers, runs both task pools, lets the board
 * stage tick, and — once the stage reports itself mid-setup — pushes one of two
 * result overlays depending on a byte in the save record.  Nothing calls this
 * from inside the overlay; the dispatcher reaches it through a function pointer,
 * so it has no incoming relocations.
 */
void func_ov039_02082e98(TinPinSlammer_Scene* scene) {
    OverlayTag menuTag;
    OverlayTag resultTag;

    TouchInput_Update();
    OamMgr_Reset3DState();
    OamMgr_Reset(&g_OamMgr[0], 0, 0);
    OamMgr_Reset(&g_OamMgr[1], 0, 0);
    OamMgr_SetAffineCount(&g_OamMgr[2], 0);
    OamMgr_ResetCommandQueues(&g_OamMgr[0]);
    OamMgr_ResetCommandQueues(&g_OamMgr[1]);

    EasyTask_ProcessPendingTasks(OTU_POOL1(scene));
    if (scene->playing != 0) {
        EasyTask_ProcessPendingTasks(OTU_POOL2(scene));
    }

    func_ov039_02082774(scene);

    EasyTask_UpdateActiveTasks(OTU_POOL1(scene));
    EasyTask_UpdateActiveTasks(OTU_POOL2(scene));

    if (scene->done != 0) {
        // 0x1F is the record's "finished" marker; anything else means the
        // wireless game is still in progress and the menu overlay is wanted.
        if (gSaveData.unk_341C != 0x1F) {
            MainOvlDisp_ReplaceTop(&menuTag, OTU_OVERLAY_ID, ProcessOverlay_OtosuMenu_SinglePlayerRanking, NULL, 0);
        } else {
            MainOvlDisp_ReplaceTop(&resultTag, OTU_OVERLAY_ID, ProcessOverlay_OtosuMenu_RoleSelection, NULL, 0);
        }
    }

    OamMgr_Swap3DBuffers();
    OamMgr_FlushCommands(&g_OamMgr[0]);
    OamMgr_FlushCommands(&g_OamMgr[1]);
    PaletteMgr_Flush(g_PaletteManagers[0], NULL);
    PaletteMgr_Flush(g_PaletteManagers[1], NULL);
    PaletteMgr_Flush(g_PaletteManagers[2], NULL);
    func_0200d90c();
}

/**
 * @brief The wireless scene's per-frame update.
 *
 * The twin of func_ov039_02082e98, with two differences. It does no per-frame
 * work while the overlay's slot state says a wireless game is in progress --
 * only the task updates run -- and it picks its exit overlay from the scene's
 * own latched link status rather than from the save record, because the record
 * has already been consumed by the time this runs.
 */

void func_ov039_02082ff8(TinPinSlammer_Scene* scene) {
    OverlayTag linkTag;
    OverlayTag resultTag;

    TouchInput_Update();
    OamMgr_Reset3DState();
    OamMgr_Reset(&g_OamMgr[0], 0, 0);
    OamMgr_Reset(&g_OamMgr[1], 0, 0);
    OamMgr_SetAffineCount(&g_OamMgr[2], 0);
    OamMgr_ResetCommandQueues(&g_OamMgr[0]);
    OamMgr_ResetCommandQueues(&g_OamMgr[1]);

    if (OtuScene_SlotState.count == 0) {
        EasyTask_ProcessPendingTasks(OTU_POOL1(scene));
        if (scene->playing != 0) {
            EasyTask_ProcessPendingTasks(OTU_POOL2(scene));
        }

        func_ov039_020827d0(scene);
    }

    EasyTask_UpdateActiveTasks(OTU_POOL1(scene));
    EasyTask_UpdateActiveTasks(OTU_POOL2(scene));

    if (scene->done != 0) {
        if (scene->linkError == 0) {
            MainOvlDisp_ReplaceTop(&linkTag, OTU_OVERLAY_ID, ProcessOverlay_OtosuMenu_MultiplayerRanking, NULL, 0);
        } else {
            MainOvlDisp_ReplaceTop(&resultTag, OTU_OVERLAY_ID, ProcessOverlay_OtosuMenu_ConnectionError, NULL, 0);
        }
    }

    OamMgr_Swap3DBuffers();
    OamMgr_FlushCommands(&g_OamMgr[0]);
    OamMgr_FlushCommands(&g_OamMgr[1]);
    PaletteMgr_Flush(g_PaletteManagers[0], NULL);
    PaletteMgr_Flush(g_PaletteManagers[1], NULL);
    PaletteMgr_Flush(g_PaletteManagers[2], NULL);
    func_0200d90c();
}

/**
 * @brief Scene entry point for the plain board.
 *
 * Selects the plain layout (func_ov039_02082724) and enters OtuScene_FirstStage.
 */
void func_ov039_02082978(void) {
    const char*          sequenceName = *((const char* const*)OtuScene_SequenceNames + 2);
    TinPinSlammer_Scene* scene        = Mem_AllocHeapTail(&gDebugHeap, sizeof(TinPinSlammer_Scene));

    Mem_SetSequence(&gDebugHeap, scene, sequenceName);
    MainOvlDisp_SetCbArg(scene);
    Mem_InitializeHeap(OTU_HEAP(scene), OTU_HEAP_BUFFER(scene), 0x30000);

    scene->spareDataType = DatMgr_AllocateSlot();
    scene->dataType      = DatMgr_AllocateSlot();

    OtuScene_SlotState.count = 0;
    OtuScene_SlotState.flag  = 0;
    scene->linkLost          = 0;
    scene->linkTimeout       = 0;
    scene->linkOpen          = 0;

    func_ov039_02083928();
    scene->prevResMgr = ResourceMgr_ReinitManagers(&scene->resMgr);
    EasyTask_InitializePool(OTU_POOL1(scene), OTU_HEAP(scene), 8, NULL, NULL);
    EasyTask_InitializePool(OTU_POOL2(scene), OTU_HEAP(scene), 0x180, NULL, NULL);
    func_0200d8f0();

    data_02066aec                            = 0;
    data_02066eec                            = 0;
    g_DisplaySettings.controls[0].brightness = 0x10;
    g_DisplaySettings.controls[1].brightness = 0x10;

    func_ov039_020825e8(scene);
    func_ov039_02082724(scene);

    scene->matchIndex = 0;

    func_ov039_020824a0(scene);
    func_ov039_02098a40(OTU_STAGE(scene), &OtuScene_FirstStage);
    MainOvlDisp_NextProcessStage();
}

/**
 * @brief Scene entry point for the wireless board.
 *
 * Same bring-up as the plain board, but it seeds the entry byte from the
 * wireless save record instead of clearing it, and enters
 * `OtuScene_WirelessBoard` -- the stage that draws the link-state rows.
 */
void func_ov039_02082ae0(void) {
    const char*          sequenceName = *((const char* const*)OtuScene_SequenceNames + 0);
    TinPinSlammer_Scene* scene        = Mem_AllocHeapTail(&gDebugHeap, sizeof(TinPinSlammer_Scene));

    Mem_SetSequence(&gDebugHeap, scene, sequenceName);

    MainOvlDisp_SetCbArg(scene);
    Mem_InitializeHeap(OTU_HEAP(scene), OTU_HEAP_BUFFER(scene), 0x30000);

    scene->spareDataType = DatMgr_AllocateSlot();
    scene->dataType      = DatMgr_AllocateSlot();

    OtuScene_SlotState.count = 0;
    OtuScene_SlotState.flag  = 0;
    scene->linkLost          = 0;
    scene->linkTimeout       = 0;
    scene->linkOpen          = 0;

    func_ov039_02083928();
    scene->prevResMgr = ResourceMgr_ReinitManagers(&scene->resMgr);
    EasyTask_InitializePool(OTU_POOL1(scene), OTU_HEAP(scene), 8, NULL, NULL);
    EasyTask_InitializePool(OTU_POOL2(scene), OTU_HEAP(scene), 0x180, NULL, NULL);
    func_0200d8f0();

    data_02066aec                            = 0;
    data_02066eec                            = 0;
    g_DisplaySettings.controls[0].brightness = 0x10;
    g_DisplaySettings.controls[1].brightness = 0x10;

    func_ov039_020825e8(scene);
    func_ov039_02082724(scene);

    scene->matchIndex = gSaveData.unk_341C;

    func_ov039_020824a0(scene);
    func_ov039_02098a40(OTU_STAGE(scene), &OtuScene_WirelessBoard);
    MainOvlDisp_NextProcessStage();
}

/**
 * @brief The scene's teardown.
 *
 * Runs the stage's exit routine, tears down both task pools, returns the
 * resource managers, releases the two data slots, and frees the scene block
 * back to the debug heap.
 */
void func_ov039_02083164(TinPinSlammer_Scene* scene) {
    func_ov039_02083960();
    func_ov039_02082754(scene);
    EasyTask_DestroyPool(OTU_POOL1(scene));
    EasyTask_DestroyPool(OTU_POOL2(scene));
    ResourceMgr_ReinitManagers(NULL);
    DatMgr_ClearSlot(scene->spareDataType);
    DatMgr_ClearSlot(scene->dataType);
    Mem_Free(&gDebugHeap, scene);
}

/**
 * @brief The scene's teardown thunk, reached by the dispatcher by address only.
 *
 * A bare `bx` of the real teardown's address -- no prologue and no argument
 * setup, so the dispatcher must already have the scene in r0.
 */
void func_ov039_020831cc(TinPinSlammer_Scene* scene) {
    func_ov039_02083164(scene);
}

/**
 * @brief The wireless scene's teardown, restoring the latched status bits.
 *
 * Same teardown, but it first writes the bits the entry point consumed back
 * into `SystemStatusFlags` -- bits 6 and 7, from `state.linkStatus` and
 * `state.wirelessStatus` -- so the wireless stack sees them again on the way
 * out.  The entry point cleared them; this puts them back.
 */
void func_ov039_020831d8(TinPinSlammer_Scene* scene) {
    SystemStatusFlags; // Unused volatile read, as in OtosuMenu
    SystemStatusFlags.unk_06 = scene->linkStatus;
    SystemStatusFlags;
    SystemStatusFlags.unk_07 = scene->wirelessStatus != 0;

    func_ov039_02083164(scene);
}

/**
 * @brief Process-stage dispatch for the plain scene's render callback.
 *
 * `MainOvlDisp_GetProcessStage` returns 0x7FFFFFFF when no stage is active, in
 * which case the scene is torn down; otherwise the stage table at
 * 0x02098e30 is indexed by stage.
 */
void func_ov039_02083240(TinPinSlammer_Scene* scene) {
    s32 stage = MainOvlDisp_GetProcessStage();

    if (stage == 0x7FFFFFFF) {
        func_ov039_020831cc(scene);
        return;
    }

    OtuScene_PlainHandlers[stage](scene); // NOLINT: untyped by design
}

/**
 * @brief Process-stage dispatch for the wireless scene.
 *
 * The wireless twin of func_ov039_02083240: same shape, indexing the wireless
 * handler table and reaching the wireless teardown on the no-stage path.
 */
void func_ov039_02083280(TinPinSlammer_Scene* scene) {
    s32 stage = MainOvlDisp_GetProcessStage();

    if (stage == 0x7FFFFFFF) {
        func_ov039_020831d8(scene);
        return;
    }

    OtuScene_WirelessHandlers[stage](scene); // NOLINT: untyped by design
}
