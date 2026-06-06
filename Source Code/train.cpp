// =====================================================
// train.cpp  –  Kereta Detail (Lokomotif + 2 Gerbong)
//
// Sistem koordinat:
//   • Rel membentang di sumbu Z dunia.
//   • display() memanggil:
//       glTranslatef(0, 0, posZ);
//       glRotatef(90, 0, 1, 0);   ← lokal +X = arah rel (+Z dunia)
//   • Jadi di ruang lokal kereta:
//       +X  = arah maju (sepanjang rel)
//       +Y  = atas
//       +Z  = kanan kereta
//
// Skala: ~1 unit = 0.35m
//   Lokomotif panjang ±4.2u, lebar 1.4u
//   Gerbong panjang ±4.5u
// =====================================================
#include "train.h"
#include "lighting.h"
#include "globals.h"
#include <cmath>

#ifndef PI
#define PI 3.14159265358979323846f
#endif

// Helper material
static void mat(float ar,float ag,float ab,
                float dr,float dg,float db,
                float sh){
    if(!lightingEnabled){ glColor3f(dr,dg,db); return; }
    setMaterial(ar,ag,ab, dr,dg,db, sh);
}

// Kotak (panjang x, tinggi y, lebar z) --> buat bikin keretanya
static void box(float sx, float sy, float sz){
    glPushMatrix();
    glScalef(sx, sy, sz);
    glutSolidCube(1.0f);
    glPopMatrix();
}

// Tabung (+Y), radius r, tinggi h --> cerobong
static void cylY(float r, float h, int slices=12){
    glPushMatrix();
    glRotatef(-90.0f, 1,0,0);
    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluCylinder(q, r, r, h, slices, 2);
    gluDeleteQuadric(q);
    glPopMatrix();
}

// Roda tunggal (internal) --
static void drawWheelAt(float x, float y, float z, float angle){
    glPushMatrix();
    glTranslatef(x, y, z);

 	if(z < 0) {
        // Roda kiri: menghadap ke kanan (ke dalam)
        glScalef(1, 1, 1);
    } else {
        // Roda kanan: flip agar menghadap ke kiri (ke dalam)
        glScalef(1, 1, -1);
    }

    glRotatef(angle, 0, 0, 1);

    const float r = 0.38f;
    const float w = 0.13f;

    mat(0.20f,0.20f,0.22f, 0.45f,0.45f,0.48f, 80.0f);

    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);

    glPushMatrix();
    glRotatef(90.0f, 0,0,1);

    gluCylinder(q, r, r, w, 18, 2);
    gluDisk(q, 0, r, 18, 1);

    glTranslatef(0,0,w);
    gluDisk(q, 0, r, 18, 1);
    glPopMatrix();

    // Hub tengah
    mat(0.50f,0.40f,0.15f, 0.70f,0.55f,0.18f, 90.0f);
    glutSolidSphere(r*0.26f, 8, 8);

    // Spoke
    mat(0.30f,0.30f,0.32f, 0.50f,0.50f,0.53f, 60.0f);
    for(int s=0;s<4;s++){
        glPushMatrix();
        glRotatef(s*90.0f, 1,0,0);
        glTranslatef(0, r*0.5f, 0);
        glScalef(0.04f, r*0.88f, 0.04f);
        glutSolidCube(1);
        glPopMatrix();
    }
    gluDeleteQuadric(q);
    glPopMatrix();
}

//Roda-roda
void drawWheel(float x, float y, float z){
    drawWheelAt(x, y, z, wheelAngle);
}

