#include "Debug/Sugata/TinPinSlammer.h"
#include "Display.h"
#include "Engine/Core/Memory.h"
#include "Engine/Core/System.h"
#include "Engine/EasyTask.h"
#include "Engine/Resources/ResourceMgr.h"

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
 * @brief Scene entry point for the plain wireless board.
 *
 * Selects the plain layout (func_ov039_02082724) and installs the main-display
 * VBlank pair.
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
