#ifndef _LOGICH_
#define _LOGICH_
#include "structs.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define CLAMP(x, min, max) (MAX(MIN(x, max), min))

#define TILE_COUNT_WIDTH 8
#define TILE_COUNT_HEIGHT 13
#define MAP_SIZE TILE_COUNT_WIDTH *TILE_COUNT_HEIGHT
#define OUTSIDE_MAP -1

#define STATE_IDLE 0
#define STATE_MOVE 1
#define STATE_FIRE 2

#define DIR_COUNT 8
#define DIR_U 0
#define DIR_D 1
#define DIR_L 2
#define DIR_R 3
#define DIR_UL 4
#define DIR_UR 5
#define DIR_DL 6
#define DIR_DR 7

extern short Targets[DIR_COUNT]; // up, down, left, right, upleft, upright,
                                 // downleft, downright

extern short GameState;
extern bool CPUEnabled;

extern void InitGame();
extern short GetSurfaceType(short index);
extern bool HasUnit(short index);
extern bool UnitDamaged(short index);
extern short GetUnitType(short index);
extern short GetUnitTeam(short index);
extern bool WalkableTypeIs(short index, short type);
extern void RunGame();

#endif
