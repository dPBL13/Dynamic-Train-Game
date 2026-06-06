#pragma once
// =====================================================
// globals.h - Konstanta & State Global
// TRAIN DYNAMICS - Kelompok 8
// =====================================================

#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <deque>
#include <cstdlib>
#include <ctime>

// -- Konstanta ---------------------------------------
#define PI            3.14159f
#define REL_PANJANG   300.0f
#define JUMLAH_POHON  80
#define TOTAL_TURNS   10
#define TURN_DISTANCE 100.0f // Jarak per 1 turn (loop)

extern float posY;

// -- Posisi & Fisika Kereta --------------------------
extern float posZ;        // posisi kereta di sumbu Z
extern float wheelAngle;  // sudut rotasi roda (derajat)
extern float trainYaw;	  // Rotasi Yaw absolut lokomotif

// Struktur untuk menyimpan jejak posisi dan rotasi
struct TrainState {
    float x;
    float z;
    float yaw;
};

// Deklarasi buffer global untuk mencatat jejak lokomotif
extern std::deque<TrainState> trainHistory;

struct CarriageState {
    float x;
    float z;
    float yaw;
};

// -- Status Siklus Siang-Malam & Bayangan Kelompok 8 --
extern float sunAngle;         // Sudut rotasi matahari (0 - 360 derajat)
extern bool isNight;           // Status penanda waktu malam hari

// Fungsi pembentuk matriks proyeksi bayangan datar
void shadowMatrix(float shadowMat[16], float groundplane[4], float lightpos[4]);

// Variabel global pemegang koordinat real-time gerbong
extern CarriageState gerbong1;
extern CarriageState gerbong2;

// -- State Baru untuk Mekanisme Turntable --
extern bool  diAtasTurntable;      // Status apakah kereta sedang dikunci di atas piringan
extern float turntableX;           // Posisi pusat X piringan (tempat percabangan)
extern float turntableZ;           // Posisi pusat Z piringan (tempat percabangan)
extern float targetTurntableYaw;   // Sudut tujuan setelah diputar (misal: -15.0f, 0.0f, 15.0f)
extern bool turntableSelesai;
extern float lastHistoryZ;

// -- Mode Kamera -------------------------------------
// 1 = Driver / In-Train
// 2 = Free Cam
// 3 = Side View
// 4 = Top-Down miring
// 5 = 3rd Person Smooth
extern int cameraMode;

// -- Free Camera State -------------------------------
extern float camX, camY, camZ;
extern float yaw, pitch;

// Variabel untuk menghaluskan pergerakan cameraMode 5 (agar tidak kaku)
extern float smoothedCamX;
extern float smoothedCamY;

// -- Proyeksi ----------------------------------------
// 1 = Perspective (gluPerspective)
// 2 = Orthographic (glOrtho)
extern int projectionMode;

// -- Lighting & Shading ------------------------------
extern int lightingEnabled;
// 1 = GL_FLAT, 2 = GL_SMOOTH (Gouraud)
extern int shadingMode;

// -- Auto-Move & Cinematic ---------------------------
extern int   autoMove;
extern float autoSpeed;
extern int   cinematic;
extern float cinAngle;

// -- Holder Textxure  ---------------------------

extern GLuint texDaun;
extern GLuint texBricks;
extern GLuint texRoof;
extern GLuint texRoad;
extern GLuint texWood;
extern GLuint texConcrete;
extern GLuint texRock;
extern GLuint texSky;

// -- Data Pohon --------------------------------------
struct TreePos { float x, z, scale; };
extern TreePos trees[JUMLAH_POHON];

// posisi rel kereta
extern float posX;          // Posisi lateral kereta (X) untuk belok
extern int jalurPilihan;    // 0 = Lurus, 1 = Kiri, 2 = Kanan
extern int diAreaCabang;    // Status apakah kereta berada di area pencabangan (0 = Tidak, 1 = Ya)

// -- Game Mechanics State ---------------------------
// gameState: -1 = Main Menu, 0 = Playing, 1 = Win, 2 = Game Over
extern int gameState;       
extern int gameMode;        // 1 = Countdown Timer, 2 = Real-time Clock
extern float timeRemaining; // Digunakan untuk Mode 1 (Countdown)
extern float timeElapsed;   // Digunakan untuk Mode 2 (Real-time Clock)
extern float startZ;        

// Batas area kotak finish (Bounding Box)
extern float finishMinX, finishMaxX;
extern float finishMinZ, finishMaxZ;

// MODE 4
extern float infiniteScore;      // Mencatat total jarak tempuh akumulatif di Mode 4
extern float timeElapsedMode4;  // Waktu bertahan hidup dalam hitungan detik
extern int targetLoopCount;       // Jumlah total pengulangan (di-set acak nanti)
extern int currentLoopCount;       // Menghitung putaran yang sudah diselesaikan
extern float globalTimer;       // Timer game yang akumulatif (tidak di-reset!)

// -- Fungsi Global Prototyping -----------------------
void gameTimerUpdate(int value);
void updateJalurDanPosisi(bool isMovingForward);
void updateGerbongPositions();



// -------------------------------------- IZZAT UPDATE --------------------------------------

// Type Rintangan
enum ObstacleType {
    NONE = 0,
    // Fatal (Game Over)
    ORANG, HEWAN, LUBANG, REL_PUTUS, MOBIL, BOM,
    // Non-Fatal (Barrier / Bounce Back)
    POHON_TUMBANG, BATU_BESAR, GERBANG_TERTUTUP, RUMAH_SIMPLE
};

struct TurnData {
    ObstacleType kiriType;
    ObstacleType tengahType;
    ObstacleType kananType;
    float startZ; // Posisi awal Z turn ini
};

// ---- State Tambahan untuk 10 Turns System ----
extern TurnData gameTurns[TOTAL_TURNS];
extern int currentTurnIndex; // Mengikuti turn aktif saat ini (0 s/d 9)

// Tahan variabel fisika dasar bawaan Anda  
extern int jalurPilihan;    // -1 = Kiri, 0 = Tengah, 1 = Kanan 
extern int gameState;       // -1 = Menu, 0 = Playing, 1 = Win, 2 = Game Over
extern bool laneLocked;   // MECHANIC 3: Pengunci jalur aktif

// <trainstate - carriage state diatas>

// Prototyping Fungsi Baru
void InitProceduralTurns();
void GenerateTurn(int turnNumber);
void BounceBackToTurn();
void GameOver();
void CheckObstacleCollisions();

extern bool historyInitialized;

extern float glowTimer;



extern bool isMuted;            // Status audio mute

// Deklarasi fungsi audio pengendali global
void playBGM();
void toggleMusic();
void stopBGM();
