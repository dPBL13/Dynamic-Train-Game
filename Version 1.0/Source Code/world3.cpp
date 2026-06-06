#include "globals.h"
#include <GL/glut.h>
#include "world3.h"
#include <cmath>

// ============================= ============================= =====
// 3. Dekor part 2 (plus texture handling) & generate environemnet
// ============================= ============================= =====

// Pengaturan batas maksimum array memori
const int MAX_DECORATIONS = 200;
StaticDecoration decorList[MAX_DECORATIONS];
int totalDecorations = 0;

// Variabel posisi Z kereta eksternal untuk Culling Jarak 
GLuint hillTextureID = 0;

// ===================================================================
// 1. DATA INITIALIZATION: STRATEGI KLUSTER PERUMAHAN DUNIA NYATA
// ===================================================================
void initEnvironmentData() {
    totalDecorations = 0;

    // --- BLOK A: SISI KIRI (X = -90.0f s/d -12.0f) ---
    // Kluster Kampung Padat 1 (Z: -100 s/d -220) -> Jarak Rapat, Rotasi Organik
    float currentZ = -100.0f;
    while (currentZ > -220.0f && totalDecorations < MAX_DECORATIONS) {
        // Tiga rumah berdempetan rapat (Sisi Kiri Jauh)
        decorList[totalDecorations++] = { 1, -45.0f, -0.73f, currentZ,        12.0f, 1.0f, 1.0f, 1.0f };
        decorList[totalDecorations++] = { 3, -42.0f, -0.73f, currentZ - 8.0f,  -5.0f, 1.0f, 1.0f, 1.0f };
        decorList[totalDecorations++] = { 1, -46.0f, -0.73f, currentZ - 16.0f,  0.0f, 1.0f, 1.0f, 1.0f };
        
        // Sepasang ruko di depan jalan (Sisi Kiri Dekat)
        decorList[totalDecorations++] = { 2, -22.0f, -0.73f, currentZ - 4.0f,  90.0f, 1.0f, 1.0f, 1.0f };
        decorList[totalDecorations++] = { 2, -22.0f, -0.73f, currentZ - 12.0f, 90.0f, 1.0f, 1.0f, 1.0f };

        // Deretan Tiang Listrik di tepi jalan
        decorList[totalDecorations++] = { 5, -13.0f, -0.73f, currentZ,         0.0f, 1.0f, 1.0f, 1.0f };
        decorList[totalDecorations++] = { 5, -13.0f, -0.73f, currentZ - 25.0f,  0.0f, 1.0f, 1.0f, 1.0f };

        currentZ -= 45.0f; // Jeda spasi sedang menuju sub-blok berikutnya
    }

    // Kluster Kampung Padat 2 (Z: -300 s/d -600) -> Rumah Limas & Gedung Fasum
    currentZ = -300.0f;
    while (currentZ > -600.0f && totalDecorations < MAX_DECORATIONS) {
        if (currentZ == -450.0f) {
            // Taruh Gedung Fasilitas Umum di tengah kampung
            decorList[totalDecorations++] = { 4, -35.0f, -0.73f, currentZ, 0.0f, 1.2f, 1.2f, 1.2f };
        } else {
            decorList[totalDecorations++] = { 3, -50.0f, -0.73f, currentZ,        35.0f, 1.0f, 1.0f, 1.0f };
            decorList[totalDecorations++] = { 1, -40.0f, -0.73f, currentZ - 7.0f,  -20.0f, 1.0f, 1.0f, 1.0f };
        }
        decorList[totalDecorations++] = { 5, -13.0f, -0.73f, currentZ, 0.0f, 1.0f, 1.0f, 1.0f };
        currentZ -= 35.0f;
    }

    // --- BLOK B: SISI KANAN (X = 12.0f s/d 90.0f) ---
    // Kluster Pertokoan / Ruko Modern Linear (Z: -150 s/d -500)
    currentZ = -150.0f;
    while (currentZ > -500.0f && totalDecorations < MAX_DECORATIONS) {
        // Barisan ruko rapat terstruktur
        decorList[totalDecorations++] = { 2, 22.0f, -0.73f, currentZ,        -90.0f, 1.0f, 1.0f, 1.0f };
        decorList[totalDecorations++] = { 2, 22.0f, -0.73f, currentZ - 6.0f,  -90.0f, 1.0f, 1.0f, 1.0f };
        decorList[totalDecorations++] = { 2, 22.0f, -0.73f, currentZ - 12.0f, -90.0f, 1.0f, 1.0f, 1.0f };
        
        // Rumah warga di belakang ruko (Sisi Kanan Luar)
        decorList[totalDecorations++] = { 1, 48.0f, -0.73f, currentZ - 3.0f,   180.0f, 1.0f, 1.0f, 1.0f };
        decorList[totalDecorations++] = { 3, 54.0f, -0.73f, currentZ - 10.0f,  145.0f, 1.0f, 1.0f, 1.0f };

        // Tiang listrik sisi kanan
        decorList[totalDecorations++] = { 5, 13.0f, -0.73f, currentZ, 0.0f, 1.0f, 1.0f, 1.0f };

        currentZ -= 65.0f; // Jeda lahan kosong luas antar-kompleks ruko
    }

    // Generasi sisa area Z ekstrem (-600 s/d -1200) secara berkala
    for (float zSisa = -650.0f; zSisa >= -1150.0f; zSisa -= 80.0f) {
        if (totalDecorations >= MAX_DECORATIONS) break;
        decorList[totalDecorations++] = { 1, -38.0f, -0.73f, zSisa,         45.0f, 1.0f, 1.0f, 1.0f };
        decorList[totalDecorations++] = { 3,  42.0f, -0.73f, zSisa + 15.0f, -45.0f, 1.0f, 1.0f, 1.0f };
        decorList[totalDecorations++] = { 5, -13.0f, -0.73f, zSisa,           0.0f, 1.0f, 1.0f, 1.0f };
        decorList[totalDecorations++] = { 5,  13.0f, -0.73f, zSisa + 15.0f,  0.0f, 1.0f, 1.0f, 1.0f };
    }
}

