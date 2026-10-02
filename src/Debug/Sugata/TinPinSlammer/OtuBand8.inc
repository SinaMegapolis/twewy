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
 * Returns nothing: the handle is discarded, because the parent records the
 * child by handle out of the pool itself rather than being handed it here. That
 * is why the target's last two instructions are the call and the epilogue with
 * no `mov r0` in between -- r0 holds the new task and simply is not used.
 */
void func_ov039_02098394(TaskPool* pool, s32 arg1, s32 arg2) {
    s32 args[2];

    args[0] = arg1;
    args[1] = arg2;

    EasyTask_CreateTask(pool, &data_ov039_0209a024, NULL, 0, NULL, args);
}