//Loko
void drawLocomotiveOnly() {
    // Bogie / underframe
    mat(0.18f,0.18f,0.18f, 0.35f,0.35f,0.38f, 30.0f);
    glPushMatrix(); glTranslatef(0, 0.10f, 0);
    box(4.0f, 0.20f, 1.50f);
    glPopMatrix();

    // Badan utama
    mat(0.12f,0.22f,0.58f, 0.28f,0.40f,0.68f, 55.0f);
    glPushMatrix(); glTranslatef(0, 0.9f, 0);
    box(3.8f, 1.4f, 1.4f);
    glPopMatrix();

    // Kabin masinis
    mat(0.10f,0.18f,0.52f, 0.25f,0.35f,0.62f, 50.0f);
    glPushMatrix(); glTranslatef(-1.1f, 1.9f, 0);
    box(1.4f, 0.9f, 1.3f);
    glPopMatrix();

    // Atap kabin
    mat(0.08f,0.08f,0.08f, 0.20f,0.20f,0.20f, 20.0f);
    glPushMatrix(); glTranslatef(-1.1f, 2.45f, 0);
    box(1.5f, 0.13f, 1.42f);
    glPopMatrix();

    // Jendela depan kabin
    mat(0.50f,0.75f,0.92f, 0.82f,0.92f,1.00f, 120.0f);
    glPushMatrix(); glTranslatef(-1.82f, 1.92f, 0);
    box(0.06f, 0.45f, 0.82f);
    glPopMatrix();

    // Jendela samping kabin
    for(int s=-1;s<=1;s+=2){
        glPushMatrix(); glTranslatef(-1.1f, 1.92f, s*0.66f);
        box(1.0f, 0.40f, 0.06f);
        glPopMatrix();
    }

    // Hidung depan
    mat(0.14f,0.24f,0.62f, 0.30f,0.42f,0.68f, 50.0f);
    glPushMatrix(); glTranslatef(1.6f, 0.7f, 0);
    glScalef(1.0f, 0.85f, 1.0f);
    box(0.75f, 1.1f, 1.30f);
    glPopMatrix();

    // Lampu depan
    mat(1.0f, 1.0f, 0.8f, 1.0f, 1.0f, 0.9f, 128.0f);
    if(lightingEnabled){
        GLfloat em[]={0.7f,0.7f,0.3f,1.0f};
        glMaterialfv(GL_FRONT,GL_EMISSION,em);
    }
    glPushMatrix(); glTranslatef(2.05f, 0.82f, 0);
    glutSolidSphere(0.14f, 8, 8);
    glPopMatrix();
    if(lightingEnabled){
        GLfloat emOff[]={0,0,0,1};
        glMaterialfv(GL_FRONT,GL_EMISSION,emOff);
    }

    // Buffer
    mat(0.38f,0.38f,0.40f, 0.65f,0.65f,0.68f, 80.0f);
    for(int bz=-1;bz<=1;bz+=2){
        glPushMatrix(); glTranslatef( 2.1f, 0.22f, bz*0.50f);
        box(0.15f,0.25f,0.15f); glPopMatrix();
        glPushMatrix(); glTranslatef(-2.1f, 0.22f, bz*0.50f);
        box(0.15f,0.25f,0.15f); glPopMatrix();
    }

    // Cerobong asap
    mat(0.12f,0.12f,0.12f, 0.28f,0.28f,0.28f, 25.0f);
    glPushMatrix(); glTranslatef(1.25f, 1.65f, 0);
    cylY(0.14f, 0.55f, 10);
    glTranslatef(0, 0.55f, 0);
    {
        GLUquadric* qc = gluNewQuadric();
        gluQuadricNormals(qc, GLU_SMOOTH);
        glPushMatrix(); glRotatef(-90.0f,1,0,0);
        gluDisk(qc, 0.10f, 0.22f, 12, 1);
        glPopMatrix();
        gluDeleteQuadric(qc);
    }
    glPopMatrix();

    // Dome (kubah uap)
    mat(0.50f,0.40f,0.08f, 0.75f,0.55f,0.12f, 100.0f);
    glPushMatrix(); glTranslatef(0.5f, 1.72f, 0);
    glutSolidSphere(0.25f, 10, 8);
    glPopMatrix();

    // 4 Pasang Roda
    float wheelXpos[4]={-1.40f, -0.47f, 0.47f, 1.40f};
    for(int w=0;w<4;w++){
        for(int s=-1;s<=1;s+=2){
            float ang = (s>0) ? wheelAngle : -wheelAngle;
            drawWheelAt(wheelXpos[w], 0.0f, s*0.70f, ang);
        }
    }

    // Connecting rod
    mat(0.32f,0.27f,0.18f, 0.55f,0.48f,0.20f, 75.0f);
    {
        float rodR  = 0.38f;
        float rodX2 = cosf(wheelAngle * PI/180.0f) * rodR * 0.65f;
        float rodY2 = sinf(wheelAngle * PI/180.0f) * rodR * 0.65f;
        for(int s=-1;s<=1;s+=2){
            glPushMatrix();
            glTranslatef(rodX2, rodY2, s*0.71f);
            box(2.9f, 0.06f, 0.06f);
            glPopMatrix();
        }
    }
}

