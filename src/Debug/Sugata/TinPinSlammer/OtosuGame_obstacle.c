/**
 * @file OtosuGame_obstacle.c
 * @brief The `Tsk_OtosuGame_obstacle` task.
 */

#include "OtuFieldAccessShared.h"

typedef struct {
    /* 0x00 */ s32                        dataType; // the sprite template's dataType
    /* 0x04 */ s16                        unk_04;
    /* 0x06 */ u16                        slot;     // Which of the three palette slots to use.
    /* 0x08 */ OtosuGame_obstacle_Params* params;
} OtosuGame_obstacle_Args;                          // Size: 0xC

/* Five tables of three, indexed by OtosuGame_obstacle_Params.kind. They are separate
 * arrays rather than one array of a struct because the target loads each base
 * address into its own register and indexes them independently. */

// -> params.unk_26

// -> params.unk_1C

// -> params.unk_28

s32              OtosuGame_obstacle_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);
SpriteFrameInfo* OtosuGame_obstacle_GetFrameInfo(Sprite* sprite, s32 arg, s32 mode);

static const s16 data_ov039_02099918[3] = {
    0x2,
    0x2,
    0x2,
};

static const s16 data_ov039_02099924[3] = {
    0x3,
    0x3,
    0x3,
};

static const s16 data_ov039_0209991e[3] = {
    0x1,
    0x1,
    0x1,
};

static const s16 data_ov039_0209992a[3] = {
    0x1,
    0x2,
    0x3,
};

static const TaskHandle Tsk_OtosuGame_obstacle = {"Tsk_OtosuGame_obstacle", OtosuGame_obstacle_RunTask,
                                                  sizeof(OtosuGame_obstacle)};

static const s16 data_ov039_02099958[3][3] = {
    0x4, 0x5, 0x6, 0x4, 0x5, 0x6, 0x4, 0x5, 0x6,
};

/** Every field Load does not patch is already correct here, and the five it
 *  does patch all hold their kind-0 values, so this is the index-0 case the
 *  table lookups then re-derive. */
static const SpriteAnimation OtosuGame_obstacle_Anim = {
    .bits_0_1          = 2,
    .dataType          = 0,
    .bit_6             = 0,
    .bits_7_9          = 5,
    .bits_10_11        = 0,
    .bits_12_13        = 2,
    .bits_14_15        = 0,
    .unk_02.raw        = 0,
    .posX              = 0x50,
    .posY              = 0x50,
    .frameInfoCallback = OtosuGame_obstacle_GetFrameInfo,
    .callbackArg       = 0,
    .owner             = NULL,
    .binIden           = &data_ov039_0209a104,
    .unk_18            = 2,
    .packIndex         = 1,
    .unk_1C            = 1,
    .unk_1E            = 0,
    .unk_20            = 4,
    .unk_22            = 1,
    .unk_24            = 0,
    .unk_26            = 2,
    .unk_28            = 3,
    .animIndex         = 1,
};

/** Palette slots. A row per kind, a column per OtosuGame_obstacle_Args.slot; all three
 *  rows currently hold the same {4, 5, 6}, so kind does not yet change colour. */

SpriteFrameInfo* OtosuGame_obstacle_GetFrameInfo(Sprite* sprite, s32 arg, s32 mode) {
    OtosuGame_obstacle* owner = sprite->owner;

    Sprite_FrameInfoCallbackSorted(sprite, mode, func_ov039_02088400(3, owner->pos.y, 0));
}

/**
 * Loads the obstacle sprite, patching the template's art by `kind` and its
 * palette by `kind` and `slot`.
 */
void OtosuGame_obstacle_Load(OtosuGame_obstacle* self, Sprite* sprite, OtosuGame_obstacle_Args* args) {
    SpriteAnimation params = OtosuGame_obstacle_Anim;
    u16             kind   = args->params->kind;

    params.owner     = self;
    params.dataType  = (u16)args->dataType;
    params.packIndex = data_ov039_0209992a[kind];
    params.unk_20    = data_ov039_02099958[kind][args->slot];
    params.unk_26    = data_ov039_02099918[kind];
    params.unk_1C    = data_ov039_0209991e[kind];
    params.unk_28    = data_ov039_02099924[kind];
    params.posX      = (s16)((self->pos.x - self->origin.x) >> 12);
    params.posY      = (s16)((self->pos.y - self->origin.y) >> 12);
    _Sprite_Load(sprite, &params);
}

s32 OtosuGame_obstacle_Init(TaskPool* pool, Task* task, void* args) {
    OtosuGame_obstacle*      self      = task->data;
    OtosuGame_obstacle_Args* a         = args;
    s32                      scales[3] = {0x20000, 0x18000, 0x10000}; // 16.16, per kind

    self->origin.x    = 0;
    self->origin.y    = 0;
    self->pos.x       = (s32)a->params->targetX << 12;
    self->pos.y       = (s32)a->params->targetY << 12;
    self->scale       = scales[a->params->kind];
    self->animPending = 0;

    OtosuGame_obstacle_Load(self, &self->sprite, a);
    return 1;
}

/** Switches to the hit animation once when asked, then steps the sprite. */
s32 OtosuGame_obstacle_Update(TaskPool* pool, Task* task, void* args) {
    OtosuGame_obstacle* self = task->data;

    if (self->animPending != 0) {
        self->sprite.animationMode = ANIM_MODE_ONCE;
        Sprite_SetAnimation(&self->sprite, self->sprite.animData, 2, self->sprite.cellTable);
        self->animPending = 0;
    }

    Sprite_Update(&self->sprite);
    return 1;
}

s32 OtosuGame_obstacle_Render(TaskPool* pool, Task* task, void* args) {
    OtosuGame_obstacle* self = task->data;

    self->sprite.posX = (s16)((self->pos.x - self->origin.x) >> 12);
    self->sprite.posY = (s16)((self->pos.y - self->origin.y) >> 12);

    Sprite_RenderFrame(&self->sprite);
    return 1;
}

s32 OtosuGame_obstacle_Destroy(TaskPool* pool, Task* task, void* args) {
    OtosuGame_obstacle* self = task->data;

    Sprite_Release(&self->sprite);
    return 1;
}

s32 OtosuGame_obstacle_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = OtosuGame_obstacle_Init,
        .update     = OtosuGame_obstacle_Update,
        .render     = OtosuGame_obstacle_Render,
        .cleanup    = OtosuGame_obstacle_Destroy,
    };
    return stages.iter[stage](pool, task, args);
}

s32 OtosuGame_obstacle_CreateTask(TaskPool* pool, s32 dataType, s16 unk_04, s16 slot, OtosuGame_obstacle_Params* params) {
    OtosuGame_obstacle_Args args;

    args.dataType = dataType;
    args.unk_04   = unk_04;
    args.slot     = slot;
    args.params   = params;

    return EasyTask_CreateTask(pool, &Tsk_OtosuGame_obstacle, NULL, 0, NULL, &args);
}

/** Sets the point the obstacle is drawn relative to. */
void func_ov039_02092730(OtosuGame_obstacle* self, OtuPoint* origin) {
    self->origin = *origin;
}

void func_ov039_02092744(OtosuGame_obstacle* self, OtuPoint* out) {
    *out = self->pos;
}

s32 func_ov039_02092758(OtosuGame_obstacle* self) {
    return self->scale;
}

/** Plays the hit animation on the next update. */
void func_ov039_02092760(OtosuGame_obstacle* self) {
    self->animPending = 1;
}
