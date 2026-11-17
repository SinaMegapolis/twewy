#include "Battle/Core/BtlSprite.h"
#include "Combat/Core/BtlAura.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Fusion/BtlFusion.h"
#include "Engine/EasyTask.h"
#include "Engine/Math/Random.h"
#include "SpriteMgr.h"
#include "main_02026528.h"

static s32  func_ov007_020e7c98(TaskPool* pool, Task* task, void* args);
static s32  func_ov007_020e7d48(TaskPool* pool, Task* task, void* args);
static s32  func_ov007_020e7da4(TaskPool* pool, Task* task, void* args);
static s32  func_ov007_020e7eb0(TaskPool* pool, Task* task, void* args);
static s32  func_ov007_020e7ee0(TaskPool* pool, Task* task, void* args, s32 stage);
static void func_ov007_020e7b54(BtlAuraLast*);

const TaskHandle Tsk_BtlAuraLast = {"Tsk_BtlAuraLast", func_ov007_020e7ee0, 0x810};

static const TaskStages data_ov007_020e7f70 = {
    .initialize = func_ov007_020e7c98,
    .update     = func_ov007_020e7d48,
    .render     = func_ov007_020e7da4,
    .cleanup    = func_ov007_020e7eb0,
};

static const SpriteAnimEntry data_ov007_020e7f80[2] = {
    {0x10, 0x12, 0x11, 0},
    {0x10, 0x12, 0x11, 2},
};

static void func_ov007_020e75e4(BtlAuraLast* data, void (*callback)(BtlAuraLast*)) {
    data->unk_7F4 = callback;
    data->unk_7FA = 0;
    data->unk_7F8 = 0;
}

static void func_ov007_020e75fc(s32 arg0, BattleSprite* cSprite, u16 arg2, u16 arg3) {
    BtlSprite_LoadFromTable(arg0, cSprite, &Iden_BtlMenu, data_ov007_020e7f80, 0, arg3, 0x40);
    BtlSprite_SetAnimFromTable(cSprite, arg2, 0);
}

static void func_ov007_020e764c(void) {
    if (gBattle->auraRate < 0x1F4) {
        BtlSe_PlayAtMainX(0x365, gBattle->player->position.x);
    } else {
        BtlSe_PlayAtMainX(0x366, gBattle->player->position.x);
    }
}

static void func_ov007_020e769c(s16* out) {
    *out = (s8)gBattle->auraPasses * 0x32 + 0xC8;
    if (*out > 0x1F4) {
        *out = 0x1F4;
    }
    BtlMgr_CountAuraPass();
}

static void func_ov007_020e76dc(BtlAuraLast* data, s32 arg) {
    s32 mode = arg;

    if (data->unk_7F0 > 0) {
        data->unk_7F0--;
        return;
    }

    s32            flag;
    BtlAuraSprite* sprite;
    s16            posX;
    s16            posY;
    s16            animIndex;
    u16            size;
    s32            posXBase;

    sprite = BtlList_GetHead(&data->freeSprites);
    if (sprite == NULL) {
        return;
    }

    if (mode != 0 && mode == 1) {
        animIndex = (s16)RNG_Next(5);
        if (data->unk_808 < 0xC0000) {
            posXBase = data->unk_804;
            flag     = 1;
            size     = 0x40;
            posX     = (s16)((RNG_Next(0x11) - 8) + (posXBase << 12));
            posY     = (s16)(data->unk_808 << 12);
        } else {
            posXBase = data->unk_804;
            flag     = 0;
            size     = 0x3E;
            posX     = (s16)((RNG_Next(0x11) - 8) + (posXBase << 12));
            posY     = (s16)((data->unk_808 << 12) - 0xC0);
        }
        data->unk_7F0 = 2;
    } else {
        animIndex     = (s16)(RNG_Next(2) + 5);
        flag          = 0;
        size          = 0x3E;
        posX          = (s16)(gBattle->player->screenX + (RNG_Next(0x19) - 0xC));
        posY          = (s16)(gBattle->player->screenY + (RNG_Next(0x31) - 0x30));
        data->unk_7F0 = 4;
    }

    s32 flip;
    if (flag == 0) {
        flip = 0x80000000 - gBattle->player->position.y;
    } else {
        flip = 0;
    }

    BtlSprite_LoadDirect(flag, &sprite->sprite, &Iden_BtlMenu, 0x13, 0x15, 0x14, size, (u16)animIndex);
    BtlSprite_SetAnim(&sprite->sprite, (u16)animIndex, 1);
    BtlSprite_SetPosition(&sprite->sprite, posX, posY);
    BtlSprite_SetSortKey(&sprite->sprite, flip);
    BtlList_Remove(&data->freeSprites, &sprite->node);
    BtlList_Append(&data->activeSprites, &sprite->node, sprite);
}

