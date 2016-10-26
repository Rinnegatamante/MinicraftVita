#pragma once
#include <vitasdk.h>
#include <vita2d.h>
#include <string.h> 
#include <ctype.h>
#include <stdlib.h>
#include "Globals.h"
#include "MapGen.h"

vita2d_texture *playerLightBake;
vita2d_texture *lanternLightBake;
int offsetX, offsetY;

void render(int32_t xp, int32_t yp, uint32_t xTile, uint32_t yTile, uint8_t bits);
void renderb(int32_t xp, int32_t yp, uint32_t xTile, uint32_t yTile, uint8_t bits, uint32_t color);
void renderr(int32_t xp, int32_t yp, uint32_t xTile, uint32_t yTile, uint8_t bits, float rotate);
void renderc(int32_t xp, int32_t yp, uint32_t xTile, uint32_t yTile, int sizeX, int sizeY,
		uint8_t bits);
void renderbc(int32_t xp, int32_t yp, uint32_t xTile, uint32_t yTile, int sizeX, int sizeY,
		uint8_t bits, uint32_t color);
void render16(int32_t xp, int32_t yp, uint32_t xTile, uint32_t yTile, uint8_t bits);
void render16c(int32_t xp, int32_t yp, uint32_t xTile, uint32_t yTile, uint8_t bits, float scaleX,
		float scaleY);
void render16b(int32_t xp, int32_t yp, uint32_t xTile, uint32_t yTile, uint8_t bits, uint32_t color);
void render16s(int32_t xp, int32_t yp, uint32_t tile, uint8_t bits, uint32_t color);

void renderTitle(int x, int y);
void renderFrame(int x1, int y1, int x2, int y2, uint32_t bgColor);
void renderTile(int i, int x, int y);
void renderBackground(int xScroll, int yScroll);
void renderMenuBackground(int xScroll, int yScroll); //Renders the darkness
void renderButtonIcon(uint32_t icon, int x, int y, float scale);
void renderButtonIconNorm(uint32_t icon, int x, int y, float scale);

void bakeLights();
void freeLightBakes();
void renderLightsToStencil();
void resetStencilStuff();
void bakeLight(vita2d_texture* texture, int x, int y, int r);
void renderLight(int x, int y, vita2d_texture* texture);

void renderGui();
void renderZoomedMap();
void renderPlayer();

void drawText(char * msg, uint32_t x, uint32_t y);
void drawSizedText(char * msg, uint32_t x, uint32_t y, float size);
void drawTextColor(char * msg, uint32_t x, uint32_t y, uint32_t color);
void drawTextColorSpecial(char * msg, uint32_t x, uint32_t y, uint32_t color, uint32_t color2);
void drawSizedTextColor(char * msg, int x, int y, float size, uint32_t color);

void renderFurniture(int itemID, int x, int y);
void renderEntity(Entity* e, int x, int y);
void renderEntities(int x, int y, EntityManager* em);

void renderRecipes(RecipeManager * r, int xo, int yo, int x1, int y1,
		int selected);
void renderItemList(Inventory * inv, int xo, int yo, int x1, int y1,
		int selected);
void renderItemWithText(Item* item, int x, int y);
void renderItemStuffWithText(int itemID, int itemCL, bool onlyOne, int x, int y);
void renderItemWithTextCentered(Item* item, int width, int y);
void renderItemIcon(int itemID, int countLevel, int x, int y);
void renderItemIcon2(int itemID, int countLevel, int x, int y, int z);

void defineTables();
