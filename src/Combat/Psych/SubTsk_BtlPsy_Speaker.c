#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatPsychEffects.h"
#include <nitro/mi/cpumem.h>

static const s32   data_ov003_020dd074[2];
static const Vec2p data_ov003_020dd088[6];
static const Vec2p data_ov003_020dd0b8[8];
static const Vec2p data_ov003_020dd0f8[9];
static const Vec2p data_ov003_020dd140[9];
static const Vec2p data_ov003_020dd188[9];

static const TaskHandle data_ov003_020dd07c;

s32 func_ov003_020b9be0(Ov003EffectState* effect, s32 x, s32 y, s32 kind, const s32* phase) {
    Ov003SpeakerArgs args;
    args.effect = effect;
    args.x      = x;
    args.y      = y;
    args.kind   = kind;
    args.phase  = phase;
    return func_ov003_020a6000(&data_ov003_020dd07c, &args);
}

// Nonmatching: phase-2 conditional execution and sprite-loop register allocation differ.
s32 func_ov003_020b9c18(TaskPool* pool, Task* task, void* args, s32 stage) {
    Ov003SpeakerArgs* arg = args;
    Ov003Speaker*     self;
    switch (stage) {
        default:
        case 0:
            self = task->data;
            MI_CpuSet(self, 0, sizeof(*self));
            self->effect      = arg->effect;
            self->position.x  = arg->x;
            self->position.y  = arg->y;
            self->position.z  = 0;
            self->kind        = arg->kind;
            self->parentPhase = arg->phase;
            for (s32 i = 0; i < 3; i++)
                self->scales[i] = NULL;
            self->effect->unk_0A++;
            self->phase = 0;
            CombatPsyAnim_Load(&self->sprites[0], 2, 1, data_ov003_020dd074[self->kind], 0);
            self->frame = 0;
            return 1;
        case 1:
            self = task->data;
            switch (self->phase) {
                case 0:
                    if (self->frame == 8) {
                        self->scales[0] = NULL;
                        self->phase     = 1;
                        CombatPsyAnim_SetAnim(&self->sprites[0], data_ov003_020dd074[self->kind] + 1, 0);
                        if (self->kind == 0) {
                            CombatPsyAnim_Load(&self->sprites[1], 2, 1, 0xD7, 0);
                            CombatPsyAnim_Load(&self->sprites[2], 2, 1, 0xD8, 0);
                        }
                    } else {
                        self->scales[0] = &data_ov003_020dd0b8[self->frame];
                        self->frame++;
                    }
                    break;
                case 1:
                    switch (*self->parentPhase) {
                        case 1:
                            self->phase = 2;
                            self->frame = 0;
                            CombatPsyAnim_SetAnim(&self->sprites[0], data_ov003_020dd074[self->kind] + 2, 0);
                            break;
                        case 2:
                            self->phase = 3;
                            self->frame = 0;
                            break;
                    }
                    break;
                case 2:
                    if (*self->parentPhase == 2) {
                        for (s32 i = 0; i < 3; i++)
                            self->scales[i] = NULL;
                        self->phase = 3;
                        self->frame = 0;
                        break;
                    }
                    if (self->kind != 0) {
                        self->scales[0] = &data_ov003_020dd0f8[self->frame];
                    } else {
                        self->scales[1] = &data_ov003_020dd140[self->frame];
                        self->scales[2] = &data_ov003_020dd188[self->frame];
                    }
                    self->frame++;
                    if (self->frame >= 9)
                        self->frame = 0;
                    break;
                case 3:
                    if (self->frame == 6)
                        return 0;
                    if (self->frame == 2) {
                        CombatPsyAnim_SetAnim(&self->sprites[0], data_ov003_020dd074[self->kind], 0);
                        if (self->kind == 0) {
                            BtlSprite_Release(&self->sprites[1]);
                            BtlSprite_Release(&self->sprites[2]);
                        }
                    }
                    self->scales[0] = &data_ov003_020dd088[self->frame];
                    self->frame++;
                    break;
            }
            for (s32 i = 0; i < 3; i++) {
                if (Sprite_HasAnimation(&self->sprites[i].sprite))
                    BtlSprite_Update(&self->sprites[i]);
            }
            return 1;
        case 2: {
            self           = task->data;
            s32 projectedX = CombatStage_ToScreenX(0, self->position.x);
            s32 projectedY = CombatStage_ToScreenY(0, self->position.y, self->position.z);
            s16 x          = projectedX;
            s16 y          = projectedY;
            s16 topY       = projectedY - 24;
            for (s32 i = 0; i < 3; i++) {
                if (Sprite_HasAnimation(&self->sprites[i].sprite)) {
                    switch (i) {
                        case 1:
                        case 2:
                            BtlSprite_SetPosition(&self->sprites[i], x, topY);
                            break;
                        default:
                            BtlSprite_SetPosition(&self->sprites[i], x, y);
                            break;
                    }
                    if (self->scales[i] != NULL)
                        BtlSprite_SetAffineTransform(&self->sprites[i], 0, self->scales[i]->x, self->scales[i]->y, 0);
                    else
                        BtlSprite_SetAffineTransform(&self->sprites[i], 0, 0x1000, 0x1000, 0);
                    BtlSprite_SetSortKey(&self->sprites[i], self->position.y + i * 0x1000 + 0x1F4000);
                    BtlSprite_Render(&self->sprites[i]);
                }
            }
            return 1;
        }
        case 3:
            self = task->data;
            for (s32 i = 0; i < 3; i++) {
                if (Sprite_HasAnimation(&self->sprites[i].sprite))
                    BtlSprite_Release(&self->sprites[i]);
            }
            self->effect->unk_0A--;
            return 1;
    }
}

