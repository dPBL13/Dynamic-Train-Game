#include "world3.h"
#include <cmath>

// Pengaturan batas maksimum array memori
const int MAX_DECORATIONS = 200;
StaticDecoration decorList[MAX_DECORATIONS];
int totalDecorations = 0;

// Variabel posisi Z kereta eksternal untuk Culling Jarak 
GLuint hillTextureID = 0;
GLuint roofTextureID = 0; // Inisialisasi awal ID Tekstur bricks.bmp

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

    // JEDA LAHAN KOSONG / PERSIMPANGAN (Z: -220 s/d -300) -> Dikosongkan secara sengaja

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
        decorList[totalDecorations++] = { 5, -13.0f, -0.73f, zSisa,          0.0f, 1.0f, 1.0f, 1.0f };
        decorList[totalDecorations++] = { 5,  13.0f, -0.73f, zSisa + 15.0f,  0.0f, 1.0f, 1.0f, 1.0f };
    }
}

// ===================================================================
// 2. RENDER PIPELINE: DRAW ENVIRONMENT WITH DISTANCE CULLING
// ===================================================================
void drawStaticEnvironment() {
	// ---------------------------------------------------------------
    // A. INFRASTRUKTUR JALAN RAYA ASFIKSIA & PEMBATAS HORIZONTAL BLOCK
    // ---------------------------------------------------------------
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

    // 2. Jalan Penghubung Horizontal Antar-Blok (Memotong Tegak Lurus X, Putus di dekat Rel)
    float crossRoadZ[] = { -240.0f, -620.0f, -980.0f };
    for (int j = 0; j < 3; j++) {
        if (fabsf(crossRoadZ[j] - posZ) > 250.0f) continue;
        glBegin(GL_QUADS);
            // Sayap Kiri: dari -90.0f sampai jalan utama -8.75f
            glVertex3f(-90.0f, -0.72f, crossRoadZ[j] + 2.0f);
            glVertex3f(-8.75f, -0.72f, crossRoadZ[j] + 2.0f);
            glVertex3f(-8.75f, -0.72f, crossRoadZ[j] - 2.0f);
            glVertex3f(-90.0f, -0.72f, crossRoadZ[j] - 2.0f);

            // Sayap Kanan: dari 8.75f sampai 90.0f
            glVertex3f(8.75f,  -0.72f, crossRoadZ[j] + 2.0f);
            glVertex3f(90.0f,  -0.72f, crossRoadZ[j] + 2.0f);
            glVertex3f(90.0f,  -0.72f, crossRoadZ[j] - 2.0f);
            glVertex3f(8.75f,  -0.72f, crossRoadZ[j] - 2.0f);
        glEnd();
    }
    glEnable(GL_LIGHTING);
    glPopMatrix();

    // ---------------------------------------------------------------
    // B. ITERASI DRAWING ASET STATIS BERBASIS ARRAY (CULLING RADIUS 250)
    // ---------------------------------------------------------------
    StaticDecoration* lastPoleKiri = nullptr;
    StaticDecoration* lastPoleKanan = nullptr;

    for (int i = 0; i < totalDecorations; i++) {
        StaticDecoration& decor = decorList[i];

        // OPTIMASI DISTANCE CULLING LAYER
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

        // Koleksi pointer tiang untuk kalkulasi bentangan kabel listrik dinamis
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
	
    // State Culling Jarak: Jangan render jika kereta sudah terlalu jauh di depan
    if (posZ < -350.0f) return; 

    // Loop pembuatan kluster perumahan & infrastruktur padat di zona penyangga
    // Steril dari jalur tengah (X = -10.0f sampai X = 10.0f)
    
    // Sisi Kiri Jalan Rel (X negatif mendekati bukit)
    for (float zPos = 120.0f; zPos >= -120.0f; zPos -= 25.0f) {
        // Kluster Ruko & Rumah Deret 1 (X = -25.0f)
        drawIndonesianRuko(-25.0f, zPos);
        drawTiangListrikKabel(-15.0f, zPos);

        // Perumahan Lapis Kedua Mendekati Dinding Bukit (X = -55.0f)
        drawIndonesianRuko(-55.0f, zPos + 5.0f);
        drawIndonesianRuko(-80.0f, zPos - 5.0f);
    }

    // Sisi Kanan Jalan Rel (X positif mendekati bukit)
    for (float zPos = 120.0f; zPos >= -120.0f; zPos -= 25.0f) {
        // Kluster Kompleks Kanan Lapis 1 (X = 25.0f)
        drawIndonesianRuko(25.0f, zPos);
        drawTiangListrikKabel(15.0f, zPos);

        // Perumahan Lapis Kedua Mendekati Dinding Bukit Kanan (X = 60.0f)
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
    int segments = 6; // Dibagi 6 segmen garis kecil agar melengkung mulus
    for (int i = 0; i <= segments; i++) {
        float t = (float)i / (float)segments;
        
        // Interpolasi Linier posisi X dan Z
        float currX = x1 + t * (x2 - x1);
        float currZ = z1 + t * (z2 - z1);
        
        // Rumus Parabola Gravitasi Kuadrat: y = h - sag * (4 * t * (1 - t))
        float sag = 0.35f; // Nilai lendutan kabel ke bawah (35 cm)
        float currY = height - (sag * 4.0f * t * (1.0f - t)) - 0.73f;

        glVertex3f(currX, currY, currZ);
    }
    glEnd();
    
    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

// ===================================================================
// 4. GEOMETRI VARIASI MODEL RUMAH & MAPPING TEKSTUR "BRICKS.BMP"
// ===================================================================

// --- TIPE 1: RUMAH SUBSIDI 1 LANTAI (Atap Prisma Segitiga) ---
void drawHouseType1() {
    // 1. Dinding (Murni Warna, Tanpa Tekstur)
    glDisable(GL_TEXTURE_2D); 
    glColor3f(0.2f, 0.6f, 0.5f);
    glPushMatrix();
    glTranslatef(0.0f, 0.75f, 0.0f);
    glScalef(2.4f, 1.5f, 2.4f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // 2. Atap (Aktifkan Tekstur Bata)
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texRoof);
    glColor3f(1.0f, 1.0f, 1.0f); // Reset warna ke putih agar warna asli tekstur keluar
    
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
    glDisable(GL_TEXTURE_2D); // Matikan kembali agar tidak bocor ke objek lain
}

// --- TIPE 2: RUKO KOMERSIAL 2 LANTAI (Atap Dak Flat Modern) ---
void drawHouseType2() {
    // Lantai 1 (Biru Denim)
    glColor3f(0.18f, 0.35f, 0.52f);
    glPushMatrix();
    glTranslatef(0.0f, 0.8f, 0.0f);
    glScalef(2.6f, 1.6f, 3.0f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Lantai 2 (Putih Minimalis dengan Ornamen Abu)
    glColor3f(0.85f, 0.85f, 0.85f);
    glPushMatrix();
    glTranslatef(0.0f, 2.3f, 0.0f);
    glScalef(2.6f, 1.4f, 3.0f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Atap Dak Beton Bertekstur Bata Atas
    if (roofTextureID != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texRoof);
    }
    glColor3f(0.9f, 0.9f, 0.9f);
    glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.31f, 3.01f, 1.51f);
        glTexCoord2f(2.0f, 0.0f); glVertex3f(1.31f, 3.01f, 1.51f);
        glTexCoord2f(2.0f, 2.0f); glVertex3f(1.31f, 3.01f, -1.51f);
        glTexCoord2f(0.0f, 2.0f); glVertex3f(-1.31f, 3.01f, -1.51f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

// --- TIPE 3: RUMAH ATAP LIMAS ORGANIK (Khas Pedesaan Java) ---
void drawHouseType3() {
    // Dinding Kuning Krem Tradisional
    glColor3f(0.82f, 0.71f, 0.55f);
    glPushMatrix();
    glTranslatef(0.0f, 0.7f, 0.0f);
    glScalef(2.5f, 1.4f, 2.5f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Atap Model Limas (Piramida Terpotong Atas / Kerucut Segi Empat)
    if (roofTextureID != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texRoof);
    }
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glTranslatef(0.0f, 1.4f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    glRotatef(45, 0, 0, 1);
    
    // Menggunakan Cone 4 sisi sebagai aproksimasi limas miring proporsional
    glutSolidCone(2.0f, 1.4f, 4, 1);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

// --- TIPE 4: GEDUNG FASILITAS UMUM / BALAI DESA ---
void drawHouseType4() {
    // Struktur Utama Besar Elegan (Warna Krem Pucat)
    glColor3f(0.9f, 0.88f, 0.82f);
    glPushMatrix();
    glTranslatef(0.0f, 1.1f, 0.0f);
    glScalef(5.0f, 2.2f, 4.0f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Atap Pelana Besar Bertekstur Kuat
    if (roofTextureID != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texRoof);
    }
    glColor3f(0.8f, 0.8f, 0.8f);
    glBegin(GL_TRIANGLES);
        glNormal3f(-0.707f, 0.707f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-2.6f, 2.2f, 2.1f);
        glTexCoord2f(3.0f, 0.0f); glVertex3f(-2.6f, 2.2f, -2.1f);
        glTexCoord2f(1.5f, 2.0f); glVertex3f(0.0f, 3.4f, 0.0f);

        glNormal3f(0.707f, 0.707f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(2.6f, 2.2f, 2.1f);
        glTexCoord2f(3.0f, 0.0f); glVertex3f(2.6f, 2.2f, -2.1f);
        glTexCoord2f(1.5f, 2.0f); glVertex3f(0.0f, 3.4f, 0.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

// --- TIPE 5: TIANG LISTRIK INFRASTRUKTUR & PENDARAN EMISSION ---
void drawUtilityPole() {
    // 1. Tiang Kayu Silindris Vertikal Utama
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texWood);
    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.06f, 0.0f,  0.06f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.06f, 0.0f,  0.06f);
        glTexCoord2f(1.0f, 5.0f); glVertex3f( 0.06f, 4.5f,  0.06f); // V=5.0f agar urat kayu memanjang ke atas
        glTexCoord2f(0.0f, 5.0f); glVertex3f(-0.06f, 4.5f,  0.06f);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    // 2. Palang Besi T Atas Horizontal
    glColor3f(0.25f, 0.25f, 0.25f);
    glPushMatrix();
    glTranslatef(0.0f, 4.2f, 0.0f);
    glScalef(1.2f, 0.1f, 0.12f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // 3. Kap Rumah Lampu Jalan (Neon Pendaran)
    GLfloat mat_bulb[] = { 1.0f, 1.0f, 0.5f, 1.0f };
    GLfloat mat_emission[] = { 0.6f, 0.6f, 0.2f, 1.0f }; // Efek pendaran neon menyala aktif
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_bulb);
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);

    glPushMatrix();
    glTranslatef(0.5f, 4.0f, 0.0f); // Menjorok ke sisi jalan raya
    glutSolidSphere(0.14f, 6, 6);
    glPopMatrix();

    // Reset state emission agar objek lain tidak ikut menyala mandiri
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

    // LAYER 1: BUKIT UTAMA INTERNAL (Sisi Kiri X: -100 ke -70, Sisi Kanan X: 70 ke 100)
    for (float z = startZ; z >= endZ + stepZ; z -= stepZ) {
        if (fabsf(z - posZ) > 300.0f) continue; // Distance-Based Culling aktif

        // Sayap Kiri Layer 1
        glBegin(GL_TRIANGLE_STRIP);
        for (float x = -100.0f; x <= -70.0f; x += stepX) {
            float slope = fabsf(x - (-70.0f)) / 30.0f; 
            float waveY = sinf(z * 0.05f) * cosf(x * 0.1f) * 22.0f + sinf(z * 0.01f) * 8.0f; // Tinggi ditingkatkan
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

        // ===================================================================
        // LAYER 2: PEGUNUNGAN RAKSASA BELAKANG (EKSTRA PENEBALAN ANTI-OOB)
        // Offset koordinat dilempar keluar (X: -140 ke -100 dan X: 100 ke 140)
        // Skala Amplitudo Tinggi Di-boost Keras mencapai Y = 65.0f
        // ===================================================================
        
        // Sayap Kiri Mega Bukit Layer 2
        glBegin(GL_TRIANGLE_STRIP);
        for (float x = -140.0f; x <= -100.0f; x += stepX) {
            float slope = fabsf(x - (-100.0f)) / 40.0f; 
            float waveY = sinf(z * 0.03f) * 45.0f + cosf(x * 0.05f) * 20.0f; // Total Y_MAX raksasa memblokir langit luar
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

        // Sayap Kanan Mega Bukit Layer 2
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
	
	// 1. Aktifkan Tekstur 2D
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texBricks);

    // 2. Aktifkan Otomatisasi Koordinat Tekstur (S dan T)
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);

    // 3. Atur mode proyeksi tekstur (GL_OBJECT_LINEAR biasanya paling stabil untuk kubus diam)
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    
    
    glPushMatrix();
    glTranslatef(x, -0.6f, z);
    
    // Badan Gedung Utama Ruko
    glColor3f(1.0f, 1.0f, 1.0f); // Warna semen dasar dinding
    glPushMatrix();
    glTranslatef(0.0f, 3.0f, 0.0f);
    glScalef(8.0f, 6.0f, 10.0f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_2D);

    // Ornamen Rolling Door Toko Lantai 1
    glColor3f(0.3f, 0.35f, 0.4f);
    glPushMatrix();
    glTranslatef(0.0f, 1.5f, 5.01f); // Menempel di dinding depan ruko
    glScalef(5.0f, 3.0f, 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    glPopMatrix();
}

// Pembuatan Tiang Listrik Prosedural beserta Efek Jaringan Kabel Gantung
void drawTiangListrikKabel(float x, float z) {
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.15f, 0.15f, 0.15f); // Warna tiang besi beton hitam pekat

    // Batang Utama Tiang Listrik
    glPushMatrix();
    glTranslatef(x, -0.6f, z);
    glPushMatrix(); glTranslatef(0.0f, 4.5f, 0.0f); glScalef(0.3f, 9.0f, 0.3f); glutSolidCube(1.0f); glPopMatrix();
    // Palang Atas Isolator Kabel
    glPushMatrix(); glTranslatef(0.0f, 8.5f, 0.0f); glScalef(2.5f, 0.2f, 0.4f); glutSolidCube(1.0f); glPopMatrix();
    glPopMatrix();

    // Rendering Geometri Jalur Kabel Gantung Menggunakan GL_LINE_STRIP (Konsep Katener Sederhana)
    glColor3f(0.05f, 0.05f, 0.05f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 10; i++) {
        float t = (float)i / 10.0f;
        float interpZ = z - (t * 25.0f); // Menghubungkan tiang saat ini ke tiang berikutnya di Z-25
        // Rumus kelengkungan kabel gantung akibat gravitasi (Fungsi kuadratis parabola)
        float sagY = 7.3f + (t * t - t) * 4.0f; 
        glVertex3f(x, sagY, interpZ);
    }
    glEnd();
    glLineWidth(1.0f);
}

void drawTrainDepot(float centerX, float centerY, float centerZ) {
    // Lakukan Culling Jarak: Hanya diproses jika kereta sudah mendekati area finish
    // (Optional: bisa dihapus atau disesuaikan dengan kebutuhan)
    // if (posZ > centerZ + 300.0f) return;

    glPushMatrix();
    glTranslatef(centerX, centerY, centerZ); // Jangkar bisa ditempatkan di mana saja

    // -----------------------------------------------------------------
    // A. STRUKTUR PILAR-PILAR BETON PENYAGGA UTAMA HANGGAR (Kiri & Kanan)
    // -----------------------------------------------------------------
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.5f, 0.52f, 0.55f); // Beton Industri Abu Maskulin Kelompok 8
    
    // Total panjang bangunan hanggar depo disimulasikan sejauh 50 unit ke belakang
    for (float localZ = 25.0f; localZ >= -25.0f; localZ -= 12.5f) {
        // Pilar Sisi Kiri (Steril dari rel tengah X = 0)
        glPushMatrix(); glTranslatef(-12.0f, 6.0f, localZ); glScalef(1.5f, 12.0f, 1.5f); glutSolidCube(1.0f); glPopMatrix();
        // Pilar Sisi Kanan
        glPushMatrix(); glTranslatef( 12.0f, 6.0f, localZ); glScalef(1.5f, 12.0f, 1.5f); glutSolidCube(1.0f); glPopMatrix();
        
        // Pilar Penyangga Balok Atas Horizontal (Cross-Beam)
        glPushMatrix(); glTranslatef(0.0f, 12.0f, localZ); glScalef(24.0f, 0.8f, 1.2f); glutSolidCube(1.0f); glPopMatrix();
    }

    // -----------------------------------------------------------------
    // B. STRUKTUR ATAP SEGITIGA MEGAH (Bermaterial Bata/Seng Bertekstur)
    // -----------------------------------------------------------------
    if (texRoof != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texRoof); // Mengikat bricks.bmp
        glColor3f(1.0f, 1.0f, 1.0f);
    } else {
        glColor3f(0.5f, 0.15f, 0.15f);
    }

    // Penggambaran Atap Menggunakan Teknik GL_QUADS Manual Agar Tekstur Terpetakan Rapi
    glBegin(GL_QUADS);
        // Sisi Atap Kemiringan Kiri
        glNormal3f(-0.5f, 0.866f, 0.0f);
        glTexCoord2f(0.0f, 0.0f);   glVertex3f(-13.0f, 12.0f,  27.0f);
        glTexCoord2f(10.0f, 0.0f);  glVertex3f(0.0f,   17.0f,  27.0f);
        glTexCoord2f(10.0f, 10.0f); glVertex3f(0.0f,   17.0f, -27.0f);
        glTexCoord2f(0.0f, 10.0f);  glVertex3f(-13.0f, 12.0f, -27.0f);

        // Sisi Atap Kemiringan Kanan
        glNormal3f(0.5f, 0.866f, 0.0f);
        glTexCoord2f(0.0f, 0.0f);   glVertex3f(0.0f,   17.0f,  27.0f);
        glTexCoord2f(10.0f, 0.0f);  glVertex3f(13.0f,  12.0f,  27.0f);
        glTexCoord2f(10.0f, 10.0f); glVertex3f(13.0f,  12.0f, -27.0f);
        glTexCoord2f(0.0f, 10.0f);  glVertex3f(0.0f,   17.0f, -27.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    // -----------------------------------------------------------------
    // C. DETAIL INTERIOR DEPO (Perkakas Bengkel & Lampu Gantung Industri)
    // -----------------------------------------------------------------
    // Perkakas Bengkel Statis Kotak Besi Hidrolik di Samping Kiri Jalur Dalam Hanggar
    glColor3f(0.2f, 0.4f, 0.6f); // Warna biru mesin manufaktur
    glPushMatrix(); glTranslatef(-8.5f, 1.0f,  10.0f); glScalef(2.0f, 2.0f, 3.0f); glutSolidCube(1.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(-8.5f, 0.75f, -5.0f); glScalef(1.8f, 1.5f, 2.0f); glutSolidCube(1.0f); glPopMatrix();
    
    // Tabung Kompresor Gas Kuning Kanan Dalam Hanggar
    glColor3f(0.85f, 0.7f, 0.1f);
    glPushMatrix(); glTranslatef(8.5f, 1.5f, 0.0f); glScalef(1.2f, 3.0f, 1.2f); glutSolidSphere(0.8f, 16, 16); glPopMatrix();

    // Lampu Gantung Industri Kap Mesin (Sisi Tengah Atas Langit-langit)
    glColor3f(0.1f, 0.1f, 0.1f);
    for (float lightZ = 15.0f; lightZ >= -15.0f; lightZ -= 15.0f) {
        // Kabel Gantung Lampu
        glPushMatrix(); glTranslatef(0.0f, 13.5f, lightZ); glScalef(0.05f, 3.0f, 0.05f); glutSolidCube(1.0f); glPopMatrix();
        // Rumah Kap Lampu Mangkuk Gantung
        glColor3f(0.3f, 0.3f, 0.3f);
        glPushMatrix(); 
        glTranslatef(0.0f, 12.0f, lightZ); 
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f); 
        glutSolidCone(1.0f, 0.6f, 12, 1); 
        glPopMatrix();
        
        // Pijar Cahaya Kuning Lampu (Emissive Visual Effect Sederhana)
        glDisable(GL_LIGHTING); // Matikan lighting agar kubus kecil ini terlihat menyala terang murni
        glColor3f(1.0f, 0.95f, 0.7f);
        glPushMatrix(); glTranslatef(0.0f, 11.35f, lightZ); glutSolidSphere(0.25f, 8, 8); glPopMatrix();
        glEnable(GL_LIGHTING);
    }

    glPopMatrix();
}

void drawTurnZoneIndicators() {
    // 1. ANIMASI PULSATING (BERDENYUT)
    float globalTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    float pulsatingAlpha = 0.525f + 0.275f * sinf(globalTime * 4.0f);
    
    // 2. ATURAN STATE MACHINE & OPTIMASI GRAPHICS CARD
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_LIGHTING);      
    glDisable(GL_TEXTURE_2D);     
    glDepthMask(GL_FALSE);        // Mencegah Z-fighting / kedip-kedip dengan rel

    float centerX = 0.0f;         
    float centerY = -0.70f;       // Tipis di atas rel (Y = -0.73f)

    // ===================================================================
    // KOREKSI UTAMA: LOOPING PROSEDURAL DINAMIS SEPANJANG REL (-1100.0f)
    // Berlaku untuk Turn 1, Turn 2, Turn 3, dst. Setiap kelipatan 100 unit Z
    // ===================================================================
    for (float currentZ = -1.0f; currentZ >= -1000.0f; currentZ -= 100.0f) {
        
        // OPTIMASI: Distance-Based Culling untuk Indikator
        // Jika lingkaran berada terlalu jauh di depan atau di belakang kereta, jangan di-render (hemat FPS)
        extern float posZ; // Mengambil variabel posisi Z kereta dari main.cpp
        if (fabsf(currentZ - posZ) > 150.0f) continue;

        // --- GEOMETRI 1: PIRINGAN SEMI-TRANSPARAN DALAM (GL_TRIANGLE_FAN) ---
        glColor4f(0.0f, 0.8f, 1.0f, pulsatingAlpha * 0.3f); // Cyan halus
        glBegin(GL_TRIANGLE_FAN);
            glVertex3f(centerX, centerY, currentZ); 
            float radiusDalam = 4.2f;              
            for (int i = 0; i <= 360; i += 15) {
                float rad = i * 3.14159265f / 180.0f;
                glVertex3f(centerX + cosf(rad) * radiusDalam, centerY, currentZ + sinf(rad) * radiusDalam);
            }
        glEnd();

        // --- GEOMETRI 2: CINCIN PEMBATAS LUAR (GL_LINE_LOOP) ---
        glLineWidth(4.0f); 
        glColor4f(0.0f, 1.0f, 0.5f, pulsatingAlpha); // Hijau Neon
        glBegin(GL_LINE_LOOP);
            float radiusLuar = 4.3f; 
            for (int i = 0; i < 360; i += 5) { 
                float rad = i * 3.14159265f / 180.0f;
                glVertex3f(centerX + cosf(rad) * radiusLuar, centerY, currentZ + sinf(rad) * radiusLuar);
            }
        glEnd();
    }

    // 4. RESET STATE MACHINE OPENGL
    glLineWidth(1.0f);            
    glDepthMask(GL_TRUE);         
    glEnable(GL_LIGHTING);        
    glDisable(GL_BLEND);          
}

/**
 * Fungsi: drawSkybox (Updated dengan Animasi Rotasi)
 * Deskripsi: Merender kubus langit 3D yang berpusat dinamis pada posisi kereta
 * dan berputar perlahan ke samping untuk efek pergerakan awan/langit dinamis.
 */
void drawSkybox(float trainX, float trainY, float trainZ) {
    if (texSky == 0) return; // Proteksi jika file langit.bmp gagal dimuat

    // 1. STATE MACHINE OPENGL & OPTIMASI
    glDisable(GL_LIGHTING);       
    glEnable(GL_TEXTURE_2D);      
    glBindTexture(GL_TEXTURE_2D, texSky);
    glDepthMask(GL_FALSE); // Kunci posisi paling belakang

    glPushMatrix();
    // Langkah A: Ikuti koordinat posisi kereta/kamera
    glTranslatef(trainX, trainY, trainZ);

    // ===================================================================
    // LANGKAH B: IMPLEMENTASI ROTASI LANGIT PERLAHAN (KE SAMPING / Y-AXIS)
    // ===================================================================
    // Mengambil waktu berjalan (milidetik) lalu dikalikan angka kecil (0.02f) 
    // agar kecepatan putarnya konstan, halus, dan lambat (anggun).
    float skyAngle = (glutGet(GLUT_ELAPSED_TIME) / 1000.0f) * 10.5f; // Berputar 0.5 derajat per detik
    glRotatef(skyAngle, 0.0f, 1.0f, 1.0f); // Rotasi mengelilingi sumbu vertikal Y

    float radius = 350.0f; // Radius bola langit
    int slices = 36; // Jumlah slice (keliling)
    int stacks = 18; // Jumlah stack (tinggi)

    // Render bola (interior)
    for (int i = 0; i <= stacks; i++) {
        float theta1 = (float)i / stacks * PI; // Sudut dari kutub utara ke selatan (0 to PI)
        float theta2 = (float)(i + 1) / stacks * PI;
        
        for (int j = 0; j <= slices; j++) {
            float phi1 = (float)j / slices * 2.0f * PI; // Sudut keliling (0 to 2PI)
            float phi2 = (float)(j + 1) / slices * 2.0f * PI;
            
            // Hitung koordinat untuk 4 titik quad
            float x1 = radius * sin(theta1) * cos(phi1);
            float y1 = radius * cos(theta1);
            float z1 = radius * sin(theta1) * sin(phi1);
            
            float x2 = radius * sin(theta2) * cos(phi1);
            float y2 = radius * cos(theta2);
            float z2 = radius * sin(theta2) * sin(phi1);
            
            float x3 = radius * sin(theta2) * cos(phi2);
            float y3 = radius * cos(theta2);
            float z3 = radius * sin(theta2) * sin(phi2);
            
            float x4 = radius * sin(theta1) * cos(phi2);
            float y4 = radius * cos(theta1);
            float z4 = radius * sin(theta1) * sin(phi2);
            
            // Hitung texture coordinates (pemetaan bola ke texture 2D)
            float u1 = (float)j / slices;
            float v1 = 1.0f - (float)i / stacks;
            float u2 = (float)(j + 1) / slices;
            float v2 = 1.0f - (float)(i + 1) / stacks;
            
            glBegin(GL_QUADS);
                // Normal untuk lighting (optional untuk skybox)
                glNormal3f(x1 / radius, y1 / radius, z1 / radius);
                glTexCoord2f(u1, v1);
                glVertex3f(x1, y1, z1);
                
                glNormal3f(x2 / radius, y2 / radius, z2 / radius);
                glTexCoord2f(u1, v2);
                glVertex3f(x2, y2, z2);
                
                glNormal3f(x3 / radius, y3 / radius, z3 / radius);
                glTexCoord2f(u2, v2);
                glVertex3f(x3, y3, z3);
                
                glNormal3f(x4 / radius, y4 / radius, z4 / radius);
                glTexCoord2f(u2, v1);
                glVertex3f(x4, y4, z4);
            glEnd();
        }
    }

    glPopMatrix();

    // 3. RESET STATE OPENGL
    glDepthMask(GL_TRUE);         
    glEnable(GL_LIGHTING);        
    glDisable(GL_TEXTURE_2D);
}
