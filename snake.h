#include "kbhit.c"

typedef enum { LEFT, FORWARD, RIGHT, BACKWARD, FREEZE } direction;

typedef struct {
    int x;
    int y;
    direction dir;
} tailSegment;

typedef struct {
    int x;
    int y;
    direction newDir;
    direction oldDir;
} turnPoint;

typedef struct {
    int headX;
    int headY;
    tailSegment tail[20];
    turnPoint turns[20];
} snake;

typedef struct {
    int x;
    int y;
} point;

void input(snake * snek, bool * gameOn, direction * oldDir);
void move(int screenX, int screenY, snake * snek, bool * gameOn, int * appleX, int * appleY, direction * oldDir);
void draw(int screenX, int screenY, snake * snek, bool * gameOn, int * appleX, int * appleY);

int genAppleX(int screenX);
int genAppleY(int screenY);

int getTurns(snake * snek);

int getTailSegments(snake * snek);