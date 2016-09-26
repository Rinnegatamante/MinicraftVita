#pragma once
#include <stdio.h>
#include <string.h>
#include "Entity.h"

void saveCurrentWorld(char * filename, EntityManager * eManager, Entity * player,  uint8_t * map, uint8_t * mapData);
int loadWorld(char * filename, EntityManager * eManager, Entity * player, uint8_t * map, uint8_t * mapData);
