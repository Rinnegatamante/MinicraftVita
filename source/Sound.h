#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#ifndef SceGxtErrorCode
#  define SceGxtErrorCode uint8_t
#endif
#include <vitasdk.h>


typedef struct {
    uint32_t size;
    uint8_t* buffer;
} Sound;

void loadSound(Sound * snd, char * filename);
void playSound(Sound snd);
void freeSounds();
void initSound();

Sound snd_playerHurt;
Sound snd_playerDeath;
Sound snd_monsterHurt;
Sound snd_test;
Sound snd_pickup;
Sound snd_bossdeath;
Sound snd_craft;
