#pragma once
#include <vitasdk.h>
#include <vita2d.h>
#include <string.h> 
#include <ctype.h>

#include "Render.h"

extern uint8_t pageNum;
extern uint8_t maxPageNum;

char pageText[18];

void renderTutorialPage(bool topScreen);
