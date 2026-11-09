#include "Combat/Core/BtlAura.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatActor.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatAura.h"
#include "Combat/Core/CombatList.h"
#include "Combat/Core/CombatSprite.h"
#include "Engine/EasyTask.h"
#include "Engine/Math/Random.h"
#include "SpriteMgr.h"
#include "main_02026528.h"

// The aura of the player's gauge (`Tsk_BtlAura`): while the partner sync rate is high, flames drift up out of the
// ground below the gauge actor of one screen. The strength of the aura follows the sync rate, a badge can boost it
// and when it flips between the screens the old side's flames burn out first. The task is `CombatSprite`-shaped at the
// front because it drives two big sprites of its own.

struct BtlAura;

typedef void (*BtlAuraState)(struct BtlAura*);

/// One flame: a sprite plus its link in the free or active list.
typedef struct BtlAura {
    /* 0x000 */ CombatSprite       sprites[2]; // the two burst sprites, main screen first
    /* 0x0C0 */ CombatList         free;
    /* 0x0C8 */ CombatList         active;
    /* 0x0D0 */ BtlAuraFlameSprite flames[16];
    /* 0x790 */ s16                cooldown; // frames until the next flame
    /* 0x792 */ s16                reserved792;
    /* 0x794 */ s32                rising;   // the burst is on the way to its screen
    /* 0x798 */ BtlAuraState       state;
    /* 0x79C */ s16                timer;
    /* 0x79E */ s16                phase;
    /* 0x7A0 */ s32                side; // 0 main screen, 1 sub screen
    /* 0x7A4 */ s32                x;
    /* 0x7A8 */ s32                y;
    /* 0x7AC */ s32                intensity; // 0..0x1000
    /* 0x7B0 */ s16                level;
    /* 0x7B2 */ s16                total;
    /* 0x7B4 */ s16                remaining;
    /* 0x7B6 */ s16                reserved7B6;
} BtlAura; // Size: 0x7B8

extern const BinIdentifier data_ov003_020d7800;

static s32 BtlAura_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

const TaskHandle Tsk_BtlAura = {"Tsk_BtlAura", BtlAura_RunTask, sizeof(BtlAura)};

static void func_ov003_02096afc(BtlAura* self);

/// The animation entries of the two burst sprites.
static const SpriteAnimEntry data_ov003_020d7eb8[2];

static void func_ov003_0209609c(BtlAura* self, BtlAuraState state) {
    self->state = state;
    self->phase = 0;
    self->timer = 0;
}

/// Moves `*value` a fraction of the way to `target`; the fraction shrinks with the frames left.
static void func_ov003_020960b4(s32* value, s32 target, u16 frames) {
    u16 count = frames * 3;
    s32 diff  = target - *value;

    if (count < 1) {
        count = 1;
    }
    *value = *value + diff / count;
}

/// The sound that plays when the main screen's aura starts.
static void func_ov003_020960f0(void) {
    if (g_CombatGlobal->unk3D8EE < 500) {
        CombatSe_PlayAtMainX(0x365, ((CombatActor*)g_CombatGlobal->unk3D898)->position.x);
    } else {
        CombatSe_PlayAtMainX(0x366, ((CombatActor*)g_CombatGlobal->unk3D898)->position.x);
    }
}

/// The sound that plays when the sub screen's aura starts, depending on the partner.
static void func_ov003_02096140(void) {
    if (g_CombatGlobal->unk3D8EE < 500) {
        switch (g_CombatGlobal->unk3D88D) {
            case 0:
                CombatSe_PlayAtSubX(0x38D, ((CombatActor*)g_CombatGlobal->unk3D89C)->position.x);
                break;
            case 1:
                CombatSe_PlayAtSubX(0x3B5, ((CombatActor*)g_CombatGlobal->unk3D89C)->position.x);
                break;
            case 2:
                CombatSe_PlayAtSubX(0x3DD, ((CombatActor*)g_CombatGlobal->unk3D89C)->position.x);
                break;
        }
    } else {
        switch (g_CombatGlobal->unk3D88D) {
            case 0:
                CombatSe_PlayAtSubX(0x38E, ((CombatActor*)g_CombatGlobal->unk3D89C)->position.x);
                break;
            case 1:
                CombatSe_PlayAtSubX(0x3B6, ((CombatActor*)g_CombatGlobal->unk3D89C)->position.x);
                break;
            case 2:
                CombatSe_PlayAtSubX(0x3DE, ((CombatActor*)g_CombatGlobal->unk3D89C)->position.x);
                break;
        }
    }
}

