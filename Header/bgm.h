#pragma once
#include <GL/glut.h>
#include <deque>

extern bool isMuted;            // Status audio mute

// Deklarasi fungsi audio pengendali global
void playBGM();
void toggleMusic();
void stopBGM();
