#include "logic.h"
#include "input.h"
#include "structs.h"
#include "util.h"
#include <complex.h>
#include <stdbool.h>
#include <stdlib.h>

#define UNIT_SPLIT MAP_SIZE / 2

short Targets[DIR_COUNT];

short GameState;
bool CPUEnabled;

Surface map[MAP_SIZE];
Unit units[MAP_SIZE];

short playerUnitCount;
short cpuUnitCount;
short selectedLocation;

void zeroWalkableArea() {
  for (short i = 0; i < MAP_SIZE; i++) {
    map[i].walkable = (Path){.type = WALK_DENIED, .cost = -1, .cameFrom = -1};
  }
}

void zeroTargets() {
  for (short i = 0; i < DIR_COUNT; i++) {
    Targets[i] = -1;
  }
}

void zeroUnits() {
  for (short i = 0; i < MAP_SIZE; i++) {
    units[i] = (Unit){.health = -1, .mapIndex = -1, .team = -1, .type = -1};
  }
}

void updateGameState(short state, short mIndex) {
  GameState = state;
  selectedLocation = mIndex;
  if (GameState == STATE_IDLE) {
    CPUEnabled = !CPUEnabled;
  }
}

void addTestShape() {
  map[1].type = SURFACE_TEST_A;
  map[2].type = SURFACE_TEST_A;
  map[3].type = SURFACE_TEST_A;

  map[TILE_COUNT_WIDTH].type = SURFACE_TEST_B;
  map[TILE_COUNT_WIDTH * 2].type = SURFACE_TEST_B;
  map[TILE_COUNT_WIDTH * 3].type = SURFACE_TEST_B;
  map[TILE_COUNT_WIDTH * 4].type = SURFACE_TEST_B;
  map[TILE_COUNT_WIDTH * 5].type = SURFACE_TEST_B;
  map[TILE_COUNT_WIDTH * 6].type = SURFACE_TEST_B;
}

void generateMap() {
  // TEMP
  // for generating completely random maps for testing
  for (short i = 0; i < MAP_SIZE; i++) {
    short surfaceType = GetRandomValue(0, 2);
    short team = GetRandomValue(0, 12);
    short unitType = GetRandomValue(0, 1);
    Unit unit;
    if (unitType == UNIT_TANK) {
      unit = CreateTank(i, team);
    } else if (unitType == UNIT_GRUNT) {
      unit = CreateGrunt(i, team);
    }
    short unitIndex = -1;
    if (team == PLAYER_TEAM && playerUnitCount < UNIT_SPLIT) {
      units[playerUnitCount] = unit;
      unitIndex = playerUnitCount;
      playerUnitCount++;
    } else if (team == CPU_TEAM && cpuUnitCount < MAP_SIZE) {
      units[cpuUnitCount] = unit;
      unitIndex = cpuUnitCount;
      cpuUnitCount++;
    }
    map[i] = (Surface){.type = surfaceType, .unitIndex = unitIndex};
  }
  // addTestShape();
}

void InitGame() {
  playerUnitCount = 0;
  cpuUnitCount = UNIT_SPLIT - 1;
  updateGameState(STATE_IDLE, OUTSIDE_MAP);
  CPUEnabled = false;
  zeroWalkableArea();
  zeroTargets();
  zeroUnits();
  generateMap();
}

short GetSurfaceType(short mIndex) { return map[mIndex].type; }

bool HasUnit(short mIndex) {
  return map[mIndex].unitIndex > -1; // could check health
}

bool UnitDamaged(short mIndex) {
  if (units[map[mIndex].unitIndex].health <
      GetUnitHealth(units[map[mIndex].unitIndex].type)) {
    return true;
  }
  return false;
}

void damageIndex(short mIndex, short damage) {
  // could check if theres a unit :D
  units[map[mIndex].unitIndex].health -= damage;
  if (units[map[mIndex].unitIndex].health < 1) {
    units[map[mIndex].unitIndex].mapIndex = -1;
    map[mIndex].unitIndex = -1;
  }
}

bool targetInDir(short dir) { return Targets[dir] >= 0; }

