
#include <stdio.h> 
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include <raylib.h>

const uint32_t MAX_SOUNDS = 10;
const uint32_t GRIDSIZE = 64; // Pixesl

// Grids
const uint32_t screenWidht = 12;
const uint32_t screenHeight = 15;
const uint32_t mapWidth = 6; 
const uint32_t mapHeight = 10;
const uint32_t mapPosX = 3;
const uint32_t mapPosY = 2;
const float blockSpeed = 0.05f;
const uint32_t powerRange = 5;
const uint32_t powersHigherThanHighestBlock = 3;
const uint32_t timeTillNextBlock = 3; // In Seconds

typedef struct {
    uint32_t value;
    Color color;
    bool isActive;
    bool isFalling;
    bool hasCollided;
    float posX;
    float posY;
    float interpolation; // percent to next coordiante
    uint32_t id; // Unqiue for everyone
    uint32_t randId; // One of the 5 stages
} Block;

typedef struct {
    uint32_t x;
    uint32_t y;
} Vector2U32;

Vector2 mulByScalarF(Vector2U32 vec, float scalar) {
    return (Vector2){vec.x * scalar, vec.y * scalar};
}

Vector2 addVector2(Vector2 a, Vector2 b) {
    return (Vector2){a.x + b.x, a.y + b.y};
}

Vector2 lerp(Vector2U32 p1, Vector2U32 p2, float t) {
    Vector2 a = mulByScalarF(p1, 1 - t);
    Vector2 b = mulByScalarF(p2, t);
    return addVector2(a, b);
}

void toggleIndex(uint32_t* index) {
    if (*index == 0) *index = 1;
    else *index = 0;
}

Block* findBlockById(Block blocks[mapWidth][mapHeight], uint32_t id) {
    for (uint32_t i = 0; i < mapWidth; i++) {
        for (uint32_t j = 0; j < mapHeight; j++) {
            if (blocks[i][j].id == id) return &blocks[i][j];
        }
    }
    return NULL;
}

void init(Block blocks[mapWidth][mapHeight], Color colors[powerRange], Block blocksQueue[2],
          uint32_t lowestBlockPower, Texture2D* bgTexture, Texture2D* shadowTexture, Sound* soundEffects) {
    // Zero initen
    for (uint32_t i = 0; i < mapWidth; i++) {
        for (uint32_t j = 0; j < mapHeight; j++) {
            blocks[i][j] = (Block){0};
        }
    }

    // Default Color Array
    for (uint32_t i = 0; i < powerRange; i++) {
        uint32_t r = rand() % 100 + 55;
        uint32_t g = rand() % 100 + 55;
        uint32_t b = rand() % 100 + 55;
        colors[i] = (Color){r, g, b, 255};
    }

    uint32_t random = rand() % powerRange;
    Block block = {
        .value = pow(2, random + lowestBlockPower),
        .color = colors[random],
        .isActive = false,
        .posX = 0.0f,
        .posY = 0.0f,
        .interpolation = 0.0f,
        .id = random,
        .randId = random,
        .isFalling = false,
        .hasCollided = false,
    };
    blocksQueue[0] = block;

    random = rand() % powerRange;
    Block block1 = {
        .value = pow(2, random + lowestBlockPower),
        .color = colors[random],
        .isActive = false,
        .posX = 0.0f,
        .posY = 0.0f,
        .interpolation = 0.0f,
        .id = random,
        .randId = random,
        .isFalling = false,
        .hasCollided = false,
    };
    blocksQueue[1] = block1;
    
    // Texutures
    // Bg
    Image bg = LoadImage("res/images/forest.png");
    ImageColorBrightness(&bg, -5);
    ImageBlurGaussian(&bg, 15);
    *bgTexture = LoadTextureFromImage(bg);
    UnloadImage(bg);

    // Shadow Image generation 
    Image shadow = GenImageColor((mapWidth + 2) * GRIDSIZE, (mapHeight + 2) * GRIDSIZE, BLANK);
    Color blackTransparant = (Color){0, 0, 0, 200};
    ImageDrawRectangle(&shadow, GRIDSIZE, GRIDSIZE, mapWidth * GRIDSIZE, mapHeight * GRIDSIZE, blackTransparant);
    ImageBlurGaussian(&shadow, 10);
    *shadowTexture = LoadTextureFromImage(shadow);
    UnloadImage(shadow);

    // Sounds
    soundEffects[0] = LoadSound("res/sfx/boom.wav");
    for (uint32_t i = 1; i < MAX_SOUNDS; i++) soundEffects[i] = LoadSoundAlias(soundEffects[0]);

}

