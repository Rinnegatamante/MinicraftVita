#include "Menu.h"

uint32_t vita2d_texture_get_pixel(const vita2d_texture* texture, int x, int y ){
	uint32_t* data = vita2d_texture_get_datap(texture);
	uint32_t pitch = vita2d_texture_get_stride(texture);
	return data[x+y*pitch];
}

char options[][12] = {"Start Game", "How To Play","Settings", "About", "Exit"};
char pOptions[][24] = {"Return to game", "Save Progress", "Exit to title"};
char keybOptions[][24] = {"Exit and Save", "Exit and Don't save","Reset to default"};
char setOptions[][24] = {"Rebind Buttons", "Texture packs", "Debug Text:   ", "444 Mhz Mode:   ", "Return to title"};

// Rebind buttons menu (Settings)
int keys[] = {
	SCE_CTRL_CROSS,SCE_CTRL_CIRCLE,SCE_CTRL_SQUARE,SCE_CTRL_TRIANGLE,
    0,0,0,0,
    SCE_CTRL_UP,SCE_CTRL_DOWN,SCE_CTRL_LEFT,SCE_CTRL_RIGHT,
    0,0,0,0,
    SCE_CTRL_LTRIGGER,SCE_CTRL_RTRIGGER,0,0,
    SCE_CTRL_START,SCE_CTRL_SELECT
};
int keyProp[12] = {[0 ... 11] = 0};
bool areYouSure = false;
bool areYouSureSave = false;
bool bindOpt = false;
bool left = false;
bool selBut = false;
int8_t errorBut = -1;
int8_t curSaveSel = 0;

// Load Game Menu (Start Game)
char fileNames[1000][256];
int fileScore[1000];
bool fileWin[1000];
int16_t worldFileCount = 0;
bool enteringName = false;
int8_t touchDelay = 0;
bool isTouching = false;
int touchX = 0, touchY = 0, touchW = 0, touchH = 0;
int8_t errorFileName = 0;

// Load Texturepacks Menu
char tpFileNames[1000][256];
char tpFileComment[1000][60];
int16_t tpFileCount = 0;
int8_t isLoadingTP = 0;

int16_t pauseSaveDisplayTimer = 0;

void readFiles(){
    memset(&fileNames, 0, sizeof(fileNames)); // reset fileNames
    worldFileCount = 0;
    SceUID d;
    SceIoDirent dir;
    d = sceIoDopen(".");
    if (d >= 0){
        while (sceIoDread(d, &dir) > 0) {
            if (strstr(dir.d_name, ".wld") != NULL) { // Check if filename contains ".wld"
                strncpy(fileNames[worldFileCount], dir.d_name, strlen(dir.d_name)-4);
                FILE * file = fopen(dir.d_name, "rb");
                fread(&fileScore[worldFileCount],sizeof(int), 1, file);
                fread(&fileWin[worldFileCount],sizeof(bool), 1, file);
                fclose(file);
                ++worldFileCount;
            }
        }
        sceIoDclose(d);
    }
}

void readTPFiles(){
    memset(&tpFileNames, 0, sizeof(tpFileNames)); // reset tp fileNames
    memset(&tpFileComment, 0, sizeof(tpFileComment)); // reset zip comments
    tpFileCount = 1; // 0 = default.
    SceUID d;
    SceIoDirent dir;
    d = sceIoDopen("ux0:/data/Minicraft/texturepacks/");
    if (d >= 0){
	    while (sceIoDread(d, &dir) > 0) {
            if (strstr(dir.d_name, ".zip") != NULL) { // Check if filename contains ".zip"
                strncpy(tpFileNames[tpFileCount], dir.d_name, strlen(dir.d_name)-4);
                
                char fullDirName[256];
                sprintf(fullDirName,"ux0:/data/Minicraft/texturepacks/%s",dir.d_name);
                //int err = 
                getTexturePackComment(fullDirName, tpFileComment[tpFileCount]);
                /*
                if(err > 0){
                    char errorText[10];    
                    sprintf(errorText,"err:%d",err);
                    strncpy(tpFileComment[tpFileCount], errorText, strlen(errorText));
                }*/
                ++tpFileCount;
            }
        }
        sceIoDclose(d);
    }
}

int8_t checkFileNameForErrors(){
    int length = strlen(fileNames[worldFileCount]);
    if(length < 1)return 1; // Error: Length cannot be 0.
    int i;
    bool isGood = false;
    for(i=0; i < length; ++i){ if(isalnum((int)fileNames[worldFileCount][i])) isGood = true; }
    if(!isGood) return 2; // Error: Filename must contain atleast one letter or number.
    
    SceUID d;
    SceIoDirent dir;
    d = sceIoDopen(".");
    if (d >= 0){
        while (sceIoDread(d, &dir) > 0) {
            if (strstr(dir.d_name, ".wld") != NULL) { // Check if filename contains ".wld"
                char cmprFile[256];
                strncpy(cmprFile, dir.d_name, strlen(dir.d_name)-4);
                if(strncmp(fileNames[worldFileCount],cmprFile,strlen(fileNames[worldFileCount])) == 0) return 3; // Error: Filename cannot already exist.
            }
        }
        sceIoDclose(d);
    }
    
    return 0; // No errors found!
}

void addToFileName(char * c){
    strncat(fileNames[worldFileCount], c, 1);
}

