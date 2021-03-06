#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>

#include "snake.h"

int tps = 4;
int screenX = 20;
int screenY = 10;

int main() {
    bool gameOn = true;

    direction oldDir;

    int appleX = genAppleX(screenX);
    int appleY = genAppleY(screenY);

    node * head = (node *) malloc(sizeof(node));
    
    snake * snek = (snake *) malloc(sizeof(snake));
    snek->loc.x = (screenX - 2) / 2;
    snek->loc.y = (screenY - 2) / 2;
    snek->dir = FREEZE;
    snek->tailHead = head;

    int cycleFramPos = 0;

    // Main event loop
    while (gameOn) {
        input(snek, &gameOn);
        move(screenX, screenY, snek, &gameOn, &appleX, &appleY);
        draw(screenX, screenY, snek, &gameOn, &appleX, &appleY);
        cycleFramPos++;
        usleep(1000000 / tps);
    }
}

void input(snake * snek, bool * gameOn) {
    // If keyboard is pressed
    if (kbhit()) {
        // Store the key pressed in this
        char charPressed = getchar();

        // Change the Snakes direction based on the key pressed
        if (charPressed == *"w") {
            if (snek->dir != BACKWARD) {
                snek->dir = FORWARD;
            }
        } else if (charPressed == *"a") {
            if (snek->dir != RIGHT) {
                snek->dir = LEFT;
            }
        } else if (charPressed == *"s") {
            if (snek->dir != FORWARD) {
                snek->dir = BACKWARD;
            }
        } else if (charPressed == *"d") {
            if (snek->dir != LEFT) {
                snek->dir = RIGHT;
            }
        }
    }
}

void move(int screenX, int screenY, snake * snek, bool * gameOn, int * appleX, int * appleY) {
    screenX = screenX - 2;
    screenY = screenY -2;

    // Increment head location based on direction stored
    if (snek->dir == FORWARD) {
        snek->loc.y--;
    } else if (snek->dir == LEFT) {
        snek->loc.x--;
    } else if (snek->dir == BACKWARD) {
        snek->loc.y++;
    } else if (snek->dir == RIGHT) {
        snek->loc.x++;
    }

    // Move tail points
    int i;
    for (i = 0; i < getSegments(snek->tailHead); i++) {
        bool isLastTail = i == getSegments(snek->tailHead) - 1;

        tailSegment * tail = getSegment(snek->tailHead, i);
        if (tail != NULL) {
            // Turn tail direction
            tail->dir = tail->nextDir;

            // Set next dir
            if (i == 0) {
                // Only tail, set next direction to snek head direction
                tail->nextDir = snek->dir;
            } else {
                // There are other tail segments in front
                tail->nextDir = getSegment(snek->tailHead, i - 1)->dir;
            }

            // Increment new location based on direction stored
            if (tail->dir == FORWARD) {
               tail->loc.y--;
            } else if (tail->dir == LEFT) {  
                tail->loc.x--;
            } else if (tail->dir == BACKWARD) {
                tail->loc.y++;
            } else if (tail->dir == RIGHT) {
                tail->loc.x++;
            }
        }
    }

    if (*appleX == snek->loc.x + 1 && *appleY == snek->loc.y + 1) {
    //if (true) {
        // Eat the apple
        *appleX = genAppleX(screenX);
        *appleY = genAppleY(screenY);

        // Get last tail info
        int adjIndex = getSegments(snek->tailHead) - 1;
        if (adjIndex == -1) {
            adjIndex = 0;
        }
        tailSegment * lastTail = getSegment(snek->tailHead, adjIndex);
        direction lastDirection;
        point * lastTailCoords = (point *) malloc(sizeof(point));
        if (lastTail != NULL) {
            // Set last direction to the last segments info
            lastDirection = lastTail->dir;
            lastTailCoords->x = lastTail->loc.x;
            lastTailCoords->y = lastTail->loc.y;
        } else {
            // No tail segments, set to head info
            lastDirection = snek->dir;
            lastTailCoords->x = snek->loc.x;
            lastTailCoords->y = snek->loc.y;
        }

        // Add new tail point
        int segments = getSegments(snek->tailHead);

        tailSegment * tail = (tailSegment *) malloc(sizeof(tailSegment));

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

        tail->loc.x = newCoords->x;
        tail->loc.y = newCoords->y;
        tail->dir = lastDirection;
        
        // Set next dir
        if (segments == 0) {
            // Only tail, set next direction to snek head direction
            tail->nextDir = snek->dir;
        } else {
            // There are other tail segments in front
            tail->nextDir = getSegment(snek->tailHead, segments - 1)->dir;
        }

        // Create next node (NOT SEGMENT, NODE)
        node * nextNode = (node *) malloc(sizeof(node));

        nextNode->segment = tail;

        if (segments > 0) {
            // Get last node
            node * lastNode = getNode(snek->tailHead, segments - 1);

            lastNode->nextNode = nextNode;
        } else {
            snek->tailHead = nextNode;
        }
    }

    // Detect collisions with tail
    int l;
    for (l = 0; l < 20; l++) {
        tailSegment * tail = getSegment(snek->tailHead, l);
        if (tail != NULL) {
            if (tail->loc.x == snek->loc.x && tail->loc.y == snek->loc.y) {
                *gameOn = false;
            }
        }
    }

    // Clamp headX
    if (snek->loc.x > screenX) {
        *gameOn = false;
    } else if (snek->loc.x < 0) {
        *gameOn = false;
    }

    // Clamp headY
    if (snek->loc.y > screenY) {
        *gameOn = false;
    } else if (snek->loc.y < 0) {
        *gameOn = false;
    }
}

