//
// Created by akseli on 12/06/22.
//

#include "game.h"
#include "weapon.h"

// Shared variables
Camera playerCamera;
bool Game_initialized;

// Prototypes
void Game_HudUpdate();
void Game_MenuUpdate();

// Initialization
void Game_Initialize()
{
    Game_initialized = false;
    // Initialize game
    Settings_Initialize();

    Level_Build();
    // Add player camera
    Vector3 startPos = Level_startPosition;
    playerCamera     = Player_InitializeCamera(startPos.x, startPos.z);

    Game_initialized = true;
}

// Main game loop
void Game_Update()
{
    BeginDrawing();

    ClearBackground(BLACK);

    BeginMode3D(playerCamera);

    if(Game_initialized)
    {
        Player_Update(&playerCamera);
        Level_Update();
    }

    EndMode3D();

    Game_HudUpdate();
    Game_MenuUpdate();

    EndDrawing();
}

// TODO: Move these to their own files
void Game_HudUpdate()
{
    Hud_Draw();
}

void Game_MenuUpdate()
{
    DisableCursor();
    // menu presses etc come here
    // Enable and disable cursor based on if menu is on or off
}