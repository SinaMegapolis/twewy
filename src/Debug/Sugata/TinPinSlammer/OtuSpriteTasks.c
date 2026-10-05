#include "OtuFieldAccessShared.h"

/*
 * ov039 region 0x0209702c - 0x02098394: the fragment and effect tasks
 * (meteohahen, smoke, warp, needlehahen, hammerhahen). The shared types,
 * externs and prototypes are in OtuFieldAccessShared.h.
 *
 * The fragment constants (data_ov039_0209a2fc...0209a328) are declared
 * non-const: they live in `.data`, and told they are const mwcc keeps them in a
 * register across FX_Divide where the target reloads them.
 */

/* ==================================================================== */
/* Tsk_OtosuGame_meteohahen (continued from OtuTaskStages.c)            */
/* ==================================================================== */

/**
 * Flies the fragment: shrinks it with its remaining life, bleeds its speed,
 * moves it along `dir` and bounces its height off the ground with damping.
 * The rescale is a rounded 64-bit Q12 multiply; `/ 0x1000` calls __ll_sdiv.
 */
s32 func_ov039_0209702c(TaskPool* pool, Task* task, void* args) {
    OtuHahen* self = task->data;
    void*     pin  = EasyTask_GetTaskData(pool, self->pinId);

    if (pin != NULL) {
        if (self->active != 0) {
            self->life = self->life - 1;

            if (self->life <= 0) {
                self->active  = 0;
                self->visible = 0;
            } else {
                func_ov039_0208e85c(pin, &self->origin);
                self->affine.scaleY = _s32_div_f(self->life * 0x1800, self->lifeMax);
                self->affine.scaleX = self->affine.scaleY;

                self->speed = self->speed - data_ov039_0209a304;

                if (self->speed < 0) {
                    self->speed = 0;
                }

                self->vz = self->vz + data_ov039_0209a310;
                func_ov039_02098c00(self->speed, &self->dir, &self->pos, &self->pos);

                self->height = self->height + self->vz;

                if (self->height > 0) {
                    self->height = 0;

                    if (self->vz > 0) {
                        self->vz = (s32)(((s64)self->vz * -data_ov039_0209a300 + 0x800) >> 0xC);
                    }
                }

                Sprite_Update(&self->sprite);
                self->visible = 1;
            }
        }
    } else {
        self->active  = 0;
        self->visible = 0;
    }

    return 1;
}

s32 func_ov039_02097160(TaskPool* pool, Task* task, void* args) {
    OtuHahen* self = task->data;

    if (self->visible != 0) {
        self->sprite.posX = (s16)((self->pos.x - self->origin.x) >> 0xC);
        self->sprite.posY = (s16)((self->pos.y - self->origin.y + self->height) >> 0xC);
        Sprite_RenderFrame(&self->sprite);
    }

    return 1;
}

s32 func_ov039_020971b0(TaskPool* pool, Task* task, void* args) {
    Sprite_Release(&((OtuHahen*)task->data)->sprite);
    return 1;
}

s32 func_ov039_020971c4(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = func_ov039_02096fcc,
        .update     = func_ov039_0209702c,
        .render     = func_ov039_02097160,
        .cleanup    = func_ov039_020971b0,
    };

    return stages.iter[stage](pool, task, args);
}

s32 func_ov039_0209720c(TaskPool* pool, s32 dataType, s32 pinId) {
    OtuPinSpriteArgs args;

    args.dataType = dataType;
    args.childId  = pinId;
    return EasyTask_CreateTask(pool, &data_ov039_02099f04, NULL, 0, NULL, &args);
}

/**
 * Launches a meteor fragment from `at`: a jittered speed, an upward kick, a
 * random direction from the base game's table and one of animations 10..16.
 */
// Nonmatching: 87.6%, register allocation and load scheduling only.
void func_ov039_02097240(OtuHahen* self, OtuPoint* at) {
    s32 index;

    self->active        = 1;
    self->affine.scaleX = 0x1800;
    self->affine.scaleY = 0x1800;
    self->pos.x         = at->x;
    self->pos.y         = at->y;
    self->height        = 0;

    self->speed = data_ov039_0209a314;
    self->speed = self->speed + RNG_Next(data_ov039_0209a328 - data_ov039_0209a314);
    self->vz    = 0 - data_ov039_0209a2fc;
    self->vz    = self->vz - RNG_Next(data_ov039_0209a30c - data_ov039_0209a2fc);

    self->lifeMax = OTU_ABS_AIRTIME(self->vz);
    self->life    = self->lifeMax;

    index       = RNG_Next(0x10000) >> 4;
    self->dir.x = ((s16*)data_0205e4e0)[index + 1];
    self->dir.y = ((s16*)data_0205e4e0)[index];

    Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, (s16)(RNG_Next(7) + 0xA), self->sprite.cellTable);
}

