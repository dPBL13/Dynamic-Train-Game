#pragma once

#include <GL/glut.h>
#include <cmath>
#include <cstdio>
#include <deque>
#include <cstdlib>
#include <ctime>

#define PI            3.14159f
#define REL_PANJANG   300.0f
#define JUMLAH_POHON  80
#define TOTAL_TURNS   10
#define TURN_DISTANCE 100.0f

// Struktur Data
struct TrainState {
    float x;
    float z;
    float yaw;
};

struct CarriageState {
    float x;
    float z;
    float yaw;
};

enum ObstacleType {
    NONE = 0,
    ORANG, HEWAN, LUBANG, REL_PUTUS, MOBIL, BOM,             // Fatal
    POHON_TUMBANG, BATU_BESAR, GERBANG_TERTUTUP, RUMAH_SIMPLE // Non-Fatal
};

struct TurnData {
    ObstacleType kiriType;
    ObstacleType tengahType;
    ObstacleType kananType;
    float startZ;
};

struct TreePos { 
    float x, z, scale; 
};

// Variabel Global (Extern)
extern float posX, posY, posZ;
extern float wheelAngle;
extern float trainYaw;
extern std::deque<TrainState> trainHistory;
extern CarriageState gerbong1;
extern CarriageState gerbong2;

extern float sunAngle;
extern bool isNight;

extern bool diAtasTurntable;
extern float turntableX, turntableZ;
extern float targetTurntableYaw;
extern bool turntableSelesai;
extern float lastHistoryZ;

extern int cameraMode;
extern float camX, camY, camZ;
extern float yaw, pitch;
extern float smoothedCamX, smoothedCamY;

extern int projectionMode;
extern int lightingEnabled;
extern int shadingMode;

extern int autoMove;
extern float autoSpeed;
extern int cinematic;
extern float cinAngle;

extern unsigned int texDaun;
extern unsigned int texConcrete;
extern unsigned int texRoad;
extern unsigned int texRoof;
extern unsigned int texWood;
extern unsigned int texRock;
extern unsigned int texBricks;
extern unsigned int texSky;

extern TreePos trees[JUMLAH_POHON];

extern int jalurPilihan; // -1 = Kiri, 0 = Tengah, 1 = Kanan
extern int diAreaCabang;
extern bool laneLocked;

extern int gameState; // -1 = Menu, 0 = Playing, 1 = Win, 2 = Game Over
extern int gameMode;
extern float timeRemaining;
extern float timeElapsed;
extern float startZ;
extern float finishMinX, finishMaxX, finishMinZ, finishMaxZ;

extern float infiniteScore;
extern float timeElapsedMode4;
extern int targetLoopCount;
extern int currentLoopCount;
extern float globalTimer;

extern TurnData gameTurns[TOTAL_TURNS];
extern int currentTurnIndex;
extern bool historyInitialized;
extern float glowTimer;
extern bool isMuted;

// Prototipe Fungsi
void shadowMatrix(float shadowMat[16], float groundplane[4], float lightpos[4]);
void gameTimerUpdate(int value);
void updateJalurDanPosisi(bool isMovingForward);
void updateGerbongPositions();
void InitProceduralTurns();
void GenerateTurn(int turnNumber);
void BounceBackToTurn();
void GameOver();
void CheckObstacleCollisions();
