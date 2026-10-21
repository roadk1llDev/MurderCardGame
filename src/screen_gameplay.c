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

bool debug;

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

Music baseTrack;

struct musicLayer {
    Music track;
    float volume;
};

struct musicLayer phaseLayer[5];

struct Die die[2];

enum gamePhases { Roll, Rummage, Shop, Murder, War, Double};
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

struct rowPosInfo {
    float currentPos;
    float desiredPos;
};

struct rowPosInfo row0Pos[6];
struct rowPosInfo row2Pos[20];

#define CARDS_IN_HAND 4
struct Deciple hand[CARDS_IN_HAND];

bool deck[56];
int banish[56];
int murder[56];
int warHand[4];

int rummage[19];
int rummagePower;

int shop[6];
bool shopStocked;

bool buttonPressedThisFrame;

void InitGameplayScreen(void)
{
    debug = true;

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
        banish[i] = 28;
        murder[i] = 28;

        if(i % 14 == 0 || i % 14 == 1){
            deck[i] = false;
        } else {
            deck[i] = true;
        }
    }

    for(int i = 0; i < 4; i++){
        warHand[i] = 28;
    }

    for(int i = 0; i < 19; i++){
        rummage[i] = 28;
    }
    rummagePower = 0;

    currentPhase = Roll;
    currentState = Wait;

    buttonPressedThisFrame = false;

    baseTrack = LoadMusicStream("resources/TillWinterBase.wav");
    phaseLayer[0].track = LoadMusicStream("resources/TillWinterRoll.wav");
    phaseLayer[1].track = LoadMusicStream("resources/TillWinterRummage.wav");
    phaseLayer[2].track = LoadMusicStream("resources/TillWinterShop.wav"); 
    phaseLayer[3].track = LoadMusicStream("resources/TillWinterMurder.wav"); 
    phaseLayer[4].track = LoadMusicStream("resources/TillWinterWar.wav");
    
    PlayMusicStream(baseTrack);
    for(int i = 0; i < 5; i++){
        phaseLayer[i].volume = 0;
        PlayMusicStream(phaseLayer[i].track);
        SetMusicVolume(phaseLayer[i].track, phaseLayer[i].volume);
    }
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
    
    if(currentPhase == Rummage && banish[0] == 28 && rummage[0] == 28){
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

    UpdateMusicStream(baseTrack);
    SetMusicVolume(baseTrack, 0.5f);
    for(int i = 0; i < 5; i++){
        UpdateMusicStream(phaseLayer[i].track);

        if(currentPhase == i){
            if(phaseLayer[i].volume < 0.5f){
                phaseLayer[i].volume += 0.01f; 
            } else if (phaseLayer[i].volume > 0.5f){
                phaseLayer[i].volume = 0.5f;
            }
        } else {
            if(phaseLayer[i].volume > 0.0f){
                phaseLayer[i].volume -= 0.01f;
            } else if (phaseLayer[i].volume < 0.0f){
                phaseLayer[i].volume = 0.0f;
            }
        }

        SetMusicVolume(phaseLayer[i].track, phaseLayer[i].volume);
    }
    
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

    if(currentPhase == Murder && currentState == Return && !buttonPressedThisFrame){
        for(int i = 0; i < 6; i++){
            ReturnCards(KEY_ONE + i, 1 + i, shop); 
        }
    }

    if(currentPhase == Rummage && currentState == Choose && !buttonPressedThisFrame){
        for(int i = 0; i < 10; i++){
            CheckForRummageMurder(KEY_ZERO + i, i); 
        }
       
        for(int i = 0; i < 10; i++){
            CheckForRummageMurder(KEY_A + i, 10 + i); 
        }
    }
    
    if(currentPhase == Rummage && currentState == Return && !buttonPressedThisFrame){
        for(int i = 0; i < 9; i++){
            ReturnCards(KEY_ONE + i, 1 + i, rummage); 
        }
        
        for(int i = 0; i < 10; i++){
            ReturnCards(KEY_A + i, 10 + i, rummage); 
        }
    }

    if(currentPhase == Double){
        if(currentState == Return){
            ReturnDouble(KEY_ENTER, 1); 
        } else if (currentState == Choose){
            for(int i = 0; i < 4; i++){
                ReturnDouble(KEY_ONE + i, 1 + i);
            }
        }
    }

    buttonPressedThisFrame = false;
}

