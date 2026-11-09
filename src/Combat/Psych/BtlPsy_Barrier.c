#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatPsychEffects.h"
#include <nitro/mi/cpumem.h>

typedef struct BtlBarrierCharge {
    Ov003BarrierArgs args;
    s32              timer;
} BtlBarrierCharge; // Size: 0x14

static const TaskHandle data_ov003_020dc6c4;
static s32 func_ov003_020aaad4(Ov003EffectContext* context, Ov003EffectState* effect, s32 automatic, s32 mode, s32 kind);

s32 func_ov003_020aaa84(Ov003EffectContext* context, Ov003EffectState* effect) {
    return func_ov003_020aaad4(context, effect, 0, 0, 0);
}
s32 func_ov003_020aaa9c(Ov003EffectContext* context, Ov003EffectState* effect) {
    return func_ov003_020aaad4(context, effect, 0, 0, 1);
}
s32 func_ov003_020aaab8(Ov003EffectContext* context, Ov003EffectState* effect) {
    return func_ov003_020aaad4(context, effect, 0, 1, 2);
}

static s32 func_ov003_020aaad4(Ov003EffectContext* context, Ov003EffectState* effect, s32 automatic, s32 mode, s32 kind) {
    BattleTouchWork* touch = func_ov003_020a62a0();
    if (func_ov003_020a5b18(0x17) || func_ov003_020a5b18(0x18) || func_ov003_020a5b18(0x19))
        return 0;
    if (automatic == 0) {
        if (!BtlTouch_IsGesture(BTL_TOUCH_GESTURE_DRAG))
            return 0;
        if (touch->held != func_ov003_020a6438())
            return 0;
    }
    Ov003BarrierArgs* arg = func_ov003_020a5d2c(sizeof(*arg), data_ov003_020dc6c4.taskName);
    arg->spawn.effect     = effect;
    arg->automatic        = automatic;
    arg->spawn.mode       = mode;
    arg->spawn.kind       = kind;
    context->handle       = &data_ov003_020dc6c4;
    context->arg          = arg;
    context->post         = NULL;
    return 1;
}

static s32 func_ov003_020aaba0(TaskPool* pool, Task* task, void* args, s32 stage) {
    Ov003BarrierArgs* arg = args;
    BtlBarrierCharge* charge;
    switch (stage) {
        default:
        case 0:
            charge = task->data;
            MI_CpuSet(charge, 0, sizeof(*charge));
            charge->args.spawn.effect = arg->spawn.effect;
            charge->args.spawn.mode   = arg->spawn.mode;
            charge->args.spawn.kind   = arg->spawn.kind;
            charge->args.automatic    = arg->automatic;
            charge->timer             = 0;
            charge->args.spawn.effect->unk_0A++;
            if (charge->args.automatic == 0) {
                func_ov003_020a6464(1);
                func_ov003_020a64ac();
            }
            return 1;
        case 1: {
            BtlBarrierCharge* charge = task->data;
            func_ov003_020a6438();
            BattleTouchWork* touch = func_ov003_020a62a0();
            func_ov003_020a5a94(charge->args.spawn.effect);
            if (func_ov003_020a63a8(charge->args.spawn.effect) || !BtlTouch_IsGesture(BTL_TOUCH_GESTURE_DRAG) ||
                touch->held != func_ov003_020a6438())
            {
                func_ov003_020a6464(0);
                return 0;
            }
            charge->timer++;
            if (charge->timer < 60)
                return 1;
            func_ov003_020aa32c(charge->args.spawn.effect, charge->args.spawn.mode, charge->args.spawn.kind);
            return 0;
        }
        case 2:
            return 1;
        case 3:
            charge = task->data;
            charge->args.spawn.effect->unk_0A--;
            return 1;
    }
}

static char             data_ov003_020e4a70[20] = "Tsk_BtlPsy_Barrier";
static const TaskHandle data_ov003_020dc6c4     = {data_ov003_020e4a70, func_ov003_020aaba0, sizeof(BtlBarrierCharge)};
