#include "meta.h"

int *const chnum = (int *) 0x03000010;

// Current game state
volatile short gameState = GS_SPLASH;

void SetGameState(short new_state) { gameState = new_state; }

void IrqHandler(void) {
    reg16(IME) = IME_OFF;   /// Prevent multiplex IRQ
    int flag = reg16(IF);   /// Get IRQ flag

    if (flag & IF_TM2) {
        DmgStep();
    }
    if (flag & IF_DMA1) {
        wavecnt -= 16;      /// DMA Xfer done for 16 bytes
        if (wavecnt <= 0) {	/// Xfer completed?
            reg16(TM0CTRL) = 0;
            reg16(DMA1CNT) = 0;
        }
    }

    reg16(IF) = flag;       /// Acknowkedge IRQ
    reg16(IME) = IME_ON;    /// Resume IRQ
}

/// Entry point
int main(void)
{
// ---------- Int_Handler ---------- //
    reg32(IRQ_VECTOR) = (int) IrqHandler;   // Register handler to the hook

// --- Initialization --- //
    BgInit();
    DSInit();
    DmgInit();
    ObjInit(chrPal);

    for (short i=0; i < NOTES_TOTAL_ICONS; i++)
        chnum[i] = ObjLoadImage(
            (OBJIMG *) chrTiles + (OBJDATASIZE_0808 * i),
            OBJDATASIZE_0808);

// ---------- Intr. Enablement ---------- //
    reg16(IE) = IF_DMA1 | IF_TM2;
    reg16(IME) = IME_ON;

    while(1) {
        KeyStateUpdate();

        while(reg16(LCDSTAT) ^ LCDSTAT_VBMASK);  /// Wait until V-blank starts
        (IWRAM_FRAMEC)++;
        TitleStep();
        MenuStep();
        SheetStep();
        while(reg16(LCDSTAT) & LCDSTAT_VBMASK);  /// Wait until V-blank ends
    }
    return 0;
}