void ReturnDouble(int keycode, int indexPos){
    if(!IsKeyPressed(keycode)){
        return;
    }

    if(currentState == Return){
        int highestCard = 0;
        for(int i = 0; i < 4; i++){
            if(hand[i].level > highestCard){
                indexPos = i + 1;
                highestCard = hand[i].level;
            }
        }
    } else {
        int highestCard = 0;
        bool validIndex[4];
        for(int i = 0; i < 4; i++){
            if(hand[i].level == highestCard){
                validIndex[i] = true;
            } else if (hand[i].level > highestCard){
                for(int j = 0; j < 4; j++){
                    validIndex[j] = false;
                }

                validIndex[i] = true;
                highestCard = hand[i].level;
            }
        }

        if(!validIndex[indexPos - 1]){
            return; 
        }
    }
    
    int memIndex = -1;
    for(int i = 0; i < 13; i++){
        if(hand[indexPos - 1].mem[i] == - 1 && i != 0 && i != 1){
            memIndex = i - 1;
            break; 
        }
    }

    if(memIndex == -1){
        memIndex = 12;
    }

    for(int i = 0; i < 56; i++){
        if(banish[i] == 28){
            banish[i] = hand[indexPos - 1].mem[memIndex];
            break;
        }
    }

    hand[indexPos - 1].mem[memIndex] = -1;
    hand[indexPos - 1].level = hand[indexPos - 1].mem[memIndex - 1] % 14;

    currentPhase = Roll;
    currentState = Wait;
    buttonPressedThisFrame = true;
}

void CheckForRummageMurder(int keycode, int indexPos){
    if(!IsKeyPressed(keycode)){
        return;
    } 

    if(keycode == KEY_ZERO){
        currentState = Return;
        buttonPressedThisFrame = true;
        return;
    }

    int murderPower = hand[2].level + hand[2].boostedLevel;

    if(rummage[indexPos - 1] % 14 > murderPower || rummage[indexPos - 1] == 28){
        return;
    } 
    
    for (int i = 0; i < 56; i++){
        if(murder[i] == 28){
            murder[i] = rummage[indexPos - 1];
            rummage[indexPos - 1] = 28;
            break;
        }
    }
    
    for(int i = 0; i < 19; i++){
        if(rummage[i] != 28){
            currentState = Return;
            buttonPressedThisFrame = true;
            return;
        }
    }

    currentPhase = Shop;
    currentState = Choose;
    shopStocked = false;
            
    for(int i = 0; i < 6; i++){
        shop[i] = 28;
    }
    
    buttonPressedThisFrame = true;
}

void ReturnCards(int keycode, int indexPos, int pile[]){
    if(!IsKeyPressed(keycode) || pile[indexPos - 1] == 28){
        return;
    }

    for(int i = 0; i < 56; i++){
        if(banish[i] == 28){
            banish[i] = pile[indexPos - 1];
            break;
        }
    }

    pile[indexPos - 1] = 28;

    switch(currentPhase){
        case Murder:
            for(int i = 0; i < 6; i++){
                if(shop[i] != 28){
                    return;
                }
            }
    
            currentPhase = Roll;
            currentState = Wait;
            monthTextTimer = 1.0f;
            currentMonth++;

            for(int i = 0; i < 2; i++){
                die[i].posIndex = 0;
                die[i].hasBeenRolled = false;
                die[i].hasBeenMoved = false;
            }
            break;
        case Rummage:
            for(int i = 0; i < 19; i++){
                if(rummage[i] != 28){
                    return;
                }
            }

            currentPhase = Shop;
            currentState = Choose;
            shopStocked = false;
            
            for(int i = 0; i < 6; i++){
                shop[i] = 28;
            }

            break;
    } 
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
            break;
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
                currentState = Choose;
                buttonPressedThisFrame = true;

                for(int i = 0; i < 19; i++){
                    rummage[i] = 28;
                }

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
        if(i >= 9){
            snprintf(introBuffer, sizeof(introBuffer), "%c", 'a' + (i - 9));
        } else {
	        snprintf(introBuffer, sizeof(introBuffer), "%d", i + 1);
        }
		
	    float fontSize = 40.0f;
	    float spacing = 2.0f;

	    Vector2 textSize = MeasureTextEx(font, introBuffer, fontSize, spacing);

	    Vector2 textPos = {sideBuffer + ((cardWidth * scale) / 2) - (textSize.x / 2.0f) + (i * cardWidth * scale) + (i * spacingSize), (float)GetScreenHeight() - (textSize.y / 2.0f) - (bottomBuffer / 2) - (rowPos * cardHeight * scale) - (rowPos * bottomBuffer)};
	    DrawTextEx(font, introBuffer, textPos, fontSize, spacing, DARKBROWN);
    }
}

