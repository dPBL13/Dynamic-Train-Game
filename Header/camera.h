#pragma once
// =====================================================
// camera.h  –  Setup Kamera (gluLookAt per mode)
// =====================================================

// Memanggil gluLookAt sesuai cameraMode saat ini.
// Harus dipanggil setelah glLoadIdentity() di display().
void setCamera();
