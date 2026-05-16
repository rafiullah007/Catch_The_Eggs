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