static char             data_ov003_020e5014[24] = "SubTsk_BtlPsy_Speaker";
static const s32        data_ov003_020dd074[2]  = {0xD4, 0xD9};
static const TaskHandle data_ov003_020dd07c     = {data_ov003_020e5014, func_ov003_020b9c18, sizeof(Ov003Speaker)};
static const Vec2p      data_ov003_020dd088[6]  = {
    {0x1000, 0x1000},
    {0x1000, 0x1000},
    {0x1000, 0x1000},
    {0x1000, 0x1000},
    { 0x800,  0x800},
    { 0x800,  0x800}
};

static const Vec2p data_ov003_020dd0b8[8] = {
    { 0x800,  0x800},
    { 0x800,  0x800},
    {0x2000,  0x4CD},
    {0x2000,  0x4CD},
    {0x1666, 0x1CCD},
    {0x1666, 0x1CCD},
    {0x1000, 0x1000},
    {0x1000, 0x1000}
};

static const Vec2p data_ov003_020dd0f8[9] = {
    {0x1000, 0x1000},
    {0x1000, 0x1000},
    {0x1000, 0x1000},
    {0x1333, 0x1000},
    {0x1333, 0x1000},
    {0x1333, 0x1000},
    {0x1000, 0x1333},
    {0x1000, 0x1333},
    {0x1000, 0x1333}
};

static const Vec2p data_ov003_020dd140[9] = {
    {0x1000, 0x1000},
    {0x1000, 0x1000},
    {0x1000, 0x1000},
    {0x1333, 0x1333},
    {0x1333, 0x1333},
    {0x1333, 0x1333},
    {0x1333, 0x1333},
    {0x1333, 0x1333},
    {0x1333, 0x1333}
};

static const Vec2p data_ov003_020dd188[9] = {
    {0x1000, 0x1000},
    {0x1000, 0x1000},
    {0x1000, 0x1000},
    {0x1000, 0x1000},
    {0x1000, 0x1000},
    {0x1000, 0x1000},
    {0x1000, 0x1000},
    {0x1333, 0x1333},
    {0x1333, 0x1333}
};
