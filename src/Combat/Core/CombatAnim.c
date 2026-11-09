#include "Combat/Core/CombatAnim.h"
#include "Battle/Core/BtlActor.h"
#include "Battle/Core/BtlSprite.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatActorParams.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatBgAnimSlot.h"
#include "Combat/Core/CombatBgTiles.h"
#include "Engine/Core/DMA.h"
#include "Engine/File/DatMgr.h"
#include "Engine/Math/Random.h"
#include "Engine/Resources/PaletteMgr.h"
#include "Engine/Resources/ScreenMapper.h"
#include "SpriteMgr.h"

/// One 6-byte animation record: which pack entry to show, for how long, and when the animation
/// switches palettes (`entry == -2`) or ends (`entry == -1`).

/// One sprite slot of a battle actor's graphic set.

/// Common prefix of every battle unit that owns a combat sprite: the `BattleSprite` sits at 0x84.

/// Points `sprite` (the sprite embedded at 0x84 of `owner`) at `cmd->table[index]` and gives it a
/// new palette. Returns 0 when the animation could not be loaded.
static s32 func_ov003_020d36fc(s32 engine, Ov003EnemyActor* owner, const Ov003SpriteLoadCmd* cmd, u16 index, u16 arg4,
                               void* paletteSrc, u16 palIdx, u16 flip) {
    Sprite* sprite = &owner->sprite.sprite;
    s32 result = BtlSprite_LoadFromTable(engine, &owner->sprite, (const BinIdentifier*)&cmd->entries[index], cmd->table, arg4,
                                         0, cmd->unk_0C);

    if (result == 0) {
        return 0;
    }
    Sprite_ChangePalette(sprite, engine, paletteSrc, 5, palIdx);
    sprite->animationMode = (flip == 0) ? ANIM_MODE_LOOPED : ANIM_MODE_ONCE;
    func_ov003_020c4988(owner);
    return result;
}

/// Releases the current animation of `owner`'s sprite and starts `cmd->table[index]` with the
/// palette the sprite had before the switch.
// Nonmatching: the palette entry count and the engine index land in the opposite callee-saved
// registers (r9/r10).
static s32 func_ov003_020d37a0(Ov003EnemyActor* owner, const Ov003SpriteLoadCmd* cmd, u16 index, u16 arg3, u16 flip) {
    Sprite*           sprite  = &owner->sprite.sprite;
    PaletteResource*  palette = sprite->paletteData;
    UnkSmallInternal* palSrc  = sprite->unk3C;
    s32               engine  = sprite->bits_0_1;
    u16               count   = palette->chunkCount;

    Sprite_Release(sprite);
    BtlSprite_LoadFromTable(engine, &owner->sprite, (const BinIdentifier*)&cmd->entries[index], cmd->table, arg3, 0,
                            cmd->unk_0C);
    Sprite_ChangePalette(sprite, engine, palSrc, 5, count);
    sprite->animationMode = (flip == 0) ? ANIM_MODE_LOOPED : ANIM_MODE_ONCE;
    return 0;
}

/// Starts the first frame of an animation script on `owner`'s sprite.
void func_ov003_020d3858(s32 engine, Ov003EnemyActor* owner, const Ov003SpriteLoadCmd* cmd, Ov003AnimScript* script,
                         const Ov003AnimScriptStep* steps, void* paletteSrc, u16 palIdx, u16 flip) {
    script->steps = steps;
    script->index = 0;
    script->loop  = flip;
    script->flags = 0;
    func_ov003_020d36fc(engine, owner, cmd, steps->entry, steps->dataId, paletteSrc, palIdx, 1);
}

/// Advances an animation script once the sprite's current frame has played out.
// Nonmatching: the target keeps the step pointer in r7 and reuses r7 for the result; we keep it in
// a caller-saved register.
s32 func_ov003_020d38ac(Ov003EnemyActor* owner, const Ov003SpriteLoadCmd* cmd, Ov003AnimScript* script) {
    const Ov003AnimScriptStep* step;
    s32                        result = 0;

    script->flags &= ~2;
    if (script->flags & 1) {
        return result;
    }
    if (SpriteMgr_IsAnimationFinished(&owner->sprite.sprite)) {
        script->index++;
        step = &script->steps[script->index];
        if (step->dataId == -1) {
            if (script->loop == 0) {
                script->index = 0;
                step          = script->steps;
                script->flags |= 2;
            } else {
                script->flags |= 1;
                return result;
            }
        }
        result = func_ov003_020d37a0(owner, cmd, step->entry, step->dataId, 1);
    }
    return result;
}

