#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdio>

#include "world.h"
#include "world2.h"
#include "globals.h"
#include "lighting.h"

#include <GL/glut.h>

void initEnvironment();

// FEfek & Dekorasi Lingkungan
void drawGlowingTurntableZone();
void drawEnvironmentDecorations();

// Helper Generate Bangunan (procedural)
void drawTexturedHouse(float x, float z, float w, float h, float d, float roofHeight);
void drawGuardrails();
void drawOverpassBridge(float zPos);
