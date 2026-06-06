// =====================================================
// main.cpp  –  Entry Point & GLUT Callbacks
// TRAIN DYNAMICS - Kelompok 8
//
// Komponen Rendering Pipeline:
//   1. Representasi Objek 3D   (kereta, rel, pohon, gunung)
//   2. Proyeksi                (Perspective / Orthographic)
//   3. Sudut Pandang:
//        [1] In-Train / 1-point perspective
//        [2] Free Camera
//        [3] Side View / 2-point perspective
//        [4] Top-Down miring / 3-point perspective
//   4. Rendering Pipeline:
//        Geometri → Transformasi → Kamera → Lighting →
//        Material → Shading → Depth Test → Rasterisasi
//   5. Auto-Move / Cinematic  [C] / [M]
// =====================================================
#include "globals.h"
#include "lighting.h"
#include "train.h"
#include "world.h"
#include "world2.h"
#include "world3.h"
#include "camera.h"
#include "hud.h"

CarriageState gerbong1 = {0.0f, 0.0f, 0.0f};
CarriageState gerbong2 = {0.0f, 0.0f, 0.0f};

void init() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.47f, 0.73f, 0.98f, 1.0f);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);

    initTrees();

    glEnable(GL_TEXTURE_2D);
    texDaun = loadBMP("dauntexture.bmp");
    texBricks = loadBMP("bricktexture.bmp");
    
    texRoof = loadBMP("bricks.bmp");
	texRoad = loadBMP("asphalt.bmp");
	texWood = loadBMP("wood.bmp");
	texConcrete = loadBMP("concrete.bmp");
	texRock = loadBMP("bricks.bmp");
	texSky = loadBMP("langit.bmp");

	// nanti cek di system "pause" terminal
    if (texDaun == 0) {
        printf("GAGAL LOAD TEXTURE RUMPUT\n");
    } else {
        printf("BERHASIL LOAD TEXTURE RUMPUT\n");
    }
    
    if (texBricks == 0) {
        printf("GAGAL LOAD TEXTURE BATA\n");
    } else {
        printf("BERHASIL LOAD TEXTURE BATA\n");
    }
    // Jalankan seed acak berdasarkan waktu lokal komputer Anda
    srand(glutGet(GLUT_ELAPSED_TIME));
    
    // Set jumlah putaran secara acak antara rentang 2 sampai 5 kali
    targetLoopCount = (rand() % 4) + 2; 
    currentLoopCount = 0;
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (projectionMode == 1)
        gluPerspective(60.0, (float)w / h, 0.3, 600.0);
    else {
        float asp = (float)w / h;
        glOrtho(-10 * asp, 10 * asp, -10, 10, 0.3, 600.0);
    }
    glMatrixMode(GL_MODELVIEW);
}


// cek start-finish
void checkGameStatus() {
    if (gameState == 0) { // Hanya cek jika game sedang berjalan
        
        // LOGIKA BARU: Cek apakah koordinat posisi kereta masuk ke dalam Bounding Box Finish
        if (posX >= finishMinX && posX <= finishMaxX &&
            posZ >= finishMinZ && posZ <= finishMaxZ) {
            
            if (gameMode == 4) {
            // Cek apakah masih ada kuota pengulangan loop waktu
            if (currentLoopCount < targetLoopCount) {
                currentLoopCount++;
                
                // --- PROSES TELEPORTASI INSTAN (NEAR-INFINITE RESET) ---
                posZ = 150.0f;   // Kembalikan posisi kereta ke koordinat awal stasiun start
                posX = 0.0f;     // Reset posisi jalur ke tengah
                
                // CATATAN KRUSIAL: globalTimer TIDAK DI-RESET agar waktu akumulatif tetap terbawa!
                
                printf("[TIME LOOP RESET] Putaran %d/%d Selesai! Dilempar kembali ke awal. Timer berjalan: %.2f detik\n", 
                       currentLoopCount, targetLoopCount, globalTimer);
            } 
            else {
                // Jika semua putaran acak sudah dipenuhi, barulah trigger MENANG SEBENARNYA
                gameState = 1;  // Nyalakan flag menang (Game Over / Victory Screen)
                autoMove = 0;   // Hentikan laju otomatis kereta
                printf("[MISSION ACCOMPLISHED] Akhirnya keluar dari Time Loop setelah %d putaran! Waktu Total: %.2f detik\n", 
                       targetLoopCount, globalTimer);
            }
        } 
        else {
            // Logika Normal untuk Game Mode selain Mode 4
            gameState = 1;
            autoMove = 0;
        }
            printf("[MISSION ACCOMPLISHED] Kereta Kelompok 8 Berhasil Masuk Depo dengan Aman!\n");
        }
    }
}