void spwanBlock(Block* blocksQueue, Block blocks[mapWidth][mapHeight], uint32_t* currentQueueIndex,
                uint32_t* currentId, uint32_t* blockId, Color* colors, uint32_t lowestBlockPower) {
    uint32_t mapIndexX = rand() % mapWidth;
            
    // set pos of queue block
    blocksQueue[*currentQueueIndex].isActive = true;
    blocksQueue[*currentQueueIndex].posX = mapIndexX;
    blocksQueue[*currentQueueIndex].posY = 0;
    blocksQueue[*currentQueueIndex].isFalling = true;
    *currentId = blocksQueue[*currentQueueIndex].id;
    blocks[mapIndexX][0] = blocksQueue[*currentQueueIndex];
    
    // Generate next queue block
    uint32_t random = rand() % powerRange;

    Block block = {
        .value = pow(2, random + lowestBlockPower),
        .color = colors[random],
        .isActive = false,
        .posX = 0.0f,
        .posY = 0.0f,
        .interpolation = 0.0f,
        .id = (*blockId)++,
        .randId = random,
        .isFalling = false,
        .hasCollided = false,
    };
    blocksQueue[*currentQueueIndex] = block;
    toggleIndex(currentQueueIndex);

}

void mergeBlockLR(Block* block, Block* collisionBlock, uint32_t* lowestBlockPower, uint32_t* score, uint32_t highestPower, Color* colors) {
    
    if (block->isFalling) {
        block->value *= 2;
        block->randId++;
        if (block->randId >= powerRange) {
            block->randId = 0;
        }

        if (block->value == pow(2, highestPower + powersHigherThanHighestBlock)) {
            (*lowestBlockPower)++;
        }

        block->color = colors[block->randId];
        *score += block->value;
        *collisionBlock = (Block){0};

    }
    else if (collisionBlock->isFalling) {
        collisionBlock->value *= 2;
        collisionBlock->randId++;
        if (collisionBlock->randId >= powerRange) {
            collisionBlock->randId = 0;
        }

        if (collisionBlock->value == pow(2, highestPower + powersHigherThanHighestBlock)) {
            (*lowestBlockPower)++;
        };

        collisionBlock->color = colors[collisionBlock->randId];
        *score += collisionBlock->value;
        *block = (Block){0};
    }
}

void updateBlocks(Block blocks[mapWidth][mapHeight], uint32_t highestPower, uint32_t* lowestBlockPower,
                  uint32_t* score, bool* activeFalling, uint32_t currentId, Sound* soundEffects, uint32_t* currentSound,
                  Color* colors) {
    for (int32_t i = mapWidth - 1; i > -1; i--) {
        for (int32_t j = mapHeight - 1; j > -1; j--) {

            Block* block = &blocks[i][j];
            if (!block->isActive) continue;

            // Can Fall?
            if (j + 1 < mapHeight && !blocks[i][j + 1].isActive) {
                block->isFalling = true;
                block->interpolation += blockSpeed;

                Vector2 pos = lerp((Vector2U32){i, j}, (Vector2U32){i, j + 1}, block->interpolation);
                block->posX = pos.x;
                block->posY = pos.y;

                if (block->interpolation >= 1.0f) {
                    block->interpolation = 0.0f;
                    block->posX = i;
                    block->posY = j + 1;
                    blocks[i][j + 1] = *block;
                    *block = (Block){0};
                }

            }

            // Collision
            else {
                // Bottom rank
                if (j == mapHeight - 1) {
                    block->isFalling = false;
                    if (block->id == currentId) *activeFalling = false;
                }

                // First Collision 
                if (!block->hasCollided) {
                    block->hasCollided = true;
                    PlaySound(soundEffects[*currentSound]);
                    (*currentSound)++;
                    if (*currentSound >= MAX_SOUNDS) {
                        *currentSound = 0;
                    }
                }

                // Collision Downwards
                if (j + 1 < mapHeight && blocks[i][j + 1].isActive) {
                    // Merge
                    if (block->value == blocks[i][j + 1].value) {
                        Block* collisionBlock = &blocks[i][j + 1];
                        collisionBlock->value *= 2;

                        // Get Next Color Id
                        collisionBlock->randId++;
                        if (collisionBlock->randId >= powerRange) {
                            collisionBlock->randId = 0;
                        }

                        if (collisionBlock->value == pow(2, highestPower + powersHigherThanHighestBlock)) {
                            (*lowestBlockPower)++;
                        }    

                        collisionBlock->color = colors[collisionBlock->randId];
                        *score += block->value;
                        *block = (Block){0};
                    }
                }

                // Collision Left
                else if (i - 1 > -1 && blocks[i - 1][j].isActive) {
                    Block* collisionBlock = &blocks[i - 1][j];
                    mergeBlockLR(block, collisionBlock, lowestBlockPower, score, highestPower, colors);
                }

                // Collision Right 
                else if (i + 1 < mapWidth && blocks[i + 1][j].isActive) {
                    Block* collisionBlock = &blocks[i + 1][j];
                    mergeBlockLR(block, collisionBlock, lowestBlockPower, score, highestPower, colors);
                }
            }
        }
    }
}


