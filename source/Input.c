#include "Input.h"

#define lerp(value, from_max, to_max) ((((value*10) * (to_max*10))/(from_max*10))/10)

void toggleKey(Key* key, bool held, bool down){
    key->down = held;
	key->clicked = down;
}
    
void tickKeys(uint32_t held, uint32_t down){
    SceTouchData touch;
	sceTouchPeek(SCE_TOUCH_PORT_FRONT, &touch, 1);
	k_touch = touch.report[0];
	k_touch.x = lerp(k_touch.x, 1920, 960);
	k_touch.y = lerp(k_touch.y, 1088, 544);
    toggleKey(&k_up, held & k_up.input, down & k_up.input);
    toggleKey(&k_down, held & k_down.input, down & k_down.input);
    toggleKey(&k_left, held & k_left.input, down & k_left.input);
    toggleKey(&k_right, held & k_right.input, down & k_right.input);
    toggleKey(&k_pause, held & k_pause.input, down & k_pause.input);
    toggleKey(&k_attack, held & k_attack.input, down & k_attack.input);
    toggleKey(&k_menu, held & k_menu.input, down & k_menu.input);
    toggleKey(&k_accept, held & k_accept.input, down & k_accept.input);
    toggleKey(&k_decline, held & k_decline.input, down & k_decline.input);
    toggleKey(&k_delete, held & k_delete.input, down & k_delete.input);
    toggleKey(&k_menuNext, held & k_menuNext.input, down & k_menuNext.input);
    toggleKey(&k_menuPrev, held & k_menuPrev.input, down & k_menuPrev.input);
}

