
//{{BLOCK(maps)

//======================================================================
//
//	maps, 512x768@8, 
//	Transparent color : 00,00,00
//	+ palette 256 entries, not compressed
//	+ 15 tiles (t|f reduced) not compressed
//	+ regular map (in SBBs), not compressed, 64x96 
//	Total size: 512 + 960 + 12288 = 13760
//
//	Time-stamp: 2020-09-23, 22:11:41
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.9
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_MAPS_H
#define GRIT_MAPS_H

#define mapsTilesLen 960
extern const unsigned int mapsTiles[240];

#define mapsMapLen 12288
extern const unsigned short mapsMap[6144];

#define mapsPalLen 512
extern const unsigned short mapsPal[256];

#endif // GRIT_MAPS_H

//}}BLOCK(maps)
