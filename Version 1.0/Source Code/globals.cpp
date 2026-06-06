#include "globals.h"

// Inisialisasi Variabel Global
float posX = 0.0f; 
float posY = 0.0f;
float posZ = 0.0f;
float wheelAngle = 0.0f;
float trainYaw = 0.0f;

std::deque<TrainState> trainHistory;

int cameraMode = 1;
float camX = 6.0f, camY = 3.0f, camZ = 5.0f;
float yaw = -30.0f, pitch = -10.0f;
float smoothedCamX = 0.0f;
float smoothedCamY = 3.5f;

bool diAtasTurntable = false;   
float turntableX = 0.0f;    
float turntableZ = -54.4f;  
float targetTurntableYaw = 0.0f;    
bool turntableSelesai = false;   

int projectionMode = 1;
int lightingEnabled = 1;
int shadingMode = 2;

float sunAngle = 90.0f;
bool isNight = false;

int autoMove = 0;
float autoSpeed = 0.9f;
int cinematic = 0;
float cinAngle = 0.0f;

GLuint texDaun = 0, texBricks = 0, texRoof = 0, texRoad = 0;
GLuint texWood = 0, texConcrete = 0, texRock = 0, texSky = 0;

int jalurPilihan = 0;
int diAreaCabang = 0;
bool laneLocked = false;
int gameState = -1; 

TreePos trees[JUMLAH_POHON];
float startZ = 0.0f;
float lastHistoryZ = 0.0f;
float finishMinX = -1.5f;
float finishMaxX = 1.5f;
float finishMinZ = -10502.0f; 
float finishMaxZ = -1000.0f; 

int gameMode = 1; 
float timeRemaining = 30.0f; 
float timeElapsed = 0.0f;  

TurnData gameTurns[TOTAL_TURNS];
int currentTurnIndex = 0;
bool historyInitialized = false;
float glowTimer = 0.0f;
bool isMuted = false;

float infiniteScore = 0.0f;
float timeElapsedMode4 = 0.0f;
int targetLoopCount = -1;       
int currentLoopCount = 0;       
float globalTimer = 0.0f;       

static float lastRecordedZ = 0.0f;
static float lastRecordedX = 0.0f;

// Implementasi Logika Mandiri

float hitungJarak2D(float x1, float z1, float x2, float z2) {
    return sqrtf((x2 - x1) * (x2 - x1) + (z2 - z1) * (z2 - z1));
}

void GenerateTurn(int turnNumber) {
    int idx = turnNumber - 1;
    gameTurns[idx].startZ = -(idx * TURN_DISTANCE);

    if (turnNumber == 1) {
        gameTurns[idx].kiriType = ORANG;          
        gameTurns[idx].tengahType = NONE;           
        gameTurns[idx].kananType = POHON_TUMBANG;  
    } else {
        ObstacleType fatalPool[] = {ORANG, HEWAN, LUBANG, REL_PUTUS, MOBIL, BOM};
        ObstacleType barrierPool[] = {POHON_TUMBANG, BATU_BESAR, GERBANG_TERTUTUP, RUMAH_SIMPLE};
        
        ObstacleType selectedFatal = fatalPool[rand() % 6];
        ObstacleType selectedBarrier = barrierPool[rand() % 4];

        int safeLane = rand() % 3;
        if (safeLane == 0) {
            gameTurns[idx].kiriType = NONE;
            gameTurns[idx].tengahType = selectedFatal;
            gameTurns[idx].kananType = selectedBarrier;
        } else if (safeLane == 1) {
            gameTurns[idx].kiriType = selectedFatal;
            gameTurns[idx].tengahType = NONE;
            gameTurns[idx].kananType = selectedBarrier;
        } else {
            gameTurns[idx].kiriType = selectedFatal;
            gameTurns[idx].tengahType = selectedBarrier;
            gameTurns[idx].kananType = NONE;
        }
    }
}

void InitProceduralTurns() {
    srand(time(NULL));
    currentTurnIndex = 0;
    for(int i = 1; i <= TOTAL_TURNS; i++) {
        GenerateTurn(i);
    }
    
    trainHistory.clear();
    for(int i = 0; i < 150; i++) {
        TrainState initialNode = { 0.0f, (i * 0.2f), 0.0f };
        trainHistory.push_back(initialNode);
    }
    historyInitialized = true;
    lastRecordedX = 0.0f;
    lastRecordedZ = 0.0f;
}

