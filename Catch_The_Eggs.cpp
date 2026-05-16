#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include <sstream>
#include <cmath>
using namespace std;

// =====================================================
// WINDOW MABCDE
// =====================================================

const int WIDTH = 1000;  // Window width
const int HEIGHT = 700;


// =====================================================
// GAME STATE
// =====================================================

bool gameStarted = false;
bool gamePaused = false;
bool gameOver = false;

int score = 0;
float remainingTime = 120.0f;

// =====================================================
// BASKET
// =====================================================

float basketX = 450;
float basketY = 40;
float basketWidth = 120;
float basketHeight = 25;
float basketSpeed = 18;

// =====================================================
// CHICKEN
// =====================================================

struct Chicken
{
    float x;
    float y;
    float speed;
    int dir;
};

vector<Chicken> chickens; // chicken nummbers 

// =====================================================
// OBJECT TYPES
// =====================================================

enum ObjectType
{
    NORMAL_EGG,
    BLUE_EGG,
    GOLDEN_EGG,
    POOP,
    POWER_BIG,
    POWER_SLOW,
    POWER_TIME
};

struct FallingObject
{
    float x;
    float y;
    float speed;
    ObjectType type;
};

vector<FallingObject> objects;

float globalFallSpeed = 4.0f;

// =====================================================
// POWERUP TIMERS
// =====================================================

int bigBasketTimer = 0;
int slowMotionTimer = 0;

// =====================================================
// FUNCTION PROTOTYPES
// =====================================================

void spawnObject(float x, float y);
void resetGame();

// =====================================================
// TEXT DRAWING
// =====================================================

void drawText(float x, float y, string text)
{
    glRasterPos2f(x, y);

    for (char c : text)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

// =====================================================
// DRAW CIRCLE
// =====================================================

void drawCircle(float cx, float cy, float r)
{
    glBegin(GL_POLYGON);

    for (int i = 0; i < 360; i++)
    {
        float theta = i * 3.1416f / 180.0f;

        float x = r * cos(theta);
        float y = r * sin(theta);

        glVertex2f(cx + x, cy + y);
    }

    glEnd();
}