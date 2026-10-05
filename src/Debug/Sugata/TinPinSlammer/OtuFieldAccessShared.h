#ifndef OTU_FIELDACCESS_SHARED_H
#define OTU_FIELDACCESS_SHARED_H

/*
 * Declarations the ov039 (Tin Pin Slammer) source files share: the types more
 * than one file uses, the overlay data they read, and the functions called
 * across files. A declaration only one file needs lives in that file.
 */

#include "CriSndMgr.h"
#include "Debug/Sugata/TinPinSlammer.h"
#include "EasyFade.h"
#include "Engine/Core/OamMgr.h"
#include "Engine/Core/System.h"
#include "Engine/EasyTask.h"
#include "Engine/File/BinMgr.h"
#include "Engine/IO/TouchInput.h"
#include "Engine/Math/Random.h"
#include "Save.h"
#include "SndMgr.h"
#include "SpriteMgr.h"
#include "common_data.h"
#include <nitro/fx/fx_atan.h>
#include <nitro/fx/fx_division.h>
#include <nitro/mi/cpumem.h>
#include <nitro/wm.h>

/* Types */

/* The 0x10-byte animated-palette object at the background task's +0x840. */
typedef struct {
    /* 0x00 */ const u16* table; // compared against the table pointer to see if it is set
    /* 0x04 */ s32        count;
    /* 0x08 */ s16        timer;
    /* 0x0A */ u8         pad_0A[2];
    /* 0x0C */ u16*       base;
} OtuPaletteAnim; // Size: 0x10

/** Tsk_OtosuGame_ovbg's state: the board's four-layer background. */
typedef struct {
    /* 0x000 */ Heap*                  heap;
    /* 0x004 */ struct OtuBoardLayout* layout;
    /* 0x008 */ Data*                  fileData;
    /* 0x00C */ PaletteResource*       palettes[4];
    /* 0x01C */ BgResource*            chars[4];
    /* 0x02C */ BgResource*            screens[4];
    /* 0x03C */ u32                    screenHeader; // the layer-2 screen data, built in place
    /* 0x040 */ u16                    screen[32][32];
    /* 0x840 */ OtuPaletteAnim         paletteAnim;
} OtuOvbg;

/** Tsk_OtosuGame_badgeradar's state. */
typedef struct {
    /* 0x00 */ s32                    dataType;
    /* 0x04 */ Sprite                 sprite;
    /* 0x44 */ s32                    x;     // Q12.12, converted to pixels by the render stage
    /* 0x48 */ s32                    y;     // Q12.12
    /* 0x4C */ u32                    pinId; // the badge being tracked
    /* 0x50 */ s32                    index; // the badge's player index, added to the animation
    /* 0x54 */ struct OtuBoardLayout* board;
    /* 0x58 */ s32                    linked;
    /* 0x5C */ s32                    isFirst; // 1 selects animation 5, anything else animation 1
} OtuBadgeRadar;                               // Size: 0x60

/** Tsk_OtosuGame_badgecount's state. */
typedef struct {
    /* 0x000 */ s32    dataType;
    /* 0x004 */ Sprite spriteA;   // only present when hasSpriteA is set
    /* 0x044 */ Sprite spriteB;
    /* 0x084 */ Sprite digits[4]; // gated by the bitmask at +0x194
    /* 0x184 */ u32    pinId;     // the badge whose count is shown
    /* 0x188 */ s32    index;     // the badge's player index: 7 animations and a y offset each
    /* 0x18C */ s32    resolved;  // sourceId resolved in the pool
    /* 0x190 */ s32    hasSpriteA;
    /* 0x194 */ u16    visible;   // bit N is digits[N]
    /* 0x196 */ u8     pad_196[2];
} OtuBadgeCount;                  // Size: 0x198

/** Tsk_OtosuGame_timer's state: the round's time limit, three digit sprites. */
typedef struct {
    /* 0x000 */ s32    dataType;
    /* 0x004 */ Sprite digits[3];
    /* 0x0C4 */ s32    countdown; // 1/60th ticks remaining
    /* 0x0C8 */ s32    visible;
    /* 0x0CC */ s32    alarmed;   // one-frame pulse at the 0x4B0 crossing
} OtuTimer;                       // Size: 0xD0

typedef struct {
    /* 0x00 */ u8 fromX; // a badge stopping on this cell...
    /* 0x01 */ u8 fromY;
    /* 0x02 */ u8 toX;   // ...is moved to this one
    /* 0x03 */ u8 toY;
} OtuBoardWarp;          // Size: 0x4

/**
 * @brief The board's layout, loaded from the stage's bin into the stage block
 * at +0x150 and handed by pointer to the badges, the floor, the backgrounds and
 * the obstacles.
 *
 * `cells` is width x height two-byte cells; the first byte of a cell is its
 * tile type, which func_ov039_0208a794 maps to the badge's movement rules.
 */
typedef struct OtuBoardLayout {
    /* 0x00 */ u8            kind;    // picks the floor art; 1 also clears the OBJ layers each round
    /* 0x01 */ u8            variant; // the floor style; the obstacle factory's palette slot
    /* 0x02 */ u8            width;   // in cells
    /* 0x03 */ u8            height;
    /* 0x04 */ u8            obstacleCount;
    /* 0x05 */ u8            warpCount;
    /* 0x06 */ u8            pad_06[2];
    /* 0x08 */ u8            start[4][2]; // each badge's starting cell
    /* 0x10 */ u8*           cells;       // 0x1388 bytes from the stage heap
    /* 0x14 */ u8*           obstacles;   // 8-byte obstacle records
    /* 0x18 */ OtuBoardWarp* warps;
} OtuBoardLayout;                         // Size: 0x1C

/**
 * @brief One player's touch-pad descriptor, six bytes.
 *
 * `data_ov039_0209ad20` is one of these and `data_ov039_0209af20` is an array of
 * them; `func_ov039_02088440` is `data_ov039_0209af20 + index * 6`. The
 * wireless stages also exchange the phase byte, 1 to 4, to keep the players'
 * rounds in step.
 */
typedef struct {
    /* 0x00 */ u8  touched;
    /* 0x01 */ u8  phase;
    /* 0x02 */ u8  x;
    /* 0x03 */ u8  y;
    /* 0x04 */ u16 sysControl;
} OtuPadState; // Size: 0x6

/** A point with a reach: an attack's hit, or a sprite's position and scale. */
typedef struct {
    /* 0x0 */ s32 x;
    /* 0x4 */ s32 y;
    /* 0x8 */ s32 scale;
} OtuPinRecord; // Size: 0xC

