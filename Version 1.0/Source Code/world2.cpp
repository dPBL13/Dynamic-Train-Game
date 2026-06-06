#include "world2.h"

// =====================================
// 2. Objek Decor Generate (procedural)
// =====================================

// Helper Rumah 3D w/ tekstur
void drawTexturedHouse(float x, float z, float w, float h, float d, float roofHeight) {
    glPushMatrix();
    glTranslatef(x, -0.6f, z); 

    // -- Dinding bata --
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
    
    glDisable(GL_TEXTURE_2D); 

    // -- atap piramid --
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

    // vektor normal atap miring
    float ny = w / 2.0f;
    float nx = roofHeight;
    float len = sqrtf(nx*nx + ny*ny);
    nx /= len; ny /= len; // dinormalisasiin

    glBegin(GL_QUADS);
        // Kemiringan Atap Kanan
        glNormal3f(nx, ny, 0.0f);
        glVertex3f( w/2, h,  d/2);
        glVertex3f( w/2, h, -d/2);
        glVertex3f(0.0f, h + roofHeight, -d/2);
        glVertex3f(0.0f, h + roofHeight,  d/2);
        // Kemiringan Atap Kiri
        glNormal3f(-nx, ny, 0.0f);
        glVertex3f(-w/2, h, -d/2);
        glVertex3f(-w/2, h,  d/2);
        glVertex3f(0.0f, h + roofHeight,  d/2);
        glVertex3f(0.0f, h + roofHeight, -d/2);
    glEnd();
    
    glPopMatrix();
}

// pagar procedural sepanjang z
void drawGuardrails() {
    glColor3f(0.75f, 0.75f, 0.75f); // warna perak
    float lebarRelAman = 12.0f;     

    for (float z = -200.0f; z >= -1000.0f; z -= 8.0f) {
        for (int sisi = -1; sisi <= 1; sisi += 2) {
            float xPos = sisi * lebarRelAman;

            // Tiang Penopang
            glPushMatrix();
            glTranslatef(xPos, -0.6f, z);
            glScalef(0.15f, 1.2f, 0.15f);
            glutSolidCube(1.0f);
            glPopMatrix();

            // Balok Pengunci Horizontal 
            glPushMatrix();
            glTranslatef(xPos, 0.3f, z - 4.0f);
            glScalef(0.08f, 0.12f, 8.0f);
            glutSolidCube(1.0f);
            glPopMatrix();
        }
    }
}

void drawOverpassBridge(float zPos) {
    glPushMatrix();
    glTranslatef(0.0f, -0.6f, zPos);

    // support tiang bridge
    glDisable(GL_TEXTURE_2D); 
    glColor3f(0.45f, 0.47f, 0.49f); 
    glPushMatrix(); glTranslatef(-14.0f, 2.25f, 0.0f); glScalef(1.5f, 4.5f, 2.0f); glutSolidCube(1.0f); glPopMatrix();
    glPushMatrix(); glTranslatef( 14.0f, 2.25f, 0.0f); glScalef(1.5f, 4.5f, 2.0f); glutSolidCube(1.0f); glPopMatrix();

	// walkpath jembatan
    if (texConcrete != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texConcrete);
    }
    glColor3f(1.0f, 1.0f, 1.0f); 

    glPushMatrix();
    glTranslatef(0.0f, 4.5f, 0.0f); // ngepasin platform ini

    glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f);  glVertex3f(-15.0f, 0.0f,  1.5f);
        glTexCoord2f(10.0f, 0.0f); glVertex3f( 15.0f, 0.0f,  1.5f);
        glTexCoord2f(10.0f, 1.0f); glVertex3f( 15.0f, 0.0f, -1.5f);
        glTexCoord2f(0.0f, 1.0f);  glVertex3f(-15.0f, 0.0f, -1.5f);
    glEnd();
    glPopMatrix();
    
    glDisable(GL_TEXTURE_2D); 

    // pagar di jembatan
    glColor3f(0.2f, 0.6f, 0.8f);
    glPushMatrix(); glTranslatef(0.0f, 5.0f,  1.45f); glScalef(29.5f, 0.8f, 0.05f); glutSolidCube(1.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(0.0f, 5.0f, -1.45f); glScalef(29.5f, 0.8f, 0.05f); glutSolidCube(1.0f); glPopMatrix();

    // anak tangga (langsung kiri-kanan jadi)
    glColor3f(0.38f, 0.40f, 0.42f);
    for (int i = 0; i < 15; i++) {
        float stepOffset = i * 0.3f; 
        
        // Tangga Sisi Kiri
        glPushMatrix();
        glTranslatef(-14.0f + stepOffset, 4.35f - stepOffset, 0.0f);
        glScalef(0.6f, 0.3f, 2.0f);
        glutSolidCube(1.0f);
        glPopMatrix();

        // Tangga Sisi Kanan
        glPushMatrix();
        glTranslatef(14.0f - stepOffset, 4.35f - stepOffset, 0.0f);
        glScalef(0.6f, 0.3f, 2.0f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }
    
    glPopMatrix(); 
}

void drawEnvironmentDecorations() {
    // Zona Pedesaan & Pemukiman Awal (After Turn 2)
    drawTexturedHouse(-18.0f, -220.0f, 5.0f, 4.0f, 6.0f, 2.0f);
    drawTexturedHouse(-25.0f, -250.0f, 7.0f, 5.0f, 8.0f, 3.0f);
    drawTexturedHouse( 18.0f, -240.0f, 6.0f, 4.0f, 6.0f, 2.5f);
    drawTexturedHouse( 24.0f, -290.0f, 8.0f, 6.0f, 7.0f, 3.5f);

    // Kompleks Perkotaan Gedung Tinggi (Turn 4+)
    drawTexturedHouse(-20.0f, -420.0f, 8.0f, 12.0f, 8.0f, 4.0f);
    drawTexturedHouse( 22.0f, -460.0f, 9.0f, 15.0f, 9.0f, 5.0f);
    drawTexturedHouse(-22.0f, -500.0f, 7.0f, 9.0f,  7.0f, 3.0f);
    drawTexturedHouse( 19.0f, -580.0f, 8.0f, 11.0f, 8.0f, 4.0f);
    
    // Pemukiman Akhir Lapisan Luar (Turn 7 s/d Turn 10)
    drawTexturedHouse(-18.0f, -750.0f, 6.0f, 4.0f, 6.0f, 2.0f);
    drawTexturedHouse( 19.0f, -820.0f, 7.0f, 5.0f, 7.0f, 2.5f);
    drawTexturedHouse(-26.0f, -900.0f, 8.0f, 7.0f, 8.0f, 3.5f);

    drawGuardrails();
    
    drawOverpassBridge(-350.0f); // Jembatan 1
    drawOverpassBridge(-700.0f); // Jembatan 2
}
