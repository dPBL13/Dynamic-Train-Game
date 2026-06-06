// =====================================================
// world.cpp - Rendering Dunia: Rel, Tanah, Pohon, Gunung
// =====================================================
#define _USE_MATH_DEFINES // WAJIB DI ATAS: Mengaktifkan M_PI di MinGW/Dev-C++
#include <cmath>          // WAJIB: Menyediakan fungsi std::cos dan std::sin
#include "world.h"
#include "lighting.h"
#include "globals.h"

GLuint loadBMP(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Texture file not found: %s\n", filename);
        return 0;
    }

    unsigned char header[54];
    fread(header, 1, 54, file);

    int width = *(int*)&header[18];
    int height = *(int*)&header[22];
    int imageSize = 3 * width * height;

    unsigned char* data = new unsigned char[imageSize];
    fread(data, 1, imageSize, file);
    fclose(file);

    for (int i = 0; i < imageSize; i += 3) {
        unsigned char temp = data[i];
        data[i] = data[i + 2];
        data[i + 2] = temp;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        width,
        height,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        data
    );

    delete[] data;
    return textureID;
} 

// Fungsi pembantu lingkaran 3D tipis untuk rel melingkar piringan
void drawCircle3D(float radius, int segments, float yOffset) {
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= segments; i++) {
        // Menggunakan PI dari globals.h atau M_PI
        float angle = 2.0f * PI * i / segments; 
        float x = radius * std::cos(angle);
        float z = radius * std::sin(angle);
        glVertex3f(x, yOffset, z);
    }
    glEnd();
}

// Fungsi pembantu silinder solid untuk platform piringan
void drawCylinderSimple(float radius, float height, int segments) {
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * PI * i / segments;
        float x = radius * std::cos(angle);
        float z = radius * std::sin(angle);
        
        // Normal vektor untuk kalkulasi pencahayaan silinder yang halus
        glNormal3f(std::cos(angle), 0.0f, std::sin(angle));
        glVertex3f(x, -height / 2.0f, z);
        glVertex3f(x, height / 2.0f, z);
    }
    glEnd();
    
    // Tutup Atas (Normal mengarah ke +Y)
    glNormal3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, height / 2.0f, 0.0f);
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * PI * i / segments;
        glVertex3f(radius * std::cos(angle), height / 2.0f, radius * std::sin(angle));
    }
    glEnd();

    // Tutup Bawah (Normal mengarah ke -Y)
    glNormal3f(0.0f, -1.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, -height / 2.0f, 0.0f);
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * PI * i / segments;
        glVertex3f(radius * std::cos(angle), -height / 2.0f, radius * std::sin(angle));
    }
    glEnd();
}

//  ===================================================== ===================================================== =====================================================

// -- Pohon --------------------------------------------
void initTrees() {
    float seed = 7.0f;
    for (int i = 0; i < JUMLAH_POHON; i++) {
        float side = (i % 2 == 0) ? 1.0f : -1.0f;

        seed       = fmod(seed * 17.3f + 31.7f, 100.0f);
        float xOff = seed * 0.09f + 3.0f;

        seed       = fmod(seed * 13.1f + 19.3f, 100.0f);
        float zPos = (seed / 100.0f) * REL_PANJANG - REL_PANJANG * 0.5f;

        seed      = fmod(seed * 11.7f + 7.3f, 100.0f);
        float sc  = 0.7f + seed * 0.008f; // variasi ukuran

        trees[i].x     = side * xOff;
        trees[i].z     = zPos;
        trees[i].scale = sc;
    }
}