// ===================================================================
// 2. RENDER PIPELINE: DRAW ENVIRONMENT WITH DISTANCE CULLING
// ===================================================================
void drawStaticEnvironment() {
    // A. INFRASTRUKTUR JALAN RAYA ASFIKSIA & PEMBATAS HORIZONTAL BLOCK
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glColor3f(0.15f, 0.15f, 0.15f); // Warna aspal gelap

    // 1. Jalan Utama Longitudinal Kiri (-10) & Kanan (10)
    float mainRoadX[2] = { -10.0f, 10.0f };
    for (int r = 0; r < 2; r++) {
        for (float zSeg = 160.0f; zSeg >= -1300.0f; zSeg -= 100.0f) {
            if (fabsf(zSeg - posZ) > 280.0f) continue;
            glBegin(GL_QUADS);
                glVertex3f(mainRoadX[r] - 1.25f, -0.72f, zSeg);
                glVertex3f(mainRoadX[r] + 1.25f, -0.72f, zSeg);
                glVertex3f(mainRoadX[r] + 1.25f, -0.72f, zSeg - 100.0f);
                glVertex3f(mainRoadX[r] - 1.25f, -0.72f, zSeg - 100.0f);
            glEnd();
        }
    }

    // 2. Jalan Penghubung Horizontal Antar-Blok
    float crossRoadZ[] = { -240.0f, -620.0f, -980.0f };
    for (int j = 0; j < 3; j++) {
        if (fabsf(crossRoadZ[j] - posZ) > 250.0f) continue;
        glBegin(GL_QUADS);
            // Sayap Kiri
            glVertex3f(-90.0f, -0.72f, crossRoadZ[j] + 2.0f);
            glVertex3f(-8.75f, -0.72f, crossRoadZ[j] + 2.0f);
            glVertex3f(-8.75f, -0.72f, crossRoadZ[j] - 2.0f);
            glVertex3f(-90.0f, -0.72f, crossRoadZ[j] - 2.0f);

            // Sayap Kanan
            glVertex3f(8.75f,  -0.72f, crossRoadZ[j] + 2.0f);
            glVertex3f(90.0f,  -0.72f, crossRoadZ[j] + 2.0f);
            glVertex3f(90.0f,  -0.72f, crossRoadZ[j] - 2.0f);
            glVertex3f(8.75f,  -0.72f, crossRoadZ[j] - 2.0f);
        glEnd();
    }
    glEnable(GL_LIGHTING);
    glPopMatrix();

    // B. ITERASI DRAWING ASET STATIS BERBASIS ARRAY (CULLING RADIUS 250)
    StaticDecoration* lastPoleKiri = nullptr;
    StaticDecoration* lastPoleKanan = nullptr;

    for (int i = 0; i < totalDecorations; i++) {
        StaticDecoration& decor = decorList[i];

        if (fabsf(decor.z - posZ) > 250.0f) continue;

        glPushMatrix();
        glTranslatef(decor.x, decor.y, decor.z);
        glRotatef(decor.rotation, 0.0f, 1.0f, 0.0f);
        glScalef(decor.scaleX, decor.scaleY, decor.scaleZ);

        switch (decor.type) {
            case 1: drawHouseType1(); break;
            case 2: drawHouseType2(); break;
            case 3: drawHouseType3(); break;
            case 4: drawHouseType4(); break;
            case 5: drawUtilityPole(); break;
        }
        glPopMatrix();

        // Koleksi bentangan kabel listrik dinamis antar tiang array
        if (decor.type == 5) {
            if (decor.x < 0.0f) {
                if (lastPoleKiri != nullptr && fabsf(lastPoleKiri->z - decor.z) < 100.0f) {
                    drawHangingCables(lastPoleKiri->x, lastPoleKiri->z, decor.x, decor.z, 4.2f);
                }
                lastPoleKiri = &decor;
            } else {
                if (lastPoleKanan != nullptr && fabsf(lastPoleKanan->z - decor.z) < 100.0f) {
                    drawHangingCables(lastPoleKanan->x, lastPoleKanan->z, decor.x, decor.z, 4.2f);
                }
                lastPoleKanan = &decor;
            }
        }
    }
    
    if (posZ < -350.0f) return; 

    // Loop pembuatan kluster perumahan di zona penyangga awal (Steril dari rel tengah)
    // Sisi Kiri Jalan Rel
    for (float zPos = 120.0f; zPos >= -120.0f; zPos -= 25.0f) {
        drawIndonesianRuko(-25.0f, zPos);
        drawTiangListrikKabel(-15.0f, zPos);
        drawIndonesianRuko(-55.0f, zPos + 5.0f);
        drawIndonesianRuko(-80.0f, zPos - 5.0f);
    }

    // Sisi Kanan Jalan Rel
    for (float zPos = 120.0f; zPos >= -120.0f; zPos -= 25.0f) {
        drawIndonesianRuko(25.0f, zPos);
        drawTiangListrikKabel(15.0f, zPos);
        drawIndonesianRuko(60.0f, zPos - 8.0f);
        drawIndonesianRuko(85.0f, zPos + 2.0f);
    }
}

