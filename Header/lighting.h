#pragma once
// =====================================================
// lighting.h  –  Setup Lighting & Material
// =====================================================

// Mengaktifkan/menonaktifkan lighting dan mengatur
// LIGHT0 (matahari) dan LIGHT1 (fill biru).
void setupLighting();

// Mengatur properti material untuk objek berikutnya.
//   r,g,b   = warna diffuse utama
//   sR,sG,sB = warna specular
//   shine   = GL_SHININESS (0–128)
void setMaterial(float r, float g, float b,
                 float sR, float sG, float sB, float shine);
                 
// PROTOTIPE FUNGSI BARU (Gaya 1D murni)
void shadowMatrix(float shadowMat[16], float groundplane[4], float lightpos[4]);
