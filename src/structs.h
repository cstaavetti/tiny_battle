#ifndef _STRUCTSH_
#define _STRUCTSH_

#include <raylib.h>

#define UNIT_TANK 0
#define UNIT_GRUNT 1

#define TARGET_TYPE_COUNT 3 // includes damaged tank

#define TEAM_GRAY 0
#define TEAM_BLUE 1

#define CPU_TEAM TEAM_GRAY
#define PLAYER_TEAM TEAM_BLUE

#define HEALTH_TANK 2
#define HEALTH_GRUNT 1

#define STEPS_TANK 2
#define STEPS_GRUNT 1

#define RANGE_TANK 2
#define RANGE_GRUNT 1

#define DAMAGE_TANK 1
#define DAMAGE_GRUNT 1

#define SURFACE_GRASS_PLAIN 0
#define SURFACE_GRASS 1
#define SURFACE_GRASS_FLOWER 2
#define SURFACE_TEST_A 3
#define SURFACE_TEST_B 4

#define WALK_DENIED 0
#define WALK_ALLOWED 1
#define WALK_PASS 2
#define WALK_CURRENT 3

extern short gameSWidth;
extern short gameSHeight;

typedef struct Unit {
  short type;
  short health;
  short team;
  short mapIndex;
} Unit;

typedef struct Path {
  short type;
  short cost;
  short cameFrom;
} Path;

typedef struct Surface {
  short type;
  short unitIndex;
  Path walkable;
} Surface;

typedef struct TType {
  short unitType;
  bool lowHealth;
} TType;

extern Rectangle GetUnitRect(short type, short team);
extern Rectangle GetSurfaceRect(short type);
extern short GetUnitSteps(short type);
extern short GetUnitFireRange(short type);
extern short GetUnitDamage(short type);
extern short GetUnitHealth(short type);
extern bool CanFireDiagonal(short type);

extern Unit CreateTank(short index, short team);
extern Unit CreateGrunt(short index, short team);

#endif
