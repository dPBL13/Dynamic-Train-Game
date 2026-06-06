#define _USE_MATH_DEFINES 
#include <cmath>          
#include <cstdio>
#include "world.h"
#include "lighting.h"
#include "globals.h"

// =============================
// 1. Rel, Tanah, Pohon, Gunung
// =============================


// Pointer quadric global agar tidak alokasi-dealokasi setiap frame
GLUquadric *quadricPohon = NULL;

GLuint loadBMP(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Texture file not found: %s\n", filename);
        return 0;
    }

    unsigned char header[54];
    if (fread(header, 1, 54, file) != 54) {
        printf("Invalid BMP header: %s\n", filename);
        fclose(file);
        return 0;
    }

    int width = *(int*)&header[18];
    int height = *(int*)&header[22];
    int imageSize = 3 * width * height;

    unsigned char* data = new unsigned char[imageSize];
    if (fread(data, 1, imageSize, file) != (size_t)imageSize) {
        printf("Corrupted BMP data: %s\n", filename);
        delete[] data;
        fclose(file);
        return 0;
    }
    fclose(file);

    // Konversi format warna BGR ke RGB internal OpenGL
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

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    delete[] data;
    return textureID;
} 

void drawCircle3D(float radius, int segments, float yOffset) {
    glBegin(GL_LINE_LOOP);
    glNormal3f(0.0f, 1.0f, 0.0f);
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * PI * i / segments; 
        float x = radius * std::cos(angle);
        float z = radius * std::sin(angle);
        glVertex3f(x, yOffset, z);
    }
    glEnd();
}

void drawCylinderSimple(float radius, float height, int segments) {
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * PI * i / segments;
        float x = radius * std::cos(angle);
        float z = radius * std::sin(angle);
        
        glNormal3f(std::cos(angle), 0.0f, std::sin(angle));
        glVertex3f(x, -height / 2.0f, z);
        glVertex3f(x, height / 2.0f, z);
    }
    glEnd();
    
    // Tutup Atas
    glNormal3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, height / 2.0f, 0.0f);
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * PI * i / segments;
        glVertex3f(radius * std::cos(angle), height / 2.0f, radius * std::sin(angle));
    }
    glEnd();

    // Tutup Bawah
    glNormal3f(0.0f, -1.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, -height / 2.0f, 0.0f);
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * PI * i / segments;
        glVertex3f(radius * std::cos(angle), -height / 2.0f, radius * std::sin(angle));
    }
    glEnd();
}

// ===================================================================================================

void initTrees() {
    // Inisialisasi quadric sekali saja di sini
    if (quadricPohon == NULL) {
        quadricPohon = gluNewQuadric();
    }

    float seed = 7.0f;
    for (int i = 0; i < JUMLAH_POHON; i++) {
        float side = (i % 2 == 0) ? 1.0f : -1.0f;

        seed       = fmod(seed * 17.3f + 31.7f, 100.0f);
        float xOff = seed * 0.09f + 3.0f;

        seed       = fmod(seed * 13.1f + 19.3f, 100.0f);
        float zPos = (seed / 100.0f) * REL_PANJANG - REL_PANJANG * 0.5f;

        seed      = fmod(seed * 11.7f + 7.3f, 100.0f);
        float sc  = 0.7f + seed * 0.008f; 

        trees[i].x     = side * xOff;
        trees[i].z     = zPos;
        trees[i].scale = sc;
    }
}