/* Keypad */
void doTouchButton(){
    int xVal = k_touch.x, yVal = k_touch.y;
    int strLength = strlen(fileNames[worldFileCount]);
    if(yVal >= 100 && yVal < 140){ // 0 to 9
        if(xVal >= 140 && xVal < 140+48){ touchX = 140; if(strLength < 24)addToFileName("1");}
        else if(xVal >= 140+48 && xVal < 140+48*2){ touchX = 140+48; if(strLength < 24)addToFileName("2");}
        else if(xVal >= 140+48*2 && xVal < 140+48*3){ touchX = 140+48*2; if(strLength < 24)addToFileName("3");}
        else if(xVal >= 140+48*3 && xVal < 140+48*4){ touchX = 140+48*3; if(strLength < 24)addToFileName("4");}
        else if(xVal >= 140+48*4 && xVal < 140+48*5){ touchX = 140+48*4; if(strLength < 24)addToFileName("5");}
        else if(xVal >= 140+48*5 && xVal < 140+48*6){ touchX = 140+48*5; if(strLength < 24)addToFileName("6");}
        else if(xVal >= 140+48*6 && xVal < 140+48*7){ touchX = 140+48*6; if(strLength < 24)addToFileName("7");}
        else if(xVal >= 140+48*7 && xVal < 140+48*8){ touchX = 140+48*7; if(strLength < 24)addToFileName("8");}
        else if(xVal >= 140+48*8 && xVal < 140+48*9){ touchX = 140+48*8; if(strLength < 24)addToFileName("9");}
        else if(xVal >= 140+48*9 && xVal < 140+48*10){ touchX = 140+48*9; if(strLength < 24)addToFileName("0");}
        else return;
        touchY = 100;
        touchW = 48;
        touchH = 40;
        touchDelay = 6;
        isTouching = true;
    } else if(yVal >= 140 && yVal < 180){ // Q to P
        if(xVal >= 140 && xVal < 140+48){ touchX = 140; if(strLength < 24)addToFileName("Q");}
        else if(xVal >= 140+48 && xVal < 140+48*2){ touchX = 140+48; if(strLength < 24)addToFileName("W");}
        else if(xVal >= 140+48*2 && xVal < 140+48*3){ touchX = 140+48*2; if(strLength < 24)addToFileName("E");}
        else if(xVal >= 140+48*3 && xVal < 140+48*4){ touchX = 140+48*3; if(strLength < 24)addToFileName("R");}
        else if(xVal >= 140+48*4 && xVal < 140+48*5){ touchX = 140+48*4; if(strLength < 24)addToFileName("T");}
        else if(xVal >= 140+48*5 && xVal < 140+48*6){ touchX = 140+48*5; if(strLength < 24)addToFileName("Y");}
        else if(xVal >= 140+48*6 && xVal < 140+48*7){ touchX = 140+48*6; if(strLength < 24)addToFileName("U");}
        else if(xVal >= 140+48*7 && xVal < 140+48*8){ touchX = 140+48*7; if(strLength < 24)addToFileName("I");}
        else if(xVal >= 140+48*8 && xVal < 140+48*9){ touchX = 140+48*8; if(strLength < 24)addToFileName("O");}
        else if(xVal >= 140+48*9 && xVal < 140+48*10){ touchX = 140+48*9; if(strLength < 24)addToFileName("P");}
        else return;
        touchY = 140;
        touchW = 48;
        touchH = 40;
        touchDelay = 6;
        isTouching = true;
    } else if(yVal >= 180 && yVal < 220){ // A to L
        if(xVal >= 140 && xVal < 140+48){ touchX = 140; if(strLength < 24)addToFileName("A");}
        else if(xVal >= 140+48 && xVal < 140+48*2){ touchX = 140+48; if(strLength < 24)addToFileName("S");}
        else if(xVal >= 140+48*2 && xVal < 140+48*3){ touchX = 140+48*2; if(strLength < 24)addToFileName("D");}
        else if(xVal >= 140+48*3 && xVal < 140+48*4){ touchX = 140+48*3; if(strLength < 24)addToFileName("F");}
        else if(xVal >= 140+48*4 && xVal < 140+48*5){ touchX = 140+48*4; if(strLength < 24)addToFileName("G");}
        else if(xVal >= 140+48*5 && xVal < 140+48*6){ touchX = 140+48*5; if(strLength < 24)addToFileName("H");}
        else if(xVal >= 140+48*6 && xVal < 140+48*7){ touchX = 140+48*6; if(strLength < 24)addToFileName("J");}
        else if(xVal >= 140+48*7 && xVal < 140+48*8){ touchX = 140+48*7; if(strLength < 24)addToFileName("K");}
        else if(xVal >= 140+48*8 && xVal < 140+48*9){ touchX = 140+48*8; if(strLength < 24)addToFileName("L");}
        else return;
        touchY = 180;
        touchW = 48;
        touchH = 40;
        touchDelay = 6;
        isTouching = true;
    } else if(yVal >= 220 && yVal < 260){ // Z to M
        if(xVal >= 140 && xVal < 140+48){ touchX = 140; if(strLength < 24)addToFileName("Z");}
        else if(xVal >= 140+48 && xVal < 140+48*2){ touchX = 140+48; if(strLength < 24)addToFileName("X");}
        else if(xVal >= 140+48*2 && xVal < 140+48*3){ touchX = 140+48*2; if(strLength < 24)addToFileName("C");}
        else if(xVal >= 140+48*3 && xVal < 140+48*4){ touchX = 140+48*3; if(strLength < 24)addToFileName("V");}
        else if(xVal >= 140+48*4 && xVal < 140+48*5){ touchX = 140+48*4; if(strLength < 24)addToFileName("B");}
        else if(xVal >= 140+48*5 && xVal < 140+48*6){ touchX = 140+48*5; if(strLength < 24)addToFileName("N");}
        else if(xVal >= 140+48*6 && xVal < 140+48*7){ touchX = 140+48*6; if(strLength < 24)addToFileName("M");}
        else return;
        touchY = 220;
        touchW = 48;
        touchH = 40;
        touchDelay = 6;
        isTouching = true;
    } else if(yVal >= 260 && yVal < 300){ // SPACE and BACKSPACE
        if(xVal >= 140 && xVal < 140+48*4){ 
            touchX = 140; 
            touchW = 48*4;
            if(strLength < 24)addToFileName("_"); // Underscores don't appear in the game.
        }
        else if(xVal >= 140+48*4 && xVal < 140+48*9){ 
            if(strLength <= 0) return;
            touchX = 140+48*4; 
            touchW = 48*5;
            fileNames[worldFileCount][strLength-1] = '\0';
        }
        else return;
        touchY = 250;
        touchH = 40;
        touchDelay = 6;
        isTouching = true;
    }
    errorFileName = 0;
}

void switchGameBut(bool left, int buttonID){
    int id;
    for(id = 0; id < 7; ++id){
        if(keyProp[id] & buttonID){
            keyProp[id] ^= buttonID; // Toggle buttonID bit
            if(left){
                int id2 = id -1;
                if (id2 < 0) return;
                keyProp[id2] ^= buttonID;
            } else { 
                int id2 = id+1;
                if (id2 > 6) return;
                keyProp[id2] ^= buttonID;
            }
            return;
        }
    }
    if(left) keyProp[6] ^= buttonID;
    else keyProp[0] ^= buttonID;
    
}
void switchMenuBut(bool left, int buttonID){
    int id;
    for(id = 0; id < 12; ++id){
        if(id > 3 && id < 7) continue;
        if(keyProp[id] & buttonID){
            keyProp[id] ^= buttonID; // Toggle buttonID bit
            if(left){
                int id2 = id - 1;
                if (id2 == 6) id2 = 3;
                if (id2 < 0) return;
                keyProp[id2] ^= buttonID;
            } else { 
                int id2 = id+1;
                if (id2 == 4) id2 = 7;
                if (id2 > 11) return;
                keyProp[id2] ^= buttonID;
            }
            return;
        }
    }
    if(left) keyProp[11] ^= buttonID;
    else keyProp[0] ^= buttonID;
}

int8_t checkPropButtons(){
    if(keyProp[0] == 0) return 0;
    if(keyProp[1] == 0) return 1;
    if(keyProp[2] == 0) return 2;
    if(keyProp[3] == 0) return 3;
    if(keyProp[4] == 0) return 4;
    if(keyProp[5] == 0) return 5;
    if(keyProp[6] == 0) return 6;
    if(keyProp[7] == 0) return 7;
    if(keyProp[8] == 0) return 8;
    if(keyProp[9] == 0) return 9;
    if(keyProp[10] == 0) return 10;
    if(keyProp[11] == 0) return 11;
    return -1;
}