void drawTree(float x, float z, float sc) {
    glPushMatrix();
    glTranslatef(x, -0.73f, z);
    glScalef(sc, sc, sc);

    // gluCylinder tumbuh ke +Z lokal.
    // Rotasi -90 derajat di X mengubah +Z lokal -> +Y dunia, sehingga pohon tegak.
    glRotatef(-90.0f, 1, 0, 0);

    GLUquadric *q = gluNewQuadric();

    // Batang (coklat kayu)
    if (lightingEnabled) setMaterial(0.48f, 0.30f, 0.10f, 0.05f, 0.05f, 0.03f, 4.0f);
    else                 glColor3f(0.48f, 0.30f, 0.10f);
    gluCylinder(q, 0.13f, 0.09f, 1.6f, 8, 3);

    // Tiga tingkat daun (kerucut hijau).
    if (lightingEnabled)
        setMaterial(0.12f, 0.58f, 0.12f, 0.05f, 0.15f, 0.05f, 4.0f);
    else
        glColor3f(0.12f, 0.58f, 0.12f);

    // Daun tetap lancip ke atas
    glTranslatef(0.0f, 0.0f, 1.30f);
    gluCylinder(q, 0.80f, 0.0f, 1.3f, 12, 3);

    glTranslatef(0.0f, 0.0f, 0.75f);
    gluCylinder(q, 0.60f, 0.0f, 1.1f, 12, 3);

    glTranslatef(0.0f, 0.0f, 0.65f);
    gluCylinder(q, 0.38f, 0.0f, 0.9f, 12, 3);

    gluDeleteQuadric(q);
    glPopMatrix();
}

void drawAllTrees() {
    for (int i = 0; i < JUMLAH_POHON; i++)
        drawTree(trees[i].x, trees[i].z, trees[i].scale);
}

//  ===================================================== ===================================================== =====================================================

// Fungsi Helper Gambar Objek Rintangan 3D Primitif dengan Warna Representatif
void DrawProceduralObstacle3D(ObstacleType type, float x, float z) {
    if (type == NONE) return;

    glPushMatrix();
    glTranslatef(x, -0.4f, z); // Berdiri tegak di permukaan rel

    switch(type) {
        case ORANG:
            glColor3f(1.0f, 0.75f, 0.8f); // Merah Muda (Kulit)
            glTranslatef(0.0f, 0.4f, 0.0f);
            glutSolidSphere(0.3f, 8, 8);  // Kepala
            glTranslatef(0.0f, -0.5f, 0.0f);
            glScalef(0.4f, 0.8f, 0.3f);
            glutSolidCube(1.0f);          // Badan
            break;
        case POHON_TUMBANG:
            glColor3f(0.55f, 0.27f, 0.07f); // Coklat Batang Kayu
            glRotatef(90.0f, 0.0f, 0.0f, 1.0f); // Tumbang melintang di atas rel
            glScalef(0.2f, 2.5f, 0.2f);
            glutSolidCube(1.0f);
            break;
        case BATU_BESAR:
            glColor3f(0.5f, 0.5f, 0.5f); // Abu-abu
            glutSolidSphere(0.7f, 6, 6);  // Batu bersudut tajam
            break;
        case HEWAN:
            glColor3f(1.0f, 0.5f, 0.0f); // Oranye
            glScalef(0.5f, 0.4f, 0.8f);
            glutSolidCube(1.0f);
            break;
        case LUBANG:
            glColor3f(0.05f, 0.05f, 0.05f); // Hitam Pekat
            glScalef(1.2f, 0.01f, 1.5f);
            glutSolidCube(1.0f);
            break;
        case BOM:
            glColor3f(0.2f, 0.2f, 0.2f);
            glutSolidSphere(0.4f, 10, 10); // Sferis Bom
            break;
        case GERBANG_TERTUTUP:
            glColor3f(0.8f, 0.1f, 0.1f); // Palang Merah
            glScalef(2.2f, 0.15f, 0.15f);
            glutSolidCube(1.0f);
            break;
        case MOBIL:
            glColor3f(0.1f, 0.2f, 0.8f); // Biru
            glScalef(1.2f, 0.8f, 2.0f);
            glutSolidCube(1.0f);
            break;
        default:
            glColor3f(0.7f, 0.7f, 0.3f);
            glutSolidCube(0.8f);
            break;
    }
    glPopMatrix();
}

