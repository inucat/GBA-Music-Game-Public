/// OBJ Driver
/// @author Hayato Kohara
///
/// Modified by inucat

#include "meta.h"

/// OBJ Tile data address offset in VRAM for the next loaded image
static int chrtop = 0;

void ObjInit(const hword *obj_palette) {
    short i;
    hword* pal = (hword*) PALETTE_OBJ;

    for (i = 0; i < PALETTE_COL; i++)
        pal[ i ] = obj_palette[ i ];

    /* OAM 0 clear */
    for (i = 0; i < OBJMAXNUMBER; i++)
        ObjAlloc(i, 0, 0, 0);

    /* OBJ mapping mode fixed to 1D */
    reg16(LCDCNT) |= LCDCNT_OBJON | LCDCNT_OBJ1D;
}

/// Load OBJ graphics to VRAM.
/// @param ch ?
/// @param size OBJ size in tiles (8x8 px == 1 tile)
/// @return VRAM offset of the loaded image (head of the loaded image)
int ObjLoadImage(OBJIMG *ch, int size) {
    short i, j;
    int chnum;
    volatile OBJIMG *ptr;
    chnum = chrtop;     /* Loading start position will be returned */
    /* Return -1 if no more tile data can be loaded to VRAM */
    if ((chnum + (size * 2)) > OBJMAXIMAGE) {
        return -1;
    }
    /* Load images by 8x8 px to VRAM */
    ptr = (OBJIMG *) OBJ_TILE;
    ptr += (chrtop >> 1);
    for (i = 0; i < size; i++) {
        for (j = 0; j < 32; j++) {
            ptr->data[ j ] = ch[ i ].data[ j ];
        }
        ptr++;
        chrtop += 2;/* Add to the offset by the tile data size */
    }
    return chnum;
}

/// Allocate OBJ to OAM
/// @param spnum OBJ number associated with the OBJ (0-127)
/// @param shape ?
/// @param priority Display priority (0-3, 0 is the highest)
/// @param chnum OBJ image index returned from @p ObjLoadImage() (0-1023)
void ObjAlloc(char spnum, short shape, short priority, short chnum) {
    short i, j;
    volatile SPRITEATTR *sp = (SPRITEATTR *) OAM;
    sp += spnum;

    /* Color mode is fixed to 256-color */
    sp->attr0 = OBJA0_COL256 | OBJA0_HIDDEN;

    sp->attr1 |= 0;

    sp->attr2 = ((priority & 3) << 10);
    sp->attr2 |= (chnum & OBJA2_TILEIDMASK);

    i = shape >> 2;
    sp->attr0 |= (i << 14);
    j = shape & 0b11;
    sp->attr1 |= (j << 14);

    sp->dummy  = 0;
}

void ObjShowHide(char spnum, short disp) {
    volatile SPRITEATTR *sp = (SPRITEATTR *) OAM;
    sp += spnum;

    if (disp)
        sp->attr0 &= ~OBJA0_HIDDEN;
    else
        sp->attr0 |= OBJA0_HIDDEN;
}