/** Tsk_OtosuGame_badge's state: one badge on the board. Positions are Q12.12. */
typedef struct OtuBadge {
    /* 0x000 */ TinPinSlammer_Scene* scene;
    /* 0x004 */ s32                  dataType; // copied into SpriteAnimation.dataType
    /* 0x008 */ TaskPool*            pool;     // saved by the init stage
    /* 0x00C */ Sprite               spriteA;
    /* 0x04C */ Sprite               spriteB;
    /* 0x08C */ Sprite               spriteC;
    /* 0x0CC */ OamAffineParam       affine;      // rotation follows `travel`
    /* 0x0DC */ s32                  visible;     // shown by the shadow; func_ov039_0208e890
    /* 0x0E0 */ s32                  index;       // which badge this is; passed to the pool resolvers
    /* 0x0E4 */ OtuBoardLayout*      board;
    /* 0x0E8 */ OtuPadState*         pad;         // the controlling player's touch pad; NULL for the CPU
    /* 0x0EC */ u16                  lastKeys;    // pad->sysControl last frame
    /* 0x0EE */ u16                  pressedKeys; // keys newly pressed this frame
    /* 0x0F0 */ u16                  touchFlags;  // bit 0 touching, 1 touch began, 2 touch ended
    /* 0x0F4 */ s32                  step;        // sub-phase within `kind`
    /* 0x0F8 */ s32              phase; // 1..9; 6, 7 and 8 show the needle, hammer and meteo; the special gauge also watches 9
    /* 0x0FC */ s32              subKind; // gates kind 8 in the render stage
    /* 0x100 */ s32              frameBudget;
    /* 0x104 */ s32              lastTileType;
    /* 0x108 */ s32              lastCellX;
    /* 0x10C */ s32              lastCellY;
    /* 0x110 */ OtuPoint         origin;      // the render subtracts these
    /* 0x118 */ OtuPoint         homeOffset;  // func_ov039_0208e87c's pair
    /* 0x120 */ OtuPoint         pos;         // Q12.12
    /* 0x128 */ s32              height;      // <= 0; added into the drawn y
    /* 0x12C */ OtuPoint         vel;
    /* 0x134 */ s32              vz;          // gravity is data_ov039_0209a318
    /* 0x138 */ OtuPoint         dir;         // vel re-normalised in here when vel is non-zero
    /* 0x140 */ s32              travel;      // integrated from vel each update
    /* 0x144 */ s32              velMag;      // decays by a fixed step each update
    /* 0x148 */ s32              stun;        // frames left stunned; the AI waits it out
    /* 0x14C */ OtuPoint         aimStart;    // where the current aim began
    /* 0x154 */ OtuPoint         aimCur;      // where it is aiming now
    /* 0x15C */ OtuPoint         startPos;    // the badge's starting cell, centred
    /* 0x164 */ s32              unk_164;
    /* 0x168 */ s32              flags;       // init 0xA2; bit 1 blocks the render
    /* 0x16C */ u16*             pinID;       // a tray slot; 0x130 means "no pin"
    /* 0x170 */ OtuBadgeParam*   slots;       // per-pin record, 0x1C stride, indexed by *pinID
    /* 0x174 */ u16*             chanceTbl;   // one 0x10000 chance per AI
    /* 0x178 */ s16              trackFrames; // the phase 6..9 budgets, seeded from slots->tile[]
    /* 0x17A */ s16              bounceTimer;
    /* 0x17C */ s16              arcFrames;
    /* 0x17E */ s16              spinFrames;
    /* 0x180 */ s32              hitCount;    // how many of `hits` the current attack fills
    /* 0x184 */ OtuPinRecord     hits[2];     // the attack's hit points, with their reach
    /* 0x19C */ s16              trailIndex;  // the next of trackIds to place
    /* 0x19E */ s16              trailTimer;  // frames between track marks
    /* 0x1A0 */ s16              pointCursor; // which of pointIds shows the next score
    /* 0x1A4 */ s32              unk_1A4;     // raised to 1 by the init stage
    /* 0x1A8 */ s32              hasLabel;    // gates two of the children
    /* 0x1AC */ s32              score;       // clamped to 999
    /* 0x1B0 */ struct OtuBadge* partner;     // the pin this one last touched
    /* 0x1B4 */ s32              curAI;       // 0x11 = parked
    /* 0x1B8 */ s32              unk_1B8;     // counts down, then re-raises curAI
    /* 0x1BC */ struct OtuBadge* chaseTarget;
    /* 0x1C0 */ OtuPoint         anchorPt;    // where the current AI sent us
    /* 0x1C8 */ s32              smokeCursor; // the next of smokeIds to puff
    /* 0x1CC */ s32              aimFrames;   // 0x1E = commit
    /* 0x1D0 */ s32              mode;        // 0x10 once the badge is committed
    /* 0x1D4 */ s32              shadowId;    // the badge's child tasks, created by func_ov039_0208dcb0
    /* 0x1D8 */ s32              piyoId;
    /* 0x1DC */ s32              markerId;
    /* 0x1E0 */ s32              meteoId;
    /* 0x1E4 */ u32              hammerId;
    /* 0x1E8 */ u32              needleId;
    /* 0x1EC */ u32              handId;
    /* 0x1F0 */ s32              radarId;
    /* 0x1F4 */ s32              counterId;
    /* 0x1F8 */ u32              trackIds[12];
    /* 0x228 */ s32              pointIds[2];
    /* 0x230 */ s32              entryId;
    /* 0x234 */ s32              deadId;
    /* 0x238 */ s32              smokeIds[8];
    /* 0x258 */ u32              warpId;
} OtuBadge;

/* The handle allocates 0x25C bytes. */
typedef char OtuBadge_SizeMustBe_0x25C[(sizeof(OtuBadge) == 0x25C) ? 1 : -1];

/**
 * @brief One of the four slots in "Tsk_OtosuGame_point", 0x14 bytes.
 *
 * A 0x40-stride array of these at +0x120, one per sprite. `live` gates the
 * whole slot; `delay` counts the sprite's own animation down; `offsetX` and
 * `accum` are the two components of the position the render adds to the
 * anchor difference; `phase` is the step the update integrates into `accum`.
 */
typedef struct {
    /* 0x00 */ s32 live;
    /* 0x04 */ s32 delay;
    /* 0x08 */ s32 offsetX;
    /* 0x0C */ s32 accum;
    /* 0x10 */ s32 phase;
} OtuPointSlot; // Size: 0x14

