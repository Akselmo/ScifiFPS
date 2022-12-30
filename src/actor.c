#include "actor.h"

// Private functions
bool Actor_UpdatePosition(Actor_Data* actor);
bool Actor_TestPlayerHit(Actor_Data* actor);
bool Actor_FireAtPlayer(Actor_Data* actor, float nextFire);
Ray Actor_CreateRay(Actor_Data* actor);
void Actor_PlayAnimation(Actor_Data* actor, float animationSpeed);

Actor_Data Actor_Add(const float pos_x, const float pos_z, const int id, const char* modelFileName)
{
    const Vector3 actorPosition = (Vector3) { pos_x, ACTOR_POSITION_Y, pos_z };
    const Vector3 actorRotation = Vector3Zero();
    const Vector3 actorSize     = (Vector3) { 0.25f, 0.8f, 0.25f };
    const float randomTickRate  = ((float)rand() / (float)(RAND_MAX)) * 2;

    unsigned int animationsCount = 0;

    ModelAnimation* loadedAnimations = LoadModelAnimations(modelFileName, &animationsCount);

    Animator_Animation* animations;
    animations = calloc(animationsCount, sizeof(Animator_Animation));

    Animator_Animation deathAnim = { .animation     = loadedAnimations[DEATH],
                                     .interruptable = false,
                                     .loopable      = false };

    Animator_Animation attackAnim = { .animation     = loadedAnimations[ATTACK],
                                      .interruptable = false,
                                      .loopable      = false };

    Animator_Animation idleAnim = { .animation     = loadedAnimations[IDLE],
                                    .interruptable = true,
                                    .loopable      = true };

    Animator_Animation moveAnim = { .animation     = loadedAnimations[MOVE],
                                    .interruptable = true,
                                    .loopable      = true };

    animations[DEATH]  = deathAnim;
    animations[ATTACK] = attackAnim;
    animations[IDLE]   = idleAnim;
    animations[MOVE]   = moveAnim;

    Animator_Data animator = {
        .animations       = animations,
        .animationsCount  = animationsCount,
        .currentAnimation = IDLE,
    };

    Actor_Data actor = {
        .position      = actorPosition,
        .rotation      = actorRotation,
        .size          = actorSize,
        .model         = LoadModel(modelFileName),
        .dead          = false,
        .moving        = false,
        .attacking     = false,
        .damage        = 5,
        .health        = 15,  // Check actor health balance later
        .boundingBox   = Utilities_MakeBoundingBox(actorPosition, actorSize),
        .id            = id,
        .tickRate      = randomTickRate,
        .nextTick      = -1.0f,
        .movementSpeed = ACTOR_DEFAULT_MOVEMENT_SPEED,
        .rotationSpeed = ACTOR_DEFAULT_ROTATION_SPEED,
        .fireRate      = 5.75f,
        .nextFire      = 5.75f,
    };

    actor.animator = animator;

    return actor;
}

// TODO: The animations need to be tied completely to the
// firing and moving.
// First we start playing animation, then we start doing the action related to animation

void Actor_Update(Actor_Data* actor)
{
    Actor_Draw(actor);
    if(!actor->dead)
    {
        if(actor->nextTick > 0)
        {
            actor->nextTick -= GetFrameTime();
        }
        else
        {
            actor->nextTick = actor->tickRate;
        }

        if(Actor_TestPlayerHit(actor))
        {
            if(Actor_FireAtPlayer(actor, actor->nextFire))
            {
                // TODO: instead of directly changing the animation, use an animator that handles
                //       the animation loops
                actor->animator.currentAnimation = ATTACK;
            }
            else
            {
                if(Actor_UpdatePosition(actor))
                {
                    actor->animator.currentAnimation = MOVE;
                }
                else
                {
                    actor->animator.currentAnimation = IDLE;
                }
            }
            actor->nextFire -= GetFrameTime();
        }
    }
    Actor_PlayAnimation(actor, 1.0f);
}

void Actor_Draw(Actor_Data* actor)
{
    DrawModel(actor->model, actor->position, 0.5f, WHITE);
}

Ray Actor_CreateRay(Actor_Data* actor)
{

    const BoundingBox playerBb   = Player->boundingBox;
    const Vector3 playerPosition = Player->position;
    const Vector3 v = Vector3Normalize(Vector3Subtract(actor->position, playerPosition));
    Ray rayCast     = {
            .direction = (Vector3) {-1.0f * v.x, -1.0f * v.y, -1.0f * v.z},
            .position  = actor->position
    };
    return rayCast;
}

