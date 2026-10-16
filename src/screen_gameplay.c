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
const int bottomBuffer = 40;
const int sideBuffer = 40;

struct Die {
    bool hasBeenRolled;
    bool hasBeenMoved; 
    int numberRolled;
    int posIndex;
};

struct Die die[2];

enum gamePhases { Roll, Rummage, Shop, Murder, War };
enum gameState { Wait, Choose, Return};
enum decipleType { Banker, Pol, Knight, Doc };

enum gamePhases currentPhase;
enum gameState currentState;

#define CARDS_IN_MEM 13
struct Deciple {
   enum decipleType deciple;
   int level;
   int boostedLevel;
   int mem[CARDS_IN_MEM];
};

#define CARDS_IN_HAND 4
struct Deciple hand[CARDS_IN_HAND];

bool deck[56];
int banish[56];
int murder[56];

int shop[6];
bool shopStocked;

bool buttonPressedThisFrame;

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
        for (int j = 0; j < CARDS_IN_MEM; j++){
            if(j == 0){
                hand[i].mem[j] = 1;
            } else {
                hand[i].mem[j] = -1;
            }
        }
    }

    for(int i = 0; i < 6; i++){
        shop[i] = 28;
    }

    shopStocked = false;

    for(int i = 0; i < 56; i++){
        banish[i] = -1;
        murder[i] = 28;

        if(i % 14 == 0 || i % 14 == 1){
            deck[i] = false;
        } else {
            deck[i] = true;
        }
    }

    currentPhase = Roll;
    currentState = Wait;

    buttonPressedThisFrame = false;
}

void UpdateGameplayScreen(void)
{
    if (monthTextTimer > 0.0f){
	    monthTextTimer -= GetFrameTime();
    } else if (currentPhase == Roll && currentState == Wait){
        for(int i = 0; i < CARDS_IN_HAND; i++){
            hand[i].boostedLevel = 0;
        }

        currentState = Choose;
    }

    if(currentPhase == Rummage && banish[0] == -1){
        currentPhase = Shop;
        shopStocked = false;

        for(int i = 0; i < 6; i++){
            shop[i] = 28;
        }
    }
    
    /*
    if (IsKeyPressed(KEY_ENTER))
    {
        finishScreen = 1;
        PlaySound(fxCoin);
    }
    */
    
    if(currentPhase == Roll && currentState == Choose && !buttonPressedThisFrame){
        for(int i = 0; i < 4; i++){
            CheckForMoveDie(KEY_ONE + i, 1 + i); 
        }
    }

    if(currentPhase == Shop && currentState == Choose && !buttonPressedThisFrame){
        for(int i = 0; i < 6; i++){
            CheckForPurchace(KEY_ONE + i, 1 + i);
        }
    }

    if(currentPhase == Murder && currentState == Choose && !buttonPressedThisFrame){
        for(int i = 0; i < 6; i++){
            CheckForMurder(KEY_ONE + i, 1 + i);
        }
    }

    buttonPressedThisFrame = false;
}

void CheckForMurder(int keycode, int indexPos){
    if(!IsKeyPressed(keycode)){
        return;
    }

    if(indexPos == 6){
        buttonPressedThisFrame = true;
        currentState = Return;
        return;
    }

    int murderPower = hand[1].level + hand[1].boostedLevel;

    int murderItemType = shop[indexPos - 1] / 14;
    if(shop[indexPos - 1] % 14 > murderPower || shop[indexPos - 1] == 28){
        return;
    } 

    for(int i = 0; i < 56; i++){
        if(murder[i] == 28){
            murder[i] = shop[indexPos - 1];
            break;
        }
    } 

    buttonPressedThisFrame = true;
    currentState = Return;
    shop[indexPos - 1] = 28;
}