/** "Tsk_OtosuGame_track": one mark of a badge's movement trail. */
typedef struct {
    /* 0x00 */ TaskPool* pool;
    /* 0x04 */ s32       pinId;
    /* 0x08 */ Sprite    sprite;
    /* 0x48 */ OtuPoint  origin;
    /* 0x50 */ OtuPoint  pos;
    /* 0x58 */ OtuPoint  vel;
    /* 0x60 */ s32       active;
    /* 0x64 */ s32       visible;
    /* 0x68 */ u8        pad_68[0x8];
} OtuTrackTask; // Size: 0x70

/** "Tsk_OtosuGame_point": a score popped up over a pin, up to four digits bouncing. */
typedef struct {
    /* 0x000 */ Sprite       sprite[4];
    /* 0x100 */ OtuPoint     origin;
    /* 0x108 */ OtuPoint     pos;
    /* 0x110 */ s32          pinId;
    /* 0x114 */ s32          active;
    /* 0x118 */ s32          visible;
    /* 0x11C */ s32          count; // the value shown
    /* 0x120 */ OtuPointSlot slot[4];
    /* 0x170 */ s32          timer;
} OtuPointTask; // Size: 0x174

/** "Tsk_OtosuGame_entry": the round-start banner, with an optional label sprite. */
typedef struct {
    /* 0x00 */ Sprite         sprite;
    /* 0x40 */ Sprite         labelSprite;
    /* 0x80 */ s32            visible;
    /* 0x84 */ s32            state;    // 1 while showing, back to 0 when the timer runs out
    /* 0x88 */ s32            timer;    // 0x78 frames
    /* 0x8C */ s32            hasLabel; // draws and animates labelSprite too
    /* 0x90 */ OamAffineParam affine0;
    /* 0xA0 */ OamAffineParam affine1;
    /* 0xB0 */ OtuScaleAnim   scaleAnim0;
    /* 0xBC */ OtuScaleAnim   scaleAnim1;
} OtuEntryTask; // Size: 0xC8

/** The creation block of a task that only needs the sprites' dataType. */
typedef struct {
    s32 dataType;
} OtuTaskArgs1;

/**
 * @brief The two-word argument block the pin tasks are created with.
 *
 * `dataType` is read back by the four `_Sprite_Load` wrappers (it lands in the
 * animation template's dataType nibble); `childId` is read by the four init steps,
 * which store it as the id of the pin the new task will track.
 */
typedef struct {
    s32 dataType;
    s32 childId;
} OtuPinSpriteArgs; // Size: 0x8

/** "Tsk_OtosuGame_dead": a sprite played over a pin when it is knocked out. */
typedef struct {
    /* 0x00 */ Sprite   sprite;
    /* 0x40 */ OtuPoint origin;
    /* 0x48 */ OtuPoint pos;
    /* 0x50 */ s32      pinId;
    /* 0x54 */ s32      visible;
    /* 0x58 */ s32      state; // 1 from func_ov039_0209657c until the animation ends
} OtuDead;                     // Size: 0x5C

/** "Tsk_OtosuGame_gameover": the round-end banner, one of three sprites. */
typedef struct {
    /* 0x00 */ Sprite         sprite0;
    /* 0x40 */ Sprite         sprite1; // animations 1 and 2
    /* 0x80 */ Sprite         sprite2;
    /* 0xC0 */ s32            visible;
    /* 0xC4 */ s32            which;   // which sprite: 0, 1 or 2 (sprite1), 3 (sprite2)
    /* 0xC8 */ s32            state;   // 0..3
    /* 0xCC */ s32            counter; // 0x3C frames per state
    /* 0xD0 */ OamAffineParam affine;
    /* 0xE0 */ OtuScaleAnim   scaleAnim;
} OtuGameover; // Size: 0xEC

/**
 * "Tsk_OtosuGame_meteohahen" and "Tsk_OtosuGame_needlehahen": a fragment
 * thrown off a pin. It flies along `dir` while `speed` bleeds away, bounces on
 * `height` under gravity, and shrinks as `life` runs out.
 */
typedef struct {
    /* 0x00 */ Sprite         sprite;
    /* 0x40 */ OamAffineParam affine;
    /* 0x50 */ OtuPoint       origin;
    /* 0x58 */ OtuPoint       pos;
    /* 0x60 */ s32            height; // <= 0, added into the drawn y
    /* 0x64 */ OtuPoint       dir;
    /* 0x6C */ s32            speed;
    /* 0x70 */ s32            vz;
    /* 0x74 */ s32            pinId;
    /* 0x78 */ s32            active;
    /* 0x7C */ s32            visible;
    /* 0x80 */ s32            lifeMax;
    /* 0x84 */ s32            life;
} OtuHahen; // Size: 0x88

/** "Tsk_OtosuGame_hammerhahen": OtuHahen with a spin rate. */
typedef struct {
    /* 0x00 */ Sprite         sprite;
    /* 0x40 */ OamAffineParam affine;
    /* 0x50 */ OtuPoint       origin;
    /* 0x58 */ OtuPoint       pos;
    /* 0x60 */ s32            height;
    /* 0x64 */ OtuPoint       dir;
    /* 0x6C */ s32            speed;
    /* 0x70 */ s32            vz;
    /* 0x74 */ u16            spin; // added to the rotation every frame
    /* 0x76 */ u8             pad_76[2];
    /* 0x78 */ s32            pinId;
    /* 0x7C */ s32            active;
    /* 0x80 */ s32            visible;
    /* 0x84 */ s32            lifeMax;
    /* 0x88 */ s32            life;
} OtuHammerHahen; // Size: 0x8C

/** "Tsk_OtosuGame_smoke": a puff that drifts out from a point and fades. */
typedef struct {
    /* 0x00 */ Sprite   sprite;
    /* 0x40 */ OtuPoint origin;
    /* 0x48 */ OtuPoint pos;
    /* 0x50 */ s32      height;
    /* 0x54 */ OtuPoint dir;
    /* 0x5C */ s32      speed; // falls by `decel` each frame
    /* 0x60 */ s32      decel;
    /* 0x64 */ s32      unk_64;
    /* 0x68 */ s32      pinId;
    /* 0x6C */ s32      visible;
    /* 0x70 */ s32      state; // 1 intro, 2 hold for `hold` frames, 3 outro
    /* 0x74 */ s32      hold;
} OtuSmoke;                    // Size: 0x78

