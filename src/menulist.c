#include "meta.h"

#define LIST_ROWS 14
#define LIST_TOP_OFFSET_T 2
#define LIST_CURSOR_TX 2
#define RESERVED_SONGS 1

enum BGNUM_FOR_ELEMENTS {
    BG_FRAM,
    BG_FRBG,
    BG_LIST,
    BG_BACK
};

static short songsNum = 0;
static short tcurpos = 0;
static short tscrval = 0;

static void draw_frame(void) {
    for (short t=0; t < LCD_VTHEIG; t++)
        for (short s=0; s < LCD_VTWIDT; s++)
            PutTile(BG_BACK, s, t, 0x04 + (((s^t)&4)&&1));

    for (songsNum = RESERVED_SONGS; *songs[songsNum]; songsNum++) {
        PutStr(
            BG_LIST,
            LIST_CURSOR_TX + 1,
            songsNum + LIST_TOP_OFFSET_T - RESERVED_SONGS,
            songs[songsNum][SONGTITLE]);
    }
    songsNum -= RESERVED_SONGS; /// songs[0] is reserved for title song.
    PutStr(BG_FRAM, 0, 0, "\x90\x91\x91\x91\x91\x91\x94 CHOOSE A SONG \x93\x91\x91\x91\x91\x91\x91\x92");
    for (short i=1; i<18; i++) {
        PutTile(BG_FRAM,  0, i, 0xa0);
        PutTile(BG_FRAM, 29, i, 0xa2);
    }
    PutTile(BG_FRAM, 0, 18, 0xb0);
    PutTile(BG_FRAM, 29,18, 0xb2);
    for (short i=1; i < 29; i++) PutTile(BG_FRAM, i, 18, 0xb1);
    PutStr(BG_FRAM, 0, 19, "\x8E/\x8F:Move  A:Listen  START:Play");
}

void MenuStep() {
    switch (gameState)
    {
    case GS_MENUINIT:
        draw_frame();
        if (!(reg16(TM2CTRL) & TM_START)) {
            DmgLoad(TITLE_BGM, 0);
            DmgPlay();
        }
        SetGameState(GS_MENULOOP);
        return;

    case GS_MENULOOP:
    case GS_SONGSELECTED:
        reg16(BG3OFSH) = reg16(BG3OFSV) = IWRAM_FRAMEC>>2;
        if (gameState == GS_SONGSELECTED) {
            if (reg16(TM0CTRL)) {
                PutStr(
                    BG_LIST,
                    LIST_CURSOR_TX + 1,
                    tcurpos + LIST_TOP_OFFSET_T,
                    IWRAM_FRAMEC & 4 ? songs[tcurpos + RESERVED_SONGS][SONGTITLE] : "                           ");
                return;
            }
            BgZeroClear(BGZCA);
            DmgStop();
            DmgLoad(tcurpos + RESERVED_SONGS, TRUE);
            SetGameState(GS_SHEETINIT);
        } else {
            PutTile(BG_LIST, LIST_CURSOR_TX, tcurpos + LIST_TOP_OFFSET_T, 0);   /// Remove previous cursor
            if (KeyTyped(KEY_DD)) {
                tcurpos = (tcurpos == songsNum - 1) ? 0 : tcurpos + 1;
                tscrval = (tcurpos - tscrval > LIST_ROWS) ? tscrval + 1 : 0;
                DSPlay(SFX_CURSOR);
            } else if (KeyTyped(KEY_DU)) {
                tcurpos = tcurpos ? tcurpos - 1 : songsNum - 1;
                tscrval = tcurpos >= LIST_ROWS ? tcurpos - LIST_ROWS : 0;
                DSPlay(SFX_CURSOR);
            }
            reg16(BG2OFSV) = tscrval << 3;          /// Scroll list
            PutTile(BG_LIST, LIST_CURSOR_TX, tcurpos + LIST_TOP_OFFSET_T, '>');    /// Put current cursor
            if (KeyTyped(KEY_A)) {
                DmgLoad(tcurpos + RESERVED_SONGS, FALSE);
                DmgPlay();
            }
            else if (KeyTyped(KEY_STA)) {
                DSPlay(SFX_START);
                SetGameState(GS_SONGSELECTED);
            }
            else if (KeyTyped(KEY_B)) {
                DSPlay(SFX_CANCEL);
                BgZeroClear(BGZCA & ~BGZC3);
                SetGameState(GS_SPLASH);
            }
        }
        return;
    }
}
