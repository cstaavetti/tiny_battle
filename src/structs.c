#include "structs.h"
#include "tiles.h"

short gameSWidth = 128;
short gameSHeight = 208;

Rectangle GetUnitRect(short type, short team) {
  switch (type) {
  case UNIT_TANK:
    switch (team) {
    case TEAM_GRAY:
      return TILE_GRAY_TANK;
    case TEAM_BLUE:
      return TILE_BLUE_TANK;
    default:
      return TILE_QUESTION_MARK;
    }
  case UNIT_GRUNT:
    switch (team) {
    case TEAM_GRAY:
      return TILE_GRAY_GRUNT;
    case TEAM_BLUE:
      return TILE_BLUE_GRUNT;
    default:
      return TILE_QUESTION_MARK;
    }
  default:
    return TILE_QUESTION_MARK;
  }
}

Rectangle GetSurfaceRect(short type) {
  switch (type) {
  case SURFACE_GRASS_PLAIN:
    return TILE_GRASS_PLAIN;
  case SURFACE_GRASS:
    return TILE_GRASS;
  case SURFACE_GRASS_FLOWER:
    return TILE_GRASS_FLOWER;
  case SURFACE_TEST_A:
    return TILE_HIGHLIGHT_R;
  case SURFACE_TEST_B:
    return TILE_HIGHLIGHT_Y;
  }

  return TILE_QUESTION_MARK;
}

short GetUnitSteps(short type) {
  switch (type) {
  case UNIT_TANK:
    return STEPS_TANK;
  case UNIT_GRUNT:
    return STEPS_GRUNT;
  }
  return 0;
}

short GetUnitFireRange(short type) {
  switch (type) {
  case UNIT_TANK:
    return RANGE_TANK;
  case UNIT_GRUNT:
    return RANGE_GRUNT;
  }
  return 0;
}

short GetUnitDamage(short type) {
  switch (type) {
  case UNIT_TANK:
    return DAMAGE_TANK;
  case UNIT_GRUNT:
    return DAMAGE_GRUNT;
  }
  return 0;
}

short GetUnitHealth(short type) {
  switch (type) {
  case UNIT_TANK:
    return HEALTH_TANK;
  case UNIT_GRUNT:
    return HEALTH_GRUNT;
  }
  return 0;
}

bool CanFireDiagonal(short type) {
  switch (type) {
  case UNIT_TANK:
    return false;
  case UNIT_GRUNT:
    return true;
  }
  return false;
}

Unit CreateTank(short index, short team) {
  return (Unit){.type = UNIT_TANK,
                .team = team,
                .health = HEALTH_TANK,
                .mapIndex = index};
}

Unit CreateGrunt(short index, short team) {
  return (Unit){.type = UNIT_GRUNT,
                .team = team,
                .health = HEALTH_GRUNT,
                .mapIndex = index};
}