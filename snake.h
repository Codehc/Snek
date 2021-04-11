#include "kbhit.c"

typedef enum { LEFT, FORWARD, RIGHT, BACKWARD, FREEZE } direction;
typedef struct {
    int x;
    int y;
} point;

typedef struct {
    point loc;
    direction dir;
    direction nextDir;
} tailSegment;

typedef struct nodeT {
    tailSegment * segment;
    struct nodeT * nextNode;
} node;

typedef struct {
    point loc;
    direction dir;
    node * tailHead;
} snake;

void input(snake * snek, bool * gameOn);
void move(int screenX, int screenY, snake * snek, bool * gameOn, int * appleX, int * appleY);
void draw(int screenX, int screenY, snake * snek, bool * gameOn, int * appleX, int * appleY);

int genAppleX(int screenX);
int genAppleY(int screenY);


void addSegment(node * head, tailSegment * segment);
void removeSegment(node * head, int index);
tailSegment * getSegment(node * head, unsigned int index);
node * getNode(node * head, unsigned int index);

int getSegments(node * head);