// ===================================================================
// 3. LOGIKA VERTEKS KABEL LISTRIK MELENGKUNG (PARABOLA PIECEWISE)
// ===================================================================
void drawHangingCables(float x1, float z1, float x2, float z2, float height) {
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glColor3f(0.05f, 0.05f, 0.05f); // Kabel hitam legam
    glLineWidth(1.2f);

    glBegin(GL_LINE_STRIP);
    int segments = 6; 
    for (int i = 0; i <= segments; i++) {
        float t = (float)i / (float)segments;
        float currX = x1 + t * (x2 - x1);
        float currZ = z1 + t * (z2 - z1);
        
        float sag = 0.35f; 
        float currY = height - (sag * 4.0f * t * (1.0f - t)) - 0.73f;

        glVertex3f(currX, currY, currZ);
    }
    glEnd();
    
    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

// ===================================================================
// 4. GEOMETRI VARIASI MODEL RUMAH & MAPPING TEKSTUR
// ===================================================================

// --- TIPE 1: RUMAH SUBSIDI 1 LANTAI (Atap Prisma Segitiga) ---
void drawHouseType1() {
    glDisable(GL_TEXTURE_2D); 
    glColor3f(0.2f, 0.6f, 0.5f);
    glPushMatrix();
    glTranslatef(0.0f, 0.75f, 0.0f);
    glScalef(2.4f, 1.5f, 2.4f);
    glutSolidCube(1.0f);
    glPopMatrix();

    if (texRoof != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texRoof);
        glColor3f(1.0f, 1.0f, 1.0f); 
        
        glBegin(GL_TRIANGLES);
            // Sisi Depan Atap
            glNormal3f(0.0f, 0.707f, 0.707f);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.3f, 1.5f, 1.3f);
            glTexCoord2f(1.0f, 0.0f); glVertex3f(1.3f, 1.5f, 1.3f);
            glTexCoord2f(0.5f, 1.0f); glVertex3f(0.0f, 2.3f, 0.0f);
            
            // Sisi Belakang Atap
            glNormal3f(0.0f, 0.707f, -0.707f);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.3f, 1.5f, -1.3f);
            glTexCoord2f(1.0f, 0.0f); glVertex3f(1.3f, 1.5f, -1.3f);
            glTexCoord2f(0.5f, 1.0f); glVertex3f(0.0f, 2.3f, 0.0f);
        glEnd();
        glDisable(GL_TEXTURE_2D); 
    }
}

