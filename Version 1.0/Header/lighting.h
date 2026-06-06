#pragma once

// SMLi (shading, material, and ligthning interface)

void setupLighting();
void setMaterial(float r, float g, float b, float sR, float sG, float sB, float shine);
void shadowMatrix(float shadowMat[16], float groundplane[4], float lightpos[4]);
