#include "raylib.h"
#include "raymath.h"

#define windowWidth 1280
#define windowHeight 720

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

int main(void)
{
    SetTraceLogLevel(LOG_ALL);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(windowWidth, windowHeight, "Raylib - template");

    // the screen size we render to, that is then scaled
    int virtualWidth = 0, virtualHeight = 0;
    {
        int m = GetCurrentMonitor();
        virtualWidth = GetMonitorWidth(m);
        virtualHeight = GetMonitorHeight(m);
    }

    // RTT target for the virtual screen
    RenderTexture2D target = LoadRenderTexture(virtualWidth, virtualHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

    int fsToggle = 0;

    float dT = 0;

    ClearBackground(BLACK);

    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        dT = GetFrameTime();

        // adjust mouse coordinates for scale and offset
        float screenScale = min((float)GetScreenWidth() / virtualWidth, (float)GetScreenHeight() / virtualHeight);

        SetMouseScale(1.0 / screenScale, 1.0 / screenScale);
        SetMouseOffset(-(GetScreenWidth() - (virtualWidth * screenScale)) * 0.5,
                       -(GetScreenHeight() - (virtualHeight * screenScale)) * 0.5);
        
        if (fsToggle)
        {
            fsToggle--;
            if (!fsToggle)
            {
                ToggleFullscreen();
                // need to set window size here or it upsets toggling
                if (!IsWindowFullscreen())
                {
                    SetWindowSize(windowWidth, windowHeight);
                    SetWindowPosition((GetScreenWidth() - windowWidth) / 2,
                                      (GetScreenHeight() - windowHeight) / 2);
                }
            }
        }

        if (IsKeyDown(KEY_F1))
        {
            fsToggle = 2; // at least two or toggling doesn't work
            if (!IsWindowFullscreen())
            {
                SetWindowSize(virtualWidth, virtualHeight);
            }
        }

        BeginDrawing();

        if (IsWindowResized())
        {
            ClearBackground(BLACK);
        }

        // render to a texture so we can scale to the window
        BeginTextureMode(target);
        ClearBackground((Color){64, 128, 255, 255});

        // 2D overlay
        DrawText(TextFormat("screen scale %.4f", screenScale), 20, 80, 40, WHITE);
        DrawText("F1 to toggle fullscreen", 20, 120, 40, WHITE);

        Vector2 mp = GetMousePosition();
        DrawCircle(mp.x, mp.y, 12, RED);

        EndTextureMode();

        // draw the virtual screen on the actual one
        DrawTexturePro(target.texture, (Rectangle){0.0f, 0.0f, target.texture.width, -target.texture.height},
                       (Rectangle){(GetScreenWidth() - virtualWidth * screenScale) * 0.5,
                                   (GetScreenHeight() - virtualHeight * screenScale) * 0.5,
                                   virtualWidth * screenScale, virtualHeight * screenScale},
                       (Vector2){0, 0}, 0.0f, WHITE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}