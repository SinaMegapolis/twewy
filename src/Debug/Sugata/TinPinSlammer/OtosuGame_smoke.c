/**
 * @file OtosuGame_smoke.c
 * @brief The `Tsk_OtosuGame_smoke` task.
 */

#include "OtuFieldAccessShared.h"

SpriteFrameInfo* func_ov039_02097398(Sprite* sprite, s32 arg, s32 mode) {
    OtuSmoke* owner = sprite->owner;

    Sprite_FrameInfoCallbackSorted(sprite, mode, func_ov039_02088400(4, owner->pos.y, owner->height));
}

void func_ov039_02097454(OtuSmoke* self, Sprite* sprite, OtuPinSpriteArgs* args) {
    SpriteAnimation anim = data_ov039_02099f68;

    anim.owner    = self;
    anim.dataType = (u16)args->dataType;
    anim.posX     = (s16)(self->pos.x >> 0xC);
    anim.posY     = (s16)(self->pos.y >> 0xC);
    _Sprite_Load(sprite, &anim);
}

s32 func_ov039_020974e0(TaskPool* pool, Task* task, void* args) {
    OtuSmoke*         self = task->data;
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
    func_ov039_02097454(self, &self->sprite, a);

    return 1;
}

/**
 * Drifts the puff along `dir` while its speed decays, and runs the intro / hold
 * / outro animation states. `== TRUE` keeps the target's `cmp #1` on the bit.
 */
s32 func_ov039_02097528(TaskPool* pool, Task* task, void* args) {
    OtuSmoke* self = task->data;
    void*     pin  = EasyTask_GetTaskData(pool, self->pinId);

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

s32 func_ov039_02097670(TaskPool* pool, Task* task, void* args) {
    OtuSmoke* self = task->data;

    if (self->visible != 0) {
        self->sprite.posX = (s16)((self->pos.x - self->origin.x) >> 0xC);
        self->sprite.posY = (s16)((self->pos.y - self->origin.y + self->height) >> 0xC);
        Sprite_RenderFrame(&self->sprite);
    }

    return 1;
}

s32 func_ov039_020976c0(TaskPool* pool, Task* task, void* args) {
    Sprite_Release(&((OtuSmoke*)task->data)->sprite);
    return 1;
}

s32 func_ov039_020976d4(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = func_ov039_020974e0,
        .update     = func_ov039_02097528,
        .render     = func_ov039_02097670,
        .cleanup    = func_ov039_020976c0,
    };

    return stages.iter[stage](pool, task, args);
}

s32 func_ov039_0209771c(TaskPool* pool, s32 dataType, s32 pinId) {
    OtuPinSpriteArgs args;

    args.dataType = dataType;
    args.childId  = pinId;
    return EasyTask_CreateTask(pool, &data_ov039_02099f4c, NULL, 0, NULL, &args);
}

/**
 * Puffs smoke at `at`, drifting in the table direction `angle >> 4` at
 * `speed`, slowing by `decel`, and holding for `hold` frames.
 */
// Nonmatching: 46.3%, the scheduler interleaves the stores and the two table
// loads differently; every field, constant and argument position is settled.
void func_ov039_02097750(OtuSmoke* self, OtuPoint* at, s32 angle, s32 speed, s32 decel, s32 hold) {
    s32 index = angle >> 4;

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
