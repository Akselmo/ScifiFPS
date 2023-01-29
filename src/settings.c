#include "settings.h"

// TODO: List all input keys
// c: int Settings_KeyForward
// h: extern int Settings_KeyForward
// And so on
struct Settings_Data Settings;

void Settings_Initialize(void)
{
    const char* fileName = "settings.txt";

    if(access(fileName, F_OK) == 0)
    {
        Settings = Settings_Read();
    }
    else
    {
        Settings = Settings_CreateDefault();
        Settings_Write(&Settings);
    }

    InitWindow(Settings.screenWidth, Settings.screenHeight, "Artificial Rage");

    SetTargetFPS(Settings.maxFPS);
}

Settings_Data Settings_CreateDefault(void)
{
    // clang-format off
    Settings_Data data = {
        .screenWidth        = 800,
        .screenHeight       = 600,
        .mouseSensitivity   = 0.25f,
        .cameraFOV          = 90,
        .maxFPS             = 200,
        .keyMoveForward     = KEY_W,
        .keyMoveBackward    = KEY_S,
        .keyMoveLeft        = KEY_A,
        .keyMoveRight       = KEY_D,
        .keyFire            = MOUSE_BUTTON_LEFT,
        .keyUse             = KEY_E,
        .keyJump            = KEY_SPACE,
        .keyWeaponOne       = KEY_ONE,
        .keyWeaponTwo       = KEY_TWO,
        .keyWeaponThree     = KEY_THREE,
        .keyWeaponFour      = KEY_FOUR,
        .keyWeaponFive      = KEY_FIVE
    };
    // clang format on
    // Write settings to datafile here

    return data;
}

Settings_Data Settings_Read(void)
{
    const char *fileName = "settings.txt";
    Settings_Data settings = {};

    FILE *filePointer = fopen(fileName, "r");
    if (NULL == filePointer) {
        printf("======\n");
        printf("Failed to open settings file %s \n", fileName);
        printf("Using default settings! \n");
        printf("======\n");
        return Settings_CreateDefault();
    }

    printf("======\n");
    printf("Loaded following settings \n");

    int bufferLength = 255;
    char buffer[bufferLength];

    while(fgets(buffer, bufferLength, filePointer))
    {
        char *token = strtok(buffer, " ");
        char *key;
        char *value;
        for (int i = 0; i < 2; i++)
        {
            if (i == 0)
            {
                key = token;
            }
            else if (i == 1)
            {
               value = token;
            }
            token = strtok(NULL, " ");
        }

        printf("KEY: %s\n", key);
        printf("VALUE: %f\n", atof(value));
    }

    fclose(filePointer);

    printf("======\n");

    return settings;
}

void Settings_Write(Settings_Data* settings)
{
    const char *fileName = "settings.txt";

    FILE *filePointer = fopen(fileName, "w");
    if (filePointer == NULL)
    {
        printf("======\n");
        printf("Failed to open settings file %s \n", fileName);
        printf("======\n");
    }

    fprintf(filePointer, "screenWidth %d \n", settings->screenWidth);
    fprintf(filePointer, "screenHeight %d \n", settings->screenHeight);

    fclose(filePointer);
}
