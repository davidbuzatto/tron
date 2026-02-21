/**
 * @file ResourceManager.c
 * @author Prof. Dr. David Buzatto
 * @brief ResourceManager implementation.
 * 
 * @copyright Copyright (c) 2025
 */
#include <stdio.h>
#include <stdlib.h>

#include "ResourceManager.h"
#include "raylib/raylib.h"

ResourceManager rm = { 0 };

void loadResourcesResourceManager( void ) {
    rm.playerShader = LoadShader( NULL, "resources/shaders/playerShader.fs" );
}

void unloadResourcesResourceManager( void ) {
    UnloadShader( rm.playerShader );
}