#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdio>
#include "world.h"
#include "world2.h"
#include "globals.h"
#include "lighting.h"


float glowTimer = 0.0f;


// ===================================================================
// 3. DESAIN LINGKUNGAN 3D PROCEDURAL (PASCA TURN 2)
// ===================================================================

// Helper Fungsi: Rumah 3D Tertekstur & Beratap Prisma
void drawTexturedHouse(float x, float z, float w, float h, float d, float roofHeight) {
    glPushMatrix();
    glTranslatef(x, -0.6f, z); // Letakkan tepat di atas permukaan tanah hijau

    // [A] Render Dinding Kubus Tertekstur Bata (bricks.bmp)
    if (texBricks != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texBricks);
    }
    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);
    // Dinding Depan
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-w/2, 0.0f,  d/2);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( w/2, 0.0f,  d/2);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( w/2,  h,    d/2);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-w/2,  h,    d/2);
    // Dinding Belakang
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( w/2, 0.0f, -d/2);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-w/2, 0.0f, -d/2);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-w/2,  h,   -d/2);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( w/2,  h,   -d/2);
    // Dinding Kiri
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-w/2, 0.0f, -d/2);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-w/2, 0.0f,  d/2);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-w/2,  h,    d/2);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-w/2,  h,   -d/2);
    // Dinding Kanan
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( w/2, 0.0f,  d/2);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( w/2, 0.0f, -d/2);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( w/2,  h,   -d/2);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( w/2,  h,    d/2);
    glEnd();

    // [B] Render Atap Prisma Segitiga (Solid Cokelat Tua)
    glColor3f(0.45f, 0.22f, 0.08f); 
    glBegin(GL_TRIANGLES);
    // Sisi Segitiga Depan
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-w/2, h,  d/2);
    glVertex3f( w/2, h,  d/2);
    glVertex3f(0.0f, h + roofHeight, d/2);
    // Sisi Segitiga Belakang
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f( w/2, h, -d/2);
    glVertex3f(-w/2, h, -d/2);
    glVertex3f(0.0f, h + roofHeight, -d/2);
    glEnd();

    glBegin(GL_QUADS);
    // Kemiringan Atap Kanan
    glNormal3f(0.707f, 0.707f, 0.0f);
    glVertex3f( w/2, h,  d/2);
    glVertex3f( w/2, h, -d/2);
    glVertex3f(0.0f, h + roofHeight, -d/2);
    glVertex3f(0.0f, h + roofHeight,  d/2);
    // Kemiringan Atap Kiri
    glNormal3f(-0.707f, 0.707f, 0.0f);
    glVertex3f(-w/2, h, -d/2);
    glVertex3f(-w/2, h,  d/2);
    glVertex3f(0.0f, h + roofHeight,  d/2);
    glVertex3f(0.0f, h + roofHeight, -d/2);
    glEnd();
    
    glDisable(GL_TEXTURE_2D); 
    
    glPopMatrix();
}

// Pagar Pembatas Kereta Otomatis (Prosedural Sumbu Z)
void drawGuardrails() {
    glColor3f(0.75f, 0.75f, 0.75f); // Warna logam perak
    float lebarRelAman = 12.0f;     // Batas samping jalur rel (+-12 dari titik tengah)

    // Perulangan menelusuri koridor rel dari z=-200 sampai ujung lintasan z=-1000
    for (float z = -200.0f; z >= -1000.0f; z -= 8.0f) {
        for (int sisi = -1; sisi <= 1; sisi += 2) {
            float xPos = sisi * lebarRelAman;

            // 1. Tiang Vertikal Kecil
            glPushMatrix();
            glTranslatef(xPos, -0.6f, z);
            glScalef(0.15f, 1.2f, 0.15f);
            glutSolidCube(1.0f);
            glPopMatrix();

            // 2. Bilah Horizontal Atas Penghubung Antar Tiang
            glPushMatrix();
            glTranslatef(xPos, 0.3f, z - 4.0f);
            glScalef(0.08f, 0.12f, 8.0f);
            glutSolidCube(1.0f);
            glPopMatrix();
        }
    }
}

