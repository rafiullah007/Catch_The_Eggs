/*
 * Pacman Game
 * Computer Graphics Lab Final Project
 *
 * Authors : Group Project
 * Course  : Computer Graphics Lab
 *
 * Controls:
 *   Arrow Keys  –  Move Pacman
 *   P           –  Pause / Resume
 *   R           –  Restart
 *   ESC / Q     –  Quit
 *
 * Build:
 *   make
 * Run:
 *   ./pacman
 */

#include <GL/glut.h>
#include <cmath>
#include <cstring>
#include <string>
#include <climits>
#include <algorithm>
#include <random>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* =====================================================================
 * Constants
 * ===================================================================== */
static const int WIN_W    = 560;   /* window width  (28 cols × 20 px)     */
static const int WIN_H    = 680;   /* window height (31 rows × 20 + 60)   */
static const int CELL     = 20;    /* pixels per maze cell                 */
static const int ROWS     = 31;
static const int COLS     = 28;
static const int MAZE_BOT = 60;    /* y-pixel of maze bottom edge          */

/* Maze cell values */
static const int V_DOT   = 0;
static const int V_WALL  = 1;
static const int V_POWER = 2;
static const int V_EMPTY = 3;
static const int V_DOOR  = 4;

/* Timing constants (frames at ~60 fps) */
static const int POWER_DURATION      = 360; /* power-pellet effect lasts ~6 s  */
static const int FRIGHT_FLASH_FRAMES = 60;  /* start flashing when timer < this */

/* Directions: RIGHT=0, DOWN=1, LEFT=2, UP=3, NONE=-1 */
static const int D_RIGHT = 0;
static const int D_DOWN  = 1;
static const int D_LEFT  = 2;
static const int D_UP    = 3;
static const int D_NONE  = -1;

/* Cell deltas in (col, row) maze space for each direction */
static const int DCOL[4] = { 1,  0, -1,  0 };
static const int DROW[4] = { 0,  1,  0, -1 };

/* Pixel deltas (right=+x, down=-y because OpenGL y grows upward) */
static const float DPX[4] = { 1.f,  0.f, -1.f,  0.f };
static const float DPY[4] = { 0.f, -1.f,  0.f,  1.f };

/* =====================================================================
 * Maze template
 * 0=dot  1=wall  2=power pellet  3=empty  4=ghost door
 * ===================================================================== */
