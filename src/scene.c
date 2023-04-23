#include "scene.h"
#include "raylib.h"
// Level has level data, Level_enemies, Level_items and Level_Projectiles
// Level is basically the "scene"

// Public variables
Scene_Data Scene = {0};

// Private variables
Scene_Entity *Scene_entities[BLOCKS_TOTAL]; // Remember to update this if you add more blocks to below

// Entities used in the game
// TODO: More entities. For entities and their RGB values: check README.md
Scene_Entity Scene_noneBlock = {
    .mapColor = (Color){0, 0, 0, 255},
    .type = NONE,
    .fileName = ""};

Scene_Entity Scene_startBlock = {
    .mapColor = (Color){0, 255, 0, 255},
    .type = start,
    .fileName = ""};

Scene_Entity Scene_endBlock = {
    .mapColor = (Color){0, 0, 255, 255},
    .type = end,
    .fileName = ""};

Scene_Entity Scene_wall1Block = {
    .mapColor = (Color){255, 255, 255, 255},
    .type = wall,
    .fileName = "./assets/textures/wall1.png"};

Scene_Entity Scene_wall2Block = {
    .mapColor = (Color){255, 255, 254, 255},
    .type = wall,
    .fileName = "./assets/textures/wall2.png"};

Scene_Entity Scene_actorBlock = {
    .mapColor = (Color){255, 0, 0, 255},
    .type = actor,
    .fileName = "./assets/models/enemy.m3d"};

// Private functions
void Scene_PlaceBlocks(Texture2D sceneCubicMap, Color *sceneMapPixels);
void Scene_AllocateMeshData(Mesh *mesh, int triangleCount);
void Scene_SetBlockTypes(void);
void Scene_UpdateProjectiles(void);
void Scene_AddEntityToScene(Scene_Entity *entity, float mx, float my, int id);

Camera Scene_Initialize(void)
{
    Scene_Build();
    return Player_InitializeCamera(Scene.startPosition.x, Scene.startPosition.z);
}

// TODO: Add integer so you can select which level to load
//       Load textures from file, instead of being built into EXE
//
void Scene_Build(void)
{
    // Initialize block types
    Scene_SetBlockTypes();

    // Load level cubicmap image (RAM)
    const Image sceneImageMap = LoadImage("./assets/level1/level.png");
    const Texture2D sceneCubicMap = LoadTextureFromImage(sceneImageMap);

    // Get map image data to be used for collision detection
    Scene_PlaceBlocks(sceneCubicMap, LoadImageColors(sceneImageMap));

    // Unload image from RAM
    UnloadImage(sceneImageMap);
}

void Scene_PlaceBlocks(Texture2D sceneCubicMap, Color *sceneMapPixels)
{
    // Place all Level_items based on their colors

    const float mapPosZ = (float)sceneCubicMap.height;
    const float mapPosX = (float)sceneCubicMap.width;
    const Texture2D ceilingTexture = LoadTexture("./assets/textures/ceiling1.png"); // TODO: Load texture from config
    const Texture2D floorTexture = LoadTexture("./assets/textures/floor1.png");     // TODO: Load texture from config
    Scene.ceilingPlane = LoadModelFromMesh(Scene_MakeCustomPlaneMesh(mapPosZ, mapPosX, 1.0f));
    Scene.floorPlane = LoadModelFromMesh(Scene_MakeCustomPlaneMesh(mapPosZ, mapPosX, 1.0f));

    Scene.ceilingPlane.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = ceilingTexture;
    Scene.floorPlane.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = floorTexture;

    Scene.position = (Vector3){-mapPosX / 2, 0.5f, -mapPosZ / 2};
    Scene.size = sceneCubicMap.height * sceneCubicMap.width;

    Scene.blocks = calloc(Scene.size, sizeof(Scene_BlockData));
    Scene.actors = calloc(Scene.size, sizeof(Actor_Data));
    Scene.projectiles = calloc(MAX_PROJECTILE_AMOUNT, sizeof(Projectile));

    for (int y = 0; y < sceneCubicMap.height; y++)
    {
        for (int x = 0; x < sceneCubicMap.width; x++)
        {

            const float mx = Scene.position.x - 0.5f + x * 1.0f;
            const float my = Scene.position.z - 0.5f + y * 1.0f;
            const int id = y * sceneCubicMap.width + x;

            const Color pixelColor = Utilities_GetLevelPixelColor(sceneMapPixels, x, sceneCubicMap.width, y);

            for (int b = 0; b < BLOCKS_TOTAL; b++)
            {
                if (Utilities_CompareColors(pixelColor, Scene_entities[b]->mapColor))
                {
                    Scene_AddEntityToScene(Scene_entities[b], mx, my, id);
                }
            }
        }
    }

    printf("Level has total %d blocks \n", Scene.size);
}