// Fungsi Helper Menggambar Visual Cue Berupa Lampu Sinyal Menjelang Percabangan
void DrawVisualCueSign(float turnZStart, int turnNum) {
    glPushMatrix();
    glTranslatef(3.5f, 0.5f, turnZStart - 5.0f); // Tempatkan di bahu kanan rel utama sebelum percabangan
    
    // Tiang Sinyal
    glColor3f(0.3f, 0.3f, 0.3f);
    glPushMatrix(); glScalef(0.15f, 2.0f, 0.15f); glutSolidCube(1.0f); glPopMatrix();
    
    // Papan Indikator Turn Text Neon
    glTranslatef(0.0f, 1.0f, 0.0f);
    glColor3f(0.0f, 1.0f, 0.9f); // Cyan terang
    glPushMatrix(); glScalef(1.2f, 0.5f, 0.1f); glutSolidCube(1.0f); glPopMatrix();
    glPopMatrix();
}

//  ===================================================== ===================================================== =====================================================

// Fungsi pembantu untuk menggambar segmen rel lurus pendek
void drawSingleRailSegment(float currentX, float currentZ, float segmentLength) {
    // Menggambar bantalan kayu rel (Sleeper)
    if (lightingEnabled) setMaterial(0.3f, 0.15f, 0.05f, 0.1f, 0.1f, 0.1f, 5.0f);
    else glColor3f(0.3f, 0.15f, 0.05f);
    
    glPushMatrix();
    glTranslatef(currentX, -0.68f, currentZ);
    glScalef(1.8f, 0.05f, segmentLength); // Lebar bantalan kayu dibuat pas dengan rel
    glutSolidCube(1.0f);
    glPopMatrix();

    // Menggambar sepasang rel baja (Kiri & Kanan)
    if (lightingEnabled) setMaterial(0.6f, 0.6f, 0.6f, 0.8f, 0.8f, 0.8f, 32.0f);
    else glColor3f(0.6f, 0.6f, 0.6f);

    float trackGauge = 1.0f; // Jarak antar batang rel baja
    
    // Rel Baja Kiri
    glPushMatrix();
    glTranslatef(currentX - (trackGauge / 2.0f), -0.63f, currentZ);
    glScalef(0.08f, 0.06f, segmentLength);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Rel Baja Kanan
    glPushMatrix();
    glTranslatef(currentX + (trackGauge / 2.0f), -0.63f, currentZ);
    glScalef(0.08f, 0.06f, segmentLength);
    glutSolidCube(1.0f);
    glPopMatrix();
}



// Rombong Total Konstruksi Lintasan 3D Kereta Api (10 Turns Loop Spline Structure)
void drawRail() {
    float step = 1.5f; // Resolusi per segmen rel

    for (int tNum = 0; tNum < TOTAL_TURNS; tNum++) {
        float turnStartAbsZ = gameTurns[tNum].startZ;
        
        // Gambar Visual Cue Penanda Masuk Turn Baru
        DrawVisualCueSign(turnStartAbsZ, tNum + 1);

        // Render Geometri Rel Sepanjang 100 Unit per Turn
        for (float localZ = -140.0f; localZ < TURN_DISTANCE; localZ += step) {
            float zAbsPos = turnStartAbsZ - localZ;

            // --- PHASE 1: SPLIT PATH (0 s/d 30) ---
            if (localZ >= 0.0f && localZ < 30.0f) {
                float t = localZ / 30.0f;
                float smoothT = t * t * (3.0f - 2.0f * t);
                
                // Jalur Tengah (Lurus Tetap)
                drawSingleRailSegment(0.0f, zAbsPos, step);
                // Jalur Kiri (Melebar Halus ke X = -7.0f)
                drawSingleRailSegment(smoothT * -7.0f, zAbsPos, step);
                // Jalur Kanan (Melebar Halus ke X = 7.0f)
                drawSingleRailSegment(smoothT * 7.0f, zAbsPos, step);
            }
            // --- PHASE 2: STRAIGHT PATH (30 s/d 50) ---
            else if (localZ >= 30.0f && localZ < 50.0f) {
                drawSingleRailSegment(-7.0f, zAbsPos, step);
                drawSingleRailSegment(0.0f, zAbsPos, step);
                drawSingleRailSegment(7.0f, zAbsPos, step);

                // SPAWN TRIGGER: Render Model Rintangan 3D Prosedural tepat di tengah zona (localZ == 40)
                if (localZ >= 39.0f && localZ <= 40.5f) {
                    DrawProceduralObstacle3D(gameTurns[tNum].kiriType, -7.0f, zAbsPos);
                    DrawProceduralObstacle3D(gameTurns[tNum].tengahType, 0.0f, zAbsPos);
                    DrawProceduralObstacle3D(gameTurns[tNum].kananType, 7.0f, zAbsPos);
                }
            }
            // --- PHASE 3: MERGE PATH (50 s/d 80) ---
            else if (localZ >= 50.0f && localZ < 80.0f) {
                float t = (localZ - 50.0f) / 30.0f;
                float smoothT = t * t * (3.0f - 2.0f * t);

                // Jalur Tengah (Lurus)
                drawSingleRailSegment(0.0f, zAbsPos, step);
                // Jalur Kiri menyempit masuk kembali ke tengah
                drawSingleRailSegment(-7.0f * (1.0f - smoothT), zAbsPos, step);
                // Jalur Kanan menyempit masuk kembali ke tengah
                drawSingleRailSegment(7.0f * (1.0f - smoothT), zAbsPos, step);
            }
            // --- PHASE 4: SINGLE CORE TRACK (80 s/d 100) ---
            else {
                drawSingleRailSegment(0.0f, zAbsPos, step);
            }
        }
    }
}