static const int MAZE_TEMPL[ROWS][COLS] = {
  /* 0  */ {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  /* 1  */ {1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
  /* 2  */ {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
  /* 3  */ {1,2,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,2,1},
  /* 4  */ {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
  /* 5  */ {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  /* 6  */ {1,0,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,0,1},
  /* 7  */ {1,0,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,0,1},
  /* 8  */ {1,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,1},
  /* 9  */ {1,1,1,1,1,1,0,1,1,1,1,1,3,1,1,3,1,1,1,1,1,0,1,1,1,1,1,1},
  /* 10 */ {1,1,1,1,1,1,0,1,1,1,1,1,3,1,1,3,1,1,1,1,1,0,1,1,1,1,1,1},
  /* 11 */ {1,1,1,1,1,1,0,1,1,3,3,3,3,3,3,3,3,3,3,1,1,0,1,1,1,1,1,1},
  /* 12 */ {1,1,1,1,1,1,0,1,1,3,1,1,1,4,4,1,1,1,3,1,1,0,1,1,1,1,1,1},
  /* 13 */ {1,1,1,1,1,1,0,1,1,3,1,3,3,3,3,3,3,1,3,1,1,0,1,1,1,1,1,1},
  /* 14 */ {3,3,3,3,3,3,3,3,3,3,1,3,3,3,3,3,3,1,3,3,3,3,3,3,3,3,3,3},
  /* 15 */ {1,1,1,1,1,1,0,1,1,3,1,3,3,3,3,3,3,1,3,1,1,0,1,1,1,1,1,1},
  /* 16 */ {1,1,1,1,1,1,0,1,1,3,1,1,1,1,1,1,1,1,3,1,1,0,1,1,1,1,1,1},
  /* 17 */ {1,1,1,1,1,1,0,1,1,3,3,3,3,3,3,3,3,3,3,1,1,0,1,1,1,1,1,1},
  /* 18 */ {1,1,1,1,1,1,0,1,1,3,1,1,1,1,1,1,1,1,3,1,1,0,1,1,1,1,1,1},
  /* 19 */ {1,1,1,1,1,1,0,1,1,3,1,1,1,1,1,1,1,1,3,1,1,0,1,1,1,1,1,1},
  /* 20 */ {1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
  /* 21 */ {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
  /* 22 */ {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
  /* 23 */ {1,2,0,0,1,1,0,0,0,0,0,0,0,3,3,0,0,0,0,0,0,0,1,1,0,0,2,1},
  /* 24 */ {1,1,1,0,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,0,1,1,1},
  /* 25 */ {1,1,1,0,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,0,1,1,1},
  /* 26 */ {1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
  /* 27 */ {1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1},
  /* 28 */ {1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1},
  /* 29 */ {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  /* 30 */ {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

static int maze[ROWS][COLS];   /* working copy, dots removed as eaten */

/* =====================================================================
 * Game-state enum
 * ===================================================================== */
enum GameState {
    ST_INTRO,
    ST_READY,   /* brief "READY!" pause before play starts */
    ST_PLAY,
    ST_DYING,   /* death animation */
    ST_PAUSE,
    ST_WIN,
    ST_OVER
};
static GameState gState = ST_INTRO;
static int       gReady = 0;   /* countdown frames for READY! */

/* =====================================================================
 * Ghost mode enum
 * ===================================================================== */
enum GhostMode { GM_HOUSE, GM_EXIT, GM_CHASE, GM_SCATTER, GM_FRIGHT, GM_EATEN };

/* =====================================================================
 * Pacman
 * ===================================================================== */
struct Pacman {
    float x, y;         /* pixel center                    */
    float speed;        /* pixels per frame                */
    int   dir;          /* current movement direction       */
    int   nextDir;      /* queued direction from player     */
    float mouth;        /* mouth angle 0..44 degrees        */
    bool  mouthOpen;    /* animation toggle                 */
    int   lives;
    int   score;
    int   dieTimer;     /* frames remaining in die anim    */
};
static Pacman pac;

/* =====================================================================
 * Ghost
 * ===================================================================== */
struct Ghost {
    float     x, y;
    float     speed;
    int       dir;
    GhostMode mode;
    int       modeTimer;    /* frames until next mode change   */
    int       houseTimer;   /* frames until ghost exits house  */
    float     cr, cg, cb;  /* normal colour                   */
    int       scCol, scRow; /* scatter-target corner           */
};
static Ghost ghosts[4];

/* =====================================================================
 * Global counters / flags
 * ===================================================================== */
static int  totalDots   = 0;
static int  dotsEaten   = 0;
static bool powerActive = false;
static int  powerTimer  = 0;
static int  eatCombo    = 0;    /* consecutive ghosts eaten this power */
static int  frame       = 0;   /* global frame counter                 */

/* =====================================================================
 * Coordinate helpers
 * ===================================================================== */

/* pixel centre of maze cell (col, row) */
static void cellCentre(int col, int row, float &cx, float &cy)
{
    cx = col * CELL + CELL * 0.5f;
    cy = MAZE_BOT + (ROWS - 1 - row) * CELL + CELL * 0.5f;
}

/* maze cell that contains pixel (x, y) */
static void pixelCell(float x, float y, int &col, int &row)
{
    col = (int)(x / CELL);
    row = ROWS - 1 - (int)((y - MAZE_BOT) / CELL);
    if (col < 0)     col = 0;
    if (col >= COLS) col = COLS - 1;
    if (row < 0)     row = 0;
    if (row >= ROWS) row = ROWS - 1;
}

/* =====================================================================
 * Passability checks
 * ===================================================================== */
static bool pacCanEnter(int col, int row)
{
    /* tunnel wrap handled by caller */
    if (col < 0 || col >= COLS || row < 0 || row >= ROWS) return false;
    int v = maze[row][col];
    return v != V_WALL && v != V_DOOR;
}

static bool ghostCanEnter(int col, int row, GhostMode mode)
{
    if (col < 0 || col >= COLS || row < 0 || row >= ROWS) return false;
    int v = maze[row][col];
    if (v == V_WALL) return false;
    /* only allow through door when exiting/entering house */
    if (v == V_DOOR && mode != GM_EXIT && mode != GM_EATEN) return false;
    return true;
}

/* =====================================================================
 * Drawing helpers
 * ===================================================================== */
static void drawFilledCircle(float cx, float cy, float r, int segs = 32)
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segs; ++i) {
        float a = (float)i / segs * 2.f * (float)M_PI;
        glVertex2f(cx + r * cosf(a), cy + r * sinf(a));
    }
    glEnd();
}

/* Pacman body: filled arc (pie slice) */
static void drawPacmanBody(float cx, float cy, float r, float mouthDeg, int dir)
{
    /* rotate according to movement direction */
    float base = 0.f;
    switch (dir) {
        case D_RIGHT: base =   0.f; break;
        case D_DOWN:  base = 270.f; break;
        case D_LEFT:  base = 180.f; break;
        case D_UP:    base =  90.f; break;
        default:      base =   0.f; break;
    }
    float startA = base + mouthDeg;
    float endA   = base + 360.f - mouthDeg;

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    int segs = 36;
    for (int i = 0; i <= segs; ++i) {
        float a = startA + (float)i / segs * (endA - startA);
        a = a * (float)M_PI / 180.f;
        glVertex2f(cx + r * cosf(a), cy + r * sinf(a));
    }
    glEnd();
}

/* Ghost body: dome + "skirt" waves + eyes */
static void drawGhost(const Ghost &g)
{
    float cx = g.x, cy = g.y;
    float r  = CELL * 0.45f;

    float dr, dg, db;
    if (g.mode == GM_FRIGHT) {
        if (powerTimer > FRIGHT_FLASH_FRAMES && (frame / 10) % 2 == 0) {
            dr = 1.f; dg = 1.f; db = 1.f;   /* flashing white */
        } else {
            dr = 0.2f; dg = 0.2f; db = 0.9f; /* blue */
        }
    } else if (g.mode == GM_EATEN) {
        dr = 0.5f; dg = 0.5f; db = 0.5f;    /* grey eyes/shell only */
    } else {
        dr = g.cr; dg = g.cg; db = g.cb;
    }

    if (g.mode != GM_EATEN) {
        glColor3f(dr, dg, db);

        /* dome (upper half circle) */
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy);
        int segs = 20;
        for (int i = 0; i <= segs; ++i) {
            float a = (float)i / segs * (float)M_PI;
            glVertex2f(cx + r * cosf(a), cy + r * sinf(a));
        }
        glEnd();

        /* rectangular body below dome */
        glBegin(GL_QUADS);
        glVertex2f(cx - r, cy);
        glVertex2f(cx + r, cy);
        glVertex2f(cx + r, cy - r);
        glVertex2f(cx - r, cy - r);
        glEnd();

        /* wavy bottom: 4 bumps */
        int   n     = 4;
        float bumpR = r / n;
        glColor3f(0, 0, 0);   /* bg colour for cut-out effect */
        for (int i = 0; i < n; ++i) {
            float bx = cx - r + bumpR + i * 2.f * bumpR;
            float by = cy - r;
            drawFilledCircle(bx, by, bumpR);
        }
        glColor3f(dr, dg, db);
    }

    /* Eyes (always drawn unless eaten mode which just draws outlines) */
    float eyeOffX = r * 0.35f;
    float eyeOffY = r * 0.35f;

    /* whites */
    glColor3f(1, 1, 1);
    drawFilledCircle(cx - eyeOffX, cy + eyeOffY, r * 0.22f);
    drawFilledCircle(cx + eyeOffX, cy + eyeOffY, r * 0.22f);

    /* pupils */
    float pupilX = 0.f, pupilY = 0.f;
    switch (g.dir) {
        case D_RIGHT: pupilX =  r * 0.10f; break;
        case D_LEFT:  pupilX = -r * 0.10f; break;
        case D_UP:    pupilY =  r * 0.10f; break;
        case D_DOWN:  pupilY = -r * 0.10f; break;
        default: break;
    }
    glColor3f(0.1f, 0.1f, 0.9f);
    drawFilledCircle(cx - eyeOffX + pupilX, cy + eyeOffY + pupilY, r * 0.12f);
    drawFilledCircle(cx + eyeOffX + pupilX, cy + eyeOffY + pupilY, r * 0.12f);
}

/* Draw a string at (x,y) using GLUT bitmap font */
static void drawText(float x, float y, const std::string &s,
                     void *font = GLUT_BITMAP_HELVETICA_18)
{
    glRasterPos2f(x, y);
    for (char c : s)
        glutBitmapCharacter(font, c);
}

/* =====================================================================
 * Maze rendering
 * ===================================================================== */
static void drawMaze()
{
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            float left  = c * CELL;
            float bot   = MAZE_BOT + (ROWS - 1 - r) * CELL;
            float right = left + CELL;
            float top   = bot  + CELL;

            int v = maze[r][c];

            if (v == V_WALL) {
                glColor3f(0.f, 0.f, 0.8f);
                glBegin(GL_QUADS);
                glVertex2f(left,  bot);
                glVertex2f(right, bot);
                glVertex2f(right, top);
                glVertex2f(left,  top);
                glEnd();

                /* subtle inner bevel */
                glColor3f(0.f, 0.2f, 1.f);
                glLineWidth(1.f);
                glBegin(GL_LINE_LOOP);
                glVertex2f(left  + 1, bot  + 1);
                glVertex2f(right - 1, bot  + 1);
                glVertex2f(right - 1, top  - 1);
                glVertex2f(left  + 1, top  - 1);
                glEnd();
            } else if (v == V_DOOR) {
                /* ghost house door – thin pink line */
                glColor3f(1.f, 0.6f, 0.8f);
                glLineWidth(3.f);
                glBegin(GL_LINES);
                glVertex2f(left,  (bot + top) * 0.5f);
                glVertex2f(right, (bot + top) * 0.5f);
                glEnd();
                glLineWidth(1.f);
            } else if (v == V_DOT) {
                float cx = left + CELL * 0.5f;
                float cy = bot  + CELL * 0.5f;
                glColor3f(1.f, 0.85f, 0.65f);
                drawFilledCircle(cx, cy, 2.f);
            } else if (v == V_POWER) {
                float cx = left + CELL * 0.5f;
                float cy = bot  + CELL * 0.5f;
                /* pulse */
                float pulse = 1.f + 0.3f * sinf(frame * 0.15f);
                glColor3f(1.f, 0.85f, 0.65f);
                drawFilledCircle(cx, cy, 5.5f * pulse);
            }
        }
    }
}

/* =====================================================================
 * HUD (score / lives / labels)
 * ===================================================================== */
static void drawHUD()
{
    /* Score */
    glColor3f(1, 1, 1);
    drawText(10, 36, "SCORE:", GLUT_BITMAP_HELVETICA_12);
    drawText(58, 36, std::to_string(pac.score), GLUT_BITMAP_HELVETICA_12);

    /* High-score placeholder */
    drawText(200, 36, "HIGH SCORE", GLUT_BITMAP_HELVETICA_12);

    /* Lives */
    drawText(400, 36, "LIVES:", GLUT_BITMAP_HELVETICA_12);
    for (int i = 0; i < pac.lives; ++i) {
        float lx = 450.f + i * 22.f;
        float ly = 40.f;
        glColor3f(1.f, 1.f, 0.f);
        drawPacmanBody(lx, ly, 8.f, 20.f, D_RIGHT);
    }

    /* Bottom label bar */
    glColor3f(0.f, 0.f, 0.f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0); glVertex2f(WIN_W, 0);
    glVertex2f(WIN_W, 58); glVertex2f(0, 58);
    glEnd();

    glColor3f(0.9f, 0.9f, 0.9f);
    drawText(10, 10, "SCORE: " + std::to_string(pac.score), GLUT_BITMAP_HELVETICA_12);

    if (powerActive) {
        glColor3f(0.3f, 1.f, 0.3f);
        drawText(220, 10, "* POWER *", GLUT_BITMAP_HELVETICA_12);
    }
}

/* =====================================================================
 * Overlay messages
 * ===================================================================== */
static void drawOverlay()
{
    if (gState == ST_INTRO) {
        /* dim background */
        glColor4f(0, 0, 0, 0.6f);
        glBegin(GL_QUADS);
        glVertex2f(0,0); glVertex2f(WIN_W,0);
        glVertex2f(WIN_W,WIN_H); glVertex2f(0,WIN_H);
        glEnd();

        glColor3f(1.f, 1.f, 0.f);
        drawText(180, 440, "PACMAN", GLUT_BITMAP_TIMES_ROMAN_24);
        glColor3f(1, 1, 1);
        drawText(130, 390, "Computer Graphics Lab Final", GLUT_BITMAP_HELVETICA_12);
        drawText(155, 360, "Arrow Keys  -  Move Pacman",  GLUT_BITMAP_HELVETICA_12);
        drawText(185, 340, "P  -  Pause / Resume",         GLUT_BITMAP_HELVETICA_12);
        drawText(205, 320, "R  -  Restart",                GLUT_BITMAP_HELVETICA_12);
        drawText(195, 300, "ESC  -  Quit",                 GLUT_BITMAP_HELVETICA_12);
        glColor3f(0.2f, 1.f, 0.2f);
        drawText(130, 260, "Press any key to start!", GLUT_BITMAP_HELVETICA_18);
    } else if (gState == ST_READY) {
        glColor3f(1.f, 1.f, 0.f);
        drawText(215, 260, "READY!", GLUT_BITMAP_HELVETICA_18);
    } else if (gState == ST_PAUSE) {
        glColor3f(1.f, 1.f, 0.f);
        drawText(210, 400, "PAUSED", GLUT_BITMAP_TIMES_ROMAN_24);
        glColor3f(1, 1, 1);
        drawText(185, 370, "Press P to resume", GLUT_BITMAP_HELVETICA_12);
    } else if (gState == ST_WIN) {
        glColor4f(0, 0, 0, 0.5f);
        glBegin(GL_QUADS);
        glVertex2f(0,0); glVertex2f(WIN_W,0);
        glVertex2f(WIN_W,WIN_H); glVertex2f(0,WIN_H);
        glEnd();
        glColor3f(0.2f, 1.f, 0.2f);
        drawText(195, 420, "YOU WIN!", GLUT_BITMAP_TIMES_ROMAN_24);
        glColor3f(1, 1, 1);
        drawText(160, 380, "Score: " + std::to_string(pac.score), GLUT_BITMAP_HELVETICA_18);
        drawText(170, 340, "Press R to play again", GLUT_BITMAP_HELVETICA_12);
    } else if (gState == ST_OVER) {
        glColor4f(0, 0, 0, 0.5f);
        glBegin(GL_QUADS);
        glVertex2f(0,0); glVertex2f(WIN_W,0);
        glVertex2f(WIN_W,WIN_H); glVertex2f(0,WIN_H);
        glEnd();
        glColor3f(1.f, 0.2f, 0.2f);
        drawText(185, 420, "GAME OVER", GLUT_BITMAP_TIMES_ROMAN_24);
        glColor3f(1, 1, 1);
        drawText(160, 380, "Score: " + std::to_string(pac.score), GLUT_BITMAP_HELVETICA_18);
        drawText(170, 340, "Press R to play again", GLUT_BITMAP_HELVETICA_12);
    }
}

/* =====================================================================
 * Init / Reset helpers
 * ===================================================================== */
static void resetMaze()
{
    totalDots = 0;
    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c < COLS; ++c) {
            maze[r][c] = MAZE_TEMPL[r][c];
            if (maze[r][c] == V_DOT || maze[r][c] == V_POWER) ++totalDots;
        }
    dotsEaten = 0;
}

static void resetPacman()
{
    /* Start at centre of row 23, between the double-wall: col 13 is V_EMPTY */
    float cx, cy;
    cellCentre(13, 23, cx, cy);
    pac.x        = cx;
    pac.y        = cy;
    pac.speed    = 2.f;
    pac.dir      = D_LEFT;
    pac.nextDir  = D_LEFT;
    pac.mouth    = 0.f;
    pac.mouthOpen = true;
    pac.dieTimer = 0;
}

static void resetGhosts()
{
    /* Scatter target corners */
    struct { int sc, sr; float cr, cg, cb; int startCol, startRow; } gd[4] = {
        { COLS - 3, 0,        1.f, 0.f,  0.f,   13, 11 }, /* Blinky – red,    top-right   */
        { 0,        0,        1.f, 0.7f, 0.8f,  13, 14 }, /* Pinky  – pink,   top-left    */
        { COLS - 1, ROWS - 1, 0.f, 1.f,  1.f,   11, 14 }, /* Inky   – cyan,   btm-right   */
        { 0,        ROWS - 1, 1.f, 0.5f, 0.f,   15, 14 }, /* Clyde  – orange, btm-left    */
    };

    for (int i = 0; i < 4; ++i) {
        float cx, cy;
        cellCentre(gd[i].startCol, gd[i].startRow, cx, cy);
        ghosts[i].x         = cx;
        ghosts[i].y         = cy;
        ghosts[i].speed     = 1.8f;
        ghosts[i].dir       = D_LEFT;
        ghosts[i].cr        = gd[i].cr;
        ghosts[i].cg        = gd[i].cg;
        ghosts[i].cb        = gd[i].cb;
        ghosts[i].scCol     = gd[i].sc;
        ghosts[i].scRow     = gd[i].sr;

        if (i == 0) {
            /* Blinky starts outside the house immediately */
            ghosts[i].mode      = GM_SCATTER;
            ghosts[i].modeTimer = 300;
            ghosts[i].houseTimer = 0;
        } else {
            ghosts[i].mode      = GM_HOUSE;
            ghosts[i].modeTimer = 0;
            ghosts[i].houseTimer = 60 + i * 120; /* staggered release */
        }
    }
}

static void initGame()
{
    resetMaze();
    pac.lives  = 3;
    pac.score  = 0;
    resetPacman();
    resetGhosts();
    powerActive = false;
    powerTimer  = 0;
    eatCombo    = 0;
    frame       = 0;
    gState      = ST_READY;
    gReady      = 120;
}

/* =====================================================================
 * Ghost AI helpers
 * ===================================================================== */
static int dist2(int c1, int r1, int c2, int r2)
{
    return (c1 - c2) * (c1 - c2) + (r1 - r2) * (r1 - r2);
}

/* Choose best direction at an intersection toward (targetCol, targetRow) */
static int chooseDir(int col, int row, int curDir,
                     int targetCol, int targetRow,
                     GhostMode mode)
{
    int best = D_NONE;
    int bestDist = INT_MAX;
    /* Adding 2 (mod 4) inverts direction in the RIGHT/DOWN/LEFT/UP encoding */
    int opp = (curDir + 2) % 4;

    for (int d = 0; d < 4; ++d) {
        if (d == opp) continue;
        int nc = col + DCOL[d];
        int nr = row + DROW[d];
        /* tunnel wrap */
        if (nc < 0) nc = COLS - 1;
        if (nc >= COLS) nc = 0;
        if (!ghostCanEnter(nc, nr, mode)) continue;
        int dd = dist2(nc, nr, targetCol, targetRow);
        if (dd < bestDist) { bestDist = dd; best = d; }
    }
    return best == D_NONE ? curDir : best;
}

/* Random valid direction (for frightened mode).
 * Uses a Mersenne-Twister engine instead of rand() for better quality. */
static int randomDir(int col, int row, int curDir, GhostMode mode)
{
    static std::mt19937 rng(std::random_device{}());
    /* Adding 2 (mod 4) inverts direction in the RIGHT/DOWN/LEFT/UP encoding */
    int opp = (curDir + 2) % 4;
    int choices[4]; int n = 0;
    for (int d = 0; d < 4; ++d) {
        if (d == opp) continue;
        int nc = col + DCOL[d];
        int nr = row + DROW[d];
        if (nc < 0) nc = COLS - 1;
        if (nc >= COLS) nc = 0;
        if (ghostCanEnter(nc, nr, mode)) choices[n++] = d;
    }
    if (n == 0) return (curDir + 2) % 4; /* reverse if completely stuck */
    std::uniform_int_distribution<int> dist(0, n - 1);
    return choices[dist(rng)];
}

/* =====================================================================
 * Movement helpers
 * ===================================================================== */
static bool nearCentre(float x, float y, int col, int row, float tol)
{
    float cx, cy;
    cellCentre(col, row, cx, cy);
    return (fabsf(x - cx) <= tol) && (fabsf(y - cy) <= tol);
}

/* =====================================================================
 * Update Pacman
 * ===================================================================== */
static void updatePacman()
{
    /* Mouth animation */
    if (pac.mouthOpen) {
        pac.mouth += 3.f;
        if (pac.mouth >= 44.f) pac.mouthOpen = false;
    } else {
        pac.mouth -= 3.f;
        if (pac.mouth <= 0.f) pac.mouthOpen = true;
    }

    /* Current cell */
    int col, row;
    pixelCell(pac.x, pac.y, col, row);

    /* Snap tolerance */
    const float SNAP = pac.speed + 1.f;

    /* Try to switch to queued direction when near cell centre */
    if (pac.nextDir != pac.dir && nearCentre(pac.x, pac.y, col, row, SNAP)) {
        int nc = col + DCOL[pac.nextDir];
        int nr = row + DROW[pac.nextDir];
        /* tunnel wrap */
        if (nc < 0)     nc = COLS - 1;
        if (nc >= COLS) nc = 0;
        if (pacCanEnter(nc, nr)) {
            pac.dir = pac.nextDir;
            /* snap to centre so movement stays grid-aligned */
            float cx, cy;
            cellCentre(col, row, cx, cy);
            pac.x = cx; pac.y = cy;
        }
    }

    /* Advance in current direction */
    float nx = pac.x + DPX[pac.dir] * pac.speed;
    float ny = pac.y + DPY[pac.dir] * pac.speed;

    /* Find which cell the new position centre would occupy */
    int ncol, nrow;
    /* For checking: use cell in the direction of movement */
    int fcol = col + DCOL[pac.dir];
    int frow = row + DROW[pac.dir];

    /* tunnel wrap at horizontal edges */
    if (fcol < 0)     fcol = COLS - 1;
    if (fcol >= COLS) fcol = 0;

    if (pacCanEnter(fcol, frow)) {
        pac.x = nx;
        pac.y = ny;
        /* Apply tunnel wrap */
        if (pac.x < 0)           pac.x += COLS * CELL;
        if (pac.x >= COLS * CELL) pac.x -= COLS * CELL;
    } else {
        /* Snap to current cell centre so Pacman doesn't straddle wall */
        float cx, cy;
        cellCentre(col, row, cx, cy);
        pac.x = cx;
        pac.y = cy;
    }

    /* Check for dot / power pellet at new cell */
    pixelCell(pac.x, pac.y, ncol, nrow);
    int &cell = maze[nrow][ncol];
    if (cell == V_DOT) {
        cell = V_EMPTY;
        pac.score += 10;
        ++dotsEaten;
    } else if (cell == V_POWER) {
        cell = V_EMPTY;
        pac.score += 50;
        ++dotsEaten;
        powerActive = true;
        powerTimer  = POWER_DURATION;
        eatCombo    = 0;
        /* Put all non-house ghosts into frightened mode */
        for (int i = 0; i < 4; ++i) {
            if (ghosts[i].mode != GM_HOUSE && ghosts[i].mode != GM_EXIT) {
                ghosts[i].mode      = GM_FRIGHT;
                ghosts[i].modeTimer = POWER_DURATION;
            }
        }
    }
}

/* =====================================================================
 * Update one ghost
 * ===================================================================== */
static void updateGhost(Ghost &g)
{
    int col, row;
    pixelCell(g.x, g.y, col, row);
    const float SNAP = g.speed + 1.f;

    /* --- Mode transitions --- */
    if (g.mode == GM_HOUSE) {
        /* bounce left/right inside house */
        g.x += DPX[g.dir] * (g.speed * 0.5f);
        float hcx, hcy;
        cellCentre(col, row, hcx, hcy);
        /* XOR with 2 flips between RIGHT(0)↔LEFT(2); DOWN(1)↔UP(3) stays intact.
         * 0.4f = 40% of cell width; exceeding this triggers the direction flip. */
        if (fabsf(g.x - hcx) > CELL * 0.4f) g.dir ^= 2;  /* flip LEFT↔RIGHT */

        --g.houseTimer;
        if (g.houseTimer <= 0) {
            g.mode = GM_EXIT;
            g.dir  = D_UP;
        }
        return;
    }

    if (g.mode == GM_EXIT) {
        /* Move toward exit cell (row 11, col 13) then enter maze */
        int exitCol = 13, exitRow = 11;
        float ecx, ecy;
        cellCentre(exitCol, exitRow, ecx, ecy);

        /* First move to centre column of house (col 13) */
        if (fabsf(g.x - ecx) > SNAP) {
            /* Move horizontally toward col 13 */
            g.dir = (g.x < ecx) ? D_RIGHT : D_LEFT;
        } else {
            g.x = ecx;
            /* Move upward */
            g.dir = D_UP;
        }
        g.x += DPX[g.dir] * g.speed;
        g.y += DPY[g.dir] * g.speed;

        if (g.y >= ecy - SNAP && g.y <= ecy + SNAP && fabsf(g.x - ecx) < SNAP) {
            g.x    = ecx;
            g.y    = ecy;
            g.mode = GM_SCATTER;
            g.modeTimer = 240;
            g.dir  = D_LEFT;
        }
        return;
    }

    /* Power pellet: frighten or eat timer */
    if (g.mode == GM_FRIGHT) {
        --g.modeTimer;
        if (g.modeTimer <= 0) {
            g.mode = GM_SCATTER;
            g.modeTimer = 240;
        }
    }

    /* Scatter / Chase cycling */
    if (g.mode == GM_SCATTER || g.mode == GM_CHASE) {
        --g.modeTimer;
        if (g.modeTimer <= 0) {
            if (g.mode == GM_SCATTER) {
                g.mode = GM_CHASE;
                g.modeTimer = 400;
            } else {
                g.mode = GM_SCATTER;
                g.modeTimer = 240;
            }
        }
    }

    if (g.mode == GM_EATEN) {
        /* Return to house */
        int homeCol = 13, homeRow = 14;
        if (nearCentre(g.x, g.y, homeCol, homeRow, SNAP)) {
            g.x    = (float)(homeCol * CELL + CELL / 2);
            g.y    = (float)(MAZE_BOT + (ROWS - 1 - homeRow) * CELL + CELL / 2);
            g.mode = GM_SCATTER;
            g.modeTimer = 240;
            g.dir  = D_LEFT;
            return;
        }
    }

    /* --- At cell centre: pick new direction --- */
    if (nearCentre(g.x, g.y, col, row, SNAP)) {
        float cx, cy;
        cellCentre(col, row, cx, cy);
        g.x = cx; g.y = cy;

        if (g.mode == GM_FRIGHT) {
            g.dir = randomDir(col, row, g.dir, g.mode);
        } else {
            int targetCol, targetRow;
            if (g.mode == GM_SCATTER) {
                targetCol = g.scCol;
                targetRow = g.scRow;
            } else if (g.mode == GM_CHASE) {
                int pc, pr;
                pixelCell(pac.x, pac.y, pc, pr);
                targetCol = pc;
                targetRow = pr;
            } else { /* GM_EATEN */
                targetCol = 13;
                targetRow = 11;
            }
            g.dir = chooseDir(col, row, g.dir, targetCol, targetRow, g.mode);
        }
    }
    /* Move ghost */
    float nx = g.x + DPX[g.dir] * g.speed;
    float ny = g.y + DPY[g.dir] * g.speed;

    int fcol = col + DCOL[g.dir];
    int frow = row + DROW[g.dir];
    if (fcol < 0)     fcol = COLS - 1;
    if (fcol >= COLS) fcol = 0;

    if (ghostCanEnter(fcol, frow, g.mode)) {
        g.x = nx;
        g.y = ny;
        if (g.x < 0)           g.x += COLS * CELL;
        if (g.x >= COLS * CELL) g.x -= COLS * CELL;
    } else {
        /* Force a direction change at next opportunity */
        float cx, cy;
        cellCentre(col, row, cx, cy);
        g.x = cx; g.y = cy;
        g.dir = chooseDir(col, row, g.dir,
                          g.mode == GM_SCATTER ? g.scCol : 13,
                          g.mode == GM_SCATTER ? g.scRow : 14,
                          g.mode);
    }
}

/* =====================================================================
 * Collision: Pacman vs Ghosts
 * ===================================================================== */
static bool checkGhostCollision()
{
    float hitRadius = CELL * 0.75f;
    for (int i = 0; i < 4; ++i) {
        Ghost &g = ghosts[i];
        if (g.mode == GM_HOUSE || g.mode == GM_EXIT || g.mode == GM_EATEN)
            continue;
        float dx = pac.x - g.x;
        float dy = pac.y - g.y;
        float d2 = dx * dx + dy * dy;
        if (d2 < hitRadius * hitRadius) {
            if (g.mode == GM_FRIGHT) {
                /* Eat ghost – score doubles each consecutive eat: 200, 400, 800, 1600 */
                g.mode = GM_EATEN;
                ++eatCombo;
                int pts = 200 * (1 << (eatCombo - 1));
                pac.score += pts;
            } else {
                return true; /* Pacman dies */
            }
        }
    }
    return false;
}

/* =====================================================================
 * Die sequence
 * ===================================================================== */
static void startDie()
{
    gState        = ST_DYING;
    pac.dieTimer  = 90;
    pac.dir       = D_NONE;
    pac.nextDir   = D_NONE;
    /* Freeze ghosts */
    for (int i = 0; i < 4; ++i)
        ghosts[i].speed = 0.f;
}

/* =====================================================================
 * Main update loop (called each timer tick)
 * ===================================================================== */
static void update()
{
    ++frame;

    if (gState == ST_READY) {
        --gReady;
        if (gReady <= 0) gState = ST_PLAY;
        return;
    }
    if (gState != ST_PLAY) return;

    /* Power timer */
    if (powerActive) {
        --powerTimer;
        if (powerTimer <= 0) {
            powerActive = false;
            eatCombo    = 0;
            for (int i = 0; i < 4; ++i)
                if (ghosts[i].mode == GM_FRIGHT) {
                    ghosts[i].mode = GM_SCATTER;
                    ghosts[i].modeTimer = 240;
                }
        }
    }

    updatePacman();
    for (int i = 0; i < 4; ++i) updateGhost(ghosts[i]);

    if (checkGhostCollision()) {
        startDie();
        return;
    }

    if (dotsEaten >= totalDots) {
        gState = ST_WIN;
    }
}

/* =====================================================================
 * Die animation update (separate from normal update)
 * ===================================================================== */
static void updateDying()
{
    ++frame;
    /* Animate mouth going wide open */
    pac.mouth = 90.f * (1.f - (float)pac.dieTimer / 90.f);
    --pac.dieTimer;
    if (pac.dieTimer <= 0) {
        --pac.lives;
        if (pac.lives <= 0) {
            gState = ST_OVER;
        } else {
            resetPacman();
            resetGhosts();
            powerActive = false;
            powerTimer  = 0;
            gState      = ST_READY;
            gReady      = 90;
        }
    }
}

/* =====================================================================
 * GLUT display callback
 * ===================================================================== */
static void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    drawMaze();

    /* Pacman */
    if (gState != ST_OVER && gState != ST_WIN) {
        glColor3f(1.f, 1.f, 0.f);
        float angle = pac.mouth;
        drawPacmanBody(pac.x, pac.y, CELL * 0.45f, angle,
                       (pac.dir == D_NONE) ? D_LEFT : pac.dir);
    }

    /* Ghosts */
    for (int i = 0; i < 4; ++i) {
        if (ghosts[i].mode != GM_HOUSE)
            drawGhost(ghosts[i]);
    }

    drawHUD();
    drawOverlay();

    glutSwapBuffers();
}

/* =====================================================================
 * GLUT timer callback (~60 fps)
 * ===================================================================== */
static void timer(int /*v*/)
{
    if (gState == ST_PLAY || gState == ST_READY)
        update();
    else if (gState == ST_DYING)
        updateDying();

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

/* =====================================================================
 * Keyboard callbacks
 * ===================================================================== */
static void onKeyboard(unsigned char key, int /*x*/, int /*y*/)
{
    if (key == 27 || key == 'q' || key == 'Q') {
        exit(0);
    }
    if (key == 'p' || key == 'P') {
        if (gState == ST_PLAY)  gState = ST_PAUSE;
        else if (gState == ST_PAUSE) gState = ST_PLAY;
        return;
    }
    if (key == 'r' || key == 'R') {
        initGame();
        return;
    }
    /* Any key on intro starts game */
    if (gState == ST_INTRO) {
        initGame();
        return;
    }
}

static void onSpecialKey(int key, int /*x*/, int /*y*/)
{
    /* Any special key on intro starts game */
    if (gState == ST_INTRO) { initGame(); return; }

    if (gState != ST_PLAY) return;

    switch (key) {
        case GLUT_KEY_RIGHT: pac.nextDir = D_RIGHT; break;
        case GLUT_KEY_LEFT:  pac.nextDir = D_LEFT;  break;
        case GLUT_KEY_UP:    pac.nextDir = D_UP;    break;
        case GLUT_KEY_DOWN:  pac.nextDir = D_DOWN;  break;
        default: break;
    }
}

/* =====================================================================
 * OpenGL init
 * ===================================================================== */
static void initGL()
{
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WIN_W, 0, WIN_H);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

/* =====================================================================
 * main
 * ===================================================================== */
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIN_W, WIN_H);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Pacman – Computer Graphics Lab Final");

    initGL();
    initGame();
    gState = ST_INTRO;   /* override initGame so intro screen shows first */

    glutDisplayFunc(display);
    glutKeyboardFunc(onKeyboard);
    glutSpecialFunc(onSpecialKey);
    glutTimerFunc(16, timer, 0);

    glutMainLoop();
    return 0;
}