void draw(int screenX, int screenY, snake * snek, bool * gameOn, int * appleX, int * appleY) {
    if (!*gameOn) {
        int spamNum = (screenY/2) - 1;
        for (int i = 0; i < spamNum; i++) {
            printf("\n");
        }
        printf("GAME OVER\nScore: %d", getSegments(snek->tailHead));
        for (int i = 0; i < spamNum; i++) {
            printf("\n");
        }
        free(snek);
        return;
    }

    int x;
    int y;

    printf("Segments: %d\n", getSegments(snek->tailHead));

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

            if (snek->loc.x + 1 == x && snek->loc.y + 1 == y) {
                // Snake head
                if (snek->dir == FORWARD) {
                    printf("^");
                } else if (snek->dir == LEFT) {  
                    printf("<");
                } else if (snek->dir == BACKWARD) {
                    printf("V");
                } else if (snek->dir == RIGHT) {
                    printf(">");
                } else {
                    printf("&");
                }
                isPixelFull = true;
            }

            // Render tail points
            int i;
            for (i = 0; i < getSegments(snek->tailHead); i++) {
                if (!isPixelFull) {
                    tailSegment * tail = getSegment(snek->tailHead, i);
                    if (tail->loc.x != -1) {
                        if (tail->loc.x + 1 == x && tail->loc.y + 1 == y) {
                            printf("*");
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
    srand(time(NULL));
    int gen = (rand() % ((screenX - 2) + 1)) + 1;
    return gen;
}

int genAppleY(int screenY) {
    srand(time(NULL));
    int gen = (rand() % ((screenY - 2) + 1)) + 1;
    return gen;
}

void addSegment(node * head, tailSegment * segment) {
    // Set the current node
    node * current = head;

    // Go to last segment in the list
    for (int i = 0; i < getSegments(head); i++) {
        // Set the current to the next node
        current = current->nextNode;
    }

    // This is the last segment, update it with the new segment we want to add
    current->segment = segment;
}

void removeSegment(node * head, int index) {
    // Set the current node
    node * current = head;

    // Go to the segment at the correct index
    for (int i = 0; i < index; i++) {
        // Set the current to the next node
        current = current->nextNode;
    }

    // Remove the node by resetting values
    current->segment = NULL;
    current->nextNode = NULL;
}

tailSegment * getSegment(node * head, unsigned int index) {
    // Set the current node
    node * current = head;

    // Go to the node at the correct index
    for (int i = 0; i < index; i++) {
        // Set the current to the next node
         if (current->nextNode != NULL) {
            current = current->nextNode;
        } else {
            return NULL;
        }
    }

    // current is equal to the node at the index specified
    return current->segment;
}

int getSegments(node * head) {
    // Set the current node
    node * current = head;

    // Counter int
    int i = 0;

    // Checks case where there is no first segment
    if (current->segment != NULL) {
        i++;
        // Go to last node in the list
        while (current->nextNode != NULL) {
            // Set the current to the next node
            current = current->nextNode;
            i++;
        }
    }

    return i;
}

node * getNode(node * head, unsigned int index) {
    // Set the current node
    node * current = head;

    // Go to the node at the correct index
    for (int i = 0; i < index; i++) {
        // Set the current to the next node
         if (current->nextNode != NULL) {
            current = current->nextNode;
        } else {
            return NULL;
        }
    }

    // current is equal to the node at the index specified
    return current;
}