// =====================================================
// globals.cpp – Definisi variabel global & Logic Dynamics
// TRAIN DYNAMICS - Kelompok 8
// =====================================================
#include "globals.h"

float posX = 0.0f; 
float posY = 0.0f;
float posZ = 0.0f;

float wheelAngle  = 0.0f;
float trainYaw    = 0.0f;

std::deque<TrainState> trainHistory;

int   cameraMode = 1;

float camX = 6.0f, camY = 3.0f, camZ = 5.0f;
float yaw  = -30.0f, pitch = -10.0f;

bool  diAtasTurntable    = false;   
float turntableX         = 0.0f;    
float turntableZ         = -54.4f;  
float targetTurntableYaw = 0.0f;    
bool  turntableSelesai   = false;   

float smoothedCamX = 0.0f;
float smoothedCamY = 3.5f;

int projectionMode  = 1;
int lightingEnabled = 1;
int shadingMode     = 2;

float sunAngle = 90.0f;        // Mulai dari 90 derajat (Tepat di atas kepala / Siang bolong)
bool isNight = false;

int   autoMove  = 0;
float autoSpeed = 0.9f;
int   cinematic = 0;
float cinAngle  = 0.0f;


GLuint texDaun = 0;
GLuint texBricks = 0;
GLuint texRoof = 0;
GLuint texRoad = 0;
GLuint texWood = 0;
GLuint texConcrete = 0;
GLuint texRock = 0;
GLuint texSky = 0;


int   jalurPilihan = 0;
int   diAreaCabang = 0;

TreePos trees[JUMLAH_POHON];

// ==== Game Mechanics State & Finish Area ====
int historyHeadIndex = 0;
bool laneLocked   = false; // MECHANIC 3: Default tidak terkunci sebelum cabang
int   gameState    = -1; 

float startZ       = 0.0f;
float lastHistoryZ = 0.0f;
float finishMinX   = -1.5f;
float finishMaxX   = 1.5f;
float finishMinZ   = -10502.0f; 
float finishMaxZ   = -1000.0f; 

int   gameMode      = 1;    // mode game ==> gajadi, 1 gamemode aja jadinya
float timeRemaining = 30.0f; 
float timeElapsed   = 0.0f;  


// Alokasi memori global baru
TurnData gameTurns[TOTAL_TURNS];
int currentTurnIndex = 0;

// Variabel internal pembantu untuk melacak posisi rekaman terakhir lokomotif
static float lastRecordedZ = 0.0f;
static float lastRecordedX = 0.0f;
bool historyInitialized = false;

//MODE 4
float infiniteScore = 0.0f;
float timeElapsedMode4 = 0.0f;
int targetLoopCount = -1;       // Jumlah total pengulangan (di-set acak nanti)
int currentLoopCount = 0;       // Menghitung putaran yang sudah diselesaikan
float globalTimer = 0.0f;       // Timer game yang akumulatif (tidak di-reset!)

//  ===================================================== ===================================================== =====================================================

// Fungsi pembantu untuk menghitung jarak 2D antar titik
float hitungJarak2D(float x1, float z1, float x2, float z2) {
    return sqrtf((x2 - x1) * (x2 - x1) + (z2 - z1) * (z2 - z1));
}

//  ===================================================== ===================================================== =====================================================


