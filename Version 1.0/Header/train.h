#pragma once
#include <GL/glut.h>
#include "globals.h"

// --- CORE TRAIN MODEL DECORATOR ---
void drawWheel(float x, float y, float z);
void drawTrain();
void drawLocomotiveOnly();
void drawPassengerCarOnly(int carIndex);
