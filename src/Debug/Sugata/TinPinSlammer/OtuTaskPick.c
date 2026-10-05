#include "Debug/Sugata/TinPinSlammer.h"
#include "Engine/EasyTask.h"
#include "OtuFieldAccessShared.h"

/**
 * @file OtuTaskPick.c
 * @brief The badge AI's target queries: each returns the other badge nearest to
 *        badge `which` among those its filter accepts, or NULL.
 */

/** A candidate: its distance score (func_ov039_02098ca8) and the badge. */
typedef struct {
    s32       score;
    OtuBadge* task;
} OtuPick; // Size: 0x8

/** The nearest stunned badge. */
OtuBadge* func_ov039_02087f4c(TaskPool* pool, TinPinSlammer_Scene* scene, s32 which) {
    OtuBoardStage* stage = (OtuBoardStage*)func_ov039_02098b70(OTU_STAGE(scene));
    OtuPick        picks[3];
    OtuPoint       selfPos;
    OtuPoint       candPos;
    s32            found;
    s32            best;
    s32            i;

    func_ov039_0208e6e0((OtuBadge*)EasyTask_GetTaskData(pool, stage->badgeIds[which]), &selfPos);

    found = 0;
    for (i = 0; i < stage->badgeCount; i++) {
        OtuBadge* cand;

        if (i == which) {
            continue;
        }

        cand = (OtuBadge*)EasyTask_GetTaskData(pool, stage->badgeIds[i]);

        if (!func_ov039_0208ee84(cand)) {
            continue;
        }

        func_ov039_0208e6e0(cand, &candPos);

        picks[found].score = func_ov039_02098ca8(&selfPos, &candPos);
        picks[found].task  = cand;
        found++;
    }

    if (found <= 0) {
        return NULL;
    }

    if (found <= 1) {
        return picks[0].task;
    }

    // The nearest candidate.
    best = 0;
    for (i = 1; i < found; i++) {
        if (picks[best].score > picks[i].score) {
            best = i;
        }
    }

    return picks[best].task;
}

/** The nearest badge in phase 6. */
OtuBadge* func_ov039_02088064(TaskPool* pool, TinPinSlammer_Scene* scene, s32 which) {
    OtuBoardStage* stage = (OtuBoardStage*)func_ov039_02098b70(OTU_STAGE(scene));
    OtuPick        picks[3];
    OtuPoint       selfPos;
    OtuPoint       candPos;
    s32            found;
    s32            best;
    s32            i;

    func_ov039_0208e6e0((OtuBadge*)EasyTask_GetTaskData(pool, stage->badgeIds[which]), &selfPos);

    found = 0;
    for (i = 0; i < stage->badgeCount; i++) {
        OtuBadge* cand;

        if (i == which) {
            continue;
        }

        cand = (OtuBadge*)EasyTask_GetTaskData(pool, stage->badgeIds[i]);

        if (!func_ov039_0208e9d0(cand)) {
            continue;
        }

        func_ov039_0208e6e0(cand, &candPos);

        picks[found].score = func_ov039_02098ca8(&selfPos, &candPos);
        picks[found].task  = cand;
        found++;
    }

    if (found <= 0) {
        return NULL;
    }

    if (found <= 1) {
        return picks[0].task;
    }

    // The nearest candidate.
    best = 0;
    for (i = 1; i < found; i++) {
        if (picks[best].score > picks[i].score) {
            best = i;
        }
    }

    return picks[best].task;
}

