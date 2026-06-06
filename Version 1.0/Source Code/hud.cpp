#include "hud.h"
#include "globals.h"

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

    if (gameState == -1) {
        // HUD main menu
        glEnable(GL_BLEND); 
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
        glBegin(GL_QUADS);
            glVertex2f(w/2 - 240, h/2 - 140); 
            glVertex2f(w/2 + 240, h/2 - 140);
            glVertex2f(w/2 + 240, h/2 + 120);
            glVertex2f(w/2 - 240, h/2 + 120);
        glEnd();
        glDisable(GL_BLEND);

        glColor3f(1.0f, 0.9f, 0.1f);
        renderText(w / 2 - 120, h / 2 + 80, "=== TRAIN DYNAMICS THE GAME ===");
        
        glColor3f(1.0f, 1.0f, 1.0f);
        renderText(w / 2 - 150, h / 2 + 30, "Silahkan Pilih Mode Permainan Anda:");
        
        glColor3f(0.4f, 1.0f, 1.0f);
        renderText(w / 2 - 150, h / 2 - 10, "[1] MODE COUNTDOWN (30s Time Limit)");
        
        glColor3f(0.4f, 1.0f, 0.4f);
        renderText(w / 2 - 150, h / 2 - 35, "[2] MODE SPEEDRUN (Stopwatch)");
        
        glColor3f(1.0f, 0.3f, 0.3f); 
        renderText(w / 2 - 150, h / 2 - 60, "[3] MODE ENDLESS (Infinite Loop Ride)");

        glColor3f(0.8f, 0.8f, 0.8f);
        renderText(w / 2 - 210, h / 2 - 105, "Tekan Tombol [1], [2], atau [3] untuk memulai game");
    }
    else {
        // HUD in-game
        glColor3f(1.0f, 0.9f, 0.1f);
        renderText(10, h - 18, "TRAIN DYNAMICS - Kelompok 8");

        // Info Timer per Game Mode
        if (gameMode == 1) {
            glColor3f(1.0f, 1.0f, 1.0f);
            snprintf(buf, sizeof(buf), "SISA WAKTU: %.1f DETIK", timeRemaining);
            renderText(w - 240, h - 18, buf);
        } 
        else if (gameMode == 2) {
            glColor3f(0.2f, 1.0f, 1.0f);
            snprintf(buf, sizeof(buf), "STOPWATCH: %.1f DETIK", timeElapsed);
            renderText(w - 240, h - 18, buf);
        }
        else if (gameMode == 4) {
            glColor3f(1.0f, 0.7f, 0.2f);
            snprintf(buf, sizeof(buf), "TIME ELAPSED: %.2f s", timeElapsedMode4);
            renderText(w - 240, h - 18, buf);
            
            snprintf(buf, sizeof(buf), "DISTANCE: %.1f m", infiniteScore);
            renderText(20, 65, buf);
        }
        
        // Info Navigasi & Kontrol
        glColor3f(1.0f, 0.5f, 0.5f); 
        renderText(10, 20, "[R] Reset Session | [TAB] Menu Utama"); 

        glColor3f(0.6f, 0.85f, 1.0f);
        if (cameraMode == 2) {
            renderText(10, 40, "W/S/A/D = Gerak | Q/E = Yaw | R/F = Pitch");
        } else {
            renderText(10, 40, "W/S = Gas/Rem | A/D = Pilih Jalur (Zona Turn Indicator)");
        }

        // Status Audio BGM
        if (isMuted) {
            glColor3f(1.0f, 0.0f, 0.0f);
            snprintf(buf, sizeof(buf), "MUSIC: MUTED [M]");
        } else {
            glColor3f(0.3f, 1.0f, 0.3f);
            snprintf(buf, sizeof(buf), "MUSIC: ON [M]");
        }
        renderText(w - 180, 40, buf); 

        // Telemetri Kereta
        glColor3f(1.0f, 1.0f, 1.0f); 
        snprintf(buf, sizeof(buf), "Posisi Z: %.1f", posZ);
        renderText(10, h - 40, buf);
        
        const char* modeLabel = (gameMode == 1) ? "Countdown" : ((gameMode == 2) ? "Speedrun" : "Endless Loop");
        snprintf(buf, sizeof(buf), "Mode Game: %s", modeLabel);
        renderText(10, h - 60, buf);

        // State Screen Akhir (Win / Game Over)
        if (gameState == 1) { 
            glColor3f(0.0f, 1.0f, 0.0f);
            renderText(w / 2 - 100, h / 2 + 20, "=============================");
            renderText(w / 2 - 100, h / 2,      "    FINISH! ANDA MENANG!     ");
            if (gameMode == 2) {
                snprintf(buf, sizeof(buf), "  WAKTU TEMPUH: %.1f DETIK", timeElapsed);
                renderText(w / 2 - 100, h / 2 - 15, buf);
            }
            renderText(w / 2 - 100, h / 2 - 35, "=============================");
        } 
        else if (gameState == 2) { 
            glColor3f(1.0f, 0.0f, 0.0f);
            renderText(w / 2 - 110, h / 2 + 20, "=============================");
            renderText(w / 2 - 110, h / 2,      "     GAME OVER! Press 'R'    ");
            renderText(w / 2 - 110, h / 2 - 20, "=============================");
        }
    }

    // Pulihkan OpenGL State
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glEnable(GL_DEPTH_TEST);
    if (lightingEnabled) glEnable(GL_LIGHTING);
}