/** "Tsk_OtosuGame_warp": a one-shot sprite at a point. */
typedef struct {
    /* 0x00 */ Sprite   sprite;
    /* 0x40 */ OtuPoint origin;
    /* 0x48 */ OtuPoint pos;
    /* 0x50 */ s32      pinId;
    /* 0x54 */ s32      active;
    /* 0x58 */ s32      visible;
} OtuWarp; // Size: 0x5C

/** "Tsk_OtosuGame_spark": a spark thrown off a collision, an OtuHahen without a pin. */
typedef struct {
    /* 0x00 */ Sprite         sprite;
    /* 0x40 */ OamAffineParam affine;
    /* 0x50 */ OtuPoint       origin;
    /* 0x58 */ OtuPoint       pos;
    /* 0x60 */ s32            height;
    /* 0x64 */ OtuPoint       dir;
    /* 0x6C */ s32            speed;
    /* 0x70 */ s32            vz;
    /* 0x74 */ s32            active;
    /* 0x78 */ s32            visible;
    /* 0x7C */ s32            lifeMax;
    /* 0x80 */ s32            life;
} OtuSpark; // Size: 0x84

/** The caller's per-obstacle numbers, reached through OtuObstacle_Args.params. */
typedef struct {
    /* 0x00 */ u16 targetX; // <<12 into OtuObstacle.targetX
    /* 0x02 */ u16 targetY; // <<12 into OtuObstacle.targetY
    /* 0x04 */ u16 kind;    // 0..2; picks a row of each sprite table below
} OtuObstacle_Params;       // Size: 0x6

typedef struct {
    /* 0x00 */ Sprite   sprite;      // Size: 0x40. Passed straight to the sprite API.
    /* 0x40 */ OtuPoint origin;
    /* 0x48 */ OtuPoint pos;         // Q12.12
    /* 0x50 */ s32      scale;       // 16.16 scale factor, picked by kind.
    /* 0x54 */ s32      animPending; // One-shot: set at spawn, cleared by the first Update.
} OtuObstacle;                       // Size: 0x58

/**
 * A keyframe of the hammer's animations: three halfwords on a six-byte stride
 * (the target's `smulbb` against an immediate 6).
 */
typedef struct {
    /* 0x00 */ s16 duration; // frames to hold this entry
    /* 0x02 */ s16 value;
    /* 0x04 */ s16 scale;    // <<12 on use
} OtuFrame6;                 // Size: 0x6

/** A cursor over OtuFrame6 keys; the 6-byte twin of OtuCursor. */
typedef struct {
    /* 0x00 */ OtuFrame6* table;
    /* 0x04 */ s16        index;
    /* 0x06 */ s16        count;
    /* 0x08 */ s16        framesLeft;
} OtuCursor6; // Size: 0xA

/**
 * "Tsk_OtosuGame_hammer": a pin's hammer attack. A two-part sprite (the head A
 * and the shaft B) swung around the pin through seven states, throwing four
 * hammerhahen fragments as it lands.
 */
typedef struct {
    /* 0x000 */ TaskPool*      pool;
    /* 0x004 */ Sprite         spriteA;
    /* 0x044 */ Sprite         spriteB;
    /* 0x084 */ OamAffineParam affineA; // scaleX > 0 reveals sprite A
    /* 0x094 */ OamAffineParam affineB; // scaleY is the shaft's current length
    /* 0x0A4 */ OtuPoint       origin;
    /* 0x0AC */ OtuPoint       posA;
    /* 0x0B4 */ OtuPoint       posB;
    /* 0x0BC */ OtuPoint       pinPos; // the pin the hammer swings around
    /* 0x0C4 */ s32            height; // the pin's height, added into both sprites' y
    /* 0x0C8 */ s32            pinId;
    /* 0x0CC */ s32            live;   // the pin is in phase 7
    /* 0x0D0 */ s32            showA;
    /* 0x0D4 */ s32            showB;
    /* 0x0D8 */ s32            scale;
    /* 0x0DC */ s32            angle0;
    /* 0x0E0 */ s32            angle;
    /* 0x0E4 */ s32            halfLen;
    /* 0x0E8 */ s32            state; // 0..6; entered at 1 by func_ov039_02091028
    /* 0x0EC */ s32            framesLeft;
    /* 0x0F0 */ s32            rate0;
    /* 0x0F4 */ s32            scale0;
    /* 0x0F8 */ s32            totalFrames;
    /* 0x0FC */ s32            angleBase;
    /* 0x100 */ OtuCursor      cursorScale;
    /* 0x10C */ OtuCursor6     cursorSpin;
    /* 0x118 */ OtuCursor6     cursorTrail;
    /* 0x124 */ s32            reportTimer;
    /* 0x128 */ s32            children[4]; // hammerhahen
} OtuHammer;                                // Size: 0x138

/** "Tsk_OtosuGame_needle": a pin's needle attack, charged, held, then thrown. */
typedef struct {
    /* 0x00 */ TaskPool*      pool;
    /* 0x04 */ Sprite         sprite;
    /* 0x44 */ OamAffineParam affine;
    /* 0x54 */ OtuPoint       origin;
    /* 0x5C */ OtuPoint       pos;
    /* 0x64 */ s32            height;
    /* 0x68 */ s32            pinId;
    /* 0x6C */ s32            visible; // the pin is in phase 6
    /* 0x70 */ s32            chargeFrames;
    /* 0x74 */ s32            holdFrames;
    /* 0x78 */ s32            timer;
    /* 0x7C */ s32            holdTimer;
    /* 0x80 */ s32            state;       // 1 start, 2 charge, 3 hold, 4 release
    /* 0x84 */ s32            hahenIds[8]; // needlehahen
} OtuNeedle;                               // Size: 0xA4

/** "Tsk_OtosuGame_hand": a pin's grab attack, a hand thrust out along `dir`. */
typedef struct {
    /* 0x00 */ Sprite         sprite;
    /* 0x40 */ OamAffineParam affine;
    /* 0x50 */ OtuPoint       origin;
    /* 0x58 */ OtuPoint       pos;
    /* 0x60 */ OtuPoint       dir;
    /* 0x68 */ s32            pinId;
    /* 0x6C */ s32            visible;
    /* 0x70 */ s32            state; // 1 aim, 2 thrust, 3 hold, 4 retract
    /* 0x74 */ s32            timer;
} OtuHand;                           // Size: 0x78

/**
 * The engine's BG tilemap object (set up by func_0200d1d8, released through
 * func_0200d858). Only the leading words the overlay touches are named.
 */