// --- TIPE 2: RUKO KOMERSIAL 2 LANTAI (Atap Dak Flat Modern) ---
void drawHouseType2() {
    glDisable(GL_TEXTURE_2D);
    // Lantai 1 
    glColor3f(0.18f, 0.35f, 0.52f);
    glPushMatrix();
    glTranslatef(0.0f, 0.8f, 0.0f);
    glScalef(2.6f, 1.6f, 3.0f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Lantai 2 
    glColor3f(0.85f, 0.85f, 0.85f);
    glPushMatrix();
    glTranslatef(0.0f, 2.3f, 0.0f);
    glScalef(2.6f, 1.4f, 3.0f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Atap Dak Flat
    if (texRoof != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texRoof);
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_QUADS);
            glNormal3f(0.0f, 1.0f, 0.0f);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.31f, 3.01f, 1.51f);
            glTexCoord2f(2.0f, 0.0f); glVertex3f(1.31f, 3.01f, 1.51f);
            glTexCoord2f(2.0f, 2.0f); glVertex3f(1.31f, 3.01f, -1.51f);
            glTexCoord2f(0.0f, 2.0f); glVertex3f(-1.31f, 3.01f, -1.51f);
        glEnd();
        glDisable(GL_TEXTURE_2D);
    }
}

// --- TIPE 3: RUMAH ATAP LIMAS TRADISIONAL (Koreksi Poligon Manual) ---
void drawHouseType3() {
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.82f, 0.71f, 0.55f);
    glPushMatrix();
    glTranslatef(0.0f, 0.7f, 0.0f);
    glScalef(2.5f, 1.4f, 2.5f);
    glutSolidCube(1.0f);
    glPopMatrix();

    if (texRoof != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texRoof);
        glColor3f(1.0f, 1.0f, 1.0f);
    }

    // Membangun Limas Segi Empat Prosedural Tertekstur Presisi
    glBegin(GL_TRIANGLES);
        // Sisi Depan
        glNormal3f(0.0f, 0.6f, 0.8f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.35f, 1.4f,  1.35f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.35f, 1.4f,  1.35f);
        glTexCoord2f(0.5f, 1.0f); glVertex3f( 0.0f,  2.5f,  0.0f);
        // Sisi Belakang
        glNormal3f(0.0f, 0.6f, -0.8f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.35f, 1.4f, -1.35f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.35f, 1.4f, -1.35f);
        glTexCoord2f(0.5f, 1.0f); glVertex3f( 0.0f,  2.5f,  0.0f);
        // Sisi Kiri
        glNormal3f(-0.8f, 0.6f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.35f, 1.4f, -1.35f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.35f, 1.4f,  1.35f);
        glTexCoord2f(0.5f, 1.0f); glVertex3f( 0.0f,  2.5f,  0.0f);
        // Sisi Kanan
        glNormal3f(0.8f, 0.6f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.35f, 1.4f,  1.35f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.35f, 1.4f, -1.35f);
        glTexCoord2f(0.5f, 1.0f); glVertex3f( 0.0f,  2.5f,  0.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

// --- TIPE 4: GEDUNG FASILITAS UMUM / BALAI DESA ---
void drawHouseType4() {
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.9f, 0.88f, 0.82f);
    glPushMatrix();
    glTranslatef(0.0f, 1.1f, 0.0f);
    glScalef(5.0f, 2.2f, 4.0f);
    glutSolidCube(1.0f);
    glPopMatrix();

    if (texRoof != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texRoof);
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_TRIANGLES);
            glNormal3f(-0.707f, 0.707f, 0.0f);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-2.6f, 2.2f,  2.1f);
            glTexCoord2f(3.0f, 0.0f); glVertex3f(-2.6f, 2.2f, -2.1f);
            glTexCoord2f(1.5f, 2.0f); glVertex3f( 0.0f, 3.4f,  0.0f);

            glNormal3f(0.707f, 0.707f, 0.0f);
            glTexCoord2f(0.0f, 0.0f); glVertex3f( 2.6f, 2.2f,  2.1f);
            glTexCoord2f(3.0f, 0.0f); glVertex3f( 2.6f, 2.2f, -2.1f);
            glTexCoord2f(1.5f, 2.0f); glVertex3f( 0.0f, 3.4f,  0.0f);
        glEnd();
        glDisable(GL_TEXTURE_2D);
    }
}

