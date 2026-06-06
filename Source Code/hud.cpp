// =====================================================
// hud.cpp  –  Head-Up Display (overlay teks 2D)
// =====================================================
#include "hud.h"
#include "globals.h"

// Render string bitmap di koordinat 2D (x, y) dalam pixel.
static void renderText(float x, float y, const char* str) {
    glRasterPos2f(x, y);
    while (*str) glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *str++);
}

void drawHUD() {
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    char buf[256];
    char bufferBGM[64];

    // =========================================================
    // -- TAMPILAN JIKA BERADA DI MENU UTAMA (gameState == -1) --
    // =========================================================
    if (gameState == -1) {
        // -- LAYAR MENU UTAMA --
        glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
        glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBegin(GL_QUADS);
            glVertex2f(w/2 - 240, h/2 - 140);   // Diperlebar sedikit ke bawah
            glVertex2f(w/2 + 240, h/2 - 140);
            glVertex2f(w/2 + 240, h/2 + 120);
            glVertex2f(w/2 - 240, h/2 + 120);
        glEnd();
        glDisable(GL_BLEND);

        glColor3f(1.0f, 0.9f, 0.1f);
        renderText(w / 2 - 120, h / 2 + 80, "=== TRAIN CHOICES GAME MENU ===");
        
        glColor3f(1.0f, 1.0f, 1.0f);
        renderText(w / 2 - 150, h / 2 + 30, "Silahkan Pilih Mode Permainan Anda:");
        
        glColor3f(0.4f, 1.0f, 1.0f);
        renderText(w / 2 - 150, h / 2 - 10, "[1] MODE COUNTDOWN (30s Time Limit)");
        
        glColor3f(0.4f, 1.0f, 0.4f);
        renderText(w / 2 - 150, h / 2 - 35, "[2] MODE SPEEDRUN (Stopwatch)");
        
        glColor3f(1.0f, 0.0f, 0.0f); // Teks Merah
        renderText(w / 2 - 150, h / 2 - 60, "[3] MODE SISYPHUS (RUN. ARRIVED. LOOP.)");

        glColor3f(0.8f, 0.8f, 0.8f);
        renderText(w / 2 - 210, h / 2 - 105, "Tekan Tombol [1] atau [2] atau [3] untuk memulai game");
    }
    else {
        // -- LAYAR SAAT BERMAIN / SELESAI --
        glColor3f(1.0f, 0.9f, 0.1f);
        renderText(10, h - 18, "THREE MUST KE TRAIN - Kelompok 8");

        // Cetak Timer sesuai Mode aktif
        if (gameMode == 1) {
            glColor3f(1.0f, 1.0f, 1.0f);
            snprintf(buf, sizeof(buf), "SISA WAKTU: %.1f DETIK", timeRemaining);
            renderText(w - 240, h - 18, buf);
        } 
        else if (gameMode == 2) {
            glColor3f(0.2f, 1.0f, 1.0f);
            snprintf(buf, sizeof(buf), "CLOCK: %.1f DETIK", timeElapsed);
            renderText(w - 240, h - 18, buf);
        }
        else if (gameMode == 3) {
            // Mode 3 HUD
            if (timeRemaining <= 5.0f) glColor3f(1.0f, 0.3f, 0.3f); // Merah jika aksi menipis
            else                       glColor3f(1.0f, 0.7f, 0.2f);
            snprintf(buf, sizeof(buf), "SISA AKSI (WAKTU): %.1f DETIK", timeRemaining);
            renderText(w - 280, h - 18, buf);
        }
        
        glColor3f(1.0f, 0.5f, 0.5f); // Warna soft red/orange agar terlihat sebagai opsi navigasi
        renderText(10, 20, "[R] Untuk Reset Session"); 

        // Info kontrol pergerakan bawaan (atur posisi Y-nya sedikit ke atas agar tidak bertumpuk)
        glColor3f(0.6f, 0.85f, 1.0f);
        if (cameraMode == 2)
            renderText(10, 40, "W/S/A/D=Gerak | Q/E=Yaw | r/f=Naik/Turun | R/F=Pitch");
        else
            renderText(10, 40, "W/S=Jalankan Kereta | A/D=Belokan Kereta (Zona Hijau) | 1/2/3/4=Ganti Kamera");
		

        // PANEL NOTIFIKASI AKHIR GAME
        if (gameState == 1) { 
            glColor3f(0.0f, 1.0f, 0.0f);
            renderText(w / 2 - 100, h / 2 + 20, "=============================");
            renderText(w / 2 - 100, h / 2,      "    FINISH! ANDA MENANG!     ");
            if (gameMode == 2) {
                snprintf(buf, sizeof(buf), "  WAKTU TEMPUH: %.1f DETIK", timeElapsed);
                renderText(w / 2 - 100, h / 2 - 15, buf);
            }
            renderText(w / 2 - 100, h / 2 - 35, "=============================");
            
            glColor3f(1.0f, 1.0f, 1.0f);
            renderText(w / 2 - 115, h / 2 - 65, "Tekan 'R' untuk Reset || Tekan TAB untuk Kembali ke Menu");
        } 
        else if (gameState == 2) { 
            glColor3f(1.0f, 0.0f, 0.0f);
            renderText(w / 2 - 110, h / 2 + 20, "=============================");
            renderText(w / 2 - 110, h / 2,      "   GAME OVER!   ");
            renderText(w / 2 - 110, h / 2 - 20, "=============================");
            
            glColor3f(1.0f, 1.0f, 1.0f);
            renderText(w / 2 - 115, h / 2 - 50, "Tekan 'R' untuk Reset || Tekan TAB untuk Kembali ke Menu");
        }
        
        if (gameMode == 4) {
        glColor3f(1.0f, 0.0f, 0.0f); // Teks Merah

        // 1. Tampilkan Judul Mode
        sprintf(bufferBGM, "MODE: INFINITE SISYPHHUS / MUNGEN SUKOYOMI (ENDLESS RIDE)");
        renderText(20, 115, bufferBGM); // Fungsi helper render bitmap string Anda

		glColor3f(1.0f, 1.0f, 1.0f); // Teks Putih
        // 2. Tampilkan Total Jarak Akumulatif
        sprintf(bufferBGM, "Distance Traveled: %.1f m", infiniteScore);
        renderText(20, 65, bufferBGM);

        // 3. Tampilkan Waktu Bertahan (Time Elapsed)
        sprintf(bufferBGM, "Time Elapsed: %.2f s", timeElapsedMode4);
        renderText(20, 90, bufferBGM);
    }

    // ===================================================================
    // INDIKATOR VISUAL AUDIO GLOBAL (DI SUDUT KANAN ATAS LAYAR)
    // ===================================================================
    if (isMuted) {
        glColor3f(1.0f, 0.3f, 0.3f); // Merah peringatan jika Muted
        sprintf(bufferBGM, "MUSIC: MUTED [M]");
    } else {
        glColor3f(0.3f, 1.0f, 0.3f); // Hijau segar jika Active
        sprintf(bufferBGM, "MUSIC: ON [M]");
    }
    // Asumsi lebar layar jendela game standar Anda adalah sekitar 800px
    renderText(640, 40, bufferBGM); 

        // Cetak telemetry kereta (Info posisi Z bawaan Anda)
        glColor3f(1.0f, 1.0f, 1.0f); // Teks Putih
        snprintf(buf, sizeof(buf), "Posisi Kereta Z: %.1f", posZ);
        renderText(10, h - 40, buf);
        
        // Cetak informasi Mode aktif di HUD samping bawah
        glColor3f(1.0f, 1.0f, 1.0f); // Teks Putih
        const char* modeLabel = (gameMode == 1) ? "Countdown" : ((gameMode == 2) ? "Time Attack" : "Action Cost");
        snprintf(buf, sizeof(buf), "Mode Aktif: %s", modeLabel);
        renderText(10, h - 60, buf);
        
        // Kembalikan status lighting untuk frame selanjutnya
    glEnable(GL_LIGHTING);
    }

    // Pulihkan State OpenGL
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glEnable(GL_DEPTH_TEST);
    if (lightingEnabled) glEnable(GL_LIGHTING);
}
