#ifndef _MUSIC_H_
#define _MUSIC_H_

#include "exgba.h"

#define NUM_OF_SOUNDCHS 4
#define SND_SHT_CHS     5
#define TITLE_BGM 0

/// Sound channel 1-4, song title, and Sheet note track 1-3
enum SONGDATA_TRACKS {
    CH1,
    CH2,
    CH3,
    CH4,
    SONGTITLE,
    SHEET1,
    SHEET2,
    SHEET3,
    TOTAL_CHS
};

/// Song Data
extern const byte *const songs[][TOTAL_CHS];

/// @note Do NOT remove or rearrange these items if at least one
/// song data is converted with the actual command values, or the command
/// values will conflict with the ENUM value and the
/// song will get a mess.
enum SONGTRACK_CONTROL_COMMANDS {
    REST = 128,
    DRCG,// Duration Change
    TPDC,// Temporary Duration Change
    DOTT,// Dotted note
    RPHD,// Repeat Section Head
    RPTL,// Repeat Section Tail
    TMCG,// Tempo Change
    INIT,// Initialization Section Signal
    INED,// Init. Section End
    PCSF,// Pitch Shift
    DRDT,// Duration Dotted (ex. "l4.")
    VECG,// Velocity Change
    TERM = 255// Terminate Signal
};

#endif
