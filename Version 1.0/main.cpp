#include "globals.h"
#include "lighting.h"
#include "train.h"
#include "world.h"
#include "world2.h"
#include "world3.h"
#include "camera.h"
#include "hud.h"
#include "bgm.h"

#include <GL/glut.h>

CarriageState gerbong1 = {0.0f, 0.0f, 0.0f};
CarriageState gerbong2 = {0.0f, 0.0f, 0.0f};

void init() {
	printf("Selamat Datang di mode debugging play, untuk menonaktifkan dapat dengan memberi \n");
	printf("tanda comment pada line 484: system('pause'); \n\n");
	
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

    if (texDaun == 0 || texBricks == 0) {
        printf("[TEXTURE ERROR] Salah satu berkas BMP gagal dimuat!\n");
    } else {
        printf("[TEXTURE SUCCESS] Seluruh aset tekstur berhasil dimuat.\n");
    }
    
    srand(glutGet(GLUT_ELAPSED_TIME));
    targetLoopCount = (rand() % 4) + 2; 
    currentLoopCount = 0;
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (projectionMode == 1) {
        gluPerspective(60.0, (float)w / h, 0.3, 600.0);
    } else {
        float asp = (float)w / h;
        glOrtho(-10 * asp, 10 * asp, -10, 10, 0.3, 600.0);
    }
    glMatrixMode(GL_MODELVIEW);
}

void checkGameStatus() {
    if (gameState == 0) {
        // Cek interseksi Bounding Box area finish stasiun akhir
        if (posX >= finishMinX && posX <= finishMaxX &&
            posZ >= finishMinZ && posZ <= finishMaxZ) {
            
            if (gameMode == 4) {
                if (currentLoopCount < targetLoopCount) {
                    currentLoopCount++;
                    // Teleportasi Instan (Near-Infinite Loop Reset)
                    posZ = 150.0f;   
                    posX = 0.0f;     
                    printf("[TIME LOOP RESET] Putaran %d/%d Selesai! Mengulang waktu...\n", currentLoopCount, targetLoopCount);
                } else {
                    gameState = 1;  // Kemenangan Sejati
                    autoMove = 0;   
                    printf("[MISSION ACCOMPLISHED] Keluar dari Time Loop setelah %d putaran!\n", targetLoopCount);
                }
            } else {
                gameState = 1;
                autoMove = 0;
                printf("[MISSION ACCOMPLISHED] Kereta Kelompok 8 Berhasil Masuk Depo!\n");
            }
        }
    }
}

void initTrainHistory() {
    float rad = trainYaw * 3.14159265f / 180.0f;
    gerbong1.x   = posX + sin(rad);
    gerbong1.z   = posZ + cos(rad);
    gerbong1.yaw = trainYaw;
    gerbong2.x   = posX + sin(rad);
    gerbong2.z   = posZ + cos(rad);
    gerbong2.yaw = trainYaw;
}

