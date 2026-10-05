/**
 * @file OtosuGame_gameover.c
 * @brief The `Tsk_OtosuGame_gameover` task.
 */

#include "OtuFieldAccessShared.h"

extern const TaskHandle      data_ov039_02099e1c;
extern const TaskStages      data_ov039_02099e28;
extern const SpriteAnimation data_ov039_02099e38;
extern const SpriteAnimation data_ov039_02099e64;
extern const SpriteAnimation data_ov039_02099e90;
extern const OtuScaleKey     data_ov039_0209aa0c[];

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
