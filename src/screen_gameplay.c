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
static int numberRolled2 = 0;
static int die1IndexPos = 0;
static int die2IndexPos = 0;
static bool die1Moved = false;
static bool die2Moved = false;

static bool buttonPressed = false;

void InitGameplayScreen(void)
{
    framesCounter = 0;
    finishScreen = 0;

    monthTextTimer = 3.0f;
    currentMonth = 1;

    die1Rolled = false;
    die2Rolled = false;

    numberRolled1 = 0;
    numberRolled2 = 0;

    die1IndexPos = 0;
    die2IndexPos = 0;

    die1Moved = false;
    die2Moved = false;

    buttonPressed = false;
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
    
    CheckForMoveDie(KEY_ONE, 1); 
    CheckForMoveDie(KEY_TWO, 2); 
    CheckForMoveDie(KEY_THREE, 3); 
    CheckForMoveDie(KEY_FOUR, 4); 
}

void CheckForMoveDie(int keycode, int indexPos){
    if(die1Moved && die2Moved){
	return;
    }

    if(IsKeyPressed(keycode)){
	if (!die1Moved){
	    die1IndexPos = indexPos;
	    die1Moved = true;
	} else if (!die2Moved){
	    die2IndexPos = indexPos;
	    die2Moved = true;
	}
    }
}

void DrawGameplayScreen(void)
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BEIGE);

    int cardWidth = 62;
    int cardHeight = 86;
    int xOffset = 5;
    int yOffset = 5;
    int cardAmount = 5;
    int cardIndex = 0;
   
    int scale = 2;
    int bottomBuffer = 50;
    int sideBuffer = 50;

    Vector2 origin = {0.0f, 0.0f};
    float rotation = 0.0f;
    
    int spacingSize = FindScreenSpacing(sideBuffer, cardAmount, cardWidth * scale);
 
    Vector2 pos = {sideBuffer, GetScreenHeight() - (cardHeight * scale) - bottomBuffer}; 
    for (int i = 0; i < cardAmount; i++){
        if(i != (cardAmount - 1)){
            cardIndex = 1 + (i * 14);
        } else {
	    cardIndex = 28;
        } 
           
        Rectangle sourceRec = GetSpriteInSheet(deckSpriteSheet, cardIndex, cardWidth, cardHeight, xOffset, yOffset, 1);
        
	Rectangle scaledRec = {pos.x, pos.y, cardWidth * scale, cardHeight * scale};
	DrawTexturePro(deckSpriteSheet, sourceRec, scaledRec, origin, rotation, WHITE);

        pos.x += (cardWidth * scale) + spacingSize;	 
    }

    pos.x = sideBuffer;
    pos.y = GetScreenHeight() - (cardHeight * scale * 2) - (bottomBuffer * 2);
    for(int i = 0; i < cardAmount; i++){
        if (i < cardAmount - 1){
            cardIndex = 28;
	} else { 
	    cardIndex = 0;
	}

	Rectangle sourceRec = GetSpriteInSheet(deckSpriteSheet, cardIndex, cardWidth, cardHeight, xOffset, yOffset, 1);
	
	Rectangle scaledRec = {pos.x, pos.y, cardWidth * scale, cardHeight * scale};
	DrawTexturePro(deckSpriteSheet, sourceRec, scaledRec, origin, rotation, WHITE);

	pos.x += (cardWidth * scale) + spacingSize;
    } 
	
    int dieWidth = 32;
    int dieHeight = 32;
    int dieBuffer = 50;
    xOffset = 0;
    yOffset = 0;

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
    } else if (!die2Rolled) {
	die2Rolled = true;

	numberRolled2 = rand() % 6;
    }
	
    pos = CalculateDiePosition(die1IndexPos, dieWidth, dieHeight, dieBuffer, cardWidth, cardHeight, spacingSize, scale, sideBuffer, bottomBuffer, 1);

    if(die1Rolled){
	Rectangle sourceRec = GetSpriteInSheet(dieSpriteSheet, numberRolled1, dieWidth, dieHeight, xOffset, yOffset, 1);

	Rectangle scaledRec = {pos.x, pos.y, dieWidth * scale, dieHeight * scale};
	DrawTexturePro(dieSpriteSheet, sourceRec, scaledRec, origin, rotation, WHITE);
    }
  
    pos = CalculateDiePosition(die2IndexPos, dieWidth, dieHeight, dieBuffer, cardWidth, cardHeight, spacingSize, scale, sideBuffer, bottomBuffer, 2);
    if(die2Rolled){
	Rectangle sourceRec = GetSpriteInSheet(dieSpriteSheet, numberRolled2, dieWidth, dieHeight, xOffset, yOffset, 1);

	Rectangle scaledRec = {pos.x, pos.y, dieWidth * scale, dieHeight * scale};
	DrawTexturePro(dieSpriteSheet, sourceRec, scaledRec, origin, rotation, WHITE);
    }

    if(die1Rolled && die2Rolled && (!die1Moved || !die2Moved)){
	for (int i = 0; i < cardAmount - 1; i++){
            char introBuffer[32];
	    snprintf(introBuffer, sizeof(introBuffer), "%d", i + 1);
		
	    float fontSize = 40.0f;
	    float spacing = 2.0f;

	    Vector2 textSize = MeasureTextEx(font, introBuffer, fontSize, spacing);

	    Vector2 textPos = {sideBuffer + ((cardWidth * scale) / 2) - (textSize.x / 2.0f) + (i * cardWidth * scale) + (i * spacingSize),
	    (float)GetScreenHeight() - (textSize.y / 2.0f) - (bottomBuffer / 2)};
	    DrawTextEx(font, introBuffer, textPos, fontSize, spacing, DARKBROWN);
	}
    }
}

Vector2 CalculateDiePosition(int dieIndexPos, int dieWidth, int dieHeight, int dieBuffer, int cardWidth, int cardHeight, int spacingSize, int scale, int sideBuffer, int bottomBuffer, int die){
    int xPos = 0;
    int yPos = 0;

    switch (dieIndexPos){
	case 0:
	    if(die == 1){
                xPos = GetScreenWidth() - (GetScreenWidth() / 2) - (dieWidth * scale) - (dieBuffer / 2);
	    } else {
		xPos = GetScreenWidth() - (GetScreenWidth() / 2) + (dieBuffer / 2);
	    }
	    
    	    yPos = GetScreenHeight() - (cardHeight * scale * 1.5f) - (bottomBuffer * 2) - ((dieHeight * scale) / 2);
            break;
	case 1:
	case 2:
	case 3:
    	case 4:
	    xPos = sideBuffer + ((dieIndexPos - 1) * cardWidth * scale) + ((cardWidth * scale) / 2) + ((dieIndexPos - 1) * spacingSize) - ((dieWidth * scale) / 2);
            yPos = GetScreenHeight() - (bottomBuffer * 2) - (cardHeight * scale);
	    break;
	default:
	    break;	    
    }   

    Vector2 finalPos = {xPos, yPos};
    return finalPos;
}

void UnloadGameplayScreen(void)
{

}

int FinishGameplayScreen(void)
{
    return finishScreen;
}
