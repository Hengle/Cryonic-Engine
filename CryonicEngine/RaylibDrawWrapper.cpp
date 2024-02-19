#include "RaylibDrawWrapper.h"
#include "raylib.h"

void DrawRectangleWrapper(float posX, float posY, float sizeX, float sizeY, float rotation, unsigned char colorR, unsigned char colorG, unsigned char colorB, unsigned char colorA)
{
    DrawRectanglePro(
        {
            posX,
            posY,
            5.0f * sizeX,
            5.0f * sizeY
        },
            {
                5.0f * sizeX / 2,
                5.0f * sizeY / 2
            },
        rotation,
        { colorR, colorG, colorB, colorA });
}

void DrawCircleWrapper(int posX, int posY, float radius, unsigned char colorR, unsigned char colorG, unsigned char colorB, unsigned char colorA)
{
    DrawCircle(posX, posY, radius, { colorR, colorG, colorB, colorA });
}

void UnloadTextureWrapper()
{
    //UnloadTexture();
}
