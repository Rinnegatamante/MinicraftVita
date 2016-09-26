#pragma once
#include "Crafting.h"
#include <stdlib.h>

// Entity types
#define ENTITY_NULL 0
#define ENTITY_ITEM 1
#define ENTITY_FURNITURE 2
#define ENTITY_ZOMBIE 3
#define ENTITY_SLIME 4
#define ENTITY_AIRWIZARD 5
#define ENTITY_SPARK 6
#define ENTITY_TEXTPARTICLE 7
#define ENTITY_SMASHPARTICLE 8
#define ENTITY_PLAYER 9

typedef struct Entity Entity;

typedef struct {
    int8_t ax;
    int8_t ay;
    uint8_t dir;
    int8_t health;
    int8_t stamina;
    int8_t staminaRecharge;
    int8_t staminaRechargeDelay;
    int8_t attackTimer;
    uint8_t spawnTrigger;
    bool isDead;
    bool hasWon;
    bool hasWonSaved;
    int8_t endTimer;
    int16_t walkDist;
    bool isCarrying;
    bool isSwimming;
    int swimTimer;
	int score;
    Inventory* inv;
    Item* activeItem;
} Player;


typedef struct {
    float xa;
    float ya;
    float za;
    float xx;
    float yy;
    float zz;
    int16_t age;
    Item item;
} EntityItem;

typedef struct {
    int16_t itemID;
    bool active;
    int8_t r; // light radius for lantern. window select for chests.
    Inventory* inv; // Points to chest inventory.
    int16_t oSel; // other selection inside the chest inv.
} EntityFurniture;

typedef struct {
    int8_t xa;
    int8_t ya;
    int16_t health;
    int8_t dir;
    int8_t lvl;
    int8_t randWalkTime;
    int8_t walkDist;
    uint32_t color;
} Zombie;

typedef struct {
    int8_t xa;
    int8_t ya;
    int16_t health;
    int8_t lvl;
    int8_t dir;
    int8_t jumpTime;
    uint32_t color;
} Slime;

typedef struct {
    int8_t xa;
    int8_t ya;
    int16_t health;
    int8_t randWalkTime;
    int8_t walkDist;
    int8_t dir;
	int attackDelay;
	int attackTime;
	int attackType;
	int8_t spriteAdjust;
} AirWizard;

typedef struct {
    Entity* parent;
    int16_t age;
    float xa;
    float ya;
    float xx;
    float yy;
} Spark;

typedef struct {
    float xa;
    float ya;
    float za;
    float xx;
    float yy;
    float zz;
    int16_t age;
    char* text;
    int color;
} TextParticleEntity;

typedef struct {
    int16_t age;
} SmashParticleEntity;

struct Entity {
    int16_t x;
    int16_t y;
    int8_t xKnockback,yKnockback;
    uint8_t xr,yr;
    uint8_t type;
    uint8_t level;
    int8_t hurtTime;
    int16_t slotNum; // Read-only. Do not mess with this.
    bool canPass;
    bool canSwim;
    union {
        Player p;
        EntityItem entityItem;
        EntityFurniture entityFurniture;
        Zombie zombie;
        Slime slime;
        AirWizard wizard;
        Spark spark;
        TextParticleEntity textParticle;
        SmashParticleEntity smashParticle;
    };
};

typedef struct {
    Entity entities[5][1000];
    Entity wizardSparks[120];
    int16_t lastSlot[5];
    Inventory invs[301];//1 for the player, 300 for chests.
    int16_t nextInv;
} EntityManager;

EntityManager eManager;
int8_t currentLevel;


double gaussrand();
Entity newItemEntity(Item item, int x, int y, int level);
Entity newFurnitureEntity(int itemID,Inventory * invPtr, int x, int y, int level);
Entity newZombieEntity(int lvl, int x, int y, int level);
Entity newSlimeEntity(int lvl, int x, int y, int level);
Entity newAirWizardEntity(int x, int y, int level);
Entity newSparkEntity(Entity* parent, float xa, float ya);
Entity newTextParticleEntity(char * str, uint32_t color, int xa, int ya, int level);
Entity newSmashParticleEntity(int xa, int ya, int level);
void addEntityToList(Entity e, EntityManager* em);
void removeEntityFromList(Entity * e,int level,EntityManager* em);



