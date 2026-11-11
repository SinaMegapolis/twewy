#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatPsychEffects.h"
#include "Engine/Math/Random.h"
#include <nitro/math.h>
#include <nitro/mi/cpumem.h>

// Retail names the first record's fields as separate objects. Keep both
// records in one bounded array; the interior aliases still need matching work.
static const Ov003PsychoFallKind data_ov003_020dc9f0[2] = {
    {100, 2, 0, 0, 333, 2, 0x3000, 0},
    { 94, 4, 0, 1, 334, 4, 0x4000, 1},
};

static const s32 data_ov003_020dc9c0[4];
static const s32 data_ov003_020dc9d0[2][4];

static void func_ov003_020b4788(Ov003PsychoFall* self);

static const TaskHandle data_ov003_020dc9b4;

s32 func_ov003_020b42b4(Ov003EffectState* effect, s32 index, s32 x, s32 y, s32 z, s32 zStep) {
    Ov003PsychoFallArgs args;
    args.effect = effect;
    args.index  = index;
    args.x      = x;
    args.y      = y;
    args.z      = z;
    args.zStep  = zStep;
    return func_ov003_020a6000(&data_ov003_020dc9b4, &args);
}

// Nonmatching: only the literal-pool relocation names differ (see the alias note above).
static s32 func_ov003_020b42f4(TaskPool* pool, Task* task, void* args, s32 stage) {
    Ov003PsychoFallArgs* arg = args;
    Ov003PsychoFall*     self;
    switch (stage) {
        default:
        case 0: {
            self = task->data;
            MI_CpuSet(self, 0, sizeof(*self));
            self->effect = arg->effect;
            self->phase  = 0;
            self->index  = arg->index;
            self->zStep  = arg->zStep;
            self->x      = arg->x;
            self->y      = arg->y;
            self->z = self->baseZ = arg->z;
            self->variant         = 0;
            BtlStage_ClampToFloor(&self->x, &self->y, 0, 0);
            CombatPsyAnim_Load(&self->sprite, 2, 1, data_ov003_020dc9f0[self->index].animation, 0);
            BtlHit_LoadSpark(0, &self->icon, self->variant);
            BtlSe_PlayAtMainX(0x35, self->x);
            return 1;
        }
        case 1: {
            Ov003PsychoFall* self = task->data;
            switch (self->phase) {
                case 0:
                    self->z += self->zStep;
                    if (self->z >= 0) {
                        BtlSe_PlayAtMainX(0x36, self->x);
                        self->z     = 0;
                        self->phase = 1;
                        CombatPsyAnim_SetAnim(&self->sprite, data_ov003_020dc9f0[self->index].animation + 1, 0);
                        Ov003PsychEffectArgs effect;
                        func_ov003_020a88c0(&effect, self->x, self->y - 0x1000, self->z, 2, 0x5D);
                        if (self->index == 0)
                            func_ov003_020a89c8(&effect, 0, 0x800, 0x800);
                        func_ov003_020a88a8(&effect);
                        func_ov003_020a88c0(&effect, self->x, self->y + 0x1000, self->z, 2, 0x5C);
                        if (self->index == 0)
                            func_ov003_020a89c8(&effect, 0, 0x800, 0x800);
                        func_ov003_020a88a8(&effect);
                        if (data_ov003_020dc9f0[self->index].shake)
                            func_ov003_020a8768();
                    } else {
                        self->zStep += 0x333;
                    }
                    break;
                case 1: {
                    // The declaration order and split initialisers here are load-bearing: they
                    // decide how mwcc hoists the two loop-invariant table loads and allocates
                    // the loop registers.
                    s32 angle;
                    s32 rand = RNG_Next(4);
                    s32 trail;
                    s32 radiusBase;
                    s32 i;
                    radiusBase = data_ov003_020dc9f0[self->index].radiusBase;
                    trail      = data_ov003_020dc9f0[self->index].trail;
                    for (i = 0; i < data_ov003_020dc9f0[self->index].particleCount; i++) {
                        s32 tilt;
                        s32 radius;
                        angle  = data_ov003_020dc9c0[(rand + i) % 4] + RNG_Next(0x200) - 0x100;
                        radius = RNG_Next(3) * 0x19A + radiusBase;
                        tilt   = 0x400;
                        if ((rand + i) % 2)
                            tilt = -0x400;
                        func_ov003_020b4048(
                            self->effect, self->x, self->y, self->z, FX_Mul(radius, COS2(FX_SinCosTable_, angle)), tilt,
                            -FX_Mul(radius, SIN2(FX_SinCosTable_, angle)), data_ov003_020dc9d0[self->index][i], trail);
                    }
                    self->phase = 2;
                    break;
                }
                case 2:
                    return 0;
            }
            func_ov003_020a6628(&self->effect->streak, (u16)data_ov003_020dc9f0[self->index].attackId, self->x, self->y,
                                self->z, 0, 0);
            func_ov003_020b4788(self);
            BtlSprite_Update(&self->sprite);
            BtlSprite_Update(&self->icon);
            return 1;
        }
        case 2: {
            Ov003PsychoFall* self = task->data;
            s32              y    = BtlStage_ToScreenY(0, self->y, self->z);
            BtlSprite_SetPosition(&self->sprite, BtlStage_ToScreenX(0, self->x), y);
            BtlSprite_SetSortKey(&self->sprite, self->y + 0x1F4000);
            BtlSprite_Render(&self->sprite);
            BtlHit_DrawSpark(&self->icon, self->x, self->y);
            return 1;
        }
        case 3: {
            Ov003PsychoFall* self = task->data;
            BtlSprite_Release(&self->sprite);
            BtlSprite_Release(&self->icon);
            return 1;
        }
    }
}

/// Re-picks the impact icon's frame from the shard's remaining fall distance.
// Nonmatching: the pool load names data_ov003_020dc9f4, an interior alias of the table above.
static void func_ov003_020b4788(Ov003PsychoFall* self) {
    u16 frames = data_ov003_020dc9f0[self->index].frames;
    u16 frame  = frames - (FX_Mul(frames << 12, FX_Divide(-self->z, -self->baseZ)) >> 12);

    if (frame == self->variant)
        return;
    BtlSprite_SetAnim(&self->icon, frame, 1);
}
static char             data_ov003_020e4e50[28]   = "SubTsk_BtlPsy_PsychoFall_T";
static const TaskHandle data_ov003_020dc9b4       = {data_ov003_020e4e50, func_ov003_020b42f4, sizeof(Ov003PsychoFall)};
static const s32        data_ov003_020dc9c0[4]    = {0x2800, 0x3800, 0x4800, 0x5800};
static const s32        data_ov003_020dc9d0[2][4] = {
    {0x67, 0x67, 0x67, 0x67},
    {0x61, 0x63, 0x65, 0x67}
};