/* ==================================================================== */
/* Tsk_OtosuGame_smoke                                                  */
/* ==================================================================== */

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

/* ==================================================================== */
/* Tsk_OtosuGame_warp                                                   */
/* ==================================================================== */

SpriteFrameInfo* func_ov039_020977d0(Sprite* sprite, s32 arg, s32 mode) {
    OtuWarp* owner = sprite->owner;

    Sprite_FrameInfoCallbackSorted(sprite, mode, func_ov039_02088400(3, owner->pos.y, 0));
}

void func_ov039_0209788c(OtuWarp* self, Sprite* sprite, OtuPinSpriteArgs* args) {
    SpriteAnimation anim = data_ov039_02099fb0;

    anim.owner    = self;
    anim.dataType = (u16)args->dataType;
    anim.posX     = (s16)(self->pos.x >> 0xC);
    anim.posY     = (s16)(self->pos.y >> 0xC);
    _Sprite_Load(sprite, &anim);
}

s32 func_ov039_02097918(TaskPool* pool, Task* task, void* args) {
    OtuWarp*          self = task->data;
    OtuPinSpriteArgs* a    = args;

    self->active   = 0;
    self->visible  = 0;
    self->pinId    = a->childId;
    self->origin.x = 0;
    self->origin.y = 0;
    self->pos.x    = 0;
    self->pos.y    = 0;
    func_ov039_0209788c(self, &self->sprite, a);

    return 1;
}

/**
 * Plays the animation once at its point. Nested positively: written as
 * `if (pin == NULL) ... else if`, mwcc tail-merges the two clears.
 */
s32 func_ov039_02097954(TaskPool* pool, Task* task, void* args) {
    OtuWarp* self = task->data;
    void*    pin  = EasyTask_GetTaskData(pool, self->pinId);

    if (pin != NULL) {
        if (self->active != 0) {
            if (SpriteMgr_IsAnimationFinished(&self->sprite) == 0) {
                func_ov039_0208e85c(pin, &self->origin);
                Sprite_Update(&self->sprite);
                self->visible = 1;
            } else {
                self->active  = 0;
                self->visible = 0;
            }
        }
    } else {
        self->active  = 0;
        self->visible = 0;
    }

    return 1;
}

s32 func_ov039_020979cc(TaskPool* pool, Task* task, void* args) {
    OtuWarp* self = task->data;

    if (self->visible != 0) {
        self->sprite.posX = (s16)((self->pos.x - self->origin.x) >> 0xC);
        self->sprite.posY = (s16)((self->pos.y - self->origin.y) >> 0xC);
        Sprite_RenderFrame(&self->sprite);
    }

    return 1;
}

s32 func_ov039_02097a14(TaskPool* pool, Task* task, void* args) {
    Sprite_Release(&((OtuWarp*)task->data)->sprite);
    return 1;
}

s32 func_ov039_02097a28(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = func_ov039_02097918,
        .update     = func_ov039_02097954,
        .render     = func_ov039_020979cc,
        .cleanup    = func_ov039_02097a14,
    };

    return stages.iter[stage](pool, task, args);
}

s32 func_ov039_02097a70(TaskPool* pool, s32 dataType, s32 pinId) {
    OtuPinSpriteArgs args;

    args.dataType = dataType;
    args.childId  = pinId;
    return EasyTask_CreateTask(pool, &data_ov039_02099f94, NULL, 0, NULL, &args);
}

/**
 * Plays `animation` at `at`. `animation` stays s32: declared s16, mwcc drops
 * the narrowing pair the target has.
 */
// Nonmatching: 68.3%, scheduling of the narrowing pair and the pool load.
void func_ov039_02097aa4(OtuWarp* self, OtuPoint* at, s32 animation) {
    self->active = 1;
    self->pos.x  = at->x;
    self->pos.y  = at->y;
    Sprite_SetAnimation(&self->sprite, self->sprite.animData, (s16)animation, self->sprite.cellTable);
}

/** True while the warp is playing. */
s32 func_ov039_02097ad8(OtuWarp* self) {
    return self->active;
}

/* ==================================================================== */
/* Tsk_OtosuGame_needlehahen                                            */
/* ==================================================================== */

SpriteFrameInfo* func_ov039_02097ae0(Sprite* sprite, s32 arg, s32 mode) {
    OtuHahen* owner = sprite->owner;

    Sprite_FrameInfoCallbackAffineSorted(sprite, mode, &owner->affine, func_ov039_02088400(3, owner->pos.y, owner->height));
}