void drawPassengerCarOnly(int carIndex) {
    // Berikan warna variasi yang berbeda untuk gerbong 1 dan gerbong 2
    float cr = 0.7f, cg = 0.7f, cb = 0.7f; // Default abu-abu
    if (carIndex == 1) { cr = 0.2f; cg = 0.6f; cb = 0.3f; } // Gerbong 1 Hijau
    if (carIndex == 2) { cr = 0.7f; cg = 0.2f; cb = 0.2f; } // Gerbong 2 Merah

    // Underframe
    mat(0.16f,0.16f,0.16f, 0.28f,0.28f,0.28f, 20.0f);
    glPushMatrix(); glTranslatef(0, 0.10f, 0);
    box(4.5f, 0.20f, 1.50f);
    glPopMatrix();

    // Badan
    mat(cr*0.7f, cg*0.7f, cb*0.7f, cr, cg, cb, 50.0f);
    glPushMatrix(); glTranslatef(0, 1.0f, 0);
    box(4.2f, 1.5f, 1.4f);
    glPopMatrix();

    // Atap
    mat(0.15f,0.15f,0.15f, 0.25f,0.25f,0.25f, 20.0f);
    glPushMatrix(); glTranslatef(0, 1.85f, 0);
    box(4.25f, 0.15f, 1.45f);
    glPopMatrix();

    // Jendela (5 per sisi)
    mat(0.50f,0.75f,0.92f, 0.82f,0.92f,1.00f, 120.0f);
    for(int wi=0;wi<5;wi++){
        float wx = -1.60f + wi*0.8f;
        for(int ws=-1;ws<=1;ws+=2){
            glPushMatrix(); glTranslatef(wx, 1.02f, ws*0.71f);
            box(0.50f, 0.45f, 0.06f);
            glPopMatrix();
        }
    }

    // Pintu kedua ujung
    mat(cr*0.75f, cg*0.75f, cb*0.75f, cr*0.65f, cg*0.65f, cb*0.65f, 30.0f);
    for(int de=-1;de<=1;de+=2){
        glPushMatrix(); glTranslatef(de*1.9f, 0.8f, 0);
        box(0.6f, 1.1f, 1.42f);
        glPopMatrix();
    }

    // Buffer
    mat(0.38f,0.38f,0.40f, 0.65f,0.65f,0.68f, 80.0f);
    for(int bz=-1;bz<=1;bz+=2){
        glPushMatrix(); glTranslatef( 2.32f, 0.22f, bz*0.50f);
        box(0.15f,0.25f,0.15f); glPopMatrix();
        glPushMatrix(); glTranslatef(-2.32f, 0.22f, bz*0.50f);
        box(0.15f,0.25f,0.15f); glPopMatrix();
    }

    // Roda: 2 bogie x 2 as x 2 sisi
    float bogieX[2]={-1.4f, 1.4f};
    float axleOff[2]={-0.40f, 0.40f};
    for(int b=0;b<2;b++){
        for(int a=0;a<2;a++){
            for(int s=-1;s<=1;s+=2){
                float ang = (s>0) ? wheelAngle : -wheelAngle;
                drawWheelAt(bogieX[b]+axleOff[a], 0.0f, s*0.70f, ang);
            }
        }
    }
}


void drawTrain(){
    // Fungsi ini sekarang murni bertugas menggambar lokomotif di titik (0,0,0) lokalnya.
    // Semua gerbong sudah ditarik keluar ke fungsi display() untuk akurasi sumbu koordinat.
    drawLocomotiveOnly();
}
