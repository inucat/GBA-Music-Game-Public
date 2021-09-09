#include "meta.h"

#define SH_KEY_TX     2
#define SH_KEY_TY     2
#define SH_KEY_HOLDED_TILE_OFFSET 6

const char keys[] = {0x82, 0x84, 0x85, 0x83, 0x86, 0x87, 0x88, 0x8a, 0x8b, 0x89, 0x8c, 0x8d};
const hword key_mask[] = {KEY_DU, KEY_DL, KEY_DR, KEY_DD, KEY_A, KEY_B};

__inline__ static void draw_keys(void) {
    for (short i=0; i < 6; i++)
        PutTile(
            0,
            SH_KEY_TX,
            SH_KEY_TY + (i<<1),
            keys[i + (keyCurrent & key_mask[i] ? SH_KEY_HOLDED_TILE_OFFSET : 0)]);
}

static void draw_lines(void) {
    PutStr(0, 3, 0, CODENAME);
    for (short s=1; s<=13; s+=2)
        for (short i=0; i < LCD_VTWIDT; i++)
            PutTile(0, i, s, 0x81);

    for (short t=1; t <= 13; t++)
        for (short s=0; s < LCD_VTWIDT; s++)
            PutTile(1, s, t, 2);
}

void SheetStep(void) {
    switch (gameState)
    {
    case GS_SHEETINIT:
        for (short t=0; t < LCD_VTHEIG; t++)
            for (short s=0; s < LCD_VTWIDT; s++)
                PutTile(3, s, t, 0x02 + (((s^t)&2)&&1));

        draw_lines();
        draw_keys();
        NotesInit();
        PutStr(0, 0, 19, "Are you ready?  Press any key!");
        SetGameState(GS_STANDBY);
        return;

    case GS_STANDBY:
        if (KeyTyped(KEY_ALL))
            SetGameState(GS_PLAYSTART);
        return;

    case GS_PLAYSTART:
        for (short i=0; i < LCD_VTWIDT; i++)
            PutTile(0, i, 19, 0);
        DmgPlay();
        SetGameState(GS_PLAYING);
    case GS_PLAYING:    // intended fall-through
        hitKey();
        draw_keys();
        NotesStep();
        reg16(BG3OFSH) = IWRAM_DMGTICKC >> 5;

        if (KeyTyped(KEY_STA)) {
            DmgStop();
            DSPlay(SFX_CURSOR);
            PutStr(0, 5, 19, "PAUSED (L&R to quit)");
            SetGameState(GS_PAUSE);
        }
        else if (KeyTyped(KEY_DU|KEY_DD|KEY_DL|KEY_DR))
            reg16(DMG4RLN) = RLFN_RESET | RLN_div(3) | RLN_pstep(1);
        else if (KeyTyped(KEY_A|KEY_B))
            reg16(DMG4RLN) = RLFN_RESET | RLN_div(3) | RLN_pstep(4);
        return;

    case GS_PAUSE:
        if (KeyPressed(KEY_SL) && KeyPressed(KEY_SR)) {
            DSPlay(SFX_CANCEL);
            SetGameState(GS_RESULT);
        }
        else if (KeyTyped(KEY_STA)) {
            SetGameState(GS_PLAYSTART);
        }
        return;

    case GS_RESULT:

        PutStr(0, 9, 14, "Hit :");
        PrintShort(0, 14, 14, ok_count);
        PutStr(0, 9, 17, "Miss:");
        PrintShort(0, 14, 17, notes_sum - ok_count);
        PutStr(0, 0, 19, "Finish! Push anykey to proceed");

        if (KeyTyped(KEY_ALL)) {
            DSPlay(SFX_CURSOR);
            while(reg16(TM0CTRL));
            notes_clearall();
            BgZeroClear(BGZCA);
            DmgStop();
            SetGameState(GS_MENUINIT);
        }
        return;
    }
}
