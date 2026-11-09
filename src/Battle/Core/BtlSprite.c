#include "Battle/Core/BtlSprite.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatSpriteDefault.h"
#include "Engine/Core/OamMgr.h"
#include "Engine/Core/System.h"
#include "SpriteMgr.h"
#include "common_data.h"

s32 BtlSprite_SetAnimFromTable(BattleSprite* cSprite, u16 animTableIndex, s32 playOnce) {
    if (animTableIndex == cSprite->animTableIndex) {
        return 1;
    }
    cSprite->animTableIndex = animTableIndex;
    if (playOnce & 1) {
        cSprite->sprite.animationMode = ANIM_MODE_ONCE;
    } else {
        cSprite->sprite.animationMode = ANIM_MODE_LOOPED;
    }

    Data*            data      = cSprite->sprite.resourceData;
    SpriteAnimEntry* animEntry = &cSprite->animTable[animTableIndex];
    void*            animData  = Data_GetPackEntryData(data, animEntry->animDataIndex);
    SpriteCell*      cellTable = Data_GetPackEntryData(data, animEntry->cellDataIndex);

    Sprite_ChangeAnimation(&cSprite->sprite, animData, (s16)(animEntry->animIndex + 1), cellTable);

    switch (cSprite->sprite.bits_3_4) {
        case 0:
        case 2:
        case 3:
        default:
            break;
        case 1:
            cSprite->sprite.unk34 = Data_GetPackEntryData(cSprite->sprite.resourceData, animEntry->charDataIndex);
            break;
    }
    return 0;
}

s32 BtlSprite_SetAnim(BattleSprite* cSprite, u16 animIndex, s32 playOnce) {
    if (animIndex == cSprite->animTableIndex) {
        return 1;
    }
    cSprite->animTableIndex = animIndex;
    if (playOnce & 1) {
        cSprite->sprite.animationMode = ANIM_MODE_ONCE;
    } else {
        cSprite->sprite.animationMode = ANIM_MODE_LOOPED;
    }
    Sprite_ChangeAnimation(&cSprite->sprite, cSprite->sprite.animData, animIndex + 1, cSprite->sprite.cellTable);
    return 0;
}

void BtlSprite_SetAffineTransform(BattleSprite* cSprite, u32 rotation, s32 scaleX, s32 scaleY, s32 doubleSize) {
    if (cSprite->flags & BTL_SPRITE_FLAG_FLIPPED) {
        scaleX = -scaleX;
    }

    s32 flipFlags = 0;
    if (scaleX < 0) {
        flipFlags |= 1;
        scaleX = -scaleX;
    }
    if (scaleY < 0) {
        flipFlags |= 2;
        scaleY = -scaleY;
    }

    s32 val     = cSprite->sprite.bits_0_1;
    s32 temp_r0 = OamMgr_AllocAffineGroup(&g_OamMgr[val], rotation, scaleX, scaleY, flipFlags);

    val = cSprite->sprite.unk_0A.raw & (~0x3E0);

    cSprite->sprite.unk_0A.raw = (u16)(val | (((u32)(((u16)temp_r0) << 0x1B)) >> 0x16));

    val = val;

    cSprite->sprite.unk_0A.raw = (u16)((cSprite->sprite.unk_0A.raw & (~1)) | 1);

    if (doubleSize != 0) {
        temp_r0 = 1;
    } else {
        temp_r0 = 0;
    }
    cSprite->sprite.unk_0A.raw = (cSprite->sprite.unk_0A.raw & (~2)) | (((u32)(((u16)temp_r0) << 0x1F)) >> 0x1E);
}

void BtlSprite_SetPaletteSource(BattleSprite* cSprite, s32 paletteEntry) {
    cSprite->sprite.unk3C = Data_GetPackEntryData(cSprite->sprite.resourceData, paletteEntry);
    PaletteMgr_SetSource(g_PaletteManagers[cSprite->sprite.bits_0_1], cSprite->sprite.paletteData, cSprite->sprite.unk3C);
}

void BtlSprite_SetPosition(BattleSprite* cSprite, s32 posX, s32 posY) {
    cSprite->sprite.posX = posX;
    cSprite->sprite.posY = posY;
}

void BtlSprite_SetSortKey(BattleSprite* cSprite, s32 sortKey) {
    cSprite->sortKey = sortKey;
}

void BtlSprite_SetPriority(BattleSprite* cSprite, s32 priority) {
    cSprite->sprite.unk_0A.raw = (u16)((cSprite->sprite.unk_0A.raw & ~0xC00) | ((u32)(priority << 0x1E) >> 0x14));
}

