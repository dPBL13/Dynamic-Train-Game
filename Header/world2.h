#pragma once
#include <GL/glut.h>

// Fungsi inisialisasi visual asset & loader
GLuint loadBMP(const char* filename);
void initEnvironment();

// Fungsi rendering elemen visual baru
void drawGlowingTurntableZone();
void drawEnvironmentDecorations();

// Helper dasar bangunan prosedural
void drawTexturedHouse(float x, float z, float w, float h, float d, float roofHeight);
void drawGuardrails();
void drawOverpassBridge(float zPos);