bool locationInTargets(short mIndex) {
  for (short i = 0; i < DIR_COUNT; i++) {
    if (mIndex == Targets[i]) {
      return true;
    }
  }
  return false;
}

short GetUnitType(short mIndex) { return units[map[mIndex].unitIndex].type; }

short GetUnitTeam(short mIndex) { return units[map[mIndex].unitIndex].team; }

short getCurrentTeam() { return CPUEnabled ? CPU_TEAM : PLAYER_TEAM; }

bool hasFriendlyUnit(short mIndex, short team) {
  return HasUnit(mIndex) && GetUnitTeam(mIndex) == team;
}

bool hasEnemyUnit(short mIndex, short team) {
  return HasUnit(mIndex) && GetUnitTeam(mIndex) != team;
}

bool isInPlayingArea(short mIndex) { return mIndex > -1 && mIndex < MAP_SIZE; }

bool WalkableTypeIs(short index, short type) {
  return map[index].walkable.type == type;
}

bool areaIsPassable(short index) {
  return map[index].walkable.type != WALK_DENIED;
}

bool areaIsPassableSafe(short mIndex) {
  return isInPlayingArea(mIndex) && areaIsPassable(mIndex);
}

void genPath(short mIndex, short from, short steps, short cost) {
  if (steps == cost - 1 || mIndex == OUTSIDE_MAP || !isInPlayingArea(mIndex)) {
    return;
  }
  if (HasUnit(mIndex)) {
    if (hasFriendlyUnit(mIndex, getCurrentTeam())) {
      map[mIndex].walkable =
          (Path){.type = WALK_PASS, .cost = cost, .cameFrom = from};
    } else {
      return;
    }
  }
  if (map[mIndex].walkable.cost < 0 || map[mIndex].walkable.cost < cost) {
    short type = WALK_ALLOWED;
    if (map[mIndex].walkable.type == WALK_PASS) {
      type = WALK_PASS;
    }
    map[mIndex].walkable = (Path){.type = type, .cost = cost, .cameFrom = from};
  }
  genPath(IndexUp(mIndex), mIndex, steps, cost + 1);
  genPath(IndexDown(mIndex), mIndex, steps, cost + 1);
  genPath(IndexLeft(mIndex), mIndex, steps, cost + 1);
  genPath(IndexRight(mIndex), mIndex, steps, cost + 1);
}

void genWalkableArea(short mIndex) {
  if (!HasUnit(mIndex)) {
    return;
  }
  zeroWalkableArea();
  genPath(mIndex, mIndex, GetUnitSteps(GetUnitType(mIndex)), 0);
  map[mIndex].walkable =
      (Path){.type = WALK_CURRENT, .cost = 0, .cameFrom = mIndex};
}

bool moveUnitOnMap(short mX, short mY) {
  // returns if it moved
  if (mX == mY) {
    return false;
  }
  if (!HasUnit(mY)) {
    map[mY].unitIndex = map[mX].unitIndex;
    units[map[mX].unitIndex].mapIndex = mY;
    map[mX].unitIndex = -1;
    return true;
  }
  short cameFrom = map[mY].walkable.cameFrom;
  if (map[cameFrom].walkable.type == WALK_ALLOWED ||
      map[cameFrom].walkable.type == WALK_CURRENT) {
    if (cameFrom != mX) {
      map[cameFrom].unitIndex = map[mY].unitIndex;
      units[map[mY].unitIndex].mapIndex = cameFrom;
      map[mY].unitIndex = map[mX].unitIndex;
      units[map[mX].unitIndex].mapIndex = mY;
      map[mX].unitIndex = -1;
    } else {
      short tU = map[mY].unitIndex;
      units[tU].mapIndex = units[map[mX].unitIndex].mapIndex;
      map[mY].unitIndex = map[mX].unitIndex;
      units[map[mX].unitIndex].mapIndex = mY;
      map[mX].unitIndex = tU;
    }
  }
  return true;
}