Item median;
void tickMenu(int menu){
    switch(menu){
        case MENU_SETTINGS_REBIND:
        if(!bindOpt){
            if(!selBut){
		        if (k_up.clicked){ --currentSelection; if(currentSelection < 0)currentSelection=21;}
		        if (k_down.clicked){ ++currentSelection; if(currentSelection > 21)currentSelection=0;}
		        if (k_left.clicked){ left = true;}
		        if (k_right.clicked){ left = false;}
            } else {
                if (k_left.clicked){ 
                    if(left)switchGameBut(true,keys[currentSelection]);
                    else switchMenuBut(true,keys[currentSelection]);
                } else if (k_right.clicked) { 
                    if(left)switchGameBut(false,keys[currentSelection]);
                    else switchMenuBut(false,keys[currentSelection]);
                }
            }
		    
            if (k_accept.clicked) selBut = !selBut;
            if (k_decline.clicked){ 
                bindOpt = true;
                curSaveSel = 0;
            }
        } else {
		    if (k_up.clicked){ --curSaveSel; if(curSaveSel < 0)curSaveSel=2;}
		    if (k_down.clicked){ ++curSaveSel; if(curSaveSel > 2)curSaveSel=0;}
            if (k_decline.clicked){ 
                bindOpt = false;
                errorBut = -1;
            }
            if (k_accept.clicked){
                switch(curSaveSel){
                    case 0: // Exit and save  
                        if(checkPropButtons() == -1){
                            k_up.input = keyProp[0];
                            k_down.input = keyProp[1];
                            k_left.input = keyProp[2];
                            k_right.input = keyProp[3];
                            k_attack.input = keyProp[4];
                            k_menu.input = keyProp[5];
                            k_pause.input = keyProp[6];
                            k_accept.input = keyProp[7];
                            k_decline.input = keyProp[8];
                            k_delete.input = keyProp[9];
                            k_menuNext.input = keyProp[10];
                            k_menuPrev.input = keyProp[11];
                            
                            FILE *fs=fopen("ux0:/data/Minicraft/btnSave.bin","wb");
                            fwrite(keyProp,sizeof(int),12,fs);
                            fclose(fs);
                            
                            currentSelection = 0;
                            currentMenu = MENU_SETTINGS;
                            errorBut = -1;
                        } else errorBut = checkPropButtons();
                        break;
                    case 1: // Exit and DON'T save  
                        currentSelection = 0;
                        currentMenu = MENU_SETTINGS;
                        errorBut = -1;
                        break;
                    case 2: // reset defaults
	                    keyProp[0] = SCE_CTRL_UP;
	                    keyProp[1] = SCE_CTRL_DOWN;
	                    keyProp[2] = SCE_CTRL_LEFT;
	                    keyProp[3] = SCE_CTRL_RIGHT;
	                    keyProp[4] = SCE_CTRL_CROSS | SCE_CTRL_CIRCLE | SCE_CTRL_LTRIGGER;
	                    keyProp[5] = SCE_CTRL_SQUARE | SCE_CTRL_TRIANGLE | SCE_CTRL_RTRIGGER;
	                    keyProp[6] = SCE_CTRL_START;
                        keyProp[7] = SCE_CTRL_CROSS;
                        keyProp[8] = SCE_CTRL_CIRCLE;
                        keyProp[9] = SCE_CTRL_TRIANGLE;
                        keyProp[10] = SCE_CTRL_RTRIGGER;
                        keyProp[11] = SCE_CTRL_LTRIGGER;
	                    bindOpt = false;
                        errorBut = -1;
                        break;
                    }
                }
            }
        break;
        case MENU_PAUSED:
            if(!areYouSure && !areYouSureSave){
                if(pauseSaveDisplayTimer > 0) --pauseSaveDisplayTimer;
                if (k_pause.clicked) currentMenu = MENU_NONE;
		        if (k_up.clicked){ --currentSelection; if(currentSelection < 0)currentSelection=2;}
		        if (k_down.clicked){ ++currentSelection; if(currentSelection > 2)currentSelection=0;}
                if (k_accept.clicked){
                    switch(currentSelection){
                    case 0:      
                        currentMenu = MENU_NONE;
                        break;
                    case 1:
	                    areYouSureSave = true;
                        break;
                    case 2:
	                    areYouSure = true;
                        break;
                    }
                }
            } else if(areYouSureSave) {
                if (k_accept.clicked){
                    pauseSaveDisplayTimer = 60;
                    saveCurrentWorld(currentFileName, &eManager, &player, (uint8_t*)map, (uint8_t*)data);
                    areYouSureSave = false;
                    areYouSure = false;
                } else if (k_decline.clicked){
                    areYouSureSave = false;
                    areYouSure = false;
                }
            } else {
                if (k_accept.clicked){
                    areYouSure = false;
                    areYouSureSave = false;
	                vita2d_set_clear_color(0xFF000000);
                    currentSelection = 0;
                    currentMenu = MENU_TITLE;
                } else if (k_decline.clicked){
                    areYouSure = false;
                    areYouSureSave = false;
                }
            }
        break;
        case MENU_INVENTORY:
            if (k_menu.clicked || k_decline.clicked){
                 currentMenu = MENU_NONE;
                 player.p.activeItem = &noItem;
                 player.p.isCarrying = false;
            }
            if (k_accept.clicked){ // Select item from inventory
                if(player.p.inv->lastSlot!=0){
                    median = player.p.inv->items[curInvSel]; // create copy of item.
                    removeItemFromInventory(curInvSel, player.p.inv); // remove original
                    pushItemToInventoryFront(median, player.p.inv); // add copy to front
                    player.p.activeItem = &player.p.inv->items[0]; // active item = copy.
                    if(player.p.activeItem->id > 27) player.p.isCarrying = true;
                    else player.p.isCarrying = false;
                }
                currentMenu = MENU_NONE;
            }
		    if (k_up.clicked){ --curInvSel; if(curInvSel < 0)curInvSel=player.p.inv->lastSlot-1;}
		    if (k_down.clicked){ ++curInvSel; if(curInvSel > player.p.inv->lastSlot-1)curInvSel=0;}
        break;
        
        case MENU_CRAFTING:
        if (k_menu.clicked || k_decline.clicked) currentMenu = MENU_NONE;
        if (k_accept.clicked){
            int newslot = player.p.activeItem->slotNum + 1;
            if(craftItem(currentRecipes, &currentRecipes->recipes[curInvSel], player.p.inv)){
                playSound(snd_craft);
                if(player.p.activeItem != &noItem)player.p.activeItem = &player.p.inv->items[newslot];
            }
        }
		if (k_up.clicked){ --curInvSel; if(curInvSel < 0)curInvSel=currentRecipes->size-1;}
		if (k_down.clicked){ ++curInvSel; if(curInvSel > currentRecipes->size-1)curInvSel=0;}
        break;
        
        case MENU_WIN:
            if (k_accept.clicked){ 
	            vita2d_set_clear_color(0xFF000000);
                currentSelection = 0;
                currentMenu = MENU_TITLE;
                saveCurrentWorld(currentFileName, &eManager, &player, (uint8_t*)map, (uint8_t*)data);
            }
        break;
        case MENU_LOSE:
            if (k_accept.clicked){ 
	            vita2d_set_clear_color(0xFF000000);
                currentSelection = 0;
                currentMenu = MENU_TITLE;
            }
        break;
        case MENU_ABOUT:
            if (k_decline.clicked) currentMenu = MENU_TITLE;
        break;
        
        case MENU_CONTAINER:
        if (k_menu.clicked || k_decline.clicked)currentMenu = MENU_NONE;
        
        if (k_left.clicked) {
			curChestEntity->entityFurniture.r = 0;
			int tmp = curInvSel;
			curInvSel = curChestEntity->entityFurniture.oSel;
			curChestEntity->entityFurniture.oSel = tmp;
		}
		if (k_right.clicked) {
			curChestEntity->entityFurniture.r = 1;
			int tmp = curInvSel;
			curInvSel = curChestEntity->entityFurniture.oSel;
			curChestEntity->entityFurniture.oSel = tmp;
		}
		
		Inventory* i1 = curChestEntity->entityFurniture.r == 1 ? player.p.inv : curChestEntity->entityFurniture.inv;
		Inventory* i2 = curChestEntity->entityFurniture.r == 0 ? player.p.inv : curChestEntity->entityFurniture.inv;
		int len = i1->lastSlot;
		if (curInvSel < 0) curInvSel = 0;
		if (curInvSel >= len) curInvSel = len - 1;
		if (k_up.clicked) --curInvSel;
		if (k_down.clicked) ++curInvSel;
		if (len == 0) curInvSel = 0;
		if (curInvSel < 0) curInvSel += len;
		if (curInvSel >= len) curInvSel -= len;
		
		if(k_accept.clicked && len > 0){
            Item* pullItem = &i1->items[curInvSel];
            Item pushItem = newItem(pullItem->id,pullItem->countLevel);
            pushItem.chestPtr = pullItem->chestPtr;
            pushItemToInventoryFront(pushItem, i2);
            if(i2 == player.p.inv){
                int newslot = player.p.activeItem->slotNum + 1;
                player.p.activeItem = &player.p.inv->items[newslot];
            } else if(pullItem == player.p.activeItem){
                player.p.activeItem = &noItem;
            }
            removeItemFromCurrentInv(pullItem);
			if (curInvSel >= i1->lastSlot) curInvSel = i1->lastSlot - 1;
        }
		
        break;
        
        case MENU_LOADGAME:
            if(!enteringName && !areYouSure){ // World select
                if (k_decline.clicked){
                    currentMenu = MENU_TITLE;
                    currentSelection = 0;
                }
                if (k_up.clicked){ --currentSelection; if(currentSelection < 0)currentSelection = worldFileCount;}
                if (k_down.clicked){ ++currentSelection; if(currentSelection > worldFileCount)currentSelection=0;}
            
                if(k_delete.clicked){
                    if(currentSelection < worldFileCount) areYouSure = true;
                }
            
                if(k_accept.clicked){
                    if(currentSelection == worldFileCount){
                        
                        enteringName = true;
                    } else {
                        memset(&currentFileName, 0, 255); // reset currentFileName
                            sprintf(currentFileName,"%s.wld",fileNames[currentSelection]);
                            playSound(snd_test);
                            initGame = 1;
                            currentMenu = MENU_NONE;
                    }
                }
            } else if (areYouSure){
                if (k_decline.clicked || k_delete.clicked) areYouSure = false;
                if (k_accept.clicked){
                    sprintf(currentFileName,"%s.wld",fileNames[currentSelection]);
                    remove(currentFileName);
                    readFiles();
                    enteringName = false;
                    areYouSure = false;
                    memset(&currentFileName, 0, 255); // reset currentFileName
                }
            }else { // Enter new world name.
                if(k_decline.clicked) enteringName = false;
                if(k_accept.clicked && errorFileName == 0){
                    errorFileName = checkFileNameForErrors();
                    if(errorFileName == 0){ // If no errors are found with the filename, then start a new game!
                        memset(&currentFileName, 0, 255); // reset currentFileName
                        sprintf(currentFileName,"%s.wld",fileNames[worldFileCount]);
                        currentMenu = MENU_NONE;
                        playSound(snd_test);
                        initGame = 2;
                        ++worldFileCount;
                    }
                }
                if((k_touch.x != 0 || k_touch.y != 0) && touchDelay == 0){ 
                    if(!isTouching)doTouchButton(); 
                }
                else if(k_touch.x == 0 || k_touch.y == 0) isTouching = false;
                if(touchDelay > 0) --touchDelay;
            }
        break;
        case MENU_SETTINGS_TP:
            
            if (k_up.clicked){ --currentSelection; if(currentSelection < 0)currentSelection = tpFileCount-1;}
            if (k_down.clicked){ ++currentSelection; if(currentSelection > tpFileCount-1)currentSelection=0;}
            if (k_decline.clicked){
                if(isLoadingTP < 1){
                    currentMenu = MENU_SETTINGS;
                    currentSelection = 1;
                }
            }
            if (k_accept.clicked){
                
                if(currentSelection > 0){
                    isLoadingTP = 4;
                } else {
	                icons = vita2d_load_PNG_buffer(icons2_png);
	                dirtColor[0] = SWAP_UINT32(vita2d_texture_get_pixel(icons, 16, 0)); 
	                dirtColor[1] = SWAP_UINT32(vita2d_texture_get_pixel(icons, 16, 1)); 
	                dirtColor[2] = SWAP_UINT32(vita2d_texture_get_pixel(icons, 16, 2)); 
	                dirtColor[3] = SWAP_UINT32(vita2d_texture_get_pixel(icons, 16, 3)); 
	                dirtColor[4] = SWAP_UINT32(vita2d_texture_get_pixel(icons, 16, 4)); 
	                font = vita2d_load_PNG_buffer(Font_png);
	                bottombg = vita2d_load_PNG_buffer(bottombg_png);
                    currentMenu = MENU_SETTINGS;
                    currentSelection = 1;
                    remove("lastTP.bin");
                }
            }
        break;
        
        case MENU_SETTINGS:
		    if (k_up.clicked){ 
                --currentSelection; 
                if(currentSelection < 0)currentSelection=4;
            }
		    if (k_down.clicked){ 
                ++currentSelection; 
                if(currentSelection > 4)currentSelection=0;
            }
            if(k_decline.clicked){
                currentMenu = MENU_TITLE;
                currentSelection = 2;
            }
		    if(k_accept.clicked){
                switch(currentSelection){
                    case 0:
                        keyProp[0] = k_up.input;
                        keyProp[1] = k_down.input;
                        keyProp[2] = k_left.input;
                        keyProp[3] = k_right.input;
                        keyProp[4] = k_attack.input;
                        keyProp[5] = k_menu.input;
                        keyProp[6] = k_pause.input;
                        keyProp[7] = k_accept.input;
                        keyProp[8] = k_decline.input;
                        keyProp[9] = k_delete.input;
                        keyProp[10] = k_menuNext.input;
                        keyProp[11] = k_menuPrev.input;
                        left = true;
                        selBut = false;
                        bindOpt = false;
                        currentSelection = 0;
                        currentMenu = MENU_SETTINGS_REBIND;
                        break;
                    case 1:
                        readTPFiles();
                        currentMenu = MENU_SETTINGS_TP;
                        currentSelection = 0;
                        break;
                    case 2:
                        shouldRenderDebug = !shouldRenderDebug; // toggle option
                        break;
                    case 3:
                        shouldSpeedup = !shouldSpeedup; // toggle option
                        scePowerSetArmClockFrequency(shouldSpeedup ? 444 : 333);
                        break;
                    case 4:
                        if(true){
                            FILE * fset = fopen("ux0:/data/Minicraft/settings.bin","wb");
                            fwrite(&shouldRenderDebug,sizeof(bool),1,fset);
                            fwrite(&shouldSpeedup,sizeof(bool),1,fset);
                            fclose(fset);
                        }
                        currentMenu = MENU_TITLE;
                        currentSelection = 2;
                        break;
                }
            }
        break;
        case MENU_TITLE:
		    if (k_up.clicked){ --currentSelection; if(currentSelection < 0)currentSelection=4;}
		    if (k_down.clicked){ ++currentSelection; if(currentSelection > 4)currentSelection=0;}
		    
		    if(k_accept.clicked){
                switch(currentSelection){
                    case 0:
                        currentMenu = MENU_LOADGAME;
                        readFiles();
                        currentSelection = 0;
                        enteringName = false;
                        areYouSure = false;
                    break;
                    case 1:
                        sprintf(pageText,"Page: %d/%d",pageNum+1,maxPageNum+1);
                        currentMenu = MENU_TUTORIAL;
                    break;
                    case 2:
                        currentSelection = 0;
                        currentMenu = MENU_SETTINGS;
                    break;
                    case 3: 
                        currentMenu = MENU_ABOUT;
                    break;
                    case 4: 
                        quitGame = true;
                    break;
                }
                
            }
        break;
        case MENU_TUTORIAL:
            if(k_decline.clicked){ 
                currentSelection = 1;
                currentMenu = MENU_TITLE;
            }
            if(k_menuNext.clicked){
                if(pageNum < maxPageNum){ 
                    ++pageNum;
                    sprintf(pageText,"Page: %d/%d",pageNum+1,maxPageNum+1);
                }
            }
            if(k_menuPrev.clicked){
                if(pageNum > 0){
                    --pageNum;
                    sprintf(pageText,"Page: %d/%d",pageNum+1,maxPageNum+1);
                }
            }
            
        break;
    }
    
}

