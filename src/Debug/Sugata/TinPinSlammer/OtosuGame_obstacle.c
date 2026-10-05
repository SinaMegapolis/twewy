/**
 * @file OtosuGame_obstacle.c
 * @brief The `Tsk_OtosuGame_obstacle` task.
 */

#include "OtuFieldAccessShared.h"

typedef struct {
    /* 0x00 */ s32                        oamAttrs; // ORed into the template's OAM word, shifted to bit 6.
    /* 0x04 */ s16                        unk_04;
    /* 0x06 */ s16                        slot;     // Which of the three palette slots to use.
    /* 0x08 */ OtosuGame_obstacle_Params* params;
} OtosuGame_obstacle_Args;                          // Size: 0xC

/** The handle, the stage table and the sprite template are all already in the
 *  overlay's .rodata, so they are referenced rather than redefined. The name
 *  string inside the handle is the ground truth for what this task is. */
extern const TaskHandle Tsk_OtosuGame_obstacle;

/** The sprite template. Every field Load does not patch is already correct here,
 *  and the five it does patch all hold their kind-0 values, so this is literally
 *  the index-0 case that the table lookups below then re-derive. */
extern const SpriteAnimation OtosuGame_obstacle_Anim;

/* Five tables of three, indexed by OtosuGame_obstacle_Params.kind. They are separate
 * arrays rather than one array of a struct because the target loads each base
 * address into its own register and indexes them independently. */
extern const s16 data_ov039_02099918[3];

// -> params.unk_26
extern const s16 data_ov039_0209991e[3];

// -> params.unk_1C
extern const s16 data_ov039_02099924[3];

// -> params.unk_28
extern const s16 data_ov039_0209992a[3];

/** Palette slots. A row per kind, a column per OtosuGame_obstacle_Args.slot; all three
 *  rows currently hold the same {4, 5, 6}, so kind does not yet change colour. */
extern const s16 data_ov039_02099958[3][3];

SpriteFrameInfo* OtosuGame_obstacle_GetFrameInfo(Sprite* sprite, s32 arg, s32 mode) {
    OtosuGame_obstacle* owner = sprite->owner;

    Sprite_FrameInfoCallbackSorted(sprite, mode, func_ov039_02088400(3, owner->pos.y, 0));
}

/**
 * Loads the obstacle sprite, patching the template's art by `kind` and its
 * palette by `kind` and `slot`. The dataType edit is one expression because
 * mwcc hoists the argument loads above it regardless.
 */
void OtosuGame_obstacle_Load(OtosuGame_obstacle* self, Sprite* sprite, OtosuGame_obstacle_Args* args) {
    SpriteAnimation params = OtosuGame_obstacle_Anim;
    u16             kind   = args->params->kind;

    *(u16*)&params   = (u16)(*(u16*)&params & ~0x3C) | (u16)((u16)args->oamAttrs << 2);
    params.owner     = self;
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

s32 OtosuGame_obstacle_CreateTask(TaskPool* pool, s32 oamAttrs, s16 unk_04, s16 slot, OtosuGame_obstacle_Params* params) {
    OtosuGame_obstacle_Args args;

    args.oamAttrs = oamAttrs;
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