void drawTree(float x, float z, float sc) {
    if (!quadricPohon) return;

    glPushMatrix();
    glTranslatef(x, -0.73f, z);
    glScalef(sc, sc, sc);
    glRotatef(-90.0f, 1, 0, 0);

    // Batang (coklat kayu)
    if (lightingEnabled) setMaterial(0.48f, 0.30f, 0.10f, 0.05f, 0.05f, 0.03f, 4.0f);
    else glColor3f(0.48f, 0.30f, 0.10f);
    gluCylinder(quadricPohon, 0.13f, 0.09f, 1.6f, 8, 3);

    // Tiga tingkat daun (kerucut hijau)
    if (lightingEnabled) setMaterial(0.12f, 0.58f, 0.12f, 0.05f, 0.15f, 0.05f, 4.0f);
    else glColor3f(0.12f, 0.58f, 0.12f);

    glTranslatef(0.0f, 0.0f, 1.30f);
    gluCylinder(quadricPohon, 0.80f, 0.0f, 1.3f, 12, 3);

    glTranslatef(0.0f, 0.0f, 0.75f);
    gluCylinder(quadricPohon, 0.60f, 0.0f, 1.1f, 12, 3);

    glTranslatef(0.0f, 0.0f, 0.65f);
    gluCylinder(quadricPohon, 0.38f, 0.0f, 0.9f, 12, 3);

    glPopMatrix();
}

void drawAllTrees() {
    for (int i = 0; i < JUMLAH_POHON; i++)
        drawTree(trees[i].x, trees[i].z, trees[i].scale);
}

// ===================================================================================================