typedef struct {
    /* 0x00 */ s32 display;
    /* 0x04 */ s32 layer;
    /* 0x08 */ s32 flags; // bit 1: the map needs re-uploading
    /* 0x0C */ u8  unk_0C[0x1C];
} OtuBgMap;               // Size: 0x28

/** The creation block of the board's three background tasks (floor, bg, ovbg). */
typedef struct {
    s32             dataType;
    Heap*           heap;
    OtuBoardLayout* layout;
} OtuBoardArgs;

/** One of the floor's animated palettes. */
typedef struct {
    /* 0x00 */ OtuPaletteAnim   anim;
    /* 0x10 */ PaletteResource* palette;
} OtuFloorPalette; // Size: 0x14

/** "Tsk_OtosuGame_floor": the board's tiled floor, a BG built from its cell grid. */
typedef struct {
    /* 0x00 */ s32              dataType;
    /* 0x04 */ Heap*            heap;
    /* 0x08 */ s32              loaded;
    /* 0x0C */ OtuBoardLayout*  layout;
    /* 0x10 */ OtuBgMap         bg;
    /* 0x38 */ u8**             tiles;    // per-cell pointers into tilePool
    /* 0x3C */ u8*              tilePool; // 0x800 bytes per 8x8-tile cell
    /* 0x40 */ Data*            animData;
    /* 0x44 */ Data*            data;
    /* 0x48 */ BgResource*      animChars;
    /* 0x4C */ PaletteResource* palette;
    /* 0x50 */ BgResource*      chars;
    /* 0x54 */ OtuFloorPalette  animPalettes[5];
} OtuFloor; // Size: 0xB8

/** "Tsk_OtosuGame_bg": the board's two scrolling background layers. */
typedef struct {
    /* 0x00 */ s32              dataType;
    /* 0x04 */ Heap*            heap;
    /* 0x08 */ s32              active; // the layout has background layers at all
    /* 0x0C */ OtuBoardLayout*  layout;
    /* 0x10 */ Data*            data[2];
    /* 0x18 */ PaletteResource* palettes[2];
    /* 0x20 */ BgResource*      chars[2];
    /* 0x28 */ OtuBgMap         maps[2];
    /* 0x78 */ void*            buffers[2];
    /* 0x80 */ s32              widths[2]; // Q12.12, the wrap points of the scroll
    /* 0x88 */ s32              heights[2];
    /* 0x90 */ s32              scrollX[2];
    /* 0x98 */ s32              scrollY[2];
} OtuBg; // Size: 0xA0

/**
 * @brief The board stage's block (`OtuStageDispatch.stageBlock`, 0x2D0 bytes):
 * the round's state and the handles of every task it creates. The single-player
 * board stage (OtuBoard.c) and the wireless menu and result stages
 * (OtuPinLogic.c) share it.
 */
typedef struct {
    /* 0x000 */ s32            step;       // the wireless stages' handshake step
    /* 0x004 */ u16            readyMask;  // one bit per player that has answered
    /* 0x006 */ u16            packetKind; // sent to and matched against the link partner
    /* 0x008 */ s32            gotPacket;
    /* 0x00C */ WMBssDesc      parent;     // the beacon 02088698 accepted
    /* 0x0CC */ u16            active;
    /* 0x0CE */ u8             pad_0CE[0x72];
    /* 0x140 */ s32            badgeCount;  // latched from state.unk_EE0
    /* 0x144 */ s32            obstacleCount;
    /* 0x148 */ s32            sparkCursor; // round-robin over sparkIds
    /* 0x14C */ s32            stageIndex;  // indexes the ten board tables
    /* 0x150 */ OtuBoardLayout layout;
    /* 0x16C */ OtuMatch*      match;
    /* 0x170 */ s32            timerSeconds; // the round's time limit
    /* 0x174 */ s32            timer;        // 0x258 on entry
    /* 0x178 */ s32            outcome;      // 020893fc's verdict: 1, 2 or 3
    /* 0x17C */ s32            badgeIds[4];
    /* 0x18C */ s32            sparkIds[0x40];
    /* 0x28C */ s32            obstacleIds[8];
    /* 0x2AC */ s32            slashId;
    /* 0x2B0 */ s32            floorId;
    /* 0x2B4 */ s32            bgId;
    /* 0x2B8 */ s32            ovbgId;
    /* 0x2BC */ s32            timerId;
    /* 0x2C0 */ s32            gaugeId;
    /* 0x2C4 */ s32            gameoverId; // in pool 1, like the next two
    /* 0x2C8 */ s32            wriconId;
    /* 0x2CC */ s32            wrwaitId;
} OtuBoardStage; // Size: 0x2D0

/** "Tsk_OtosuGame_piyo": the dizzy-stars sprite, banded by how much life is left. */
typedef struct {
    /* 0x00 */ Sprite   sprite;
    /* 0x40 */ OtuPoint origin;
    /* 0x48 */ OtuPoint pos;
    /* 0x50 */ s32      pinId;
    /* 0x54 */ s32      visible;
    /* 0x58 */ s32      stunMax; // the stun is banded as a percentage of this
    /* 0x5C */ s32      timer;   // frames until the next func_ov039_02087d04 nudge
} OtuPiyo;                       // Size: 0x60

/** "Tsk_OtosuGame_meteo": the meteor attack, with eight fragment children. */
typedef struct {
    /* 0x00 */ Sprite         sprite;
    /* 0x40 */ OamAffineParam affine;
    /* 0x50 */ OtuPoint       origin;
    /* 0x58 */ OtuPoint       pos;
    /* 0x60 */ s32            height;      // the pin's height, added into the drawn y
    /* 0x64 */ s32            pinId;
    /* 0x68 */ s32            alive;       // the phase == 8 filter's answer
    /* 0x6C */ s32            state;       // 0..3, entered through 0208fefc/0208ff30/0208ff68
    /* 0x70 */ s32            timer;
    /* 0x74 */ s32            hahenIds[8]; // "Tsk_OtosuGame_meteohahen" children
    /* 0x94 */ OtuScaleAnim   scaleAnim;
} OtuMeteo;                                // Size: 0xA0

/* Overlay data, still gap-filled from the ROM, by the build's names. */

/* The SDK's sine/cosine table: `s16` {sin, cos} pairs in Q12 for each of 4096
 * angles, so halfword `(angle >> 4) * 2` is the sine and the next one the
 * cosine. Declared as words, as the rest of the build declares it: reading it
 * through an `s16` array type lets mwcc reorder the loads past `s32` stores,
 * which the target does not (66.9% against 94.7% on func_ov039_02095788). */
