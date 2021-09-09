#ifndef _BGOBJMGR_H_
#define _BGOBJMGR_H_

#define BG_TOTAL_TILES_PER_MAP  4096   /// 64*64
#define BG_TILE_FLIP_H  0x400
#define BG_TILE_FLIP_V  0x800

#define BG0_MAP 16
#define BG1_MAP 20
#define BG2_MAP 24
#define BG3_MAP 28


void BgInit(void);

#define BGZC0 1
#define BGZC1 2
#define BGZC2 4
#define BGZC3 8
#define BGZCA 15

/// Zero-clear the maps
/// @param flag
void BgZeroClear(short flag);

/// Put a tile
/// @param bgnum Specify which BG to put the string to (0-3).
/// @param tx Tile-based x coordinate
/// @param ty Tile-based y coordinate
/// @param tileId Tile ID (0-1023?)
/// @remarks ALL tiles including Id=0 are put.
void PutTile(
    short bgnum,
    short tx,
    short ty,
    short tileId);

/// Put a string to vram map
/// @param bgnum Specify which BG to put the string to (0-3).
/// @param tx Tile-based x coordinate
/// @param ty Tile-based y coordinate
/// @param str String to put
/// @remarks ALL chars including control characters are put, except only '\0'.
void PutStr(
    short bgnum,
    short tx,
    short ty,
    const char *str);

/// Put a short value to vram map
/// @param bgnum Specify which BG to put the string to (0-3).
/// @param tx Tile-based x coordinate
/// @param ty Tile-based y coordinate
/// @param num Value to put
/// @remarks Right-aligned.
void PrintShort(
    short bgnum,
    short tx,
    short ty,
    short num);

/// Put OBJ to (px, py).
/// @param spnum OBJ number
/// @param px X-coordinate (in px)
/// @param py Y-coordinate (in px)
void ObjPut(
    char spnum,
    short px,
    short py);

/// Change image tile associated with OBJ
/// @param spnum OBJ number
/// @param chnum Tile number
void ObjAnimate(
    char spnum,
    short chnum);

/// Apply Rotation/Scaling Parameter to OBJ.
/// @param spnum OBJ ID
/// @param rspgroup RS Parameter ID (out of [0 .. 31] turns off the rotation)
void ObjApplyRotate(
    char spnum,
    char rspgroup);

/// Flips OBJ.
/// @param spnum OBJ ID
/// @param fMode Flip flag to apply (0 to normal)
void ObjFlip(
    char spnum,
    short fMode);

/// Sets Rotation/Scaling Parameters.
/// @param rspindex Specify R/S Param group (0-31)
/// @param pa  Cos (alpha) / xMag
/// @param pb -Sin (alpha) / xMag
/// @param pc  Sin (alpha) / yMag
/// @param pd  Cos (alpha) / yMag
void ObjSetRSParam(
    char rspindex,
    short pa,
    short pb,
    short pc,
    short pd);

/// Sets Rotation/Scaling Parameters.
/// @param rspindex Specify R/S Param group (0-31)
/// @param rsp Pointer to struct tagRSPARAMETER
void ObjSetRSParamIndirect(
    char rspindex,
    RSPARAMETER *rsp);

#endif
