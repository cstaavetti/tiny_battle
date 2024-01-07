#include "input.h"
#include "logic.h"
#include "structs.h"
#include <complex.h>
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>

typedef struct Touch {
  bool keyDown;
  bool pressed;
} Touch;

Touch touchMap[6]; // up, down, left, right, a, b
int keyMap[6] = {KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_X, KEY_Z};

void sortDir(short dir, bool keyDown) {
  if (touchMap[dir].keyDown == true) {
    touchMap[dir].pressed = false;
  } else if (keyDown == true) {
    touchMap[dir].pressed = true;
  }
  touchMap[dir].keyDown = keyDown;
}

void UpdateTouchMap() {
  bool dir[6] = {false, false, false, false, false, false};
  int touches = GetTouchPointCount();
  float scale = MIN((float)GetScreenWidth() / gameScreenWidth,
                    (float)GetScreenHeight() / gameScreenHeight);
  for (int i = 0; i < touches; i++) {
    Vector2 touchLoc = GetTouchPosition(i);
    Vector2 virtualtouchLoc = {0};
    virtualtouchLoc.x =
        (touchLoc.x - (GetScreenWidth() - (gameScreenWidth * scale)) * 0.5f) /
        scale;
    virtualtouchLoc.y =
        (touchLoc.y - (GetScreenHeight() - (gameScreenHeight * scale)) * 0.5f) /
        scale;
    virtualtouchLoc = Vector2Clamp(
        virtualtouchLoc, (Vector2){0, 0},
        (Vector2){(float)gameScreenWidth, (float)gameScreenHeight});
    if (CheckCollisionPointCircle(virtualtouchLoc, (Vector2){.x = 33, .y = 150},
                                  12.0f)) {
      dir[0] = true;
    }
    if (CheckCollisionPointCircle(virtualtouchLoc, (Vector2){.x = 33, .y = 170},
                                  12.0f)) {
      dir[1] = true;
    }
    if (CheckCollisionPointCircle(virtualtouchLoc, (Vector2){.x = 21, .y = 160},
                                  12.0f)) {
      dir[2] = true;
    }
    if (CheckCollisionPointCircle(virtualtouchLoc, (Vector2){.x = 43, .y = 160},
                                  12.0f)) {
      dir[3] = true;
    }
    if (CheckCollisionPointCircle(virtualtouchLoc,
                                  (Vector2){.x = 106, .y = 152}, 12.0f)) {
      dir[4] = true;
    }
    if (CheckCollisionPointCircle(virtualtouchLoc, (Vector2){.x = 78, .y = 165},
                                  12.0f)) {
      dir[5] = true;
    }
  }
  for (short i = 0; i < 6; i++) {
    printf("%d, ", dir[i]);
    sortDir(i, dir[i]);
  }
}

bool TouchIsKeyDown(int key) {
#if defined(PLATFORM_WEB)
  bool press = false;
  for (short i = 0; i < 6; i++) {
    if (keyMap[i] == key) {
      press = touchMap[i].keyDown;
    }
  }
  return press == false ? IsKeyDown(key) : true;
#else
  return IsKeyDown(key);
#endif
}

bool TouchIsKeyPressed(int key) {
#if defined(PLATFORM_WEB)
  bool press = false;
  for (short i = 0; i < 6; i++) {
    if (keyMap[i] == key) {
      press = touchMap[i].pressed;
    }
  }
  return press == false ? IsKeyPressed(key) : true;
#else
  return IsKeyPressed(key);
#endif
}
