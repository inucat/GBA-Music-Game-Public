#include "meta.h"

/// Flag of Game mode or not
static byte gameMode = 0;

/// Note Num. --> Freq_G value
static hword FreqG[128] = {0};

/// Internal step counter
static int* const tickCount = (int *) 0x3000000;

/// Tempo & Pitch shifter
static hword tempoMusic = 120;
static hword tempoNotes = 120;

/// Pointer to CHANNELS of a song.
static const byte* songData[SND_SHT_CHS];

/// Delay of music (Constant?)
static hword tickGap = FOURBEAT;

/// Output for each channel
static const int ChOutput[NUM_OF_SOUNDCHS] = {DMG1RLF, DMG2RLF, DMG3RLF, DMG4RLN};

static unsigned int nextTick[SND_SHT_CHS] = {0};
static hword cmdIndex[SND_SHT_CHS]   = {0};
static hword mnoteLen[SND_SHT_CHS]       = {4,4,4,4,4};
static hword pplenDot[SND_SHT_CHS]  = {0};
static hword repHead[SND_SHT_CHS]   = {0};
static hword repCount[SND_SHT_CHS]    = {0};

void DmgLoad(short idx, short gmf) {
    gameMode = gmf;
    DmgStop();
    for (int i=CH1; i<SND_SHT_CHS; i++) {
        if (i != SONGTITLE) songData[i] = songs[idx][i];
        else if (gmf) songData[i] = songs[idx][SHEET1]; /// SHEET1 to SHEET3 could be used to change difficulty
        else songData[i] = "\xff";
    }
    *tickCount = 0;
    tempoMusic = tempoNotes = 120;
    for (short ch = CH1; ch < SND_SHT_CHS; ch++) {
        nextTick[ch] = 0;
        cmdIndex[ch] = 0,
        mnoteLen[ch] = 4,
        pplenDot[ch] = 0,
        repHead[ch] = 0,
        repCount[ch] = 0;
    }
    if (songData[4][0] == TMCG) tempoNotes=songData[4][1]<<1;
    tickGap = FOURBEAT;
    reg16(TM2COUNT) = 65536 - 16777216 / INT_FREQ;
}

void DmgPlay() {
    reg16(SNDMASTER) = SNDMASTER_ON;
    reg16(DMGCNT) |= DMGCNT_ALLON;
    reg16(TM2CTRL) = TM_START | TM_PRESC0001 | TM_IRQEN;
}

void DmgStop() {
    reg16(TM2CTRL) = 0;
    reg16(DMGCNT) &= ~DMGCNT_ALLON;
}

void DmgInit() {
    int freqbase[] = { 8372, 8870, 9397, 9956, 10548, 11175, 11840, 12544, 6645<<1, 7040<<1, 7459<<1, 7902<<1 };
    for (int i=127, f=0; i>=36; i--) {
        f = freqbase[i%12] >> (10 - i/12);
        FreqG[i] = RLF_freq(f);
    }
    reg16(DMGMSW)= DMGMSW_ON;
    reg16(DMGMVL)|= DMGMVL_100;
    reg16(DMGCNT)= DMGCNT_ALLON | DMGCNT_Lvol(7) | DMGCNT_Rvol(7);
    reg16(DMG1SWEEP)= SWEEP_OFF;
    reg16(DMG1EDL)= EDL_DUTY50 | EDL_ENVDEC | EDL_ENVstep(7) | EDL_ENVvol(12);
    reg16(DMG2EDL)= EDL_DUTY25 | EDL_ENVDEC | EDL_ENVstep(7) | EDL_ENVvol(12);
    reg16(DMG4EDL)= EDL_ENVDEC | EDL_ENVstep(2) | EDL_ENVvol(10);
    reg16(DMG3VL)= DMG3VL_VOL75;
    reg16(DMG3BANKSEL)= BANKSEL_2x32 | BANKSEL_PLAY1;
    #define Wave1 0x07FFFFFF
    #define Wave2 0xFFFFF711
    reg32(DMG3WRAM(0))= MAKEINTWRAM(Wave1);
    reg32(DMG3WRAM(4))= MAKEINTWRAM(Wave2);
    reg32(DMG3WRAM(8))= MAKEINTWRAM(Wave1);
    reg32(DMG3WRAM(12))= MAKEINTWRAM(Wave2);
    reg16(DMG3BANKSEL)= BANKSEL_ON | BANKSEL_2x32 | BANKSEL_PLAY0;
}