/// Starts the aura level at the carried over value, or at 200 plus the badge's bonus.
static void func_ov003_02096230(s16* level) {
    if (g_CombatGlobal->unk3D8ED != 0) {
        *level = g_CombatGlobal->unk3D8EE;
    } else if (CombatAbility_IsActive(0x39) == 0) {
        *level = 0xC8;
    } else {
        *level = CombatAbility_GetValue(0x39) + 0xC8;
    }
    func_ov003_0208cbc8();
}

/// Picks the aura level of the battle from how much of the aura is left.
static void func_ov003_0209628c(BtlAura* self) {
    s32 left = self->total - self->remaining;

    if (left <= self->total * 2 / 100) {
        g_CombatGlobal->unk3D8EE = self->level + 0x32;
    } else if (left <= self->total * 8 / 100) {
        g_CombatGlobal->unk3D8EE = self->level + 0x14;
    } else if (left <= self->total * 30 / 100) {
        g_CombatGlobal->unk3D8EE = self->level + 0xA;
    } else {
        g_CombatGlobal->unk3D8EE = self->level - 0x64;
    }
    if (g_CombatGlobal->unk3D8EE > 500) {
        g_CombatGlobal->unk3D8EE = 500;
    }
    if (g_CombatGlobal->unk3D8EE < 200) {
        g_CombatGlobal->unk3D8EE = 200;
    }
}

/// Starts a flame.
static void func_ov003_020963a0(BtlAura* self, s32 mode) {
    BtlAuraFlameSprite* flame;
    s32                 engine;
    s32                 palette;
    s32                 x;
    s32                 y;
    s16                 anim;
    s32                 sortKey;

    if (self->cooldown > 0) {
        self->cooldown--;
        return;
    }
    flame = CombatList_GetHead(&self->free);
    if (flame != NULL) {
        switch (mode) {
            case 1:
                anim = RNG_Next(5);
                if (self->y < 0xC0000) {
                    engine  = 1;
                    palette = 0x40;
                    x       = self->x + ((RNG_Next(0x11) - 8) << 12);
                    y       = self->y;
                } else {
                    engine  = 0;
                    palette = 0x3E;
                    x       = self->x + ((RNG_Next(0x11) - 8) << 12);
                    y       = self->y - 0xC0000;
                }
                self->cooldown = 2;
                break;
            case 0:
            default:
                anim = RNG_Next(2) + 5;
                if (self->side == 1 && self->y < 0xC0000) {
                    engine  = 1;
                    palette = 0x40;
                    x       = self->x + ((RNG_Next(0x19) - 0xC) << 12);
                    y       = self->y + ((RNG_Next(0x31) - 0x10) << 12);
                }
                if (self->side == 0 && self->y > 0xC0000) {
                    engine  = 0;
                    palette = 0x3E;
                    x       = self->x + ((RNG_Next(0x19) - 0xC) << 12);
                    y       = self->y + ((RNG_Next(0x31) - 0x10) << 12) - 0xC0000;
                }
                self->cooldown = 4;
                break;
        }
        if (engine == 0) {
            sortKey = 0x80000000 - ((CombatActor*)g_CombatGlobal->unk3D898)->position.y;
        } else {
            sortKey = 0x80000000 - ((CombatActor*)g_CombatGlobal->unk3D89C)->position.y;
        }
        CombatSprite_LoadDirectNoProto(engine, &flame->sprite, &data_ov003_020d7800, 0x13, 0x15, 0x14, palette, (u16)anim);
        CombatSprite_SetAnim(&flame->sprite, (u16)anim, 1);
        CombatSprite_SetPosition(&flame->sprite, (x * 0x10) >> 16, (y * 0x10) >> 16);
        func_ov003_02082730(&flame->sprite, sortKey);
        CombatList_Remove(&self->free, &flame->node);
        CombatList_Append(&self->active, &flame->node, flame);
    }
}

