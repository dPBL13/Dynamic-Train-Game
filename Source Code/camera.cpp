// =====================================================
// camera.cpp  –  Setup Kamera (gluLookAt per mode)
//
// Disesuaikan untuk kereta detail (skala lebih besar):
//   Lokomotif panjang ~4.2u, lebar ~1.4u, tinggi ~2.6u
//   + 2 gerbong, total panjang rangkaian ~14-15u
//
// Koordinat dunia:
//   Rel di sumbu Z, lebar 1.0u antar rel.
//   Kereta bergerak ke Z negatif (posZ berkurang).
//   Kereta di-rotate 90° Y di display(), jadi:
//     arah maju rangkaian = Z dunia (dari kamera mode 1/3/4)
//
// Mode kamera:
//   1 = In-Train / 1-point perspective (kabin masinis)
//   2 = Free Camera
//   3 = Side View / 2-point perspective
//   4 = Top-Down miring / 3-point perspective
// =====================================================
#include "camera.h"
#include "globals.h"

void setCamera() {
    switch (cameraMode) {

        case 1:{
            // ── 1-POINT PERSPECTIVE: Dari kabin masinis ──
            // Kabin ada di X lokal = -1.1, tapi di dunia (setelah rotasi 90°Y)
            // offset X lokal → offset Z dunia, offset Z lokal → offset X dunia.
            // Kabin masinis: X lokal -1.1 → Z dunia = posZ + 1.1
            // Ketinggian kabin: Y lokal 1.9 + offset angkat 0.46 = 2.36 + y rel
            // Rel ground ~ y = -0.66; kereta ground = 0.
            // Posisi mata = (0, 2.4, posZ + 1.1), pandang ke (0, 2.0, posZ - 30)
            
			// -- 1-POINT PERSPECTIVE: Kamera Kabin Masinis --
    		float headingRad = trainYaw * PI / 180.0f;
    		float lookX = posX - sin(headingRad) * 5.0f;
    		float lookZ = posZ - cos(headingRad) * 5.0f;

    		gluLookAt(
        		posX, 1.1f, posZ + 1.1f, // Posisi mata masinis (mengikuti posX kereta)
        		lookX, 1.0f, lookZ,       // Target pandangan (menoleh halus mengikuti arah moncong kereta)
        		0.0f, 1.0f, 0.0f
    		);
    		}
    		break;

        case 2:{
            // ── FREE CAMERA ──
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
        	}
            break;

        case 3:
            // ── 2-POINT PERSPECTIVE: Samping kereta ──
            // Rangkaian total panjang ~15u, tengah di lokomotif (Z=posZ).
            // Kamera dari samping kanan (X besar), melihat ke tengah kereta.
            // Geser sedikit ke belakang agar seluruh rangkaian terlihat.
            gluLookAt(
                18.0f, 3.5f, posZ - 5.0f,   // kamera samping kanan, sedikit ke belakang
                 0.0f, 1.5f, posZ - 5.0f,   // lihat kereta dari samping
                 0.0f, 1.0f, 0.0f
            );
            break;

        case 4:
            // ── 3-POINT PERSPECTIVE: Atas-miring (bird's eye) ──
            // Lihat dari atas-depan-samping, pusat pandang di tengah rangkaian.
            gluLookAt(
                14.0f, 18.0f, posZ + 10.0f,   // atas-depan-samping
                 0.0f,  1.5f, posZ - 5.0f,    // pusat: tengah rangkaian
                 0.0f,  1.0f, 0.0f
            );
            break;
            
        case 5:
    		// -- 3rd PERSON CAMERA (Belakang Atas Kereta) --
    		// Target ideal posisi kamera berada di belakang kereta (Z + 8.5) dan di atas (Y = 3.5)
    		float targetCamX = posX;
    		float targetCamY = 3.5f;
    		float targetCamZ = posZ + 8.5f;

    		// Teknik LERP: Membuat kamera bergeser mulus mengikuti koordinat X kereta
    		// Nilai 0.1f berarti kamera bergerak mendekati target sebesar 10% setiap frame
    		smoothedCamX += (targetCamX - smoothedCamX) * 0.1f;
    		smoothedCamY += (targetCamY - smoothedCamY) * 0.1f;

    		gluLookAt(
       		 	smoothedCamX, smoothedCamY, targetCamZ, // Posisi Kamera (Mata)
        		posX,         0.2f,          posZ - 5.0f, // Titik yang Dilihat (Fokus ke depan kereta)
        		0.0f,         1.0f,          0.0f         // Sumbu Atas (Up Vector)
    		);
    		break;
    }
}
