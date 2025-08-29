
#include <stdio.h> 
#include <stdint.h>
#include <stdbool.h>

#include <raylib.h>

const uint32_t GRIDSIZE = 64; // Pixesl

// Grids
const uint32_t screenWidht = 12;
const uint32_t screenHeight = 15;
const uint32_t mapWidth = 6; 
const uint32_t mapHeight = 10;
const uint32_t mapPosX = 3;
const uint32_t mapPosY = 2;
const float blockSpeed = 0.2f;

typedef struct {
    uint32_t value;
    Color color;
    bool isActive;
    float posX;
    float posY;
    float interpolation; // percent to next coordiante
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
}

void drawBlocks(Block blocks[mapWidth][mapHeight]) {
    for (uint32_t i = 0; i < mapWidth; i++) {
        for (uint32_t j = 0; j < mapHeight; j++) {
            if (!blocks[i][j].isActive) continue;
            drawBlock(&blocks[i][j]);
        }
    }
}

void updateBlocks(Block blocks[mapWidth][mapHeight]) {
    for (int32_t i = mapWidth - 1; i > -1; i--) {
        for (int32_t j = mapHeight - 1; j > -1; j--) {
            if (blocks[i][j].isActive && j + 1 < mapHeight && !blocks[i][j + 1].isActive) {
                blocks[i][j].interpolation += blockSpeed;

                Vector2 pos = lerp((Vector2U32){i, j}, (Vector2U32){i, j + 1}, blocks[i][j].interpolation);
                blocks[i][j].posX = pos.x;
                blocks[i][j].posY = pos.y;

                if (blocks[i][j].interpolation >= 1.0f) {
                    blocks[i][j].interpolation = 0.0f;
                    blocks[i][j].posX = i;
                    blocks[i][j].posY = j + 1;
                    blocks[i][j + 1] = blocks[i][j];
                    blocks[i][j] = (Block){0};
                }
            }            
        }
    }
}

int main() {

    InitWindow(screenWidht * GRIDSIZE, screenHeight * GRIDSIZE, "Mef 2048");
    SetTargetFPS(60);
    
    Block blocks[mapWidth][mapHeight];
    // Zero initen
    for (uint32_t i = 0; i < mapWidth; i++) {
        for (uint32_t j = 0; j < mapHeight; j++) {
            blocks[i][j] = (Block){0};
        }
    }
 

    while (!WindowShouldClose()) {

        // Input 
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePos = GetMousePosition();
           
            if (!(mousePos.x < mapPosX * GRIDSIZE || 
                mousePos.x > (mapPosX + mapWidth) * GRIDSIZE || 
                mousePos.y < mapPosY * GRIDSIZE || 
                mousePos.y > (mapPosY + mapHeight) * GRIDSIZE)) {

                Vector2 mapRelativeMousePos = {0};
                mapRelativeMousePos.x = mousePos.x - mapPosX * GRIDSIZE;
                mapRelativeMousePos.y = mousePos.y - mapPosY * GRIDSIZE;

                
                uint32_t mapIndexX = mapRelativeMousePos.x / GRIDSIZE;
                uint32_t mapIndexY = mapRelativeMousePos.y / GRIDSIZE;
    
                Block block = {
                    .value = 100,
                    .color = RAYWHITE,
                    .isActive = true,
                    .posX = mapIndexX,
                    .posY = mapIndexY,
                    .interpolation = 0.0f,
                };
                blocks[mapIndexX][mapIndexY] = block;

            }
        }
    
        updateBlocks(blocks);

        // Render
        BeginDrawing();

        drawMap();
        drawBlocks(blocks);
            
        EndDrawing();

    }

    CloseWindow();

    return 0;
}