extern s32 data_0205e4e0[];

/** The overlay-global phase byte: 1, 2, 3 or 4, raised by the stage's pollers. */
extern u8 data_ov039_0209ad00;

extern const BinIdentifier data_ov039_0209a0b4[5];

/* Macros */

/**
 * Frames a fragment launched at `vz` stays airborne: three times its time to
 * apex. A macro because the target evaluates it three times (sign test and
 * either arm), where a helper would be called once.
 */
#define OTU_AIRTIME(vz) ((FX_Divide((vz), data_ov039_0209a310) * 3) >> 0xC)

#define OTU_ABS_AIRTIME(vz) (OTU_AIRTIME(vz) < 0 ? -OTU_AIRTIME(vz) : OTU_AIRTIME(vz))

/* Helpers */

/**
 * @brief The palette block inside a loaded `Data`'s buffer.
 *
 * The overlay addresses this the same way in 020934e0 and 02093fcc, and the
 * arithmetic is `buffer + 0x20 + the word at buffer + 0x48` -- i.e. a table of
 * sub-resource offsets starting 0x20 into the buffer, indexed by one of them.
 * Which sub-resource it is (palette, char or screen) depends on the caller.
 */
static inline void* OtuPaletteSource(Data* file) {
    u8* buffer = (u8*)file->buffer + 0x20;

    return buffer + *(u32*)(buffer + 0x28);
}

/** One Q12.12 multiply, rounded to nearest. */
static inline s32 OtuQ12Mul(s32 a, s32 b) {
    return (s32)(((s64)a * b + 0x800) >> 12);
}

/* OtuWirelessStage.c */

void func_ov039_02087cac(TinPinSlammer_Scene* scene, s32 slot, const void* src, s32 len);

void func_ov039_02087d04(s32 se, OtuPoint* from, OtuPoint* to); // plays `se` panned by from - to

/* OtuTaskPick.c */

extern OtuBadge* func_ov039_02087e2c(TaskPool* pool, TinPinSlammer_Scene* scene, s32 index);

extern OtuBadge* func_ov039_0208817c(TaskPool* pool, TinPinSlammer_Scene* scene, s32 index);

extern OtuBadge* func_ov039_02088294(TaskPool* pool, TinPinSlammer_Scene* scene, s32 index);

/* OtuFieldAccess.c */

/** Number of children in a child list (`dispatch->stageBlock` + 0x144). */
s32 func_ov039_020883ac(TinPinSlammer_Scene* scene);

/** The `index`th child of a child list, resolved out of pool 2. */
void* func_ov039_020883c8(TinPinSlammer_Scene* scene, s32 index);

s32 func_ov039_02088400(s32 x, s32 y, s32 axis);

/** Maps the scene's mode selector onto a local player index.
 *
 *  Read from the target: 0 for mode 0, ov040's index for mode 1, 0 for anything
 *  else. Re-read inside several loops rather than hoisted, which is why the call
 *  sites below spell it out each time. */
s32 func_ov039_02088418(s32 mode);

/** The six-byte touch-pad descriptor for player slot `i`. */
void* func_ov039_02088440(s32 slot);

void func_ov039_02088454(void);

/** Polls the touch screen into one of those descriptors and resets the
 *  "was touched" latch. */
void func_ov039_02088564(void* pad);

void func_ov039_020885cc(void);

/** Wireless receive callback: (record, scene). Reached only as a function pointer. */
s32 func_ov039_020885f4(void* record, TinPinSlammer_Scene* scene);

void func_ov039_02088688(s32 unused, TinPinSlammer_Scene* scene);

/* OtuBoard.c */

/** Wireless receive callback: (record, scene). Reached only as a function pointer. */
void func_ov039_02088698(WMBssDesc* bss, TinPinSlammer_Scene* scene);

/** The stage's "board has changed" entry; clears state.unk_698 and re-seeds. */
void func_ov039_02088ec4(TinPinSlammer_Scene* scene);

/** The stage's board teardown. */
void func_ov039_02088f18(TinPinSlammer_Scene* scene);

void func_ov039_02089064(TinPinSlammer_Scene* scene, OtuPoint* at);

/** The board's per-frame physics, run at the end of both tick routines. */
void func_ov039_020894cc(TinPinSlammer_Scene* scene);

void func_ov039_02089780(TinPinSlammer_Scene* scene);

void func_ov039_020897d0(TinPinSlammer_Scene* scene);

void func_ov039_020897dc(TinPinSlammer_Scene* scene);

void func_ov039_020898a8(TinPinSlammer_Scene* scene);

void func_ov039_02089918(TinPinSlammer_Scene* scene);

/* OtuPinLogic.c */

void func_ov039_02089950(TinPinSlammer_Scene* scene);

void func_ov039_02089a80(TinPinSlammer_Scene* scene);

void func_ov039_02089d3c(TinPinSlammer_Scene* scene);

void func_ov039_02089d6c(TinPinSlammer_Scene* scene);

void func_ov039_02089e0c(TinPinSlammer_Scene* scene);

void func_ov039_02089e78(TinPinSlammer_Scene* scene);

void func_ov039_02089ec0(TinPinSlammer_Scene* scene);

void func_ov039_02089f30(TinPinSlammer_Scene* scene);

void func_ov039_02089f68(TinPinSlammer_Scene* scene);

void func_ov039_0208a098(TinPinSlammer_Scene* scene);

void func_ov039_0208a324(TinPinSlammer_Scene* scene);

void func_ov039_0208a354(TinPinSlammer_Scene* scene);

void func_ov039_0208a3f4(TinPinSlammer_Scene* scene);

void func_ov039_0208a454(TinPinSlammer_Scene* scene);

void func_ov039_0208a490(OtuBadge* self, s32 value);

void func_ov039_0208a6c4(OtuPoint* v);

void func_ov039_0208a6f8(OtuBadge* self, OtuPoint* dir, s32 angle);

u8 func_ov039_0208a794(OtuPoint* p, OtuBoardLayout* layout);

s32 func_ov039_0208a988(OtuPoint* self, OtuBoardLayout* layout, TinPinSlammer_Scene* scene);

void func_ov039_0208ac98(OtuBadge* self);

/* OtosuGame_badge.c */

/** Creates one pin child. Ten parameters, four in registers and six on the
 *  stack; the last six are the block's parameter block, the scene, the scene's
 *  +0x41EF0 block, one pin-tray slot, an "is this the first child" flag and a
 *  per-group sprite base. */
