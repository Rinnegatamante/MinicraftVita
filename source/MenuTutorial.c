#include "MenuTutorial.h"

uint8_t pageNum = 0;
uint8_t maxPageNum = 6;

uint32_t biasedCirclePad(uint32_t in){
    if(in & SCE_CTRL_UP) return SCE_CTRL_UP;
    else if(in & SCE_CTRL_DOWN) return SCE_CTRL_DOWN;
    else if(in & SCE_CTRL_LEFT) return SCE_CTRL_LEFT;
    else if(in & SCE_CTRL_RIGHT) return SCE_CTRL_RIGHT;
    else return (in & -in);
}

uint32_t biasedMenuXY(uint32_t in){
    if(in & SCE_CTRL_TRIANGLE) return SCE_CTRL_TRIANGLE;
    else if(in & SCE_CTRL_SQUARE) return SCE_CTRL_SQUARE;
    else return (in & -in);
}

/** Oh my god, this was so damn tedious to make. */
void renderTutorialPage(bool topScreen){
    if(topScreen){
		vita2d_clear_screen();
        drawSizedTextColor("How to Play",(660-11*48)/2,12,4.0,0xFF00AFAF);
        switch(pageNum){
            case 0: // Moving the character
                drawSizedTextColor("Movement",(540-8*48)/2,30,4.0,0xFF007FBF);
                drawSizedText("Press   to move up",144,190,2.0);
                renderButtonIcon(biasedCirclePad(k_up.input), 370, 172, 1);
                drawSizedText("Press   to move down",144,220,2.0);
                renderButtonIcon(biasedCirclePad(k_down.input), 370, 202, 1);
                drawSizedText("Press   to move left",144,250,2.0);
                renderButtonIcon(biasedCirclePad(k_left.input), 370, 232, 1);
                drawSizedText("Press   to move right",144,280,2.0);
                renderButtonIcon(biasedCirclePad(k_right.input), 370, 262, 1);
                break;
            case 1: // Attacking
                drawSizedTextColor("Attacking",(575-9*48)/2,30,4.0,0xFF007FBF);
                drawSizedText("Press    to Attack",144,190,2.0);
                renderButtonIcon(k_attack.input & -k_attack.input, 374, 172, 1);
                drawSizedText("Attack with an item to use it",80,250,2.0);
                drawSizedText("Use the axe to cut down trees",80,295,2.0);
                drawSizedText("Use the sword to attack enemies",80,320,2.0);
                drawSizedText("Use the shovel to dig ground",80,345,2.0);
                drawSizedText("Use the pickaxe to mine rock/ore",80,370,2.0);
                break;
            case 2: // Inventory
                drawSizedTextColor("Inventory",(575-9*48)/2,30,4.0,0xFF007FBF);
                drawSizedText("Press    to open the menu",144,190,2.0);
                renderButtonIcon(biasedMenuXY(k_menu.input), 374, 172, 1);
                drawSizedText("Press   to scroll up",144,230,2.0);
                renderButtonIcon(biasedCirclePad(k_up.input), 370, 212, 1);
                drawSizedText("Press   to scroll down",144,270,2.0);
                renderButtonIcon(biasedCirclePad(k_down.input), 370, 252, 1);
                drawSizedText("Press    to select an item",144,310,2.0);
                renderButtonIcon(k_accept.input & -k_accept.input, 374, 292, 1);
                drawSizedText("Press    to close the menu",144,350,2.0);
                renderButtonIcon(k_decline.input & -k_decline.input, 374, 332, 1);
                break;
            case 3: // Furniture
                drawSizedTextColor("Furniture",(575-9*48)/2,30,4.0,0xFF007FBF);
                drawSizedText("Use furniture for item crafting",80,190,2.0);
                drawSizedText("Press    to open the menu",80,230,2.0);
                renderButtonIcon(biasedMenuXY(k_menu.input), 244, 212, 1);
                drawSizedText("while infront of the furniture",80,270,2.0);
                drawSizedText("Use the lantern item to light",80,310,2.0);
                drawSizedText("up underground areas",80,350,2.0);
                drawSizedText("Use the power glove item to",80,390,2.0);
                drawSizedText("pick up furniture",80,430,2.0);
                break;
            case 4: // Crafting
                drawSizedTextColor("Crafting",(540-8*48)/2,30,4.0,0xFF007FBF);
                drawSizedText("Create new items and tools",80,190,2.0);
                drawSizedText("Go up to a furniture item and",80,230,2.0);
                drawSizedText("Press    to open the menu",80,270,2.0);
                renderButtonIcon(biasedMenuXY(k_menu.input), 244, 252, 1);
                drawSizedText("Gather up the required materials",80,310,2.0);
                drawSizedText("and then press    to craft it",80,350,2.0);
                renderButtonIcon(k_accept.input & -k_accept.input, 387, 332, 1);
                break;
            case 5: // Farming
                drawSizedTextColor("Farming",(505-7*48)/2,30,4.0,0xFF007FBF);
                drawSizedText("Grow wheat to make bread",80,190,2.0);
                drawSizedText("Dig up grass to gather seeds",80,220,2.0);
                drawSizedText("Use the hoe to till ground",80,250,2.0);
                drawSizedText("Harvest wheat when it is yellow",80,280,2.0);
                drawSizedText("Use the oven to bake bread",80,310,2.0);
                drawSizedText("It takes 4 wheat to craft bread",80,340,2.0);
                break;
            case 6: // Mining
                drawSizedTextColor("Mining",(470-6*48)/2,30,4.0,0xFF007FBF);
                drawSizedText("Use a pickaxe tool for mining",80,190,2.0);
                drawSizedText("Mine rocks for stone",80,220,2.0);
                drawSizedText("Mine iron ore for iron",80,250,2.0);
                drawSizedText("Mine gold ore for gold",80,280,2.0);
                drawSizedText("Mine gem ore to get gems",80,310,2.0);
                drawSizedText("It takes 4 ore and 1 coal to",80,340,2.0);
                drawSizedText("make an ingot inside a furnace",80,370,2.0);
                break;
        }
    } else {
        switch(pageNum){
            case 0: // Moving the character
                render16(30,56,16,112,0);//Player up
                renderButtonIcon(biasedCirclePad(k_up.input), 30,40, 2);
                render16(60,56,0,112,0);//Player down
                renderButtonIcon(biasedCirclePad(k_down.input), 60,40, 2);
                render16(90,56,48,112,1);//Player left
                renderButtonIcon(biasedCirclePad(k_left.input), 90,40, 2);
                render16(120,56,48,112,0);//Player right
                renderButtonIcon(biasedCirclePad(k_right.input), 120,40, 2);
                break;
            case 1: // Attacking
                render16(60,56,0,112,0);//Player-down
                renderButtonIcon(k_attack.input & -k_attack.input, 80, 56, 2);
                renderc(60,68,16,160,16,8,2);//Slash
                
                render16b(12,20,16,80,0,0xFF69B569);// grass pit
                render16b(12+16,20,32,80,0,0xFF69B569);
                render16b(12,20+16,48,80,0,0xFF69B569);
                render16b(12+16,20+16,64,80,0,0xFF69B569);
                render16(12+8,20+4,0,16,0);//Tree
                renderc(12+9,20+14,16,160,16,8,0);//Slash
                render(12+9+4,20+14,192,144,0);//Axe
                render16(12+9,20+18,16,112,0);//Player-up
                
                render16b(122,62,16,80,0,0xFF7BF7F7);// sand pit
                render16b(122+16,62,32,80,0,0xFF7BF7F7);
                render16b(122,62+16,48,80,0,0xFF7BF7F7);
                render16b(122+16,62+16,64,80,0,0xFF7BF7F7);
                render16b(130,70,0,96,0,0xFF383838);// hole
                render16(116,70,48,112,0);//Player-right
                renderb(136,76,16,152,0,0xFF8197AF);// Dirt item
                renderc(128,70,40,160,8,16,0);//Slash
                render(130,74,0,144,0);//Shovel
                break;
            case 2: // Inventory
                renderFrame(4,4,17,11,0xFFAF0000);
                renderItemStuffWithText(ITEM_APPLE,5,false,80,78);
                renderItemStuffWithText(ITEM_SLIME,11,false,80,94);
                renderItemStuffWithText(TOOL_SWORD,4,true,80,110);
                renderItemStuffWithText(ITEM_IRONORE,3,false,80,126);
                renderItemStuffWithText(ITEM_IRONINGOT,11,false,80,142);
                vita2d_draw_rectangle(64, 110, 12, 12, 0xFF000000);
		        drawText(">", 64, 110);
                renderButtonIcon(biasedCirclePad(k_up.input), 44, 92, 1);
                renderButtonIcon(k_accept.input & -k_accept.input, 44, 108, 1);
                renderButtonIcon(biasedCirclePad(k_down.input), 44, 125, 1);
                break;
            case 3: // Furniture
                vita2d_draw_rectangle(64, 48, 192, 32, 0xFF69B569);
                renderc(32,24,64,128,96,16,0);//Furniture entities
                
                renderFurniture(ITEM_WORKBENCH, 50,60);
                render16(50,46,0,112,0);//Player-down
                renderc(50,58,16,160,16,8,2);//Slash
                render(54,58,56,152,0);//Power glove
                
                render16(92,56,0,128,0);//Player(Carrying)
                render16(92,44,128,128,0);//Workbench
                break;
            case 4: // Crafting
                renderFrame(11,3,19,6,0xFFFF1010);
                renderFrame(11,7,19,12,0xFFFF1010);
                renderFrame(1,3,10,12,0xFFFF1010);
                renderItemStuffWithText(TOOL_AXE,0,true,28,64);
                renderItemIcon(TOOL_AXE, 0, 94, 32);
                drawText("0", 206, 66);
                renderItemIcon(ITEM_WOOD, 0, 94, 64);
                drawText("16/5", 206, 130);
                break;
            case 5: // Farming (Bottom screen)
                renderc(24,16,144,0,112,16,0); // Wheat Stages
                
                render16(20,40,144,0,0); // Farm Tile
                render16(36,40,240,0,0); // Wheat Tile
                render16(52,40,240,0,0); // Wheat Tile
                render16(20,54,16,112,0); // Player (Up)
                renderc(20,50,16,160,16,8,0); // Slash (Up)
                render(19,45,40,152,0); // Seeds
                render(26,39,48,152,0); // Wheat1
                render(29,44,48,152,0); // Wheat2
                
                renderc(72,40,144,0,32,16,0); // Farm Tile + Seeded Wheat Tile
                render16(72,54,16,112,0); // Player (Up)
                renderc(72,50,16,160,16,8,0); // Slash (Up)
                render(76,48,40,152,0); // Seeds
                
                vita2d_draw_rectangle(216, 80, 32, 32, dirtColor[1]); // Dirt color for grass
                render16b(108, 40, 0, 80, 0, 0xFF69B569); // Grass
                render16(124, 40,144,0,0); // Farm Tile
                render16(108,54,16,112,0); // Player (Up)
                renderc(108,50,16,160,16,8,0); // Slash (Up)
                render(112,48,72,144,0); // Gem Hoe
                
                vita2d_draw_rectangle(112, 156, 32, 32, dirtColor[1]); // Dirt color for grass
                render16b(56, 78, 0, 80, 0, 0xFF69B569); // Grass
                vita2d_draw_rectangle(80, 156, 32, 32, dirtColor[1]); // Dirt color
                render16b(40, 78, 0, 0, 0, 0xFF8F8FA8); // Dirt Dots
                render(44, 82, 40,152,0); // Seeds
                
                render16(24,78,48,112,0); // Player (Right)
                renderc(36,78,40,160,8,16,0); // Slash (Right)
                render(38,82,32,144,0); // Gem Shovel
                
                render(82,78,48,152,0); // Wheat
                render(90,78,48,152,0); // Wheat
                render(82,86,48,152,0); // Wheat
                render(90,86,48,152,0); // Wheat
                drawText(">",203,164);
                render16(108,76,96,128,0); // Oven
                drawText(">",246,164);
                render(132,82,72,152,0); // Bread
                break;
            case 6: //Mining
                render16b(23,32,80,0,0,0xFFC8C8DF); // iron ore
                render16b(23,52,80,0,0,0xFFB9E8E5); // gold ore
                render16b(23,72,80,0,0,0xFFDE98DF); // gem ore
                renderb(41,38,88,152,0,0xFFC8C8DF); // Iron ore item
                renderb(41,58,88,152,0,0xFFB9E8E5); // Gold ore item
                render(41,78,112,152,0); // Gem item
                drawText(">",104,74);
                drawText(">",104,114);
                drawText(">",104,154);
                render16(60,32,112,128,0); // Furnace
                render16(60,52,112,128,0); // Furnace
                render16(60,72,64,128,0); // Anvil
                drawText(">",160,74);
                drawText(">",160,114);
                drawText(">",160,154);
                renderb(88,36,96,152,0,0xFFC8C8DF); // Iron ingot item
                renderb(88,56,96,152,0,0xFFB9E8E5); // Gold ingot item
                renderb(88,76,152,144,0,0xFFB9E8E5); // Gem Pickaxe
                drawText(">",200,74);
                drawText(">",200,114);
                render16(106,32,64,128,0); // Anvil
                render16(106,52,64,128,0); // Anvil
                drawText(">",244,74);
                drawText(">",244,114);
                render(130,36,136,144,0); // Iron Pickaxe
                render(130,56,144,144,0); // Gold Pickaxe
                break;
        }
        
        drawText(pageText,(320-(strlen(pageText))*12)/2,12);
        if(pageNum > 0){
            drawText("<",2,16);
            renderButtonIcon(k_menuPrev.input & -k_menuPrev.input, 8, 2, 2);
        }
        if(pageNum < maxPageNum){
            drawText(">",306,16);
            renderButtonIcon(k_menuNext.input & -k_menuNext.input, 136, 2, 2);
        }
        drawText("Press   to exit",(320-(15*12))/2,218);
        renderButtonIcon(k_decline.input & -k_decline.input, 140, 216, 1);
    }
}
