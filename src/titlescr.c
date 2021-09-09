#include "meta.h"

#define TT_TITLE_TX     7
#define TT_TITLE_TY     7
#define TT_AUTHOR       "Program by  inucat"
#define TT_AUTHOR_TX    6
#define TT_AUTHOR_TY    19
#define TT_PROMPT       "Press START!"
#define TT_PROMPT_TX    9
#define TT_PROMPT_TY    12

void TitleStep(void) {
    switch (gameState)
    {
    case GS_SPLASH: /// Draw title scr.
        if (!(reg16(TM2CTRL) & TM_START)) {
            DmgLoad(TITLE_BGM, 0);
            DmgPlay();
        }
        PutStr(0, TT_TITLE_TX, TT_TITLE_TY, "\xa3\xa4\xa5\xa6\xa7\xa8\xa9 \xa3\xa4\xab\xac\xad\xae");
        PutStr(0, TT_TITLE_TX, TT_TITLE_TY + 1, "\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xb3\xb4\xbb\xbc\xbd\xbe\xbb\xcc");
        PutStr(0, TT_TITLE_TX, TT_TITLE_TY + 2, "  \xc5\xc6");
        PutStr(0, TT_AUTHOR_TX, TT_AUTHOR_TY, TT_AUTHOR);
        /// Put background tiles
        for (short t=0; t < LCD_VTHEIG; t++)
            for (short s=0; s < LCD_VTWIDT; s++)
                PutTile(3, s, t, 0x04 + (((s^t)&4)&&1));

        SetGameState(GS_TITLELOOP);
        return;

    case GS_TITLELOOP:  /// Wait for key input
    case GS_ENTERINGMENU:   /// Blink the prompt for a while (eyecandy)
        reg16(BG3OFSH) = reg16(BG3OFSV) = IWRAM_FRAMEC>>2;
        if (gameState != GS_TITLELOOP) {
            PutStr(0, TT_PROMPT_TX, TT_PROMPT_TY, (IWRAM_FRAMEC & 4) ? TT_PROMPT : "            ");
            if (reg16(TM0CTRL))
                return;
            if (gameState == GS_ENTERINGMENU) {
                BgZeroClear(BGZC0 | BGZC1);
                SetGameState(GS_MENUINIT);
            }
            return;
        }

        PutStr(0, TT_PROMPT_TX, TT_PROMPT_TY, ((IWRAM_FRAMEC) & 16) ? TT_PROMPT : "            ");
        if (KeyPressed(KEY_STA)) {
            DSPlay(SFX_START);
            SetGameState(GS_ENTERINGMENU);
            return;
        }
        return;
    }
}
