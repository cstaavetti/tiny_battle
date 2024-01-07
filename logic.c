#include "logic.h"
#include "input.h"
#include "structs.h"
#include "util.h"
#include <complex.h>
#include <stdbool.h>
#include <stdlib.h>

#define UNIT_SPLIT MAP_SIZE / 2

short Targets[DIR_COUNT];

short Cursor;
short SelIndex;
short GameState;
bool CPUEnabled;

Surface map[MAP_SIZE];
Unit units[MAP_SIZE];

short playerUnitCount;
short cpuUnitCount;

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

void updateGameState(short state) {
  GameState = state;
  if (GameState == STATE_IDLE) {
    CPUEnabled = !CPUEnabled;
  }
}

void generateMap() {
  // TEMP
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
}

void InitGame() {
  Cursor = 20;
  SelIndex = -1;
  playerUnitCount = 0;
  cpuUnitCount = UNIT_SPLIT - 1;
  updateGameState(STATE_IDLE);
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

void clearSelection() { SelIndex = -1; }

bool tileSelected() { return SelIndex > -1; }

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

short checkForTarget(short mIndex, short team, short dir) {
  if (isInPlayingArea(mIndex)) {
    if (hasEnemyUnit(mIndex, team)) {
      Targets[dir] = mIndex;
      return 1;
    } else if (hasFriendlyUnit(mIndex, team)) {
      return 2;
    }
  }
  return 0;
}

bool findTargets(short mIndex) {
  zeroTargets();
  bool found = false;
  short type = GetUnitType(mIndex);
  short range = GetUnitFireRange(type);
  short team = GetUnitTeam(mIndex);

  for (short i = 1; i <= range; i++) {
    short check = checkForTarget(IndexUpBy(mIndex, i), team, DIR_U);
    if (check == 1) {
      found = true;
    }
    if (check > 0) {
      break;
    }
  }
  for (short i = 1; i <= range; i++) {
    short check = checkForTarget(IndexRightBy(mIndex, i), team, DIR_R);
    if (check == 1) {
      found = true;
    }
    if (check > 0) {
      break;
    }
  }
  for (short i = 1; i <= range; i++) {
    short check = checkForTarget(IndexDownBy(mIndex, i), team, DIR_D);
    if (check == 1) {
      found = true;
    }
    if (check > 0) {
      break;
    }
  }
  for (short i = 1; i <= range; i++) {
    short check = checkForTarget(IndexLeftBy(mIndex, i), team, DIR_L);
    if (check == 1) {
      found = true;
    }
    if (check > 0) {
      break;
    }
  }
  if (CanFireDiagonal(type)) {
    for (short i = 1; i <= range; i++) {
      short check =
          checkForTarget(IndexLeftBy(IndexUpBy(mIndex, i), i), team, DIR_UL);
      if (check == 1) {
        found = true;
      }
      if (check > 0) {
        break;
      }
    }
    for (short i = 1; i <= range; i++) {
      short check =
          checkForTarget(IndexRightBy(IndexUpBy(mIndex, i), i), team, DIR_UR);
      if (check == 1) {
        found = true;
      }
      if (check > 0) {
        break;
      }
    }
    for (short i = 1; i <= range; i++) {
      short check =
          checkForTarget(IndexRightBy(IndexDownBy(mIndex, i), i), team, DIR_DR);
      if (check == 1) {
        found = true;
      }
      if (check > 0) {
        break;
      }
    }
    for (short i = 1; i <= range; i++) {
      short check =
          checkForTarget(IndexLeftBy(IndexDownBy(mIndex, i), i), team, DIR_DL);
      if (check == 1) {
        found = true;
      }
      if (check > 0) {
        break;
      }
    }
  }
  return found;
}

void playerControl() {
  short nCursor = OUTSIDE_MAP;
  if (GameState == STATE_FIRE) {
    if (TouchIsKeyDown(KEY_UP) && targetInDir(DIR_U)) {
      nCursor = Targets[DIR_U];
    }
    if (TouchIsKeyDown(KEY_DOWN) && targetInDir(DIR_D)) {
      nCursor = Targets[DIR_D];
    }
    if (TouchIsKeyDown(KEY_LEFT) && targetInDir(DIR_L)) {
      nCursor = Targets[DIR_L];
    }
    if (TouchIsKeyDown(KEY_RIGHT) && targetInDir(DIR_R)) {
      nCursor = Targets[DIR_R];
    }
    if (TouchIsKeyDown(KEY_UP) && TouchIsKeyDown(KEY_RIGHT) &&
        targetInDir(DIR_UR)) {
      nCursor = Targets[DIR_UR];
    }
    if (TouchIsKeyDown(KEY_UP) && TouchIsKeyDown(KEY_LEFT) &&
        targetInDir(DIR_UL)) {
      nCursor = Targets[DIR_UL];
    }
    if (TouchIsKeyDown(KEY_DOWN) && TouchIsKeyDown(KEY_RIGHT) &&
        targetInDir(DIR_DR)) {
      nCursor = Targets[DIR_DR];
    }
    if (TouchIsKeyDown(KEY_DOWN) && TouchIsKeyDown(KEY_LEFT) &&
        targetInDir(DIR_DL)) {
      nCursor = Targets[DIR_DL];
    }
  } else {
    if (TouchIsKeyPressed(KEY_LEFT)) {
      nCursor = IndexLeft(Cursor);
    } else if (TouchIsKeyPressed(KEY_RIGHT)) {
      nCursor = IndexRight(Cursor);
    }
    if (TouchIsKeyPressed(KEY_UP)) {
      nCursor = IndexUp(Cursor);
    } else if (TouchIsKeyPressed(KEY_DOWN)) {
      nCursor = IndexDown(Cursor);
    }
  }
  Cursor = nCursor == OUTSIDE_MAP ? Cursor : nCursor;
  if (TouchIsKeyPressed(KEY_X)) {
    if (GameState == STATE_IDLE) {
      // Try to select a unit
      if (HasUnit(Cursor) && GetUnitTeam(Cursor) == PLAYER_TEAM) {
        SelIndex = Cursor;
        genWalkableArea(Cursor);
        updateGameState(STATE_MOVE);
      }
    } else if (GameState == STATE_MOVE) {
      if (areaIsPassable(Cursor)) {
        moveUnitOnMap(SelIndex, Cursor);
        SelIndex = Cursor;
        if (findTargets(SelIndex)) {
          updateGameState(STATE_FIRE);
        } else {
          clearSelection();
          updateGameState(STATE_IDLE);
        }
      } else {
        clearSelection();
        updateGameState(STATE_IDLE);
      }
    } else if (GameState == STATE_FIRE) {
      if (Cursor != SelIndex) {
        damageIndex(Cursor, GetUnitDamage(GetUnitType(SelIndex)));
      }
      Cursor = SelIndex;
      clearSelection();
      updateGameState(STATE_IDLE);
    }
  }
  if (TouchIsKeyPressed(KEY_Z)) {
    if (tileSelected()) {
      clearSelection();
      updateGameState(STATE_IDLE);
    }
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
  for (short i = range; i > 0; i--) {
    if (areaIsPassableSafe(IndexUpBy(mTarget, i))) {
      return IndexUpBy(mTarget, i);
    }
    if (areaIsPassableSafe(IndexRightBy(mTarget, i))) {
      return IndexRightBy(mTarget, i);
    }
    if (areaIsPassableSafe(IndexDownBy(mTarget, i))) {
      return IndexDownBy(mTarget, i);
    }
    if (areaIsPassableSafe(IndexLeftBy(mTarget, i))) {
      return IndexLeftBy(mTarget, i);
    }
    if (CanFireDiagonal(type)) {
      if (areaIsPassableSafe(IndexLeftBy(IndexUpBy(mTarget, i), i))) {
        return IndexLeftBy(IndexUpBy(mTarget, i), i);
      }
      if (areaIsPassableSafe(IndexRightBy(IndexUpBy(mTarget, i), i))) {
        return IndexRightBy(IndexUpBy(mTarget, i), i);
      }
      if (areaIsPassableSafe(IndexRightBy(IndexDownBy(mTarget, i), i))) {
        return IndexRightBy(IndexDownBy(mTarget, i), i);
      }
      if (areaIsPassableSafe(IndexLeftBy(IndexDownBy(mTarget, i), i))) {
        return IndexLeftBy(IndexDownBy(mTarget, i), i);
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
#if defined(PLATFORM_WEB)
  UpdateTouchMap();
#endif
  if (CPUEnabled) {
    cpuControl();
    clearSelection();
    updateGameState(STATE_IDLE);
  } else {
    playerControl();
  }
}
