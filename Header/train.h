// =====================================================
// train.h  –  Rendering Kereta Detail & Roda
//
// drawWheel: roda tunggal di posisi (x,y,z) relatif kereta.
//            Sudut diambil dari wheelAngle global.
// drawTrain: lokomotif + 2 gerbong, sudah disesuaikan
//            dengan orientasi rel (sumbu Z dunia, rotasi 90°Y).
// =====================================================

#pragma once
#include "globals.h"

// Roda tunggal (torus diganti gluCylinder detail + spoke)
void drawWheel(float x, float y, float z);

// Kereta lengkap: lokomotif + 2 gerbong
void drawTrain();

void drawLocomotiveOnly();    // Khusus draw Lokomotif
void drawPassengerCarOnly(int carIndex);  // Khusus draw Satu Gerbong
