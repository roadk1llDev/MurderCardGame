/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Title Screen Functions Definitions (Init, Update, Draw, Unload)
*
*   Copyright (c) 2014-2022 Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#include "raylib.h"
#include "screens.h"

static int framesCounter = 0;
static int finishScreen = 0;
static float titleTextMultiplier = 10.0f;
static float startTextMultiplier = 5.0f;

void InitTitleScreen(void)
{
    framesCounter = 0;
    finishScreen = 0;

    titleTextMultiplier = 10.0f;
    startTextMultiplier = 5.0f;
}

void UpdateTitleScreen(void)
{

    if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
    {
        //finishScreen = 1;   // OPTIONS
        finishScreen = 2;   // GAMEPLAY
        PlaySound(fxCoin);
    }
}

void DrawTitleScreen(void)
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), RED);

    const char *title = "MURDER CARD GAME";
    int textSize = MeasureTextEx(font, title, font.baseSize*titleTextMultiplier, 4).x;
    Vector2 pos = {(GetScreenWidth() - textSize) / 2, 100};

    DrawTextEx(font, title, pos, font.baseSize*titleTextMultiplier, 4, DARKBROWN);
    
    const char *startText = "PRESS ENTER TO START";
    textSize = MeasureTextEx(font, startText, font.baseSize*startTextMultiplier, 4).x;
    pos.x = (GetScreenWidth() - textSize) / 2;
    pos.y = GetScreenHeight() / 2;

    DrawTextEx(font, startText, pos, font.baseSize*startTextMultiplier, 4, DARKBROWN);
}

void UnloadTitleScreen(void)
{

}

int FinishTitleScreen(void)
{
    return finishScreen;
}