void func_ov039_02097ba4(OtuHahen* self, Sprite* sprite, OtuPinSpriteArgs* args) {
    SpriteAnimation anim = data_ov039_02099ff8;

    anim.owner    = self;
    anim.dataType = (u16)args->dataType;
    anim.posX     = (s16)(self->pos.x >> 0xC);
    anim.posY     = (s16)(self->pos.y >> 0xC);
    _Sprite_Load(sprite, &anim);
}

s32 func_ov039_02097c30(TaskPool* pool, Task* task, void* args) {
    OtuHahen*         self = task->data;
    OtuPinSpriteArgs* a    = args;

    self->active          = 0;
    self->visible         = 0;
    self->pinId           = a->childId;
    self->origin.x        = 0;
    self->origin.y        = 0;
    self->pos.x           = 0;
    self->pos.y           = 0;
    self->height          = 0;
    self->dir.x           = 0;
    self->dir.y           = 0;
    self->affine.rotation = 0;
    self->affine.scaleX   = 0x1000;
    self->affine.scaleY   = 0x1000;
    self->affine.unk_0C   = 0;
    self->affine.unk_0E   = 0;
    func_ov039_02097ba4(self, &self->sprite, a);

    return 1;
}

/** func_ov039_0209702c at full scale, with the origin read before the tick. */
s32 func_ov039_02097c90(TaskPool* pool, Task* task, void* args) {
    OtuHahen* self = task->data;
    s32       life;
    void*     pin = EasyTask_GetTaskData(pool, self->pinId);

    if (pin != NULL) {
        if (self->active != 0) {
            func_ov039_0208e85c(pin, &self->origin);
            life       = self->life - 1;
            self->life = life;

            if (life <= 0) {
                self->active  = 0;
                self->visible = 0;
            } else {
                self->affine.scaleY = _s32_div_f(life * 0x1000, self->lifeMax);
                self->affine.scaleX = self->affine.scaleY;

                self->speed = self->speed - data_ov039_0209a304;

                if (self->speed < 0) {
                    self->speed = 0;
                }

                self->vz = self->vz + data_ov039_0209a310;
                func_ov039_02098c00(self->speed, &self->dir, &self->pos, &self->pos);

                self->height = self->height + self->vz;

                if (self->height > 0) {
                    self->height = 0;

                    if (self->vz > 0) {
                        self->vz = (s32)(((s64)self->vz * -data_ov039_0209a300 + 0x800) >> 0xC);
                    }
                }

                Sprite_Update(&self->sprite);
                self->visible = 1;
            }
        }
    } else {
        self->active  = 0;
        self->visible = 0;
    }

    return 1;
}

s32 func_ov039_02097dbc(TaskPool* pool, Task* task, void* args) {
    OtuHahen* self = task->data;

    if (self->visible != 0) {
        self->sprite.posX = (s16)((self->pos.x - self->origin.x) >> 0xC);
        self->sprite.posY = (s16)((self->pos.y - self->origin.y + self->height) >> 0xC);
        Sprite_RenderFrame(&self->sprite);
    }

    return 1;
}

s32 func_ov039_02097e0c(TaskPool* pool, Task* task, void* args) {
    Sprite_Release(&((OtuHahen*)task->data)->sprite);
    return 1;
}

s32 func_ov039_02097e20(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = func_ov039_02097c30,
        .update     = func_ov039_02097c90,
        .render     = func_ov039_02097dbc,
        .cleanup    = func_ov039_02097e0c,
    };

    return stages.iter[stage](pool, task, args);
}

s32 func_ov039_02097e68(TaskPool* pool, s32 dataType, s32 pinId) {
    OtuPinSpriteArgs args;

    args.dataType = dataType;
    args.childId  = pinId;
    return EasyTask_CreateTask(pool, &data_ov039_02099fdc, NULL, 0, NULL, &args);
}

