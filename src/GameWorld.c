/**
 * @file GameWorld.h
 * @author Prof. Dr. David Buzatto
 * @brief GameWorld implementation.
 * 
 * @copyright Copyright (c) 2025
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "GameWorld.h"
#include "GameState.h"
#include "ResourceManager.h"

#include "raylib/raylib.h"
//#include "raylib/raymath.h"
//#define RAYGUI_IMPLEMENTATION    // to use raygui, comment these three lines.
//#include "raylib/raygui.h"       // other compilation units must only include
//#undef RAYGUI_IMPLEMENTATION     // raygui.h

#define USE_SHADER true

static int p1PrevPos;
static int p2PrevPos;
static Vector2 collisionPos = { -100, -100 };
static Color gridWhite;

void reset( GameWorld *gw );

/**
 * @brief Creates a dinamically allocated GameWorld struct instance.
 */
GameWorld* createGameWorld( void ) {

    GameWorld *gw = (GameWorld*) malloc( sizeof( GameWorld ) );

    gw->cellWidth = 5;
    gw->rows = (int) ( GetScreenHeight() / gw->cellWidth );
    gw->cols = (int) ( GetScreenWidth() / gw->cellWidth );
    gw->grid = (int*) malloc( sizeof(int) * gw->rows * gw->cols );
    gw->drawGridOutline = true;

    gw->p1.cellWidth = gw->cellWidth;
    gw->p1.color = BLUE;
    gw->p1.score = 0;

    gw->p2.cellWidth = gw->cellWidth;
    gw->p2.color = GREEN;
    gw->p2.score = 0;

    reset( gw );
    gw->state = GAME_STATE_START;

    gridWhite = Fade( WHITE, 0.0f );
    gw->glowTexture = LoadRenderTexture( GetScreenWidth(), GetScreenHeight() );

    return gw;

}

/**
 * @brief Destroys a GameWindow object and its dependecies.
 */
void destroyGameWorld( GameWorld *gw ) {
    free( gw->grid );
    UnloadRenderTexture( gw->glowTexture );
    free( gw );
}

/**
 * @brief Reads user input and updates the state of the game.
 */