void RenderPlayersHand(void){
    int displayHand[CARDS_IN_HAND + 1];
    
    int banishIndex = 0;
    for(int i = 0; i < 56; i++){
        if(banish[i] == 28){
            if(i != 0){
                banishIndex = i - 1;
            }

            break;
        }
    }

    for(int i = 0; i < CARDS_IN_HAND + 1; i++){
        if(i == CARDS_IN_HAND){
            displayHand[i] = banish[banishIndex];
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
    if(currentPhase == War && currentState == Wait){
        int rolesFilled = 0;
        int cardsRemaning = 0;
        for(int i = 0; i < 56; i++){
            if(banish[i] == 28){
                break;
            }

            cardsRemaning++;
        }
        
        int randomIndex;
        while(cardsRemaning > 0 && rolesFilled < 4){
            randomIndex = rand() % 56;

            if(banish[randomIndex] == 28){
                continue;
            }
          
            int banishCardType = banish[randomIndex] / 14;

            if(warHand[banishCardType] == 28){
                rolesFilled++;
            } else if (warHand[banishCardType] < banish[randomIndex]) {
                warHand[banishCardType] = banish[randomIndex];
            }

            banish[randomIndex] = 28;
            cardsRemaning--;
        }

        if(rolesFilled >= 4){
            currentState = Choose; 
        }
    }

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

void RenderRummage(void){
    int banishSize = 0;
    for(int i = 0; i < 56; i++){
        if(banish[i] == 28){
            break;
        }

        banishSize++;
    }

    int rummageSize = 0;
    for(int i = 0; i < 19; i++){
        if(rummage[i] != 28){
            rummageSize++;
        }
    }

    if(rummageSize == 0 && banishSize > 0){
        rummagePower = hand[3].level + hand[3].boostedLevel; 

        if(rummagePower > banishSize){
            rummagePower = banishSize;
        }

        int banishIndex = banishSize - 1;
        for(int i = 0; i < rummagePower; i++){
            rummage[i] = banish[banishIndex];
            banish[banishIndex] = 28;
            banishIndex--; 
        }
    }

    int row3[rummagePower + 1];
    for(int i = 0; i < rummagePower + 1; i++){
        if(i == rummagePower){
            row3[i] = 28;
        } else {
            row3[i] = rummage[i];
        }
    }

    RenderCardRow(row3, rummagePower + 1, 2);
}

void RenderMonthText(void){
        char introBuffer[32];

        if(currentMonth >= 11){
	        snprintf(introBuffer, sizeof(introBuffer), "WAR!");
            currentPhase = War;
            currentState = Wait;
        } else {
	        snprintf(introBuffer, sizeof(introBuffer), "MONTH %d", currentMonth);
        }
		
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
                deck[shop[5 - cardsToStock]] = false;
                cardsToStock--;
            }
        }
         
        shopStocked = true;
    }

    RenderCardRow(shop, 6, 2);    
}

void RenderTurnInfoText(char introBuffer[]){
	float fontSize = 80.0f;
	float spacing = 2.0f;

	Vector2 textSize = MeasureTextEx(font, introBuffer, fontSize, spacing);

	Vector2 textPos = {sideBuffer, bottomBuffer};
	DrawTextEx(font, introBuffer, textPos, fontSize, spacing, DARKBROWN);
}

