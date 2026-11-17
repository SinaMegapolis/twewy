#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatPsychEffects.h"
#include <nitro/mi/cpumem.h>

typedef struct BtlFieldShake {
    s32             state;
    Ov003CameraAnim camera;
    s32             timer;
} BtlFieldShake; // Size: 0x20

static s32 func_ov003_020a8718(TaskPool* pool, Task* task, void* args, s32 stage);
static s32 func_ov003_020a8780(TaskPool* pool, Task* task, void* args, s32 stage);

// Nonmatching: mwcc emits the referenced shake handle/name before the unreferenced absorb pair.
static char             data_ov003_020e4980[20] = "Tsk_BtlPsyAbsorb";
static char             data_ov003_020e4994[20] = "Tsk_BtlPsy_FldShake";
const TaskHandle        data_ov003_020dc614     = {data_ov003_020e4980, func_ov003_020a8718, 0};
static const TaskHandle data_ov003_020dc620     = {data_ov003_020e4994, func_ov003_020a8780, sizeof(BtlFieldShake)};

static s32 func_ov003_020a8718(TaskPool* pool, Task* task, void* args, s32 stage) {
    Ov003EffectSlots* slots = &gBattle->effectSlots;
    switch (stage) {
        case 0:
            slots->unk_0C   = 0;
            slots->cooldown = 0;
            break;
        case 1:
            if (slots->cooldown != 0)
                slots->cooldown--;
            break;
    }
    return 1;
}
void func_ov003_020a8768(void) {
    s32 args;
    func_ov003_020a6000(&data_ov003_020dc620, &args);
}
static s32 func_ov003_020a8780(TaskPool* pool, Task* task, void* args, s32 stage) {
    BtlFieldShake* shake;
    switch (stage) {
        default:
        case 0:
            shake = task->data;
            MI_CpuSet(shake, 0, sizeof(*shake));
            CombatCameraShake_Bind(&shake->camera, 0, 0);
            CombatCameraShake_Start(&shake->camera, 3, 0);
            shake->state = 0;
            return 1;
        case 1: {
            BtlFieldShake* shake = task->data;
            s32            active;
            active = 1;
            switch (shake->state) {
                case 0:
                    if (CombatCameraShake_Update(&shake->camera, 0x4000, 0, 0x4000)) {
                        shake->state = 1;
                        shake->timer = 15;
                    }
                    break;
                case 1:
                    CombatCameraShake_Update(&shake->camera, 0, 0, 0x4000);
                    if (--shake->timer == 0)
                        shake->state = 2;
                    break;
                case 2:
                    if (CombatCameraShake_Update(&shake->camera, 0x800, 0, 0x4000))
                        active = 0;
                    break;
            }
            return active;
        }
        case 2:
            return 1;
        case 3:
            shake = task->data;
            CombatCameraShake_Reset(&shake->camera);
            return 1;
    }
}
