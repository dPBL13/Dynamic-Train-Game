#include "lighting.h"
#include "globals.h"

void setupLighting() {
    if (!lightingEnabled) {
        glDisable(GL_LIGHTING);
        return;
    }

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    // light0, dari kanan atas (matahari semi-dinamis)
    GLfloat l0pos[] = { 8.0f, 20.0f, 10.0f, 0.0f };
    GLfloat l0amb[] = { 0.25f, 0.25f, 0.22f, 1.0f };
    GLfloat l0dif[] = { 1.0f,  0.95f, 0.85f, 1.0f };
    GLfloat l0spc[] = { 1.0f,  1.0f,  1.0f,  1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, l0pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  l0amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  l0dif);
    glLightfv(GL_LIGHT0, GL_SPECULAR, l0spc);

    // light1,, filling light objek kereta
    GLfloat l1pos[] = { -10.0f, 3.0f, posZ, 1.0f };
    GLfloat l1amb[] = {  0.05f, 0.05f, 0.12f, 1.0f };
    GLfloat l1dif[] = {  0.25f, 0.28f, 0.50f, 1.0f };
    GLfloat l1spc[] = {  0.05f, 0.05f, 0.10f, 1.0f };
    glLightfv(GL_LIGHT1, GL_POSITION, l1pos);
    glLightfv(GL_LIGHT1, GL_AMBIENT,  l1amb);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  l1dif);
    glLightfv(GL_LIGHT1, GL_SPECULAR, l1spc);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

void setMaterial(float r, float g, float b, float sR, float sG, float sB, float shine) {
    GLfloat amb[] = { r * 0.3f, g * 0.3f, b * 0.3f, 1.0f };
    GLfloat dif[] = { r,        g,        b,        1.0f };
    GLfloat spc[] = { sR,       sG,       sB,       1.0f };
    GLfloat sh[]  = { shine };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   dif);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  spc);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, sh);
    glColor3f(r, g, b);
}

// shadow --> kalkulasi planar
void shadowMatrix(float shadowMat[16], float groundplane[4], float lightpos[4]) {
    float dot = groundplane[0] * lightpos[0] +
                groundplane[1] * lightpos[1] +
                groundplane[2] * lightpos[2] +
                groundplane[3] * lightpos[3];

    // Kolom 1
    shadowMat[0] = dot - lightpos[0] * groundplane[0];
    shadowMat[1] = 0.0f - lightpos[1] * groundplane[0];
    shadowMat[2] = 0.0f - lightpos[2] * groundplane[0];
    shadowMat[3] = 0.0f - lightpos[3] * groundplane[0];

    // Kolom 2
    shadowMat[4] = 0.0f - lightpos[0] * groundplane[1];
    shadowMat[5] = dot - lightpos[1] * groundplane[1];
    shadowMat[6] = 0.0f - lightpos[2] * groundplane[1];
    shadowMat[7] = 0.0f - lightpos[3] * groundplane[1];

    // Kolom 3
    shadowMat[8]  = 0.0f - lightpos[0] * groundplane[2];
    shadowMat[9]  = 0.0f - lightpos[1] * groundplane[2];
    shadowMat[10] = dot - lightpos[2] * groundplane[2];
    shadowMat[11] = 0.0f - lightpos[3] * groundplane[2];

    // Kolom 4
    shadowMat[12] = 0.0f - lightpos[0] * groundplane[3];
    shadowMat[13] = 0.0f - lightpos[1] * groundplane[3];
    shadowMat[14] = 0.0f - lightpos[2] * groundplane[3];
    shadowMat[15] = dot - lightpos[3] * groundplane[3];
}
