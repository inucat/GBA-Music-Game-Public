#include "meta.h"

// ----- Defined in sfxwaves.c ----- //

extern const int* const wavesizes[];
extern const char* const waves[];

// ----- Used in main.c for IrqHandler ----- //

volatile int wavecnt = 0;

void DSInit(void)
{
    reg16(SNDMASTER) = SNDMASTER_ON;
    reg16(SNDCNT) = SNDCNT_SARIGHT | SNDCNT_SAFULL | SNDCNT_SALEFT | SNDCNT_SARIGHT;
}

/// Play Direct Sound
/// @param sndidx Sound wavedata index in waves[] (sfxwaves.c)
void DSPlay(short sndidx)
{
    /// Reset sampling rate timer and DMA Xfer controller
    reg16(DMA1CNT) = reg16(TM0CTRL) = 0;
    reg16(TM0COUNT) = 64776;	///  = 65536 - 16777216 / 22050
    reg16(TM0CTRL) = TM_START | TM_PRESC0001;

    /// Init DMA Xfer controller
    reg32(DMA1SAD) = (int) waves[sndidx];   /// Source addr.
    reg32(DMA1DAD) = SNDFIFO_A;             /// Destination addr.
    reg16(DMA1CNT) = DMACNT_ENABLE | DMACNT_IRQENB | DMACNT_STAFIFO | \
        DMACNT_SIZE32 | DMACNT_REPEAT | DMACNT_SADINC | DMACNT_DADFIX;
    wavecnt = *wavesizes[sndidx];
}
