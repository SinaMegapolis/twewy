#include "OtuFieldAccessShared.h"

/*
 * ov039 region 0x020960bc - 0x0209702c: the tail of the entry task, then the
 * dead, gameover and wricon tasks and the head of the meteohahen task. The
 * shared types, externs and prototypes are in OtuFieldAccessShared.h.
 */

s32 func_ov039_020960bc(void* pool, void* task, void* args) {
    OtuEntryTask* self = ((Task*)task)->data;

    Sprite_Release(&self->sprite);
    Sprite_Release(&self->labelSprite);
    return 1;
}

s32 func_ov039_020960dc(TaskPool* pool, Task* task, void* data, s32 stage) {
    TaskStages stages = data_ov039_02099d98;

    return stages.iter[stage](pool, task, data);
}

s32 func_ov039_02096124(TaskPool* pool, s32 dataType) {
    OtuTaskArgs1 args;

    args.dataType = dataType;
    return EasyTask_CreateTask(pool, &data_ov039_02099d8c, NULL, 0, NULL, &args);
}

/**
 * Shows the entry banner for player `which` (1 or 2) for 0x78 frames, with its
 * label sprite too when `hasLabel` is set.
 *
 * `which` is s16 because it goes to Sprite_ChangeAnimation unnarrowed. The two
 * arms are a switch: the target does both compares up front.
 */
void func_ov039_02096154(OtuEntryTask* self, s16 which, s32 hasLabel) {
    self->state            = 1;
    self->timer            = 0x78;
    self->hasLabel         = hasLabel;
    self->affine0.rotation = 0;
    self->affine0.scaleX   = 0x1000;
    self->affine0.scaleY   = 0x1000;
    self->affine0.unk_0C   = 0;
    self->affine0.unk_0E   = 0;

    switch (which) {
        case 1:
            SndMgr_StartPlayingSE(0x349);
            SndMgr_StartPlayingSE(0x55D);
            func_ov039_02087ba0(&self->scaleAnim0, data_ov039_0209a8a8, 0xC, &self->affine0);
            break;

        case 2:
            SndMgr_StartPlayingSE(0x34A);
            SndMgr_StartPlayingSE(0x55E);
            func_ov039_02087ba0(&self->scaleAnim0, data_ov039_0209a938, 0xE, &self->affine0);
            break;
    }

    Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, which, self->sprite.cellTable);

    if (self->hasLabel == 0) {
        return;
    }

    self->affine1.rotation = 0;
    self->affine1.scaleX   = 0x1000;
    self->affine1.scaleY   = 0x1000;
    self->affine1.unk_0C   = 0;
    self->affine1.unk_0E   = 0;
    func_ov039_02087ba0(&self->scaleAnim1, data_ov039_0209a830, 0xA, &self->affine1);
    Sprite_ChangeAnimation(&self->labelSprite, self->labelSprite.animData, 3, self->labelSprite.cellTable);
}

/** Hides the entry banner. */
void func_ov039_02096270(OtuEntryTask* self) {
    self->state   = 0;
    self->visible = 0;
}

/* ==================================================================== */
/* Tsk_OtosuGame_dead                                                   */
/* ==================================================================== */

SpriteFrameInfo* func_ov039_02096280(Sprite* sprite, s32 arg, s32 mode) {
    OtuDead* owner = sprite->owner;

    Sprite_FrameInfoCallbackSorted(sprite, mode, func_ov039_02088400(3, owner->pos.y, 0));
}

void func_ov039_0209633c(OtuDead* self, Sprite* sprite, OtuPinSpriteArgs* args) {
    SpriteAnimation anim = data_ov039_02099df0;

    anim.owner    = self;
    anim.dataType = (u16)args->dataType;
    anim.posX     = self->pos.x >> 12;
    anim.posY     = self->pos.y >> 12;
    _Sprite_Load(sprite, &anim);
}

s32 func_ov039_020963c8(void* pool, void* task, OtuPinSpriteArgs* args) {
    OtuDead* self = ((Task*)task)->data;

    self->pinId    = args->childId;
    self->origin.x = 0;
    self->origin.y = 0;
    self->pos.x    = 0;
    self->pos.y    = 0;
    self->visible  = 0;
    self->state    = 0;
    func_ov039_0209633c(self, &self->sprite, args);
    return 1;
}