/// Starts `steps` on `owner`'s sprite without touching `paletteSrc`.
void func_ov003_020d3978(Ov003EnemyActor* owner, const Ov003SpriteLoadCmd* cmd, Ov003AnimScript* script,
                         const Ov003AnimScriptStep* steps, u16 loop) {
    script->steps = steps;
    script->index = 0;
    script->loop  = loop;
    script->flags = 0;
    func_ov003_020d37a0(owner, cmd, steps->entry, steps->dataId, 1);
}

s32 func_ov003_020d39ac(Ov003AnimScript* script) {
    if (script->flags & 1) {
        return 1;
    }
    return 0;
}

/// Loads the cell data of `layer`'s current frame and hands the result to the layer's dest slot,
/// then mirrors the frame index down the layer chain.
static void func_ov003_020d39c0(Ov003AnimLayer* layer) {
    const Ov003AnimFrame* frame = &layer->frames[layer->index];
    void*                 source;

    if (layer->loadedId != frame->dataId) {
        if (layer->data != NULL) {
            DatMgr_ReleaseData(layer->data);
        }
        layer->data     = DatMgr_LoadPackEntry(1, NULL, 0, layer->iden, frame->dataId, 0);
        layer->loadedId = frame->dataId;
    }
    source       = Data_GetPackEntryData(layer->data, frame->entry);
    *layer->dest = ((CombatBgTiles*)source)->pixels;
    func_0200d244(layer->unk_08);
    if (layer->next != NULL) {
        layer->next->index = layer->index;
        func_ov003_020d39c0(layer->next);
    }
}

/// Steps `layer` to the next frame, or back to frame 0 when the end marker is reached.
static s16 func_ov003_020d3a90(Ov003AnimLayer* layer) {
    s16 index = layer->index;

    if (layer->frames[index + 1].entry == -1) {
        if (layer->loop == 1) {
            index = 0;
        } else {
            layer->ended = 1;
        }
    } else {
        index++;
    }
    return index;
}

/// Returns how many frames `layer`'s current entry lasts (randomised for `random` layers).
static s16 func_ov003_020d3ae0(Ov003AnimLayer* layer) {
    u16                   value;
    const Ov003AnimFrame* frame = &layer->frames[layer->index];

    value = frame->duration;

    if (layer->random == 1) {
        if (value != 0) {
            value = value + RNG_Next(frame->randomRange);
        } else if ((s32)RNG_Next(0x64) < frame->randomRange) {
            value = 1;
        } else {
            layer->index = func_ov003_020d3a90(layer);
            value        = func_ov003_020d3ae0(layer);
        }
    }
    return value;
}

/// Sets up `layer` around an already-loaded `data` block.
void CombatAnim_LayerInitLoaded(Ov003AnimLayer* layer, Data* data, s16 loadedId, u32 loop, u32 oneShot, u32 random) {
    layer->loadedId = loadedId;
    layer->data     = data;
    layer->loop     = (u16)loop;
    layer->oneShot  = (u16)oneShot;
    layer->random   = (u16)random;
    layer->ended    = 0;
    layer->chained  = 0;
}

/// Initialises `layer` and loads its first frame.
void CombatAnim_LayerInit(Ov003AnimLayer* layer, BinIdentifier* iden, s32 unk_08, void** dest, const Ov003AnimFrame* frames) {
    layer->iden     = iden;
    layer->unk_08   = unk_08;
    layer->dest     = dest;
    layer->frames   = frames;
    layer->index    = 0;
    layer->timer    = 0;
    layer->next     = NULL;
    layer->duration = func_ov003_020d3ae0(layer);
    func_ov003_020d39c0(layer);
}

