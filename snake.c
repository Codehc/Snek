#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include "snake.h"

int tps = 4;
direction dir = FREEZE;
int screenX = 20;
int screenY = 10;

int main() {
    bool gameOn = true;

    direction oldDir;

    int appleX = genAppleX(screenX);
    int appleY = genAppleY(screenY);

    snake * snek = (snake *) malloc(sizeof(snake));
    snek->headX = (screenX - 2) / 2;
    snek->headY = (screenY - 2) / 2;

    int i;
    for (i = 0; i < 20; i++) {
        turnPoint * turn = &snek->turns[i];
        turn->x = -1;
        turn->y = -1;

        tailSegment * tail = &snek->tail[i];
        tail->x = -1;
        tail->y = -1;
    }

    for (i = 0; i < 20; i++) {
        tailSegment * tail = &snek->tail[i];
        printf("Alpha (%d, %d)  ", tail->x, tail->y);
        tailSegment tailN = snek->tail[i];
        printf("Beta (%d, %d)\n", tailN.x, tailN.y);
    }

    int cycleFramPos = 0;

    // Main event loop
    while (gameOn) {
        input(snek, &gameOn, &oldDir);
        move(screenX, screenY, snek, &gameOn, &appleX, &appleY, &oldDir);
        draw(screenX, screenY, snek, &gameOn, &appleX, &appleY);
        cycleFramPos++;
        usleep(1000000 / tps);
    }
}

void input(snake * snek, bool * gameOn, direction * oldDir) {
    *oldDir = dir;
    // If keyboard is pressed
    if (kbhit()) {
        // Store the key pressed in this
        char charPressed = getchar();

        // Change the Snakes direction based on the key pressed
        if (charPressed == *"w") {
            dir = FORWARD;
        } else if (charPressed == *"a") {
            dir = LEFT;
        } else if (charPressed == *"s") {
            dir = BACKWARD;
        } else if (charPressed == *"d") {
            dir = RIGHT;
        }
    }
}

void move(int screenX, int screenY, snake * snek, bool * gameOn, int * appleX, int * appleY, direction * oldDir) {
    screenX = screenX - 2;
    screenY = screenY -2;

    // Add turn point if needed
    if (*oldDir != dir) {
        // Direction has changed.
        if (getTailSegments(snek) > 0) {
            int i;
            for (i = 0; i < 20; i++) {
                turnPoint * turn = &snek->turns[i];
                if (turn->x == -1) {
                    turn->x = snek->headX;
                    turn->y = snek->headY;
                    turn->newDir = dir;
                    turn->oldDir = *oldDir;
                    break;
                }
            }
        }
    }

    // Increment head location based on direction stored
    if (dir == FORWARD) {
        snek->headY--;
    } else if (dir == LEFT) {
        snek->headX--;
    } else if (dir == BACKWARD) {
        snek->headY++;
    } else if (dir == RIGHT) {
        snek->headX++;
    }

    int l;
    for (l = 0; l < getTailSegments(snek); l++) {
        tailSegment * tail = &snek->tail[l];
        if (tail->x != -1) {
            if (tail->x == snek->headX && tail->y == snek->headY) {
                gameOn = false;
            }
        }
    }

    // Move tail points
    int i;
    for (i = 0; i < 20; i++) {
        bool isLastTail = snek->tail[i + 1].x == -1;

        tailSegment * tail = &snek->tail[i];
        if (tail->x != -1) {
            // Check if tail is on turn point
            int i;
            for (i = 0; i < getTurns(snek); i++) {
                turnPoint * turn = &snek->turns[i];
                if (turn->x != -1) {
                    if (tail->x == turn->x && tail->y == turn->y) {
                        // Tail is on a turn point, change its direction
                        tail->dir = turn->newDir;

                        // If this is the last tail then delete the turn point
                        if (isLastTail) {
                            turn->x = -1;
                            turn->y = -1;
                        }
                    }
                }
            }

            // Increment new location based on direction stored
            if (tail->dir == FORWARD) {
               tail->y--;
            } else if (tail->dir == LEFT) {  
                tail->x--;
            } else if (tail->dir == BACKWARD) {
                tail->y++;
            } else if (tail->dir == RIGHT) {
                tail->x++;
            }
        }
    }

    if (*appleX == snek->headX + 1 && *appleY == snek->headY + 1) {
        // Eat the apple
        *appleX = genAppleX(screenX);
        *appleY = genAppleY(screenY);

        // Get last tail info
        int adjIndex = getTailSegments(snek) - 1;
        if (adjIndex == -1) {
            adjIndex = 0;
        }
        tailSegment lastTail = snek->tail[adjIndex];
        direction lastDirection;
        point * lastTailCoords = (point *) malloc(sizeof(point));
        if (lastTail.x != -1) {
            // Set last direction to the last segments info
            lastDirection = lastTail.dir;
            lastTailCoords->x = lastTail.x;
            lastTailCoords->y = lastTail.y;
        } else {
            // No tail segments, set to head info
            lastDirection = dir;
            lastTailCoords->x = snek->headX;
            lastTailCoords->y = snek->headY;
        }

        // Add new tail point
        int i;
        for (i = 0; i < 20; i++) {
            tailSegment * tail = &snek->tail[i];
            if (tail->x == -1) {
                // Generate new coords
                point * newCoords = (point *) malloc(sizeof(point));
                newCoords->x = lastTailCoords->x;
                newCoords->y = lastTailCoords->y;

                // Increment new location based on direction stored
                if (lastDirection == FORWARD) {
                    newCoords->y++;
                } else if (lastDirection == LEFT) {  
                    newCoords->x++;
                } else if (lastDirection == BACKWARD) {
                    newCoords->y--;
                } else if (lastDirection == RIGHT) {
                    newCoords->x--;
                }

                tail->x = newCoords->x;
                tail->y = newCoords->y;
                tail->dir = lastDirection;
                break;
            }
        }
    }

    // Clamp headX
    if (snek->headX > screenX) {
        *gameOn = false;
    } else if (snek->headX < 0) {
        *gameOn = false;
    }

    // Clamp headY
    if (snek->headY > screenY) {
        *gameOn = false;
    } else if (snek->headY < 0) {
        *gameOn = false;
    }
}

