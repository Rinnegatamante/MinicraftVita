#include <vitasdk.h>
#include <vita2d.h>
#include <string.h> 
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>

#include "Globals.h"
#include "Render.h"
#include "MapGen.h"
#include "Menu.h"
#include "texturepack.h"

extern void vita2d_texture_set_pixel(vita2d_texture* t, int w, int h, unsigned int clr);
extern uint32_t vita2d_texture_get_pixel(vita2d_texture* t, int x, int y);

void initMiniMap(bool loadUpWorld) {
	int i, x, y;
	for (i = 0; i < 5; ++i) {
		for (x = 0; x < 128; ++x) {
			for (y = 0; y < 128; ++y) {

				if (!loadUpWorld) { // generate stairs up when making a new world.
					switch (map[i][x + y * 128]) {
					case TILE_STAIRS_DOWN:
						map[i + 1][x + y * 128] = TILE_STAIRS_UP;
						if (i == 0) {
							map[i + 1][(x + 1) + y * 128] = TILE_HARDROCK;
							map[i + 1][x + (y + 1) * 128] = TILE_HARDROCK;
							map[i + 1][(x - 1) + y * 128] = TILE_HARDROCK;
							map[i + 1][x + (y - 1) * 128] = TILE_HARDROCK;
							map[i + 1][(x + 1) + (y + 1) * 128] = TILE_HARDROCK;
							map[i + 1][(x - 1) + (y - 1) * 128] = TILE_HARDROCK;
							map[i + 1][(x - 1) + (y + 1) * 128] = TILE_HARDROCK;
							map[i + 1][(x + 1) + (y - 1) * 128] = TILE_HARDROCK;
						} else {
							map[i + 1][(x + 1) + y * 128] = TILE_DIRT;
							map[i + 1][x + (y + 1) * 128] = TILE_DIRT;
							map[i + 1][(x - 1) + y * 128] = TILE_DIRT;
							map[i + 1][x + (y - 1) * 128] = TILE_DIRT;
							map[i + 1][(x + 1) + (y + 1) * 128] = TILE_DIRT;
							map[i + 1][(x - 1) + (y - 1) * 128] = TILE_DIRT;
							map[i + 1][(x - 1) + (y + 1) * 128] = TILE_DIRT;
							map[i + 1][(x + 1) + (y - 1) * 128] = TILE_DIRT;
						}
					}
				}

				/* Minimaps */
				vita2d_texture_set_pixel(minimap[i], x, y, getTileColor(map[i][x + y * 128]));
			}
		}
	}
}

void initNewMap() {
	newSeed();
	createAndValidateSkyMap(128, 128, map[0], data[0]);
	createAndValidateTopMap(128, 128, map[1], data[1]);
	createAndValidateUndergroundMap(128, 128, 1, map[2], data[2]);
	createAndValidateUndergroundMap(128, 128, 2, map[3], data[3]);
	createAndValidateUndergroundMap(128, 128, 3, map[4], data[4]);
}

void setupGame(bool loadUpWorld) {
	currentLevel = 1;

	// Reset entity manager.
	memset(&eManager, 0, sizeof(eManager));
	vita2d_set_clear_color(RGBA8(0x82, 0x6D, 0x6C, 0xFF));

	if (!loadUpWorld) {
		initNewMap();
		initPlayer();
		airWizardHealthDisplay = 2000;
		int i;
		for (i = 0; i < 5; ++i) {
			trySpawn(500, i);
		}
		addEntityToList(newAirWizardEntity(630, 820, 0), &eManager);
		player.p.hasWonSaved = false;
	} else {
		initPlayer();
		loadWorld(currentFileName, &eManager, &player, (uint8_t*) map, (uint8_t*) data);
	}

	initMiniMap(loadUpWorld);
	shouldRenderMap = false;
	mScrollX = 0;
	mScrollY = 0;
	zoomLevel = 2;
    sprintf(mapText,"x%d",zoomLevel);
	initGame = 0;
}