s32 func_ov039_0208dcb0(TaskPool* pool, s32 dataType, s32 slot, void* pad, void* params, void* scene, void* board,
                        void* traySlot, s32 isFirst, void* groupBase);

/** The pairwise interaction predicates: contact, push-apart, and effect-pull. */
s32 func_ov039_0208e28c(OtuBadge* a, OtuBadge* b);

s32 func_ov039_0208e37c(OtuBadge* a, OtuBadge* b);

s32 func_ov039_0208e504(OtuBadge* a, OtuObstacle* b);

void func_ov039_0208e6cc(OtuBadge* self, OtuPoint* out);

/** Copies the badge's position into `out`. */
void func_ov039_0208e6e0(OtuBadge* task, OtuPoint* out);

s32 func_ov039_0208e6f4(OtuBadge* task);

/** Reads a pin child's *own* aim point -- a different pair from the +0x120 one
 *  that func_ov039_0208e6e0 copies out. */
void func_ov039_0208e6fc(OtuBadge* self, OtuPoint* out);

void func_ov039_0208e848(OtuBadge* self, OtuPoint* origin);

/* The +0x110/+0x114, +0x118/+0x11C and +0x120/+0x124 pair copy-outs. */
void func_ov039_0208e85c(OtuBadge* self, OtuPoint* out);

void func_ov039_0208e870(OtuBadge* self, s32 x, s32 y);

void func_ov039_0208e87c(OtuBadge* self, OtuPoint* out);

s32 func_ov039_0208e890(OtuBadge* task);

s32 func_ov039_0208e8c4(OtuBadge* task);

s32 func_ov039_0208e950(OtuBadge* task);

/* The phase filters: true when the badge is in that phase. */
s32 func_ov039_0208e984(OtuBadge* task); // phase 8

s32 func_ov039_0208e998(OtuBadge* task); // phase 7

s32 func_ov039_0208e9ac(OtuBadge* task);

s32 func_ov039_0208e9d0(OtuBadge* task); // phase 6

s32 func_ov039_0208e9e4(OtuBadge* task); // phase 9

/** The pin child's "is this one worth resolving" test, and its follower. */
s32 func_ov039_0208e9f8(TaskPool* pool, OtuBadge* self);

void func_ov039_0208eaa0(OtuBadge* other, OtuBadge* self);

s32 func_ov039_0208ee84(OtuBadge* task); // alive

s32 func_ov039_0208ee98(OtuBadge* task);

s16 func_ov039_0208eea0(OtuBadge* task);

s16 func_ov039_0208eeac(OtuBadge* task);

s16 func_ov039_0208eeb8(OtuBadge* task);

s16 func_ov039_0208eec4(OtuBadge* task);

s32 func_ov039_0208eed0(OtuBadge* pin);

void func_ov039_0208ef14(void* pin, OtuPoint* a, OtuPoint* b);

s32 func_ov039_0208ef38(OtuBadge* pin);

extern s32 func_ov039_0208ef4c(void* task, u32 arg1);

s32 func_ov039_0208efb0(OtuBadge* task, s32 which);

s32 func_ov039_0208eff8(OtuBadge* task);

s32 func_ov039_0208f00c(OtuBadge* self);

void func_ov039_0208f024(OtuBadge* self);

s32 func_ov039_0208f034(OtuBadge* task);

void func_ov039_0208f03c(OtuBadge* task);

void func_ov039_0208f048(OtuBadge* self, OtuPoint* at, s32 selector);

s32 func_ov039_0208f0b0(OtuBadge* self);

void func_ov039_0208f0c8(OtuBadge* task);

void func_ov039_0208f0f0(OtuBadge* self, OtuBadge* other);

void func_ov039_0208f104(OtuBadge* self);

/* OtosuGame_shadow.c */

s32 func_ov039_0208f40c(TaskPool* pool, s32 dataType, s32 childId);

/* OtosuGame_piyo.c */

s32 func_ov039_0208f770(TaskPool* pool, s32 dataType, s32 childId);

void func_ov039_0208f7a4(OtuPiyo* self, s32 stunMax);

/* OtosuGame_marker.c */

s32 func_ov039_0208fa5c(TaskPool* pool, s32 dataType, s32 childId);

/* OtosuGame_meteo.c */

s32 func_ov039_0208fe60(TaskPool* pool, s32 dataType, s32 arg2);

void func_ov039_0208fee0(OtuMeteo* self, OtuPinRecord* out);

void func_ov039_0208fefc(OtuMeteo* self, s32 frames);

void func_ov039_0208ff30(OtuMeteo* self);

void func_ov039_0208ff68(OtuMeteo* self);

/* OtosuGame_hammer.c */

u32 func_ov039_02090e1c(TaskPool* pool, s32 arg1, s32 arg2);

s32 func_ov039_02090e9c(OtuHammer* self, OtuPinRecord* out);

s32 func_ov039_02091014(OtuHammer* self);

void func_ov039_02091028(OtuHammer* self, s32 rate, s32 scale, s32 frames, u16 arc);

void func_ov039_0209104c(OtuHammer* self);

u16 func_ov039_02091060(OtuHammer* self);

void func_ov039_02091070(OtuHammer* self);

/* OtosuGame_needle.c */

s32 func_ov039_020915a8(TaskPool* pool, s32 dataType, s32 pinId);

s32 func_ov039_02091628(OtuNeedle* self, OtuPinRecord* out);

void func_ov039_02091654(OtuNeedle* self, s32 chargeFrames, s32 holdFrames);

void func_ov039_02091668(OtuNeedle* self);

s32 func_ov039_0209167c(OtuNeedle* self);

void func_ov039_02091690(OtuNeedle* self);

/* OtosuGame_hand.c */

s32 func_ov039_02091b00(TaskPool* pool, s32 dataType, s32 pinId);

void func_ov039_02091b34(OtuHand* self);

/* OtosuGame_floor.c */

s32 func_ov039_02092310(TaskPool* pool, s32 dataType, Heap* heap, OtuBoardLayout* layout);

/** Places one of two sprites at a point, given a bearing and a length. */
void func_ov039_02092348(OtuFloor* self, s32 x, s32 y);

void func_ov039_020923b4(OtuFloor* self, s32 event);

/* OtosuGame_obstacle.c */

s32 func_ov039_020926f0(TaskPool* pool, s32 oamAttrs, s16 unk_04, s16 slot, OtuObstacle_Params* params);

