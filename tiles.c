#include "tiles.h"
#include <raylib.h>

const short TILE_SIZE = 16;

Texture2D TILE_MAP;

Rectangle TILE_QUESTION_MARK;
Rectangle TILE_CURSOR;
Rectangle TILE_HIGHLIGHT_Y;
Rectangle TILE_HIGHLIGHT_R;
Rectangle TILE_HAND;
Rectangle TILE_HALF_HEART;

Rectangle TILE_GRASS_PLAIN;
Rectangle TILE_GRASS;
Rectangle TILE_GRASS_FLOWER;

Rectangle TILE_GRAY_TANK;
Rectangle TILE_GRAY_GRUNT;

Rectangle TILE_BLUE_TANK;
Rectangle TILE_BLUE_GRUNT;

Texture2D CONTROLLER_TILE_MAP;
Rectangle TILE_CONTROLLER;

void InitTiles() {
  TILE_MAP = LoadTexture("resources/tilemap_packed.png");
  CONTROLLER_TILE_MAP = LoadTexture("resources/controller.png");
  TILE_CONTROLLER = (Rectangle){0.0f, 0.0f, 128, 64};

  TILE_QUESTION_MARK = (Rectangle){10.0f * TILE_SIZE, 10.0f * TILE_SIZE,
                                   (float)TILE_SIZE, (float)TILE_SIZE};
  TILE_CURSOR = (Rectangle){7.0f * TILE_SIZE, 3.0f * TILE_SIZE,
                            (float)TILE_SIZE, (float)TILE_SIZE};
  TILE_HIGHLIGHT_Y = (Rectangle){7.0f * TILE_SIZE, 4.0f * TILE_SIZE,
                                 (float)TILE_SIZE, (float)TILE_SIZE};
  TILE_HIGHLIGHT_R = (Rectangle){0.0f * TILE_SIZE, 11.0f * TILE_SIZE,
                                 (float)TILE_SIZE, (float)TILE_SIZE};
  TILE_HAND = (Rectangle){1.0f * TILE_SIZE, 11.0f * TILE_SIZE, (float)TILE_SIZE,
                          (float)TILE_SIZE};
  TILE_HALF_HEART = (Rectangle){2.0f * TILE_SIZE, 11.0f * TILE_SIZE,
                                (float)TILE_SIZE, (float)TILE_SIZE};
  TILE_GRASS_PLAIN =
      (Rectangle){0.0f, 0.0f, (float)TILE_SIZE, (float)TILE_SIZE};
  TILE_GRASS = (Rectangle){1.0f * TILE_SIZE, 0.0f * TILE_SIZE, (float)TILE_SIZE,
                           (float)TILE_SIZE};
  TILE_GRASS_FLOWER = (Rectangle){2.0f * TILE_SIZE, 0.0f * TILE_SIZE,
                                  (float)TILE_SIZE, (float)TILE_SIZE};

  TILE_GRAY_TANK = (Rectangle){8.0f * TILE_SIZE, 5.0f * TILE_SIZE,
                               (float)TILE_SIZE, (float)TILE_SIZE};
  TILE_GRAY_GRUNT = (Rectangle){17.0f * TILE_SIZE, 5.0f * TILE_SIZE,
                                (float)TILE_SIZE, (float)TILE_SIZE};

  TILE_BLUE_TANK = (Rectangle){8.0f * TILE_SIZE, 7.0f * TILE_SIZE,
                               (float)TILE_SIZE, (float)TILE_SIZE};
  TILE_BLUE_GRUNT = (Rectangle){17.0f * TILE_SIZE, 7.0f * TILE_SIZE,
                                (float)TILE_SIZE, (float)TILE_SIZE};
}
