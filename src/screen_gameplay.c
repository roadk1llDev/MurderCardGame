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

const int cardWidth = 62;
const int cardHeight = 86;
const int xOffset = 5;
const int yOffset = 5;
    
const int dieWidth = 32;
const int dieHeight = 32;
const int dieBuffer = 50;
const int xDieOffset = 0;
const int yDieOffset = 0;

const int scale = 2;
const int bottomBuffer = 50;
const int sideBuffer = 50;

struct Die {
    bool hasBeenRolled;
    bool hasBeenMoved; 
    int numberRolled;
    int posIndex;
};

struct Die die[2];

enum gamePhases { Roll, Rummage, Shop, War };
enum decipleType { Banker, Pol, Knight, Doc };

enum gamePhases currentPhase;

struct Deciple {
   enum decipleType deciple;
   int level;
   int boostedLevel; 
};

#define CARDS_IN_HAND 4
struct Deciple hand[CARDS_IN_HAND];

void InitGameplayScreen(void)
{
    framesCounter = 0;
    finishScreen = 0;

    monthTextTimer = 1.0f;
    currentMonth = 1;
    
    for(int i = 0; i < 2; i++){
        die[i].hasBeenRolled = false;
        die[i].hasBeenMoved = false;
        die[i].numberRolled = 0;
        die[i].posIndex = 0;
    }

    for(int i = 0; i < CARDS_IN_HAND; i++){
        hand[i].deciple = i;
        hand[i].level = 1;
        hand[i].boostedLevel = 0;	
    }

    currentPhase = Roll;
}

void UpdateGameplayScreen(void)
{
    if (monthTextTimer > 0.0f){
	    monthTextTimer -= GetFrameTime();
    }
    
    /*
    if (IsKeyPressed(KEY_ENTER))
    {
        finishScreen = 1;
        PlaySound(fxCoin);
    }
    */
    
    if(currentPhase == Roll && monthTextTimer <= 0.0f){
        CheckForMoveDie(KEY_ONE, 1); 
        CheckForMoveDie(KEY_TWO, 2); 
        CheckForMoveDie(KEY_THREE, 3); 
        CheckForMoveDie(KEY_FOUR, 4); 
    }
}

void CheckForMoveDie(int keycode, int indexPos){
    if(die[0].hasBeenMoved && die[1].hasBeenMoved){
    	return;
    }

    if(IsKeyPressed(keycode)){
        for(int i = 0; i < 2; i++){
            if(die[i].posIndex == indexPos || die[(i + 1) % 2].posIndex == indexPos){
                int placeholderIndex = die[i].posIndex;
                die[i].posIndex = die[(i + 1) % 2].posIndex;
                die[(i + 1) % 2].posIndex = placeholderIndex;
                die[i].hasBeenMoved = !die[i].hasBeenMoved;
                die[(i + 1) % 2].hasBeenMoved = !die[(i + 1) % 2].hasBeenMoved;
                return; 
            }

            if(!die[i].hasBeenMoved){
                die[i].hasBeenMoved = true;
                die[i].posIndex = indexPos;

                for(int i = 0; i < 2; i++){
                    if(!die[i].hasBeenMoved){
                        return;
                    }
                }

                currentPhase = Rummage;
                return;
            }
        }
    }
}

void RenderCardRow(int cardRow[], int cardRowSize, int rowPos){
    Vector2 origin = {0.0f, 0.0f};
    float rotation = 0.0f;

    int spacingSize = FindScreenSpacing(sideBuffer, cardRowSize, cardWidth * scale);

    Vector2 pos = {sideBuffer, GetScreenHeight() - ((rowPos + 1) * cardHeight * scale) - ((rowPos + 1) * bottomBuffer)};
    for(int i = 0; i < cardRowSize; i++){
       Rectangle sourceRec = GetSpriteInSheet(deckSpriteSheet, cardRow[i], cardWidth, cardHeight, xOffset, yOffset, 1);

        Rectangle scaledRec = {pos.x, pos.y, cardWidth * scale, cardHeight * scale};
        DrawTexturePro(deckSpriteSheet, sourceRec, scaledRec, origin, rotation, WHITE);

        pos.x += (cardWidth * scale) + spacingSize; 
    }
}