bool checkForTarget(short mIndex, short team, short dir) {
  if (isInPlayingArea(mIndex)) {
    if (hasEnemyUnit(mIndex, team)) {
      Targets[dir] = mIndex;
      return true;
    } else if (hasFriendlyUnit(mIndex, team)) {
      return true;
    }
  }
  return false;
}

bool findTargets(short mIndex) {
  zeroTargets();
  short type = GetUnitType(mIndex);
  short range = GetUnitFireRange(type);
  short team = GetUnitTeam(mIndex);
  short dirCount = CanFireDiagonal(type) ? DIR_COUNT : DIR_COUNT / 2;
  bool result = false;
  for (short dir = 0; dir < dirCount; dir++) {
    for (short i = 1; i <= range; i++) {
      if (checkForTarget(IndexToDirBy(dir, mIndex, i), team, dir)) {
        result = true;
        break;
      }
    }
  }
  return result;
}

void playerControl() {
  short inputIndex = ClickTouchIndex();
  if (inputIndex < 0) {
    return;
  }

  if (GameState == STATE_IDLE) {
    // Try to select a unit
    if (HasUnit(inputIndex) && GetUnitTeam(inputIndex) == PLAYER_TEAM) {
      genWalkableArea(inputIndex);
      updateGameState(STATE_MOVE, inputIndex);
    }
  } else if (GameState == STATE_MOVE) {
    if (areaIsPassable(inputIndex)) {
      moveUnitOnMap(selectedLocation, inputIndex);
      if (findTargets(inputIndex)) {
        updateGameState(STATE_FIRE, inputIndex);
      } else {
        updateGameState(STATE_IDLE, OUTSIDE_MAP);
      }
    } else {
      updateGameState(STATE_IDLE, OUTSIDE_MAP);
    }
  } else if (GameState == STATE_FIRE) {
    if (locationInTargets(inputIndex)) {
      damageIndex(inputIndex, GetUnitDamage(GetUnitType(selectedLocation)));
    }
    updateGameState(STATE_IDLE, OUTSIDE_MAP);
  }
}

short canFireAt(short mTarget, short type) {
  if (areaIsPassable(mTarget)) {
    return true;
  }
  // Requires walkable area to be precalculated for the firing unit.
  // Relies on the idea that the pattern of where a unit can fire is identical
  // to where it can fire from.
  short range = GetUnitFireRange(type);
  short dirCount = CanFireDiagonal(type) ? DIR_COUNT : DIR_COUNT / 2;
  for (short i = range; i > 0; i--) {
    for (short dir = 0; dir < dirCount; dir++) {
      if (areaIsPassableSafe(IndexToDirBy(dir, mTarget, i))) {
        return IndexToDirBy(dir, mTarget, i);
      }
    }
  }
  return -1;
}

short cpuUnitAlreadyInPosition(short mIndex) {
  short directions[4] = {IndexUp(mIndex), IndexDown(mIndex), IndexLeft(mIndex),
                         IndexRight(mIndex)};
  for (short i = 0; i < 4; i++) {
    if (isInPlayingArea(directions[i]) &&
        hasFriendlyUnit(directions[i], CPU_TEAM)) {
      return directions[i];
    }
  }
  return -1;
}

