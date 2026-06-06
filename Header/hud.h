#pragma once
// =====================================================
// hud.h  –  Head-Up Display (overlay teks 2D)
// =====================================================

// Menggambar overlay HUD di atas scene 3D.
// Menonaktifkan lighting & depth test sementara,
// lalu memulihkan state OpenGL setelah selesai.
void drawHUD();