void BounceBackToTurn() {
    posZ = gameTurns[currentTurnIndex].startZ; 
    posX = 0.0f;       
    trainYaw = 0.0f;
    laneLocked = false;   
    jalurPilihan = 0;     
    trainHistory.clear(); 
    historyInitialized = true; 
    printf("KERETA TERPENTAL! Menabrak rintangan non-fatal di Turn %d.\n", currentTurnIndex + 1);
}

void GameOver() {
    gameState = 2; 
    printf("GAME OVER: Menabrak rintangan fatal!\n");
}

void CheckObstacleCollisions() {
    TurnData currentTurn = gameTurns[currentTurnIndex];
    ObstacleType hitObstacle = NONE;

    if (jalurPilihan == -1) hitObstacle = currentTurn.kiriType;
    else if (jalurPilihan == 0) hitObstacle = currentTurn.tengahType;
    else if (jalurPilihan == 1) hitObstacle = currentTurn.kananType;

    if (hitObstacle != NONE) {
        if (hitObstacle == ORANG || hitObstacle == HEWAN || hitObstacle == LUBANG || 
            hitObstacle == REL_PUTUS || hitObstacle == MOBIL || hitObstacle == BOM) {
            GameOver();
        } else {
            BounceBackToTurn();
        }
    }
}

void updateJalurDanPosisi(bool isMovingForward) {
    bool sedangMundur = (posZ > lastHistoryZ);
    if (gameState != 0) return;
    
    // Golden Experience Requiem Mode (reset loop u/ Mode 4)
    if (gameMode == 4 && posZ <= -990.0f && !sedangMundur) {
        float offsetZ = 990.0f; 
        posZ += offsetZ; 
        lastHistoryZ += offsetZ;

        for (size_t i = 0; i < trainHistory.size(); i++) {
            trainHistory[i].z += offsetZ;
        }

        jalurPilihan = 0;
        laneLocked = false;
        currentTurnIndex = 0; 
        return; 
    }

    // Traceback Mech. (MUNDUR!!)
    if (sedangMundur) {
        if (!trainHistory.empty()) {
            posX = trainHistory.front().x;
            trainYaw = trainHistory.front().yaw;
            
            int calculatedTurn = (int)(floor(fabs(posZ) / TURN_DISTANCE));
            if (calculatedTurn >= 0 && calculatedTurn < TOTAL_TURNS) {
                currentTurnIndex = calculatedTurn;
            }
            
            float localZ = fabs(posZ) - (currentTurnIndex * TURN_DISTANCE);
            if (localZ < 0.0f) localZ = 0.0f;
            
            laneLocked = (localZ >= 0.0f && localZ < 30.0f && localZ > 1.0f);
            
            if (trainHistory.size() > 1) {
                trainHistory.pop_front();
            }
        }
        lastHistoryZ = posZ;
        return;
    }

    // spine logic (MAJU!!)
    float absZ = fabs(posZ);
    int calculatedTurn = (int)(absZ / TURN_DISTANCE); 
    
    if (calculatedTurn < 0) calculatedTurn = 0;
    if (calculatedTurn >= TOTAL_TURNS) {
        gameState = 1; 
        return;
    }
    currentTurnIndex = calculatedTurn;

    float localZ = fmodf(absZ, TURN_DISTANCE); 
    if (localZ < 0.0f) localZ = 0.0f; 

    // // --- KAITAN LOGIKA TURN INDICATOR (Z: 0 s/d 30) ---
    if (localZ >= 0.0f && localZ < 30.0f) {
        float t = localZ / 30.0f;
        float smoothT = t * t * (3.0f - 2.0f * t); 
        
        if (localZ > 1.4f) laneLocked = true;
        
        posX = smoothT * (jalurPilihan * 7.0f);
        trainYaw = (-jalurPilihan * 18.0f) * (1.0f - smoothT); 
    }
    // // --- BOUNDING BOX STERIL REL (Z: 30 s/d 50) ---
    else if (localZ >= 30.0f && localZ < 50.0f) {
        posX = jalurPilihan * 7.0f; 
        trainYaw = 0.0f;            
        
        if (localZ >= 39.5f && localZ <= 40.5f) {
            CheckObstacleCollisions();
        }
    }
    else if (localZ >= 50.0f && localZ < 80.0f) {
        float t = (localZ - 50.0f) / 30.0f;
        float smoothT = t * t * (3.0f - 2.0f * t);
        
        posX = (jalurPilihan * 7.0f) * (1.0f - smoothT);
        trainYaw = (jalurPilihan * 18.0f) * (smoothT) * (1.0f - smoothT);
    }
    else {
        posX = 0.0f;
        trainYaw = 0.0f;
        jalurPilihan = 0; 
        laneLocked = false;
    }
    
    TrainState currentState = { posX, posZ, trainYaw };

    if (posZ == lastHistoryZ && diAtasTurntable && !turntableSelesai) {
        trainHistory.push_front(currentState);
        if (trainHistory.size() > 3000) trainHistory.pop_back();
    }
    else if (posZ != lastHistoryZ && posZ < lastHistoryZ) {
        trainHistory.push_front(currentState);
        if (trainHistory.size() > 3000) trainHistory.pop_back();
    }

    lastHistoryZ = posZ;
}