void RenderDebugText(char introBuffer[]){
    if(debug == false){
        return;
    }

	float fontSize = 40.0f;
	float spacing = 2.0f;

	Vector2 textSize = MeasureTextEx(font, introBuffer, fontSize, spacing);

	Vector2 textPos = {sideBuffer, bottomBuffer + 100};
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
	
    if ((currentPhase == Roll || currentPhase == War) && currentState == Wait){
        RenderMonthText();
    } else if (!die[0].hasBeenRolled || !die[1].hasBeenRolled) {
	    srand(time(NULL));

        for(int i = 0; i < 2; i++){
            if(!die[i].hasBeenRolled){
                die[i].hasBeenRolled = true;
                die[i].numberRolled = rand() % 6;
            } 
        } 

        if(die[0].numberRolled == die[1].numberRolled){
            currentPhase = Double;
            currentState = Wait;
        }
    }
    
    if(currentPhase == Rummage && (currentState == Choose || currentState == Return)){
        RenderRummage();
        RenderNumberLable(rummage, rummagePower + 1, 2);
    }

    if((currentPhase == Shop || currentPhase == Murder) && (currentState == Choose || currentState == Return)){
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

    if(currentPhase == Murder && currentState == Return){
        char introBuffer[64];
	    snprintf(introBuffer, sizeof(introBuffer), "Press 1-5 to return cards");
        RenderTurnInfoText(introBuffer);
    }

    if(currentPhase == Rummage && currentState == Choose){
        char introBuffer[64];
	    snprintf(introBuffer, sizeof(introBuffer), "Press 1-9 and a-i to murder card and 0 to skip");
        RenderTurnInfoText(introBuffer);
    }

    if(currentPhase == Rummage && currentState == Return){
        char introBuffer[64];
	    snprintf(introBuffer, sizeof(introBuffer), "Press 1-9 and a-i to return cards");
        RenderTurnInfoText(introBuffer);
    }
        
    char introBuffer[64];

    enum gamePhases { Roll, Rummage, Shop, Murder, War, Double};
    enum gameState { Wait, Choose, Return};
   
    char phase[16]; 
    switch(currentPhase){
        case 0:
	        snprintf(phase, sizeof(phase), "Roll");
            break;
        case 1:
	        snprintf(phase, sizeof(phase), "Rummage");
            break;
        case 2:
	        snprintf(phase, sizeof(phase), "Shop");
            break;
        case 3:
	        snprintf(phase, sizeof(phase), "Murder");
            break;
        case 4:
	        snprintf(phase, sizeof(phase), "War");
            break;
        case 5:
	        snprintf(phase, sizeof(phase), "Double");
            break;
    }

    char state[16];
    switch(currentState){
        case 0:
	        snprintf(state, sizeof(state), "Wait");
            break;
        case 1:
	        snprintf(state, sizeof(state), "Choose");
            break;
        case 2:
	        snprintf(state, sizeof(state), "Return");
            break;
    }

	snprintf(introBuffer, sizeof(introBuffer), "Phase: %s, State: %s", phase, state);
    RenderDebugText(introBuffer);

    if(currentPhase == Double && currentState == Wait){
        int maxCards = 0;
        int maxCardAmount = 0;
        for(int i = 0; i < 4; i++){
            if(maxCardAmount == hand[i].level){
                maxCards++;
            } else if (maxCardAmount < hand[i].level){
                maxCards = 1;
                maxCardAmount = hand[i].level;
            } 
        }        

        if(maxCards == 1){
            currentState = Return; 
        } else if (maxCards == 4 && maxCardAmount == 1){
            currentPhase = Roll;
            currentState = Choose; 
        } else {
            currentState = Choose;
        }
    }

    if(currentPhase == Double){
        char introBuffer[64];

        if(currentState == Return){
	        snprintf(introBuffer, sizeof(introBuffer), "DOUBLE!! Press Enter to lose highest card");
            RenderTurnInfoText(introBuffer);
        } else {
	        snprintf(introBuffer, sizeof(introBuffer), "DOUBLE!! Press 1-4 to lose highest card");
            RenderTurnInfoText(introBuffer);
           
            int highestNum = 0; 
            int highCards[5];
            for(int i = 0; i < 4; i++){
                if(hand[i].level > highestNum){
                    for(int j = 0; j < 4; j++){
                        if(i == j){
                            highCards[j] = 1;
                        } else {
                            highCards[j] = 28;
                        }
                    }
                } else if (hand[i].level == highestNum){
                    highCards[i] = 1;
                }
            }
            highCards[4] = 28;

            RenderNumberLable(highCards, 5, 0);
        }
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
    UnloadMusicStream(baseTrack);
    for(int i = 0; i < 5; i++){
        UnloadMusicStream(phaseLayer[i].track);
    }

    CloseAudioDevice();
}

int FinishGameplayScreen(void)
{
    return finishScreen;
}
