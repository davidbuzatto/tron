#pragma once

#include "raylib/raylib.h"

#define BASE_VEL 200

typedef struct Player {
    Vector2 pos;
    int cellWidth;
    Vector2 vel;
    Color color;
    int orientation; // 0: right, 1: down, 2: left, 3: up
    int score;
} Player;

void updatePlayer( Player *player, int keyLeft, int keyRight, float delta );
void drawPlayer( Player *player );