int xscr = 0, yscr = 0;
void tick() {
	if (player.p.isDead) {
		if (player.p.endTimer < 1) {
			currentMenu = MENU_LOSE;
		}
		--player.p.endTimer;
		return;
	} else if (player.p.hasWon) {
		if (player.p.endTimer < 1) {
			currentMenu = MENU_WIN;
		}
		--player.p.endTimer;
		return;
	}
	
	tickTouchMap();

	int i;
	for (i = 0; i < 324; ++i) {
		int xx = rand() & 127;
		int yy = rand() & 127;
		tickTile(xx, yy);
	}
	tickPlayer();
	xscr = player.x - 100;
	yscr = player.y - 56;
	if (xscr < 16)
		xscr = 16;
	else if (xscr > 1832)
		xscr = 1832;
	if (yscr < 16)
		yscr = 16;
	else if (yscr > 1912)
		yscr = 1912;

	for (i = 0; i < eManager.lastSlot[currentLevel]; ++i) {
		Entity * e = &eManager.entities[currentLevel][i];
		if ((e->type != ENTITY_ZOMBIE && e->type != ENTITY_SLIME)
				|| (e->x > player.x - 160 && e->y > player.y - 125
						&& e->x < player.x + 160 && e->y < player.y + 125))
			tickEntity(e);
	}

}

void clearScreen(int* data, uint8_t fill, int size) {
	int i;
	for (i = 0; i < size / 4; ++i)
		data[i] = 0xFF000000;
}

