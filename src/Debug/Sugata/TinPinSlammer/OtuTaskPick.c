#include "Debug/Sugata/TinPinSlammer.h"
#include "Engine/EasyTask.h"
#include "OtuFieldAccessShared.h"

/**
 * @file OtuTaskPick.c
 *
 * The five "nearest child of a given sort" queries.
 *
 * These are the overlay's first look at the pinball simulation rather than at
 * the results screen. The stage keeps a pool of child tasks; each of these
 * functions walks that pool, keeps the children its own filter accepts, and
 * returns the data pointer of the nearest one to the calling task.
 *
 * Three of the five are the *same function body* with a different constant in
 * the filter: 0208e9d0, 0208e998 and 0208e984 test a child's `kind` field
 * against 6, 7 and 8 respectively and are otherwise identical, and so are the
 * three queries that call them. That is the clearest evidence in the overlay
 * that 6, 7 and 8 name three sorts of the same object -- almost certainly the
 * three pin types the results screen scores separately.
 *
 * The other two filters differ in kind rather than in constant: 0208ee84 asks
 * whether a child is still alive, and 0208efb0 asks whether it is a real pin
 * rather than an empty tray slot, which is the one that takes the 0x444
 * argument.
 *
 * Every query has the same five-step shape:
 *
 *   1. read the calling task's own position, once, before the scan;
 *   2. walk the pool, skipping the caller's own slot and anything the filter
 *      rejects;
 *   3. score each survivor against the caller's position;
 *   4. keep the survivor with the *lowest* score;
 *   5. return its data pointer, or 0 if nothing survived.
 *
 * Step 4 is a minimum scan written as `if (best > candidate) best = candidate`,
 * which is worth stating because the register form reads like a maximum: the
 * target compares the incumbent against the challenger and adopts the
 * challenger when the incumbent is larger, which can only shrink the value.
 *
 * The score itself comes from func_ov039_02098ca8, which does not return a
 * distance. It writes the squared difference of the two points into the
 * overlay's vector unit at 0x40002B0, spins on a status bit there, and returns
 * half of a counter read back from 0x40002B4. So the ordering it induces is
 * the vector unit's, not a plain integer comparison, and the "nearest" reading
 * above is an inference from the register block's role rather than something
 * these functions state.
 */

/**
 * @brief One survivor of the scan: its score and the child it came from.
 *
 * The stride is 8 and the array starts at sp+0x10, so the frame is exactly two
 * 8-byte points followed by three of these. Three is what the frame allows; the
 * pool count at +0x140 is not bounded against it, so a pool larger than three
 * accepting children would overrun the array. That is the target's behaviour and
 * is reproduced rather than defended against.
 */
typedef struct {
    s32         score;
    OtuPinTask* task;
} OtuPick; // Size: 0x8

