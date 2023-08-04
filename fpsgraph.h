/*

Usage:

#include <raylib.h>

#include "fpsgraph.h"

#define TARGETFPS 240

int main(void)
{
    InitWindow(...);
    SetTargetFPS(TARGETFPS);

    initFPSGraph(TARGETFPS, 100, 50);

    while (!WindowShouldClose())
    {
        BeginDrawing();

            tickFPSGraph(GetFPS());
            drawFPSGraph(0, 0);

        EndDrawing();
    }

    unloadFPSGraph();

    CloseWindow();

    return 0;
}

*/

#ifndef FPSGRAPH_H
#define FPSGRAPH_H

#include <raylib.h>
#include <stdlib.h>

#define FPSDISPLAYPIXELINCREMENT 100 /* amount of pixels the graph moves forward per second */
#define FPSDISPLAYLINECOLOR GREEN /* the color of the fps graph itself */
#define FPSDISPLAYTARGETLINECOLOR WHITE /* the color of the line indicating the target fps */
#define FPSDISPLAYCURRENTXLINECOLOR RED /* the color of the line at the front of the fps line */
#define FPSDISPLAYFONTSIZE 10
#define FPSDISPLAYBACKGROUNDCOLOR BLACK

static RenderTexture g_fps_graphTexture; /* contains only the fps graph */
static RenderTexture g_fps_canvasTexture; /* contains fps graph and things like text */
static bool g_fps_initialized = false;
static int g_fps_targetFPS = 0;

static float g_fps_lastX = .0f;
static int g_fps_lastY = 0;

/* fps_intern_maps value in range min1-max2 to range min2-max2 */
inline static float fps_intern_map(float min1, float max1, float min2, float max2, float val)
{
    float diff1 = (max1 - min1);
    float between = max1 - val;
    float i = between / diff1;
    return min2 + (max2 - min2) * i;
}

/* initializes the fps graph textures */
inline static void initFPSGraph(int targetFPS, int width, int height)
{
    if (g_fps_initialized) return;

    g_fps_graphTexture = LoadRenderTexture(width, height);
    g_fps_canvasTexture = LoadRenderTexture(width, height);
    
    g_fps_targetFPS = targetFPS;

    g_fps_initialized = true;
}

inline static void tickFPSGraph(int fps)
{
    if (!g_fps_initialized)
    {
        TraceLog(LOG_ERROR, "fps graph not initialized");
        abort();
    }
    else
    {
        // draw fpsgraph itself

        BeginTextureMode(g_fps_graphTexture);

            if (g_fps_lastX < g_fps_graphTexture.texture.width)
            {
                int nextY = fps_intern_map(0, g_fps_targetFPS*2, 0, g_fps_graphTexture.texture.height, fps);

                // increment x position relative to the current frame time
                float nextX = g_fps_lastX + FPSDISPLAYPIXELINCREMENT * GetFrameTime();

                DrawLine(g_fps_lastX, g_fps_lastY, nextX, nextY, FPSDISPLAYLINECOLOR);

                g_fps_lastX = nextX;
                g_fps_lastY = nextY;
            }
            else
            {
                g_fps_lastX = .0f;

                ClearBackground(BLANK);
            }

        EndTextureMode();

        // draw util

        BeginTextureMode(g_fps_canvasTexture);

            ClearBackground(BLACK);

            // target drawing
            DrawLine(0, g_fps_canvasTexture.texture.height/2, g_fps_canvasTexture.texture.width, g_fps_canvasTexture.texture.height/2, FPSDISPLAYTARGETLINECOLOR);
            DrawText(TextFormat("target: %d fps", g_fps_targetFPS), 0, g_fps_canvasTexture.texture.height/2, 10, FPSDISPLAYTARGETLINECOLOR);
            DrawText(TextFormat("current: %d fps", fps), 0, g_fps_canvasTexture.texture.height/2 + 10, 10, FPSDISPLAYTARGETLINECOLOR);

            // line at the front of fps graph
            DrawLine(g_fps_lastX, 0, g_fps_lastX, g_fps_canvasTexture.texture.height, FPSDISPLAYCURRENTXLINECOLOR);

            DrawTextureRec(g_fps_graphTexture.texture, (Rectangle){ 0, 0, g_fps_graphTexture.texture.width, -g_fps_graphTexture.texture.height }, (Vector2){ 0, 0 }, WHITE);

        EndTextureMode();
    }
}

inline static void drawFPSGraph(int x, int y)
{
    DrawTextureRec(g_fps_canvasTexture.texture, (Rectangle){ 0, 0, g_fps_canvasTexture.texture.width, -g_fps_canvasTexture.texture.height }, (Vector2){ x, y }, WHITE);
}

/* returns fps graph texture. This is y flipped because of opengl coordinate space */
inline static Texture2D getFPSGraphTexture(void)
{
    if (!g_fps_initialized)
    {
        TraceLog(LOG_ERROR, "fps graph not initialized");
        abort();
    }
    else 
        return g_fps_canvasTexture.texture;
}

inline static void unloadFPSGraph(void)
{
    UnloadRenderTexture(g_fps_canvasTexture);
    UnloadRenderTexture(g_fps_graphTexture);
}

#endif // FPSGRAPH_H