void DrawProceduralObstacle3D(ObstacleType type, float x, float z) {
    if (type == NONE) return;

    glPushMatrix();
    glTranslatef(x, -0.4f, z); 

    switch(type) {
        case ORANG:
            glColor3f(1.0f, 0.75f, 0.8f); 
            glPushMatrix();
                glTranslatef(0.0f, 0.4f, 0.0f);
                glutSolidSphere(0.3f, 8, 8);  
            glPopMatrix();
            glPushMatrix();
                glScalef(0.4f, 0.8f, 0.3f);
                glutSolidCube(1.0f);          
            glPopMatrix();
            break;
        case POHON_TUMBANG:
            glColor3f(0.55f, 0.27f, 0.07f); 
            glRotatef(90.0f, 0.0f, 0.0f, 1.0f); 
            glScalef(0.2f, 2.5f, 0.2f);
            glutSolidCube(1.0f);
            break;
        case BATU_BESAR:
            glColor3f(0.5f, 0.5f, 0.5f); 
            glutSolidSphere(0.7f, 6, 6);  
            break;
        case HEWAN:
            glColor3f(1.0f, 0.5f, 0.0f); 
            glScalef(0.5f, 0.4f, 0.8f);
            glutSolidCube(1.0f);
            break;
        case LUBANG:
            glDisable(GL_LIGHTING); // Lubang tidak memantulkan cahaya agar tetap hitam pekat murni
            glColor3f(0.05f, 0.05f, 0.05f); 
            glScalef(1.2f, 0.01f, 1.5f);
            glutSolidCube(1.0f);
            glEnable(GL_LIGHTING);
            break;
        case BOM:
            glColor3f(0.2f, 0.2f, 0.2f);
            glutSolidSphere(0.4f, 10, 10); 
            break;
        case GERBANG_TERTUTUP:
            glColor3f(0.8f, 0.1f, 0.1f); 
            glScalef(2.2f, 0.15f, 0.15f);
            glutSolidCube(1.0f);
            break;
        case MOBIL:
            glColor3f(0.1f, 0.2f, 0.8f); 
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

void DrawVisualCueSign(float turnZStart, int turnNum) {
    glPushMatrix();
    glTranslatef(3.5f, 0.5f, turnZStart - 5.0f); 
    
    // Tiang Sinyal
    glColor3f(0.3f, 0.3f, 0.3f);
    glPushMatrix(); glScalef(0.15f, 2.0f, 0.15f); glutSolidCube(1.0f); glPopMatrix();
    
    // Papan Indikator
    glTranslatef(0.0f, 1.0f, 0.0f);
    glColor3f(0.0f, 1.0f, 0.9f); 
    glPushMatrix(); glScalef(1.2f, 0.5f, 0.1f); glutSolidCube(1.0f); glPopMatrix();
    glPopMatrix();
}

// ===================================================================================================

void drawSingleRailSegment(float currentX, float currentZ, float segmentLength) {
    // Bantalan kayu rel (Sleeper)
    if (lightingEnabled) setMaterial(0.3f, 0.15f, 0.05f, 0.1f, 0.1f, 0.1f, 5.0f);
    else glColor3f(0.3f, 0.15f, 0.05f);
    
    glPushMatrix();
    glTranslatef(currentX, -0.68f, currentZ);
    glScalef(1.8f, 0.05f, segmentLength); 
    glutSolidCube(1.0f);
    glPopMatrix();

    // Sepasang rel baja (Kiri & Kanan)
    if (lightingEnabled) setMaterial(0.6f, 0.6f, 0.6f, 0.8f, 0.8f, 0.8f, 32.0f);
    else glColor3f(0.6f, 0.6f, 0.6f);

    float trackGauge = 1.0f; 
    
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

void drawRail() {
    float step = 1.5f; 

    for (int tNum = 0; tNum < TOTAL_TURNS; tNum++) {
        float turnStartAbsZ = gameTurns[tNum].startZ;
        
        DrawVisualCueSign(turnStartAbsZ, tNum + 1);

        for (float localZ = -140.0f; localZ < TURN_DISTANCE; localZ += step) {
            float zAbsPos = turnStartAbsZ - localZ;

            // --- PHASE 1: SPLIT PATH (0 s/d 30) ---
            if (localZ >= 0.0f && localZ < 30.0f) {
                float t = localZ / 30.0f;
                float smoothT = t * t * (3.0f - 2.0f * t);
                
                drawSingleRailSegment(0.0f, zAbsPos, step);
                drawSingleRailSegment(smoothT * -7.0f, zAbsPos, step);
                drawSingleRailSegment(smoothT * 7.0f, zAbsPos, step);
            }
            // --- PHASE 2: STRAIGHT PATH (30 s/d 50) ---
            else if (localZ >= 30.0f && localZ < 50.0f) {
                drawSingleRailSegment(-7.0f, zAbsPos, step);
                drawSingleRailSegment(0.0f, zAbsPos, step);
                drawSingleRailSegment(7.0f, zAbsPos, step);

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

                drawSingleRailSegment(0.0f, zAbsPos, step);
                drawSingleRailSegment(-7.0f * (1.0f - smoothT), zAbsPos, step);
                drawSingleRailSegment(7.0f * (1.0f - smoothT), zAbsPos, step);
            }
            // --- PHASE 4: SINGLE CORE TRACK (80 s/d 100) ---
            else {
                drawSingleRailSegment(0.0f, zAbsPos, step);
            }
        }
    }
}

// ===================================================================================================

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
        glTexCoord2f(0.0f, 0.0f);     glVertex3f(-100.0f, -0.73f, 160.0f);
        glTexCoord2f(250.0f, 0.0f);   glVertex3f(100.0f, -0.73f, 160.0f);
        glTexCoord2f(250.0f, 500.0f); glVertex3f(100.0f, -0.73f, -1300.0f);
        glTexCoord2f(0.0f, 500.0f);    glVertex3f(-100.0f, -0.73f, -1300.0f);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

// ===================================================================================================

void drawGate(float zPos, const char* label) {
    glPushMatrix();
    glTranslatef(0.0f, -0.73f, zPos); 

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

    // Palang Atas
    glPushMatrix();
    glTranslatef(0.0f, 4.0f, 0.0f);
    if (lightingEnabled) setMaterial(0.9f, 0.9f, 0.1f, 0.8f, 0.8f, 0.8f, 32.0f); 
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
    float finishLineZ = - (TOTAL_TURNS * TURN_DISTANCE);
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 1.0f, 0.4f, 0.4f); 
    glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f); // Ditambahkan normal agar konsisten
        glVertex3f(-3.0f, -0.6f, finishLineZ - 10.0f);
        glVertex3f(3.0f, -0.6f, finishLineZ - 10.0f);
        glVertex3f(3.0f, -0.6f, finishLineZ + 10.0f);
        glVertex3f(-3.0f, -0.6f, finishLineZ + 10.0f);
    glEnd();
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}