/// Swaps `layer` to a different frame table.
void CombatAnim_LayerSetFrames(Ov003AnimLayer* layer, const Ov003AnimFrame* frames) {
    if (layer->data != NULL) {
        DatMgr_ReleaseData(layer->data);
        layer->data = NULL;
    }
    layer->loadedId = 0;
    layer->frames   = frames;
    layer->index    = 0;
    layer->timer    = 0;
    layer->next     = NULL;
    layer->chained  = 0;
    layer->ended    = 0;
    layer->duration = func_ov003_020d3ae0(layer);
    func_ov003_020d39c0(layer);
}

/// Chains `next` behind `layer` so it follows the parent's frame index.
void CombatAnim_LayerChain(Ov003AnimLayer* layer, Ov003AnimLayer* next) {
    layer->next   = next;
    next->chained = 1;
}

/// Ticks `layer`, advancing to the next frame when the current one has played out.
void CombatAnim_LayerUpdate(Ov003AnimLayer* layer) {
    if (layer->timer >= layer->duration) {
        layer->timer    = 0;
        layer->index    = func_ov003_020d3a90(layer);
        layer->duration = func_ov003_020d3ae0(layer);
        func_ov003_020d39c0(layer);
    }
    layer->timer++;
}

void CombatAnim_LayerRelease(Ov003AnimLayer* layer) {
    if (layer->data != NULL) {
        DatMgr_ReleaseData(layer->data);
    }
}

/// Copies the BG tiles of `slot`'s current frame to its BG char VRAM destination.
static void CombatAnim_SlotUploadChars(Ov003BgAnimSlot* slot) {
    const Ov003BgAnimFrame* anim = &slot->anims[slot->animIndex];
    s32                     size;
    void*                   source;

    if (slot->charData != NULL) {
        DatMgr_ReleaseData(slot->charData);
    }
    slot->charData = DatMgr_LoadPackEntry(1, NULL, 0, slot->charIden, anim->dataId, 0);
    source         = Data_GetPackEntryData(slot->charData, anim->entry);
    size           = (((const CombatBgTiles*)source)->header & ~0xFF) >> 8;
    if ((((const CombatBgTiles*)source)->format & 0xF0) == 0) {
        size -= 4;
    }
    slot->dataId = anim->dataId;
    func_02001b44(7, (s32)slot->charDest, ((const CombatBgTiles*)source)->pixels, size);
}

/// Reloads `slot`'s palette from the current frame, if the slot has one. Returns the new palette resource.
// Nonmatching: register allocation around the allocation (the palette-manager base and the
// start/count temporaries are assigned differently).
PaletteResource* CombatAnim_SlotReloadPalette(Ov003BgAnimSlot* slot) {
    const Ov003BgAnimFrame* anim = &slot->anims[slot->animIndex];
    void*                   source;

    if (!slot->hasPal) {
        return NULL;
    }
    if (slot->palette != NULL) {
        PaletteMgr_ReleaseResource(g_PaletteManagers[slot->palEngine], slot->palette);
    }
    if (slot->paletteData != NULL) {
        DatMgr_ReleaseData(slot->paletteData);
    }
    slot->paletteData = DatMgr_LoadPackEntry(1, NULL, 0, slot->palIden, anim->dataId, 0);
    source            = Data_GetPackEntryData(slot->paletteData, anim->entry);
    slot->palette     = PaletteMgr_AllocPalette(g_PaletteManagers[slot->palEngine], source, 0, slot->palStart, slot->palCount);
    return slot->palette;
}

/// Sets `slot` up to animate `anims` into `charDest`; nothing is uploaded until the first update.
void CombatAnim_SlotInit(Ov003BgAnimSlot* slot, BinIdentifier* iden, const Ov003BgAnimFrame* anims, void* charDest, u32 loop,
                         u32 disabled) {
    if (anims == NULL) {
        return;
    }
    slot->charIden    = iden;
    slot->anims       = anims;
    slot->charData    = NULL;
    slot->charDest    = charDest;
    slot->paletteData = NULL;
    slot->palette     = NULL;
    slot->dataId      = -1;
    slot->animIndex   = 0;
    slot->timer       = 0;
    slot->hasPal      = 0;
    slot->loop        = (u16)loop;
    slot->disabled    = (u16)disabled;
    slot->finished    = 0;
}

