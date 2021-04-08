#include "kbhit.c"

typedef enum { LEFT, FORWARD, RIGHT, BACKWARD, FREEZE } direction;
typedef struct {
    int x;
    int y;
} point;

typedef struct {
    point loc;
    direction dir;
} tailSegment;

typedef struct {
    point loc;
    direction dir;
    tailSegment tail[20];
} snake;

void input(snake * snek, bool * gameOn, direction * oldDir);
void move(int screenX, int screenY, snake * snek, bool * gameOn, int * appleX, int * appleY, direction * oldDir);
void draw(int screenX, int screenY, snake * snek, bool * gameOn, int * appleX, int * appleY);

int genAppleX(int screenX);
int genAppleY(int screenY);

int getTailSegments(snake * snek);