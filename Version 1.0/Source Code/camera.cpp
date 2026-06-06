#include "camera.h"
#include "globals.h"
#include <cmath>

void setCamera() {
    switch (cameraMode) {

        case 1: {
            // 1-POINT PERSPECTIVE: Kabin Masinis
            float headingRad = trainYaw * PI / 180.0f;
            float lookX = posX - sin(headingRad) * 5.0f;
            float lookZ = posZ - cos(headingRad) * 5.0f;

            gluLookAt(
                posX, 1.1f, posZ + 1.1f,  // Posisi mata masinis
                lookX, 1.0f, lookZ,       // Pandangan mengikuti moncong kereta
                0.0f, 1.0f, 0.0f
            );
            break;
        }

        case 2: {
            // FREE CAMERA MODE 
            float ry = yaw   * PI / 180.0f;
            float rp = pitch * PI / 180.0f;
            float dX =  sinf(ry) * cosf(rp);
            float dZ = -cosf(ry) * cosf(rp);
            float dY =  sinf(rp);
            
            gluLookAt(
                camX,      camY,      camZ,
                camX + dX, camY + dY, camZ + dZ,
                0.0f, 1.0f, 0.0f
            );
            break;
        }

        case 3:
            // 2-POINT PERSPECTIVE: Samping Kereta
            gluLookAt(
                18.0f, 3.5f, posZ - 5.0f,  // Kamera side kanan
                 0.0f, 1.5f, posZ - 5.0f, 
                 0.0f, 1.0f, 0.0f
            );
            break;

        case 4:
            // 3-POINT PERSPECTIVE: Bird's Eye View
            gluLookAt(
                14.0f, 18.0f, posZ + 10.0f, // Atas-depan-samping
                 0.0f,  1.5f, posZ - 5.0f,  
                 0.0f,  1.0f, 0.0f
            );
            break;

        case 5: {
            // 3rd PERSON CAMERA: Chase Cam Belakang Kereta 
            float targetCamX = posX;
            float targetCamY = 3.5f;
            float targetCamZ = posZ + 8.5f;

            // Interpolasi biar kamera smooth
            smoothedCamX += (targetCamX - smoothedCamX) * 0.1f;
            smoothedCamY += (targetCamY - smoothedCamY) * 0.1f;

            gluLookAt(
                smoothedCamX, smoothedCamY, targetCamZ, // Posisi kamera chase
                posX,         0.2f,         posZ - 5.0f, // depan loko
                0.0f,         1.0f,         0.0f
            );
            break;
        }
    }
}