/// Returns a finished flame to the free list.
static void func_ov003_020965dc(BtlAura* self, BtlAuraFlameSprite* flame) {
    if (flame != NULL) {
        CombatSprite_Release(&flame->sprite);
        CombatList_Remove(&self->active, &flame->node);
        CombatList_Append(&self->free, &flame->node, flame);
    }
}

/// Puts every flame on the free list.
static void func_ov003_02096614(BtlAura* self) {
    s32                 i;
    BtlAuraFlameSprite* flame;

    CombatList_Init(&self->free);
    CombatList_Init(&self->active);
    self->cooldown = 0;
    flame          = self->flames;
    for (i = 0; i < 16; i++) {
        CombatSprite_Init(&flame->sprite);
        CombatList_Append(&self->free, &flame->node, flame);
        flame++;
    }
}

/// Advances the live flames and retires the finished ones.
static void func_ov003_02096668(BtlAura* self) {
    BtlAuraFlameSprite* flame = CombatList_GetHead(&self->active);
    BtlAuraFlameSprite* next;

    if (flame != NULL) {
        do {
            if (SpriteMgr_IsAnimationFinished(&flame->sprite.sprite)) {
                next = CombatList_GetNext(&flame->node);
                func_ov003_020965dc(self, flame);
                flame = next;
            } else {
                CombatSprite_Update(&flame->sprite);
                flame = CombatList_GetNext(&flame->node);
            }
        } while (flame != NULL);
    }
}

/// Draws the live flames behind the gauge actor of their screen.
static void func_ov003_020966d0(BtlAura* self) {
    BtlAuraFlameSprite* flame = CombatList_GetHead(&self->active);

    if (flame != NULL) {
        do {
            if (flame->sprite.sprite.bits_0_1 == 1) {
                func_ov003_02082730(&flame->sprite, 0x80000000 - ((CombatActor*)g_CombatGlobal->unk3D89C)->position.y);
            } else {
                func_ov003_02082730(&flame->sprite, 0x80000000 - ((CombatActor*)g_CombatGlobal->unk3D898)->position.y);
            }
            CombatSprite_Render(&flame->sprite);
            flame = CombatList_GetNext(&flame->node);
        } while (flame != NULL);
    }
}

/// Releases the flames that are still loaded.
static void func_ov003_02096750(BtlAura* self) {
    s32 i;

    for (i = 0; i < 16; i++) {
        if (Sprite_HasAnimation(&self->flames[i].sprite.sprite)) {
            CombatSprite_Release(&self->flames[i].sprite);
        }
    }
}

/// Follows the gauge actor of `side`.
static void func_ov003_02096788(BtlAura* self, s32 side) {
    if (side == 1) {
        self->x = ((CombatActor*)g_CombatGlobal->unk3D89C)->screenX << 12;
        self->y = (((CombatActor*)g_CombatGlobal->unk3D89C)->screenY - 0x20) << 12;
    } else {
        self->x = ((CombatActor*)g_CombatGlobal->unk3D898)->screenX << 12;
        self->y = (((CombatActor*)g_CombatGlobal->unk3D898)->screenY + 0xA0) << 12;
    }
}

/// Works out how long the aura lasts from the sync rate, the battle index and the badges.
static void func_ov003_0209680c(BtlAura* self) {
    if (!(self->intensity >= 0x1000)) {
        self->intensity = 0x1000;
        self->total     = g_CombatGlobal->unk3D896 * 0x708 / 1000;
        self->total -= (s16)((g_CombatGlobal->unk3D8ED - 1) * 30);
        if (self->total < 0x78) {
            self->total = 0x78;
        }
        if (self->side == 0 && CombatAbility_IsActiveForPlayer(0x3A)) {
            self->total += CombatAbility_GetPlayerValue(0x3A);
        }
        if (self->side == 1 && CombatAbility_IsActiveForPartner(0x3A)) {
            self->total += CombatAbility_GetPartnerValue(0x3A);
        }
        self->remaining = self->total;
    }
}