/// Gives `slot` its own palette from the pack `iden`, loaded at the first palette switch marker.
void CombatAnim_SlotSetPalette(Ov003BgAnimSlot* slot, PaletteResource* palette, BinIdentifier* iden, u32 engine, u8 palStart,
                               u8 palCount) {
    slot->palIden     = iden;
    slot->paletteData = NULL;
    slot->palette     = palette;
    slot->palEngine   = (u16)engine;
    slot->palStart    = palStart;
    slot->palCount    = palCount;
    slot->hasPal      = 1;
}

/// Swaps `slot` to a different animation table without reloading anything else.
void CombatAnim_SlotSetAnims(Ov003BgAnimSlot* slot, const Ov003BgAnimFrame* anims) {
    if (anims == NULL) {
        return;
    }
    slot->anims     = anims;
    slot->animIndex = 0;
    slot->timer     = 0;
    slot->finished  = 0;
}

/// Advances `slot`'s animation while it has one, uploading tiles and switching palettes on markers.
// Nonmatching: the target lays the palette-marker (-2) case out of line and falls through into the
// end-marker (-1) case; ours has the two cases the other way round.
void CombatAnim_SlotUpdate(Ov003BgAnimSlot* slot) {
    const Ov003BgAnimFrame* anim;
    s32                     run;

    if (slot->anims == NULL) {
        return;
    }
    if (slot->disabled != 1 && slot->finished != 1) {
        anim = &slot->anims[slot->animIndex];
        if (slot->timer == 0) {
            do {
                if (anim->entry == -1) {
                    if (slot->loop == 1) {
                        slot->animIndex = 0;
                        run             = 1;
                        anim            = &slot->anims[slot->animIndex];
                    } else {
                        run            = 0;
                        slot->finished = 1;
                    }
                } else if (anim->entry == -2) {
                    slot->animIndex++;
                    CombatAnim_SlotReloadPalette(slot);
                    run = 1;
                    anim += 2;
                    slot->animIndex++;
                } else {
                    CombatAnim_SlotUploadChars(slot);
                    run         = 0;
                    slot->timer = anim->duration;
                }
            } while (run != 0);
        }
        slot->timer--;
        if (slot->timer == 0) {
            slot->animIndex++;
        }
    }
}

void CombatAnim_SlotRelease(Ov003BgAnimSlot* slot) {
    if (slot->anims == NULL) {
        return;
    }
    if (slot->charData != NULL) {
        DatMgr_ReleaseData(slot->charData);
    }
    if (slot->paletteData != NULL) {
        DatMgr_ReleaseData(slot->paletteData);
    }
    if (slot->palette != NULL) {
        PaletteMgr_ReleaseResource(g_PaletteManagers[slot->palEngine], slot->palette);
    }
}

/// Starts the scripted translation `steps` on `shake`.
void CombatAnim_ShakeStart(Ov003Shake* shake, const Ov003ShakeStep* steps) {
    shake->steps   = steps;
    shake->frame   = 0;
    shake->index   = 0;
    shake->offsetX = steps->offsetX;
    shake->offsetY = steps->offsetY;
}

/// Advances the scripted translation by one frame, loading the next step's offset when it runs out.
void CombatAnim_ShakeUpdate(Ov003Shake* shake) {
    Ov003ShakeStep* step;

    if (shake->steps == NULL) {
        return;
    }
    step = &((Ov003ShakeStep*)shake->steps)[shake->index];
    if (step->duration == -1) {
        return;
    }
    shake->frame++;
    if (shake->frame < step->duration) {
        return;
    }
    shake->frame = 0;
    shake->index++;
    shake->offsetX = step[1].offsetX;
    shake->offsetY = step[1].offsetY;
}

/// Applies the scripted translation to `actor`'s position.
void CombatAnim_ShakeApply(Ov003Shake* shake, BattleActor* actor) {
    if (shake->steps == NULL) {
        return;
    }
    actor->position.x += shake->offsetX;
    actor->position.y += shake->offsetY;
}

void CombatAnim_ShakeStop(Ov003Shake* shake) {
    shake->steps = NULL;
}
