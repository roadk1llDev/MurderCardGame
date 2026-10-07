/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Gameplay Screen Functions Definitions (Init, Update, Draw, Unload)
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
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

static int framesCounter = 0;
static int finishScreen = 0;
static float monthTextTimer = 0.0f;
static int currentMonth = 0;
static bool die1Rolled = false;
static bool die2Rolled = false;
static int numberRolled1 = 0;

void InitGameplayScreen(void)
{
    framesCounter = 0;
    finishScreen = 0;

    monthTextTimer = 3.0f;
    currentMonth = 1;

    die1Rolled = false;
    die2Rolled = false;

    numberRolled1 = 0;
}

void UpdateGameplayScreen(void)
{
    if (monthTextTimer > 0.0f){
	    monthTextTimer -= GetFrameTime();
    }

    if (IsKeyPressed(KEY_ENTER))
    {
        finishScreen = 1;
        PlaySound(fxCoin);
    }
}

void DrawGameplayScreen(void)
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BEIGE);

    int spriteWidth = 62;
    int spriteHeight = 86;
    int xOffset = 5;
    int yOffset = 5;
    int cardAmount = 5;
    int cardIndex = 0;
   
    int scale = 2.5f;
    int bottomBuffer = 50;
    int sideBuffer = 50;

    Vector2 origin = {0.0f, 0.0f};
    float rotation = 0.0f;
    
    int spacingSize = FindScreenSpacing(sideBuffer, cardAmount, spriteWidth * scale);
 
    Vector2 pos = {sideBuffer, GetScreenHeight() - (spriteHeight * scale) - bottomBuffer}; 
    for (int i = 0; i < cardAmount; i++){
        if(i != (cardAmount - 1)){
            cardIndex = 1 + (i * 14);
        } else {
	    cardIndex = 28;
        } 
           
        Rectangle sourceRec = GetSpriteInSheet(deckSpriteSheet, cardIndex, spriteWidth, spriteHeight, xOffset, yOffset, 1);
        
	Rectangle scaledRec = {pos.x, pos.y, spriteWidth * scale, spriteHeight * scale};
	DrawTexturePro(deckSpriteSheet, sourceRec, scaledRec, origin, rotation, WHITE);

        pos.x += (spriteWidth * scale) + spacingSize;	 
    }

    pos.x = sideBuffer;
    pos.y = GetScreenHeight() - (spriteHeight * scale * 2) - (bottomBuffer * 2);
    for(int i = 0; i < cardAmount; i++){
        if (i < cardAmount - 1){
            cardIndex = 28;
	} else { 
	    cardIndex = 0;
	}

	Rectangle sourceRec = GetSpriteInSheet(deckSpriteSheet, cardIndex, spriteWidth, spriteHeight, xOffset, yOffset, 1);
	
	Rectangle scaledRec = {pos.x, pos.y, spriteWidth * scale, spriteHeight * scale};
	DrawTexturePro(deckSpriteSheet, sourceRec, scaledRec, origin, rotation, WHITE);

	pos.x += (spriteWidth * scale) + spacingSize;
    } 
	
    spriteWidth = 32;
    spriteHeight = 32;
    xOffset = 0;
    yOffset = 0;

    pos.x = 0;
    pos.y = 0;

    if (monthTextTimer > 0.0f){
        char introBuffer[32];
	snprintf(introBuffer, sizeof(introBuffer), "MONTH %d", currentMonth);
		
	float fontSize = 400.0f;
	float spacing = 2.0f;

	Vector2 textSize = MeasureTextEx(font, introBuffer, fontSize, spacing);

	Vector2 textPos = {((float)GetScreenWidth() / 2.0f) - (textSize.x / 2.0f),
	((float)GetScreenHeight() / 2.0f) - (textSize.y / 2.0f)};
	DrawTextEx(font, introBuffer, textPos, fontSize, spacing, DARKBROWN);
    } else if (!die1Rolled) {
	die1Rolled = true;
	
	srand(time(NULL));
	numberRolled1 = rand() % 6;
    }
	
    if(die1Rolled){
	Rectangle sourceRec = GetSpriteInSheet(dieSpriteSheet, numberRolled1, spriteWidth, spriteHeight, xOffset, yOffset, 1);

	Rectangle scaledRec = {pos.x, pos.y, spriteWidth * scale, spriteHeight * scale};
	DrawTexturePro(dieSpriteSheet, sourceRec, scaledRec, origin, rotation, WHITE);
    }
}

void UnloadGameplayScreen(void)
{

}

int FinishGameplayScreen(void)
{
    return finishScreen;
}