// ==================================== 

void initTrainHistory() {
    
    float rad = trainYaw * 3.14159265f / 180.0f;

    gerbong1.x   = posX + sin(rad) ;
    gerbong1.z   = posZ + cos(rad) ;
    gerbong1.yaw = trainYaw;

    gerbong2.x   = posX + sin(rad) ;
    gerbong2.z   = posZ + cos(rad) ;
    gerbong2.yaw = trainYaw;
}

void timer(int value) {
	// Jarak gerbong
    float jarakG1_dari_Loko = 4.3f; 
    float jarakG2_dari_Loko = 8.8f; 
	
	if (posZ > 10.0f) {
        posZ = 10.0f;
	}
        
    // -- PROSES ROTASI OTOMATIS DI ATAS TURNTABLE --
    if (diAtasTurntable) {
        posX = turntableX;
        posZ = turntableZ;

        float toleransi = 0.5f;
        if (fabs(trainYaw - targetTurntableYaw) > toleransi) {
            if (trainYaw < targetTurntableYaw) {
                trainYaw += 1.0f; 
                wheelAngle -= 4.0f; // Efek visual: roda bergeser sedikit mengikuti gesekan piringan
            } else {
                trainYaw -= 1.0f; 
                wheelAngle += 4.0f; 
            }
        } else {
            trainYaw = targetTurntableYaw;
            diAtasTurntable = false; 
            turntableSelesai = true; 
            
            updateJalurDanPosisi(true); 
        }
    }
    
    if (gameState == 0 && autoMove) {
        posZ -= autoSpeed;
        wheelAngle += 10.0f; // Sinkronisasi rotasi roda kontinu otomatis
        updateJalurDanPosisi(true);
    }
    
    if (gameState == 0 && gameMode == 4) {
            // Naikkan catatan waktu (16ms per frame terkonversi ke detik)
            timeElapsedMode4 += 0.016f;
            
            // Akumulasi skor berdasarkan kecepatan gerak real-time lokomotif
            if (posZ < lastHistoryZ) {
                infiniteScore += lastHistoryZ;
            }
            sunAngle += 0.2f; // Naikkan angka ini untuk mempercepat putaran waktu
            if (sunAngle >= 360.0f) {
                sunAngle -= 360.0f;
            }
    }
    

    // Ubah sudut yaw lokomotif ke radian untuk kalkulasi buntut gerbong
    float rad = trainYaw * PI / 180.0f;

    // Hitung posisi gerbong (Otomatis ikut meliuk miring melengkung secara fleksibel)
    gerbong1.x   = posX + sin(rad) * jarakG1_dari_Loko;
    gerbong1.z   = posZ + cos(rad) * jarakG1_dari_Loko;
    gerbong1.yaw = trainYaw;

    gerbong2.x   = posX + sin(rad) * jarakG2_dari_Loko;
    gerbong2.z   = posZ + cos(rad) * jarakG2_dari_Loko;
    gerbong2.yaw = trainYaw;

    // --- Sisa Logika Kamera Cinematic Kelompok Anda ---
    if (cinematic) {
        cinAngle += 0.4f;
        if (cinAngle >= 360.0f) cinAngle -= 360.0f;

        if (cameraMode == 2) {
            float radCin = cinAngle * 3.14159265f / 180.0f;
            float dist   = 12.0f;
            float height = 4.0f + sin(radCin * 0.5f) * 2.0f;
            camX = sin(radCin) * dist;
            camZ = posZ + cos(radCin) * dist;
            camY = height;
            yaw  = -(cinAngle);
        }
        wheelAngle -= autoSpeed * 24.0f;
    }
    
    

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0); 
}

// ================================