/** @brief The nearest live child, 0x02087f4c. Filter: still in play. */
// Nonmatching: 72.2%, four bytes over the target's 280. The logic is settled --
// every field, the self-skip, the minimum scan and the two early returns all
// match, and the four kind-queries differ from each other exactly as the
// targets do.
//
// The pool count being re-read each iteration rather than hoisted came out of
// decomp-permuter, which reached it by inlining the load back into the loop
// condition. It is also plainly visible in the target, and getting it wrong cost
// eight points (64.7% -> 72.2%), so it is not a permuter artefact.
//
// decomp-permuter then ran 7500 iterations from that point and never beat the
// baseline again, so what is left is not reachable by source shuffling: mwcc
// strength-reduces the loop's scaled address into a walked pointer where the
// target keeps it computed, and rematerialises sp+0 at the call site where the
// target hoists both sp-relative bases inside its count > 0 guard. Both are
// optimisation-threshold effects the source cannot ask for.
OtuPinTask* func_ov039_02087f4c(TaskPool* self, TinPinSlammer_Scene* scene, s32 which) {
    OtuPinStage* table = (OtuPinStage*)func_ov039_02098b70(OTU_STAGE(scene));
    // Declaration order here is load-bearing: mwcc hands out stack slots in
    // reverse, and the target puts candPos at sp+0, selfPos at sp+8 and picks
    // at sp+0x10, which is what makes the frame exactly 0x28.
    OtuPick     picks[3];
    OtuPoint    selfPos;
    OtuPoint    candPos;
    OtuPinTask* result = NULL;
    s32         found  = 0;
    s32         i;

    // The caller's own position, read once before the scan rather than
    // recomputed inside it.
    func_ov039_0208e6e0((OtuPinTask*)EasyTask_GetTaskData(self, table->childIds[which]), &selfPos);

    // The pool count is re-read from the table on every iteration, not hoisted
    // into a local: the target's `ldr r0, [r6, #0x140]` sits at the *bottom* of
    // the loop, beside the increment and the exit compare, so a task that
    // changes the count mid-scan is observed. Binding it to a local first --
    // which reads as the obvious thing to do -- compiles the load once and
    // costs instructions (72.2% -> 64.7%).

    for (i = 0; i < table->childCount; i++) {
        OtuPinTask* cand;

        if (i == which) {
            continue;
        }

        cand = (OtuPinTask*)EasyTask_GetTaskData(self, table->childIds[i]);

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

    if (found == 1) {
        return picks[0].task;
    }

    // Minimum scan, written the way the target writes it: adopt the challenger
    // when the incumbent scores higher. The register form reads like a maximum
    // because the comparison is against the incumbent, but it can only shrink
    // the value, so what it finds is the minimum.
    result = picks[0].task;
    for (i = 1; i < found; i++) {
        if (picks[0].score > picks[i].score) {
            result = picks[i].task;
        }
    }

    return result;
}

/** @brief The nearest child of kind 6, 0x02088064. */
// Nonmatching: 72.2%, four bytes over the target's 280. The logic is settled --
// every field, the self-skip, the minimum scan and the two early returns all
// match, and the four kind-queries differ from each other exactly as the
// targets do.
//
// The pool count being re-read each iteration rather than hoisted came out of
// decomp-permuter, which reached it by inlining the load back into the loop
// condition. It is also plainly visible in the target, and getting it wrong cost
// eight points (64.7% -> 72.2%), so it is not a permuter artefact.
//
// decomp-permuter then ran 7500 iterations from that point and never beat the
// baseline again, so what is left is not reachable by source shuffling: mwcc
// strength-reduces the loop's scaled address into a walked pointer where the
// target keeps it computed, and rematerialises sp+0 at the call site where the
// target hoists both sp-relative bases inside its count > 0 guard. Both are
// optimisation-threshold effects the source cannot ask for.
OtuPinTask* func_ov039_02088064(TaskPool* self, TinPinSlammer_Scene* scene, s32 which) {
    OtuPinStage* table = (OtuPinStage*)func_ov039_02098b70(OTU_STAGE(scene));
    // Declaration order here is load-bearing: mwcc hands out stack slots in
    // reverse, and the target puts candPos at sp+0, selfPos at sp+8 and picks
    // at sp+0x10, which is what makes the frame exactly 0x28.
    OtuPick     picks[3];
    OtuPoint    selfPos;
    OtuPoint    candPos;
    OtuPinTask* result = NULL;
    s32         found  = 0;
    s32         i;

    // The caller's own position, read once before the scan rather than
    // recomputed inside it.
    func_ov039_0208e6e0((OtuPinTask*)EasyTask_GetTaskData(self, table->childIds[which]), &selfPos);

    // The pool count is re-read from the table on every iteration, not hoisted
    // into a local: the target's `ldr r0, [r6, #0x140]` sits at the *bottom* of
    // the loop, beside the increment and the exit compare, so a task that
    // changes the count mid-scan is observed. Binding it to a local first --
    // which reads as the obvious thing to do -- compiles the load once and
    // costs instructions (72.2% -> 64.7%).

    for (i = 0; i < table->childCount; i++) {
        OtuPinTask* cand;

        if (i == which) {
            continue;
        }

        cand = (OtuPinTask*)EasyTask_GetTaskData(self, table->childIds[i]);

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

    if (found == 1) {
        return picks[0].task;
    }

    // Minimum scan, written the way the target writes it: adopt the challenger
    // when the incumbent scores higher. The register form reads like a maximum
    // because the comparison is against the incumbent, but it can only shrink
    // the value, so what it finds is the minimum.
    result = picks[0].task;
    for (i = 1; i < found; i++) {
        if (picks[0].score > picks[i].score) {
            result = picks[i].task;
        }
    }

    return result;
}

/** @brief The nearest child of kind 7, 0x0208817c. */
// Nonmatching: 72.2%, four bytes over the target's 280. The logic is settled --
// every field, the self-skip, the minimum scan and the two early returns all
// match, and the four kind-queries differ from each other exactly as the
// targets do.
//
// The pool count being re-read each iteration rather than hoisted came out of
// decomp-permuter, which reached it by inlining the load back into the loop
// condition. It is also plainly visible in the target, and getting it wrong cost
// eight points (64.7% -> 72.2%), so it is not a permuter artefact.
//
// decomp-permuter then ran 7500 iterations from that point and never beat the
// baseline again, so what is left is not reachable by source shuffling: mwcc
// strength-reduces the loop's scaled address into a walked pointer where the
// target keeps it computed, and rematerialises sp+0 at the call site where the
// target hoists both sp-relative bases inside its count > 0 guard. Both are
// optimisation-threshold effects the source cannot ask for.
OtuPinTask* func_ov039_0208817c(TaskPool* self, TinPinSlammer_Scene* scene, s32 which) {
    OtuPinStage* table = (OtuPinStage*)func_ov039_02098b70(OTU_STAGE(scene));
    // Declaration order here is load-bearing: mwcc hands out stack slots in
    // reverse, and the target puts candPos at sp+0, selfPos at sp+8 and picks
    // at sp+0x10, which is what makes the frame exactly 0x28.
    OtuPick     picks[3];
    OtuPoint    selfPos;
    OtuPoint    candPos;
    OtuPinTask* result = NULL;
    s32         found  = 0;
    s32         i;

    // The caller's own position, read once before the scan rather than
    // recomputed inside it.
    func_ov039_0208e6e0((OtuPinTask*)EasyTask_GetTaskData(self, table->childIds[which]), &selfPos);

    // The pool count is re-read from the table on every iteration, not hoisted
    // into a local: the target's `ldr r0, [r6, #0x140]` sits at the *bottom* of
    // the loop, beside the increment and the exit compare, so a task that
    // changes the count mid-scan is observed. Binding it to a local first --
    // which reads as the obvious thing to do -- compiles the load once and
    // costs instructions (72.2% -> 64.7%).

    for (i = 0; i < table->childCount; i++) {
        OtuPinTask* cand;

        if (i == which) {
            continue;
        }

        cand = (OtuPinTask*)EasyTask_GetTaskData(self, table->childIds[i]);

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

    if (found == 1) {
        return picks[0].task;
    }

    // Minimum scan, written the way the target writes it: adopt the challenger
    // when the incumbent scores higher. The register form reads like a maximum
    // because the comparison is against the incumbent, but it can only shrink
    // the value, so what it finds is the minimum.
    result = picks[0].task;
    for (i = 1; i < found; i++) {
        if (picks[0].score > picks[i].score) {
            result = picks[i].task;
        }
    }

    return result;
}

/** @brief The nearest child of kind 8, 0x02088294. */
// Nonmatching: 72.2%, four bytes over the target's 280. The logic is settled --
// every field, the self-skip, the minimum scan and the two early returns all
// match, and the four kind-queries differ from each other exactly as the
// targets do.
//
// The pool count being re-read each iteration rather than hoisted came out of
// decomp-permuter, which reached it by inlining the load back into the loop
// condition. It is also plainly visible in the target, and getting it wrong cost
// eight points (64.7% -> 72.2%), so it is not a permuter artefact.
//
// decomp-permuter then ran 7500 iterations from that point and never beat the
// baseline again, so what is left is not reachable by source shuffling: mwcc
// strength-reduces the loop's scaled address into a walked pointer where the
// target keeps it computed, and rematerialises sp+0 at the call site where the
// target hoists both sp-relative bases inside its count > 0 guard. Both are
// optimisation-threshold effects the source cannot ask for.
OtuPinTask* func_ov039_02088294(TaskPool* self, TinPinSlammer_Scene* scene, s32 which) {
    OtuPinStage* table = (OtuPinStage*)func_ov039_02098b70(OTU_STAGE(scene));
    // Declaration order here is load-bearing: mwcc hands out stack slots in
    // reverse, and the target puts candPos at sp+0, selfPos at sp+8 and picks
    // at sp+0x10, which is what makes the frame exactly 0x28.
    OtuPick     picks[3];
    OtuPoint    selfPos;
    OtuPoint    candPos;
    OtuPinTask* result = NULL;
    s32         found  = 0;
    s32         i;

    // The caller's own position, read once before the scan rather than
    // recomputed inside it.
    func_ov039_0208e6e0((OtuPinTask*)EasyTask_GetTaskData(self, table->childIds[which]), &selfPos);

    // The pool count is re-read from the table on every iteration, not hoisted
    // into a local: the target's `ldr r0, [r6, #0x140]` sits at the *bottom* of
    // the loop, beside the increment and the exit compare, so a task that
    // changes the count mid-scan is observed. Binding it to a local first --
    // which reads as the obvious thing to do -- compiles the load once and
    // costs instructions (72.2% -> 64.7%).

    for (i = 0; i < table->childCount; i++) {
        OtuPinTask* cand;

        if (i == which) {
            continue;
        }

        cand = (OtuPinTask*)EasyTask_GetTaskData(self, table->childIds[i]);

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

    if (found == 1) {
        return picks[0].task;
    }

    // Minimum scan, written the way the target writes it: adopt the challenger
    // when the incumbent scores higher. The register form reads like a maximum
    // because the comparison is against the incumbent, but it can only shrink
    // the value, so what it finds is the minimum.
    result = picks[0].task;
    for (i = 1; i < found; i++) {
        if (picks[0].score > picks[i].score) {
            result = picks[i].task;
        }
    }

    return result;
}

/**
 * @brief The nearest real pin, 0x02087e2c.
 *
 * The odd one out: its filter, func_ov039_0208efb0, is the only one of the five
 * that takes a second argument, and 0x444 is the value the target passes. The
 * filter rejects a child whose tray slot holds 0x130 -- the same "no pin"
 * sentinel func_ov039_020824a0 fills the tray with -- so this query is the only
 * one of the five that can be handed an empty slot and has to notice.
 *
 * That extra argument is why this one is not routed through the shared scan: the
 * filter's type differs, and forcing it through would mean either a cast or a
 * second parameter threaded through the helper for the sake of one caller.
 */
// Nonmatching: 72.2%, four bytes over the target's 280. The logic is settled --
// every field, the self-skip, the minimum scan and the two early returns all
// match, and the four kind-queries differ from each other exactly as the
// targets do.
//
// The pool count being re-read each iteration rather than hoisted came out of
// decomp-permuter, which reached it by inlining the load back into the loop
// condition. It is also plainly visible in the target, and getting it wrong cost
// eight points (64.7% -> 72.2%), so it is not a permuter artefact.
//
// decomp-permuter then ran 7500 iterations from that point and never beat the
// baseline again, so what is left is not reachable by source shuffling: mwcc
// strength-reduces the loop's scaled address into a walked pointer where the
// target keeps it computed, and rematerialises sp+0 at the call site where the
// target hoists both sp-relative bases inside its count > 0 guard. Both are
// optimisation-threshold effects the source cannot ask for.
OtuPinTask* func_ov039_02087e2c(TaskPool* self, TinPinSlammer_Scene* scene, s32 which) {
    OtuPinStage* table  = (OtuPinStage*)func_ov039_02098b70(OTU_STAGE(scene));
    OtuPinTask*  result = NULL;
    OtuPoint     selfPos;
    OtuPoint     candPos;
    OtuPick      picks[3];
    s32          found = 0;
    s32          i;

    func_ov039_0208e6e0((OtuPinTask*)EasyTask_GetTaskData(self, table->childIds[which]), &selfPos);

    // The pool count is re-read from the table on every iteration, not hoisted
    // into a local: the target's `ldr r0, [r6, #0x140]` sits at the *bottom* of
    // the loop, beside the increment and the exit compare, so a task that
    // changes the count mid-scan is observed. Binding it to a local first --
    // which reads as the obvious thing to do -- compiles the load once and
    // costs instructions (72.2% -> 64.7%).

    for (i = 0; i < table->childCount; i++) {
        OtuPinTask* cand;

        if (i == which) {
            continue;
        }

        cand = (OtuPinTask*)EasyTask_GetTaskData(self, table->childIds[i]);

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

    if (found == 1) {
        return picks[0].task;
    }

    result = picks[0].task;
    for (i = 1; i < found; i++) {
        if (picks[0].score > picks[i].score) {
            result = picks[i].task;
        }
    }

    return result;
}