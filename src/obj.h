#ifndef _OBJ_H_
#define _OBJ_H_

#include "exgba.h"

extern int *const chnum;

/// Load palette, OAM zero-clear, enable OBJ
/// @param obj_palette Pointer to the palette data
void ObjInit(const hword *obj_palette);

int  ObjLoadImage(
    OBJIMG *ch,
    int size);

void ObjAlloc(
    char spnum,
    short shape,
    short priority,
    short chnum);

/// Show or hide OBJ.
/// @param spnum OBJ number
/// @param disp 0 to hide, non-0 to show
void ObjShowHide(
    char spnum,
    short disp);

#endif
