#include "logic.h"
#include "tiles.h"
#include <stdlib.h>

Vector2 IndexToVec(short mIndex) {
  return (Vector2){.x = mIndex % TILE_COUNT_WIDTH * TILE_SIZE,
                   .y = mIndex / TILE_COUNT_HEIGHT * TILE_SIZE};
}

short Distance(short mX, short mY) {
  short xRow = mX / TILE_COUNT_HEIGHT;
  short yRow = mY / TILE_COUNT_HEIGHT;
  if (xRow == yRow) {
    return abs(mX - mY);
  }
  return abs(mX % TILE_COUNT_WIDTH - mY % TILE_COUNT_WIDTH) + abs(xRow - yRow);
}

short IndexUpBy(short mIndex, short by) {
  return mIndex >= TILE_COUNT_WIDTH * by ? mIndex - TILE_COUNT_HEIGHT * by
                                         : OUTSIDE_MAP;
}

short IndexDownBy(short mIndex, short by) {
  return mIndex + TILE_COUNT_HEIGHT * by < MAP_SIZE
             ? mIndex + TILE_COUNT_HEIGHT * by
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
