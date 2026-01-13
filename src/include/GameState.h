#pragma once

typedef enum GameState {
    GAME_STATE_START,
    GAME_STATE_STOPPED,
    GAME_STATE_RUNNING,
    GAME_STATE_P1_WON,
    GAME_STATE_P2_WON,
    GAME_STATE_DRAW
} GameState;