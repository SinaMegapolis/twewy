#include "Combat/Core/Combat.h"
#include "Combat/Noise/Private/BtlEnm026.h"
#include "SndMgr.h"

#include "Combat/Core/CombatApi.h"
#include <nitro/mi/cpumem.h>

typedef struct BtlEnm026Icon {
    /* 0x000 */ BattleSprite unk_00;
    /* 0x060 */ BattleSprite unk_60;
    /* 0x0C0 */ BattleSprite unk_C0;
    /* 0x120 */ BtlEnm026*   unk_120;
    /* 0x124 */ s16          unk_124;
    /* 0x126 */ u16          unk_126;
    /* 0x128 */ void*        unk_128;
    /* 0x12C */ s16          unk_12C;
    /* 0x12E */ u16          unk_12E;
    /* 0x130 */ s32          unk_130;
} BtlEnm026Icon; // Size: 0x134
static s32  func_ov015_02126860(TaskPool* pool, Task* task, void* args, s32 stage);
static char data_ov015_0212846c[20];
static char data_ov015_02128480[32];
static char data_ov015_021284a0[32];

static const BinIdentifier data_ov015_02128228 = {3, data_ov015_021284a0};

static const BinIdentifier data_ov015_02128230 = {3, data_ov015_02128480};

static const SpriteAnimEntry data_ov015_02128238[13] = {
    {0x1, 0x3, 0x2, 0x0},
    {0x1, 0x3, 0x2, 0x1},
    {0x1, 0x3, 0x2, 0x2},
    {0x1, 0x3, 0x2, 0x3},
    {0x1, 0x3, 0x2, 0x4},
    {0x1, 0x3, 0x2, 0x5},
    {0x1, 0x3, 0x2, 0x6},
    {0x1, 0x3, 0x2, 0x7},
    {0x1, 0x3, 0x2, 0x8},
    {0x1, 0x3, 0x2, 0x9},
    {0x1, 0x3, 0x2, 0xA},
    {0x1, 0x3, 0x2, 0xB},
    {0x1, 0x3, 0x2, 0xC},
};

const TaskHandle Tsk_BtlEnm026_Icon = {data_ov015_0212846c, func_ov015_02126860, 0x134};

static char data_ov015_0212846c[20] = "Tsk_BtlEnm026_Icon";

static char data_ov015_02128480[32] = "Apl_Hor/Grp_BtlEnm026_Bdg.bin";

static char data_ov015_021284a0[32] = "Apl_Hor/Grp_BtlEnm026_Icon.bin";

static s32 func_ov015_021261d8(BtlEnm026Icon* data, Enm026IconSpawn* args) {
    BtlEnm026* owner = args->unk_00;
    s32        r5    = 0;

    MI_CpuSet(data, 0, sizeof(BtlEnm026Icon));
    data->unk_120 = args->unk_00;
    data->unk_124 = args->unk_04;
    data->unk_128 = args->unk_08;

    switch (args->unk_04) {
        case 0: {
            SpriteAnimationEx anim;
            BtlSprite_InitAnim(&anim.anim, r5, &data_ov015_02128230);
            r5                   = 0x11;
            anim.anim.unk_18     = 2;
            anim.anim.unk_26     = 2;
            anim.anim.packIndex  = data_ov015_02128500.unk_28 + 1;
            anim.anim.bits_10_11 = 0;
            anim.anim.unk_28     = 3;
            anim.anim.unk_1C     = 1;
            anim.anim.unk_20     = 4;
            anim.anim.animIndex  = 1;
            anim.animTable       = NULL;
            BtlSprite_Load(&data->unk_60, &anim);
            BtlSprite_LoadFromTable(owner->base.sprite.sprite.bits_0_1, &data->unk_C0, &data_ov015_02128228,
                                    data_ov015_02128238, 3, 4, 0x10);
            break;
        }

        case 1:
            BtlSprite_LoadFromTable(owner->base.sprite.sprite.bits_0_1, &data->unk_60, &data_ov015_02128228,
                                    data_ov015_02128238, 3, 4, 2);
            r5 = 0x10;
            BtlSprite_SetAnimFromTable(&data->unk_60, 3, 0);
            BtlSprite_LoadFromTable(owner->base.sprite.sprite.bits_0_1, &data->unk_C0, &data_ov015_02128228,
                                    data_ov015_02128238, 3, 4, 2);
            BtlSprite_SetAnimFromTable(&data->unk_C0, 3, 0);
            break;

        case 2:
            r5 = 4;
            BtlSprite_LoadFromTable(owner->base.sprite.sprite.bits_0_1, &data->unk_60, &data_ov015_02128228,
                                    data_ov015_02128238, 3, 4, 0);
            BtlSprite_LoadFromTable(owner->base.sprite.sprite.bits_0_1, &data->unk_C0, &data_ov015_02128228,
                                    data_ov015_02128238, 3, 4, 0);
            break;
    }

    BtlSprite_LoadFromTable(owner->base.sprite.sprite.bits_0_1, &data->unk_00, &data_ov015_02128228, data_ov015_02128238,
                            data->unk_124, 4, r5);
    BtlSprite_SetAnimFromTable(&data->unk_00, data->unk_124, data->unk_124 == 2);
    if (data->unk_124 != 2) {
        BtlSprite_SetFlip(&data->unk_00, owner->base.sprite.flags & BTL_SPRITE_FLAG_FLIPPED);
    }
    return 1;
}

