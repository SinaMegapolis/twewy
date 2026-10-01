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
            MainOvlDisp_ReplaceTop(&menuTag, 2, func_ov039_0208694c, NULL, 0);
        } else {
            MainOvlDisp_ReplaceTop(&resultTag, 2, func_ov039_020869cc, NULL, 0);
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

    *(s32*)((u8*)scene + TIN_PIN_SLAMMER_ENTRY_OFFSET) = 0;

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

    *(u8*)((u8*)scene + TIN_PIN_SLAMMER_ENTRY_OFFSET) = *OTU_WIRELESS_RECORD(0x3000, 0x41C);

    func_ov039_020824a0((u16*)scene);
    func_ov039_02098a40(OTU_STAGE(scene), &OtuScene_WirelessBoard);
    MainOvlDisp_NextProcessStage();
}
