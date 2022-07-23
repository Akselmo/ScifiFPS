#pragma once
#ifndef _PROJECTILE_H_
#define _PROJECTILE_H_

#include "level.h"
#include "raylib.h"

#define PROJECTILE_TRAVEL_DISTANCE       100
#define PROJECTILE_DESTROY_AFTER_SECONDS 10

typedef struct Projectile
{
    Vector3 startPosition;
    Vector3 endPosition;
    Vector3 position;
    Vector3 size;
    Color color;
    Model model;
    BoundingBox boundingBox;
    int id;
    int damage;
    bool destroyed;
    float speed;
} Projectile;

void Projectile_Create(Ray rayCast, Vector3 size, int damage);
void Projectile_Update(Projectile* projectile);
void Projectile_Destroy(Projectile* projectile);

#endif