void func_ov039_02092730(OtuObstacle* self, OtuPoint* origin);

void func_ov039_02092744(OtuObstacle* self, OtuPoint* out);

s32 func_ov039_02092758(OtuObstacle* self);

void func_ov039_02092760(OtuObstacle* self);

/* OtosuGame_bg.c */

s32 func_ov039_02092cd4(TaskPool* pool, s32 dataType, Heap* heap, OtuBoardLayout* layout);

void func_ov039_02092d0c(OtuBg* self, s32 x, s32 y);

void func_ov039_02092e04(OtuBg* self, s32 event);

/* OtosuGame_ovbg.c */

s32 func_ov039_020934a8(TaskPool* pool, s32 dataType, Heap* heap, OtuBoardLayout* layout);

void func_ov039_020934e0(OtuOvbg* data);

/* OtosuGame_badgeradar.c */

s32 func_ov039_0209383c(TaskPool* pool, s32 dataType, s32 pinId, s32 index, OtuBoardLayout* board, s32 isFirst);

/* OtosuGame_badgecount.c */

s32 func_ov039_02093cd8(TaskPool* pool, s32 dataType, s32 pinId, s32 index, s32 hasSpriteA);

s32 func_ov039_02093d18(OtuBadgeCount* data, u16* slots);

void func_ov039_02093d68(OtuBadgeCount* data, s32 value);

/* OtosuGame_timer.c */

s32 func_ov039_020941d0(TaskPool* pool, s32 dataType, s32 seconds);

s32 func_ov039_02094204(OtuTimer* self);

s32 func_ov039_0209420c(OtuTimer* self);

/* OtosuGame_specialgauge.c */

s32 func_ov039_02094ab4(TaskPool* pool, s32 dataType, s32 pin);

/* OtosuGame_spark.c */

/** Task factories. All six are the same three-line wrapper around
 *  EasyTask_CreateTask and all six return the new handle in r0. */
s32 func_ov039_02094e58(TaskPool* pool, s32 dataType);

/** Feeds one helper child a point and resets it. */
void func_ov039_02094e88(OtuSpark* self, OtuPoint* origin);

/** Feeds one helper child a point and a fresh random offset. */
void func_ov039_02094e9c(OtuSpark* self, OtuPoint* at);

/* OtosuGame_slash.c */

s32 func_ov039_02095468(TaskPool* pool, s32 dataType, s32 pin);

/* OtosuGame_track.c */

s32 func_ov039_02095750(TaskPool* pool, s32 dataType, s32 pin);

void func_ov039_02095788(OtuTrackTask* self, OtuPoint* at, s32 angle, s32 dir, s32 len);

/* OtosuGame_point.c */

s32 func_ov039_02095ca0(TaskPool* pool, s32 dataType, s32 pin);

void func_ov039_02095cd4(OtuPointTask* self, s32 count);

void func_ov039_02095ddc(OtuPointTask* self);

/* OtosuGame_entry.c */

s32 func_ov039_02096124(TaskPool* pool, s32 dataType);

void func_ov039_02096154(OtuEntryTask* self, s16 which, s32 hasLabel);

void func_ov039_02096270(OtuEntryTask* self);

/* OtosuGame_dead.c */

s32 func_ov039_02096548(TaskPool* pool, s32 dataType, s32 pinId);

void func_ov039_0209657c(OtuDead* self);

/* OtosuGame_gameover.c */

s32 func_ov039_02096b18(TaskPool* pool, s32 dataType);

void func_ov039_02096b48(OtuGameover* self, s32 which);

s32 func_ov039_02096c44(OtuGameover* self);

/* OtosuGame_wricon.c */

s32 func_ov039_02096e4c(TaskPool* pool, s32 dataType);

/* OtosuGame_meteohahen.c */

s32 func_ov039_0209720c(TaskPool* pool, s32 dataType, s32 pinId);

void func_ov039_02097240(OtuHahen* self, OtuPoint* at);

/* OtosuGame_smoke.c */

s32 func_ov039_0209771c(TaskPool* pool, s32 dataType, s32 pinId);

void func_ov039_02097750(OtuSmoke* self, OtuPoint* at, s32 selector, s32 closeBy, s32 step, s32 holdFor);

/* OtosuGame_warp.c */

s32 func_ov039_02097a70(TaskPool* pool, s32 dataType, s32 pinId);

void func_ov039_02097aa4(OtuWarp* self, OtuPoint* at, s32 animation);

s32 func_ov039_02097ad8(OtuWarp* self);

/* OtosuGame_needlehahen.c */

s32 func_ov039_02097e68(TaskPool* pool, s32 dataType, s32 pinId);

void func_ov039_02097e9c(OtuHahen* self, OtuPoint* at);

/* OtosuGame_hammerhahen.c */

s32 func_ov039_02098394(TaskPool* pool, s32 dataType, s32 pinId);

/* The per-child step func_ov039_02091070 runs four times. */
void func_ov039_020983c8(OtuHammerHahen* self, OtuPoint* at, OamAffineParam* affine, s32 index);

/* OtosuGame_wrwait.c */

s32 func_ov039_020989f0(TaskPool* pool, s32 dataType);

/* OtuStageDispatch.c */

void func_ov039_02098a50(OtuStageDispatch* dispatch);

void func_ov039_02098b78(OtuPoint* src, OtuPoint* dst);

void func_ov039_02098b8c(OtuPoint* a, OtuPoint* b, OtuPoint* out);

/* OtuVecOps.c */

s32 func_ov039_02098c40(OtuPoint* a, OtuPoint* b);

s32 func_ov039_02098d10(OtuPoint* v);

void func_ov039_02098d3c(OtuPoint* src, OtuPoint* dest);

extern s32 func_ov039_02098d7c(void* anim, s32 base, const void* table, s16 count);

extern s32 func_ov039_02098dbc(void* anim);

/* Outside the overlay, and in overlay 40 (wireless). */

/* The base module's fixed-point divide.  No prototype exists in this repo,
 * but the two call sites here both pass a quotient-remainder pair. */
s32 _s32_div_f(s32 a, s32 b);

/** Population count of a bit mask. Declared the same way Boss03.c declares it,
 *  since the overlay uses it for exactly one thing here: how many players share
 *  a top score. */
s32 func_02047e84(u16 bits);

void func_ov040_0209d588(void);

s32 func_ov040_0209d728(void);

void func_02047338(void);

void func_0200d1d8(void* obj, s32 a, s32 b, s32 c, void* buf, s32 w, s32 h);

#endif