static void func_ov007_020e78e4(BtlAuraLast* data, BtlAuraSprite* sprite) {
    if (sprite == NULL) {
        return;
    }
    BtlSprite_Release(&sprite->sprite);
    BtlList_Remove(&data->activeSprites, &sprite->node);
    BtlList_Append(&data->freeSprites, &sprite->node, sprite);
}

static void func_ov007_020e791c(BtlAuraLast* data) {
    BtlList_Init(&data->freeSprites);
    BtlList_Init(&data->activeSprites);
    data->unk_7F0 = 0;

    s32            i;
    BtlAuraSprite* s = data->sprites;
    for (i = 0; i < 16; i++, s++) {
        BtlSprite_Init(&s->sprite);
        BtlList_Append(&data->freeSprites, &s->node, s);
    }
}

static void func_ov007_020e7970(BtlAuraLast* data) {
    BtlAuraSprite* sprite = BtlList_GetHead(&data->activeSprites);
    if (sprite == NULL) {
        return;
    }

    do {
        if (SpriteMgr_IsAnimationFinished(&sprite->sprite.sprite)) {
            BtlAuraSprite* next = BtlList_GetNext(&sprite->node);
            func_ov007_020e78e4(data, sprite);
            sprite = next;
        } else {
            BtlSprite_Update(&sprite->sprite);
            sprite = BtlList_GetNext(&sprite->node);
        }
    } while (sprite != NULL);
}

static void func_ov007_020e79d8(BtlAuraLast* data) {
    BtlAuraSprite* sprite = BtlList_GetHead(&data->activeSprites);
    if (sprite == NULL) {
        return;
    }

    do {
        if ((sprite->sprite.sprite.bits_0_1) == 1) {
            BtlSprite_SetSortKey(&sprite->sprite, 0);
        } else {
            BattleActor* target = gBattle->player;
            BtlSprite_SetSortKey(&sprite->sprite, 0x80000000 - target->position.y);
        }
        BtlSprite_Render(&sprite->sprite);
        sprite = BtlList_GetNext(&sprite->node);
    } while (sprite != NULL);
}

static void func_ov007_020e7a54(BtlAuraLast* data) {
    for (s32 i = 0; i < 16; i++) {
        if (Sprite_HasAnimation(&data->sprites[i].sprite.sprite)) {
            BtlSprite_Release(&data->sprites[i].sprite);
        }
    }
}

static void func_ov007_020e7a8c(BtlAuraLast* data) {
    if (gBattle->auraPass == 1) {
        return;
    }
    if (gBattle->auraPass == 2) {
        gBattle->auraPass = 0;
        data->unk_804     = gBattle->partner->screenX << 12;
        data->unk_808     = (gBattle->partner->screenY - 0x20) << 12;
        BtlSe_PlayAtSubX(SEIDX_SE_GREEN_LIGHT02, data->unk_804);
        gBattle->auraOnPartner = 0;
        func_ov007_020e769c(&data->unk_80C);
        data->unk_7FC = 1;
        func_ov007_020e75e4(data, func_ov007_020e7b54);
        return;
    }
    if (data->unk_800 != 0) {
        func_ov007_020e76dc(data, 0);
    }
}