static s32 func_ov015_02126464(BtlEnm026Icon* data) {
    BtlEnm026* owner = data->unk_120;

    if (data->unk_124 != 2) {
        BtlSprite_SetFlip(&data->unk_00, owner->base.sprite.flags & BTL_SPRITE_FLAG_FLIPPED);
    }
    if (owner->base.actor.flags & ACTOR_FLAG_DEFEATED) {
        return 0;
    }
    switch (data->unk_00.animTableIndex) {
        case 0:
            BtlSprite_Update(&data->unk_60);
            break;

        case 1: {
            s16 hp = *(s16*)data->unk_128;
            if (hp > 0x63) {
                hp = 0x63;
            }
            if (data->unk_130 == 0) {
                if (g_CombatGlobal->unk3D874 == 2) {
                    if (hp == 0) {
                        if (SndMgr_IsSEPlaying(SEIDX_SE_026_TIMER0) == 0) {
                            CombatSe_Play(SEIDX_SE_026_TIMER0);
                        }
                        data->unk_130 = 1;
                    } else if (data->unk_12C != hp) {
                        if (SndMgr_IsSEPlaying(SEIDX_SE_026_TIMER) == 0) {
                            CombatSe_Play(SEIDX_SE_026_TIMER);
                        }
                    }
                }
            }
            data->unk_12C = hp;
            BtlSprite_SetAnimFromTable(&data->unk_60, hp % 10 + 3, 0);
            {
                s32 tens;
                if (hp < 0xA) {
                    tens = 0;
                } else {
                    tens = hp / 10;
                }
                BtlSprite_SetAnimFromTable(&data->unk_C0, tens + 3, 0);
                if (tens > 0) {
                    BtlSprite_Update(&data->unk_C0);
                }
            }
            BtlSprite_Update(&data->unk_60);
            break;
        }

        case 2:
            if (SpriteMgr_IsAnimationFinished(&data->unk_00.sprite) == 0) {
                break;
            }
            BtlSprite_Restart(&data->unk_00);
            if (SndMgr_IsSEPlaying(SEIDX_SE_026_VOC_IBIKI) == 0) {
                if (data_ov015_02128500.flag08 == 0) {
                    if (g_CombatGlobal->unk3D874 == 2) {
                        CombatSe_Play(SEIDX_SE_026_VOC_IBIKI);
                    }
                }
            }
            break;
    }
    BtlSprite_Update(&data->unk_00);
    return 1;
}

static s32 func_ov015_02126660(BtlEnm026Icon* data) {
    BtlEnm026* owner  = data->unk_120;
    s32        engine = (func_ov003_020c37f8(&data->unk_00) != 0);
    s32        px;
    s32        pz;
    s32        py;
    s16        sx;
    s16        sy;

    if (func_ov003_020c37f8(&data->unk_00) != 0) {
        if (owner->base.flags18C & 0x10) {
            return 1;
        }
    }
    px = owner->base.actor.position.x + ((owner->base.sprite.flags & BTL_SPRITE_FLAG_FLIPPED) ? 0xF000 : -0xF000);
    pz = owner->base.actor.position.z - 0x19000;
    py = owner->base.actor.position.y;

    switch (data->unk_00.animTableIndex) {
        case 0:
            BtlStage_WorldToScreen(engine, &sx, &sy, px, py, pz);
            BtlSprite_SetPosition(&data->unk_60, sx, sy);
            BtlSprite_SetSortKey(&data->unk_60, 0x7FFFFFFD - py);
            BtlSprite_Render(&data->unk_60);
            break;

        case 1:
            if (data->unk_C0.animTableIndex != 3) {
                BtlStage_WorldToScreen(engine, &sx, &sy, px - 0x4000, py, pz);
                BtlSprite_SetPosition(&data->unk_C0, sx, sy);
                BtlSprite_SetSortKey(&data->unk_C0, 0x7FFFFFFD - py);
                BtlSprite_Render(&data->unk_C0);
                BtlStage_WorldToScreen(engine, &sx, &sy, px + 0x4000, py, pz);
                BtlSprite_SetPosition(&data->unk_60, sx, sy);
                BtlSprite_SetSortKey(&data->unk_60, 0x7FFFFFFD - py);
                BtlSprite_Render(&data->unk_60);
            } else {
                BtlStage_WorldToScreen(engine, &sx, &sy, px, py, pz);
                BtlSprite_SetPosition(&data->unk_60, sx, sy);
                BtlSprite_SetSortKey(&data->unk_60, 0x7FFFFFFD - py);
                BtlSprite_Render(&data->unk_60);
            }
            break;
    }

    BtlStage_WorldToScreen(engine, &sx, &sy, px, py, pz);
    BtlSprite_SetPosition(&data->unk_00, sx, sy);
    BtlSprite_SetSortKey(&data->unk_00, 0x7FFFFFFE - py);
    BtlSprite_Render(&data->unk_00);
    return 1;
}

static s32 func_ov015_0212683c(BtlEnm026Icon* data) {
    BtlSprite_Release(&data->unk_00);
    BtlSprite_Release(&data->unk_60);
    BtlSprite_Release(&data->unk_C0);
    return 1;
}

static s32 func_ov015_02126860(TaskPool* pool, Task* task, void* args, s32 stage) {
    BtlEnm026Icon* data = task->data;

    switch (stage) {
        case 0:
            return func_ov015_021261d8(data, (Enm026IconSpawn*)args);
        case 1:
            return func_ov015_02126464(data);
        case 2:
            return func_ov015_02126660(data);
        case 3:
            return func_ov015_0212683c(data);
    }
    return 1;
}
