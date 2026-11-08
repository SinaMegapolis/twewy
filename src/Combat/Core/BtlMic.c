#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatMic.h"
#include "Combat/Core/CombatTouch.h"
#include "Engine/Core/System.h"
#include <nitro/mi/cpumem.h>

static s32 BtlMic_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

static const TaskHandle Tsk_BtlMic = {"Tsk_BtlMic", BtlMic_RunTask, sizeof(BtlMicInput*)};

/// Starts sampling the microphone into the battle's sample buffer.
void BtlMic_Init(void) {
    BtlMicInput* input;
    s32          i;

    func_020405d4();
    func_02040ac0();
    input = &g_CombatGlobal->microphone;
    for (i = 0; i < 0x20; i++) {
        input->samples[i] = 0;
    }
    input->sampling.type        = 2;
    input->sampling.buffer      = input->samples;
    input->sampling.size        = 0x20;
    input->sampling.rate        = 0x105D;
    input->sampling.loop        = 0;
    input->sampling.callback    = func_ov003_020a50f4;
    input->sampling.callbackArg = input;
    func_02041030(1);
    func_02041048(1);
    input->cooldown = 0xB4;
}

void BtlMic_CreateTask(void) {
    BtlMicInput* input = &g_CombatGlobal->microphone;

    EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &Tsk_BtlMic, NULL, 0, NULL, &input);
}

s32 func_ov003_020a50cc(void) {
    BtlMicInput* input = &g_CombatGlobal->microphone;

    return input->cooldown != 0 ? 0 : input->detected;
}

// The retail callback tests positive signed samples only; the result code is unused.
void func_ov003_020a50f4(s32 result, BtlMicInput* input) {
    input->detected = 0;
    for (s32 i = 0; i < 32; i++) {
        if (input->samples[i] > 100) {
            input->detected = 1;
        }
    }
}

static inline BOOL BtlMic_IsSuppressed(void) {
    if (SystemStatusFlags.unk_10) {
        return TRUE;
    }
    return FALSE;
}

static s32 BtlMic_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    switch (stage) {
        default:
        case 0:
            *(BtlMicInput**)task->data = *(BtlMicInput**)args;
            return 1;
        case 1: {
            BtlMicInput** state = task->data;
            BtlMicInput*  input = *state;
            if (input->cooldown != 0) {
                input->cooldown--;
            } else if (BtlMic_IsSuppressed()) {
                input->cooldown = 180;
            }
            func_02040770(&(*state)->sampling);
            return 1;
        }
        case 2:
            func_ov003_020a50cc();
            return 1;
        case 3:
            return 1;
    }
}

// All six microphone-psych actor kinds must be represented, regardless of duplicates.
s32 func_ov003_020a51c0(void) {
    s32 count = 0;

    s32 seen[6];
    MI_CpuSet(seen, 0, sizeof(seen));

    Ov003TouchActor* actor = CombatList_GetHead(&g_CombatGlobal->unk3D8BC);
    while (actor != NULL) {
        if (actor->kind >= 212 && actor->kind <= 217) {
            s32 index = actor->kind - 212;
            if (seen[index] == 0) {
                seen[index] = 1;
                count++;
            }
        }
        actor = CombatList_GetNext(&actor->node);
    }
    return count == 6;
}