//  ===================================================== ===================================================== =====================================================

// -- Tanah --------------------------------------------
void drawGround() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texDaun);

    if (lightingEnabled) {
        setMaterial(1.0f, 1.0f, 1.0f, 0.03f, 0.07f, 0.03f, 2.0f);
    } else {
        glColor3f(1.0f, 1.0f, 1.0f);
    }

    glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);

        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-100.0f, -0.73f, 160.0f);

        glTexCoord2f(250.0f, 0.0f);
        glVertex3f(100.0f, -0.73f, 160.0f);

        glTexCoord2f(250.0f, 500.0f);
        glVertex3f(100.0f, -0.73f, -1300.0f);

        glTexCoord2f(0.0f, 500.0f);
        glVertex3f(-100.0f, -0.73f, -1300.0f);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

//  ===================================================== ===================================================== =====================================================

// Fungsi helper untuk menggambar gerbang sederhana
void drawGate(float zPos, const char* label) {
    glPushMatrix();
    glTranslatef(0.0f, -0.73f, zPos); // Berdiri di atas tanah

    // Tiang Kiri
    glPushMatrix();
    glTranslatef(-2.5f, 2.0f, 0.0f);
    if (lightingEnabled) setMaterial(0.3f, 0.3f, 0.3f, 0.5f, 0.5f, 0.5f, 10.0f);
    else glColor3f(0.3f, 0.3f, 0.3f);
    glScalef(0.3f, 4.0f, 0.3f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Tiang Kanan
    glPushMatrix();
    glTranslatef(2.5f, 2.0f, 0.0f);
    glScalef(0.3f, 4.0f, 0.3f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Palang Atas (Berpola Kotak-kotak / Warna Kontras)
    glPushMatrix();
    glTranslatef(0.0f, 4.0f, 0.0f);
    if (lightingEnabled) setMaterial(0.9f, 0.9f, 0.1f, 0.8f, 0.8f, 0.8f, 32.0f); // Kuning cerah
    else glColor3f(0.9f, 0.9f, 0.1f);
    glScalef(5.3f, 0.5f, 0.4f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPopMatrix();
}

void drawStartGates() {
    drawGate(startZ, "START");
}

void drawFinishAreaZone() {
    // AREA FINISH / WIN AREA (Tepat di akhir Turn 10, Z = -1000)
    float finishLineZ = - (TOTAL_TURNS * TURN_DISTANCE);
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 1.0f, 0.4f, 0.4f); // Neon Box Area Finish Hijau Transparan
    glBegin(GL_QUADS);
        glVertex3f(-3.0f, -0.6f, finishLineZ - 10.0f);
        glVertex3f(3.0f, -0.6f, finishLineZ - 10.0f);
        glVertex3f(3.0f, -0.6f, finishLineZ + 10.0f);
        glVertex3f(-3.0f, -0.6f, finishLineZ + 10.0f);
    glEnd();
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}
