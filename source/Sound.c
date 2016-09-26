#include "Sound.h"

void loadSound(Sound * snd, char * filename){
    FILE *file = fopen(filename, "rb");
    if(file != NULL){
        fseek(file, 0, SEEK_END);
        snd->size = ftell(file);
        fseek(file, 0, SEEK_SET);
        snd->buffer = malloc(snd->size);
        fread(snd->buffer, 1, snd->size, file);
    }
    fclose(file);
}

void playSound(Sound snd){
    //csndPlaySound(8, SOUND_FORMAT_16BIT | SOUND_ONE_SHOT, 44100, 1, 0, snd.buffer, snd.buffer, snd.size);
}

void freeSounds(){
    free(snd_playerHurt.buffer);
    free(snd_playerDeath.buffer);
    free(snd_monsterHurt.buffer);
    free(snd_test.buffer);
    free(snd_pickup.buffer);
    free(snd_bossdeath.buffer);
    free(snd_craft.buffer);
}
