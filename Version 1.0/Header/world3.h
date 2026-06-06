#ifndef WORLD3_H
#define WORLD3_H

#ifdef _WIN32
#include <windows.h>
#endif

#include "world3.h"
#include "globals.h"
#include <windows.h>
#include <GL/glut.h>

// =============================
// 3. Dekor part 2 (plus texture handling) & generate environemnet
// =============================

struct StaticDecoration {
    int type;
    float x, y, z;
    float rotation;
    float scaleX, scaleY, scaleZ;
};

void initEnvironmentData();
void drawStaticEnvironment();
void drawSkybox(float trainX, float trainY, float trainZ);
void draw3DMeshHills();
void drawTurnZoneIndicators();
void drawTrainDepot(float centerX, float centerY, float centerZ);

void drawHangingCables(float x1, float z1, float x2, float z2, float height);
void drawHouseType1();
void drawHouseType2();
void drawHouseType3();
void drawHouseType4();
void drawUtilityPole();
void drawIndonesianRuko(float x, float z);
void drawTiangListrikKabel(float x, float z);

#endif // WORLD3_H