void updateGameWorld( GameWorld *gw, float delta ) {

    float screenWidth = GetScreenWidth();
    float screenHeight = GetScreenHeight();
    float blurAmount = 3.0f;

    int screenWidthLoc = GetShaderLocation( rm.playerShader, "screenWidth" );
    int screenHeightLoc = GetShaderLocation( rm.playerShader, "screenHeight" );
    int blurAmountLoc = GetShaderLocation( rm.playerShader, "blurAmount" );

    SetShaderValue( rm.playerShader, screenWidthLoc, &screenWidth, SHADER_UNIFORM_FLOAT );
    SetShaderValue( rm.playerShader, screenHeightLoc, &screenHeight, SHADER_UNIFORM_FLOAT );
    SetShaderValue( rm.playerShader, blurAmountLoc, &blurAmount, SHADER_UNIFORM_FLOAT );

    if ( gw->state != GAME_STATE_RUNNING ) {
        if ( IsKeyPressed( KEY_ENTER ) ) {
            reset( gw );
        }
    }

    if ( gw->state == GAME_STATE_RUNNING ) {

        updatePlayer( &gw->p1, KEY_A, KEY_D, delta );
        updatePlayer( &gw->p2, KEY_LEFT, KEY_RIGHT, delta );

        int p1Row = (int) ( gw->p1.pos.y / gw->cellWidth ) % gw->rows;
        int p1Col = (int) ( gw->p1.pos.x / gw->cellWidth ) % gw->cols;
        int p2Row = (int) ( gw->p2.pos.y / gw->cellWidth ) % gw->rows;
        int p2Col = (int) ( gw->p2.pos.x / gw->cellWidth ) % gw->cols;

        int p1Pos = p1Row*gw->cols+p1Col;
        int p2Pos = p2Row*gw->cols+p2Col;

        bool p1Won = false;
        bool p2Won = false;

        if ( p1Pos != p1PrevPos ) {
            if ( gw->grid[p1Pos] == 0 ) {
                gw->grid[p1Pos] = 1;
            } else {
                //TraceLog( LOG_INFO, "row: %d | col: %d | pos: %d", p1Row, p1Col, p1Pos );
                //TraceLog( LOG_INFO, "value: %d", gw->grid[p1Pos] );
                p2Won = true;
            }
        }

        if ( p2Pos != p2PrevPos ) {
            if ( gw->grid[p2Pos] == 0 ) {
                gw->grid[p2Pos] = 2;
            } else {
                //TraceLog( LOG_INFO, "row: %d | col: %d | pos: %d", p2Row, p2Col, p2Pos );
                //TraceLog( LOG_INFO, "value: %d", gw->grid[p2Pos] );
                p1Won = true;
            }
        }

        if ( p1Won && !p2Won ) {
            gw->state = GAME_STATE_P1_WON;
            gw->p1.score++;
            collisionPos.x = (int) ( gw->p2.pos.x / gw->cellWidth ) * gw->cellWidth + gw->cellWidth / 2;
            collisionPos.y = (int) ( gw->p2.pos.y / gw->cellWidth ) * gw->cellWidth + gw->cellWidth / 2;
        } else if ( !p1Won && p2Won ) {
            gw->state = GAME_STATE_P2_WON;
            gw->p2.score++;
            collisionPos.x = (int) ( gw->p1.pos.x / gw->cellWidth ) * gw->cellWidth + gw->cellWidth / 2;
            collisionPos.y = (int) ( gw->p1.pos.y / gw->cellWidth ) * gw->cellWidth + gw->cellWidth / 2;
        } else if ( p1Won && p2Won ) {
            gw->state = GAME_STATE_DRAW;
            collisionPos.x = (int) ( ( gw->p1.pos.x + gw->p2.pos.x ) / 2 / gw->cellWidth ) * gw->cellWidth + gw->cellWidth / 2;
            collisionPos.y = (int) ( ( gw->p1.pos.y + gw->p2.pos.y ) / 2 / gw->cellWidth ) * gw->cellWidth + gw->cellWidth / 2;
        }

        p1PrevPos = p1Pos;
        p2PrevPos = p2Pos;

    }

}

/**
 * @brief Draws the state of the game.
 */
