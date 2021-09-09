#ifndef _SOUNDDRV_H_
#define _SOUNDDRV_H_

/// Resolution of a half note (1 sec. if BPM = 120).
/// Sound driver uses a timer, and its initial count is obtained
/// by '0x10000 - (0x1000000 / FREQ)'.
#define INT_FREQ    576

/// Tick count for four beats.
#define FOURBEAT    (INT_FREQ << 1)

#define GAMEMODE_YES    1
#define GAMEMODE_NO     0

/**
 * Loads song data to DMG buffer (Inits to remove prev. music)
 * @param idx Song index
 * @param gmf Game Mode Flag. Set 1 to delay sound channels for notes.
 */
void DmgLoad(short songidx, short gmf);
void DmgPlay(void);
void DmgStop(void);
void DmgInit(void);
void DmgStep(void);

#endif
