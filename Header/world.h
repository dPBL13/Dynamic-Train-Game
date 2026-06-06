#pragma once
// =====================================================
// world.h - Rendering Dunia: Rel, Tanah, Pohon, Gunung
// =====================================================

#include <GL/glut.h>

// Inisialisasi posisi & skala pohon secara pseudo-random.
// Harus dipanggil sekali di init().
void initTrees();

// Menggambar sepasang rel baja + bantalan kayu sepanjang REL_PANJANG.
void drawRail();

// Menggambar bidang tanah hijau.
void drawGround();

// Menggambar satu pohon (batang + 3 tingkat kerucut daun).
void drawTree(float x, float z, float sc);

// Menggambar semua pohon dari array trees[].
void drawAllTrees();

// Menggambar satu gunung (kerucut + puncak salju).
void drawMountain(float cx, float cz, float rad, float h,
                  float r, float g, float b);

// Menggambar semua gunung latar belakang.
void drawMountains();

void drawGate(float zPos, const char* label);
void drawStartGates();
void drawFinishAreaZone();
void drawTurntableZone();

