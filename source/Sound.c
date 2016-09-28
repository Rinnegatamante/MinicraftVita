#include "Sound.h"
#define AUDIO_CHANNELS 8
#define AUDIO_BUFSIZE 8192 // Max dimension of BGM/SFX buffer size

typedef struct sfx{
	uint8_t isFinished;
	uint8_t isPlaying;
	uint8_t* data;
	uint32_t size;
}sfx;

void loadSound(Sound * snd, char * filename){
    FILE *file = fopen(filename, "rb");
    if(file != NULL){
        fseek(file, 0, SEEK_END);
        snd->size = ftell(file);
        fseek(file, 0, SEEK_SET);
		int memsize = ((snd->size / AUDIO_BUFSIZE) + 1) * AUDIO_BUFSIZE;
        snd->buffer = malloc(memsize);
		memset(snd->buffer, 0, memsize);
        fread(snd->buffer, 1, snd->size, file);
    }
    fclose(file);
}

// SFX audio threads
SceUID sfx_threads[AUDIO_CHANNELS];
SceUID SFX_Mutex;
SceUID SFX_Mutex_ID;
volatile uint8_t mustExit = 0;
uint8_t sfx_exited = 0;
sfx sfx_sounds[AUDIO_CHANNELS];
static int sfxThread(unsigned int args, void* arg){
	int ch = sceAudioOutOpenPort(SCE_AUDIO_OUT_PORT_TYPE_MAIN, 64, 48000, SCE_AUDIO_OUT_MODE_MONO);
	if (ch < 0) sceKernelExitDeleteThread(0);

	for (;;){
		sceKernelWaitSema(SFX_Mutex, 1, NULL);

		// Check if the thread must be closed
		if (mustExit){
			sfx_exited++;
			if (sfx_exited < AUDIO_CHANNELS) sceKernelSignalSema(SFX_Mutex, 1);
			else mustExit = 0;
			sceAudioOutReleasePort(ch);
			sceKernelExitDeleteThread(0);
		}

		// Pick the next SE that wants to be played
		sceKernelWaitSema(SFX_Mutex_ID, 1, NULL);
		int idx = -1;
		int i;
		for (i = 0; i < AUDIO_CHANNELS; ++i) {
			if (!sfx_sounds[i].isPlaying && !sfx_sounds[i].isFinished) {
				idx = i;
				break;
			}
		}

		if (idx == -1) {
			sceKernelSignalSema(SFX_Mutex_ID, 1);
			continue;
		} else {
			sfx_sounds[idx].isPlaying = 1;
			sceKernelSignalSema(SFX_Mutex_ID, 1);
		}

		// Preparing audio port
		uint8_t audio_mode = SCE_AUDIO_OUT_MODE_MONO;
		int nsamples = AUDIO_BUFSIZE / ((audio_mode+1)<<1);
		sceAudioOutSetConfig(ch, nsamples, 48000, audio_mode);
		int vol = 32767;
		int vol_stereo[] = {vol, vol};
		sceAudioOutSetVolume(ch, SCE_AUDIO_VOLUME_FLAG_L_CH | SCE_AUDIO_VOLUME_FLAG_R_CH, vol_stereo);

		// Playing sound
		int ptr = 0;
		while (ptr < AUDIO_BUFSIZE){
			sceAudioOutOutput(ch, &sfx_sounds[idx].data[ptr]);
			ptr += AUDIO_BUFSIZE;
		}
		sfx_sounds[idx].isFinished = 1;
		
	}

}

void initSound(){
	SFX_Mutex = sceKernelCreateSema("SFX Mutex", 0, 0, 1, NULL);
	SFX_Mutex_ID = sceKernelCreateSema("SFX Mutex for ID read", 0, 1, 1, NULL);
	
	// Starting audio threads for SFX
	int i;
	for (i=0;i < AUDIO_CHANNELS; i++){
		sfx_threads[i] = sceKernelCreateThread("SFX Thread", &sfxThread, 0x10000100, 0x10000, 0, 0, NULL);
		int res = sceKernelStartThread(sfx_threads[i], sizeof(sfx_threads[i]), &sfx_threads[i]);
		if (res != 0){
			return;
		}
	}
}

void playSound(Sound snd){

	// Pick the next free SE slot.
	int idx = -1;
	int i;
	for (i = 0; i < AUDIO_CHANNELS; ++i) {
		if ((!sfx_sounds[i].isPlaying) || sfx_sounds[i].isFinished) {
			idx = i;
			break;
		}
	}
	if (idx == -1) {
		return;
	}
	
	// Wait and signal is required to prevent reordering
	sceKernelWaitSema(SFX_Mutex_ID, 1, NULL);
	sfx_sounds[idx].isPlaying = 0;
	sfx_sounds[idx].isFinished = 0;
	sfx_sounds[idx].size = snd.size;
	sfx_sounds[idx].data = snd.buffer;
	sceKernelSignalSema(SFX_Mutex_ID, 1);

	// Start one SE thread
	sceKernelSignalSema(SFX_Mutex, 1);
	
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