// --- TIPE 5: TIANG LISTRIK INFRASTRUKTUR & PENDARAN EMISSION ---
void drawUtilityPole() {
    if (texWood != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texWood);
        glColor3f(1.0f, 1.0f, 1.0f);
    } else {
        glDisable(GL_TEXTURE_2D);
        glColor3f(0.3f, 0.2f, 0.1f);
    }

    glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.06f, 0.0f,  0.06f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.06f, 0.0f,  0.06f);
        glTexCoord2f(1.0f, 5.0f); glVertex3f( 0.06f, 4.5f,  0.06f); 
        glTexCoord2f(0.0f, 5.0f); glVertex3f(-0.06f, 4.5f,  0.06f);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    // Palang Besi T
    glColor3f(0.25f, 0.25f, 0.25f);
    glPushMatrix();
    glTranslatef(0.0f, 4.2f, 0.0f);
    glScalef(1.2f, 0.1f, 0.12f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Lampu Jalan Neon Menyala Emisi
    GLfloat mat_bulb[] = { 1.0f, 1.0f, 0.5f, 1.0f };
    GLfloat mat_emission[] = { 0.6f, 0.6f, 0.2f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_bulb);
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);

    glPushMatrix();
    glTranslatef(0.5f, 4.0f, 0.0f); 
    glutSolidSphere(0.14f, 6, 6);
    glPopMatrix();

    GLfloat blank_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_EMISSION, blank_emission);
}

// ===================================================================
// 5. GRID 3D MESH HILLS REALISTIS (VALLEY AND PEAK TRIGONOMETRY)
// ===================================================================
void draw3DMeshHills() {
    float startZ = 160.0f;
    float endZ = -1300.0f;
    float totalLengthZ = startZ - endZ;
    float stepZ = 20.0f;  
    float stepX = 5.0f;   

    if (texRock != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texRock);
        glColor3f(1.0f, 1.0f, 1.0f); 
    } else {
        glDisable(GL_TEXTURE_2D);
        glColor3f(0.2f, 0.2f, 0.2f); 
    }

    for (float z = startZ; z >= endZ + stepZ; z -= stepZ) {
        if (fabsf(z - posZ) > 300.0f) continue;

        // Sayap Kiri Layer 1
        glBegin(GL_TRIANGLE_STRIP);
        for (float x = -100.0f; x <= -70.0f; x += stepX) {
            float slope = fabsf(x - (-70.0f)) / 30.0f; 
            float waveY = sinf(z * 0.05f) * cosf(x * 0.1f) * 22.0f + sinf(z * 0.01f) * 8.0f; 
            float finalY = (slope * waveY) - 0.73f;
            if (z > 120.0f || z < -1240.0f) finalY = -0.73f;

            float u = (x - (-100.0f)) / 30.0f;
            float v1 = (startZ - z) / totalLengthZ;
            glTexCoord2f(u * 2.0f, v1 * 50.0f); glVertex3f(x, finalY, z);

            float waveYNext = sinf((z - stepZ) * 0.05f) * cosf(x * 0.1f) * 22.0f + sinf((z - stepZ) * 0.01f) * 8.0f;
            float finalYNext = (slope * waveYNext) - 0.73f;
            if ((z - stepZ) > 120.0f || (z - stepZ) < -1240.0f) finalYNext = -0.73f;
            float v2 = (startZ - (z - stepZ)) / totalLengthZ;
            glTexCoord2f(u * 2.0f, v2 * 50.0f); glVertex3f(x, finalYNext, z - stepZ);
        }
        glEnd();

        // Sayap Kanan Layer 1
        glBegin(GL_TRIANGLE_STRIP);
        for (float x = 70.0f; x <= 100.0f; x += stepX) {
            float slope = fabsf(100.0f - x) / 30.0f; 
            float waveY = cosf(z * 0.04f) * sinf(x * 0.12f) * 22.0f + sinf(z * 0.012f) * 8.0f;
            float finalY = (slope * waveY) - 0.73f;
            if (z > 120.0f || z < -1240.0f) finalY = -0.73f;

            float u = (x - 70.0f) / 30.0f;
            float v1 = (startZ - z) / totalLengthZ;
            glTexCoord2f(u * 2.0f, v1 * 50.0f); glVertex3f(x, finalY, z);

            float waveYNext = cosf((z - stepZ) * 0.04f) * sinf(x * 0.12f) * 22.0f + sinf((z - stepZ) * 0.012f) * 8.0f;
            float finalYNext = (slope * waveYNext) - 0.73f;
            if ((z - stepZ) > 120.0f || (z - stepZ) < -1240.0f) finalYNext = -0.73f;
            float v2 = (startZ - (z - stepZ)) / totalLengthZ;
            glTexCoord2f(u * 2.0f, v2 * 50.0f); glVertex3f(x, finalYNext, z - stepZ);
        }
        glEnd();

        // LAYER 2: MEGA PEGUNUNGAN BELAKANG
        glBegin(GL_TRIANGLE_STRIP);
        for (float x = -140.0f; x <= -100.0f; x += stepX) {
            float slope = fabsf(x - (-100.0f)) / 40.0f; 
            float waveY = sinf(z * 0.03f) * 45.0f + cosf(x * 0.05f) * 20.0f; 
            float finalY = (slope * waveY) + 10.0f; 

            float u = (x - (-140.0f)) / 40.0f;
            float v1 = (startZ - z) / totalLengthZ;
            glTexCoord2f(u * 4.0f, v1 * 60.0f); glVertex3f(x, finalY, z);

            float waveYNext = sinf((z - stepZ) * 0.03f) * 45.0f + cosf(x * 0.05f) * 20.0f;
            float finalYNext = (slope * waveYNext) + 10.0f;
            float v2 = (startZ - (z - stepZ)) / totalLengthZ;
            glTexCoord2f(u * 4.0f, v2 * 60.0f); glVertex3f(x, finalYNext, z - stepZ);
        }
        glEnd();

        glBegin(GL_TRIANGLE_STRIP);
        for (float x = 100.0f; x <= 140.0f; x += stepX) {
            float slope = fabsf(140.0f - x) / 40.0f; 
            float waveY = cosf(z * 0.03f) * 45.0f + sinf(x * 0.05f) * 20.0f;
            float finalY = (slope * waveY) + 10.0f;

            float u = (x - 100.0f) / 40.0f;
            float v1 = (startZ - z) / totalLengthZ;
            glTexCoord2f(u * 4.0f, v1 * 60.0f); glVertex3f(x, finalY, z);

            float waveYNext = cosf((z - stepZ) * 0.03f) * 45.0f + sinf(x * 0.05f) * 20.0f;
            float finalYNext = (slope * waveYNext) + 10.0f;
            float v2 = (startZ - (z - stepZ)) / totalLengthZ;
            glTexCoord2f(u * 4.0f, v2 * 60.0f); glVertex3f(x, finalYNext, z - stepZ);
        }
        glEnd();
    }
    glDisable(GL_TEXTURE_2D);
}