void BtlSprite_SetFlip(BattleSprite* cSprite, s32 flipped) {
    if (flipped != 0) {
        cSprite->sprite.unk_0A.unk_05 |= 8;
        cSprite->flags |= BTL_SPRITE_FLAG_FLIPPED;
    } else {
        cSprite->sprite.unk_0A.unk_05 &= ~8;
        cSprite->flags &= ~BTL_SPRITE_FLAG_FLIPPED;
    }
}

void BtlSprite_SetPaletteMode(BattleSprite* cSprite, BattleSpritePaletteMode mode) {
    cSprite->paletteMode = mode;

    switch (cSprite->paletteMode) {
        case BTL_SPRITE_PALETTE_FLASH: {
            cSprite->sprite.paletteData = g_CombatGlobal->unk3D348[cSprite->sprite.bits_0_1];
            cSprite->sprite.unk3C       = (UnkSmallInternal*)Data_GetPackEntryData(g_CombatGlobal->unk3D350, 65);
        } break;

        case BTL_SPRITE_PALETTE_OWN: {
            cSprite->sprite.paletteData = cSprite->basePalette;
            cSprite->sprite.unk3C       = cSprite->basePaletteSource;
        } break;

        case BTL_SPRITE_PALETTE_FLICKER:
            break;
    }
}

void BtlSprite_RenderTranslucent(BattleSprite* cSprite, s32 alpha) {
    cSprite->sprite.unk_0A.unk_02 = 1;

    s32 prev             = data_0206a890.unk_0C;
    data_0206a890.unk_0C = (alpha & 0x1F);

    Sprite_RenderFrame(&cSprite->sprite);
    cSprite->sprite.unk_0A.unk_02 = 0;

    data_0206a890.unk_0C = (prev & 0x1F);
}

SpriteFrameInfo* BtlSprite_GetFrameInfo(Sprite* sprite, s32 arg, s32 mode) {
    SpriteFrameInfo* info = &g_SpriteFrameInfo;

    switch (mode) {
        case SPRITE_FRAME_UPDATE: {
            info->updateSteps = 1;
            return info;
        } break;

        case SPRITE_FRAME_RENDER: {
            SpriteFrameInfo* temp = &g_SpriteFrameInfo;

            temp->pieceCount = 0;
            temp->cellPieces = NULL;
            temp->affine     = ((BattleSprite*)sprite->owner)->affine;
            temp->sortKey    = ((BattleSprite*)sprite->owner)->sortKey;

            if ((sprite->animData != NULL) && (sprite->cellTable != NULL && sprite->cellIndex >= 0)) {
                u16* temp_r3 = (u16*)sprite->cellTable;

                s16 temp_r2 = *((u16*)sprite->cellTable + (sprite->cellIndex * 4 + 1));

                temp->pieceCount = *((u16*)sprite->cellTable + (sprite->cellIndex * 4 + 1));
                temp->cellPieces =
                    (OamCellPiece*)((u16*)sprite->cellTable + *((u16*)((u8*)sprite->cellTable + (sprite->cellIndex * 8))));
            }
            info = temp;
            return info;
        } break;
    }
    return NULL;
}

void BtlSprite_InitAnim(SpriteAnimation* anim, s32 engine, const BinIdentifier* iden) {
    *anim          = CombatSprite_DefaultAnim;
    anim->bits_0_1 = engine;
    anim->binIden  = iden;
}

s32 BtlSprite_Load(BattleSprite* cSprite, SpriteAnimationEx* anim) {
    anim->anim.owner        = cSprite;
    cSprite->animTable      = (SpriteAnimEntry*)anim->animTable;
    cSprite->animTableIndex = -1;
    cSprite->sortKey        = -1;
    cSprite->flags          = 0;
    cSprite->affine         = NULL;
    cSprite->paletteMode    = BTL_SPRITE_PALETTE_OWN;
    cSprite->paletteTimer   = 0;
    if (Sprite_Load(&cSprite->sprite, &anim->anim) == 0) {
        cSprite->flags |= BTL_SPRITE_FLAG_LOAD_FAILED;
        return 0;
    }
    cSprite->basePaletteSource = cSprite->sprite.unk3C;
    cSprite->basePalette       = cSprite->sprite.paletteData;
    return 1;
}

s32 BtlSprite_LoadFromTable(s32 engine, BattleSprite* cSprite, const BinIdentifier* iden, const SpriteAnimEntry* table,
                            u16 entry, u16 paletteEntry, u16 charTiles) {
    SpriteAnimationEx      anim;
    const SpriteAnimEntry* animEntry;

    BtlSprite_InitAnim(&anim.anim, engine, iden);
    anim.anim.bits_10_11 = 1;
    animEntry            = &table[entry];
    anim.anim.unk_1C     = animEntry->charDataIndex;
    anim.anim.unk_1E     = (charTiles << 5);
    anim.anim.unk_26     = animEntry->cellDataIndex;
    anim.anim.unk_28     = animEntry->animDataIndex;
    anim.anim.unk_20     = paletteEntry;
    anim.anim.animIndex  = animEntry->animIndex + 1;
    anim.animTable       = table;
    return BtlSprite_Load(cSprite, &anim);
}

