#include "Combat/Core/BtlField.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatSprite.h"
#include "Engine/EasyTask.h"
#include "Engine/Math/Random.h"
#include "SpriteMgr.h"

#include <nitro/mi/cpumem.h>

// The "foreground" layer of a battle field: one combat sprite per backdrop band, spawned from a
// per-background table and scrolled with the battle camera.

/// One entry of the foreground band table: where the band sits, the sheet it is cut from and the
/// four sprite indices.
typedef struct BtlFieldForePos {
    /* 0x00 */ s32 x; // world position, in 0x1000 units
    /* 0x04 */ s32 y;
    /* 0x08 */ s32 z;
} BtlFieldForePos; // Size: 0xC

typedef struct BtlFieldForeRow {
    /* 0x00 */ BtlFieldForePos      pos;
    /* 0x0C */ const BinIdentifier* pack;
    /* 0x10 */ u8                   packIndex;
    /* 0x11 */ u8                   charDataIndex;
    /* 0x12 */ u8                   animDataIndex;
    /* 0x13 */ u8                   cellTableIndex;
    /* 0x14 */ u8                   paletteDataIndex;
    /* 0x15 */ u8                   animIndex;
    /* 0x16 */ u8                   unk_16[2];
} BtlFieldForeRow; // Size: 0x18

/// One backdrop band: its sprite and the world position the sprite is projected from.
typedef struct BtlFieldForeBand {
    /* 0x00 */ CombatSprite    sprite;
    /* 0x60 */ BtlFieldForePos position;
} BtlFieldForeBand; // Size: 0x6C

/// The foreground task data, `TaskHandle.dataSize` = 0xDC.
typedef struct BtlFieldFore {
    /* 0x00 */ BtlFieldForeBand bands[2];
    /* 0xD8 */ s16              count; // live bands
} BtlFieldFore;                        // Size: 0xDC

/// The background sheets the bands are cut from.
extern const BinIdentifier data_ov003_020da114;
extern const BinIdentifier data_ov003_020da124;
extern const BinIdentifier data_ov003_020da154;
extern const BinIdentifier data_ov003_020da16c;
extern const BinIdentifier data_ov003_020da174;
extern const BinIdentifier data_ov003_020da1ac;
extern const BinIdentifier data_ov003_020da1b4;

static s32 BtlField_Fore_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

static const TaskHandle Task_BtlField_Fore = {"Task_BtlField_Fore", BtlField_Fore_RunTask, sizeof(BtlFieldFore)};

/// The foreground band table, indexed by background id and band.
static const BtlFieldForeRow data_ov003_020d7ba0[15] = {
    {    {0x01EB, 0x00BF, 0x0000}, &data_ov003_020da114, 2, 5, 7, 6, 8, 0},
    {{0x01EB, 0x00BF, 0xFFFFFF00}, &data_ov003_020da114, 2, 5, 7, 6, 8, 1},
    {    {0x011C, 0x0080, 0x0000}, &data_ov003_020da124, 2, 5, 7, 6, 8, 0},
    {    {0x01D0, 0x0088, 0x0000}, &data_ov003_020da154, 2, 5, 7, 6, 8, 0},
    {    {0x0158, 0x007F, 0x0000}, &data_ov003_020da16c, 2, 5, 7, 6, 8, 0},
    {{0x0158, 0x007F, 0xFFFFFF00}, &data_ov003_020da16c, 2, 5, 7, 6, 8, 1},
    {    {0x0180, 0x00A1, 0x0000}, &data_ov003_020da174, 2, 5, 7, 6, 8, 0},
    {{0xFFFFFFFA, 0x007F, 0x0000}, &data_ov003_020da1ac, 2, 5, 7, 6, 8, 0},
    {    {0x0180, 0x00A1, 0x0000}, &data_ov003_020da174, 2, 5, 7, 6, 9, 0},
    {    {0x0158, 0x007F, 0x0000}, &data_ov003_020da16c, 2, 5, 7, 6, 9, 0},
    {{0x0158, 0x007F, 0xFFFFFF00}, &data_ov003_020da16c, 2, 5, 7, 6, 9, 1},
    {    {0x01D0, 0x0088, 0x0000}, &data_ov003_020da154, 2, 5, 7, 6, 9, 0},
    {    {0x01EB, 0x00BF, 0x0000}, &data_ov003_020da114, 2, 5, 7, 6, 9, 0},
    {{0x01EB, 0x00BF, 0xFFFFFF00}, &data_ov003_020da114, 2, 5, 7, 6, 9, 1},
    {    {0x0180, 0x009C, 0x0000}, &data_ov003_020da1b4, 2, 5, 7, 6, 8, 0},
};

