
#include <stdio.h> 
#include <stdint.h>

#include <raylib.h>

const uint32_t GRIDSIZE = 64; // Pixesl

// Grids
const uint32_t screenWidht = 12;
const uint32_t screenHeight = 15;
const uint32_t mapWidth = 6; 
const uint32_t mapHeight = 10;
const uint32_t mapPosX = 3;
const uint32_t mapPosY = 2;

typedef struct {
    const uint32_t posX; // Relative to map
    const uint32_t posY;
    const uint32_t value;
    Color color;
} Block;

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

int main() {

    InitWindow(screenWidht * GRIDSIZE, screenHeight * GRIDSIZE, "Mef 2048");
    SetTargetFPS(60);
    
    //Block blocks[mapWidth * mapHeight];

    while (!WindowShouldClose()) {

        // Input 
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePos = GetMousePosition();
           
            if (!(mousePos.x < mapPosX * GRIDSIZE || 
                mousePos.x > (mapPosX + mapWidth) * GRIDSIZE || 
                mousePos.y < mapPosY * GRIDSIZE || 
                mousePos.y > (mapPosY + mapHeight) * GRIDSIZE)) {
                printf("Mouse Pos: %f  |  %f\n", mousePos.x, mousePos.y);
            }
        }
    

        // Render
        BeginDrawing();

        drawMap();
            
        EndDrawing();

    }

    CloseWindow();

    return 0;
}

