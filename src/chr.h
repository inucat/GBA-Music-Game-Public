
//{{BLOCK(chr)

//======================================================================
//
//	chr, 128x64@8, 
//	+ palette 256 entries, not compressed
//	+ 128 tiles not compressed
//	Total size: 512 + 8192 = 8704
//
//	Time-stamp: 2019-07-09, 17:15:18
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.15
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_CHR_H
#define GRIT_CHR_H

#define chrTilesLen 8192
extern const unsigned char chrTiles[8192];

#define chrPalLen 512
extern const unsigned short chrPal[256];

#endif // GRIT_CHR_H

//}}BLOCK(chr)