void drawIndonesianRuko(float x, float z) {
    if (texBricks != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texBricks);
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    }

    glPushMatrix();
    glTranslatef(x, -0.6f, z);
    
    glColor3f(1.0f, 1.0f, 1.0f); 
    glPushMatrix();
    glTranslatef(0.0f, 3.0f, 0.0f);
    glScalef(8.0f, 6.0f, 10.0f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_2D);

    // Rolling Door
    glColor3f(0.3f, 0.35f, 0.4f);
    glPushMatrix();
    glTranslatef(0.0f, 1.5f, 5.01f); 
    glScalef(5.0f, 3.0f, 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    glPopMatrix();
}

void drawTiangListrikKabel(float x, float z) {
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.15f, 0.15f, 0.15f); 

    glPushMatrix();
    glTranslatef(x, -0.6f, z);
    glPushMatrix(); glTranslatef(0.0f, 4.5f, 0.0f); glScalef(0.3f, 9.0f, 0.3f); glutSolidCube(1.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(0.0f, 8.5f, 0.0f); glScalef(2.5f, 0.2f, 0.4f); glutSolidCube(1.0f); glPopMatrix();
    glPopMatrix();

    // Katener Kabel Gantung
    glDisable(GL_LIGHTING);
    glColor3f(0.05f, 0.05f, 0.05f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 10; i++) {
        float t = (float)i / 10.0f;
        float interpZ = z - (t * 25.0f); 
        float sagY = 7.9f + (t * t - t) * 4.0f; // Disesuaikan tinggi palang (8.5 - offset)
        glVertex3f(x, sagY, interpZ);
    }
    glEnd();
    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
}

