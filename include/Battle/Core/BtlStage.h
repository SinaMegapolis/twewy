#ifndef TWEWY_BATTLE_CORE_BTLSTAGE_H
#define TWEWY_BATTLE_CORE_BTLSTAGE_H

#include "nitro/types.h"

/// The camera of one screen engine and the field rectangle it looks at; `BtlEnm015_Shake` jitters
/// the offset at 0x44..0x4C. Positions are 20.12 fixed point.
typedef struct BattleCamera {
    /* 0x00 */ union {
        s16 unk_00;
        u16 pathId; // field background of the battle, an index into BtlField_UG_Defs/BtlField_RG_Defs
    };
    /* 0x02 */ u16 unk_02;
    /* 0x04 */ s32 unk_04;   // left edge of the field
    /* 0x08 */ s32 unk_08;   // top edge of the field
    /* 0x0C */ s32 unk_0C;   // field width
    /* 0x10 */ s32 unk_10;   // field height
    /* 0x14 */ s32 posY;
    /* 0x18 */ s32 posX;     // main camera: top of the floor at the field's left edge
    /* 0x1C */ u16 rotation; // main camera: slope of the floor's top edge, 0x10000 = a full turn
    /* 0x1E */ s16 unk_1E;
    /* 0x20 */ s32 unk_20;   // half the field width
    /* 0x24 */ s32 unk_24;   // half the field height
    /* 0x28 */ s32 unk_28;   // screen-space centre X used by the battle projection
    /* 0x2C */ s32 unk_2C;
    /* 0x30 */ s32 unk_30;
    /* 0x34 */ s32 unk_34; // point the camera moves towards, see BtlStage_SetCameraTarget
    /* 0x38 */ s32 unk_38;
    /* 0x3C */ s32 unk_3C;
    /* 0x40 */ s32 unk_40; // screen-space centre Y used by the battle projection
    /* 0x44 */ s32 unk_44;
    /* 0x48 */ s32 unk_48;
    /* 0x4C */ s32 unk_4C;
    /* 0x50 */ s32 shakeY;
    /* 0x54 */ s32 shakeX;
} BattleCamera; // Size: 0x58

/**
 * @brief Places the main engine's camera over the field rectangle of its background
 *
 * Reads the rectangle of `pathId` from BtlField_UG_Defs and also sets up the sloped top edge of the
 * floor that BtlStage_IsOutOfBounds and the clamps keep actors below.
 */
void BtlStage_PlaceMainCamera(void);

/**
 * @brief Places the sub engine's camera over the field rectangle of its background
 *
 * Reads the rectangle of `pathId` from BtlField_RG_Defs.
 */
void BtlStage_PlaceSubCamera(void);

/**
 * @brief Places a camera over a field rectangle
 *
 * Also clears the camera's shake offset.
 *
 * @param index The camera: 0 = main engine, 1 = sub engine
 * @param x0 Left edge of the field, in pixels
 * @param y0 Top edge of the field, in pixels
 * @param x1 Right edge of the field, in pixels
 * @param y1 Bottom edge of the field, in pixels
 */
void BtlStage_SetCamera(s32 index, s32 x0, s32 y0, s32 x1, s32 y1);

/**
 * @brief Sets the point a camera moves towards
 *
 * @param index The camera: 0 = main engine, 1 = sub engine
 * @param x Horizontal position relative to the field's left edge, 20.12 fixed point
 * @param y Vertical position relative to the field's top edge, 20.12 fixed point
 * @param z Height, 20.12 fixed point
 */
void BtlStage_SetCameraTarget(s32 index, s32 x, s32 y, s32 z);

/**
 * @brief Checks whether a box sticks out of the main camera's field
 *
 * The top edge is the sloped top of the floor at `x`.
 *
 * @param x Horizontal centre of the box in the field, 20.12 fixed point
 * @param y Vertical centre of the box in the field, 20.12 fixed point
 * @param marginX Half the box width, 20.12 fixed point
 * @param marginY Half the box height, 20.12 fixed point
 * @return TRUE if any edge of the box is outside the field, FALSE if it is inside
 */
BOOL BtlStage_IsOutOfBounds(s32 x, s32 y, s32 marginX, s32 marginY);

/**
 * @brief Pushes a vertical position down below the top of the main camera's floor
 *
 * @param value The vertical position to clamp, 20.12 fixed point; updated in place
 * @param margin Distance to keep from the edge, 20.12 fixed point
 * @param x Horizontal position the sloped top edge is measured at, 20.12 fixed point
 * @return TRUE if `value` was moved, FALSE if it was already inside
 */
BOOL BtlStage_ClampToFloorTop(s32* value, s32 margin, s32 x);

