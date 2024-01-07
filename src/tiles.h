#ifndef _TILESH_
#define _TILESH_
#include <raylib.h>

extern const short TILE_SIZE;

extern Texture2D TILE_MAP;

extern Rectangle TILE_QUESTION_MARK;
extern Rectangle TILE_CURSOR;
extern Rectangle TILE_HIGHLIGHT_Y;
extern Rectangle TILE_HIGHLIGHT_R;
extern Rectangle TILE_HAND;
extern Rectangle TILE_HALF_HEART;

extern Rectangle TILE_GRASS_PLAIN;
extern Rectangle TILE_GRASS;
extern Rectangle TILE_GRASS_FLOWER;

extern Rectangle TILE_GRAY_TANK;
extern Rectangle TILE_GRAY_GRUNT;

extern Rectangle TILE_BLUE_TANK;
extern Rectangle TILE_BLUE_GRUNT;

extern Texture2D CONTROLLER_TILE_MAP;
extern Rectangle TILE_CONTROLLER;

extern void InitTiles();

#endif