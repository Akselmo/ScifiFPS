#include "entity.h"
#include "item.h"

// Entities used in the game

Entity *Entity_list[ENTITIES_TOTAL];

Entity Entity_none = { .type = SCENE_NONE, .fileName = "" };

Entity Entity_start = { .type = SCENE_START, .fileName = "" };

Entity Entity_end = { .type = SCENE_END, .fileName = "" };

Entity Entity_wall1 = { .type = SCENE_WALL, .fileName = "./assets/textures/wall1.png" };

Entity Entity_wall2 = { .type = SCENE_WALL, .fileName = "./assets/textures/wall2.png" };

Entity Entity_enemy = { .type = SCENE_ACTOR, .fileName = "./assets/models/enemy.m3d" };

Entity Entity_item = { .type = SCENE_ITEM, .item = { .type = ITEM_HEALTH_SMALL } };

void Entity_InitList(void)
{
	// In Tiled, Entity_none is not used.
	Entity_list[0] = &Entity_none;
	// Thus, in Tiled, Entity_start id is 0.
	// NOTE: Make sure the entity id's are in sync with the position they have in this array.
	// Example: Entity_list[TiledId+1] = &Entity_at_that_id;
	Entity_list[1] = &Entity_start;
	Entity_list[2] = &Entity_end;
	Entity_list[3] = &Entity_wall1;
	Entity_list[4] = &Entity_wall2;
	Entity_list[5] = &Entity_enemy;
	Entity_list[6] = &Entity_item; // TODO: Likely have to do it's own entity per item. This is just for getting started
}
