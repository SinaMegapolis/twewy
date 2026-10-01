#include "Debug/Sugata/TinPinSlammer.h"
#include "Display.h"
#include "Engine/Core/Memory.h"
#include "Engine/EasyTask.h"

/*
 * Scene entry points, not yet wired into delinks.txt.
 *
 * These allocate the 0x44A6C-byte scene block out of the debug heap, bring up
 * the managers, two task pools and the display, run the shared scene setup, and
 * hand control to the first stage.  Each variant differs only in which board
 * layout it selects and which VBlank/display pair it installs:
 *
 *   func_ov039_02082978  plain layout, func_ov039_02082724, vblank 02083928
 *   func_ov039_02082ae0  plain layout, func_ov039_02082724, vblank 02083928
 *   func_ov039_02082c50  alternate layout, func_ov039_0208273c, vblank 02083944
 *   func_ov039_02082e98  frame update (no scene allocation)
 *   func_ov039_02082ff8  frame update (no scene allocation)
 *
 * Blocked on TU layout, not on the code.  This TU's .text (0x02082978+) sorts
 * before OtuScene.c's .text, while its .data (0x0209a140, the sequence-name
 * table and the first stage descriptor) sorts after OtuScene.c's .data
 * (0x0209a2e0, the wireless stage descriptors).  dsd orders TUs so every
 * section ascends together and rejects the pair as a link-order cycle, so
 * these two files cannot coexist until the .data between them -- the
 * Shift-JIS tutorial strings at 0x0209a16c-0x0209a2e0 -- is decompiled and
 * claimed by its own TU.
 *
 * Drafts are ready; see tools/m2cdraft.py --range 0x02082978:0x02082ff8.
 */

/**
 * @brief Size of the scene block these entry points allocate.
 *
 * The last field written is at 0x44A64, and MainOvlDisp is handed the block
 * whole, so this is the block's true size.
 */
#define TIN_PIN_SLAMMER_SCENE_SIZE 0x44A6C

/* Nothing is emitted yet; the entry points land with the TU split above. */
s32 TinPinSlammer_OtuSceneEntry_Placeholder = TIN_PIN_SLAMMER_SCENE_SIZE;