void drawTrainDepot(float centerX, float centerY, float centerZ) {
    glPushMatrix();
    glTranslatef(centerX, centerY, centerZ); 

    // A. PILAR HANGGAR
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.5f, 0.52f, 0.55f); 
    
    for (float localZ = 25.0f; localZ >= -25.0f; localZ -= 12.5f) {
        glPushMatrix(); glTranslatef(-12.0f, 6.0f, localZ); glScalef(1.5f, 12.0f, 1.5f); glutSolidCube(1.0f); glPopMatrix();
        glPushMatrix(); glTranslatef( 12.0f, 6.0f, localZ); glScalef(1.5f, 12.0f, 1.5f); glutSolidCube(1.0f); glPopMatrix();
        glPushMatrix(); glTranslatef(0.0f, 12.0f, localZ); glScalef(24.0f, 0.8f, 1.2f); glutSolidCube(1.0f); glPopMatrix();
    }

    // B. MEGA ATAP DEPO
    if (texRoof != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texRoof); 
        glColor3f(1.0f, 1.0f, 1.0f);
    } else {
        glColor3f(0.5f, 0.15f, 0.15f);
    }

    glBegin(GL_QUADS);
        // Kiri
        glNormal3f(-0.5f, 0.866f, 0.0f);
        glTexCoord2f(0.0f, 0.0f);   glVertex3f(-13.0f, 12.0f,  27.0f);
        glTexCoord2f(10.0f, 0.0f);  glVertex3f(0.0f,   17.0f,  27.0f);
        glTexCoord2f(10.0f, 10.0f); glVertex3f(0.0f,   17.0f, -27.0f);
        glTexCoord2f(0.0f, 10.0f);  glVertex3f(-13.0f, 12.0f, -27.0f);
        // Kanan
        glNormal3f(0.5f, 0.866f, 0.0f);
        glTexCoord2f(0.0f, 0.0f);   glVertex3f(0.0f,   17.0f,  27.0f);
        glTexCoord2f(10.0f, 0.0f);  glVertex3f(13.0f,  12.0f,  27.0f);
        glTexCoord2f(10.0f, 10.0f); glVertex3f(13.0f,  12.0f, -27.0f);
        glTexCoord2f(0.0f, 10.0f);  glVertex3f(0.0f,   17.0f, -27.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    // C. INTERIOR
    glColor3f(0.2f, 0.4f, 0.6f); 
    glPushMatrix(); glTranslatef(-8.5f, 1.0f,  10.0f); glScalef(2.0f, 2.0f, 3.0f); glutSolidCube(1.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(-8.5f, 0.75f, -5.0f); glScalef(1.8f, 1.5f, 2.0f); glutSolidCube(1.0f); glPopMatrix();
    
    glColor3f(0.85f, 0.7f, 0.1f);
    glPushMatrix(); glTranslatef(8.5f, 1.5f, 0.0f); glScalef(1.2f, 3.0f, 1.2f); glutSolidSphere(0.8f, 16, 16); glPopMatrix();

    // Lampu Gantung Industri
    for (float lightZ = 15.0f; lightZ >= -15.0f; lightZ -= 15.0f) {
        glColor3f(0.1f, 0.1f, 0.1f);
        glPushMatrix(); glTranslatef(0.0f, 13.5f, lightZ); glScalef(0.05f, 3.0f, 0.05f); glutSolidCube(1.0f); glPopMatrix();
        glColor3f(0.3f, 0.3f, 0.3f);
        glPushMatrix(); glTranslatef(0.0f, 12.0f, lightZ); glRotatef(90.0f, 1.0f, 0.0f, 0.0f); glutSolidCone(1.0f, 0.6f, 12, 1); glPopMatrix();
        
        glDisable(GL_LIGHTING); 
        glColor3f(1.0f, 0.95f, 0.7f);
        glPushMatrix(); glTranslatef(0.0f, 11.35f, lightZ); glutSolidSphere(0.25f, 8, 8); glPopMatrix();
        glEnable(GL_LIGHTING);
    }
    glPopMatrix();
}

void drawTurnZoneIndicators() {
    float globalTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    float pulsatingAlpha = 0.525f + 0.275f * sinf(globalTime * 4.0f);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_LIGHTING);      
    glDisable(GL_TEXTURE_2D);     
    glDepthMask(GL_FALSE);        

    float centerX = 0.0f;         
    float centerY = -0.70f;       

    for (float currentZ = -1.0f; currentZ >= -1000.0f; currentZ -= 100.0f) {
        if (fabsf(currentZ - posZ) > 150.0f) continue;

        // Piringan Dalam (Cyan)
        glColor4f(0.0f, 0.8f, 1.0f, pulsatingAlpha * 0.3f); 
        glBegin(GL_TRIANGLE_FAN);
            glVertex3f(centerX, centerY, currentZ); 
            float radiusDalam = 4.2f;              
            for (int i = 0; i <= 360; i += 15) {
                float rad = i * 3.14159265f / 180.0f;
                glVertex3f(centerX + cosf(rad) * radiusDalam, centerY, currentZ + sinf(rad) * radiusDalam);
            }
        glEnd();

        // Cincin Luar (Hijau Neon)
        glLineWidth(4.0f); 
        glColor4f(0.0f, 1.0f, 0.5f, pulsatingAlpha); 
        glBegin(GL_LINE_LOOP);
            float radiusLuar = 4.3f; 
            for (int i = 0; i < 360; i += 5) { 
                float rad = i * 3.14159265f / 180.0f;
                glVertex3f(centerX + cosf(rad) * radiusLuar, centerY, currentZ + sinf(rad) * radiusLuar);
            }
        glEnd();
    }

    glLineWidth(1.0f);            
    glDepthMask(GL_TRUE);         
    glEnable(GL_LIGHTING);        
    glDisable(GL_BLEND);          
}


