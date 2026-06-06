#pragma once

// =============================
// 1. Rel, Tanah, Pohon, Gunung
// =============================

#include <GL/glut.h>
#include "globals.h"

// loader texture bmp
GLuint loadBMP(const char* filename);

// Primitives circle + silinder
void drawCircle3D(float radius, int segments, float yOffset);
void drawCylinderSimple(float radius, float height, int segments);


// -- Sistem Lingkungan & Vegetasi (Pohon) --
void initTrees();
void drawTree(float x, float z, float sc); // single
void drawAllTrees(); // all dari array


// -- Geometri Lintasan & Interaksi --
// single rail pendek 
void drawSingleRailSegment(float currentX, float currentZ, float segmentLength);
// full rail (10 turn kali / kurlenih 1100an z) 
void drawRail();

void drawGround();


// -- Fungsi Prosedural Objek & Rintangan --
void DrawProceduralObstacle3D(ObstacleType type, float x, float z);
void DrawVisualCueSign(float turnZStart, int turnNum); // neon turn circle


// -- Gerbang + Zona Area Game --
void drawGate(float zPos, const char* label);
void drawStartGates();
void drawFinishAreaZone();
