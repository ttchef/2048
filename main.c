
#include <stdio.h> 
#include <stdint.h>

#include <raylib.h>

const uint32_t GRIDSIZE = 64; // Pixesl

// Grids
const uint32_t screenWidht = 12;
const uint32_t screenHeight = 15;
const uint32_t mapWidth = 6; 
const uint32_t mapHeight = 10;

void drawMap(const uint32_t mapPosX, const uint32_t mapPosY) {
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

int main() {

    InitWindow(screenWidht * GRIDSIZE, screenHeight * GRIDSIZE, "Mef 2048");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {

        BeginDrawing();

        drawMap(3, 2);
    
        EndDrawing();

    }

    CloseWindow();

    return 0;
}

