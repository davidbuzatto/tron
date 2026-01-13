#include "raylib/raylib.h"
#include "Player.h"

void updatePlayer( Player *player, int keyLeft, int keyRight, float delta ) {

    if ( IsKeyPressed( keyLeft ) ) {
        player->orientation--;
        if ( player->orientation < 0 ) {
            player->orientation = 3;
        }
    } else if ( IsKeyPressed( keyRight ) ) {
        player->orientation++;
        player->orientation %= 4;
    }

    switch ( player->orientation ) {
        case 0:
            player->vel.x = BASE_VEL;
            player->vel.y = 0;
            break;
        case 1:
            player->vel.x = 0;
            player->vel.y = BASE_VEL;
            break;
        case 2:
            player->vel.x = -BASE_VEL;
            player->vel.y = 0;
            break;
        case 3:
            player->vel.x = 0;
            player->vel.y = -BASE_VEL;
            break;
    }

    player->pos.x += player->vel.x * delta;
    player->pos.y += player->vel.y * delta;

    if ( player->pos.x < 0 ) {
        player->pos.x = GetScreenWidth();
    } else if ( player->pos.x > GetScreenWidth() ) {
        player->pos.x = 0;
    }

    if ( player->pos.y < 0 ) {
        player->pos.y = GetScreenHeight();
    } else if ( player->pos.y > GetScreenHeight() ) {
        player->pos.y = 0;
    }

}

void drawPlayer( Player *player ) {
    //DrawCircleV( player->pos, 5, player->color );
}