void CheckForPurchace(int keycode, int indexPos){
    if(!IsKeyPressed(keycode)){
        return;
    }

    if(indexPos == 6){
        buttonPressedThisFrame = true;
        currentPhase = Murder;
        return;
    } 
    
    int spendingPower = hand[0].level + hand[0].boostedLevel;

    int shopItemType = shop[indexPos - 1] / 14;
    if(shop[indexPos - 1] % 14 > spendingPower || shop[indexPos - 1] % 14 < hand[shopItemType].level || shop[indexPos - 1] == 28){
        return;
    }   

    hand[shopItemType].level = shop[indexPos - 1] % 14;
    buttonPressedThisFrame = true;
    currentPhase = Murder; 

    for(int i = 0; i < CARDS_IN_MEM; i++){
        if (hand[shopItemType].mem[i] == -1){
            hand[shopItemType].mem[i] = shop[indexPos - 1];
        }
    }

    shop[indexPos - 1] = 28;
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

                if(die[i].hasBeenMoved){
                    hand[indexPos - 1].boostedLevel = die[i].numberRolled + 1; 
                } else {
                    hand[indexPos - 1].boostedLevel = die[(i + 1) % 2].numberRolled + 1; 
                }

                buttonPressedThisFrame = true;
                return; 
            }

            if(!die[i].hasBeenMoved){
                die[i].hasBeenMoved = true;
                die[i].posIndex = indexPos;
                hand[indexPos - 1].boostedLevel = die[i].numberRolled + 1; 

                for(int i = 0; i < 2; i++){
                    if(!die[i].hasBeenMoved){
                        buttonPressedThisFrame = true;
                        return;
                    }
                }

                currentPhase = Rummage;
                buttonPressedThisFrame = true;
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

void RenderPlayersHand(void){
    int displayHand[CARDS_IN_HAND + 1];
    for(int i = 0; i < CARDS_IN_HAND + 1; i++){
        if(i == CARDS_IN_HAND){
            displayHand[i] = 28;
        } else {
            displayHand[i] = hand[i].level + (14 * hand[i].deciple); 
        }
    } 
    RenderCardRow(displayHand, CARDS_IN_HAND + 1, 0);
}

void RenderDeck(void){
    int row2[CARDS_IN_HAND + 1];
    for(int i = 0; i < CARDS_IN_HAND + 1; i++){
        if(i == CARDS_IN_HAND){
            row2[i] = 0;
        } else {
            row2[i] = 28;
        }
    }
    RenderCardRow(row2, CARDS_IN_HAND + 1, 1);    
}

void RenderMurderPile(void){
    int row4[CARDS_IN_HAND + 1];
    for(int i = 0; i < CARDS_IN_HAND + 1; i++){
        row4[i] = 28;
    }

    for(int i = 0; i < 56; i++){
        if(murder[i] == 28){
            if(i != 0){
                row4[CARDS_IN_HAND] = murder[i - 1];
            }
            
            break;
        }
    }
    RenderCardRow(row4, CARDS_IN_HAND + 1, 3);    
}

void RenderMonthText(void){
        char introBuffer[32];
	    snprintf(introBuffer, sizeof(introBuffer), "MONTH %d", currentMonth);
		
	    float fontSize = 400.0f;
	    float spacing = 2.0f;

	    Vector2 textSize = MeasureTextEx(font, introBuffer, fontSize, spacing);

	    Vector2 textPos = {((float)GetScreenWidth() / 2.0f) - (textSize.x / 2.0f),
	    ((float)GetScreenHeight() / 2.0f) - (textSize.y / 2.0f)};
	    DrawTextEx(font, introBuffer, textPos, fontSize, spacing, DARKBROWN);
}

void RenderDie(int i){
    Vector2 origin = { 0.0f, 0.0f };
    float rotation = 0.0f;

        Vector2 pos = CalculateDiePosition(die[i].posIndex, i + 1);
        
        if(die[i].hasBeenRolled){
            Rectangle sourceRec = GetSpriteInSheet(dieSpriteSheet, die[i].numberRolled, dieWidth, dieHeight, xDieOffset, yDieOffset, 1);

            Rectangle scaledRec = {pos.x, pos.y, dieWidth * scale, dieHeight * scale};
            DrawTexturePro(dieSpriteSheet, sourceRec, scaledRec, origin, rotation, WHITE);
        } 
}

void RenderAndStockShop(){
    if(shopStocked == false){
        int cardsInDeck = 0; 
        for(int i = 0; i < 56; i++){
            if(deck[i] == true){
                cardsInDeck++;
            }
        }

        if(cardsInDeck == 0){
            currentPhase = War;
            return;
        }

        int cardsToStock;
        if(cardsInDeck >= 5){
            cardsToStock = 5;
        } else {
            cardsToStock = cardsInDeck;
        }

        int randomIndex = 0;
        while (cardsToStock > 0){
            randomIndex = rand() % 56;
            if(deck[randomIndex]){
                shop[5 - cardsToStock] = randomIndex;
                cardsToStock--;
            }
        }
        
        for(int i = 0; i < 5; i++){
            deck[shop[i]] = false;
        }
        shopStocked = true;
    }

    RenderCardRow(shop, 6, 2);    
}

void RenderTurnInfoText(char introBuffer[]){
	    float fontSize = 100.0f;
	    float spacing = 2.0f;

	    Vector2 textSize = MeasureTextEx(font, introBuffer, fontSize, spacing);

	    Vector2 textPos = {sideBuffer, bottomBuffer};
	    DrawTextEx(font, introBuffer, textPos, fontSize, spacing, DARKBROWN);
}

void DrawGameplayScreen(void)
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BEIGE);
    Vector2 origin = { 0.0f, 0.0f };
    float rotation = 0.0f;
    
    RenderPlayersHand();
    RenderDeck(); 
    RenderMurderPile();
	
    if (currentPhase == Roll && currentState == Wait){
        RenderMonthText();
    } else if (!die[0].hasBeenRolled || !die[1].hasBeenRolled) {
	    srand(time(NULL));

        for(int i = 0; i < 2; i++){
            if(!die[i].hasBeenRolled){
                die[i].hasBeenRolled = true;
                die[i].numberRolled = rand() % 6;
            } 
        } 
    }

    if((currentPhase == Shop || currentPhase == Murder) && currentState == Choose){
        RenderAndStockShop();                
        RenderNumberLable(shop, 6, 2);
    }

    for(int i = 0; i < 2; i++){
        RenderDie(i);
    }
	
    if (die[0].hasBeenRolled && die[1].hasBeenRolled && (currentPhase == Roll)) {
        int currentHand[5] = {1,1,1,1,0};
        RenderNumberLable(currentHand, CARDS_IN_HAND + 1, 0);
    }

    if(currentPhase == Shop && currentState == Choose){
        char introBuffer[64];
	    snprintf(introBuffer, sizeof(introBuffer), "Press 1-5 to buy card and 6 to skip");
        RenderTurnInfoText(introBuffer);
    }

    if(currentPhase == Murder && currentState == Choose){
        char introBuffer[64];
	    snprintf(introBuffer, sizeof(introBuffer), "Press 1-5 to murder card and 6 to skip");
        RenderTurnInfoText(introBuffer);
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
