#include "meta.h"

#define KEY_POSX 2048

GNOTES_t gnote[NOTES_MAXNUM];

volatile byte living_head = 0;
byte dead_head = 0;
hword ok_count = 0;
hword notes_sum = 0;
byte end_called = 0;

sfix vx = s2sf(2);
sfix margin = s2sf(8);

static byte hitflg = 0;

void hitKey() {
    for (short i = (living_head & NOTES_MAXIDX), nx=gnote[i].x; i != dead_head; i = (i+1) & NOTES_MAXIDX, nx=gnote[i].x) {
        if (nx > margin + KEY_POSX) return;     // next gnote has not reached the detection area
        else if (nx <= margin + KEY_POSX && nx >= KEY_POSX - margin) {  // one in the detection area
            char nzchar = gnote[i].gnote_icon;
            if ( ((keyCurrent ^ keyPrevious) & KEY_DU && nzchar == CHNUM_UP) ||
            ((keyCurrent ^ keyPrevious) & KEY_DD && nzchar == CHNUM_DW) ||
            ((keyCurrent ^ keyPrevious) & KEY_DL && nzchar == CHNUM_LE) ||
            ((keyCurrent ^ keyPrevious) & KEY_DR && nzchar == CHNUM_RI) ||
            ((keyCurrent ^ keyPrevious) & KEY_A && nzchar == CHNUM_A) ||
            ((keyCurrent ^ keyPrevious) & KEY_B && nzchar == CHNUM_B) )
            {
                hitflg=12;
                ok_count++;
                living_head++;
                ObjShowHide(i, OBJ_OFF);
            }
        } else continue;
    }
}

void notes_callend() {
    end_called = 1;
}

void NotesInit() {
    hitflg=0;
    end_called=0;
    ok_count=0;
    notes_sum=0;
    living_head = dead_head = 0;
    vx = s2sf(2);
    margin = s2sf(8);
    for (byte i=0; i < NOTES_MAXNUM; i++) {
        gnote[i].x = 0;
        gnote[i].y = 0;
        gnote[i].vx = 0;
    }
}

void NotesStep() {
    for (byte i=living_head & NOTES_MAXIDX; i != dead_head; i = (i+1) & NOTES_MAXIDX) {
        gnote[i].x -= vx;
        if (gnote[i].x >= 0)    // gnote exists in the visible area
            ObjPut(i, sf2s(gnote[i].x), gnote[i].y);
        else {
            living_head++;
            ObjShowHide(i, OBJ_OFF);
        }
    }
    PutStr(0, 1, 14, "     ");
    if (hitflg) {
        PutStr(0, 1, 14, "Hit!");
        hitflg--;
    }
    living_head &= NOTES_MAXIDX;
    if(end_called && living_head == dead_head) {
        end_called = 0;
        SetGameState(GS_RESULT);
    }
}

void notes_clearall() {
    for (short i = 0; i < NOTES_MAXNUM; i++) {
        ObjShowHide(i, OBJ_OFF);
    }
}