void Scene_Update(void)
{
    if (!Game_isStarted)
    {
        return;
    }

    DrawModel(Scene.floorPlane, (Vector3){Scene.position.x, 0.0f, Scene.position.z}, 1.0f, WHITE);
    DrawModelEx(Scene.ceilingPlane,
                (Vector3){Scene.position.x, 1.0f, -Scene.position.z},
                (Vector3){-1.0f, 0.0f, 0.0f},
                180.0f,
                (Vector3){1.0f, 1.0f, 1.0f},
                WHITE);

    for (int i = 0; i < Scene.size; i++)
    {
        Scene_BlockData *data = &Scene.blocks[i];
        Actor_Data *actor = &Scene.actors[i];
        if (data != NULL && data->id != 0)
        {
            DrawModel(data->model, data->position, 1.0f, WHITE);
        }

        if (actor != NULL && actor->id != 0)
        {
            // if Level_enemies[i] has nothing dont do anything
            Actor_Update(actor);
        }
    }
    Scene_UpdateProjectiles();
}

void Scene_UpdateProjectiles(void)
{
    for (int i = 0; i < MAX_PROJECTILE_AMOUNT; i++)
    {
        Projectile *projectile = &Scene.projectiles[i];
        if (projectile != NULL)
        {
            Projectile_Update(projectile);
        }
    }
}

bool Scene_CheckCollision(Vector3 entityPos, Vector3 entitySize, int entityId)
{
    const BoundingBox entityBox = Utilities_MakeBoundingBox(entityPos, entitySize);

    for (int i = 0; i < Scene.size; i++)
    {
        // Level blocks

        // Player and walls/enemies
        if (CheckCollisionBoxes(entityBox, Scene.blocks[i].boundingBox))
        {
            return true;
        }
        // Actor and wall/other enemies
        // Actors ignore themselves so they dont collide to themselve. Actors also ignore their
        // own projectiles
        else if (CheckCollisionBoxes(entityBox, Scene.actors[i].boundingBox) && Scene.actors[i].id != entityId)
        {
            return true;
        }
    }
    return false;
}

Mesh Scene_MakeCustomPlaneMesh(float height, float width, float textureSize)
{
    // X width, Z height
    Mesh mesh = {0};
    Scene_AllocateMeshData(&mesh, 2);
    // clang-format off
    float vertices[] = {
        0,     0, 0,
        width, 0, height,
        width, 0, 0,
        0,     0, 0,
        0,     0, height,
        width, 0, height
    };

    float normals[] = {
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,
        0, 1, 0
    };

    float texcoords[] = {
        0, 0,
        width / textureSize, height / textureSize,
        width / textureSize, 0,
        0, 0,
        0, height / textureSize,
        width / textureSize, height / textureSize
    };
    // clang-format on

    mesh.vertices = vertices;
    mesh.normals = normals;
    mesh.texcoords = texcoords;

    UploadMesh(&mesh, false);

    return mesh;
}

void Scene_AllocateMeshData(Mesh *mesh, int triangleCount)
{
    mesh->vertexCount = triangleCount * 3;
    mesh->triangleCount = triangleCount;

    mesh->vertices = (float *)MemAlloc(mesh->vertexCount * 3 * sizeof(float));
    mesh->texcoords = (float *)MemAlloc(mesh->vertexCount * 2 * sizeof(float));
    mesh->normals = (float *)MemAlloc(mesh->vertexCount * 3 * sizeof(float));
}

void Scene_SetBlockTypes(void)
{
    Scene_entities[0] = &Scene_noneBlock;
    Scene_entities[1] = &Scene_startBlock;
    Scene_entities[2] = &Scene_endBlock;
    Scene_entities[3] = &Scene_wall1Block;
    Scene_entities[4] = &Scene_wall2Block;
    Scene_entities[5] = &Scene_actorBlock;
}

void Scene_AddEntityToScene(Scene_Entity *entity, float mx, float my, int id)
{
    if (entity->type == wall)
    {
        Image textureImage = LoadImage(entity->fileName);
        // The image has to be flipped since its loaded upside down
        ImageFlipVertical(&textureImage);
        const Texture2D texture = LoadTextureFromImage(textureImage);
        // Set map diffuse texture
        const Mesh cube = GenMeshCube(1.0f, 1.0f, 1.0f);
        Model cubeModel = LoadModelFromMesh(cube);
        cubeModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;

        Scene.blocks[id].model = cubeModel;
        Scene.blocks[id].position = (Vector3){mx, Scene.position.y, my};
        Scene.blocks[id].id = WALL_MODEL_ID;
        Scene.blocks[id].size = (Vector3){1.0f, 1.0f, 1.0f};
        Scene.blocks[id].boundingBox = Utilities_MakeBoundingBox((Vector3){mx, Scene.position.y, my}, (Vector3){1.0f, 1.0f, 1.0f});
    }

    else if (entity->type == start)
    {
        Scene.startPosition = (Vector3){mx, 0.0f, my};
    }

    else if (entity->type == end)
    {
        Scene.endPosition = (Vector3){mx, 0.0f, my};
    }

    else if (entity->type == actor)
    {
        Scene.actors[id] = Actor_Add(mx, my, id, entity->fileName);
    }
}