uint8_t opacity = 255;
bool rev = true;
char scoreText[15];

char * getButtonFunctionGame(int key){
    if(keyProp[0] & key) return "Move up";
    if(keyProp[1] & key) return "Move down";
    if(keyProp[2] & key) return "Move left";
    if(keyProp[3] & key) return "Move right";
    if(keyProp[4] & key) return "Attack";
    if(keyProp[5] & key) return "Toggle Menu";
    if(keyProp[6] & key) return "Pause";
    return "Nothing";
}
char * getButtonFunctionMenu(int key){
    if(keyProp[0] & key) return "Up";
    if(keyProp[1] & key) return "Down";
    if(keyProp[2] & key) return "Left";
    if(keyProp[3] & key) return "Right";
    if(keyProp[7] & key) return "Accept";
    if(keyProp[8] & key) return "Decline";
    if(keyProp[9] & key) return "Delete";
    if(keyProp[10] & key) return "Next";
    if(keyProp[11] & key) return "Previous";
    return "Nothing";
}


char guiText0[] = "1 2 3 4 5 6 7 8 9 0";
char guiText1[] = "Q W E R T Y U I O P";
char guiText2[] = "A S D F G H J K L";
char guiText3[] = "Z X C V B N M";
char guiText4[] = " SPACE  BACKSPACE";