void RenderNumberLable(int cardRow[], int cardRowSize, int rowPos){
    int spacingSize = FindScreenSpacing(sideBuffer, cardRowSize, cardWidth * scale);

	for (int i = 0; i < cardRowSize; i++) {
        if(cardRow[i] == 0 || cardRow[i] == 28){
            continue;
        }

        char introBuffer[32];
	    snprintf(introBuffer, sizeof(introBuffer), "%d", i + 1);
		
	    float fontSize = 40.0f;
	    float spacing = 2.0f;

	    Vector2 textSize = MeasureTextEx(font, introBuffer, fontSize, spacing);

	    Vector2 textPos = {sideBuffer + ((cardWidth * scale) / 2) - (textSize.x / 2.0f) + (i * cardWidth * scale) + (i * spacingSize), (float)GetScreenHeight() - (textSize.y / 2.0f) - (bottomBuffer / 2) - (rowPos * cardHeight * scale) - (rowPos * bottomBuffer)};
	    DrawTextEx(font, introBuffer, textPos, fontSize, spacing, DARKBROWN);
	}
}

void DrawGameplayScreen(void)
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BEIGE);
    Vector2 origin = { 0.0f, 0.0f };
    float rotation = 0.0f;

    int displayHand[CARDS_IN_HAND + 1];
    for(int i = 0; i < CARDS_IN_HAND + 1; i++){
        if(i == CARDS_IN_HAND){
            displayHand[i] = 28;
        } else {
            displayHand[i] = hand[i].level + (14 * hand[i].deciple); 
        }
    } 
    RenderCardRow(displayHand, CARDS_IN_HAND + 1, 0);
    
    int row2[CARDS_IN_HAND + 1];
    for(int i = 0; i < CARDS_IN_HAND + 1; i++){
        if(i == CARDS_IN_HAND){
            row2[i] = 0;
        } else {
            row2[i] = 28;
        }
    }
    RenderCardRow(row2, CARDS_IN_HAND + 1, 1);    
	
    if (monthTextTimer > 0.0f){
        char introBuffer[32];
	    snprintf(introBuffer, sizeof(introBuffer), "MONTH %d", currentMonth);
		
	    float fontSize = 400.0f;
	    float spacing = 2.0f;

	    Vector2 textSize = MeasureTextEx(font, introBuffer, fontSize, spacing);

	    Vector2 textPos = {((float)GetScreenWidth() / 2.0f) - (textSize.x / 2.0f),
	    ((float)GetScreenHeight() / 2.0f) - (textSize.y / 2.0f)};
	    DrawTextEx(font, introBuffer, textPos, fontSize, spacing, DARKBROWN);
    } else if (!die[0].hasBeenRolled || !die[1].hasBeenRolled) {
	    srand(time(NULL));

        for(int i = 0; i < 2; i++){
            if(!die[i].hasBeenRolled){
                die[i].hasBeenRolled = true;
                die[i].numberRolled = rand() % 6;
            } 
        } 
    }

    for(int i = 0; i < 2; i++){
        Vector2 pos = CalculateDiePosition(die[i].posIndex, i + 1);
        
        if(die[i].hasBeenRolled){
            Rectangle sourceRec = GetSpriteInSheet(dieSpriteSheet, die[i].numberRolled, dieWidth, dieHeight, xDieOffset, yDieOffset, 1);

            Rectangle scaledRec = {pos.x, pos.y, dieWidth * scale, dieHeight * scale};
            DrawTexturePro(dieSpriteSheet, sourceRec, scaledRec, origin, rotation, WHITE);
        } 
    }
	
    if (die[0].hasBeenRolled && die[1].hasBeenRolled && (currentPhase == Roll)) {
        int currentHand[5] = {1,1,1,1,0};
        RenderNumberLable(currentHand, CARDS_IN_HAND + 1, 0);
    }
}

Vector2 CalculateDiePosition(int dieIndexPos, int dieNumber){
    int xPos = 0;
    int yPos = 0;

    int spacingSize = FindScreenSpacing(sideBuffer, CARDS_IN_HAND + 1, cardWidth * scale);

    switch (dieIndexPos){
	    case 0:
	        if(dieNumber == 1){
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