/// Burns the aura down.
static void func_ov003_02096914(BtlAura* self) {
    if (self->remaining > 0) {
        func_ov003_020960b4(&self->intensity, 0, (u16)self->remaining);
        self->remaining--;
        return;
    }
    self->intensity          = 0;
    self->remaining          = 0;
    g_CombatGlobal->unk3D8ED = 0;
    g_CombatGlobal->unk3D8EE = 100;
    g_CombatGlobal->unk3D8F4 = 0;
    g_CombatGlobal->unk3D8F0 = 0;
}

/// The idle state: reacts to a request to start the aura on one of the screens.
static void func_ov003_020969a4(BtlAura* self) {
    switch (g_CombatGlobal->unk3D8EC) {
        case 1:
            g_CombatGlobal->unk3D8EC = 0;
            func_ov003_02096788(self, 0);
            if (self->intensity <= 0) {
                CombatSe_PlayAtMainX(0x21, self->x);
            }
            if (self->intensity > 0 && self->side != 0) {
                return;
            }
            self->side               = 1;
            g_CombatGlobal->unk3D8F4 = 0;
            func_ov003_02096230(&self->level);
            func_ov003_0209680c(self);
            func_ov003_0209609c(self, func_ov003_02096afc);
            return;
        case 2:
            g_CombatGlobal->unk3D8EC = 0;
            func_ov003_02096788(self, 1);
            if (self->intensity <= 0) {
                CombatSe_PlayAtSubX(0x21, self->x);
            }
            if (self->intensity > 0 && self->side != 1) {
                return;
            }
            self->side               = 0;
            g_CombatGlobal->unk3D8F0 = 0;
            func_ov003_02096230(&self->level);
            func_ov003_0209680c(self);
            func_ov003_0209609c(self, func_ov003_02096afc);
            return;
        default:
            func_ov003_0209628c(self);
            func_ov003_02096914(self);
            func_ov003_02096788(self, self->side);
            if (self->intensity <= 0x14) {
                return;
            }
            func_ov003_020963a0(self, 0);
            return;
    }
}

/// The aura moves to the other screen and bursts there.
static void func_ov003_02096afc(BtlAura* self) {
    s32 x;
    s32 y;

    if (self->side == 1) {
        CombatActor* actor = g_CombatGlobal->unk3D89C;

        y = actor->screenY - 0x20;
        x = actor->screenX;
    } else {
        CombatActor* actor = g_CombatGlobal->unk3D898;

        y = actor->screenY + 0xA0;
        x = actor->screenX;
    }
    x <<= 12;
    y <<= 12;
    switch (self->phase) {
        case 0:
            if (self->timer == 0) {
                self->rising = 1;
                self->timer  = 0x5A;
                if (CombatAbility_IsActive(0x3B)) {
                    self->timer = self->timer - (s16)CombatAbility_GetValue(0x3B);
                }
                CombatSprite_SetAnimFromTable(&self->sprites[0], 0, 0);
                CombatSprite_SetAnimFromTable(&self->sprites[1], 0, 0);
                self->cooldown = 2;
            }
            if (self->timer > 0) {
                func_020265d4(&self->x, x, (u16)self->timer);
                func_020265d4(&self->y, y, self->timer);
                self->timer--;
            }
            func_ov003_020963a0(self, 1);
            if (self->timer > 0) {
                return;
            }
            self->phase++;
            self->timer = 0;
            return;
        case 1:
            if (self->timer == 0) {
                self->timer++;
                func_ov003_020974f8(x, y);
            }
            if (self->side == 1) {
                g_CombatGlobal->unk3D8F0 = 1;
                func_ov003_02096140();
            } else {
                g_CombatGlobal->unk3D8F4 = 1;
                func_ov003_020960f0();
            }
            self->x = x;
            self->y = y;
            CombatSprite_SetAnimFromTable(&self->sprites[0], 1, 0);
            CombatSprite_SetAnimFromTable(&self->sprites[1], 1, 0);
            self->rising = 0;
            func_ov003_0209609c(self, func_ov003_020969a4);
            return;
    }
}