void DmgStep() {
    for (byte ch = CH1; ch < SND_SHT_CHS; ch++) {
        //  When tick count reaches next command tick ...
        if((ch < NUM_OF_SOUNDCHS && *tickCount>= nextTick[ch] + tickGap) || (ch == 4 && *tickCount >= nextTick[4]))
        {
            if (songData[ch][cmdIndex[ch]] == TERM) {
                nextTick[ch] = 0x7fffffff;
                if (ch == CH1) notes_callend();
                continue;
            }

            hword singleLen=0, singleDot=0;

            /// Special Commands are processed first
            while (songData[ch][cmdIndex[ch]] > REST)
            {
                switch (songData[ch][cmdIndex[ch]])
                {
                case DRCG:
                    mnoteLen[ch] = songData[ch][++cmdIndex[ch]];
                    break;

                case TPDC:// not needed --> really?
                    singleLen = mnoteLen[ch];
                    mnoteLen[ch] = songData[ch][++cmdIndex[ch]];
                    break;

                case DOTT:
                    singleDot = songData[ch][++cmdIndex[ch]];
                    break;

                case RPHD:
                    repHead[ch] = cmdIndex[ch];
                    repCount[ch] = 0;
                    break;

                case RPTL:
                    repCount[ch]++;
                    hword repmax = songData[ch][++cmdIndex[ch]];     /// Get repeat limit
                    if (repmax == 0 || repCount[ch] < repmax)           /// Repeat forever or for maximum times.
                        cmdIndex[ch] = repHead[ch];
                    break;

                case DRDT:
                    pplenDot[ch] = songData[ch][++cmdIndex[ch]];
                    break;

                case TMCG:
                    if (ch != 4) {
                        tempoMusic = songData[ch][++cmdIndex[ch]] << 1;
                        reg16(TM2COUNT) = (65536 - (16777216 *120/ (INT_FREQ * tempoMusic)));
                    } else {
                        tempoNotes = songData[ch][++cmdIndex[ch]] << 1;
                        vx = s2sf(2*tempoNotes)/120;
                        margin = s2sf(8*tempoNotes)/120;
                    }
                    break;
                }
                cmdIndex[ch]++;
            }
            if (songData[ch][cmdIndex[ch]] != REST) {
                if (ch < NUM_OF_SOUNDCHS) {
                    reg16(ChOutput[ch])= RLFN_RESET | FreqG[ songData[ch][cmdIndex[ch]] ] | (ch == CH3) * RLFN_TIMED;

            // if (songData[ch][cmdIndex[ch]] == REST) {
            //     reg16(DMGCNT) &= ~(0x1100 << ch);
            // }
            // else {
            //     if (ch < NUM_OF_SOUNDCHS) {
            //         reg16(DMGCNT) |= 0x1100 << ch;
            //         reg16(DMG1RLF + (ch<<3)) = RLFN_RESET | FreqG[ songData[ch][cmdIndex[ch]] ];

                }
                else {
                    gnote[dead_head].x = s2sf(256);
                    notes_sum++;
                    switch(songData[ch][cmdIndex[ch]]) {
                    case 0x18:
                        gnote[dead_head].y = NZ_POSY_UP;
                        gnote[dead_head].gnote_icon = CHNUM_UP;
                        break;

                    case 0x19:
                        gnote[dead_head].y = NZ_POSY_DOWN;
                        gnote[dead_head].gnote_icon = CHNUM_DW;
                        break;

                    case 0x1b:
                        gnote[dead_head].y = NZ_POSY_LEFT;
                        gnote[dead_head].gnote_icon = CHNUM_LE;
                        break;

                    case 0x1a:
                        gnote[dead_head].y = NZ_POSY_RIGHT;
                        gnote[dead_head].gnote_icon = CHNUM_RI;
                        break;

                    case 'A':
                        gnote[dead_head].y = NZ_POSY_A;
                        gnote[dead_head].gnote_icon = CHNUM_A;
                        break;

                    case 'B':
                        gnote[dead_head].y = NZ_POSY_B;
                        gnote[dead_head].gnote_icon = CHNUM_B;
                        break;
                    }
                    ObjAlloc(dead_head, OBJSIZEINDEX_0808, BGOBJ_PRIOR1, chnum[gnote[dead_head].gnote_icon]);
                    ObjPut(dead_head, 256, gnote[dead_head].y);
                    ObjShowHide(dead_head, OBJ_ON);
                    dead_head++;
                    dead_head &= NOTES_MAXIDX;
                }
            }

            hword gate_time = FOURBEAT / mnoteLen[ch];
            if (singleDot == 1 || (!singleLen && pplenDot[ch] == 1))
                gate_time += (gate_time >> 1);
            else if (singleDot == 3 || (!singleLen && pplenDot[ch] == 3))
                gate_time += (gate_time >> 1) + (gate_time >> 2);
            nextTick[ch] += gate_time;

            if (singleLen) {
                mnoteLen[ch] = singleLen;
            }

            cmdIndex[ch]++;
        }
    }
    (*tickCount)++;
}