void updateGerbongPositions() {
    float jarakAntarGerbong = 4.8f;
    size_t delayGerbong1 = 11; 
    size_t delayGerbong2 = 24; 

    if (!trainHistory.empty() && trainHistory.size() > delayGerbong1) {
        gerbong1.x   = trainHistory[delayGerbong1].x;
        gerbong1.z   = trainHistory[delayGerbong1].z;
        gerbong1.yaw = trainHistory[delayGerbong1].yaw;
    } else {
        float rad = trainYaw * (PI / 180.0f);
        gerbong1.x   = posX - (jarakAntarGerbong * cosf(rad));
        gerbong1.z   = posZ + (jarakAntarGerbong * sinf(rad)); 
        gerbong1.yaw = trainYaw;
    }

    if (!trainHistory.empty() && trainHistory.size() > delayGerbong2) {
        gerbong2.x   = trainHistory[delayGerbong2].x;
        gerbong2.z   = trainHistory[delayGerbong2].z;
        gerbong2.yaw = trainHistory[delayGerbong2].yaw;
    } else {
        float rad = gerbong1.yaw * (PI / 180.0f);
        gerbong2.x   = gerbong1.x - (jarakAntarGerbong * cosf(rad));
        gerbong2.z   = gerbong1.z + (jarakAntarGerbong * sinf(rad));
        gerbong2.yaw = gerbong1.yaw;
    }
}

void gameTimerUpdate(int value) {
    static float lastHistoryZ = posZ;
    
    if (gameState == 0) { 
        if (autoMove == 1) {
            posZ -= autoSpeed;
            wheelAngle -= autoSpeed * 35.0f; 
            if (wheelAngle < -360.0f) wheelAngle += 360.0f;
            
            updateJalurDanPosisi(true);
            updateGerbongPositions(); 
        }
        
        float deltaZ = posZ - lastHistoryZ;
        if (fabs(deltaZ) > 0.001f) {
            wheelAngle += deltaZ * 70.0f; 
            if (wheelAngle > 360.0f)  wheelAngle -= 360.0f;
            if (wheelAngle < -360.0f) wheelAngle += 360.0f;
        }

        lastHistoryZ = posZ;

        if (gameMode == 2) {
            timeElapsed += 0.016f; 
        }
        
        if (diAtasTurntable && !turntableSelesai) {
            if (jalurPilihan == 1)      targetTurntableYaw = 15.0f;  
            else if (jalurPilihan == 2) targetTurntableYaw = -15.0f; 
            else                        targetTurntableYaw = 0.0f;   

            float selisihYaw = targetTurntableYaw - trainYaw;
            if (fabs(selisihYaw) > 0.05f) {
                trainYaw += selisihYaw * 0.15f; 
            } else {
                trainYaw = targetTurntableYaw;  
                turntableSelesai = true;        
                diAtasTurntable = false;        
            }
        }
        
        updateJalurDanPosisi(true);
        updateGerbongPositions();

        if (gameMode == 1) {
            timeRemaining -= 0.1f;
            if (timeRemaining <= 0.0f) { 
                timeRemaining = 0.0f; 
                gameState = 2; 
                autoMove = 0; 
            }
        } else if (gameMode == 2) {
            timeElapsed += 0.1f;
        }

        glutPostRedisplay();
    }
    glutTimerFunc(100, gameTimerUpdate, 0);
}