bool Actor_TestPlayerHit(Actor_Data* actor)
{

    const Ray rayCast = Actor_CreateRay(actor);

    bool hitPlayer                   = false;
    float distance                   = 0.0f;
    float levelDistance              = INFINITY;
    float playerDistance             = INFINITY;
    const int entitiesAmount         = Scene_data.size;
    const Scene_BlockData* levelData = Scene_data.blocks;
    Scene_BlockData levelDataHit;

    for(int i = 0; i < entitiesAmount; i++)
    {
        if(levelData[i].id != 0)
        {
            Vector3 pos           = levelData[i].position;
            RayCollision hitLevel = GetRayCollisionMesh(
                rayCast, levelData[i].model.meshes[0], MatrixTranslate(pos.x, pos.y, pos.z));
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

    // Player is closer
    hitPlayer = (playerDistance < levelDistance);

    return hitPlayer;
}

// Make this boolean: moving or not
bool Actor_UpdatePosition(Actor_Data* actor)
{
    bool moving = true;
    // Move actor towards player
    const Vector3 DistanceFromPlayer = Vector3Subtract(actor->position, Player->position);
    //- Check if player can be seen (first raycast hit returns player)

    //- If in certain range from player, stop
    if(fabsf(DistanceFromPlayer.x) >= ACTOR_MAX_DISTANCE_FROM_PLAYER ||
       fabsf(DistanceFromPlayer.z) >= ACTOR_MAX_DISTANCE_FROM_PLAYER)
    {
        const Vector3 actorOldPosition = actor->position;
        const Vector3 actorNewPosition =
            (Vector3) { Player->position.x, ACTOR_POSITION_Y, Player->position.z };
        actor->position =
            Vector3Lerp(actor->position, actorNewPosition, actor->movementSpeed * GetFrameTime());
        if(Scene_CheckCollision(actor->position, actor->size, actor->id))
        {
            actor->position = actorOldPosition;
        }
    }
    else
    {
        moving = false;
    }

    actor->boundingBox = Utilities_MakeBoundingBox(actor->position, actor->size);
    return moving;
}

void Actor_TakeDamage(Actor_Data* actor, const int damageAmount)
{
    if(!actor->dead)
    {
        actor->health -= damageAmount;
        printf("actor id %d took %d damage\n", actor->id, damageAmount);
        if(actor->health <= 0)
        {
            // Dirty hack to move bounding box outside of map so it cant be collided to.
            // We want to keep actor in the memory so we can use its position to display the
            // corpse/death anim
            const Vector3 deadBoxPos         = (Vector3) { ACTOR_GRAVEYARD_POSITION,
                                                           ACTOR_GRAVEYARD_POSITION,
                                                           ACTOR_GRAVEYARD_POSITION };
            actor->boundingBox               = Utilities_MakeBoundingBox(deadBoxPos, Vector3Zero());
            actor->dead                      = true;
            actor->animator.currentAnimation = DEATH;
        }
    }
}

bool Actor_FireAtPlayer(Actor_Data* actor, float nextFire)
{

    Actor_RotateTowards(actor, Player->position);
    if(nextFire > 0)
    {
        actor->nextFire -= GetFrameTime();
        return false;
    }
    else
    {
        // Fire animation should play before we shoot projectile
        // TODO: Need to create "oneshot" animation thing that blocks all other animations until
        // its done playing
        actor->attacking                 = true;
        actor->animator.currentAnimation = ATTACK;

        Projectile_Create(
            Actor_CreateRay(actor), (Vector3) { 0.2f, 0.2f, 0.2f }, actor->damage, actor->id);
        actor->nextFire = actor->fireRate;
        return true;
    }
}

void Actor_RotateTowards(Actor_Data* actor, const Vector3 targetPosition)
{
    // Rotates the actor around Y axis
    const Vector3 diff        = Vector3Subtract(actor->position, targetPosition);
    const float y_angle       = -(atan2(diff.z, diff.x) + PI / 2.0);
    const Vector3 newRotation = (Vector3) { 0, y_angle, 0 };

    const Quaternion start =
        QuaternionFromEuler(actor->rotation.z, actor->rotation.y, actor->rotation.x);
    const Quaternion end   = QuaternionFromEuler(newRotation.z, newRotation.y, newRotation.x);
    const Quaternion slerp = QuaternionSlerp(start, end, actor->rotationSpeed * GetFrameTime());

    actor->model.transform = QuaternionToMatrix(slerp);
    actor->rotation        = newRotation;
}

// TODO: Need somekind of set animation thing
// Set animation and is it interruptable/loopable? If not, play animation once
// blocking all the other set animations until this animation has played
// If it's interruptable, just do it like below

void Actor_PlayAnimation(Actor_Data* actor, const float animationSpeed)
{

    actor->animator.animationFrame++;
    if(actor->animator.animationFrame >=
       actor->animator.animations[actor->animator.currentAnimation].animation.frameCount)
    {
        actor->animator.animationFrame = 0;
    }

    UpdateModelAnimation(actor->model,
                         actor->animator.animations[actor->animator.currentAnimation].animation,
                         actor->animator.animationFrame);

    // printf("%d / %d\n",actor->model.animationFrame, frameCount);
}