/** Follows the pin while playing, and hides once the animation has finished. */
s32 func_ov039_02096404(void* pool, void* task, void* args) {
    OtuDead* self = ((Task*)task)->data;
    void*    pin  = EasyTask_GetTaskData(pool, self->pinId);

    if (pin != NULL) {
        switch (self->state) {
            case 0:
                self->visible = 0;
                break;

            case 1:
                func_ov039_0208e85c(pin, &self->origin);
                func_ov039_0208e6e0(pin, &self->pos);
                if (self->sprite.isPlaying != 1) {
                    self->visible = 1;
                } else {
                    self->state   = 0;
                    self->visible = 0;
                }
                break;
        }
    } else {
        self->visible = 0;
    }

    if (self->visible != 0) {
        Sprite_Update(&self->sprite);
    }
    return 1;
}

s32 func_ov039_020964a4(void* pool, void* task, void* args) {
    OtuDead* self = ((Task*)task)->data;

    if (self->visible != 0) {
        self->sprite.posX = (self->pos.x - self->origin.x) >> 12;
        self->sprite.posY = (self->pos.y - self->origin.y) >> 12;
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

s32 func_ov039_020964ec(void* pool, void* task, void* args) {
    OtuDead* self = ((Task*)task)->data;

    Sprite_Release(&self->sprite);
    return 1;
}

s32 func_ov039_02096500(TaskPool* pool, Task* task, void* data, s32 stage) {
    TaskStages stages = data_ov039_02099de0;

    return stages.iter[stage](pool, task, data);
}

s32 func_ov039_02096548(TaskPool* pool, s32 dataType, s32 pinId) {
    OtuPinSpriteArgs args;

    args.dataType = dataType;
    args.childId  = pinId;
    return EasyTask_CreateTask(pool, &data_ov039_02099dd4, NULL, 0, NULL, &args);
}

/** Plays the knockout animation. */
void func_ov039_0209657c(OtuDead* self) {
    self->state = 1;
    Sprite_SetAnimation(&self->sprite, self->sprite.animData, 4, self->sprite.cellTable);
}

/* ==================================================================== */
/* Tsk_OtosuGame_gameover                                               */
/* ==================================================================== */

SpriteFrameInfo* func_ov039_0209659c(Sprite* sprite, s32 arg, s32 mode) {
    OtuGameover* owner = sprite->owner;

    Sprite_FrameInfoCallbackAffineSorted(sprite, mode, &owner->affine, func_ov039_02088400(7, 0, 0));
}

SpriteFrameInfo* func_ov039_02096660(Sprite* sprite, s32 arg, s32 mode) {
    Sprite_FrameInfoCallbackSorted(sprite, mode, func_ov039_02088400(7, 0, 0));
}

void func_ov039_02096718(OtuGameover* self, Sprite* sprite, OtuTaskArgs1* args) {
    SpriteAnimation anim = data_ov039_02099e38;

    anim.owner    = self;
    anim.dataType = (u16)args->dataType;
    _Sprite_Load(sprite, &anim);
}

void func_ov039_0209678c(OtuGameover* self, Sprite* sprite, OtuTaskArgs1* args) {
    SpriteAnimation anim = data_ov039_02099e64;

    anim.owner    = self;
    anim.dataType = (u16)args->dataType;
    _Sprite_Load(sprite, &anim);
}

void func_ov039_02096800(OtuGameover* self, Sprite* sprite, OtuTaskArgs1* args) {
    SpriteAnimation anim = data_ov039_02099e90;

    anim.owner    = self;
    anim.dataType = (u16)args->dataType;
    _Sprite_Load(sprite, &anim);
}

s32 func_ov039_02096874(void* pool, void* task, OtuTaskArgs1* args) {
    OtuGameover* self = ((Task*)task)->data;

    self->state   = 0;
    self->visible = 0;
    func_ov039_02096718(self, &self->sprite0, args);
    func_ov039_0209678c(self, &self->sprite1, args);
    func_ov039_02096800(self, &self->sprite2, args);
    return 1;
}

/**
 * Runs the banner's sequence, 0x3C frames per state: 1 fades the main display,
 * 2 plays the sound, 3 fades both displays and finishes. While visible it steps
 * the scale keyframes and the selected sprite.
 */
s32 func_ov039_020968c0(void* pool, void* task, void* args) {
    OtuGameover* self = ((Task*)task)->data;

    switch (self->state) {
        case 0:
            self->visible = 0;
            break;

        case 1:
            self->visible = 1;
            EasyFade_FadeMainDisplay(FADER_INSTANT, 0, 0x3C);
            self->counter = self->counter - 1;
            if (self->counter <= 0) {
                self->counter = 0x3C;
                self->state   = 2;
            }
            break;

        case 2:
            self->visible = 1;
            self->counter = self->counter - 1;
            if (self->counter <= 0) {
                SndMgr_StartPlayingSE(0x34C);
                self->counter = 0x3C;
                self->state   = 3;
            }
            break;

        case 3:
            self->visible = 1;
            EasyFade_FadeBothDisplays(FADER_INSTANT, 0x10, 0x3C);
            self->counter = self->counter - 1;
            if (self->counter <= 0) {
                self->state = 0;
            }
            break;
    }

    if (self->visible != 0) {
        func_ov039_02087bf8(&self->scaleAnim, &self->affine);

        switch (self->which) {
            case 0:
                Sprite_Update(&self->sprite0);
                break;

            case 1:
            case 2:
                Sprite_Update(&self->sprite1);
                break;

            case 3:
                Sprite_Update(&self->sprite2);
                break;
        }
    }
    return 1;
}

/** Draws the selected sprite; sprite0 through an affine group for its scale. */
// Nonmatching: 83%, the scheduling of the OamMgr_AllocAffineGroup call in case
// 0: the target forms the manager address before storing the stack argument.
s32 func_ov039_020969fc(void* pool, void* task, void* args) {
    OtuGameover* self = ((Task*)task)->data;

    if (self->visible != 0) {
        switch (self->which) {
            case 0:
                self->sprite0.unk_0A.unk_05 = (u16)OamMgr_AllocAffineGroup(&g_OamMgr[self->sprite0.bits_0_1], 0,
                                                                           self->affine.scaleX, self->affine.scaleY, 0);
                Sprite_RenderFrame(&self->sprite0);
                break;

            case 1:
            case 2:
                Sprite_RenderFrame(&self->sprite1);
                break;

            case 3:
                Sprite_RenderFrame(&self->sprite2);
                break;
        }
    }
    return 1;
}

s32 func_ov039_02096ab0(void* pool, void* task, void* args) {
    OtuGameover* self = ((Task*)task)->data;

    Sprite_Release(&self->sprite0);
    Sprite_Release(&self->sprite1);
    return 1;
}

s32 func_ov039_02096ad0(TaskPool* pool, Task* task, void* data, s32 stage) {
    TaskStages stages = data_ov039_02099e28;

    return stages.iter[stage](pool, task, data);
}

s32 func_ov039_02096b18(TaskPool* pool, s32 dataType) {
    OtuTaskArgs1 args;

    args.dataType = dataType;
    return EasyTask_CreateTask(pool, &data_ov039_02099e1c, NULL, 0, NULL, &args);
}

/** Starts the banner showing `which` (see OtuGameover.which). */
void func_ov039_02096b48(OtuGameover* self, s32 which) {
    self->state   = 1;
    self->counter = 0x3C;
    self->which   = which;

    self->affine.rotation = 0;
    self->affine.scaleX   = 0x1000;
    self->affine.scaleY   = 0x1000;
    self->affine.unk_0C   = 0;
    self->affine.unk_0E   = 0;
    func_ov039_02087ba0(&self->scaleAnim, data_ov039_0209aa0c, 0xC, &self->affine);

    switch (which) {
        case 0:
            SndMgr_StartPlayingSE(0x55F);
            Sprite_ChangeAnimation(&self->sprite0, self->sprite0.animData, 1, self->sprite0.cellTable);
            break;

        case 1:
            SndMgr_StartPlayingSE(0x560);
            Sprite_ChangeAnimation(&self->sprite1, self->sprite1.animData, 1, self->sprite1.cellTable);
            break;

        case 2:
            SndMgr_StartPlayingSE(0x561);
            Sprite_ChangeAnimation(&self->sprite1, self->sprite1.animData, 2, self->sprite1.cellTable);
            break;

        case 3:
            Sprite_ChangeAnimation(&self->sprite2, self->sprite2.animData, 1, self->sprite2.cellTable);
            break;
    }

    SndMgr_StartPlayingSE(0x34B);
}

/** True while the banner's sequence is running. */
s32 func_ov039_02096c44(OtuGameover* self) {
    return self->state != 0;
}

/* ==================================================================== */
/* Tsk_OtosuGame_wricon                                                 */
/* ==================================================================== */

SpriteFrameInfo* func_ov039_02096c58(Sprite* sprite, s32 arg, s32 mode) {
    Sprite_FrameInfoCallbackSorted(sprite, mode, 3);
}

void func_ov039_02096d00(OtuWricon* self, Sprite* sprite) {
    SpriteAnimation anim = data_ov039_02099ed8;

    anim.owner    = self;
    anim.dataType = (u16)self->dataType;
    _Sprite_Load(sprite, &anim);
}

s32 func_ov039_02096d70(void* pool, void* task, OtuTaskArgs1* args) {
    OtuWricon* self = ((Task*)task)->data;

    self->visible  = 1;
    self->dataType = args->dataType;
    func_ov039_02096d00(self, &self->sprite);
    return 1;
}

s32 func_ov039_02096d98(void) {
    return 1;
}

/**
 * Shows the link strength: animation `5 - level`, where ov040 reports the
 * level. Narrowed inline so it goes straight into the argument register.
 */
s32 func_ov039_02096da0(void* pool, void* task, void* args) {
    OtuWricon* self = ((Task*)task)->data;

    if (self->visible != 0) {
        Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, (s16)(5 - func_ov040_0209cb5c()), self->sprite.cellTable);
        Sprite_Update(&self->sprite);
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

s32 func_ov039_02096dec(void* pool, void* task, void* args) {
    OtuWricon* self = ((Task*)task)->data;

    Sprite_Release(&self->sprite);
    return 1;
}

s32 func_ov039_02096e04(TaskPool* pool, Task* task, void* data, s32 stage) {
    TaskStages stages = data_ov039_02099ec8;

    return stages.iter[stage](pool, task, data);
}

s32 func_ov039_02096e4c(TaskPool* pool, s32 dataType) {
    OtuTaskArgs1 args;

    args.dataType = dataType;
    return EasyTask_CreateTask(pool, &data_ov039_02099ebc, NULL, 0, NULL, &args);
}

/* ==================================================================== */
/* Tsk_OtosuGame_meteohahen (continued in OtuSpriteTasks.c)             */
/* ==================================================================== */

SpriteFrameInfo* func_ov039_02096e7c(Sprite* sprite, s32 arg, s32 mode) {
    OtuHahen* owner = sprite->owner;

    Sprite_FrameInfoCallbackAffineSorted(sprite, mode, &owner->affine, func_ov039_02088400(3, owner->pos.y, owner->height));
}

void func_ov039_02096f40(OtuHahen* self, Sprite* sprite, OtuPinSpriteArgs* args) {
    SpriteAnimation anim = data_ov039_02099f20;

    anim.owner    = self;
    anim.dataType = (u16)args->dataType;
    anim.posX     = self->pos.x >> 12;
    anim.posY     = self->pos.y >> 12;
    _Sprite_Load(sprite, &anim);
}

/** The stores are in the target's order: the flags, then the pin, then the rest. */
s32 func_ov039_02096fcc(TaskPool* pool, Task* task, void* args) {
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
    self->affine.scaleX   = 0x1800;
    self->affine.scaleY   = 0x1800;
    self->affine.unk_0C   = 0;
    self->affine.unk_0E   = 0;
    func_ov039_02096f40(self, &self->sprite, a);
    return 1;
}