static s32 BtlAura_Init(TaskPool* pool, Task* task, void* args) {
    BtlAura* self = task->data;

    self->x                  = 0;
    self->y                  = 0;
    self->intensity          = 0;
    self->remaining          = 0;
    self->side               = 1;
    g_CombatGlobal->unk3D8EE = 100;
    g_CombatGlobal->unk3D8ED = 0;
    g_CombatGlobal->unk3D8F0 = 0;
    g_CombatGlobal->unk3D8F4 = 0;
    CombatSprite_LoadFromTableNoProto(1, &self->sprites[0], &data_ov003_020d7800, data_ov003_020d7eb8, 0, 0x40, 0x40);
    CombatSprite_LoadFromTableNoProto(0, &self->sprites[1], &data_ov003_020d7800, data_ov003_020d7eb8, 0, 0x3E, 0x40);
    CombatSprite_SetAnimFromTable(&self->sprites[0], 1, 0);
    CombatSprite_SetAnimFromTable(&self->sprites[1], 1, 0);
    func_ov003_02096614(self);
    self->rising = 0;
    func_ov003_0209609c(self, func_ov003_020969a4);
    return 1;
}

static s32 BtlAura_Update(TaskPool* pool, Task* task, void* args) {
    BtlAura* self = task->data;

    if (g_CombatGlobal->unk3D878 & 0x40) {
        return 1;
    }
    if (self->state != NULL) {
        self->state(self);
    }
    CombatSprite_Update(&self->sprites[0]);
    CombatSprite_Update(&self->sprites[1]);
    func_ov003_02096668(self);
    return 1;
}

static s32 BtlAura_Render(TaskPool* pool, Task* task, void* args) {
    BtlAura* self = task->data;

    if (self->intensity > 0x14) {
        if (self->rising != 0 || self->side == 1) {
            if (self->y < 0xD8000) {
                CombatActor* actor = g_CombatGlobal->unk3D89C;

                CombatSprite_SetAffineTransform(&self->sprites[0], 0, self->intensity, self->intensity, 1);
                CombatSprite_SetPosition(&self->sprites[0], (self->x * 0x10) >> 16, (self->y * 0x10) >> 16);
                func_ov003_02082730(&self->sprites[0], 0x80000001 - actor->position.y);
                CombatSprite_Render(&self->sprites[0]);
            }
        }
        if (self->rising != 0 || self->side == 0) {
            if (self->y > 0xA8000) {
                CombatActor* actor = g_CombatGlobal->unk3D898;

                CombatSprite_SetAffineTransform(&self->sprites[1], 0, self->intensity, self->intensity, 1);
                CombatSprite_SetPosition(&self->sprites[1], (self->x * 0x10) >> 16, (s16)((self->y >> 12) - 0xC0));
                func_ov003_02082730(&self->sprites[1], 0x80000001 - actor->position.y);
                CombatSprite_Render(&self->sprites[1]);
            }
        }
    }
    func_ov003_020966d0(self);
    return 1;
}

static s32 BtlAura_Release(TaskPool* pool, Task* task, void* args) {
    BtlAura* self = task->data;

    CombatSprite_Release(&self->sprites[0]);
    CombatSprite_Release(&self->sprites[1]);
    func_ov003_02096750(self);
    return 1;
}

static s32 BtlAura_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlAura_Init,
        .update     = BtlAura_Update,
        .render     = BtlAura_Render,
        .cleanup    = BtlAura_Release,
    };

    if (func_ov003_0208b690(stage)) {
        return 1;
    }
    return stages.iter[stage](pool, task, args);
}

static const SpriteAnimEntry data_ov003_020d7eb8[2] = {
    {16, 18, 17, 0},
    {16, 18, 17, 2},
};
