#include "enemy.h"
#include "player.h"
#include "projectile.h"
#include "raylib.h"

// Private functions
void Enemy_UpdatePosition(Enemy_Data* enemy);
bool Enemy_TestPlayerHit(Enemy_Data* enemy);
float Enemy_FireAtPlayer(Enemy_Data* enemy, float nextFire);
Ray Enemy_CreateRay(Enemy_Data* enemy);
void Enemy_PlayAnimation(Enemy_Data* enemy, enum AnimationID animationId);

// TODO: Rotation
Enemy_Data Enemy_Add(float pos_x, float pos_y, int id)
{
    Vector3 enemyPosition = (Vector3) { pos_x, ENEMY_START_POSITION_Y, pos_y };
    Vector3 enemySize     = (Vector3) { 0.25f, 0.8f, 0.25f };
    float randomTickRate  = ((float)rand() / (float)(RAND_MAX)) * 2;
    Enemy_Model model = {};
    Enemy_Data enemy      = {
             .position    = enemyPosition,
             .size        = enemySize,
             .model       = model,
             .dead        = false,
             .damage      = 5,
             .health      = 15,  // Check enemy health balance later
             .boundingBox = Utilities_MakeBoundingBox(enemyPosition, enemySize),
             .id          = id,
             .tickRate    = randomTickRate,
             .nextTick    = -1.0f,
             .speed       = 0.01f,
             .fireRate    = 5.75f,
             .nextFire    = 10.0f,
    };
    return enemy;
}

void Enemy_Update(Enemy_Data* enemy)
{
    if(!enemy->dead)
    {
        Enemy_Draw(*enemy);
        if(enemy->nextTick > 0)
        {
            enemy->nextTick -= GetFrameTime();
        }
        else
        {
            enemy->nextTick = enemy->tickRate;
        }
        Enemy_UpdatePosition(enemy);
        enemy->nextFire -= GetFrameTime();
        enemy->nextFire = Enemy_FireAtPlayer(enemy, enemy->nextFire);
    }
}

void Enemy_Draw(Enemy_Data enemy)
{
    DrawCubeV(enemy.position, enemy.size, RED);
}

Ray Enemy_CreateRay(Enemy_Data* enemy)
{
    Ray rayCast;
    BoundingBox playerBb   = Player->boundingBox;
    Vector3 playerPosition = Player->position;
    Vector3 v              = Vector3Normalize(Vector3Subtract(enemy->position, playerPosition));
    rayCast.direction      = (Vector3) { -1.0f * v.x, -1.0f * v.y, -1.0f * v.z };
    rayCast.position       = enemy->position;
    return rayCast;
}

bool Enemy_TestPlayerHit(Enemy_Data* enemy)
{

    Ray rayCast = Enemy_CreateRay(enemy);

    bool hitPlayer        = false;
    float distance        = 0.0f;
    float levelDistance   = INFINITY;
    float playerDistance  = INFINITY;
    int entitiesAmount    = Level_mapSize;
    Level_Data* levelData = Level_data;
    Level_Data levelDataHit;

    for(int i = 0; i < entitiesAmount; i++)
    {
        if(levelData[i].modelId != 0)
        {
            Vector3 pos           = levelData[i].blockPosition;
            RayCollision hitLevel = GetRayCollisionMesh(
                rayCast, levelData[i].blockModel.meshes[0], MatrixTranslate(pos.x, pos.y, pos.z));
            if(hitLevel.hit)
            {
                if(hitLevel.distance < levelDistance)
                {
                    levelDistance = hitLevel.distance;
                    levelDataHit  = levelData[i];
                }
            }
        }
    }

    playerDistance = Vector3Length(Vector3Subtract(Player->position, rayCast.position));

    if(playerDistance < levelDistance)
    {
        // Player is closer
        hitPlayer = true;
    }
    else
    {
        // Wall/other entity is closer so we didnt hit player
        hitPlayer = false;
    }
    return hitPlayer;
}

void Enemy_UpdatePosition(Enemy_Data* enemy)
{
    // Move enemy towards player:
    //- Check if player can be seen (first raycast hit returns player)
    //- If in certain range from player, stop
    //- If cant see player, stop
    //- When stopped, fire
    enemy->speed               = ENEMY_DEFAULT_SPEED * GetFrameTime();
    Vector3 DistanceFromPlayer = Vector3Subtract(enemy->position, Player->position);
    Enemy_PlayAnimation(enemy, IDLE);
    if(Enemy_TestPlayerHit(enemy))
    {
        if(fabsf(DistanceFromPlayer.x) >= ENEMY_MAX_DISTANCE_FROM_PLAYER ||
           fabsf(DistanceFromPlayer.z) >= ENEMY_MAX_DISTANCE_FROM_PLAYER)
        {
            Vector3 enemyOldPosition = enemy->position;
            enemy->position          = Vector3Lerp(enemy->position, Player->position, enemy->speed);
            if(Level_CheckCollision(enemy->position, enemy->size, enemy->id))
            {
                enemy->position = enemyOldPosition;
                Enemy_PlayAnimation(enemy, IDLE);
                return;
            }
            Enemy_PlayAnimation(enemy, MOVE);
        }
    }
    enemy->boundingBox = Utilities_MakeBoundingBox(enemy->position, enemy->size);
}

void Enemy_TakeDamage(Enemy_Data* enemy, int damageAmount)
{
    if(!enemy->dead)
    {
        enemy->health -= damageAmount;
        Enemy_PlayAnimation(enemy, HIT);
        printf("Enemy id %d took %d damage\n", enemy->id, damageAmount);
        if(enemy->health <= 0)
        {
            Enemy_PlayAnimation(enemy, DEATH);
            // Dirty hack to move bounding box outside of map so it cant be collided to.
            // We want to keep enemy in the memory so we can use its position to display the
            // corpse/death anim
            Vector3 deadBoxPos = (Vector3) { ENEMY_GRAVEYARD_POSITION,
                                             ENEMY_GRAVEYARD_POSITION,
                                             ENEMY_GRAVEYARD_POSITION };
            enemy->boundingBox = Utilities_MakeBoundingBox(deadBoxPos, Vector3Zero());
            enemy->dead        = true;
        }
    }
}

float Enemy_FireAtPlayer(Enemy_Data* enemy, float nextFire)
{
    if(Enemy_TestPlayerHit(enemy))
    {
        if(nextFire > 0)
        {
            nextFire -= GetFrameTime();
        }
        else
        {
            // Fire animation should play before we shoot projectile
            // TODO: dont shoot before level is loaded!!
            Enemy_PlayAnimation(enemy, ATTACK);
            Projectile_Create(
                Enemy_CreateRay(enemy), (Vector3) { 0.2f, 0.2f, 0.2f }, enemy->damage, enemy->id);
            nextFire = enemy->fireRate;
        }
    }
    return nextFire;
}

void Enemy_PlayAnimation(Enemy_Data* enemy, enum AnimationID animationId)
{
    if(enemy->model.currentAnimation != animationId)
    {
        enemy->model.animationFrame = 0;
    }
    enemy->model.currentAnimation = animationId;
    enemy->model.animationFrame   = Utilities_PlayAnimation(
        enemy->model.model, &enemy->model.animations[animationId], enemy->model.animationFrame);
}