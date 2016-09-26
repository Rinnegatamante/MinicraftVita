#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <vita2d.h>
#include "Globals.h"
#include "minizip/unzip.h"

int loadTexturePack(char * filename);
int getTexturePackComment(char * filename, char * cmmtBuf);