extern vita2d_texture* fbo;

void renderMenu(int menu,int xscr,int yscr){
    int i = 0;
    switch(menu){
        case MENU_TUTORIAL:
		    vita2d_start_drawing();
		        renderTutorialPage(true);
		    vita2d_end_drawing();
            break;
        case MENU_SETTINGS_TP:
            offsetX = 0;offsetY = (currentSelection * 40) - 48;
		    vita2d_start_drawing();
				vita2d_clear_screen();
		        drawSizedText("Texture Packs",(715-(13*48))/2,30,4.0);
		        for(i = 0; i < tpFileCount; ++i){
                    int color = 0xFF923232;
                    char * text = tpFileNames[i];
                    char * cmtText = tpFileComment[i];
                    if(i == 0){
                        text = "Default";
                        cmtText = "Regular look of the game";
                        color = 0xFF921060;
                    }
                    if(i != currentSelection) color &= 0x7FFFFFFF; // Darken color.
                    else if(areYouSure)color = 0xFF1010DF;
                    
                    //renderFrame(1,i*5,24,(i*5)+5,color);
                    drawSizedText(text,80,180+i*30,2.0);
                    
                    drawSizedTextColor(cmtText,24,4,2.0,0xFF7FFFFF);
                }
            offsetX = 0;offsetY = 0;
                if(isLoadingTP > 0){
                    --isLoadingTP;
                    renderFrame(1,5,24,9,0xFF666666);
                    drawTextColor("Loading Texture pack...",(400-(23*12))/2,108,0xFF10FFFF);
                    if(isLoadingTP == 0){
                        char fullDirName[256];
                        sprintf(fullDirName,"ux0:/data/Minicraft/texturepacks/%s.zip",tpFileNames[currentSelection]);
                        loadedtp = loadTexturePack(fullDirName);   
                        
                        FILE * fs=fopen("ux0:/data/Minicraft/lastTP.bin","w");
                        fprintf(fs,"%s", fullDirName);
                        fclose(fs);
                        
                        currentMenu = MENU_SETTINGS;
                        currentSelection = 1; 
                    }
                }
                drawSizedText("Press    to select", 80, 300, 2.0);
                renderButtonIcon(k_accept.input & -k_accept.input, 244, 282, 1);
                drawSizedText("Press    to return", 80, 350, 2.0);
                renderButtonIcon(k_decline.input & -k_decline.input, 244, 332, 1);
		    vita2d_end_drawing();
        break;
        case MENU_LOADGAME:
		vita2d_start_drawing();
		vita2d_clear_screen();
        if(!enteringName){ // World select
            offsetX = 0;offsetY = (currentSelection * 32) - 48;
            drawSizedText("Select a file",(715-(13*48))/2,30,4.0);
            for(i = 0; i < worldFileCount + 1; ++i){
                int color = 0xFF921020;
                char * text = fileNames[i];
                if(i == worldFileCount){
                    text = "Generate New World";
                    color = 0xFF209210;
                }
                if(i != currentSelection) color &= 0xFF7F7F7F; // Darken color.
                else {
                    if(areYouSure)color = 0xFF1010DF;
                }
                
                char scoreText[24];
                sprintf(scoreText,"Score: %d",fileScore[i]);
                
                renderFrame(12,i*4,35,(i*4)+4,color);
                if(i != worldFileCount){
                    drawText(text,(690-(strlen(text)*12))/2,i*64+12);
                    drawText(scoreText,(690-(strlen(scoreText)*12))/2,i*64+32);
                } else {
                    drawText(text,(690-(strlen(text)*12))/2,i*64+24);
                }
                if(fileWin[i] && i != worldFileCount) render16(18,i*32+8,24,208,0); // Render crown
            }
            offsetX = 0;offsetY = 0;
        } else { // Enter new world name.
            drawSizedText("Enter a name",(680-(12*48))/2,30,4.0);
            drawSizedText(fileNames[worldFileCount],80, 340 ,2.0);
            
            if(errorFileName > 0){
                switch(errorFileName){// Error: Filename cannot already exist.
                    case 1: drawSizedTextColor("ERROR: Length cannot be 0!",5,315,2.0,0xFF1010AF); break;    
                    case 2: drawSizedTextColor("ERROR: You need Letters/Numbers!",5,315,2.0,0xFF1010AF); break;    
                    case 3: drawSizedTextColor("ERROR: Filename already exists!",5,315,2.0,0xFF1010AF); break;    
                }    
            }
        }
        if(!enteringName){ // World select
            if(!areYouSure){
                drawSizedTextColor("Load World",24,4,2.0,0xFF3FFFFF);
                drawSizedText("Press   or   to scroll", 80, 250,2.0);
                renderButtonIcon(k_up.input & -k_up.input, 240, 232, 1);
                renderButtonIcon(k_down.input & -k_down.input, 324, 232, 1);
                drawSizedText("Press    to load world", 80, 300,2.0);
                renderButtonIcon(k_accept.input & -k_accept.input, 244, 282, 1);
                drawSizedText("Press    to return", 80, 350, 2.0);
                renderButtonIcon(k_decline.input & -k_decline.input, 244, 332, 1);
                if(currentSelection != worldFileCount){
                    drawSizedText("Press    to delete",80, 400,2.0);
                    renderButtonIcon(k_delete.input & -k_delete.input, 244, 382, 1);
                }
            } else {
                drawSizedTextColor("Delete File?",88,12,2.0,0xFF3F3FFF);
                drawSizedText("Press   to confirm", (320-18*12)/2, 100,2.0);
                renderButtonIcon(k_accept.input & -k_accept.input, 122, 98, 1);
                drawSizedText("Press   to return", 58, 150,2.0);
                renderButtonIcon(k_decline.input & -k_decline.input, 128, 148, 1);
            }
            
        } else { // Draw the "keyboard"
            drawSizedTextColor("Touch the keypad below",24,4,2.0,0xFF33FFFF);
            
            vita2d_draw_rectangle(140, 95, 480, 210, 0xFF7F7FBF);
            drawSizedText(guiText0,50, 100, 3);
            drawSizedText(guiText1,50, 140, 3);
            drawSizedText(guiText2,50, 180, 3);
            drawSizedText(guiText3,50, 220, 3);
            drawSizedText(guiText4,50, 260, 3);
            if(touchDelay > 0){
                vita2d_draw_rectangle(touchX, touchY, touchW, touchH, 0xAF000000);
            }
            
            drawSizedText("Press    to confirm", 80, 430, 2.0);
            renderButtonIcon(k_accept.input & -k_accept.input, 244, 412, 1);
            drawSizedText("Press    to return", 80, 480, 2.0);
            renderButtonIcon(k_decline.input & -k_decline.input, 244, 462, 1);
        }
            
		vita2d_end_drawing();
        break;
        case MENU_SETTINGS_REBIND:
		    vita2d_start_drawing();
		        drawTextColor("Rebind Buttons",116,12,0xFF00AFAF);
                drawText("Button",16,32);
                drawText("Game",140,32);
                drawText("Menus",280,32);
		    
                char gameButText[34];
                char menuButText[34];
                    
                for(i = 0; i < 5; ++i){
                    if((currentSelection-2) + i > 21 || (currentSelection-2) + i < 0) continue;
                    renderButtonIcon(keys[(currentSelection-2) + i], 16, (i * 18) + 30, 2);
                    int ccol = 0xFF7F7F7F;
                    
                    sprintf(gameButText,"%s",getButtonFunctionGame(keys[(currentSelection-2) + i]));
                    sprintf(menuButText,"%s",getButtonFunctionMenu(keys[(currentSelection-2) + i]));
                    
                    if(i == 2){ 
                        if(!selBut)ccol = 0xFFFFFFFF;
                        else{ 
                            ccol = 0xFF00FF00;
                            if(left)sprintf(gameButText,"<%s>",getButtonFunctionGame(keys[(currentSelection-2) + i]));
                            else sprintf(menuButText,"<%s>",getButtonFunctionMenu(keys[(currentSelection-2) + i]));
                        }
                    }
                    if(left){
                        drawTextColor(gameButText, 112, (i * 33) + 80, ccol);
                        drawTextColor(menuButText, 280, (i * 33) + 80, 0xFF7F7F7F);
                    } else {
                        drawTextColor(gameButText, 112, (i * 33) + 80, 0xFF7F7F7F);
                        drawTextColor(menuButText, 280, (i * 33) + 80, ccol);
                    }
                }
                if(bindOpt){
                    renderFrame(1,1,24,14,0xFFBF1010);
                    drawTextColor("Save changes?",122,32,0xFF00AFAF);
                    for(i = 2; i >= 0; --i){
                        char* msg = keybOptions[i];
                        uint32_t color = 0xFF4F4F4F;
                        if(i == curSaveSel) color = 0xFFFFFFFF;
                        drawTextColor(msg,(400 - (strlen(msg) * 12))/2, (i * 24) + 92, color);    
                    }
                    drawText("Press   to return", 98, 190);
                    renderButtonIcon(k_decline.input & -k_decline.input, 168, 188, 1);
                    
                    if(errorBut >= 0 && errorBut < 12){
                        char errorText[30];
                        switch(errorBut){
                            case 0: sprintf(errorText, "Error: Missing 'Move up'"); break;
                            case 1: sprintf(errorText, "Error: Missing 'Move down'"); break;
                            case 2: sprintf(errorText, "Error: Missing 'Move right'"); break;
                            case 3: sprintf(errorText, "Error: Missing 'Move left'"); break;
                            case 4: sprintf(errorText, "Error: Missing 'Attack'"); break;
                            case 5: sprintf(errorText, "Error: Missing 'Toggle Menu'"); break;
                            case 6: sprintf(errorText, "Error: Missing 'Pause'"); break;
                            case 7: sprintf(errorText, "Error: Missing 'Accept'"); break;
                            case 8: sprintf(errorText, "Error: Missing 'Decline'"); break;
                            case 9: sprintf(errorText, "Error: Missing 'Delete'"); break;
                            case 10: sprintf(errorText, "Error: Missing 'Next'"); break;
                            case 11: sprintf(errorText, "Error: Missing 'Previous'"); break;
                        }
                        drawTextColor(errorText,(400 - (strlen(errorText) * 12))/2,50,0xFF0000FF);
                    }
                    
                }
		    if(!selBut){
                drawText("Press   to select", 58, 80);
                renderButtonIcon(k_accept.input & -k_accept.input, 128, 78, 1);
                drawText("Press   to return", 58, 130);
                renderButtonIcon(k_decline.input & -k_decline.input, 128, 128, 1);
            } else {
                drawText("Press   or   to scroll", 28, 50);
                renderButtonIcon(k_left.input & -k_left.input, 98, 48, 1);
                renderButtonIcon(k_right.input & -k_right.input, 160, 48, 1);
                drawText("Press   to unselect", 46, 100);
                renderButtonIcon(k_accept.input & -k_accept.input, 118, 98, 1);
                drawText("Press   to return", 58, 150);
                renderButtonIcon(k_decline.input & -k_decline.input, 128, 148, 1);
            }
		    vita2d_end_drawing();
        break;
        
        case MENU_PAUSED:
		    vita2d_start_drawing_advanced(fbo, VITA_2D_RESET_POOL | VITA_2D_SCENE_FRAGMENT_SET_DEPENDENCY);
                if(currentLevel == 0){ 
                    vita2d_draw_texture_part_scale(minimap[1],(-xscr/3)-256,(-yscr/3)-32,0,0,128,128,12.5,7.5);
                    vita2d_draw_rectangle(0,0,400,240, 0xAFDFDFDF);
                }
	            offsetX = xscr;offsetY = yscr;
		            renderMenuBackground(xscr,yscr);
	            offsetX = 0;offsetY = 0;
                renderFrame(1,1,24,14,0xFF1010AF);
                drawText("Paused",164,32);
                for(i = 2; i >= 0; --i){
                    char* msg = pOptions[i];
                    uint32_t color = 0xFF7F7F7F;
                    if(i == currentSelection) color = 0xFFFFFFFF;
                    drawTextColor(msg,(400 - (strlen(msg) * 12))/2, (i * 24) + 100, color);    
                }
                
                if(pauseSaveDisplayTimer > 0) drawTextColor("Game Saved!", (400-(11*12))/2, 64,0xFF20FF20);
                
                if(areYouSure || areYouSureSave){
                    if(areYouSure)renderFrame(6,5,19,10,0xFF10108F);
                    else renderFrame(6,5,19,10,0xFF108F10);
                    
                    drawText("Are you sure?",122,96);
                    drawText("   Yes", 164, 117);
                    renderButtonIconNorm(k_accept.input & -k_accept.input, 166, 114, 1);
                    drawText("   No", 170, 133);
                    renderButtonIconNorm(k_decline.input & -k_decline.input, 166, 130, 1);
                }
                
		    vita2d_end_drawing();
        break;  
        case MENU_WIN:
		    vita2d_start_drawing_advanced(fbo, VITA_2D_RESET_POOL | VITA_2D_SCENE_FRAGMENT_SET_DEPENDENCY);
                if(currentLevel == 0){ 
                    vita2d_draw_texture_part_scale(minimap[1],(-xscr/3)-256,(-yscr/3)-32,0,0,128,128,12.5,7.5);
                    vita2d_draw_rectangle(0,0,400,240, 0xAFDFDFDF);
                }
	            offsetX = xscr;offsetY = yscr;
		            renderMenuBackground(xscr,yscr);
	            offsetX = 0;offsetY = 0;
                renderFrame(5,3,21,12,0xFFFF1010);
                if(!rev){ opacity+=5; if(opacity == 255) rev = true; }
                else { opacity-=5; if(opacity == 100) rev = false; }
                sprintf(scoreText,"Score: %d", player.p.score);
                drawTextColor("You Win!",158,76,0xAFAF0000 + opacity);
                drawText(scoreText, 200-((strlen(scoreText)-1)*6), 100);
                drawText("Press   to continue", 96, 150);
                renderButtonIconNorm(k_attack.input & -k_attack.input, 166, 148, 1);
                
                //printf("0x%08X",k_attack.input & -k_attack.input);
		    vita2d_end_drawing();
        break;  
        case MENU_LOSE:
		    vita2d_start_drawing_advanced(fbo, VITA_2D_RESET_POOL | VITA_2D_SCENE_FRAGMENT_SET_DEPENDENCY);
                if(currentLevel == 0){ 
                    vita2d_draw_texture_part_scale(minimap[1],(-xscr/3)-256,(-yscr/3)-32,0,0,128,128,12.5,7.5);
                    vita2d_draw_rectangle(0,0,400,240, 0xAFDFDFDF);
                }
	            offsetX = xscr;offsetY = yscr;
		            renderMenuBackground(xscr,yscr);
	            offsetX = 0;offsetY = 0;
                renderFrame(5,3,21,12,0xFFFF1010);
                if(!rev){ opacity+=5; if(opacity == 255) rev = true; }
                else { opacity-=5; if(opacity == 100) rev = false; }
                sprintf(scoreText,"Score: %d", player.p.score);
                drawTextColor("You DIED!",158,76,0xAF000000 + opacity);
                drawText(scoreText, 200-((strlen(scoreText)-1)*6), 100);
                drawText("Press   to continue", 96, 150);
                renderButtonIconNorm(k_attack.input & -k_attack.input, 166, 148, 1);
                //printf("0x%08X",k_attack.input & -k_attack.input);
		    vita2d_end_drawing();
        break;  
        case MENU_INVENTORY:
		    vita2d_start_drawing_advanced(fbo, VITA_2D_RESET_POOL | VITA_2D_SCENE_FRAGMENT_SET_DEPENDENCY);
                if(currentLevel == 0){ 
                    vita2d_draw_texture_part_scale(minimap[1],(-xscr/3)-256,(-yscr/3)-32,0,0,128,128,12.5,7.5);
                    vita2d_draw_rectangle(0,0,400,240, 0xAFDFDFDF);
                }
	            offsetX = xscr;offsetY = yscr;
		            renderMenuBackground(xscr,yscr);
	            offsetX = 0;offsetY = 0;
                renderFrame(1,1,24,14,0xFFFF1010);
                renderItemList(player.p.inv, 1,1,24,14, curInvSel);
		    vita2d_end_drawing();
        break;  
        case MENU_CRAFTING:
		    vita2d_start_drawing_advanced(fbo, VITA_2D_RESET_POOL | VITA_2D_SCENE_FRAGMENT_SET_DEPENDENCY);
                if(currentLevel == 0){ 
                    vita2d_draw_texture_part_scale(minimap[1],(-xscr/3)-256,(-yscr/3)-32,0,0,128,128,12.5,7.5);
                    vita2d_draw_rectangle(0,0,400,240, 0xAFDFDFDF);
                }
	            offsetX = xscr;offsetY = yscr;
		            renderMenuBackground(xscr,yscr);
	            offsetX = 0;offsetY = 0;
	            
                renderFrame(15,1,24,4,0xFFFF1010);
                renderFrame(15,5,24,14,0xFFFF1010);
                renderFrame(1,1,14,14,0xFFFF1010);
                renderRecipes(currentRecipes, 1, 1, 14, 14, curInvSel);
                
                Recipe* rec = &currentRecipes->recipes[curInvSel];
                renderItemIcon(rec->itemResult,rec->itemAmountLevel,128,16);
                char craftText[12];
                sprintf(craftText,"%d",countItemInv(rec->itemResult,rec->itemAmountLevel, player.p.inv));
                drawText(craftText,274,34);
                
                if(rec->numOfCosts > 0){
                    int i;
                    for(i = 0; i < rec->numOfCosts; i++){
                        int amnt = countItemInv(rec->costs[i].costItem,0, player.p.inv);
                        int ttlCst = rec->costs[i].costAmount;
                        int col = 0xFFFFFFFF; if(amnt<ttlCst) col = 0xFF7F7F7F;
                        renderItemIcon(rec->costs[i].costItem,1,128,48+(i*8));
                        sprintf(craftText,"%d/%d",amnt,ttlCst);
                        drawTextColor(craftText,274,96+(i*18),col);
                    }
                }
                
		    vita2d_end_drawing();
        break;  
        
        case MENU_CONTAINER:
		    vita2d_start_drawing_advanced(fbo, VITA_2D_RESET_POOL | VITA_2D_SCENE_FRAGMENT_SET_DEPENDENCY);
                if(currentLevel == 0){ 
                    vita2d_draw_texture_part_scale(minimap[1],(-xscr/3)-256,(-yscr/3)-32,0,0,128,128,12.5,7.5);
                    vita2d_draw_rectangle(0,0,400,240, 0xAFDFDFDF);
                }
	            offsetX = xscr;offsetY = yscr;
		            renderMenuBackground(xscr,yscr);
		        if (curChestEntity->entityFurniture.r == 1){ offsetX = 48;offsetY = 0;}
		        else {offsetX = 0;offsetY = 0;}
		        
		        renderFrame(1,1,14,14,0xFFFF1010);
		        renderItemList(curChestEntity->entityFurniture.inv,1,1,14,14,
                curChestEntity->entityFurniture.r == 0 ? curInvSel : -curChestEntity->entityFurniture.oSel - 1);
		        renderFrame(15,1,28,14,0xFFFF1010);
		        renderItemList(player.p.inv,15,1,28,14,
                curChestEntity->entityFurniture.r == 1 ? curInvSel : -curChestEntity->entityFurniture.oSel - 1);
		        offsetX = 0;offsetY = 0;
		    vita2d_end_drawing();
        break;
        case MENU_ABOUT:
			vita2d_start_drawing();
				vita2d_clear_screen();
		        drawSizedText("About Minicraft",50,12,4.0);
		        drawSizedText("Minicraft was made by Markus Persson for the",44,46,2.0);
		        drawSizedText("22'nd ludum dare competition in december 2011.",36,68,2.0);
		        drawSizedText("it is dedicated to my father. <3",92,90,2.0);
		        drawSizedText("- Markus \"Notch\" Persson",124,112,2.0);
		        
		        drawSizedTextColor("Vita Homebrew Edition",34,60,4.0,0xFF00FF00);
		        drawSizedTextColor("This port was made by Rinnegatamante.",78,150,2.0,0xFF00FF00);
		        drawSizedTextColor("Based on the 3DS port by David Benepe (Davideesk).",44,170,2.0,0xFF00FF00);
		        drawSizedTextColor("Thanks to FirebirdTA01 for the PSVITA buttons icons.",32,200,2.0,0xFF00FF00);
			vita2d_end_drawing();
        break;
        case MENU_SETTINGS:
		    vita2d_start_drawing();
			vita2d_clear_screen();
		        drawSizedText("Settings",(540-(8*48))/2,30,4.0);
		        for(i = 4; i >= 0; --i){
                    char* msg = setOptions[i];
                    uint32_t color = 0xFF7F7F7F;
                    if(i == currentSelection) color = 0xFFFFFFFF;
                    if(i == 2){
                        if(shouldRenderDebug) drawSizedTextColor("On",152, ((8 + i) * 20 - 50) >> 1,4.0, 0xFF00DF00);    
                        else  drawSizedTextColor("Off",152, ((8 + i) * 20 - 50) >> 1,4.0, 0xFF0000DF);   
                    } else if(i == 3){
                        
                        if(shouldSpeedup) drawSizedTextColor("On",162, ((8 + i) * 20 - 50) >> 1,4.0, 0xFF00DF00);    
                        else  drawSizedTextColor("Off",162, ((8 + i) * 20 - 50) >> 1,4.0, 0xFF0000DF); 

                    }
                    drawSizedTextColor(msg,(240 - (strlen(msg) * 8))/2, ((8 + i) * 20 - 50) >> 1,4.0, color);    
                }
		        switch(currentSelection){
                    case 0:
                        drawSizedTextColor("Change the controls",24,4,2.0,0xFF7FFFFF);
                        break;
                    case 1:
                        drawSizedTextColor("Change the game's art",24,4,2.0,0xFF7FFFFF);
                        break;
                    case 2:
                        drawSizedTextColor("Show Pos/Entities",24,4,2.0,0xFF7FFFFF);
                        break;
                    case 3:
                        drawSizedTextColor("Enables 444 Mhz mode",24,4,2.0,0xFF7FFFFF);
                        break;
                    case 4:
                        drawSizedTextColor("Back to the titlescreen",24,4,2.0,0xFF7FFFFF);
                        break;
                }
                drawSizedText("Press    to select", 144, 100, 2.0);
                renderButtonIcon(k_accept.input & -k_accept.input, 374, 82, 1);
                drawSizedText("Press    to return", 144, 150, 2.0);
                renderButtonIcon(k_decline.input & -k_decline.input, 374, 132, 1);
		    vita2d_end_drawing();
        break;
        case MENU_TITLE:
            /* Top Screen */
		    vita2d_start_drawing();
				vita2d_clear_screen();
		        renderTitle(76,16);
		    
		        for(i = 4; i >= 0; --i){
                    char* msg = options[i];
                    uint32_t color = 0xFF7F7F7F;
                    if(i == currentSelection) color = 0xFFFFFFFF;
                    drawSizedTextColor(msg,(240 - (strlen(msg) * 8))/2, ((8 + i) * 20 - 50) >> 1,4.0, color);    
                }
                
		        drawSizedText(versionText,2,510,2.5);
		    
		      int startX = 0, startY = 0;// relative coordinates ftw
			  startX = 60;startY = 50;
              render16(startX,startY+12,0,128,0);//Player(Carrying)
              render16(startX,startY,128,128,0);//Workbench
              startX = 60;startY = 105;
              render16b(startX,startY,16,96,0,0xFFC11D00);// water pit
              render16b(startX+16,startY,32,96,0,0xFFC11D00);
              render16b(startX,startY+16,48,96,0,0xFFC11D00);
              render16b(startX+16,startY+16,64,96,0,0xFFC11D00);
              renderc  (startX+8,startY+12,48,160,16,8,0);//Waves
              renderc  (startX+8,startY+8,0,112,16,8,0);//Player (Top-Half)
              startX = 60;startY = 170;
              render16 (startX,startY,48,112,0);//Player
              renderc  (startX+12,startY,40,160,8,16,0);//Slash
              render   (startX+14,startY+4,152,144,0);//Pickaxe
              render16b(startX+18,startY,80,0,0,0xFFAEC6DC);//Iron ore
              startX = 360;startY = 90;
              render16b (startX,startY,128,112,0,0xFFADFFAD);//Slime
              render16 (startX+18,startY,48,112,1);//Player (Mirrored)
              renderc  (startX+14,startY,32,160,8,16,0);//Slash
              render   (startX+12,startY+4,104,144,1);//Sword
              startX = 360;startY = 190;
              render16b(startX,startY,16,80,0,0xFF69B569);// grass pit
              render16b(startX+16,startY,32,80,0,0xFF69B569);
              render16b(startX,startY+16,48,80,0,0xFF69B569);
              render16b(startX+16,startY+16,64,80,0,0xFF69B569);
              render16 (startX+8,startY+4,0,16,0);//Tree
              render   (startX+1,startY+14,80,152,0);// Apple
              render16 (startX+9,startY+18,16,112,0);//Player
              renderc  (startX+9,startY+14,16,160,16,8,0);//Slash
		        switch(currentSelection){
                    case 0: // "Start Game"
                        drawSizedTextColor("Play minicraft",24,4,2.0,0xFF7FFFFF);
                        break;
                    case 1: // "How To Play"
                        drawSizedTextColor("Learn the basics",24,4,2.0,0xFF7FFFFF);
                        break;
                    case 2: // "Settings"
                        drawSizedTextColor("Modify the game's feel",24,4,2.0,0xFF7FFFFF);
                        break;
                    case 3: // "About"
                        drawSizedTextColor("Who made this game?",24,4,2.0,0xFF7FFFFF);
                        break;
                    case 4: // "Exit"
                        drawSizedTextColor("Exit to the PSVITA dashboard",24,4,2.0,0xFF7FFFFF);
                        break;
                }
		    vita2d_end_drawing();
            break;
    }

}
