/**
 * @file OtosuGame_entry.c
 * @brief The `Tsk_OtosuGame_entry` task.
 */

#include "OtuFieldAccessShared.h"

SpriteFrameInfo* func_ov039_02095dec(Sprite* sprite, s32 arg, s32 mode) {
    Sprite_FrameInfoCallbackSorted(sprite, mode, func_ov039_02088400(7, 0, 0));
}

void func_ov039_02095ea4(OtuEntryTask* self, Sprite* sprite, OtuTaskArgs1* args) {
    SpriteAnimation anim = data_ov039_02099da8;

    anim.owner    = self;
    anim.dataType = args->dataType;

    _Sprite_Load(sprite, &anim);
}

s32 func_ov039_02095f18(TaskPool* pool, Task* task, void* args) {
    OtuEntryTask* self = task->data;
    OtuTaskArgs1* a    = args;

    self->state = 0;

    func_ov039_02095ea4(self, &self->sprite, a);
    func_ov039_02095ea4(self, &self->labelSprite, a);

    self->labelSprite.posY = 0x91;
    return 1;
}

s32 func_ov039_02095f58(TaskPool* pool, Task* task, void* args) {
    OtuEntryTask* self = task->data;

    switch (self->state) {
        case 0:
            self->visible = 0;
            break;

        case 1:
            func_ov039_02087bf8(&self->scaleAnim0, &self->affine0);
            Sprite_Update(&self->sprite);

            if (self->hasLabel != 0) {
                func_ov039_02087bf8(&self->scaleAnim1, &self->affine1);
                Sprite_Update(&self->labelSprite);
            }

            self->timer = self->timer - 1;

            if (self->timer > 0) {
                self->visible = 1;
            } else {
                self->state   = 0;
                self->visible = 0;
            }
            break;
    }
    return 1;
}

/**
 * Draws both sprites through affine groups sized by their scale keyframes;
 * the group index goes into bits 5..9 of the OAM attributes.
 */
// Nonmatching: 72.6%, argument scheduling of OamMgr_AllocAffineGroup: the
// target forms the manager address before spilling the fifth argument (the
// same gap Shop_item2.c carries for this expression).
s32 func_ov039_02095fe4(TaskPool* pool, Task* task, void* args) {
    OtuEntryTask* self = task->data;

    if (self->visible != 0) {
        self->sprite.unk_0A.unk_05 =
            (u16)OamMgr_AllocAffineGroup(&g_OamMgr[self->sprite.bits_0_1], 0, self->affine0.scaleX, self->affine0.scaleY, 0);
        Sprite_RenderFrame(&self->sprite);

        if (self->hasLabel != 0) {
            self->labelSprite.unk_0A.unk_05 = (u16)OamMgr_AllocAffineGroup(&g_OamMgr[self->labelSprite.bits_0_1], 0,
                                                                           self->affine1.scaleX, self->affine1.scaleY, 0);
            Sprite_RenderFrame(&self->labelSprite);
        }
    }
    return 1;
}

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
