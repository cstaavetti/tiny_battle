#include "util.h"
#include "logic.h"
#include "tiles.h"
#include <stdlib.h>

Vector2 IndexToVec(short mIndex) {
  return (Vector2){.x = mIndex % TILE_COUNT_WIDTH * TILE_SIZE,
                   .y = mIndex / TILE_COUNT_WIDTH * TILE_SIZE};
}

short VecToIndex(Vector2 vec) {
  short x = vec.x / TILE_SIZE;
  short y = vec.y / TILE_SIZE;
  return IndexDownBy(IndexRightBy(0, x), y);
}

short Distance(short mX, short mY) {
  short xRow = mX / TILE_COUNT_WIDTH;
  short yRow = mY / TILE_COUNT_WIDTH;
  if (xRow == yRow) {
    return abs(mX - mY);
  }
  return abs(mX % TILE_COUNT_WIDTH - mY % TILE_COUNT_WIDTH) + abs(xRow - yRow);
}

short IndexUpBy(short mIndex, short by) {
  return mIndex >= TILE_COUNT_WIDTH * by ? mIndex - TILE_COUNT_WIDTH * by
                                         : OUTSIDE_MAP;
}

short IndexDownBy(short mIndex, short by) {
  return mIndex + TILE_COUNT_WIDTH * by < MAP_SIZE
             ? mIndex + TILE_COUNT_WIDTH * by
             : OUTSIDE_MAP;
}

short IndexLeftBy(short mIndex, short by) {
  return by <= mIndex % TILE_COUNT_WIDTH ? mIndex - by : OUTSIDE_MAP;
}

short IndexRightBy(short mIndex, short by) {
  return by < (TILE_COUNT_WIDTH - (mIndex % TILE_COUNT_WIDTH)) ? mIndex + by
                                                               : OUTSIDE_MAP;
}

short IndexUp(short mIndex) { return IndexUpBy(mIndex, 1); }

short IndexDown(short mIndex) { return IndexDownBy(mIndex, 1); }

short IndexLeft(short mIndex) { return IndexLeftBy(mIndex, 1); }

short IndexRight(short mIndex) { return IndexRightBy(mIndex, 1); }

short IndexToDirBy(short dir, short mIndex, short by) {
  switch (dir) {
  case DIR_U:
    return IndexUpBy(mIndex, by);
  case DIR_D:
    return IndexDownBy(mIndex, by);
  case DIR_L:
    return IndexLeftBy(mIndex, by);
  case DIR_R:
    return IndexRightBy(mIndex, by);
  case DIR_UL:
    return IndexLeftBy(IndexUpBy(mIndex, by), by);
  case DIR_UR:
    return IndexRightBy(IndexUpBy(mIndex, by), by);
  case DIR_DL:
    return IndexLeftBy(IndexDownBy(mIndex, by), by);
  case DIR_DR:
    return IndexRightBy(IndexDownBy(mIndex, by), by);
  }
  return -1;
}