static void func_ov007_020e7b54(BtlAuraLast* data) {
    BattleActor* target = gBattle->player;
    s32          posX   = target->screenX << 12;
    s32          posY   = (target->screenY + 0xA0) << 12;

    switch (data->unk_7FA) {
        case 0:
            if (data->unk_7F8 == 0) {
                data->unk_7F8 = 0x5A;
                data->unk_7F0 = 2;
            }
            if (data->unk_7F8 > 0) {
                func_020265d4(&data->unk_804, posX, (u16)data->unk_7F8);
                func_020265d4(&data->unk_808, posY, (u16)data->unk_7F8);
                data->unk_7F8--;
            }
            func_ov007_020e76dc(data, 1);
            if (data->unk_7F8 <= 0) {
                data->unk_7FA++;
                data->unk_7F8 = 0;
            }
            break;
        case 1:
            data->unk_7FC = 0;
            data->unk_800 = 1;
            func_ov003_020974f8(posX, posY);
            gBattle->auraRate     = data->unk_80C;
            gBattle->auraOnPlayer = 1;
            func_ov007_020e764c();
            func_ov007_020e75e4(data, func_ov007_020e7a8c);
            break;
    }
}

static s32 func_ov007_020e7c98(TaskPool* pool, Task* task, void* args) {
    BtlAuraLast* data = task->data;

    data->unk_804          = 0;
    data->unk_808          = 0;
    data->unk_7FC          = 0;
    data->unk_800          = 0;
    gBattle->auraRate      = 0x64;
    gBattle->auraPasses    = 0;
    gBattle->auraOnPartner = 0;
    gBattle->auraOnPlayer  = 0;

    func_ov007_020e75fc(1, &data->unk_000, 0, 0x40);
    func_ov007_020e75fc(0, &data->unk_060, 0, 0x3E);
    func_ov007_020e75fc(0, &data->unk_0C0, 1, 0x3E);
    func_ov007_020e791c(data);
    func_ov007_020e75e4(data, func_ov007_020e7a8c);
    return 1;
}

static s32 func_ov007_020e7d48(TaskPool* pool, Task* task, void* args) {
    BtlAuraLast* data = task->data;

    if (data->unk_7F4 != NULL) {
        data->unk_7F4(data);
    }
    if (data->unk_7FC != 0) {
        BtlSprite_Update(&data->unk_000);
        BtlSprite_Update(&data->unk_060);
    }
    if (data->unk_800 != 0) {
        BtlSprite_Update(&data->unk_0C0);
    }
    func_ov007_020e7970(data);
    return 1;
}

static s32 func_ov007_020e7da4(TaskPool* pool, Task* task, void* args) {
    BtlAuraLast* data = task->data;

    if (data->unk_7FC != 0) {
        if (data->unk_808 < 0xD8000) {
            BtlSprite_SetPosition(&data->unk_000, data->unk_804 << 4 >> 16, data->unk_808 << 4 >> 16);
            BtlSprite_SetSortKey(&data->unk_000, 0);
            BtlSprite_Render(&data->unk_000);
        }
        if (data->unk_808 > 0xA8000) {
            BattleActor* target = gBattle->player;
            BtlSprite_SetPosition(&data->unk_060, (s16)(data->unk_804 >> 12), (s16)((data->unk_808 >> 12) - 0xC0));
            BtlSprite_SetSortKey(&data->unk_060, 0x80000001 - target->position.y);
            BtlSprite_Render(&data->unk_060);
        }
    }
    if (data->unk_800 != 0) {
        BattleActor* target = gBattle->player;
        BtlSprite_SetPosition(&data->unk_0C0, target->screenX, (s16)(target->screenY - 0x20));
        BtlSprite_SetSortKey(&data->unk_0C0, 0x80000001 - target->position.y);
        BtlSprite_Render(&data->unk_0C0);
    }
    func_ov007_020e79d8(data);
    return 1;
}

static s32 func_ov007_020e7eb0(TaskPool* pool, Task* task, void* args) {
    BtlAuraLast* data = task->data;

    BtlSprite_Release(&data->unk_000);
    BtlSprite_Release(&data->unk_060);
    BtlSprite_Release(&data->unk_0C0);
    func_ov007_020e7a54(data);
    return 1;
}

static s32 func_ov007_020e7ee0(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = data_ov007_020e7f70;

    if (BtlMgr_IsStageFrozen(stage) != 0) {
        return 1;
    }
    return stages.iter[stage](pool, task, args);
}