void timer(int value) {
    float jarakG1_dari_Loko = 4.3f; 
    float jarakG2_dari_Loko = 8.8f; 
    
    if (posZ > 10.0f) posZ = 10.0f;
        
    // Mekanisme Rotasi Turntable Piringan Rel
    if (diAtasTurntable) {
        posX = turntableX;
        posZ = turntableZ;
        float toleransi = 0.5f;
        if (fabs(trainYaw - targetTurntableYaw) > toleransi) {
            if (trainYaw < targetTurntableYaw) {
                trainYaw += 1.0f; 
                wheelAngle -= 4.0f;
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
        wheelAngle += 10.0f; 
        updateJalurDanPosisi(true);
    }
    
    if (gameState == 0 && gameMode == 4) {
        timeElapsedMode4 += 0.016f;
        if (posZ < lastHistoryZ) {
            infiniteScore += lastHistoryZ;
        }
        sunAngle += 0.2f; 
        if (sunAngle >= 360.0f) sunAngle -= 360.0f;
    }

    float rad = trainYaw * PI / 180.0f;
    gerbong1.x   = posX + sin(rad) * jarakG1_dari_Loko;
    gerbong1.z   = posZ + cos(rad) * jarakG1_dari_Loko;
    gerbong1.yaw = trainYaw;

    gerbong2.x   = posX + sin(rad) * jarakG2_dari_Loko;
    gerbong2.z   = posZ + cos(rad) * jarakG2_dari_Loko;
    gerbong2.yaw = trainYaw;

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

void display() {
    float lightPos[4] = { 0.0f, 50.0f, -25.0f, 1.0f };
    
    // Orbit Perputaran Siklus Siang-Malam Lampu Global (Mode Endless)
    if (gameMode == 4) {
        float rad = sunAngle * 3.14159265f / 180.0f;
        lightPos[0] = cosf(rad) * 60.0f;
        lightPos[1] = sinf(rad) * 60.0f; 
        lightPos[2] = -50.0f; 
        lightPos[3] = 1.0f;  

        if (lightPos[1] < 0.0f) {
            isNight = true;
            glClearColor(0.02f, 0.02f, 0.05f, 1.0f);
        } else {
            isNight = false;
            if (lightPos[1] < 15.0f) {
                glClearColor(0.9f, 0.4f, 0.1f, 1.0f); // Senja
            } else {
                glClearColor(0.47f, 0.73f, 0.98f, 1.0f); // Siang
            }
        }
    } else {
        glClearColor(0.47f, 0.73f, 0.98f, 1.0f);
        isNight = false;
    }
    
    checkGameStatus();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Kamera & Viewport Proyeksi
    setCamera();
    
    // Konfigurasi Pencahayaan Utama
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    if (isNight) {
        // Render Bayangan Datar Khusus Malam Hari di Bawah Senter Orbit
        float groundPlane[4] = { 0.0f, 1.0f, 0.0f, 0.6f }; 
        float matBayangan[16];
        shadowMatrix(matBayangan, groundPlane, lightPos);

        glPushMatrix(); 
        glPushAttrib(GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_TEXTURE_BIT);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_COLOR_MATERIAL); 
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        glColor4f(0.0f, 0.0f, 0.0f, 0.35f); 
        glTranslatef(0.0f, 0.005f, 0.0f); 
        glMultMatrixf(matBayangan); 
        drawTrain(); 

        glPopAttrib();
        glPopMatrix();
    } else {
        GLfloat ambientSiang[]  = { 0.3f, 0.3f, 0.3f, 1.0f };
        GLfloat diffuseSiang[]  = { 1.0f, 1.0f, 0.9f, 1.0f }; 
        GLfloat specularSiang[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glLightfv(GL_LIGHT0, GL_AMBIENT,  ambientSiang);
        glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuseSiang);
        glLightfv(GL_LIGHT0, GL_SPECULAR, specularSiang);
    }
    
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    
    // Menggambar Lingkungan & Environment Makro Dunia 3D
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
        
        drawTrainDepot(0.0f, -0.6f, -1000.0f);
        drawTrainDepot(30.0f, -0.6f, -500.0f);
        drawTrainDepot(0.0f, -0.6f, 0.0f);
    glPopMatrix();

    // Render Bayangan Datar (Planar Shadows) untuk Kondisi Siang Hari
    if (!isNight) {
        float groundPlane[4] = { 0.0f, 1.0f, 0.0f, 0.6f }; 
        float matBayangan[16];
        shadowMatrix(matBayangan, groundPlane, lightPos);

        glPushMatrix();
        glTranslatef(0.0f, 0.002f, 0.0f); // Cegah Z-Fighting
        glMultMatrixf((GLfloat*)matBayangan);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
        drawTrain();

        glDisable(GL_BLEND);
        glEnable(GL_LIGHTING);
        glPopMatrix();
    }
    
    // Menggambar Struktur Rangkaian Kereta Kompleks (Loko + Gerbong Buntut Deque)
    if (cameraMode != 1) {
        size_t delayGerbong1 = 9; 
        size_t delayGerbong2 = 18; 
        TrainState g1Visual, g2Visual;

        if (!trainHistory.empty() && trainHistory.size() > delayGerbong2) {
            g1Visual = trainHistory[delayGerbong1];
            g2Visual = trainHistory[delayGerbong2];
        } else {
            g1Visual.x = posX; g1Visual.z = posZ + 4.5f; g1Visual.yaw = trainYaw;
            g2Visual.x = posX; g2Visual.z = posZ + 9.0f; g2Visual.yaw = trainYaw;
        }

        // Lokomotif Utama
        glPushMatrix();
            glTranslatef(posX, -0.275f, posZ);
            glRotatef(trainYaw, 0.0f, 1.0f, 0.0f);
            glRotatef(90.0f + trainYaw, 0.0f, 1.0f, 0.0f); 
            drawTrain(); 
        glPopMatrix();

        // Gerbong Penumpang 1
        glPushMatrix();
            glTranslatef(g1Visual.x, -0.725f, g1Visual.z);
            glRotatef(g1Visual.yaw, 0.0f, 1.0f, 0.0f);
            glRotatef(90.0f, 0.0f, 1.0f, 0.0f); 
            glPushMatrix();
                glTranslatef(0.0f, 0.46f, 0.0f);
                drawPassengerCarOnly(1);
            glPopMatrix();
        glPopMatrix();

        // Gerbong Penumpang 2
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
    float moveSpeed = 0.5f;
    float rotSpeed  = 2.0f;
    float radYaw    = yaw * 3.14159f / 180.0f;
    
    if (key == 'M' || key == 'm') {
        toggleMusic();
    }
    
    // Tombol TAB: Kembali ke Menu Utama Screen Utama
    if (key == 9) { 
        gameMode = -1;  
        gameState = -1;         
        posX = 0.0f; posY = 0.0f; posZ = 150.0f;
        currentLoopCount = 0;
        globalTimer = 0.0f;
        initTrainHistory();
        glutPostRedisplay();
        return;
    }
    
    // --- MODE MENU UTAMA ---
    if (gameState == -1) {
        if (key == '1')      { gameMode = 1; timeRemaining = 60.0f; playBGM(); }
        else if (key == '2') { gameMode = 2; timeElapsed = 0.0f; playBGM(); }
        else if (key == '3') { 
            gameMode = 4; 
            gameState = 0; 
            infiniteScore = 0.0f;
            timeElapsedMode4 = 0.0f;
            posZ = 0.0f;
            trainHistory.clear();
            playBGM();
        }
        
        InitProceduralTurns(); 
        posZ = 0.0f; posX = 0.0f; jalurPilihan = 0;
        historyInitialized = true;
        gameState = 0; 
        trainHistory.clear();
        if (key == 27) exit(0);
        glutPostRedisplay();
        return;
    }

    // --- MODE AFTER-GAME OVER / VICTORY ---
    if (gameState == 1 || gameState == 2) {
        if (key == 'r' || key == 'R') {
            gameState = -1; 
            posZ = 0.0f; posX = 0.0f; wheelAngle = 0.0f; trainYaw = 0.0f; 
            autoMove = 0; cinematic = 0; jalurPilihan = 0; diAreaCabang = 0;
            timeRemaining = 60.0f; timeElapsed = 0.0f; 
            laneLocked = false;
            trainHistory.clear();
            InitProceduralTurns(); 
            initTrainHistory();
            glutPostRedisplay();
            return;
        }
    }

    // --- MODE IN-GAME CONTROLS ---
    if (gameState == 0) {
        // Kontrol Kereta Aktif Jika Bukan Kamera Bebas Mode 2
        if (cameraMode != 2) {
            switch (key) {
                case 'a': case 'A':
                    if (!laneLocked && jalurPilihan > -1) {
                        jalurPilihan--;
                    }
                    break;
                case 'd': case 'D':
                    if (!laneLocked && jalurPilihan < 1) {
                        jalurPilihan++;
                    }
                    break;
                case 'w': case 'W':
                    if (diAreaCabang == 1 && !diAtasTurntable) {
                        diAreaCabang = 0; posZ = -55.4f; 
                        updateJalurDanPosisi(true);
                    } else if (autoMove == 0 && !diAtasTurntable && diAreaCabang == 0) { 
                        posZ -= moveSpeed; wheelAngle -= 12.0f; 
                        updateJalurDanPosisi(true); 
                    }
                    break;
                case 's': case 'S':
                    if (diAreaCabang == 1 && !diAtasTurntable) {
                        diAreaCabang = 0; posZ = -53.2f; jalurPilihan = 0; trainYaw = 0.0f;
                        updateJalurDanPosisi(true);
                    } else if (autoMove == 0 && !diAtasTurntable && diAreaCabang == 0) { 
                        posZ += moveSpeed; wheelAngle += 12.0f; 
                        updateJalurDanPosisi(false); 
                    }
                    break;
            }
        } 
        // Kontrol Kamera Bebas Aktif di Mode Kamera 2
        else {
            switch (key) {
                case 'w': case 'W': camX += sin(radYaw) * moveSpeed; camZ -= cos(radYaw) * moveSpeed; break;
                case 's': case 'S': camX -= sin(radYaw) * moveSpeed; camZ += cos(radYaw) * moveSpeed; break;
                case 'a': case 'A': camX -= cos(radYaw) * moveSpeed; camZ -= sin(radYaw) * moveSpeed; break;
                case 'd': case 'D': camX += cos(radYaw) * moveSpeed; camZ += sin(radYaw) * moveSpeed; break;
                case 't': case 'T': camY += moveSpeed; break;
                case 'g': case 'G': camY -= moveSpeed; break;
                case 'q': case 'Q': yaw  -= rotSpeed;  break;
                case 'e': case 'E': yaw  += rotSpeed;  break;
                case 'R': pitch += rotSpeed; break;
                case 'F': pitch -= rotSpeed; break;
            }
        }

        // Manajemen Pilihan Kamera
        if (key >= '1' && key <= '5') {
            cameraMode = key - '0';
        }
    }
    
    if (key == 'r' || key == 'R') {
        InitProceduralTurns(); 
        posZ = 0.0f; posX = 0.0f; jalurPilihan = 0; lastHistoryZ = 0.0f;
        historyInitialized = true;
        gameState = 0; 
        trainHistory.clear();
        stopBGM(); playBGM();
    }

    if (key == 27) exit(0); 

    if (pitch > 89.0f)  pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(960, 680);
    glutCreateWindow("TRAIN DYNAMICS - Kelompok 8");

    init();
    initTrainHistory();
    initEnvironmentData();
    InitProceduralTurns();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    
    glutTimerFunc(100, timer, 0);
    glutTimerFunc(100, gameTimerUpdate, 0); 

    glutMainLoop();
    
    // system("pause"); // kalau mau debugging lewat terminal (setel jadi glut console, di project options)
    
    return 0;
}
