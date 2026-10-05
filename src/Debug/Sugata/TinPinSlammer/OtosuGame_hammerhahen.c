/**
 * @file OtosuGame_hammerhahen.c
 * @brief The `Tsk_OtosuGame_hammerhahen` task.
 */

#include "OtuFieldAccessShared.h"

/** The handle this spawns. Already in the overlay's .rodata. */
extern const TaskHandle Tsk_OtosuGame_hammerhahen;
extern const TaskStages data_ov039_0209a030;

extern const SpriteAnimation OtosuGame_hammerhahen_Anim;

SpriteFrameInfo* OtosuGame_hammerhahen_GetFrameInfo(Sprite* sprite, s32 arg, s32 mode) {
    OtosuGame_hammerhahen* owner = sprite->owner;

    Sprite_FrameInfoCallbackAffineSorted(sprite, mode, &owner->affine, func_ov039_02088400(3, owner->pos.y, owner->height));
}

void OtosuGame_hammerhahen_Load(OtosuGame_hammerhahen* self, Sprite* sprite, OtuPinSpriteArgs* args) {
    SpriteAnimation anim = OtosuGame_hammerhahen_Anim;

    anim.owner    = self;
    anim.dataType = (u16)args->dataType;
    anim.posX     = self->pos.x >> 12;
    anim.posY     = self->pos.y >> 12;
    _Sprite_Load(sprite, &anim);
}

s32 OtosuGame_hammerhahen_Init(TaskPool* pool, Task* task, void* args) {
    OtosuGame_hammerhahen* self = task->data;
    OtuPinSpriteArgs*      a    = args;

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

    OtosuGame_hammerhahen_Load(self, &self->sprite, a);
    return 1;
}

/**
 * OtosuGame_meteohahen_Update with a spin: the rotation advances by `spin` each frame.
 * One shared `return 1`, and the guard is `data != NULL` with the clear as the
 * else, which is the target's block layout.
 */
s32 OtosuGame_hammerhahen_Update(TaskPool* pool, Task* task, void* args) {
    OtosuGame_hammerhahen* self = task->data;
    void*                  data = EasyTask_GetTaskData(pool, self->pinId);
    s32                    count;
    s32                    scale;
    s32                    v;

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

s32 OtosuGame_hammerhahen_Render(TaskPool* pool, Task* task, void* args) {
    OtosuGame_hammerhahen* self = task->data;

    if (self->visible != 0) {
        self->sprite.posX = (s16)((self->pos.x - self->origin.x) >> 12);
        self->sprite.posY = (s16)((self->height + (self->pos.y - self->origin.y)) >> 12);
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

s32 OtosuGame_hammerhahen_Destroy(TaskPool* pool, Task* task, void* args) {
    Sprite_Release(&((OtosuGame_hammerhahen*)task->data)->sprite);
    return 1;
}

s32 OtosuGame_hammerhahen_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = data_ov039_0209a030;

    return stages.iter[stage](pool, task, args);
}

s32 OtosuGame_hammerhahen_CreateTask(TaskPool* pool, s32 dataType, s32 pinId) {
    OtuPinSpriteArgs args;

    args.dataType = dataType;
    args.childId  = pinId;

    return EasyTask_CreateTask(pool, &Tsk_OtosuGame_hammerhahen, NULL, 0, NULL, &args);
}

/**
 * Launches a hammer fragment from `at` with the hammer's current affine, a
 * random spin, and animation 2 for the first fragment and 1 for the rest.
 */
// Nonmatching: 90.6%. The target loads `at`'s halves with two `ldr`s; this
// build merges them into an `ldmia`, which also swaps their registers.
void func_ov039_020983c8(OtosuGame_hammerhahen* self, OtuPoint* at, OamAffineParam* affine, s32 index) {
    s32 ax;
    s32 ay;
    s32 cell;
    s32 airtime;

    self->active = 1;
    self->affine = *affine;

    ax = at->x;
    ay = at->y;

    self->pos.x  = ax;
    self->pos.y  = ay;
    self->height = 0;

    self->speed = data_ov039_0209a314;
    self->speed = self->speed + RNG_Next(data_ov039_0209a328 - data_ov039_0209a314);

    self->vz = -data_ov039_0209a2fc;
    self->vz = self->vz - RNG_Next(data_ov039_0209a30c - data_ov039_0209a2fc);

    airtime = OTU_ABS_AIRTIME(self->vz);

    self->lifeMax = airtime;
    self->life    = airtime;

    cell        = RNG_Next(0x10000) >> 4;
    self->dir.x = ((s16*)data_0205e4e0)[cell * 2 + 1];
    self->dir.y = ((s16*)data_0205e4e0)[cell * 2];

    self->spin = RNG_Next(0x800) + 0x800;

    Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, (s16)(index == 0 ? 2 : 1), self->sprite.cellTable);
}
