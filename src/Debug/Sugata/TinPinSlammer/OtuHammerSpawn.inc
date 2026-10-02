/* Band 8 -- see OtuBands.h for the address range and why this is an include.
 *
 * The child spawner shared by the overlay's simulation tasks. Tsk_OtosuGame_hammer's
 * CreateTask calls it four times in a loop, so it has to exist before that task
 * can link -- it was the blocker on scoping band 7.
 */

#include "Debug/Sugata/TinPinSlammer.h"
#include "Engine/EasyTask.h"

/** The handle this spawns. Already in the overlay's .rodata. */
extern const TaskHandle data_ov039_0209a024;

/** Spawns one child of a simulation task, packing two words of args.
 *
 * Typed as returning the new task's handle, because Tsk_OtosuGame_hammer's
 * CreateTask stores the result into its `children[i]` and the target's
 * `str r0, [r1, #0x128]` is unambiguous about that. This costs nothing here: the
 * body has no `mov r0` of its own in either typing, so the handle already comes
 * back in r0 and both spellings compile to the same eight instructions.
 */
s32 func_ov039_02098394(TaskPool* pool, s32 arg1, s32 arg2) {
    s32 args[2];

    args[0] = arg1;
    args[1] = arg2;

    return EasyTask_CreateTask(pool, &data_ov039_0209a024, NULL, 0, NULL, args);
}