// Integrasi Fungsi Randomizer & Inisialisasi Turn
void GenerateTurn(int turnNumber) {
    int idx = turnNumber - 1;
    gameTurns[idx].startZ = - (idx * TURN_DISTANCE);

    if (turnNumber == 1) {
        // Turn 1 Statis sesuai aturan mutlak Anda
        gameTurns[idx].kiriType   = ORANG;          // Game Over (Fatal)
        gameTurns[idx].tengahType = NONE;           // Safe Path
        gameTurns[idx].kananType  = POHON_TUMBANG;  // Barrier (Non-Fatal)
    } else {
        // Turn 2 - 10: Acak Prosedural
        ObstacleType fatalPool[] = {ORANG, HEWAN, LUBANG, REL_PUTUS, MOBIL, BOM};
        ObstacleType barrierPool[] = {POHON_TUMBANG, BATU_BESAR, GERBANG_TERTUTUP, RUMAH_SIMPLE};
        
        ObstacleType selectedFatal = fatalPool[rand() % 6];
        ObstacleType selectedBarrier = barrierPool[rand() % 4];

        // Acak penempatan jalur (0 = Kiri Safe, 1 = Tengah Safe, 2 = Kanan Safe)
        int safeLane = rand() % 3;
        if (safeLane == 0) {
            gameTurns[idx].kiriType   = NONE;
            gameTurns[idx].tengahType = selectedFatal;
            gameTurns[idx].kananType  = selectedBarrier;
        } else if (safeLane == 1) {
            gameTurns[idx].kiriType   = selectedFatal;
            gameTurns[idx].tengahType = NONE;
            gameTurns[idx].kananType  = selectedBarrier;
        } else {
            gameTurns[idx].kiriType   = selectedFatal;
            gameTurns[idx].tengahType = selectedBarrier;
            gameTurns[idx].kananType  = NONE;
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

// Mekanik Pembalikan Posisi (Terpental mundur ke awal turn terkait)
void BounceBackToTurn() {
    float turnStartZ = gameTurns[currentTurnIndex].startZ;
    posZ = turnStartZ; // Reset posisi Z ke awal Turn saat ini
    posX = 0.0f;       // Kembalikan ke jalur tengah utama
    trainYaw = 0.0f;
    laneLocked = false;   // Buka kunci rel agar pemain bisa memilih jalur lain
    jalurPilihan = 0;     // Kembalikan pilihan ke tengah default
    trainHistory.clear(); // Bersihkan history agar gerbong melakukan sinkronisasi ulang snap-teleport
    historyInitialized = true; // Reset pemicu history
    printf("KEREtA TERPENTAL! Menabrak rintangan non-fatal di Turn %d. Silakan pilih jalur lain!\n", currentTurnIndex + 1);
}

void GameOver() {
    gameState = 2; // Mengaktifkan State GAME OVER pada HUD screen
    printf("GAME OVER: Anda menabrak rintangan fatal!\n");
}

// Logika Evaluasi Colliders Axis-Aligned Bounding Box
void CheckObstacleCollisions() {
    TurnData currentTurn = gameTurns[currentTurnIndex];
    ObstacleType hitObstacle = NONE;

    if (jalurPilihan == -1) hitObstacle = currentTurn.kiriType;
    else if (jalurPilihan == 0) hitObstacle = currentTurn.tengahType;
    else if (jalurPilihan == 1) hitObstacle = currentTurn.kananType;

    if (hitObstacle != NONE) {
        // Klasifikasi Efek Rintangan
        if (hitObstacle == ORANG || hitObstacle == HEWAN || hitObstacle == LUBANG || 
            hitObstacle == REL_PUTUS || hitObstacle == MOBIL || hitObstacle == BOM) {
            GameOver();
        } else {
            BounceBackToTurn();
        }
    }
}

//  ===================================================== ===================================================== =====================================================



// Update Pergerakan Jalur & Deteksi Titik Koordinat Berulang (10 Turns Spline Logic)
void updateJalurDanPosisi(bool isMovingForward) {
    
    bool sedangMundur = (posZ > lastHistoryZ);
    
    if (gameState != 0) return;
    
    // ===================================================================
    // LOGIKA KHUSUS GAME MODE 4: INFINITE TURNS MECHANIC (SOFT RESET)
    // ===================================================================
    if (gameMode == 4 && posZ <= -990.0f && !sedangMundur) {
        
        // 1. Hitung selisih teleportasi (Kita geser balik ke area Z negatif awal, misal Z = +15.0f)
        float offsetZ = 990.0f; 
        posZ += offsetZ; // Pindahkan posisi riil lokomotif kembali ke depan
        lastHistoryZ += offsetZ;

        // 2. CRITICAL STEP: Geser seluruh isi koordinat sejarah (history) gerbong penumpang!
        // Jika data history tidak ikut digeser, gerbong akan mengalami patah/putus visual yang ekstrem.
        for (size_t i = 0; i < trainHistory.size(); i++) {
            trainHistory[i].z += offsetZ;
        }

        // 3. Reset status sirkuit percabangan agar turn baru bisa dimunculkan kembali
        jalurPilihan = 0;
        laneLocked = false;
        currentTurnIndex = 0; // Kembalikan indeks kalkulasi internal ke 0
        
        printf("Infinite Teleport Sukses: Rangkaian kereta ditransisikan ke Turn berikutnya.\n");
        return; // Keluar dari frame ini untuk mencegah pembacaan data di luar jangkauan
    }

    // ===================================================================
    // KONDISI A: JIKA KERETA BERGERAK MUNDUR (TRACEBACK MECHANIC)
    // ===================================================================
    if (sedangMundur) {
        if (!trainHistory.empty()) {
            posX = trainHistory.front().x;
            trainYaw = trainHistory.front().yaw;
            
            int calculatedTurn = (int)(floor(fabs(posZ) / TURN_DISTANCE));
            if (calculatedTurn >= 0 && calculatedTurn < TOTAL_TURNS) {
                currentTurnIndex = calculatedTurn;
            }
            
            float localZ = fabs(posZ) - (currentTurnIndex * TURN_DISTANCE);
            if (localZ < 0.0f) localZ = 0.0f; // Proteksi nilai negatif saat mundur
            
            if (localZ >= 0.0f && localZ < 30.0f && localZ > 1.0f) {
                laneLocked = true;
            } else {
                laneLocked = false;
            }
            
            if (trainHistory.size() > 1) {
                trainHistory.pop_front();
            }
        }
        
        lastHistoryZ = posZ;
        return;
    }

    // ===================================================================
    // KONDISI B: JIKA KERETA BERGERAK MAJU / DIAM (LOGIKA SPLINE ASLI)
    // ===================================================================
    
    // PERBAIKAN 1: Gunakan casting fmod atau proteksi pembulatan yang ketat
    float absZ = fabs(posZ);
    int calculatedTurn = (int)(absZ / TURN_DISTANCE); // Pembagian integer otomatis memotong desimal (truncation)
    
    if (calculatedTurn < 0) calculatedTurn = 0;
    if (calculatedTurn >= TOTAL_TURNS) {
        gameState = 1; 
        return;
    }
    currentTurnIndex = calculatedTurn;

    // PERBAIKAN 2: Hitung localZ menggunakan rumus fmodf agar dijamin aman dari hasil negatif
    float localZ = fmodf(absZ, TURN_DISTANCE); 
    if (localZ < 0.0f) localZ = 0.0f; // Garansi mutlak localZ tidak pernah minus
    
    // KODE DEBUG --> not usable unless system pause
    if (currentTurnIndex == 5) {
        printf("TURN 6 | posZ: %.2f | localZ: %.2f | jalurPilihan: %d | laneLocked: %s\n", 
               posZ, localZ, jalurPilihan, laneLocked ? "YA" : "TIDAK");
    }

    // --- PHASE 1: SPLIT PATH ZONE (0 s/d 30 unit lokal) ---
    if (localZ >= 0.0f && localZ < 30.0f) {
        float t = localZ / 30.0f;
        float smoothT = t * t * (3.0f - 2.0f * t); 
        
        if (localZ > 1.4f) {
            laneLocked = true;
        }
        
        posX = smoothT * (jalurPilihan * 7.0f);
        trainYaw = (-jalurPilihan * 18.0f) * (1.0f - smoothT); 
    }
    // --- PHASE 2: STRAIGHT OBSTACLE ZONE (30 s/d 50 unit lokal) ---
    else if (localZ >= 30.0f && localZ < 50.0f) {
        posX = jalurPilihan * 7.0f; 
        trainYaw = 0.0f;            
        
        if (localZ >= 39.5f && localZ <= 40.5f) {
            CheckObstacleCollisions();
        }
    }
    // --- PHASE 3: MERGE PATH ZONE (50 s/d 80 unit lokal) ---
    else if (localZ >= 50.0f && localZ < 80.0f) {
        float t = (localZ - 50.0f) / 30.0f;
        float smoothT = t * t * (3.0f - 2.0f * t);
        
        posX = (jalurPilihan * 7.0f) * (1.0f - smoothT);
        trainYaw = (jalurPilihan * 18.0f) * (smoothT) * (1.0f - smoothT);
    }
    // --- PHASE 4: SINGLE TRACK STANDARD (80 s/d 100 unit lokal) ---
    else {
        posX = 0.0f;
        trainYaw = 0.0f;
        jalurPilihan = 0; 
        laneLocked = false;
    }
    
    // Simpan keadaan koordinat terbaru ke struct
    TrainState currentState;
    currentState.x = posX;
    currentState.z = posZ;
    currentState.yaw = trainYaw;

    // Skenario A: Kereta sedang berputar di Turntable (posZ diam, tapi yaw berubah)
    if (posZ == lastHistoryZ && diAtasTurntable && !turntableSelesai) {
        trainHistory.push_front(currentState);
        if (trainHistory.size() > 3000) { // Ukuran ditinggikan untuk menghandle peta panjang
            trainHistory.pop_back();
        }
    }
    // Skenario B: Kereta sedang bergerak Maju Murni
    else if (posZ != lastHistoryZ) {
        if (posZ < lastHistoryZ) {
            // JIKA MAJU: Push ke depan antrean (tali baru di depan)
            trainHistory.push_front(currentState);
            if (trainHistory.size() > 3000) {
                trainHistory.pop_back();
            }
        }
    }

    // Kunci posisi Z frame ini untuk deteksi di frame berikutnya
    lastHistoryZ = posZ;
}

void updateGerbongPositions() {
    // 1. PENGATURAN JARAK (Ubah angka ini untuk memperkecil/memperbesar jarak)
    // Berdasarkan skala lokomotif Anda (panjang ~4.2), jarak ideal berkisar antara 4.5f sampai 5.2f.
    // Silakan ganti ke 4.6f atau 4.5f jika ingin gerbong lebih rapat (menempel dekat buffer).
    float jarakAntarGerbong = 4.8f;

    // 2. PENYESUAIAN DELAY ANTREAN (Untuk saat kereta sudah berjalan)
    // Jika jarak diperkecil, indeks delay history juga harus diperkecil agar sinkron!
    // Hubungan empiris: delay = jarakAntarGerbong * 2.5 (asumsi kecepatan autoSpeed 0.9f)
    size_t delayGerbong1 = 11; // Jarak indeks dari Lokomotif ke Gerbong 1
    size_t delayGerbong2 = 24; // Jarak indeks dari Lokomotif ke Gerbong 2

    // --- LOGIKA GERBONG 1 ---
    if (!trainHistory.empty() && trainHistory.size() > delayGerbong1) {
        gerbong1.x   = trainHistory[delayGerbong1].x;
        gerbong1.z   = trainHistory[delayGerbong1].z;
        gerbong1.yaw = trainHistory[delayGerbong1].yaw;
    } else {
        // PERBAIKAN INISIALISASI (Arah Belakang Kereta)
        // Karena lokal +X adalah MAJU, maka MUNDUR ke belakang adalah arah -X lokal.
        // Di sistem koordinat Anda (rotasi 90 derajat):
        // Delta X dunia = -jarak * cos(yaw), Delta Z dunia = +jarak * sin(yaw)
        float rad = trainYaw * (3.14159265f / 180.0f);
        gerbong1.x   = posX - (jarakAntarGerbong * cosf(rad));
        gerbong1.z   = posZ + (jarakAntarGerbong * sinf(rad)); 
        gerbong1.yaw = trainYaw;
    }

    // --- LOGIKA GERBONG 2 ---
    if (!trainHistory.empty() && trainHistory.size() > delayGerbong2) {
        gerbong2.x   = trainHistory[delayGerbong2].x;
        gerbong2.z   = trainHistory[delayGerbong2].z;
        gerbong2.yaw = trainHistory[delayGerbong2].yaw;
    } else {
        // Tepat di belakang gerbong 1 saat inisialisasi
        float rad = gerbong1.yaw * (3.14159265f / 180.0f);
        gerbong2.x   = gerbong1.x - (jarakAntarGerbong * cosf(rad));
        gerbong2.z   = gerbong1.z + (jarakAntarGerbong * sinf(rad));
        gerbong2.yaw = gerbong1.yaw;
    }
}

void gameTimerUpdate(int value) {
	static float lastHistoryZ = posZ;
	
    if (gameState == 0) { // Jika game aktif
        
        // 1. Pergerakan Otomatis Kereta (Maju ke Z Negatif)
        if (autoMove == 1) {
            posZ -= autoSpeed;
            wheelAngle -= autoSpeed * 35.0f; 
            if (wheelAngle < -360.0f) wheelAngle += 360.0f;
            
            updateJalurDanPosisi(true);
            updateGerbongPositions(); // FIX: Pastikan posisi gerbong terus diperbarui di dalam loop timer
        }
        
        // 2. FIX: Sinkronisasi roda otomatis berdasarkan pergerakan manual (Maju/Mundur)
        // Hitung selisih jarak pergerakan frame
        float deltaZ = posZ - lastHistoryZ;
        if (fabs(deltaZ) > 0.001f) {
            // Jika deltaZ negatif artinya kereta bergerak MAJU (menuju z-negatif)
            // Jika deltaZ positif artinya kereta bergerak MUNDUR (menuju z-positif)
            // Nilai putaran roda dikalikan proporsional terhadap magnitudo delta pergeseran Z
            wheelAngle += deltaZ * 70.0f; // FIX: Dinamika arah otomatis (Maju otomatis minus, Mundur otomatis plus)
            
            // Lakukan wrap-around sudut derajat agar tidak terkena floating point overflow
            if (wheelAngle > 360.0f)  wheelAngle -= 360.0f;
            if (wheelAngle < -360.0f) wheelAngle += 360.0f;
        }

        // Simpan posisi saat ini untuk komparasi frame berikutnya
        lastHistoryZ = posZ;

        // 3. Update real-time clock bawan program utama Anda
        if (gameMode == 2) {
            timeElapsed += 0.016f; // Tambah durasi counter waktu nyata (asumsi 60 FPS loop)
        }
        
        // 2. ANIMASI ROTASI TURNTABLE + KERETA (SAAT BERHENTI DI POROS)
        if (diAtasTurntable && !turntableSelesai) {
            // Tentukan target sudut berdasarkan jalur pilihan
            if (jalurPilihan == 1)      targetTurntableYaw = 15.0f;  // Belok Kiri (Positif)
            else if (jalurPilihan == 2) targetTurntableYaw = -15.0f; // Belok Kanan (Negatif)
            else                        targetTurntableYaw = 0.0f;   // Lurus

            // Kejar target sudut secara halus (Interpolasi)
            float selisihYaw = targetTurntableYaw - trainYaw;
            if (fabs(selisihYaw) > 0.05f) {
                trainYaw += selisihYaw * 0.15f; // Kereta ikut berputar bersama piringan
            } else {
                trainYaw = targetTurntableYaw;  // Kunci sudut presisi sejajar rel baru
                turntableSelesai = true;        // Tandai rotasi selesai
                diAtasTurntable = false;        // Lepas kunci poros, kereta boleh jalan
            }
        }
        
        // 3A. Hitung posisi lateral (X) dan transisi zona rel
        updateJalurDanPosisi(true);
        
        // 3B. PANGGIL DI SINI: Update Posisi Gerbong Berdasarkan History Terbaru
        updateGerbongPositions();

        // 4. Sistem Pengurangan Waktu/Timer Game
        if (gameMode == 1) {
            timeRemaining -= 0.1f;
            if (timeRemaining <= 0.0f) { timeRemaining = 0.0f; gameState = 2; autoMove = 0; }
        } else if (gameMode == 2) {
            timeElapsed += 0.1f;
        }

        glutPostRedisplay();
    }
    glutTimerFunc(100, gameTimerUpdate, 0);
}