/**
 * @brief Clamps a vertical position between the top of the main camera's floor and the field's bottom edge
 *
 * @param value The vertical position to clamp, 20.12 fixed point; updated in place
 * @param margin Distance to keep from the edges, 20.12 fixed point
 * @param x Horizontal position the sloped top edge is measured at, 20.12 fixed point
 * @return TRUE if `value` was moved, FALSE if it was already inside
 */
BOOL BtlStage_ClampY(s32* value, s32 margin, s32 x);

/**
 * @brief Clamps a horizontal position between the left and right edges of the main camera's field
 *
 * @param value The horizontal position to clamp, 20.12 fixed point; updated in place
 * @param margin Distance to keep from the edges, 20.12 fixed point
 * @return TRUE if `value` was moved, FALSE if it was already inside
 */
BOOL BtlStage_ClampX(s32* value, s32 margin);

/**
 * @brief Clamps a position into the main camera's floor
 *
 * Clamps `x` first, then `y` against the floor's top edge at the clamped `x`.
 *
 * @param x The horizontal position to clamp, 20.12 fixed point; updated in place
 * @param y The vertical position to clamp, 20.12 fixed point; updated in place
 * @param marginX Distance to keep from the left and right edges, 20.12 fixed point
 * @param marginY Distance to keep from the top and bottom edges, 20.12 fixed point
 * @return TRUE if either coordinate was moved
 */
BOOL BtlStage_ClampXY(s32* x, s32* y, s32 marginX, s32 marginY);

/**
 * @brief Clamps a vertical position between the top and bottom edges of the main camera's field
 *
 * Unlike BtlStage_ClampY, the top edge is the field's own, not the sloped top of the floor.
 *
 * @param value The vertical position to clamp, 20.12 fixed point; updated in place
 * @param margin Distance to keep from the edges, 20.12 fixed point
 * @return TRUE if `value` was moved, FALSE if it was already inside
 */
BOOL BtlStage_ClampYToBox(s32* value, s32 margin);

/**
 * @brief Picks the field background of the battle and places both cameras over it
 *
 * The background comes from the save, except for the encounters that bring their own.
 */
void BtlStage_SetupCameras(void);

/**
 * @brief Starts the two background tasks (main and sub screen) of the current battle
 *
 * Boss encounters start their own background tasks from other overlays; every other battle starts
 * the field tasks.
 *
 * @param flag Passed to the tasks; 0 also refreshes the area's leading brand in the save first
 */
void BtlStage_StartFieldTasks(s32 flag);

/**
 * @brief Starts the sandstorm transition task, except in the last four battles
 */
void BtlStage_StartSandstorm(void);

/**
 * @brief Projects a field position onto a screen
 *
 * @param index The camera: 0 = main engine, 1 = sub engine
 * @param screenX Receives the horizontal screen position, in pixels
 * @param screenY Receives the vertical screen position, in pixels
 * @param x Horizontal field position, 20.12 fixed point
 * @param y Vertical field position, 20.12 fixed point
 * @param z Height offset added to `y`, 20.12 fixed point
 */
void BtlStage_WorldToScreen(s32 index, s16* screenX, s16* screenY, s32 x, s32 y, s32 z);

/**
 * @brief Projects a horizontal field position onto a screen
 *
 * @param index The camera: 0 = main engine, 1 = sub engine
 * @param x Horizontal field position, 20.12 fixed point
 * @return The horizontal screen position, in pixels
 */
s16 BtlStage_ToScreenX(s32 index, s32 x);

/**
 * @brief Projects a vertical field position onto a screen
 *
 * @param index The camera: 0 = main engine, 1 = sub engine
 * @param y Vertical field position, 20.12 fixed point
 * @param z Height offset added to `y`, 20.12 fixed point
 * @return The vertical screen position, in pixels
 */
s16 BtlStage_ToScreenY(s32 index, s32 y, s32 z);

/**
 * @brief Converts a horizontal screen position into a field position
 *
 * @param index The camera: 0 = main engine, 1 = sub engine
 * @param screenX Horizontal screen position, in pixels
 * @return The horizontal field position, 20.12 fixed point
 */
s32 BtlStage_ToWorldX(s32 index, s32 screenX);

/**
 * @brief Converts a vertical screen position into a field position
 *
 * @param index The camera: 0 = main engine, 1 = sub engine
 * @param screenY Vertical screen position, in pixels
 * @param height Height offset subtracted from the result, 20.12 fixed point
 * @return The vertical field position, 20.12 fixed point
 */
s32 BtlStage_ToWorldY(s32 index, s32 screenY, s32 height);

#endif // TWEWY_BATTLE_CORE_BTLSTAGE_H
