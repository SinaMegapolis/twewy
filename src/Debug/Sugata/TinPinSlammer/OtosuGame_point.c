/**
 * @file OtosuGame_point.c
 * @brief The `Tsk_OtosuGame_point` task.
 */

#include "OtuFieldAccessShared.h"

extern TaskHandle       data_ov039_02099d44;
extern const TaskStages data_ov039_02099d50;
extern SpriteAnimation  data_ov039_02099d60;

/* Four `s16` values, 12, 11, 1, 1 -- one per sprite of the point task. */
extern const s16 data_ov039_02099d3c[4];

SpriteFrameInfo* func_ov039_020958a8(Sprite* sprite, s32 arg, s32 mode) {
    OtuPointTask* owner = sprite->owner;

    Sprite_FrameInfoCallbackSorted(sprite, mode, func_ov039_02088400(5, owner->pos.y, 0));
}

/** Loads sprite `index`, whose animation comes from data_ov039_02099d3c. */
void func_ov039_02095964(OtuPointTask* self, Sprite* sprite, OtuPinSpriteArgs* args, s32 index) {
    SpriteAnimation anim = data_ov039_02099d60;

    anim.owner     = self;
    anim.dataType  = args->dataType;
    anim.posX      = (self->pos.x - self->origin.x) >> 12;
    anim.posY      = (self->pos.y - self->origin.y) >> 12;
    anim.animIndex = data_ov039_02099d3c[index];

    _Sprite_Load(sprite, &anim);
}

s32 func_ov039_02095a18(TaskPool* pool, Task* task, void* args) {
    OtuPointTask*     self = task->data;
    OtuPinSpriteArgs* a    = args;
    s32               i;

    self->pinId    = a->childId;
    self->origin.x = 0;
    self->origin.y = 0;
    self->pos.x    = 0;
    self->pos.y    = 0;
    self->active   = 0;
    self->visible  = 0;

    for (i = 0; i < 4; i++) {
        func_ov039_02095964(self, &self->sprite[i], a, i);
    }
    return 1;
}

/**
 * Bounces each live digit: once its delay is spent it falls under gravity
 * (0x800 per frame) and, on landing, rebounds at a quarter of its speed. The
 * whole popup hides once `timer` runs out.
 */
s32 func_ov039_02095a78(TaskPool* pool, Task* task, void* args) {
    OtuPointTask* self = task->data;
    s32           i;

    if (self->active != 0) {
        for (i = 0; i < 4; i++) {
            if (self->slot[i].live != 0) {
                if (self->slot[i].delay > 0) {
                    self->slot[i].delay = self->slot[i].delay - 1;
                } else {
                    self->slot[i].accum = self->slot[i].accum + self->slot[i].phase;
                    self->slot[i].phase = self->slot[i].phase + 0x800;

                    if (self->slot[i].accum > 0) {
                        self->slot[i].accum = 0;

                        if (self->slot[i].phase > 0) {
                            // A Q12 multiply by -0.25; `/ -4` compiles to shifts.
                            self->slot[i].phase = (s32)(((s64)self->slot[i].phase * -0x400 + 0x800) >> 12);
                        }
                    }
                    Sprite_Update(&self->sprite[i]);
                }
            }
        }

        self->visible = 1;
        self->timer   = self->timer - 1;

        if (self->timer <= 0) {
            self->active  = 0;
            self->visible = 0;
        }
    }
    return 1;
}

s32 func_ov039_02095b6c(TaskPool* pool, Task* task, void* args) {
    OtuPointTask* self = task->data;
    OtuBadge*     pin;
    s32           i;

    if (self->visible != 0) {
        pin = EasyTask_GetTaskData(pool, self->pinId);

        if (pin != NULL) {
            func_ov039_0208e85c(pin, &self->origin);
            func_ov039_0208e6e0(pin, &self->pos);

            for (i = 0; i < 4; i++) {
                if (self->slot[i].live != 0 && self->slot[i].delay <= 0) {
                    // The target computes y before x.
                    s32 py = self->pos.y - self->origin.y + self->slot[i].accum - 0x10000;
                    s32 px = self->pos.x - self->origin.x + self->slot[i].offsetX;

                    self->sprite[i].posX = px >> 12;
                    self->sprite[i].posY = py >> 12;
                    Sprite_RenderFrame(&self->sprite[i]);
                }
            }
        }
    }
    return 1;
}

s32 func_ov039_02095c2c(TaskPool* pool, Task* task, void* args) {
    OtuPointTask* self = task->data;
    s32           i;

    for (i = 0; i < 4; i++) {
        Sprite_Release(&self->sprite[i]);
    }
    return 1;
}

s32 func_ov039_02095c58(TaskPool* pool, Task* task, void* args, s32 stage) {
    const TaskStages stages = data_ov039_02099d50;
    return stages.iter[stage](pool, task, args);
}

s32 func_ov039_02095ca0(TaskPool* pool, s32 dataType, s32 pin) {
    OtuPinSpriteArgs args;

    args.dataType = dataType;
    args.childId  = pin;

    return EasyTask_CreateTask(pool, &data_ov039_02099d44, NULL, 0, NULL, &args);
}

/**
 * Pops up `count` for 60 frames: the digits drop in staggered by four frames,
 * sprite 2 shows the ones digit and sprite 3 the tens (hidden when zero).
 */
void func_ov039_02095cd4(OtuPointTask* self, s32 count) {
    s32 i;
    s16 first;
    s16 last;

    self->active = 1;
    self->timer  = 0x3C;

    for (i = 0; i < 4; i++) {
        self->slot[i].live    = 1;
        self->slot[i].delay   = (3 - i) * 4;
        self->slot[i].offsetX = (0xC - i * 8) << 12;
        self->slot[i].accum   = 0;
        self->slot[i].phase   = -0x3000;
    }

    self->count = count;

    // Both digits are narrowed before use; routing the first through `last`
    // is what the register allocation needs.
    last  = (count % 10) + 1;
    first = (s16)last;
    Sprite_ChangeAnimation(&self->sprite[2], self->sprite[2].animData, first, self->sprite[2].cellTable);

    last = (s16)((self->count / 10) % 10 + 1);

    if ((s16)last <= 1) {
        self->slot[3].live = 0;
        return;
    }

    Sprite_ChangeAnimation(&self->sprite[3], self->sprite[3].animData, last, self->sprite[3].cellTable);
}

/** Hides the popup. */
void func_ov039_02095ddc(OtuPointTask* self) {
    self->active  = 0;
    self->visible = 0;
}
