#include "bgm.h"
#include "globals.h"
#include <windows.h>
#include <mmsystem.h>
#include <cstdio>

// Jalankan BGM loop async
void playBGM() {
    if (isMuted) return;
    
    BOOL success = PlaySound(TEXT("TOI.wav"), NULL, SND_ASYNC | SND_LOOP | SND_FILENAME);
    if (!success) {
        printf("Audio Warning: 'TOI.wav' tidak ditemukan!\n");
    }
}

// Matikan BGM
void stopBGM() {
    PlaySound(NULL, 0, 0);
}

// Toggle status mute audio
void toggleMusic() {
    if (!isMuted) {
        stopBGM();
        isMuted = true;
    } else {
        isMuted = false;
        playBGM();
    }
}
