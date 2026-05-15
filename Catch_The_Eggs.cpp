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
// WINDOW
// =====================================================

const int WIDTH = 1000;
const int HEIGHT = 700;


// =====================================================
// GAME STATE
// =====================================================

bool gameStarted = false;
bool gamePaused = false;
bool gameOver = false;

int score = 0;
float remainingTime = 120.0f;