// Jembatan Penyeberangan Orang (Overpass Bridge) Melintasi Rel fisik
void drawOverpassBridge(float zPos) {
    glPushMatrix();
    // Transformasi Jangkar Utama Jembatan (Menggunakan zPos bawaan sistem)
    glTranslatef(0.0f, -0.6f, zPos);

    // ===================================================================
    // 1. PILAR PONDASI UTAMA KIRI & KANAN (Murni Warna Beton Abu)
    // ===================================================================
    glDisable(GL_TEXTURE_2D); 
    glColor3f(0.45f, 0.47f, 0.49f); 
    glPushMatrix(); glTranslatef(-14.0f, 2.25f, 0.0f); glScalef(1.5f, 4.5f, 2.0f); glutSolidCube(1.0f); glPopMatrix();
    glPushMatrix(); glTranslatef( 14.0f, 2.25f, 0.0f); glScalef(1.5f, 4.5f, 2.0f); glutSolidCube(1.0f); glPopMatrix();

    // ===================================================================
    // 2. PLATFORM PENYEBERANGAN HORIZONTAL LANGIT-LANGIT (Bertekstur Beton)
    // ===================================================================
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texConcrete); // Menggunakan variabel ID tekstur Anda
    glColor3f(1.0f, 1.0f, 1.0f); // Reset modulasi ke putih agar tekstur tidak gelap

    glPushMatrix();
    // Diubah ke (0.0f, 4.5f, 0.0f) agar lantai menempel pas di atas pilar & tangga
    glTranslatef(0.0f, 4.5f, 0.0f); 

    // Struktur Lantai Jembatan Bertekstur
    glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f);  glVertex3f(-15.0f, 0.0f,  1.5f);
        glTexCoord2f(10.0f, 0.0f); glVertex3f( 15.0f, 0.0f,  1.5f);
        glTexCoord2f(10.0f, 1.0f); glVertex3f( 15.0f, 0.0f, -1.5f);
        glTexCoord2f(0.0f, 1.0f);  glVertex3f(-15.0f, 0.0f, -1.5f);
    glEnd();

    glPopMatrix();
    glDisable(GL_TEXTURE_2D); // Matikan kembali tekstur agar tidak bocor ke pagar

    // ===================================================================
    // 3. PAGAR PENGAMAN DI JEMBATAN ATAS (Kiri & Kanan Platform)
    // ===================================================================
    glColor3f(0.2f, 0.6f, 0.8f); // Warna Biru Pagar
    glPushMatrix(); glTranslatef(0.0f, 5.0f,  1.45f); glScalef(29.5f, 0.8f, 0.05f); glutSolidCube(1.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(0.0f, 5.0f, -1.45f); glScalef(29.5f, 0.8f, 0.05f); glutSolidCube(1.0f); glPopMatrix();

    // ===================================================================
    // 4. TANGGA TURUN PROSEDURAL SISI KIRI & KANAN (Menggunakan Loop)
    // ===================================================================
    glColor3f(0.38f, 0.40f, 0.42f);
    for (int i = 0; i < 15; i++) {
        float stepOffset = i * 0.3f; // Kalkulasi kemiringan sudut tangga
        
        // Tangga Kiri
        glPushMatrix();
        glTranslatef(-14.0f + stepOffset, 4.35f - stepOffset, 0.0f);
        glScalef(0.6f, 0.3f, 2.0f);
        glutSolidCube(1.0f);
        glPopMatrix();

        // Tangga Kanan
        glPushMatrix();
        glTranslatef(14.0f - stepOffset, 4.35f - stepOffset, 0.0f);
        glScalef(0.6f, 0.3f, 2.0f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }
    
    glPopMatrix(); // Tutup jangkar utama zPos
}

// Master Orchestrator untuk Seluruh Objek Dekorasi Dunia
void drawEnvironmentDecorations() {
    
    // ATURAN PEMBATASAN WILAYAH: Hanya muncul masif setelah Turn 2 (Z <= -200)
    // Skenario 1: Kompleks Perumahan & Stasiun Pedesaan
    drawTexturedHouse(-18.0f, -220.0f, 5.0f, 4.0f, 6.0f, 2.0f);
    drawTexturedHouse(-25.0f, -250.0f, 7.0f, 5.0f, 8.0f, 3.0f);
    drawTexturedHouse( 18.0f, -240.0f, 6.0f, 4.0f, 6.0f, 2.5f);
    drawTexturedHouse( 24.0f, -290.0f, 8.0f, 6.0f, 7.0f, 3.5f);

    // Skenario 2: Kompleks Perkotaan Padat (Gedung Bertingkat / Rumah Besar) setelah Turn 4
    drawTexturedHouse(-20.0f, -420.0f, 8.0f, 12.0f, 8.0f, 4.0f);
    drawTexturedHouse( 22.0f, -460.0f, 9.0f, 15.0f, 9.0f, 5.0f);
    drawTexturedHouse(-22.0f, -500.0f, 7.0f, 9.0f,  7.0f, 3.0f);
    drawTexturedHouse( 19.0f, -580.0f, 8.0f, 11.0f, 8.0f, 4.0f);
    
    // Skenario 3: Pemukiman Akhir Turn 7 s/d Turn 10
    drawTexturedHouse(-18.0f, -750.0f, 6.0f, 4.0f, 6.0f, 2.0f);
    drawTexturedHouse( 19.0f, -820.0f, 7.0f, 5.0f, 7.0f, 2.5f);
    drawTexturedHouse(-26.0f, -900.0f, 8.0f, 7.0f, 8.0f, 3.5f);

    // 4. Panggil Komponen Infrastruktur Pelengkap Rel
    drawGuardrails();
    
    // Tempatkan Jembatan Penyeberangan Megah melintang aman di koordinat Z tertentu
    drawOverpassBridge(-350.0f); // Jembatan 1 sebelum kota tengah
    drawOverpassBridge(-700.0f); // Jembatan 2 di area pedesaan akhir
}
