/**
 * @file OtuStageDispatch.c
 * @brief The scene's stage sequencer (OtuStageDispatch).
 */

#include "OtuFieldAccessShared.h"

/* The scene's stage sequencer (OtuStageDispatch). */

/** Resets the sequencer. The stores are in the target's 0/4/C/10/8/14 order. */
void func_ov039_02098a20(OtuStageDispatch* dispatch, Heap* heap) {
    dispatch->heap       = heap;
    dispatch->stage      = NULL;
    dispatch->next       = NULL;
    dispatch->pending    = FALSE;
    dispatch->stageIndex = 0;
    dispatch->stageBlock = NULL;
}

/** Queues `next` to be entered by the next func_ov039_02098a60. */
void func_ov039_02098a40(OtuStageDispatch* dispatch, const OtuSceneStage* next) {
    dispatch->next    = next;
    dispatch->pending = TRUE;
}

/** Moves on to the stage's next step. */
void func_ov039_02098a50(OtuStageDispatch* dispatch) {
    dispatch->stageIndex = dispatch->stageIndex + 1;
}

/** Enters the queued stage: allocates its block and runs its `enter`. */
void func_ov039_02098a60(OtuStageDispatch* dispatch, void* scene) {
    const OtuSceneStage* stage;
    OtuStageHandler      enter;

    if (dispatch->pending == FALSE) {
        return;
    }

    dispatch->stage      = dispatch->next;
    dispatch->stageIndex = 0;
    dispatch->next       = NULL;
    dispatch->pending    = FALSE;

    stage = dispatch->stage;
    if (stage->blockSize != 0) {
        dispatch->stageBlock = Mem_AllocHeapTail(dispatch->heap, stage->blockSize);
    }

    stage = dispatch->stage;
    if (stage != NULL && (enter = stage->enter) != NULL) {
        enter(scene);
    }
}

/** Runs the current stage's current step. */
void func_ov039_02098acc(OtuStageDispatch* dispatch, void* scene) {
    const OtuSceneStage* stage = dispatch->stage;

    if (stage == NULL) {
        return;
    }

    stage->steps[dispatch->stageIndex](scene);
}

/**
 * Exits the current stage before a queued switch: runs its `exit` and frees its
 * block. The handler test is one `&&` chain, which is the target's `ldrne`.
 */
void func_ov039_02098af4(OtuStageDispatch* dispatch, void* scene) {
    const OtuSceneStage* stage;
    void*                block;
    OtuStageHandler      exit;

    if (dispatch->pending == FALSE) {
        return;
    }

    stage = dispatch->stage;
    if (stage != NULL && (exit = stage->exit) != NULL) {
        exit(scene);
    }

    block = dispatch->stageBlock;
    if (block == NULL) {
        return;
    }

    Mem_Free(dispatch->heap, block);
    dispatch->stageBlock = NULL;
}

/** True when the sequencer is pending a switch to no stage at all. */
s32 func_ov039_02098b44(OtuStageDispatch* dispatch) {
    if (dispatch->next == NULL && dispatch->pending != FALSE) {
        return 1;
    }
    return 0;
}

const OtuSceneStage* func_ov039_02098b68(OtuStageDispatch* dispatch) {
    return dispatch->stage;
}

void* func_ov039_02098b70(OtuStageDispatch* dispatch) {
    return dispatch->stageBlock;
}

/** dst = src. */
void func_ov039_02098b78(OtuPoint* src, OtuPoint* dst) {
    dst->x = src->x;
    dst->y = src->y;
}

/** out = a + b. */
void func_ov039_02098b8c(OtuPoint* a, OtuPoint* b, OtuPoint* out) {
    out->x = a->x + b->x;
    out->y = a->y + b->y;
}
