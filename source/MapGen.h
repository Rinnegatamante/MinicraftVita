#pragma once
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#ifndef SceGxtErrorCode
#  define SceGxtErrorCode uint8_t
#endif
#include <vitasdk.h>
#include "Globals.h"

float nextFloat();
double sample(double * values, int x, int y);
double * Noise(int w, int h, int featureSize);
void newSeed();
void createAndValidateTopMap(int w, int h, uint8_t * map, uint8_t * data);
void createTopMap(int w, int h, uint8_t * map, uint8_t * data);
void createAndValidateUndergroundMap(int w, int h,int depthLevel, uint8_t * map, uint8_t * data);
void createUndergroundMap(int w, int h,int depthLevel, uint8_t * map, uint8_t * data);
void createAndValidateSkyMap(int w, int h, uint8_t * map, uint8_t * data);
void createSkyMap(int w, int h, uint8_t * map, uint8_t * data);