void drawMap() {
    uint32_t k = 0;
    for (uint32_t i = 0; i < mapWidth; i++) {
        for (uint32_t j = 0; j < mapHeight; j++) {
            Color color1 = (Color){25, 25, 25, 255};
            Color color2 = (Color){55, 55, 55, 255};
            Color useColor = k % 2 ? color1 : color2; 
            DrawRectangle((i + mapPosX) * GRIDSIZE, (j + mapPosY) * GRIDSIZE, GRIDSIZE, GRIDSIZE, useColor);
            k++;
        }
        k++;
    }
}

void drawBlock(Block* block) {
    DrawRectangle((block->posX + mapPosX) * GRIDSIZE, (block->posY + mapPosY) * GRIDSIZE, GRIDSIZE, GRIDSIZE, block->color);
    char valueStr[100];
    snprintf(valueStr, sizeof(valueStr), "%d", block->value);
    uint32_t fontSize = 25;
    int textWidth = 0;
    do {
        textWidth = MeasureText(valueStr, fontSize);
        fontSize--;
    } while (textWidth > GRIDSIZE);
    fontSize++;

    DrawText(valueStr, (block->posX + mapPosX) * GRIDSIZE + (float)GRIDSIZE / 2 - (float)textWidth / 2,
                        (block->posY + mapPosY) * GRIDSIZE + (float)GRIDSIZE / 2 - (float)fontSize / 2, fontSize, RAYWHITE);
}

void drawBlockPositionScale(Block* block, uint32_t posX, uint32_t posY, uint32_t scale) {
    DrawRectangle(posX * GRIDSIZE, posY * GRIDSIZE, GRIDSIZE * scale, GRIDSIZE * scale, block->color);
    char valueStr[100];
    snprintf(valueStr, sizeof(valueStr), "%d", block->value);
    uint32_t fontSize = 25;
    int textWidth = 0;
    do {
        textWidth = MeasureText(valueStr, fontSize);
        fontSize--;
    } while (textWidth > GRIDSIZE);
    fontSize++;

    DrawText(valueStr, posX * GRIDSIZE + ((float)GRIDSIZE / 2) * scale - (float)textWidth / 2,
                        posY * GRIDSIZE + ((float)GRIDSIZE / 2) * scale - (float)fontSize / 2, fontSize, RAYWHITE);
}


void drawBlocks(Block blocks[mapWidth][mapHeight]) {
    for (uint32_t i = 0; i < mapWidth; i++) {
        for (uint32_t j = 0; j < mapHeight; j++) {
            if (!blocks[i][j].isActive) continue;
            drawBlock(&blocks[i][j]);
        }
    }
}

