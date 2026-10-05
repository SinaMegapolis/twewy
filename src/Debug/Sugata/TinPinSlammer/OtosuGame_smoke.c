/**
 * @file OtosuGame_smoke.c
 * @brief The `Tsk_OtosuGame_smoke` task.
 */

#include "OtuFieldAccessShared.h"

s32              OtosuGame_smoke_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);
SpriteFrameInfo* OtosuGame_smoke_GetFrameInfo(Sprite* sprite, s32 arg, s32 mode);

static const TaskHandle Tsk_OtosuGame_smoke = {"Tsk_OtosuGame_smoke", OtosuGame_smoke_RunTask, sizeof(OtosuGame_smoke)};

static const SpriteAnimation OtosuGame_smoke_Anim = {
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
    .frameInfoCallback = OtosuGame_smoke_GetFrameInfo,
    .callbackArg       = 0,
    .owner             = NULL,
    .binIden           = &data_ov039_0209a0dc,
    .unk_18            = 2,
    .packIndex         = 8,
    .unk_1C            = 1,
    .unk_1E            = 0,
    .unk_20            = 4,
    .unk_22            = 2,
    .unk_24            = 0,
    .unk_26            = 2,
    .unk_28            = 3,
    .animIndex         = 4,
};

SpriteFrameInfo* OtosuGame_smoke_GetFrameInfo(Sprite* sprite, s32 arg, s32 mode) {
    OtosuGame_smoke* owner = sprite->owner;

    Sprite_FrameInfoCallbackSorted(sprite, mode, func_ov039_02088400(4, owner->pos.y, owner->height));
}

/**
 * Loads the smoke sprite. `dataType` is read as a word and narrowed: the target
 * masks it with `lsl #0x10; lsr #0x10` before folding it into the template.
 */
void OtosuGame_smoke_Load(OtosuGame_smoke* self, Sprite* sprite, OtuPinSpriteArgs* args) {
    SpriteAnimation anim = OtosuGame_smoke_Anim;

    anim.owner    = self;
    anim.dataType = (u16)args->dataType;
    anim.posX     = (s16)(self->pos.x >> 0xC);
    anim.posY     = (s16)(self->pos.y >> 0xC);
    _Sprite_Load(sprite, &anim);
}

s32 OtosuGame_smoke_Init(TaskPool* pool, Task* task, void* args) {
    OtosuGame_smoke*  self = task->data;
    OtuPinSpriteArgs* a    = args;

    self->visible  = 0;
    self->state    = 0;
    self->pinId    = a->childId;
    self->origin.x = 0;
    self->origin.y = 0;
    self->pos.x    = 0;
    self->pos.y    = 0;
    self->height   = 0;
    self->dir.x    = 0;
    self->dir.y    = 0;
    OtosuGame_smoke_Load(self, &self->sprite, a);

    return 1;
}

/**
 * Drifts the puff along `dir` while its speed decays, and runs the intro / hold
 * / outro animation states. `== TRUE` keeps the target's `cmp #1` on the bit.
 */
s32 OtosuGame_smoke_Update(TaskPool* pool, Task* task, void* args) {
    OtosuGame_smoke* self = task->data;
    void*            pin  = EasyTask_GetTaskData(pool, self->pinId);

    if (pin != NULL) {
        if (self->state != 0) {
            func_ov039_0208e85c(pin, &self->origin);

            if (self->speed > 0) {
                self->speed = self->speed - self->decel;

                if (self->speed < 0) {
                    self->speed = 0;
                }
            }

            func_ov039_02098c00(self->speed, &self->dir, &self->pos, &self->pos);
            self->visible = 1;
        }

        switch (self->state) {
            case 0:
                self->visible = 0;
                break;

            case 1:
                if (self->sprite.isPlaying == TRUE) {
                    Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, 8, self->sprite.cellTable);
                    self->state = 2;
                }
                Sprite_Update(&self->sprite);
                break;

            case 2:
                self->hold = self->hold - 1;

                if (self->hold <= 0) {
                    Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, 9, self->sprite.cellTable);
                    self->state = 3;
                }
                Sprite_Update(&self->sprite);
                break;

            case 3:
                if (self->sprite.isPlaying == TRUE) {
                    self->state   = 0;
                    self->visible = 0;
                }
                Sprite_Update(&self->sprite);
                break;

            default:
                break;
        }
    } else {
        self->visible = 0;
        self->state   = 0;
    }

    return 1;
}

s32 OtosuGame_smoke_Render(TaskPool* pool, Task* task, void* args) {
    OtosuGame_smoke* self = task->data;

    if (self->visible != 0) {
        self->sprite.posX = (s16)((self->pos.x - self->origin.x) >> 0xC);
        self->sprite.posY = (s16)((self->pos.y - self->origin.y + self->height) >> 0xC);
        Sprite_RenderFrame(&self->sprite);
    }

    return 1;
}

s32 OtosuGame_smoke_Destroy(TaskPool* pool, Task* task, void* args) {
    Sprite_Release(&((OtosuGame_smoke*)task->data)->sprite);
    return 1;
}

s32 OtosuGame_smoke_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = OtosuGame_smoke_Init,
        .update     = OtosuGame_smoke_Update,
        .render     = OtosuGame_smoke_Render,
        .cleanup    = OtosuGame_smoke_Destroy,
    };

    return stages.iter[stage](pool, task, args);
}

s32 OtosuGame_smoke_CreateTask(TaskPool* pool, s32 dataType, s32 pinId) {
    OtuPinSpriteArgs args;

    args.dataType = dataType;
    args.childId  = pinId;
    return EasyTask_CreateTask(pool, &Tsk_OtosuGame_smoke, NULL, 0, NULL, &args);
}

/**
 * Puffs smoke at `at`, drifting in the table direction `angle >> 4` at
 * `speed`, slowing by `decel`, and holding for `hold` frames.
 */
// Nonmatching: 43%, scheduling only: the target interleaves the stores and the
// two table loads differently.
void func_ov039_02097750(OtosuGame_smoke* self, OtuPoint* at, s32 angle, s32 speed, s32 decel, s32 hold) {
    s32 index = (angle >> 4) * 2;

    self->pos.x  = at->x;
    self->pos.y  = at->y;
    self->height = 0;
    self->speed  = speed;
    self->decel  = decel;
    self->unk_64 = 0;
    self->dir.x  = ((s16*)data_0205e4e0)[index + 1];
    self->dir.y  = ((s16*)data_0205e4e0)[index];
    self->hold   = hold;
    Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, 7, self->sprite.cellTable);
    self->state = 1;
}