/// Spawns band `index` as `cSprite` and counts it in.
static void func_ov003_02093c70(BtlFieldFore* fore, BtlFieldForeBand* band, s32 index) {
    SpriteAnimationEx      anim;
    const BtlFieldForeRow* row;

    row = &data_ov003_020d7ba0[index];
    CombatSprite_InitAnim(&anim.anim, 2, row->pack);
    anim.anim.packIndex  = row->packIndex;
    anim.anim.bits_10_11 = 0;
    anim.anim.unk_28     = row->animDataIndex;
    anim.anim.unk_18     = 2;
    anim.anim.unk_1C     = row->charDataIndex;
    anim.anim.unk_20     = row->paletteDataIndex;
    anim.anim.unk_26     = row->cellTableIndex;
    anim.anim.animIndex  = row->animIndex + 1;
    anim.animTable       = NULL;
    CombatSprite_Load(&band->sprite, &anim);
    func_ov003_02082738(&band->sprite, 0);
    band->position.x = row->pos.x << 12;
    band->position.y = row->pos.y << 12;
    band->position.z = row->pos.z << 12;
    fore->count++;
}

/// Spawns the bands the background id asks for.
static s32 BtlField_Fore_Init(TaskPool* pool, Task* task, void* args) {
    BtlFieldFore* fore = task->data;

    MI_CpuSet(fore, NULL, sizeof(BtlFieldFore));
    fore->count = 0;
    switch (*(s32*)args) {
        case 1:
            func_ov003_02093c70(fore, &fore->bands[0], 0);
            func_ov003_02093c70(fore, &fore->bands[1], 1);
            break;
        case 3:
            func_ov003_02093c70(fore, &fore->bands[0], 2);
            break;
        case 9:
            func_ov003_02093c70(fore, &fore->bands[0], 3);
            break;
        case 12:
            func_ov003_02093c70(fore, &fore->bands[0], 4);
            func_ov003_02093c70(fore, &fore->bands[1], 5);
            break;
        case 13:
            func_ov003_02093c70(fore, &fore->bands[0], 6);
            break;
        case 20:
            func_ov003_02093c70(fore, &fore->bands[0], 7);
            break;
        case 28:
            func_ov003_02093c70(fore, &fore->bands[0], 8);
            break;
        case 33:
            func_ov003_02093c70(fore, &fore->bands[0], 9);
            func_ov003_02093c70(fore, &fore->bands[1], 10);
            break;
        case 34:
            func_ov003_02093c70(fore, &fore->bands[0], 11);
            break;
        case 39:
            func_ov003_02093c70(fore, &fore->bands[0], 12);
            func_ov003_02093c70(fore, &fore->bands[1], 13);
            break;
        case 21:
        case 40:
            func_ov003_02093c70(fore, &fore->bands[0], 14);
            break;
    }
    return 1;
}

/// Advances every live band.
static s32 BtlField_Fore_Update(TaskPool* pool, Task* task, void* args) {
    BtlFieldFore*     fore = task->data;
    BtlFieldForeBand* band;
    s32               i;

    i = 0;
    if (fore->count > 0) {
        band = fore->bands;
        do {
            CombatSprite_Update(&band->sprite);
            i++;
            band++;
        } while (i < fore->count);
    }
    return 1;
}

/// Projects and draws the bands.
static s32 BtlField_Fore_Render(TaskPool* pool, Task* task, void* args) {
    BtlFieldFore*     fore = task->data;
    BtlFieldForeBand* band;
    s32               i;
    s32               posX;
    s32               posY;

    if (data_ov003_020e71b8->unk3D878 & 0x1000) {
        return 1;
    }
    i = 0;
    if (fore->count > 0) {
        band = fore->bands;
        do {
            posX = func_ov003_020843b0(0, band->position.x);
            posY = func_ov003_020843ec(0, band->position.y, band->position.z);
            CombatSprite_SetPosition(&band->sprite, posX, posY);
            CombatSprite_Render(&band->sprite);
            i++;
            band++;
        } while (i < fore->count);
    }
    return 1;
}

/// Releases every live band.
static s32 BtlField_Fore_Release(TaskPool* pool, Task* task, void* args) {
    BtlFieldFore*     fore = task->data;
    BtlFieldForeBand* band;
    s32               i;

    i = 0;
    if (fore->count > 0) {
        band = fore->bands;
        do {
            CombatSprite_Release(&band->sprite);
            i++;
            band++;
        } while (i < fore->count);
    }
    fore->count = 0;
    return 1;
}

static s32 BtlField_Fore_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlField_Fore_Init,
        .update     = BtlField_Fore_Update,
        .render     = BtlField_Fore_Render,
        .cleanup    = BtlField_Fore_Release,
    };

    return stages.iter[stage](pool, task, args);
}

void func_ov003_0209409c(s32 bgIndex) {
    s32 args = bgIndex;

    switch (bgIndex) {
        case 1:
        case 3:
        case 9:
        case 12:
        case 13:
        case 20:
        case 21:
        case 28:
        case 33:
        case 34:
        case 39:
        case 40:
            break;
        default:
            return;
    }
    {
        EasyTask_CreateTask(&data_ov003_020e71b8->unk_10118, &Task_BtlField_Fore, NULL, 0, NULL, &args);
    }
}
