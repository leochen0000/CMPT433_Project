// floppyBirbGame.c
// Module to start/stop the new game thread and provide routines to retrieve game data.
#include "floppyBirbGame.h"

//------------ variables and definitions -----------------------
#define BIRDBODY_LENGTH   2
#define N_WALLS 4
#define INIT_WALL_x 3   // The initial walls will be generated starting from this x-pos
#define MAX_WALL_HEIGHT 3
#define SPACE_BETWEEN_WALLS 1
#define WALL_GAP 5

#define MAX_WIDTH   7
#define MIN_WIDTH   0
#define MAX_HEIGHT  0
#define MIN_HEIGHT  7


static pthread_t floppyBirbGame_id;
static pthread_mutex_t floppyBirbGameStat = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t floppyBirbGameData = PTHREAD_MUTEX_INITIALIZER;
static birdgame_coords birdbody[BIRDBODY_LENGTH];  // coordinates of bird's body (pixels)
static birdgame_coords wall[N_WALLS];  // coordinates of walls to fly through

static _Bool gameStopped = false;
static _Bool hovering = false;
static _Bool bird_in_motion = false;
static _Bool detected_collision = false;
static int points = 0;
static int total_score = 0;

// static enum zenJoystickButton current_direction;


extern unsigned char font8x8[];

//------------ functions ---------------------------------------
//***** static functions ******************************

static void drawBirdBody()
{
	for (int k = 0; k < BIRDBODY_LENGTH; k++)
		extLED8x8DrawPixel(birdbody[k].x, birdbody[k].y, 1);
}

static void initBirdBody()
{
	pthread_mutex_lock(&floppyBirbGameData);
	{   
        // head
		birdbody[0].x = 2;  
		birdbody[0].y = 3;

        // tail
		birdbody[1].x = 1;
		birdbody[1].y = 4;
	}
	pthread_mutex_unlock(&floppyBirbGameData);
}

static void flyUp()
{
    // printf("Called flyUp()\n");
    pthread_mutex_lock(&floppyBirbGameData);
    {   
        // Check that we don't go out of bounds
        if (birdbody[0].y > MAX_HEIGHT) {
            // head 
		    birdbody[0].y--;
            // tail
		    birdbody[1].y = birdbody[0].y + 1;
            // printf("flyUp()\n");
        }
    }
    pthread_mutex_unlock(&floppyBirbGameData);
}

static void fallDown()
{
    // printf("Called flyDown()\n");
    pthread_mutex_lock(&floppyBirbGameData);
    {   
        // Check that we don't go out of bounds
        if (birdbody[0].y < MIN_HEIGHT) {
            // head 
		    birdbody[0].y++;
            // tail
		    birdbody[1].y = birdbody[0].y - 1;
            // printf("flyDown()\n");
        }
    }
    pthread_mutex_unlock(&floppyBirbGameData);
}

static void hover()
{
    pthread_mutex_lock(&floppyBirbGameData);
    {   
        birdbody[1].y = birdbody[0].y;
    }
    pthread_mutex_unlock(&floppyBirbGameData);
}

static void initWalls()
{
    pthread_mutex_lock(&floppyBirbGameData);
	{   
        for (int k = 0; k < N_WALLS; k++) {
            wall[k].x = INIT_WALL_x + (k * 2);
            wall[k].y = rand() % MAX_WALL_HEIGHT;
        }
	}
	pthread_mutex_unlock(&floppyBirbGameData);
}

static void drawWalls()
{
	for (int k = 0; k < N_WALLS; k++) {

        // Top half of wall
		for (int j = 0; j <= wall[k].y; j++) {
            extLED8x8DrawPixel(wall[k].x, wall[k].y - j, 1);
        }

        // Bottom half of wall
        for (int j = WALL_GAP + 1; j <= MIN_HEIGHT; j++) {
            extLED8x8DrawPixel(wall[k].x, wall[k].y + j, 1);
        }

    }
}

static void moveWalls()
{
	int k;

	pthread_mutex_lock(&floppyBirbGameData);
	{
		// while(1) {
			for (k = 0; k < N_WALLS; k++) {
                wall[k].x--;
                if (wall[k].x < MIN_WIDTH) {
                    wall[k].x = MAX_WIDTH;
                    wall[k].y = rand() % MAX_WALL_HEIGHT;
                }
            }
		// }
	}
	pthread_mutex_unlock(&floppyBirbGameData);
}

static _Bool detect_collision()
{
    _Bool collision = false;
    for (int i = 0; i < N_WALLS; i++) {
        for (int j = 0; j < BIRDBODY_LENGTH; j++) {
            if (((birdbody[j].x == wall[i].x) && (birdbody[j].y == wall[i].y)) ||
                ((birdbody[j].x == wall[i].x) && (birdbody[j].y == wall[i].y + WALL_GAP + 1))) 
            {
                collision = true;
            }
        }
    }
    return collision;
}

