#ifndef SceGxtErrorCode
#  define SceGxtErrorCode uint8_t
#endif
#undef bool
#ifndef bool
#  define bool uint32_t
#endif
#include <vitasdk.h>

typedef struct {
	bool down, clicked;
	uint32_t input;
} Key;

Key k_null;
Key k_up;
Key k_down;
Key k_left;
Key k_right;
Key k_attack;
Key k_menu;
Key k_pause;
Key k_accept;
Key k_decline;
Key k_delete;
Key k_menuNext;
Key k_menuPrev;
SceTouchReport k_touch;

void tickKeys(uint32_t held, uint32_t down);
bool clicked(Key key);
