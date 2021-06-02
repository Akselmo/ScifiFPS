#include"../../include/raylib.h"
#include"../main.h"
#include"level.h"
#include"../player/player.h"
#include"../../include/raymath.h"
#include<stdio.h>

//this is 128*128


//Globals

//Level
Color *levelMapPixels;
Vector3 levelMapPosition;
Texture2D levelCubicMap;
Image levelImageMap;
LevelData levelData[MAX_LEVEL_SIZE];

//Entities
Color* entityMapPixels;
Texture2D entityCubicMap;
Image entityImageMap;

Vector3 startPosition;
int levelBlockAmount;


//TODO: Add integer so you can select which level to load
//      Create map from different layers: Map layer, entity layer
void BuildLevel()
{
    // Load level cubicmap image (RAM)
    levelImageMap = LoadImage("../assets/level.png");
    levelCubicMap = LoadTextureFromImage(levelImageMap);

    // Load entity cubicmap 
    entityImageMap = LoadImage("../assets/entities.png");
    entityCubicMap = LoadTextureFromImage(entityImageMap);

    // Get map image data to be used for collision detection
    levelMapPixels = LoadImageColors(levelImageMap);
    entityMapPixels = LoadImageColors(entityImageMap);

    PlaceLevelBlocks();


    // Unload image from RAM
    UnloadImage(levelImageMap);
    UnloadImage(entityImageMap);
    PlaceAllEntities();


}

void PlaceLevelBlocks()
{

    // Place all items based on their colors
    
    float mapPosZ = (float) levelCubicMap.height;
    float mapPosX = (float) levelCubicMap.width;
    
    
    // NOTE: By default each cube is mapped to one part of texture atlas
    // Load map texture
    Texture2D texture = LoadTexture("../assets/level_texture.png");    
    // Set map diffuse texture 
    Mesh cube = GenMeshCube(1.0f,1.0f,1.0f);
    Model cubeModel = LoadModelFromMesh(cube);
    cubeModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture; 

    levelMapPosition = (Vector3){ -mapPosX/2, 0.5f, -mapPosZ/2 };

    int i = 0;
    for (int y = 0; y < levelCubicMap.height; y++)
    {
        for (int x = 0; x < levelCubicMap.width; x++)
        {

            float mx = levelMapPosition.x - 0.5f + x*1.0f;
            float my = levelMapPosition.z - 0.5f + y*1.0f;

            Rectangle rect = (Rectangle){ mx, my, 1.0f, 1.0f };

            //Find start, which is red (255,0,0)
            if (levelMapPixels[y*levelCubicMap.width + x].r == 255 &&
                levelMapPixels[y*levelCubicMap.width + x].g == 255 &&
                levelMapPixels[y*levelCubicMap.width + x].b == 255)
            {
                
                levelData[i].levelBlockModel = cubeModel;
                levelData[i].levelBlockPosition = (Vector3){mx, levelMapPosition.y, my};
                levelData[i].modelId = i;
                i++;
            }
            //TODO: 
            //Green, place goal
            //Yellow, place enemy
            //Blue, place keycard
            //Ammo and stuff??

        }
    }
    levelBlockAmount = i;
    printf("Level has total %d blocks \n", levelBlockAmount);
    
}

void PlaceAllEntities()
{
    // Place all items based on their colors
    float mapPosZ = (float) entityCubicMap.height;
    float mapPosX = (float) entityCubicMap.width;
    levelMapPosition = (Vector3){ -mapPosX/2, 0.0f, -mapPosZ/2 };

    for (int y = 0; y < entityCubicMap.height; y++)
    {
        for (int x = 0; x < entityCubicMap.width; x++)
        {

            float mx = levelMapPosition.x - 0.5f + x*1.0f;
            float my = levelMapPosition.z - 0.5f + y*1.0f;

            Rectangle rect = (Rectangle){ mx, my, 1.0f, 1.0f };

            //Find start, which is red (255,0,0)
            if (entityMapPixels[y*entityCubicMap.width + x].r == 255 &&
                entityMapPixels[y*entityCubicMap.width + x].g == 0 &&
                entityMapPixels[y*entityCubicMap.width + x].b == 0)
            {
                startPosition = (Vector3) {mx,0.0f,my};
            }
            //TODO: 
            //Green, place goal
            //Yellow, place enemy
            //Blue, place keycard
            //Ammo and stuff??
            //Remember to add up all the entities into a variable

        }
    }
}

void DrawLevel()
{

    for (int i = 0; i < MAX_LEVEL_SIZE; i++)
    {
        DrawModel(levelData[i].levelBlockModel, levelData[i].levelBlockPosition, 1.0f, WHITE);
    }
   
}

bool CheckLevelCollision(Vector3 entityPos, Vector3 entitySize)
{
    LevelData* levelData = GetLevelData();

    BoundingBox entityBox = (BoundingBox){(Vector3){ entityPos.x - entitySize.x/2,
                                                     entityPos.y - entitySize.y/2,
                                                     entityPos.z - entitySize.z/2 },
                                          (Vector3){ entityPos.x + entitySize.x/2,
                                                     entityPos.y + entitySize.y/2,
                                                     entityPos.z + entitySize.z/2 }};

    for (int i=0; i < levelBlockAmount; i++)
    {
        Vector3 wallPos = levelData[i].levelBlockPosition;
        Vector3 wallSize = {1.0f,1.0f,1.0f};
        BoundingBox levelBox = (BoundingBox){(Vector3){ wallPos.x - wallSize.x/2,
                                                        wallPos.y - wallSize.y/2,
                                                        wallPos.z - wallSize.z/2 },
                                             (Vector3){ wallPos.x + wallSize.x/2,
                                                        wallPos.y + wallSize.y/2,
                                                        wallPos.z + wallSize.z/2 }};

        DrawBoundingBox(levelBox,GREEN);

        if (CheckCollisionBoxes(entityBox,levelBox))
        {
            return true;
        } 
    }
    return false;
}

LevelData* GetLevelData()
{
    return levelData;
}

Vector3 GetMapPosition()
{
    return levelMapPosition;
}

Vector3 GetStartPosition()
{
    return startPosition;
}

int GetLevelBlockAmount()
{
    return levelBlockAmount;
}