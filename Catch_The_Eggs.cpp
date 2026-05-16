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

// =====================================================
// DRAW BASKET
// =====================================================

void drawBasket()
{
    glColor3f(0.6f, 0.3f, 0.1f);

    glBegin(GL_QUADS);

    glVertex2f(basketX, basketY);
    glVertex2f(basketX + basketWidth, basketY);
    glVertex2f(basketX + basketWidth - 20, basketY + basketHeight);
    glVertex2f(basketX + 20, basketY + basketHeight);

    glEnd();
}

// =====================================================
// DRAW CHICKEN
// =====================================================

void drawChicken(float x, float y)
{
    // body
    glColor3f(1, 1, 1);
    drawCircle(x, y, 25);

    // head
    drawCircle(x + 20, y + 20, 15);

    // beak
    glColor3f(1, 0.5f, 0);

    glBegin(GL_TRIANGLES);

    glVertex2f(x + 35, y + 20);
    glVertex2f(x + 50, y + 25);
    glVertex2f(x + 35, y + 30);

    glEnd();

    // eye
    glColor3f(0, 0, 0);
    drawCircle(x + 25, y + 25, 2);

    // bamboo stick
    glColor3f(0.3f, 0.8f, 0.3f);

    glBegin(GL_QUADS);

    glVertex2f(0, y - 40);
    glVertex2f(WIDTH, y - 40);
    glVertex2f(WIDTH, y - 30);
    glVertex2f(0, y - 30);

    glEnd();
}

// =====================================================
// DRAW OBJECT
// =====================================================

void drawObject(FallingObject& obj)
{
    switch (obj.type)
    {
    case NORMAL_EGG:
        glColor3f(1, 1, 1);
        break;

    case BLUE_EGG:
        glColor3f(0, 0, 1);
        break;

    case GOLDEN_EGG:
        glColor3f(1, 0.84f, 0);
        break;

    case POOP:
        glColor3f(0.4f, 0.2f, 0);
        break;

    case POWER_BIG:
        glColor3f(0, 1, 0);
        break;

    case POWER_SLOW:
        glColor3f(1, 0, 1);
        break;

    case POWER_TIME:
        glColor3f(0, 1, 1);
        break;
    }

    drawCircle(obj.x, obj.y, 15);
}

// =====================================================
// COLLISION
// =====================================================

bool collision(FallingObject& obj)
{
    if (obj.x > basketX &&
        obj.x < basketX + basketWidth &&
        obj.y > basketY &&
        obj.y < basketY + basketHeight)
    {
        return true;
    }

    return false;
}

// =====================================================
// SPAWN OBJECT
// =====================================================

void spawnObject(float x, float y)
{
    FallingObject obj;

    obj.x = x;
    obj.y = y;

    int r = rand() % 100;

    if (r < 50)
    {
        obj.type = NORMAL_EGG;
        obj.speed = globalFallSpeed;
    }
    else if (r < 70)
    {
        obj.type = BLUE_EGG;
        obj.speed = globalFallSpeed + 1;
    }
    else if (r < 80)
    {
        obj.type = GOLDEN_EGG;
        obj.speed = globalFallSpeed + 2;
    }
    else if (r < 90)
    {
        obj.type = POOP;
        obj.speed = globalFallSpeed + 1;
    }
    else
    {
        int p = rand() % 3;

        if (p == 0)
            obj.type = POWER_BIG;
        else if (p == 1)
            obj.type = POWER_SLOW;
        else
            obj.type = POWER_TIME;

        obj.speed = globalFallSpeed;
    }

    objects.push_back(obj);
}