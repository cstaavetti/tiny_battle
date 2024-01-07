#include "logic.h"
#include "structs.h"
#include "tiles.h"
#include "util.h"
#include <raylib.h>
#include <raymath.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

RenderTexture2D target;

void renderGame() {
  for (short tileCount = 0; tileCount < MAP_SIZE; tileCount++) {
    DrawTextureRec(TILE_MAP, GetSurfaceRect(GetSurfaceType(tileCount)),
                   IndexToVec(tileCount), WHITE);
    if (HasUnit(tileCount)) {
      DrawTextureRec(
          TILE_MAP, GetUnitRect(GetUnitType(tileCount), GetUnitTeam(tileCount)),
          IndexToVec(tileCount), WHITE);
      if (UnitDamaged(tileCount)) {
        DrawTextureRec(TILE_MAP, TILE_HALF_HEART, IndexToVec(tileCount), WHITE);
      }
    }
    if (GameState == STATE_MOVE) {
      if (WalkableTypeIs(tileCount, WALK_ALLOWED)) {
        DrawTextureRec(TILE_MAP, TILE_HIGHLIGHT_Y, IndexToVec(tileCount),
                       WHITE);
      }
    }
  }
  if (GameState == STATE_FIRE) {
    for (short k = 0; k < DIR_COUNT; k++) {
      if (Targets[k] > -1) {
        DrawTextureRec(TILE_MAP, TILE_HIGHLIGHT_R, IndexToVec(Targets[k]),
                       WHITE);
      }
    }
  }
  if (GameState == STATE_IDLE) {
    DrawTextureRec(TILE_MAP, TILE_CURSOR, IndexToVec(Cursor), WHITE);
  }
  if (GameState == STATE_MOVE || GameState == STATE_FIRE) {
    DrawTextureRec(TILE_MAP, TILE_HAND, IndexToVec(Cursor), WHITE);
  }
}

void updateFrame() {
  RunGame();

  float scale = MIN((float)GetScreenWidth() / gameScreenWidth,
                    (float)GetScreenHeight() / gameScreenHeight);
  // Draw texture
  BeginTextureMode(target);
  ClearBackground(BLACK);
  renderGame();

#if defined(PLATFORM_WEB)
  DrawTextureRec(CONTROLLER_TILE_MAP, TILE_CONTROLLER, (Vector2){0.0f, 128.0f},
                 WHITE);
#endif

  EndTextureMode();
  // ---
  // Draw render texture to screen, properly scaled
  BeginDrawing();
  ClearBackground(BLACK);
  DrawTexturePro(
      target.texture,
      (Rectangle){0.0f, 0.0f, (float)target.texture.width,
                  (float)-target.texture.height},
      (Rectangle){
          (GetScreenWidth() - ((float)gameScreenWidth * scale)) * 0.5f,
          (GetScreenHeight() - ((float)gameScreenHeight * scale)) * 0.5f,
          (float)gameScreenWidth * scale, (float)gameScreenHeight * scale},
      (Vector2){0, 0}, 0.0f, WHITE);
  EndDrawing();
}

int main() {
#if defined(PLATFORM_WEB)
  gameScreenHeight = WEB_HEIGHT;
#endif
  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
  InitWindow(800, 800, "Tiny Battle");
  InitTiles();

  target = LoadRenderTexture(gameScreenWidth, gameScreenHeight);
  SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

  SetTargetFPS(60);

  InitGame();

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(updateFrame, 60, 1);
#else
  SetTargetFPS(60);
  while (!WindowShouldClose()) {
    updateFrame();
  }
#endif

  UnloadRenderTexture(target);
  CloseWindow();
  return 0;
}
