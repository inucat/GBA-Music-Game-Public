#include "meta.h"

/// BG

hword* const pBgMap[] = {
    (hword*) VRAM_MAP(BG0_MAP),
    (hword*) VRAM_MAP(BG1_MAP),
    (hword*) VRAM_MAP(BG2_MAP),
    (hword*) VRAM_MAP(BG3_MAP)};


void BgInit(void)
{
    hword *const vramBGTile = (hword *) VRAM_TILE(0);
    hword *const paletteBG = (hword *) PALETTE_BG;
    reg16(LCDCNT) = LCDCNT_BGMODE(0) |\
        LCDCNT_BG0ON | LCDCNT_BG1ON | LCDCNT_BG2ON | LCDCNT_BG3ON;
    reg16(BG0CNT) = BGCNT_MAP3232 | BGCNT_COL256 | BGCNT_TILENo(0) | BGCNT_MAPNo(BG0_MAP); // 32x32 Tiles VMap with 256 colors
    reg16(BG1CNT) = BGCNT_MAP3232 | BGCNT_COL256 | BGCNT_TILENo(0) | BGCNT_MAPNo(BG1_MAP);
    reg16(BG2CNT) = BGCNT_MAP3232 | BGCNT_COL256 | BGCNT_TILENo(0) | BGCNT_MAPNo(BG2_MAP);
    reg16(BG3CNT) = BGCNT_MAP3232 | BGCNT_COL256 | BGCNT_TILENo(0) | BGCNT_MAPNo(BG3_MAP);
    for (short s=0; s < PALETTE_COL; s++)
        paletteBG[s] = BGtilesPal[s];

    for (short s=0; s < (BGtilesTilesLen>>1); s++)
        vramBGTile[s] = BGtilesTiles[s<<1] | (BGtilesTiles[(s<<1)+1] << 8);
}


void BgZeroClear(short flag) {
    for (short s=0; s<4; s++) {
        if ((flag >> s) & 1) {  /// Zero clear for BG0 .. BG3
            for (short t=0; t < BG_TOTAL_TILES_PER_MAP; t++)
                pBgMap[s][t] = 0;
            reg32(BG0OFSH + s*2) = 0;  // H&V offset 0-cleared
        }
    }
}


void PutTile(short bgnum, short tx, short ty, short tileId) {
    pBgMap[bgnum][ tx +  ty * LCD_VTWIDT ] = tileId;
}

void PutStr(short bgnum, short tx, short ty, const char *str)
{
    for (short i=0; str[i]; i++) {
        pBgMap[bgnum][tx + i + ty * LCD_VTWIDT] = str[i];
    }
}

void PrintShort(short bgnum, short tx, short ty, short num)
{
    char szDigits[] = "     ";  // 3,2,7,6,7,\0
    for (short i=0; i<=4; i++) {
        szDigits[4-i] = num % 10 + 0x30;
        num /= 10;
        if (!num) break;
    }
    PutStr(bgnum, tx, ty, szDigits);
}

/// OBJ

void ObjPut(char spnum, short px, short py) {
    volatile SPRITEATTR *sp = (SPRITEATTR *) OAM;
    sp += spnum;

    sp->attr0 &= ~OBJA0_YMASK;
    sp->attr0 |= (OBJA0_YMASK & py);
    sp->attr1 &= ~OBJA1_XMASK;
    sp->attr1 |= (OBJA1_XMASK & px);
}

void ObjAnimate(char spnum, short chnum) {
    volatile SPRITEATTR *sp = (SPRITEATTR *) OAM;
    sp += spnum;
    sp->attr2 &= ~OBJA2_TILEIDMASK;
    sp->attr2 |= (chnum & OBJA2_TILEIDMASK);
}

void ObjApplyRotate(char spnum, char rspgroup) {
    volatile SPRITEATTR *sp = (SPRITEATTR *) OAM;
    sp += spnum;

    // sp->attr0 &= ~OBJ_SWT_TURNOFF;
    if (rspgroup & 0xE0) {
        sp->attr0 &= ~OBJA0_ROTATE_ON;
        sp->attr1 &= ~OBJA1_RSPMASK;
        return;
    }
    sp->attr0 |= 3 << 8;
    sp->attr1 &= ~OBJA1_RSPMASK;
    sp->attr1 |= rspgroup << 9;
}

void ObjFlip(char spnum, short fMode) {
    volatile SPRITEATTR *sp = (SPRITEATTR *) OAM;
    sp += spnum;

    sp->attr0 &= ~OBJA0_ROTATE_ON;
    sp->attr1 &= ~OBJA1_RSPMASK;
    sp->attr1 |= (fMode & OBJA1_FLIPMASK);
}

void ObjSetRSParam(
    char rspindex,
    short pa,
    short pb,
    short pc,
    short pd)
{
    volatile hword *paddA = (hword *) OAM + 3 + rspindex * 4;

    reg16(paddA) = pa;
    reg16(paddA + 4) = pb;
    reg16(paddA + 8) = pc;
    reg16(paddA + 12) = pd;
}

void ObjSetRSParamIndirect(
    char rspindex,
    RSPARAMETER *rsp)
{
    ;
}