void cpuControl() {
  TType trgts[TARGET_TYPE_COUNT] = {
      (TType){.unitType = UNIT_TANK, .lowHealth = true},
      (TType){.unitType = UNIT_TANK, .lowHealth = false},
      (TType){.unitType = UNIT_GRUNT, .lowHealth = false},
  };
  for (short ttIndex = 0; ttIndex < TARGET_TYPE_COUNT; ttIndex++) {
    for (short unitIndexP = 0; unitIndexP < playerUnitCount; unitIndexP++) {
      short mIndexP = units[unitIndexP].mapIndex;
      if (mIndexP == -1) {
        continue;
      }
      if (GetUnitType(mIndexP) != trgts[ttIndex].unitType ||
          UnitDamaged(mIndexP) != trgts[ttIndex].lowHealth) {
        continue;
      }

      short potentialReadyCPU = cpuUnitAlreadyInPosition(mIndexP);
      if (potentialReadyCPU > -1) {
        damageIndex(mIndexP, GetUnitDamage(GetUnitType(potentialReadyCPU)));
        return;
      }

      for (short unitIndexCPU = UNIT_SPLIT - 1; unitIndexCPU < cpuUnitCount;
           unitIndexCPU++) {
        short mIndexCPU = units[unitIndexCPU].mapIndex;
        if (mIndexCPU == -1) {
          continue;
        }
        genWalkableArea(mIndexCPU);
        short type = GetUnitType(mIndexCPU);
        if (Distance(mIndexCPU, mIndexP) <=
            GetUnitSteps(type) + GetUnitFireRange(type) +
                1) { // magic +1 to fix grunts firing diagonally
          short ff = canFireAt(mIndexP, type);
          if (ff > -1) {
            bool moved = moveUnitOnMap(mIndexCPU, ff);
            if (findTargets(ff) && locationInTargets(mIndexP)) {
              damageIndex(mIndexP, GetUnitDamage(type));
            } else if (!moved) {
              continue;
            }
            return;
          }
        }
      }
    }
  }
  short closestTankIndex = -1;
  short closestTankDistance = 100;
  short closestGruntIndex = -1;
  short closestGruntDistance = 100;
  // nobody to attack, so try moving closer
  for (short ttIndex = 0; ttIndex < TARGET_TYPE_COUNT; ttIndex++) {
    for (short unitIndexP = 0; unitIndexP < playerUnitCount; unitIndexP++) {
      short mIndexP = units[unitIndexP].mapIndex;
      if (mIndexP == -1) {
        continue;
      }
      if (GetUnitType(mIndexP) != trgts[ttIndex].unitType ||
          UnitDamaged(mIndexP) != trgts[ttIndex].lowHealth) {
        continue;
      }

      for (short unitIndexCPU = UNIT_SPLIT - 1; unitIndexCPU < cpuUnitCount;
           unitIndexCPU++) {
        short mIndexCPU = units[unitIndexCPU].mapIndex;
        if (mIndexCPU == -1) {
          continue;
        }
        short type = GetUnitType(mIndexCPU);
        short dist = Distance(mIndexCPU, mIndexP);
        if (type == UNIT_TANK && dist < closestTankDistance) {
          closestTankDistance = dist;
          closestTankIndex = mIndexCPU;
        } else if (type == UNIT_GRUNT && dist < closestGruntDistance) {
          closestGruntDistance = dist;
          closestGruntIndex = mIndexCPU;
        }
      }
      short selected = closestTankIndex;
      short finalType = UNIT_TANK;
      if (closestGruntDistance < closestTankDistance) {
        selected = closestGruntIndex;
        finalType = UNIT_GRUNT;
      }
      if (selected == -1) {
        return;
      }
      genWalkableArea(selected);
      Vector2 cpuV = IndexToVec(selected);
      Vector2 playerV = IndexToVec(mIndexP);
      short hori = playerV.x - cpuV.x;
      short vert = playerV.y - cpuV.y;
      short steps = GetUnitSteps(finalType);
      if (abs(hori) > abs(vert)) {
        if (hori > 0 && areaIsPassableSafe(IndexRightBy(selected, steps))) {
          if (moveUnitOnMap(selected, IndexRightBy(selected, steps))) {
            return;
          }

        } else if (areaIsPassableSafe(IndexLeftBy(selected, steps))) {
          if (moveUnitOnMap(selected, IndexLeftBy(selected, steps))) {
            return;
          }
        }
      }
      if (vert > 0 && areaIsPassableSafe(IndexDownBy(selected, steps))) {
        if (moveUnitOnMap(selected, IndexDownBy(selected, steps))) {
          return;
        }
      } else if (areaIsPassableSafe(IndexUpBy(selected, steps))) {
        if (moveUnitOnMap(selected, IndexUpBy(selected, steps))) {
          return;
        }
      }
    }
  }
}

void RunGame() {
  if (CPUEnabled) {
    cpuControl();
    updateGameState(STATE_IDLE, OUTSIDE_MAP);
  } else {
    playerControl();
  }
}
