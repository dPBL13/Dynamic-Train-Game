#include <windows.h>
#include <mmsystem.h>
#include "globals.h"
#include "bgm.h"
#include <cstdio>

bool isMuted = false;

// Memutar BGM secara Asinkron (tidak memblokir thread game) dan Loop otomatis
void playBGM() {
    if (isMuted) return;
    
    // Memutar file "bgm_game.wav" yang diletakkan satu folder dengan file .exe
    BOOL success = PlaySound(TEXT("TOI.wav"), NULL, SND_ASYNC | SND_LOOP | SND_FILENAME);
    
    if (!success) {
        printf("Peringatan Kelompok 8: File audio 'TOI.wav' gagal dimuat atau tidak ditemukan!\n");
    }
}

// Menghentikan putaran musik secara total
void stopBGM() {
    PlaySound(NULL, 0, 0);
}

// Fungsi Toggle Mute (On/Off) untuk dipanggil lewat Input Keyboard
void toggleMusic() {
    if (!isMuted) {
        // Jika sedang menyala, matikan audio secara instan
        stopBGM();
        isMuted = true;
        printf("Audio Muted.\n");
    } else {
        // Jika sedang mati, hidupkan kembali dari awal
        isMuted = false;
        playBGM();
        printf("Audio Playing.\n");
    }
}
