#ifndef _MAIN_H_
#define _MAIN_H_

#define CODENAME      "RHYTHMIC-REINDEER"

#define IWRAM_DMGTICKC  (reg32(IWRAM + 0x00))
#define IWRAM_FRAMEC    (reg32(IWRAM + 0x04))
#define IWRAM_OBJIMGOFS (reg32(IWRAM + 0x08))

/// 16-bit fixed decimal
/// High-order 9-bit as unsigned integer, lower-bits as decimal.
typedef short sfix;
#define sf2s(sf) (sf>>7)
#define s2sf(s) (s<<7)

/// Game state
enum GAMESTATE {
	GS_SPLASH,
	GS_TITLELOOP,
	GS_ENTERINGMENU,	//	Start button pressed in the title screen
	GS_MENUINIT,
	GS_MENULOOP,
	GS_SONGSELECTED,
	GS_SHEETINIT,
	GS_STANDBY,			//	Before the game begins
	GS_PLAYSTART,
	GS_PLAYING,
	GS_PAUSE,
	GS_RESULT,
	GS_EASTERENTER,
	GS_EASTERLOAD,
	GS_EASTERPLAY,
	GS_EASTERPAUSE,
	NUM_OF_GAMEMODE
};

extern volatile short gameState;

void SetGameState(short new_state);

#endif
