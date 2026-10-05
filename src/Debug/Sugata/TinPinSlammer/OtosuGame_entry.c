/**
 * @file OtosuGame_entry.c
 * @brief The `Tsk_OtosuGame_entry` task.
 */

#include "OtuFieldAccessShared.h"

s32              OtosuGame_entry_RunTask(TaskPool* pool, Task* task, void* data, s32 stage);
SpriteFrameInfo* OtosuGame_entry_GetFrameInfo(Sprite* sprite, s32 arg, s32 mode);

static const TaskHandle Tsk_OtosuGame_entry = {"Tsk_OtosuGame_entry", OtosuGame_entry_RunTask, sizeof(OtosuGame_entry)};

static const SpriteAnimation OtosuGame_entry_Anim = {
    .bits_0_1          = 0,
    .dataType          = 0,
    .bit_6             = 0,
    .bits_7_9          = 5,
    .bits_10_11        = 0,
    .bits_12_13        = 2,
    .bits_14_15        = 0,
    .unk_02.raw        = 1,
    .posX              = 0x80,
    .posY              = 0x57,
    .frameInfoCallback = OtosuGame_entry_GetFrameInfo,
    .callbackArg       = 0,
    .owner             = NULL,
    .binIden           = &data_ov039_0209a0ec,
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

static OtuScaleKey data_ov039_0209a830[10] = {
    {16, 0x1000, 0x1000},
    { 2, 0x1000,  0x333},
    { 2, 0x1000,  0x4CD},
    { 2, 0x1000,  0x666},
    { 2, 0x1000,  0x800},
    { 3, 0x1000,  0x99A},
    { 3, 0x1000,  0xB33},
    { 3, 0x1000,  0xCCD},
    { 3, 0x1000,  0xE66},
    {50, 0x1000, 0x1000},
};

static OtuScaleKey data_ov039_0209a8a8[12] = {
    { 1,  0x666,  0x666},
    { 1,  0x99A,  0x99A},
    { 2,  0xCCD,  0xCCD},
    { 8, 0x1000, 0x14CD},
    { 3,  0xCCD,  0xE66},
    { 4,  0xF33, 0x1333},
    { 3,  0xCCD,  0xE66},
    { 4,  0xFAE, 0x1266},
    { 3,  0xE66,  0xE66},
    { 4, 0x1000, 0x119A},
    { 3,  0xF33,  0xF33},
    {50, 0x1000, 0x1000},
};

static OtuScaleKey data_ov039_0209a938[14] = {
    { 1,  0x666,  0x666},
    { 1,  0x99A,  0x99A},
    { 2,  0xCCD,  0xCCD},
    { 8, 0x1000, 0x14CD},
    { 3,  0xCCD,  0xE66},
    { 4,  0xF33, 0x1333},
    { 3,  0xCCD,  0xE66},
    { 4,  0xFAE, 0x1266},
    { 3,  0xF33,  0xF33},
    {15, 0x1000, 0x1000},
    { 6, 0x1000, 0x1333},
    { 1, 0x1000,  0x800},
    { 1,  0x99A,  0x148},
    { 3, 0x1000, 0x1000},
};

/* The scale keyframe tables the entry, dead and gameover tasks animate with. */

SpriteFrameInfo* OtosuGame_entry_GetFrameInfo(Sprite* sprite, s32 arg, s32 mode) {
    Sprite_FrameInfoCallbackSorted(sprite, mode, func_ov039_02088400(7, 0, 0));
}

void OtosuGame_entry_Load(OtosuGame_entry* self, Sprite* sprite, OtuTaskArgs1* args) {
    SpriteAnimation anim = OtosuGame_entry_Anim;

    anim.owner    = self;
    anim.dataType = args->dataType;

    _Sprite_Load(sprite, &anim);
}

s32 OtosuGame_entry_Init(TaskPool* pool, Task* task, void* args) {
    OtosuGame_entry* self = task->data;
    OtuTaskArgs1*    a    = args;

    self->state = 0;

    OtosuGame_entry_Load(self, &self->sprite, a);
    OtosuGame_entry_Load(self, &self->labelSprite, a);

    self->labelSprite.posY = 0x91;
    return 1;
}

s32 OtosuGame_entry_Update(TaskPool* pool, Task* task, void* args) {
    OtosuGame_entry* self = task->data;

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
s32 OtosuGame_entry_Render(TaskPool* pool, Task* task, void* args) {
    OtosuGame_entry* self = task->data;

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

s32 OtosuGame_entry_Destroy(TaskPool* pool, Task* task, void* args) {
    OtosuGame_entry* self = ((Task*)task)->data;

    Sprite_Release(&self->sprite);
    Sprite_Release(&self->labelSprite);
    return 1;
}

s32 OtosuGame_entry_RunTask(TaskPool* pool, Task* task, void* data, s32 stage) {
    TaskStages stages = {
        .initialize = OtosuGame_entry_Init,
        .update     = OtosuGame_entry_Update,
        .render     = OtosuGame_entry_Render,
        .cleanup    = OtosuGame_entry_Destroy,
    };

    return stages.iter[stage](pool, task, data);
}

s32 OtosuGame_entry_CreateTask(TaskPool* pool, s32 dataType) {
    OtuTaskArgs1 args;

    args.dataType = dataType;
    return EasyTask_CreateTask(pool, &Tsk_OtosuGame_entry, NULL, 0, NULL, &args);
}

/**
 * Shows the entry banner for player `which` (1 or 2) for 0x78 frames, with its
 * label sprite too when `hasLabel` is set.
 *
 * `which` is s16 because it goes to Sprite_ChangeAnimation unnarrowed. The two
 * arms are a switch: the target does both compares up front.
 */
void func_ov039_02096154(OtosuGame_entry* self, s16 which, s32 hasLabel) {
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
void func_ov039_02096270(OtosuGame_entry* self) {
    self->state   = 0;
    self->visible = 0;
}