void drawUi(uint32_t highestBlock, Block blocksQueue[2], uint32_t queueIndex, uint32_t score) {
    
    char highestBlockText[100];
    snprintf(highestBlockText, sizeof(highestBlockText), "Highest Block: %u", highestBlock);
    DrawText(highestBlockText, 10, 10, 35, RAYWHITE);

    // Next Blocks display
    drawBlockPositionScale(&blocksQueue[queueIndex], 8, 13, 2);
    DrawText("Next Block: ", 6 * GRIDSIZE - 25, 14 * GRIDSIZE, 25, RAYWHITE);

    // score
    char scoreText[100];
    snprintf(scoreText, sizeof(scoreText), "Score: %u", score);
    DrawText(scoreText, 10, 60, 35, RAYWHITE);
}

bool checkForLose(Block blocks[mapWidth][mapHeight]) {
    for (uint32_t i = 0; i < mapWidth; i++) {
        if (blocks[i][0].isActive && !blocks[i][0].isFalling) return true;
    } 

    return false;
}

int main() {
    
    srand(time(NULL));

    InitWindow(screenWidht * GRIDSIZE, screenHeight * GRIDSIZE, "Mef 2048");
    SetTargetFPS(60);
    InitAudioDevice();
    
    Block blocks[mapWidth][mapHeight];

    // 2 ^ lowestBlockPower == lowestBlockValue
    uint32_t lowestBlockPower = 0;
    uint32_t highestBlock = pow(2, lowestBlockPower + powerRange - 1);

    Color colors[powerRange];
    Block blocksQueue[2];
    uint32_t currentQueueIndex = 0;

    uint32_t score = 0;
    bool activeFalling = false;
    uint32_t blockId = 0;
    uint32_t currentId = 0;

    // Textures
    Texture2D bgTexture;
    Texture2D shadowTexture;

    // Sounds
    Sound soundEffects[MAX_SOUNDS];
    uint32_t currentSound = 0;

    init(blocks, colors, blocksQueue, lowestBlockPower, &bgTexture, &shadowTexture, soundEffects);

    while (!WindowShouldClose()) {

        // Input 
        if (IsKeyPressed(KEY_SPACE)) {
            PlaySound(soundEffects[currentSound]);
            currentSound++;
            if (currentSound >= MAX_SOUNDS) {
                currentSound = 0;
            }
        }
        if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) {
            for (int32_t i = 0; i < mapWidth; i++) {
                for (int32_t j = 0; j < mapHeight; j++) {
                    if (blocks[i][j].isActive && blocks[i][j].id == currentId && i - 1 > -1) {
                        blocks[i - 1][j] = blocks[i][j];
                        blocks[i][j] = (Block){0};
                    }
                }
            }
        }

        if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) {
            for (int32_t i = mapWidth - 1; i > -1; i--) {
                for (int32_t j = 0; j < mapHeight; j++) {
                    if (blocks[i][j].isActive && blocks[i][j].id == currentId && i + 1 < mapWidth) {
                        blocks[i + 1][j] = blocks[i][j];
                        blocks[i][j] = (Block){0};
                    }
                }
            }
        }

       if (IsKeyPressed(KEY_ENTER)) {
            for (int32_t i = 0; i < mapWidth; i++) {
                for (int32_t j = 0; j < mapHeight; j++) {
                    if (blocks[i][j].isActive && blocks[i][j].id == currentId) {

                    }
                }
            }
        }

        if (!activeFalling) {
            activeFalling = true;
            spwanBlock(blocksQueue, blocks, &currentQueueIndex, &currentId, &blockId, colors, lowestBlockPower);
                        
        }
    
        if (checkForLose(blocks)) {
            exit(-1);
        }

        updateBlocks(blocks, lowestBlockPower + powerRange - 1, &lowestBlockPower, &score,
                     &activeFalling, currentId, soundEffects, &currentSound, colors);

        // Render
        BeginDrawing();

        ClearBackground(BLACK);

        DrawTexture(bgTexture, 0, 0, WHITE);
        DrawTexture(shadowTexture, (mapPosX) * GRIDSIZE - 35, (mapPosY) * GRIDSIZE - 35, WHITE);
        drawMap();
        drawBlocks(blocks);
        drawUi(highestBlock, blocksQueue, currentQueueIndex, score);
            
        EndDrawing();

    }

    for (uint32_t i = 1; i < MAX_SOUNDS; i++) UnloadSoundAlias(soundEffects[i]);
    UnloadSound(soundEffects[0]);
    CloseAudioDevice();

    CloseWindow();

    return 0;
}

