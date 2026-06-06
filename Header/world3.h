#include "globals.h"
#include <GL/glut.h>

// Struktur Data Dekorasi Statis dengan Tambahan Parameter Variasi Tipe
struct StaticDecoration {
    int type;         // 1: Rumah 1 Lantai, 2: Ruko 2 Lantai, 3: Rumah Limas, 4: Fasum, 5: Tiang Listrik
    float x, y, z;
    float rotation;
    float scaleX, scaleY, scaleZ;
};

extern GLuint texSky; 

void drawSkybox(float trainX, float trainY, float trainZ);

// Fungsi Manajemen Lingkungan
void initEnvironmentData();
void drawStaticEnvironment();
void draw3DMeshHills();

// Fungsi Gambar Aset Spesifik
void drawHouseType1();
void drawHouseType2();
void drawHouseType3();
void drawHouseType4();
void drawUtilityPole();
void drawHangingCables(float x1, float z1, float x2, float z2, float height);
void drawIndonesianRuko(float x, float z);
void drawTiangListrikKabel(float x, float z);
void drawTrainDepot(float centerX, float centerY, float centerZ);
void drawTurnZoneIndicators();
