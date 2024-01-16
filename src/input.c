#include "input.h"
#include "logic.h"
#include "util.h"
#include <raymath.h>

bool beingPressed = false;

short ClickTouchIndex() {
  Vector2 rTouch;
  if (GetTouchPointCount() > 0) {
    if (!beingPressed) {
      rTouch = GetTouchPosition(0);
    } else {
      return -1;
    }
  } else if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
    if (!beingPressed) {
      rTouch = GetMousePosition();
    } else {
      return -1;
    }
  } else {
    beingPressed = false;
    return -1;
  }
  beingPressed = true;
  float scale = MIN((float)GetScreenWidth() / gameSWidth,
                    (float)GetScreenHeight() / gameSHeight);
  Vector2 vTouch = {0};
  vTouch.x =
      (rTouch.x - (GetScreenWidth() - (gameSWidth * scale)) * 0.5f) / scale;
  vTouch.y =
      (rTouch.y - (GetScreenHeight() - (gameSHeight * scale)) * 0.5f) / scale;
  vTouch = Vector2Clamp(vTouch, (Vector2){0, 0},
                        (Vector2){(float)gameSWidth, (float)gameSHeight});

  return VecToIndex(vTouch);
}
