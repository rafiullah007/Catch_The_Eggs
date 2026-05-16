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

// =====================================================
// UPDATE
// =====================================================

void update(int value)
{
    if (gameStarted && !gamePaused && !gameOver)
    {
        remainingTime -= 0.016f;

        if (remainingTime <= 0)
        {
            gameOver = true;
        }

        // chicken movement
        for (auto& c : chickens)
        {
            c.x += c.speed * c.dir;

            if (c.x > WIDTH - 80)
                c.dir = -1;

            if (c.x < 80)
                c.dir = 1;

            // random object spawn
            if (rand() % 100 < 2)
            {
                spawnObject(c.x, c.y - 50);
            }
        }

        // move objects
        for (int i = 0; i < objects.size(); i++)
        {
            objects[i].y -= objects[i].speed;

            if (collision(objects[i]))
            {
                switch (objects[i].type)
                {
                case NORMAL_EGG:
                    score += 1;
                    break;

                case BLUE_EGG:
                    score += 5;
                    break;

                case GOLDEN_EGG:
                    score += 10;
                    break;

                case POOP:
                    score -= 10;
                    break;

                case POWER_BIG:
                    basketWidth = 200;
                    bigBasketTimer = 600;
                    break;

                case POWER_SLOW:
                    globalFallSpeed = 2;
                    slowMotionTimer = 600;
                    break;

                case POWER_TIME:
                    remainingTime += 15;
                    break;
                }

                objects.erase(objects.begin() + i);
                i--;
            }
            else if (objects[i].y < 0)
            {
                objects.erase(objects.begin() + i);
                i--;
            }
        }

        // big basket timer
        if (bigBasketTimer > 0)
        {
            bigBasketTimer--;

            if (bigBasketTimer == 0)
            {
                basketWidth = 120;
            }
        }

        // slow motion timer
        if (slowMotionTimer > 0)
        {
            slowMotionTimer--;

            if (slowMotionTimer == 0)
            {
                globalFallSpeed = 4;
            }
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// =====================================================
// DISPLAY
// =====================================================

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // background
    glColor3f(0.6f, 0.9f, 1.0f);

    glBegin(GL_QUADS);

    glVertex2f(0, 0);
    glVertex2f(WIDTH, 0);
    glVertex2f(WIDTH, HEIGHT);
    glVertex2f(0, HEIGHT);

    glEnd();

    // menu
    if (!gameStarted)
    {
        glColor3f(0, 0, 0);

        drawText(400, 500, "CATCH THE EGGS");
        drawText(360, 420, "Press ENTER to Start");
        drawText(360, 380, "Press ESC to Exit");

        glutSwapBuffers();
        return;
    }

    // paused
    if (gamePaused)
    {
        glColor3f(0, 0, 0);

        drawText(430, 350, "GAME PAUSED");
        drawText(350, 300, "Press P to Resume");

        glutSwapBuffers();
        return;
    }

    // game over
    if (gameOver)
    {
        glColor3f(1, 0, 0);

        drawText(430, 400, "GAME OVER");

        stringstream ss;
        ss << "Final Score: " << score;

        drawText(420, 350, ss.str());
        drawText(320, 280, "Press R to Restart");

        glutSwapBuffers();
        return;
    }

    // chickens
    for (auto& c : chickens)
    {
        drawChicken(c.x, c.y);
    }

    // falling objects
    for (auto& obj : objects)
    {
        drawObject(obj);
    }

    // basket
    drawBasket();

    // score
    glColor3f(0, 0, 0);

    stringstream scoreText;
    scoreText << "Score: " << score;

    drawText(20, 650, scoreText.str());

    // time
    stringstream timeText;
    timeText << "Time: " << (int)remainingTime;

    drawText(20, 620, timeText.str());

    // controls
    drawText(20, 590, "A/D or Mouse = Move Basket");
    drawText(20, 560, "P = Pause");

    glutSwapBuffers();
}


// =====================================================
// RESET GAME
// =====================================================

void resetGame()
{
    score = 0;
    remainingTime = 120;

    basketWidth = 120;

    objects.clear();
    chickens.clear();

    Chicken c1;

    c1.x = 150;
    c1.y = 620;
    c1.speed = 3;
    c1.dir = 1;

    Chicken c2;

    c2.x = 700;
    c2.y = 500;
    c2.speed = 2;
    c2.dir = -1;

    chickens.push_back(c1);
    chickens.push_back(c2);

    gameOver = false;
}
// =====================================================
// KEYBOARD
// =====================================================

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 13:
        gameStarted = true;
        resetGame();
        break;

    case 'a':
    case 'A':
        basketX -= basketSpeed;
        break;

    case 'd':
    case 'D':
        basketX += basketSpeed;
        break;

    case 'p':
    case 'P':
        gamePaused = !gamePaused;
        break;

    case 'r':
    case 'R':
        if (gameOver)
        {
            resetGame();
        }
        break;

    case 27:
        exit(0);
        break;
    }

    if (basketX < 0)
        basketX = 0;

    if (basketX + basketWidth > WIDTH)
        basketX = WIDTH - basketWidth;
}


// =====================================================
// MOUSE
// =====================================================

void mouseMotion(int x, int y)
{
    basketX = x - basketWidth / 2;

    if (basketX < 0)
        basketX = 0;

    if (basketX + basketWidth > WIDTH)
        basketX = WIDTH - basketWidth;
}

// =====================================================
// INIT
// =====================================================

void init()
{
    glClearColor(1, 1, 1, 1);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluOrtho2D(0, WIDTH, 0, HEIGHT);
}


// =====================================================
// MAIN
// =====================================================

int main(int argc, char** argv)
{
    srand(time(0));

    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);

    glutCreateWindow("Catch The Eggs");

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutPassiveMotionFunc(mouseMotion);

    glutTimerFunc(16, update, 0);

    glutMainLoop();

    return 0;
}