void draw(int screenX, int screenY, snake * snek, bool * gameOn, int * appleX, int * appleY) {
    if (!*gameOn) {
        printf("GAME OVER");
        return;
    }

    int x;
    int y;

    for (y = 0; y <= screenY; y++) {
        for (x = 0; x <= screenX; x++) {
            bool isPixelFull = false;

            if (y == 0 || y == screenY || x == 0 || x == screenX) {
                // Walls
                printf("#");
                isPixelFull = true;
            }

            if (x == screenX) {
                // New lines
                printf("\n");
                isPixelFull = true;
            }

            if (*appleX == x && *appleY == y) {
                // Draw the apple
                printf("A");
                isPixelFull = true;
            }

            if (snek->headX + 1 == x && snek->headY + 1 == y) {
                // Snake head
                if (dir == FORWARD) {
                    printf("^");
                } else if (dir == LEFT) {  
                    printf("<");
                } else if (dir == BACKWARD) {
                    printf("V");
                } else if (dir == RIGHT) {
                    printf(">");
                } else {
                    printf("&");
                }
                isPixelFull = true;
            }

            // Render tail points
            int i;
            for (i = 0; i < getTailSegments(snek); i++) {
                if (!isPixelFull) {
                    tailSegment * tail = &snek->tail[i];
                    if (tail->x != -1) {
                        if (tail->x + 1 == x && tail->y + 1 == y) {
                            printf("%d", i + 1);
                            isPixelFull = true;
                        }
                    }
                }
            }

            // Render turn points WARNING: TESTING ONLY
            for (i = 0; i < getTurns(snek); i++) {
                if (!isPixelFull) {
                    turnPoint * turn = &snek->turns[i];
                    if (turn->x != -1) {
                        if (turn->x + 1 == x && turn->y + 1 == y) {
                            printf("@");
                            isPixelFull = true;
                        }
                    }
                }
            }

            if (!isPixelFull) {
                // Filler
                printf(" ");
            }
        }
    }
}

int genAppleX(int screenX) {
    return (rand() % ((screenX - 2) + 1));
}

int genAppleY(int screenY) {
    return (rand() % ((screenY - 2) + 1));
}

int getTurns(snake * snek) {
    int i;
    for (i=0; i < 20; i++) {
        turnPoint nextTurn = snek->turns[i + 1];
        if (nextTurn.x == -1) {
           // Last turn point
           i++;
           break;
        }
    }
    return i;
}

int getTailSegments(snake * snek) {
    int i;
    for (i=0; i < 20; i++) {
        tailSegment tail = snek->tail[i];
        if (tail.x == -1) {
           // Last tail segment
           break;
        }
    }
    return i;
}