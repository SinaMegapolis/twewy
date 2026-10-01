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
#include "Save.h"

/**
 * @brief The menus/results entry point -- the third scene variant.
 *
 * The longest of the three entry points and the only one that branches. Where
 * `func_ov039_02082978` and `func_ov039_02082ae0` differ only in which setup
 * routine and which stage descriptor they finish with, this one also picks
 * between the menu stage and the result stage from the save record, and swaps
 * the two text-block templates to match.
 *
 * Structurally it is the same opening as the other two -- allocate the scene,
 * set the sequence name, build the heap, take two save slots, clear the slot
 * state, reinit the resource managers, start both task pools -- and then a tail
 * the others do not have:
 *
 *   * it latches the two `SystemStatusFlags` bits into the scene (the inverse of
 *     what `func_ov039_020831d8` does at teardown), and
 *   * it exchanges `OtuScene_WirelessMenu` and `OtuScene_ResultMenu` through a
 *     stack temporary when the record says the menus are already swapped.
 *
 * The swap is four `ldm`/`stm` pairs rather than a loop or a struct copy: mwcc
 * emits a 16-byte aggregate assignment as register-pair moves, and the temporary
 * has to be a real local for that to happen.
 */
// Nonmatching: 79.8%, and 16 bytes short of the target's 584.
//
// The logic is settled and every pool word pairs one-for-one with the target's:
// the sequence-name index is 1 (the target reads `[base, #4]`), the record
// fields are at +0x40A/+0x40B/+0x40C, and the copy source really is
// `gSaveData + 0x3EC` (reached as two adds, hence OTU_WIRELESS_RECORD_SPLIT).
// The three stage descriptors, both text-block globals and both save-slot clears
// are all correct.
//
// Two things remain, both attempts at which made it *worse* and were reverted:
//
//   * the 16 missing bytes. The target loads each task-pool address from the
//     literal pool and then adds the scene base -- `ldr r0, [pc, #x]` followed
//     by `add r0, r4, r0` -- for both pools. Written inline this build folds
//     each into one `add r0, r4, #0x41598`. Routing them through locals was
//     tried and mwcc folded those too (568 -> 564, i.e. it got worse). Two
//     literal-pool words the target does not have (`0x41598`, `0x41618`) are
//     therefore missing from this build, which is most of the gap.
//   * register allocation for the `SystemStatusFlags` latch and the record
//     pointer: the target keeps the record in r12 and the latch result in r5,
//     this build uses r5 and r2.
//
// The `ldmia`/`stmia` swap of the two text blocks does match, which is the part
// most likely to have been wrong.
void func_ov039_02082c50(void) {
    const char*          sequenceName = *((const char* const*)OtuScene_SequenceNames + 1);
    TinPinSlammer_Scene* scene        = Mem_AllocHeapTail(&gDebugHeap, TIN_PIN_SLAMMER_SCENE_SIZE);
    u8*                  record       = OTU_WIRELESS_RECORD(0x3020, 0);
    OtuTextBlock         swap;

    Mem_SetSequence(&gDebugHeap, scene, sequenceName);

    MainOvlDisp_SetCbArg(scene);
    Mem_InitializeHeap(OTU_HEAP(scene), OTU_HEAP_BUFFER(scene), 0x30000);

    scene->base.spareDataType = DatMgr_AllocateSlot();
    scene->base.dataType      = DatMgr_AllocateSlot();

    OtuScene_SlotState.count = 0;
    OtuScene_SlotState.flag  = 0;
    scene->state.unk_EE4     = 0;
    scene->state.unk_EE8     = 0;
    scene->state.unk_EEC     = 0;

    func_ov039_02083944();
    scene->base.prevResMgr = ResourceMgr_ReinitManagers(&scene->base.resMgr);
    EasyTask_InitializePool(OTU_POOL1(scene), OTU_HEAP(scene), 8, NULL, NULL);
    EasyTask_InitializePool(OTU_POOL2(scene), OTU_HEAP(scene), 0x180, NULL, NULL);
    func_0200d8f0();

    data_02066aec                            = 0;
    data_02066eec                            = 0;
    g_DisplaySettings.controls[0].brightness = 0x10;
    g_DisplaySettings.controls[1].brightness = 0x10;

    func_ov039_020825e8(scene);
    func_ov039_0208273c(scene);

    scene->state.linkStatus     = SystemStatusFlags.unk_06;
    scene->state.wirelessStatus = SystemStatusFlags.unk_07 != 0;

    func_ov039_020824a0((u16*)scene);

    scene->pad_44A68[0]  = record[0x40C];
    scene->state.unk_EE0 = record[0x40B];

    swap                  = OtuScene_WirelessMenu;
    OtuScene_WirelessMenu = OtuScene_ResultMenu;
    OtuScene_ResultMenu   = swap;

    if (record[0x40A] == 0) {
        func_ov039_02098a40(OTU_STAGE(scene), &OtuScene_MenuStage);
    } else {
        MI_CpuCopyU8(OTU_WIRELESS_RECORD_SPLIT(0x3EC), (u8*)scene + 0x41ED8, 6);
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
// Nonmatching: 90.9%. Every call, field access and branch agrees with the
// target. The remainder is register choice in the prologue (target holds the
// scene in r4 and the pool-1 offset in r3; mwcc picks r5/r4) and the
// literal-pool layout that follows from it.  Five source shapes were tried and
// this is the best of them.
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
    if (scene->state.unk_698 != 0) {
        EasyTask_ProcessPendingTasks(OTU_POOL2(scene));
    }

    func_ov039_02082774(scene);

    EasyTask_UpdateActiveTasks(OTU_POOL1(scene));
    EasyTask_UpdateActiveTasks(OTU_POOL2(scene));

    if (scene->state.unk_ADC != 0) {
        // 0x1F is the record's "finished" marker; anything else means the
        // wireless game is still in progress and the menu overlay is wanted.
        if (*OTU_WIRELESS_RECORD(0x1000, 0x41C) != 0x1F) {
            MainOvlDisp_ReplaceTop(&menuTag, OTU_OVERLAY_ID, func_ov039_0208694c, NULL, 0);
        } else {
            MainOvlDisp_ReplaceTop(&resultTag, OTU_OVERLAY_ID, func_ov039_020869cc, NULL, 0);
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

// Nonmatching: 95.4%. Both functions' sizes and instruction sequences match the
// target exactly. The remainder is one word: the target loads the overlay id
// from a literal-pool word where mwcc folds it into `mov r1, #2`, which shifts
// every later pool displacement. Seven spellings of the id and a volatile local
// were tried; all are equal or worse.
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
        if (scene->state.unk_698 != 0) {
            EasyTask_ProcessPendingTasks(OTU_POOL2(scene));
        }

        func_ov039_020827d0(scene);
    }

    EasyTask_UpdateActiveTasks(OTU_POOL1(scene));
    EasyTask_UpdateActiveTasks(OTU_POOL2(scene));

    if (scene->state.unk_ADC != 0) {
        if (scene->state.unk_AE0 == 0) {
            MainOvlDisp_ReplaceTop(&linkTag, OTU_OVERLAY_ID, func_ov039_0208690c, NULL, 0);
        } else {
            MainOvlDisp_ReplaceTop(&resultTag, OTU_OVERLAY_ID, func_ov039_0208698c, NULL, 0);
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
    TinPinSlammer_Scene* scene        = Mem_AllocHeapTail(&gDebugHeap, TIN_PIN_SLAMMER_SCENE_SIZE);

    Mem_SetSequence(&gDebugHeap, scene, sequenceName);
    MainOvlDisp_SetCbArg(scene);
    Mem_InitializeHeap(OTU_HEAP(scene), OTU_HEAP_BUFFER(scene), 0x30000);

    scene->base.spareDataType = DatMgr_AllocateSlot();
    scene->base.dataType      = DatMgr_AllocateSlot();

    OtuScene_SlotState.count = 0;
    OtuScene_SlotState.flag  = 0;
    scene->state.unk_EE4     = 0;
    scene->state.unk_EE8     = 0;
    scene->state.unk_EEC     = 0;

    func_ov039_02083928();
    scene->base.prevResMgr = ResourceMgr_ReinitManagers(&scene->base.resMgr);
    EasyTask_InitializePool(OTU_POOL1(scene), OTU_HEAP(scene), 8, NULL, NULL);
    EasyTask_InitializePool(OTU_POOL2(scene), OTU_HEAP(scene), 0x180, NULL, NULL);
    func_0200d8f0();

    data_02066aec                            = 0;
    data_02066eec                            = 0;
    g_DisplaySettings.controls[0].brightness = 0x10;
    g_DisplaySettings.controls[1].brightness = 0x10;

    func_ov039_020825e8(scene);
    func_ov039_02082724(scene);

    scene->menuIndex = 0;

    func_ov039_020824a0((u16*)scene);
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
    TinPinSlammer_Scene* scene        = Mem_AllocHeapTail(&gDebugHeap, TIN_PIN_SLAMMER_SCENE_SIZE);

    Mem_SetSequence(&gDebugHeap, scene, sequenceName);

    MainOvlDisp_SetCbArg(scene);
    Mem_InitializeHeap(OTU_HEAP(scene), OTU_HEAP_BUFFER(scene), 0x30000);

    scene->base.spareDataType = DatMgr_AllocateSlot();
    scene->base.dataType      = DatMgr_AllocateSlot();

    OtuScene_SlotState.count = 0;
    OtuScene_SlotState.flag  = 0;
    scene->state.unk_EE4     = 0;
    scene->state.unk_EE8     = 0;
    scene->state.unk_EEC     = 0;

    func_ov039_02083928();
    scene->base.prevResMgr = ResourceMgr_ReinitManagers(&scene->base.resMgr);
    EasyTask_InitializePool(OTU_POOL1(scene), OTU_HEAP(scene), 8, NULL, NULL);
    EasyTask_InitializePool(OTU_POOL2(scene), OTU_HEAP(scene), 0x180, NULL, NULL);
    func_0200d8f0();

    data_02066aec                            = 0;
    data_02066eec                            = 0;
    g_DisplaySettings.controls[0].brightness = 0x10;
    g_DisplaySettings.controls[1].brightness = 0x10;

    func_ov039_020825e8(scene);
    func_ov039_02082724(scene);

    scene->menuIndex = *OTU_WIRELESS_RECORD(0x3000, 0x41C);

    func_ov039_020824a0((u16*)scene);
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
    DatMgr_ClearSlot(scene->base.spareDataType);
    DatMgr_ClearSlot(scene->base.dataType);
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
// Nonmatching: 72.8%. The bit writes and the teardown call are all correct;
// the target has no prologue and ends in a tail call (`bx ip`) where this build
// pushes {r3, lr} and uses `bl`. mwcc needs one more scratch register for the
// two read-modify-writes than the target does. Six spellings of the bitfield
// assignment and a locals-first variant were tried; all are equal or worse.
void func_ov039_020831d8(TinPinSlammer_Scene* scene) {
    SystemStatusFlags.unk_06 = scene->state.linkStatus != 0;
    SystemStatusFlags.unk_07 = scene->state.wirelessStatus != 0;

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