char debugText[34];
char bossHealthText[34];
vita2d_texture* fbo = NULL;
int main() {
	FILE * file;
	shouldRenderDebug = true;
	if ((file = fopen("ux0:/data/Minicraft/settings.bin", "r"))) {
        fread(&shouldRenderDebug,sizeof(bool),1,file);
        fread(&shouldSpeedup,sizeof(bool),1,file);
        scePowerSetArmClockFrequency(shouldSpeedup ? 444 : 333);
		fclose(file);
	}
    
	sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, 1);
	vita2d_init();
	noItem = newItem(ITEM_NULL, 0);
	
	currentMenu = MENU_TITLE;
	currentSelection = 0;
	quitGame = false;

	icons = vita2d_load_PNG_buffer(icons2_png);
	font = vita2d_load_PNG_buffer(Font_png);
	bottombg = vita2d_load_PNG_buffer(bottombg_png);
	
	dirtColor[0] = SWAP_UINT32(vita2d_texture_get_pixel(icons, 16, 0)); 
	dirtColor[1] = SWAP_UINT32(vita2d_texture_get_pixel(icons, 16, 1)); 
	dirtColor[2] = SWAP_UINT32(vita2d_texture_get_pixel(icons, 16, 2)); 
	dirtColor[3] = SWAP_UINT32(vita2d_texture_get_pixel(icons, 16, 3)); 
	dirtColor[4] = SWAP_UINT32(vita2d_texture_get_pixel(icons, 16, 4)); 
	
	initSound();
	loadSound(&snd_playerHurt, "ux0:/data/Minicraft/resources/playerhurt.raw");
	loadSound(&snd_playerDeath, "ux0:/data/Minicraft/resources/playerdeath.raw");
	loadSound(&snd_monsterHurt, "ux0:/data/Minicraft/resources/monsterhurt.raw");
	loadSound(&snd_test, "ux0:/data/Minicraft/resources/test.raw");
	loadSound(&snd_pickup, "ux0:/data/Minicraft/resources/pickup.raw");
	loadSound(&snd_bossdeath, "ux0:/data/Minicraft/resources/bossdeath.raw");
	loadSound(&snd_craft, "ux0:/data/Minicraft/resources/craft.raw");
	
	bakeLights();
	
	int i;
	for (i = 0; i < 5; ++i) {
		minimap[i] = vita2d_create_empty_texture(128, 128);
	}
	
	vita2d_set_vblank_wait(true);

	vita2d_set_clear_color(0xFF000000);

	k_up.input = SCE_CTRL_UP;
	k_down.input = SCE_CTRL_DOWN;
	k_left.input = SCE_CTRL_LEFT;
	k_right.input = SCE_CTRL_RIGHT;
	k_attack.input = SCE_CTRL_CROSS | SCE_CTRL_CIRCLE | SCE_CTRL_LTRIGGER;
	k_menu.input = SCE_CTRL_TRIANGLE | SCE_CTRL_SQUARE | SCE_CTRL_RTRIGGER;
	k_pause.input = SCE_CTRL_START;
	k_accept.input = SCE_CTRL_CROSS;
	k_decline.input = SCE_CTRL_TRIANGLE;
	k_delete.input = SCE_CTRL_SQUARE;
	k_menuNext.input = SCE_CTRL_RTRIGGER;
	k_menuPrev.input = SCE_CTRL_LTRIGGER;

	if ((file = fopen("ux0:/data/Minicraft/btnSave.bin", "rb"))) {
		fread(&k_up.input, sizeof(int), 1, file);
		fread(&k_down.input, sizeof(int), 1, file);
		fread(&k_left.input, sizeof(int), 1, file);
		fread(&k_right.input, sizeof(int), 1, file);
		fread(&k_attack.input, sizeof(int), 1, file);
		fread(&k_menu.input, sizeof(int), 1, file);
		fread(&k_pause.input, sizeof(int), 1, file);
		fread(&k_accept.input, sizeof(int), 1, file);
		fread(&k_decline.input, sizeof(int), 1, file);
		fread(&k_delete.input, sizeof(int), 1, file);
		fread(&k_menuNext.input, sizeof(int), 1, file);
		fread(&k_menuPrev.input, sizeof(int), 1, file);
		fclose(file);
	}
	
	if ((file = fopen("ux0:/data/Minicraft/lastTP.bin", "r"))) {
		char fnbuf[256];
		fgets(fnbuf, 256, file); // get directory to texturepack
		loadTexturePack(fnbuf);   
		fclose(file);
	}

	tickCount = 0;
	initRecipes();
	defineTables();
	fbo = vita2d_create_empty_texture(960,544);
	
	SceCtrlData pad, oldpad;
	for (;;) {
		++tickCount;
		sceCtrlPeekBufferPositive(0, &pad, 1);
		
		tickKeys(pad.buttons, pad.buttons & (~oldpad.buttons));
		
		if (quitGame) break;

		if (initGame > 0) setupGame(initGame == 1 ? true : false);
		
		if (currentMenu == 0) {
			tick();
			
			vita2d_start_drawing_advanced(fbo, VITA_2D_RESET_POOL | VITA_2D_SCENE_FRAGMENT_SET_DEPENDENCY);
			
			offsetX = xscr;
			offsetY = yscr;
			vita2d_draw_rectangle(0, 0, 960, 544, 0xFF0C0C0C); //RGBA8(12, 12, 12, 255)); //You might think "real" black would be better, but it actually looks better that way
			renderLightsToStencil();

			renderBackground(xscr, yscr);
			renderEntities(player.x, player.y, &eManager);
			renderPlayer();
			
			resetStencilStuff();
			offsetX = 0;
			offsetY = 0;
			
			if(shouldRenderDebug){
			    sprintf(fpsstr, " X:%d, Y:%d, E:%d", player.x, player.y, eManager.lastSlot[currentLevel]);
			    drawText(fpsstr, 2, 5);
            }
			renderGui();
            vita2d_end_drawing();
			vita2d_start_drawing_advanced(NULL, VITA_2D_SCENE_VERTEX_WAIT_FOR_DEPENDENCY);
			vita2d_draw_texture_scale(fbo,0,0,2.4,2.2667);
			vita2d_end_drawing();
			
		} else {
			tickMenu(currentMenu);
			renderMenu(currentMenu, xscr, yscr);
		}
		
		vita2d_swap_buffers();
		oldpad = pad;
	}

	/*freeRecipes();

	freeLightBakes();
	vita2d_free_texture(icons);
	vita2d_free_texture(minimap[0]);
	vita2d_free_texture(minimap[1]);
	vita2d_free_texture(minimap[2]);
	vita2d_free_texture(minimap[3]);
	vita2d_free_texture(minimap[4]);
	freeSounds();
	//csndExit();
	vita2d_fini();*/
	return 0;
}