void BtlSprite_LoadDirect(s32 engine, BattleSprite* cSprite, const BinIdentifier* iden, s32 charEntry, u16 animEntry,
                          u16 cellEntry, u16 paletteEntry, u16 animIndex) {
    SpriteAnimationEx anim;

    BtlSprite_InitAnim(&anim.anim, engine, iden);
    anim.anim.bits_10_11 = 0;
    anim.anim.unk_1C     = charEntry;
    anim.anim.unk_26     = cellEntry;
    anim.anim.unk_28     = animEntry;
    anim.anim.unk_20     = paletteEntry;
    anim.anim.animIndex  = animIndex + 1;
    anim.animTable       = NULL;
    BtlSprite_Load(cSprite, &anim);
}

void BtlSprite_Init(BattleSprite* cSprite) {
    Sprite_Init(&cSprite->sprite);
}

void BtlSprite_Update(BattleSprite* cSprite) {
    if (cSprite->flags & BTL_SPRITE_FLAG_PALETTE_FLASH) {
        cSprite->paletteTimer--;
        if (cSprite->paletteTimer <= 0) {
            cSprite->sprite.unk3C       = cSprite->basePaletteSource;
            cSprite->sprite.paletteData = cSprite->basePalette;
            cSprite->paletteMode        = BTL_SPRITE_PALETTE_OWN;
            cSprite->paletteTimer       = 0;
            cSprite->flags &= ~BTL_SPRITE_FLAG_PALETTE_FLASH;
        }
    }
    Sprite_Update(&cSprite->sprite);
}

void BtlSprite_Render(BattleSprite* cSprite) {
    if (cSprite->flags & BTL_SPRITE_FLAG_TRANSLUCENT) {
        cSprite->sprite.unk_0A.unk_02 = 1;
    } else if (cSprite->flags & BTL_SPRITE_FLAG_OBJ_WINDOW) {
        cSprite->sprite.unk_0A.unk_02 = 2;
    } else {
        cSprite->sprite.unk_0A.unk_02 = 0;
    }

    if (cSprite->paletteMode == BTL_SPRITE_PALETTE_FLICKER) {
        if (SysControl.frameCount & 1) {
            cSprite->sprite.paletteData = g_CombatGlobal->unk3D348[cSprite->sprite.bits_0_1];
            cSprite->sprite.unk3C       = (UnkSmallInternal*)Data_GetPackEntryData(g_CombatGlobal->unk3D350, 65);
        } else {
            cSprite->sprite.paletteData = cSprite->basePalette;
            cSprite->sprite.unk3C       = cSprite->basePaletteSource;
        }
    }

    if (cSprite->sprite.unk_0A.unk_00 == 1) {
        if (cSprite->flags & BTL_SPRITE_FLAG_FLIPPED) {
            cSprite->sprite.unk_0A.raw =
                (cSprite->sprite.unk_0A.raw & ~0x3E0) |
                ((u32)((u16)(((u32)(cSprite->sprite.unk_0A.raw << 0x16) >> 0x1B) & ~8) << 0x1B) >> 0x16);
        }
        Sprite_RenderFrame(&cSprite->sprite);
        if (cSprite->flags & BTL_SPRITE_FLAG_FLIPPED) {
            cSprite->sprite.unk_0A.raw =
                (cSprite->sprite.unk_0A.raw & ~0x3E0) |
                ((u32)((u16)(((u32)(cSprite->sprite.unk_0A.raw << 0x16) >> 0x1B) | 8) << 0x1B) >> 0x16);
        }
    } else {
        Sprite_RenderFrame(&cSprite->sprite);
    }
}

void BtlSprite_Release(BattleSprite* cSprite) {
    if (cSprite->paletteMode != BTL_SPRITE_PALETTE_OWN) {
        cSprite->sprite.unk3C       = cSprite->basePaletteSource;
        cSprite->sprite.paletteData = cSprite->basePalette;
        cSprite->paletteMode        = BTL_SPRITE_PALETTE_OWN;
        cSprite->paletteTimer       = 0;
        cSprite->flags &= ~BTL_SPRITE_FLAG_PALETTE_FLASH;
    }
    Sprite_Release(&cSprite->sprite);
}

void BtlSprite_Restart(BattleSprite* cSprite) {
    Sprite_Restart(&cSprite->sprite);
}