//-------------------------------------------------------
// New game.
//-------------------------------------------------------
static void *floppyBirbGameThread()
{
    struct timespec reqDelay;
    enum zenJoystickButton button;
    int samplecnt;

    points = 0;
    total_score = 0;
    gameStopped = false;
    bird_in_motion = false;

    // Initialize bird's body and walls
    initBirdBody();
    initWalls();

	// Draw bird on the left side of matrix and walls on the right side
	extLED8x8FillPixel(0);
    drawBirdBody();
    drawWalls();
	extLED8x8DisplayUpdate();

    // Initialize score on seg display
	zenSegDisplayUpdateNum(total_score);

    reqDelay.tv_sec = 0;
   	reqDelay.tv_nsec = 100000000;  // 100ms sampling rate

	while (1) {
		// Sample all joystick buttons
		for (button = JOYSTICK_UP; button < JOYSTICK_MAXBUTTONS; button++) {
			if (zenJoystickButtonPushed (button)) {
				break;
			}
		}

        if (gameStopped == false) {
            if (bird_in_motion == false) {
				if (button < JOYSTICK_MAXBUTTONS) {
                    // If any joystick button is pressed, fly up one unit
					flyUp();
					extLED8x8FillPixel(0);
					drawBirdBody();
					extLED8x8DisplayUpdate();

					bird_in_motion = true;
					samplecnt = 0;
				} 
			} 
            else { // Bird is in flight
                if (++samplecnt == 2) {
                    samplecnt = 0;

                    // Check for collision first
                    detected_collision = detect_collision();
                    if (detected_collision) {
                        zenBuzzerBeep(110, 300);
                        extLED8x8FillPixel(0);
						extLED8x8ScrollText(" Game Over!X", font8x8, 25, SCROLL_LEFT);
						gameStopped = true;
                        continue;
                    }
                    // If any joystick button is pressed, fly up one unit
                    else if (button < JOYSTICK_MAXBUTTONS) {
                        flyUp(); 
                        extLED8x8FillPixel(0);
                        drawBirdBody();
                        hovering = true;

                    } 
                    else {
                        if (hovering) { // Intermediate state between flying up and falling
                            hover();
                            extLED8x8FillPixel(0);
                            drawBirdBody();
                            hovering = false;
                        } else {
                            fallDown();
                            extLED8x8FillPixel(0);
                            drawBirdBody();
                        }
                        
                    }
                    
                    // Move walls one position to the left
                    moveWalls();
                    drawWalls();
                    extLED8x8DisplayUpdate();

                    // Increase score and display
                    points++;
                    if ((points % 10 == 0) && (points > 0)) {
                        total_score++;
                        zenBuzzerBeep(440, 100);
                    }
                    zenSegDisplayUpdateNum(total_score);
                }
                
            }
        } 

		// sampling delay
		nanosleep(&reqDelay, (struct timespec *) NULL);

		// Check for request to terminate thread
		int lockstat;
		lockstat = pthread_mutex_trylock(&floppyBirbGameStat);
		if (lockstat == 0) {  // lock acquired (0) means terminate
			pthread_exit(0);
		}
	}

	pthread_exit(0);
}





//***** public functions ******************************

void floppyBirbGame_start (void)
{
    printf("Starting game of 'Floppy Birb'\n");

	// 3, 2, 1 count down
    extLED8x8CountDown321(font8x8);

	pthread_mutex_init(&floppyBirbGameStat, NULL);

	// Lock mutex used by thread to check for request to end the thread
	pthread_mutex_lock(&floppyBirbGameStat);

	// Start the thread
	pthread_create(&floppyBirbGame_id, NULL, &floppyBirbGameThread, NULL);
}


void floppyBirbGame_stop (void)
{
	// Unlock mutex to let sorting thread know about request to finish
	pthread_mutex_unlock(&floppyBirbGameStat);

    printf("Stopping game of 'Floppy Birb'\n");

	// Exit game logo
    extLED8x8ExitGame(font8x8);

	// Wait for thread to finish
	pthread_join(floppyBirbGame_id, NULL);
}


void floppyBirbGame_GetData (
        int *birdbody_length, 
        birdgame_coords *birdbodycoords,
        int *score
    )
{
	pthread_mutex_lock(&floppyBirbGameData);
	{
		*birdbody_length = BIRDBODY_LENGTH;
        for (int i = 0; i < BIRDBODY_LENGTH; i++) {
            birdbodycoords[i].x = birdbody[i].x;
            birdbodycoords[i].y = birdbody[i].y;
        }
        *score = total_score;
	}
	pthread_mutex_unlock(&floppyBirbGameData);
}


