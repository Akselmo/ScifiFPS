#include "settings.h"

float Settings_MouseSensitivity;
float Settings_CameraFov;

// TODO: List all input keys
// c: int Settings_KeyForward
// h: extern int Settings_KeyForward
// And so on

float GetSensitivity();
float GetFOV();

void Settings_Initialize()
{
    // TODO: Allow customization of these
    const int screenWidth  = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Artificial Rage");

    Settings_MouseSensitivity = GetSensitivity();
    Settings_CameraFov        = GetFOV();
    SetTargetFPS(60);
}

float GetSensitivity()
{
    // TODO: Get Settings_MouseSensitivity from config file
    // 0.003f is default
    return 0.3f;
}

float GetFOV()
{
    // TODO: Get FOV from config file
    return 60.0f;
}

// TODO: Set up custom keys in initialization part
// Then we dont have to feed parameters
int Settings_GetCustomInput(int key)
{
    return key;
}