/** The nearest badge in phase 7. */
OtuBadge* func_ov039_0208817c(TaskPool* pool, TinPinSlammer_Scene* scene, s32 which) {
    OtuBoardStage* stage = (OtuBoardStage*)func_ov039_02098b70(OTU_STAGE(scene));
    OtuPick        picks[3];
    OtuPoint       selfPos;
    OtuPoint       candPos;
    s32            found;
    s32            best;
    s32            i;

    func_ov039_0208e6e0((OtuBadge*)EasyTask_GetTaskData(pool, stage->badgeIds[which]), &selfPos);

    found = 0;
    for (i = 0; i < stage->badgeCount; i++) {
        OtuBadge* cand;

        if (i == which) {
            continue;
        }

        cand = (OtuBadge*)EasyTask_GetTaskData(pool, stage->badgeIds[i]);

        if (!func_ov039_0208e998(cand)) {
            continue;
        }

        func_ov039_0208e6e0(cand, &candPos);

        picks[found].score = func_ov039_02098ca8(&selfPos, &candPos);
        picks[found].task  = cand;
        found++;
    }

    if (found <= 0) {
        return NULL;
    }

    if (found <= 1) {
        return picks[0].task;
    }

    // The nearest candidate.
    best = 0;
    for (i = 1; i < found; i++) {
        if (picks[best].score > picks[i].score) {
            best = i;
        }
    }

    return picks[best].task;
}

/** The nearest badge in phase 8. */
OtuBadge* func_ov039_02088294(TaskPool* pool, TinPinSlammer_Scene* scene, s32 which) {
    OtuBoardStage* stage = (OtuBoardStage*)func_ov039_02098b70(OTU_STAGE(scene));
    OtuPick        picks[3];
    OtuPoint       selfPos;
    OtuPoint       candPos;
    s32            found;
    s32            best;
    s32            i;

    func_ov039_0208e6e0((OtuBadge*)EasyTask_GetTaskData(pool, stage->badgeIds[which]), &selfPos);

    found = 0;
    for (i = 0; i < stage->badgeCount; i++) {
        OtuBadge* cand;

        if (i == which) {
            continue;
        }

        cand = (OtuBadge*)EasyTask_GetTaskData(pool, stage->badgeIds[i]);

        if (!func_ov039_0208e984(cand)) {
            continue;
        }

        func_ov039_0208e6e0(cand, &candPos);

        picks[found].score = func_ov039_02098ca8(&selfPos, &candPos);
        picks[found].task  = cand;
        found++;
    }

    if (found <= 0) {
        return NULL;
    }

    if (found <= 1) {
        return picks[0].task;
    }

    // The nearest candidate.
    best = 0;
    for (i = 1; i < found; i++) {
        if (picks[best].score > picks[i].score) {
            best = i;
        }
    }

    return picks[best].task;
}

/**
 * The nearest badge on the board, holding a pin, that func_ov039_0208ef4c
 * accepts for 0x444.
 */
OtuBadge* func_ov039_02087e2c(TaskPool* pool, TinPinSlammer_Scene* scene, s32 which) {
    OtuBoardStage* stage = (OtuBoardStage*)func_ov039_02098b70(OTU_STAGE(scene));
    OtuPoint       selfPos;
    OtuPoint       candPos;
    OtuPick        picks[3];
    s32            found;
    s32            best;
    s32            i;

    func_ov039_0208e6e0((OtuBadge*)EasyTask_GetTaskData(pool, stage->badgeIds[which]), &selfPos);

    found = 0;
    for (i = 0; i < stage->badgeCount; i++) {
        OtuBadge* cand;

        if (i == which) {
            continue;
        }

        cand = (OtuBadge*)EasyTask_GetTaskData(pool, stage->badgeIds[i]);

        if (!func_ov039_0208efb0(cand, 0x444)) {
            continue;
        }

        func_ov039_0208e6e0(cand, &candPos);

        picks[found].score = func_ov039_02098ca8(&selfPos, &candPos);
        picks[found].task  = cand;
        found++;
    }

    if (found <= 0) {
        return NULL;
    }

    if (found <= 1) {
        return picks[0].task;
    }

    // The nearest candidate.
    best = 0;
    for (i = 1; i < found; i++) {
        if (picks[best].score > picks[i].score) {
            best = i;
        }
    }

    return picks[best].task;
}