// texturing langit
float skyRotation = 0.0f;
void drawSkybox(float trainX, float trainY, float trainZ) {
	
	static float skyRotation = 0.0f;
    skyRotation += 0.1f;
    if (skyRotation > 360.0f) skyRotation -= 360.0f;
	
    if (texSky == 0) return; 

    glDisable(GL_LIGHTING);       
    glEnable(GL_TEXTURE_2D);      
    glBindTexture(GL_TEXTURE_2D, texSky);
    glDepthMask(GL_FALSE); 
    
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    
	glTranslatef(trainX, trainY, trainZ);
    glRotatef(skyRotation, 0.0f, 1.0f, 0.0f);

    float size = 400.0f; // Ukuran kubus langit (mencakup batas rendering terjauh)

    glBegin(GL_QUADS);
        // Sisi Belakang (Back)
        glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(0.25f, 0.333f); glVertex3f(-size, -size, -size);
        glTexCoord2f(0.50f, 0.333f); glVertex3f( size, -size, -size);
        glTexCoord2f(0.50f, 0.666f); glVertex3f( size,  size, -size);
        glTexCoord2f(0.25f, 0.666f); glVertex3f(-size,  size, -size);

        // Sisi Depan (Front)
        glNormal3f(0.0f, 0.0f, -1.0f);
        glTexCoord2f(0.75f, 0.333f); glVertex3f( size, -size,  size);
        glTexCoord2f(1.00f, 0.333f); glVertex3f(-size, -size,  size);
        glTexCoord2f(1.00f, 0.666f); glVertex3f(-size,  size,  size);
        glTexCoord2f(0.75f, 0.666f); glVertex3f( size,  size,  size);

        // Sisi Kiri (Left)
        glNormal3f(1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.00f, 0.0333f); glVertex3f(-size, -size,  size);
        glTexCoord2f(0.25f, 0.333f);  glVertex3f(-size, -size, -size);
        glTexCoord2f(0.25f, 0.666f);  glVertex3f(-size,  size, -size);
        glTexCoord2f(0.00f, 0.666f);  glVertex3f(-size,  size,  size);

        // Sisi Kanan (Right)
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.50f, 0.333f); glVertex3f( size, -size, -size);
        glTexCoord2f(0.75f, 0.333f); glVertex3f( size, -size,  size);
        glTexCoord2f(0.75f, 0.666f); glVertex3f( size,  size,  size);
        glTexCoord2f(0.50f, 0.666f); glVertex3f( size,  size, -size);

        // Sisi Atas (Top)
        glNormal3f(0.0f, -1.0f, 0.0f);
        glTexCoord2f(0.25f, 0.666f); glVertex3f(-size,  size, -size);
        glTexCoord2f(0.50f, 0.666f); glVertex3f( size,  size, -size);
        glTexCoord2f(0.50f, 1.000f); glVertex3f( size,  size,  size);
        glTexCoord2f(0.25f, 1.000f); glVertex3f(-size,  size,  size);

        // Sisi Bawah (Bottom)
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.25f, 0.000f); glVertex3f(-size, -size,  size);
        glTexCoord2f(0.50f, 0.000f); glVertex3f( size, -size,  size);
        glTexCoord2f(0.50f, 0.333f); glVertex3f( size, -size, -size);
        glTexCoord2f(0.25f, 0.333f); glVertex3f(-size, -size, -size);
    glEnd();

    glPopMatrix();
    glDepthMask(GL_TRUE); // Aktifkan kembali penulisan depth buffer
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
}