/** func_ov039_02097240 at full scale, with one of animations 3..13. */
// Nonmatching: 87.6%, the same register allocation as func_ov039_02097240.
void func_ov039_02097e9c(OtuHahen* self, OtuPoint* at) {
    s32 index;

    self->active        = 1;
    self->affine.scaleX = 0x1000;
    self->affine.scaleY = 0x1000;
    self->pos.x         = at->x;
    self->pos.y         = at->y;
    self->height        = 0;
    self->speed         = data_ov039_0209a314;
    self->speed         = self->speed + RNG_Next(data_ov039_0209a328 - data_ov039_0209a314);
    self->vz            = 0 - data_ov039_0209a2fc;
    self->vz            = self->vz - RNG_Next(data_ov039_0209a30c - data_ov039_0209a2fc);
    self->lifeMax       = OTU_ABS_AIRTIME(self->vz);
    self->life          = self->lifeMax;
    index               = RNG_Next(0x10000) >> 4;
    self->dir.x         = ((s16*)data_0205e4e0)[index + 1];
    self->dir.y         = ((s16*)data_0205e4e0)[index];
    Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, (s16)(RNG_Next(0xB) + 3), self->sprite.cellTable);
}

/* ==================================================================== */
/* Tsk_OtosuGame_hammerhahen                                            */
/* ==================================================================== */

SpriteFrameInfo* func_ov039_02097ff4(Sprite* sprite, s32 arg, s32 mode) {
    OtuHammerHahen* owner = sprite->owner;

    Sprite_FrameInfoCallbackAffineSorted(sprite, mode, &owner->affine, func_ov039_02088400(3, owner->pos.y, owner->height));
}

void func_ov039_020980b8(OtuHammerHahen* self, Sprite* sprite, OtuPinSpriteArgs* args) {
    SpriteAnimation anim = data_ov039_0209a040;

    anim.owner    = self;
    anim.dataType = (u16)args->dataType;
    anim.posX     = self->pos.x >> 12;
    anim.posY     = self->pos.y >> 12;
    _Sprite_Load(sprite, &anim);
}

s32 func_ov039_02098144(TaskPool* pool, Task* task, void* args) {
    OtuHammerHahen*   self = task->data;
    OtuPinSpriteArgs* a    = args;

    self->active          = 0;
    self->visible         = 0;
    self->pinId           = a->childId;
    self->origin.x        = 0;
    self->origin.y        = 0;
    self->pos.x           = 0;
    self->pos.y           = 0;
    self->height          = 0;
    self->dir.x           = 0;
    self->dir.y           = 0;
    self->affine.rotation = 0;
    self->affine.scaleX   = 0x2000;
    self->affine.scaleY   = 0x2000;
    self->affine.unk_0C   = 0;
    self->affine.unk_0E   = 0;

    func_ov039_020980b8(self, &self->sprite, a);
    return 1;
}

/**
 * func_ov039_0209702c with a spin: the rotation advances by `spin` each frame.
 * One shared `return 1`, and the guard is `data != NULL` with the clear as the
 * else, which is the target's block layout.
 */
s32 func_ov039_020981a4(TaskPool* pool, Task* task, void* args) {
    OtuHammerHahen* self = task->data;
    void*           data = EasyTask_GetTaskData(pool, self->pinId);
    s32             count;
    s32             scale;
    s32             v;

    if (data != NULL) {
        if (self->active != 0) {
            func_ov039_0208e85c(data, &self->origin);

            count      = self->life - 1;
            self->life = count;

            if (count <= 0) {
                self->active  = 0;
                self->visible = 0;
            } else {
                scale               = (count << 13) / self->lifeMax;
                self->affine.scaleY = scale;
                self->affine.scaleX = scale;

                self->affine.rotation = (u16)(self->affine.rotation + self->spin);

                v           = self->speed - data_ov039_0209a304;
                self->speed = v;
                if (v < 0) {
                    self->speed = 0;
                }

                self->vz = self->vz + data_ov039_0209a310;

                func_ov039_02098c00(self->speed, &self->dir, &self->pos, &self->pos);

                v            = self->height + self->vz;
                self->height = v;
                if (v > 0) {
                    self->height = 0;
                    if (self->vz > 0) {
                        self->vz = (s32)(((s64)self->vz * -data_ov039_0209a300 + 0x800) >> 12);
                    }
                }

                Sprite_Update(&self->sprite);
                self->visible = 1;
            }
        }
    } else {
        self->active  = 0;
        self->visible = 0;
    }
    return 1;
}

s32 func_ov039_020982e8(TaskPool* pool, Task* task, void* args) {
    OtuHammerHahen* self = task->data;

    if (self->visible != 0) {
        self->sprite.posX = (s16)((self->pos.x - self->origin.x) >> 12);
        self->sprite.posY = (s16)((self->height + (self->pos.y - self->origin.y)) >> 12);
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

s32 func_ov039_02098338(TaskPool* pool, Task* task, void* args) {
    Sprite_Release(&((OtuHammerHahen*)task->data)->sprite);
    return 1;
}

s32 func_ov039_0209834c(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = data_ov039_0209a030;

    return stages.iter[stage](pool, task, args);
}
