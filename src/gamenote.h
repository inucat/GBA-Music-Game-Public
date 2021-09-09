#ifndef _NOTES_H_
#define _NOTES_H_

#define NOTES_MAXIDX 0x3f
#define NOTES_MAXNUM (NOTES_MAXIDX + 1)

#define NZ_POSY_UP       16
#define NZ_POSY_DOWN     64
#define NZ_POSY_LEFT     32
#define NZ_POSY_RIGHT    48
#define NZ_POSY_A        80
#define NZ_POSY_B        96

#define NOTES_TOTAL_ICONS 6

enum {
    CHNUM_UP,
    CHNUM_DW,
    CHNUM_LE,
    CHNUM_RI,
    CHNUM_A,
    CHNUM_B
};

typedef struct notes {
    sfix x;
    sfix vx;
    short y;
    char gnote_icon;
} GNOTES_t;

extern hword ok_count;
extern hword notes_sum;

extern byte     dead_head;
extern GNOTES_t  gnote[NOTES_MAXNUM];
extern hword    notes_sum;
extern sfix     margin;
extern sfix     vx;

void hitKey(void);
void notes_callend(void);
void NotesInit(void);
void NotesStep(void);
void notes_draw(void);
void notes_clearall(void);

#endif