void display() {
	float lightPos[4] = { 0.0f, 50.0f, -25.0f, 1.0f };
	
	if (gameMode == 4) {
        // Konversi derajat sudut ke Radian untuk fungsi trigonometri cmath
        float rad = sunAngle * 3.14159265f / 180.0f;
        
        // Atur lintasan orbit lampu melingkar pada sumbu X (kiri-kanan) dan Y (atas-bawah)
        lightPos[0] = cosf(rad) * 60.0f;
        lightPos[1] = sinf(rad) * 60.0f; 
        lightPos[2] = -50.0f; // Tetap di area tengah lintasan Z
        lightPos[3] = 1.0f;   // 1.0f artinya lampu bertipe Positional (Point Light)

        // Deteksi apakah matahari berada di bawah garis horizon (Y < 0)
        if (lightPos[1] < 0.0f) {
            isNight = true;
            // Warna Langit Malam: Hitam Pekat
            glClearColor(0.02f, 0.02f, 0.05f, 1.0f);
        } else {
            isNight = false;
            // Jika matahari sedang terbit/terbenam (Y rendah dekat horizon)
            if (lightPos[1] < 15.0f) {
                // Warna Langit Senja / Fajar: Oranye Kemerahan
                glClearColor(0.9f, 0.4f, 0.1f, 1.0f);
            } else {
                // Warna Langit Siang Standar: Biru Cerah Kelompok 8
                glClearColor(0.47f, 0.73f, 0.98f, 1.0f);
            }
        }
    } else {
        // Mode 1-3 menggunakan langit siang standar konstan
        glClearColor(0.47f, 0.73f, 0.98f, 1.0f);
        isNight = false;
    }
	
    checkGameStatus();
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    setCamera();
    
    // ===================================================================
    // PHASE 2: PENYETELAN INTENSITAS CAHAYA LAMPU (GL_LIGHT0)
    // ===================================================================
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    if (isNight) {
        float groundPlane[4] = { 0.0f, 1.0f, 0.0f, 0.6f }; 
        float matBayangan[16]; // <--- Menggunakan Array 1D Baru (16 Elemen)
        
        // Hitung matriks dengan gaya baru
        shadowMatrix(matBayangan, groundPlane, lightPos);

        glPushMatrix(); 
        glPushAttrib(GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_TEXTURE_BIT);

        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_COLOR_MATERIAL); 
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        // Warna bayangan: Hitam transparan kalem (Alpha 0.35f)
        glColor4f(0.0f, 0.0f, 0.0f, 0.35f); 

        // Hindari Z-fighting dengan mengangkat sedikit posisi gambar
        glTranslatef(0.0f, 0.005f, 0.0f); 
        
        // Panggil matriks (Langit-langit koordinat langsung aman karena formatnya sudah Column-Major)
        glMultMatrixf(matBayangan); 

        // RENDER HANYA KERETA
        drawTrain(); 

        glPopAttrib();
        glPopMatrix();
    } else {
        // Cahaya Siang Hari Terang Benderang
        GLfloat ambientSiang[]  = { 0.3f, 0.3f, 0.3f, 1.0f };
        GLfloat diffuseSiang[]  = { 1.0f, 1.0f, 0.9f, 1.0f }; // Agak kekuningan seperti matahari asli
        GLfloat specularSiang[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glLightfv(GL_LIGHT0, GL_AMBIENT,  ambientSiang);
        glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuseSiang);
        glLightfv(GL_LIGHT0, GL_SPECULAR, specularSiang);
    }
    
    // Daftarkan koordinat posisi lampu ke dalam matriks dunia
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	
	drawSkybox(posX, posY, posZ);
	
    glPushMatrix();
        drawGround();
        drawRail();
        drawAllTrees();
        drawStartGates();
        drawFinishAreaZone();
        drawEnvironmentDecorations();
    	drawStaticEnvironment(); 
		draw3DMeshHills();
		drawTurnZoneIndicators();
		
		// Depot asli di ujung jalur rel
		drawTrainDepot(0.0f, -0.6f, -1000.0f);
		// Depot kedua di samping
		drawTrainDepot(30.0f, -0.6f, -500.0f);
		// Depot ketiga di area lain
		drawTrainDepot(0.0f, -0.6f, 0.0f);

    glPopMatrix();
    

	// ===================================================================
    // PHASE 3: REAL-TIME PLANAR SHADOW RENDERING LAYER
    // ===================================================================
    // HANYA RENDER BAYANGAN JIKA SIANG HARI (Ada Matahari)
    if (!isNight) {
        // 1. Definisikan Persamaan Bidang Tanah Anda. 
        // Jika permukaan tanah Anda berada di Y = -0.6f, maka persamaannya adalah {0, 1, 0, 0.6}
        // Rumus umum: {Nx, Ny, Nz, D} di mana D = -Y_posisi_tanah
        float groundPlane[4] = { 0.0f, 1.0f, 0.0f, 0.6f }; 
        float matBayangan[16];
        
        // Hitung matriks proyeksi berdasarkan posisi matahari saat ini
        shadowMatrix(matBayangan, groundPlane, lightPos);

        glPushMatrix();
        
        // PENCEGAHAN Z-FIGHTING: Naikkan posisi rendering matriks bayangan sedikit 
        // di atas permukaan tanah (0.002 unit) agar tidak berebut piksel dengan tanah hijau
        glTranslatef(0.0f, 0.002f, 0.0f);
        
        // Kalikan matriks transformasi aktif dengan matriks bayangan datar
        glMultMatrixf((GLfloat*)matBayangan);

        // Matikan Pencahayaan dan Tekstur agar bayangan terlukis sebagai siluet gelap homogen
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        
        // Aktifkan blending agar bayangan terlihat semi-transparan hitam di atas rumput
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        // Set warna bayangan: Hitam transparan (Alpha = 0.4f)
        glColor4f(0.0f, 0.0f, 0.0f, 0.4f);

        // PANGGIL KEMBALI fungsi kereta Anda di sini.
        // Komputer akan otomatis memipihkan model kereta ini menjadi bayangan di tanah mengikuti orbit matahari
        drawTrain();

        // Kembalikan kondisi pipeline ke status normal
        glDisable(GL_BLEND);
        glEnable(GL_LIGHTING);
        glPopMatrix();
    }
    
    if (cameraMode != 1) {
        // =============================================================
        // AMBIL HISTORI SEJARAH SESUAI ANTRIAN FRAME (SANGAT STABIL)
        // =============================================================
        size_t delayGerbong1 = 9; // Jarak frame lokomotif -> gerbong 1
        size_t delayGerbong2 = 18; // Jarak frame lokomotif -> gerbong 2

        TrainState g1Visual, g2Visual;

        // Jarak offset aman saat awal game biar tidak masuk ke dalam bodi lokomotif
        float jarakMundurBawaan1 = 4.5f; 
        float jarakMundurBawaan2 = 9.0f;

        if (!trainHistory.empty() && trainHistory.size() > delayGerbong2) {
            // Ambil data masa lalu secara konstan dari antrean deque
            g1Visual = trainHistory[delayGerbong1];
            g2Visual = trainHistory[delayGerbong2];
        } else {
            // Jika game baru mulai dan baris memori belum cukup, gunakan offset fisik lurus
            g1Visual.x = posX; g1Visual.z = posZ + jarakMundurBawaan1; g1Visual.yaw = trainYaw;
            
			g2Visual.x = posX; g2Visual.z = posZ + jarakMundurBawaan2; g2Visual.yaw = trainYaw;
        }

        // -------------------------------------------------------------
        // 1. GAMBAR LOKOMOTIF
        // -------------------------------------------------------------
        glPushMatrix();
            glTranslatef(posX, -0.275f, posZ);
            glRotatef(trainYaw, 0.0f, 1.0f, 0.0f);
            glRotatef(90.0f + trainYaw, 0.0f, 1.0f, 0.0f); 
            drawTrain(); 
        glPopMatrix();

        // -------------------------------------------------------------
        // 2. GAMBAR GERBONG 1
        // -------------------------------------------------------------
        glPushMatrix();
            glTranslatef(g1Visual.x, -0.725f, g1Visual.z);
            glRotatef(g1Visual.yaw, 0.0f, 1.0f, 0.0f);
            glRotatef(90.0f, 0.0f, 1.0f, 0.0f); 
            
            glPushMatrix();
                glTranslatef(0.0f, 0.46f, 0.0f);
                drawPassengerCarOnly(1);
            glPopMatrix();
        glPopMatrix();

        // -------------------------------------------------------------
        // 3. GAMBAR GERBONG 2
        // -------------------------------------------------------------
        glPushMatrix();
            glTranslatef(g2Visual.x, -0.725f, g2Visual.z);
            glRotatef(g2Visual.yaw, 0.0f, 1.0f, 0.0f);
            glRotatef(90.0f, 0.0f, 1.0f, 0.0f); 
            
            glPushMatrix();
                glTranslatef(0.0f, 0.46f, 0.0f);
                drawPassengerCarOnly(2);
            glPopMatrix();
        glPopMatrix();
    }

    drawHUD();
    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
	
	// -- EKSEKUSI PERGERAKAN NORMAL VIA VEKTOR TRIGONOMETRI --
    float moveSpeed = 0.5f;
    float rotSpeed  = 2.0f;
    float radYaw    = yaw * 3.14159f / 180.0f;
    
    if (key == 'M' || key == 'm') {
        toggleMusic();
    }
    
    if (key == 9){ // TABS for menu
            gameMode = -1;  // Kembalikan ke State Menu Utama
            gameState = -1;         // Reset status permainan menjadi aktif/bukan game over
            
            // Reset koordinat kereta Kelompok 8 ke posisi aman di stasiun start
            posX = 0.0f;
            posY = 0.0f;
            posZ = 150.0f;
            
            // Reset variabel pendukung Time Loop Mode 4 agar bersih kembali
            currentLoopCount = 0;
            globalTimer = 0.0f;
            
            initTrainHistory();
            glutPostRedisplay();
            return;
	}
    
    // if1: MENU UTAMA
    if (gameState == -1) {
        if (key == '1') { gameMode = 1; timeRemaining = 45.0f; playBGM();}
        else if (key == '2') { gameMode = 2; timeElapsed = 0.0f; playBGM();}
        // else if (key == '4') { gameMode = 3; timeRemaining = 30.0f; playBGM();} // FUEL GAME --> CANCELLED
        else if (key == '3') { 
            gameMode = 4; 
            gameState = 0; 
            infiniteScore = 0.0f;
            timeElapsedMode4 = 0.0f;
            posZ = 0.0f; // Mulai dari titik awal lurus
            trainHistory.clear();
            playBGM(); // Putar musik pengiring game
        }
        
		InitProceduralTurns(); // Mengacak ulang seluruh 10 turn prosedural
        posZ = 0.0f;
        posX = 0.0f;
        jalurPilihan = 0;
        
        // Reset flag pembantu perekaman jarak
    	historyInitialized = true;
    
        gameState = 0; // Mulai bermain kembali
        trainHistory.clear();
        if (key == 27) exit(0);
        glutPostRedisplay();
        return;
    }

    // if2: JIKA GAME SELESAI (WIN/LOSE)
    if (gameState == 1 || gameState == 2) {
        if (key == 'r' || key == 'R') {
            gameState = -1; 
			posZ = 0.0f; posX = 0.0f; wheelAngle = 0.0f; trainYaw = 0.0f; 
            autoMove = 0; cinematic = 0; 
			jalurPilihan = 0; diAreaCabang = 0;
            timeRemaining = 45.0f; timeElapsed = 0.0f; 
        
        	laneLocked = false;
        	trainHistory.clear();
        	InitProceduralTurns(); // Acak ulang seluruh rintangan Turn 2 sampai Turn 10 secara prosedural baru
        	printf("[GAME RESET]!\n");
            
            initTrainHistory();
            glutPostRedisplay();
            return;
        }
    }

    // if3: SAAT PERMAINAN BERJALAN
	// Pastikan kontrol hanya aktif saat gameState == 0 (Playing)
    if (gameState == 0) {
        switch (key) {
            case 'a': // Tekan A untuk pindah jalur kiri
                if (!laneLocked) {
                    // Geser target rel ke kiri jika belum mentok di Kiri (-1)
                    if (jalurPilihan > -1) {
                        jalurPilihan--;
                        printf("[LANE CHANGE] Pindah ke Jalur: %d\n", jalurPilihan);
                    }
                } else {
                    printf("[LANE LOCKED] Terkunci di belokan! Tidak bisa pindah jalur.\n");
                }
                break;
            case 'd': // Tekan D untuk pindah jalur kanan
                // BUG FIX: Pasang gerbang pelindung untuk arah kanan.
                if (!laneLocked) {
                    // Geser target rel ke kanan jika belum mentok di Kanan (1)
                    if (jalurPilihan < 1) {
                        jalurPilihan++;
                        printf("[LANE CHANGE] Pindah ke Jalur: %d\n", jalurPilihan);
                    }
                } else {
                    printf("[LANE LOCKED] Terkunci di belokan! Tidak bisa pindah jalur.\n");
                }
                break;
            case 'w': // Gas Maju Kereta
                posZ -= autoSpeed; // Tetap menggunakan kontrol gerak maju Anda
                wheelAngle += 12.0f;
                updateJalurDanPosisi(true); // Parameter true = MAJU
                break;
            case 's': // Rem / Mundur manual
                posZ += autoSpeed;
                wheelAngle -= 12.0f;
                updateJalurDanPosisi(false); // Parameter true = MUNDUR
                break;
        }
    }
        // -- MEKANISME ACTION-COST MODE 3 --
    if (gameState == 0 && gameMode == 3) {
        if (key == 'w' || key == 's' || key == '1' || key == '2' || key == '3' || key == '4' || key == 'a' || key == 'd') {
            timeRemaining -= 0.5f;
            if (timeRemaining <= 0.0f) {
                timeRemaining = 0.0f; gameState = 2; autoMove = 0;
                glutPostRedisplay();
                return;
            }
        }
    }
    
    // Sistem tombol reset manual atau kembali ke menu
    if (key == 'R' || key == 'r') {
        InitProceduralTurns(); // Mengacak ulang seluruh 10 turn prosedural
        posZ = 0.0f;
        posX = 0.0f;
        jalurPilihan = 0;
        lastHistoryZ = 0.0f;
        
        // Reset flag pembantu perekaman jarak
    	historyInitialized = true;
    
        gameState = 0; // Mulai bermain kembali
        trainHistory.clear();
        
        stopBGM();
        playBGM();
    }

    switch (key) {
        case '1': cameraMode = 1; break; 
        case '2': cameraMode = 2; break; 
        case '3': cameraMode = 3; break; 
        case '4': cameraMode = 4; break; 
        case '5': cameraMode = 5; break; 
        
        case 'w':
            if (cameraMode != 2) { 
                // Cek apakah kereta sedang diam di HUD piringan
                if (diAreaCabang == 1 && !diAtasTurntable) {
                    // JIKA PILIH MAJU: Lompat langsung melewati batas Zona 2 agar tidak terhisap kembali
                    diAreaCabang = 0;
                    posZ = -55.4f; 
                    updateJalurDanPosisi(true);
                } 
                // Pergerakan manual normal di luar piringan
                else if (autoMove == 0 && !diAtasTurntable && diAreaCabang == 0) { 
                    posZ -= moveSpeed; 
                    wheelAngle -= 12; 
                    updateJalurDanPosisi(true); 
                }
            } else { 
                camX += sin(radYaw) * moveSpeed; camZ -= cos(radYaw) * moveSpeed; 
            }
            break;
            
        case 's':
            if (cameraMode != 2) { 
                // Cek apakah kereta sedang diam di HUD piringan dan ingin mundur balik ke stasiun awal
                if (diAreaCabang == 1 && !diAtasTurntable) {
                    // JIKA PILIH MUNDUR: Lompat mundur melewati batas belakang piringan
                    diAreaCabang = 0;
                    posZ = -53.2f; 
                    jalurPilihan = 0; // Reset jalur karena kembali ke rel tunggal belakang
                    trainYaw = 0.0f;
                    updateJalurDanPosisi(true);
                } 
                // Pergerakan manual normal di luar piringan
                else if (autoMove == 0 && !diAtasTurntable && diAreaCabang == 0) { 
                    posZ += moveSpeed; 
                    wheelAngle += 12; 
                    updateJalurDanPosisi(true); 
                }
            } else { 
                camX -= sin(radYaw) * moveSpeed; camZ += cos(radYaw) * moveSpeed; 
            }
            break;

        case 'a': if (cameraMode == 2) { camX -= cos(radYaw) * moveSpeed; camZ -= sin(radYaw) * moveSpeed; } break;
        case 'd': if (cameraMode == 2) { camX += cos(radYaw) * moveSpeed; camZ += sin(radYaw) * moveSpeed; } break;
        case 't': if (cameraMode == 2) camY += moveSpeed; break;
        case 'g': if (cameraMode == 2) camY -= moveSpeed; break;
        case 'q': if (cameraMode == 2) yaw  -= rotSpeed;  break;
        case 'e': if (cameraMode == 2) yaw  += rotSpeed;  break;
        case 'R': if (cameraMode == 2) pitch += rotSpeed; break;
        case 'F': if (cameraMode == 2) pitch -= rotSpeed; break;
        case 27: exit(0);
    }
    
    if (key == 27) exit(0); // Tombol ESC untuk close program
    glutPostRedisplay();

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glutPostRedisplay();
}



int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(960, 680);
    glutCreateWindow("THREE MUST KE TRAIN - Kelompok 8");

    init();
	initTrainHistory();
	initEnvironmentData();
	
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(100, timer, 0);
	glutTimerFunc(100, gameTimerUpdate, 0); // Mulai timer game loop
	InitProceduralTurns();

    glutMainLoop();
    return 0;
}