void drawGameWorld( GameWorld *gw ) {

    BeginTextureMode( gw->glowTexture );
    ClearBackground( BLANK ); // transparent

    for ( int i = 0; i < gw->rows; i++ ) {
        for ( int j = 0; j < gw->cols; j++ ) {
            switch ( gw->grid[i*gw->cols+j] ) {
                case 1:
                    DrawRectangle( 
                        j*gw->cellWidth, 
                        i*gw->cellWidth, 
                        gw->cellWidth, 
                        gw->cellWidth, 
                        gw->p1.color
                    );
                    break;
                case 2:
                    DrawRectangle( 
                        j*gw->cellWidth, 
                        i*gw->cellWidth, 
                        gw->cellWidth, 
                        gw->cellWidth, 
                        gw->p2.color
                    );
                    break;
            }
            
        }
    }

    EndTextureMode();

    BeginDrawing();
    ClearBackground( BLACK );

    if ( gw->drawGridOutline ) {
        float gridOffset = gw->cellWidth / 2;
        for ( int i = 1; i <= gw->rows; i++ ) {
            DrawLine( 0, i*gw->cellWidth - gridOffset, GetScreenWidth(), i*gw->cellWidth - gridOffset, gridWhite );
        }
        for ( int i = 1; i <= gw->cols; i++ ) {
            DrawLine( i*gw->cellWidth - gridOffset, 0, i*gw->cellWidth - gridOffset, GetScreenHeight(), gridWhite );
        }
    }

    drawPlayer( &gw->p1 );
    drawPlayer( &gw->p2 );

    if ( USE_SHADER ) {
        BeginShaderMode( rm.playerShader );
    }
    DrawTextureRec( 
        gw->glowTexture.texture, 
        (Rectangle) { 0, 0, GetScreenWidth(), -GetScreenHeight() }, 
        (Vector2) { 0 }, 
        WHITE
    );
    if ( USE_SHADER ) {
        EndShaderMode();
    }

    DrawCircleV( collisionPos, 20, RED );

    const char *scoreP1 = TextFormat( "%d", gw->p1.score );
    const char *scoreP2 = TextFormat( "%d", gw->p2.score );

    int wScoreP1 = MeasureText( scoreP1, 60 );
    int wScoreP2 = MeasureText( scoreP1, 60 );

    DrawText( scoreP1, GetScreenWidth() / 4 - wScoreP1 / 2, 20, 60, gw->p1.color );
    DrawText( scoreP2, GetScreenWidth() * 0.75f - wScoreP2 / 2, 20, 60, gw->p2.color );

    bool stopped = false;

    if ( gw->state == GAME_STATE_START ) {
        const char *msg = "Press <ENTER> to start!";
        int msgW = MeasureText( msg, 40 );
        DrawText( msg, GetScreenWidth() / 2 - msgW / 2, GetScreenHeight() / 2 - 20, 40, WHITE );
    } else if ( gw->state == GAME_STATE_P1_WON ) {
        const char *msg = "Player 1 Won!";
        int msgW = MeasureText( msg, 40 );
        DrawRectangle( GetScreenWidth() / 2 - msgW / 2 - 10, GetScreenHeight() / 2 - 25, msgW + 20, 45, Fade( gw->p1.color, 0.3f ) );
        DrawText( msg, GetScreenWidth() / 2 - msgW / 2, GetScreenHeight() / 2 - 20, 40, gw->p1.color );
        stopped = true;
    } else if ( gw->state == GAME_STATE_P2_WON ) {
        const char *msg = "Player 2 Won!";
        int msgW = MeasureText( msg, 40 );
        DrawRectangle( GetScreenWidth() / 2 - msgW / 2 - 10, GetScreenHeight() / 2 - 25, msgW + 20, 45, Fade( gw->p2.color, 0.3f ) );
        DrawText( msg, GetScreenWidth() / 2 - msgW / 2, GetScreenHeight() / 2 - 20, 40, gw->p2.color );
        stopped = true;
    } else if ( gw->state == GAME_STATE_DRAW ) {
        const char *msg = "DRAW!";
        int msgW = MeasureText( msg, 40 );
        DrawRectangle( GetScreenWidth() / 2 - msgW / 2 - 10, GetScreenHeight() / 2 - 25, msgW + 20, 45, Fade( ORANGE, 0.3f ) );
        DrawText( msg, GetScreenWidth() / 2 - msgW / 2, GetScreenHeight() / 2 - 20, 40, ORANGE );
        stopped = true;
    }

    if ( stopped ) {
        const char *msg = "Press <ENTER> to restart!";
        int msgW = MeasureText( msg, 20 );
        DrawText( msg, GetScreenWidth() / 2 - msgW / 2, GetScreenHeight() / 2 + 25, 20, WHITE );
    }

    EndDrawing();

}

void reset( GameWorld *gw ) {

    gw->p1.vel = (Vector2) { 0 };
    gw->p1.orientation = 1;
    gw->p1.pos = (Vector2) { 
        GetScreenWidth() / 2 - gw->cellWidth / 2,
        0
    };

    gw->p2.vel = (Vector2) { 0 };
    gw->p2.orientation = 3;
    gw->p2.pos = (Vector2) { 
        GetScreenWidth() / 2 - gw->cellWidth / 2,
        GetScreenHeight()
    };

    for ( int i = 0; i < gw->rows; i++ ) {
        for ( int j = 0; j < gw->cols; j++ ) {
            gw->grid[i*gw->cols+j] = 0;
        }
    }

    gw->state = GAME_STATE_RUNNING;
    collisionPos.x = -100;
    collisionPos.y = -100;

}