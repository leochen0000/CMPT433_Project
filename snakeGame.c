// snakeGame.c
// Module to start/stop the "Snake" game thread and provide routines to retrieve game data.
#include "snakeGame.h"


//------------ variables and definitions -----------------------
#define SNAKEBODY_LENGTH   4

static pthread_t snakeGame_id;
static pthread_mutex_t snakeGameStat = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t snakeGameData = PTHREAD_MUTEX_INITIALIZER;

static snakegame_coords snakebody[SNAKEBODY_LENGTH];  // coordinates of snake's body (pixels)
static snakegame_coords apple;                        // coordinate of apple (to be eaten by snake)
static int apples_consumed = 0;

static _Bool snake_in_motion = false;
static _Bool gameStopped = false;
static enum zenJoystickButton current_direction;

extern unsigned char font8x8[];

//------------ functions ---------------------------------------
//***** static functions ******************************
static void drawSnakeBody()
{
	for (int k = 0; k < SNAKEBODY_LENGTH; k++)
		extLED8x8DrawPixel(snakebody[k].x, snakebody[k].y, 1);
}

static void generateNextAppleCoord()
{
	int k;

	pthread_mutex_lock(&snakeGameData);
	{
		while(1) {
				// Generate random coordinate for apple
			apple.x = rand() % 8;
			apple.y = rand() % 8;

			for (k = 0; k < SNAKEBODY_LENGTH; k++) {
					// Check for conflicts with snake's body pixel locations
				if (k == 0) {  // head
					if (((apple.x >= snakebody[k].x) && ((apple.x - snakebody[k].x) < 2)) ||
					    ((apple.x <  snakebody[k].x) && ((snakebody[k].x - apple.x) < 2)) ||
					    ((apple.y >= snakebody[k].y) && ((apple.y - snakebody[k].y) < 2)) ||
					    ((apple.y <  snakebody[k].y) && ((snakebody[k].y - apple.y) < 2)))
						break;
				}
				else {
					if ((apple.x == snakebody[k].x) || (apple.y == snakebody[k].y))
						break;
				}
			}

			if (k == SNAKEBODY_LENGTH)  // No conflicts with body
				break;
		}
	}
	pthread_mutex_unlock(&snakeGameData);
}

static void initSnakeBody()
{
	pthread_mutex_lock(&snakeGameData);
	{
		snakebody[0].x = 4;  // head
		snakebody[0].y = 3;
		snakebody[1].x = 3;
		snakebody[1].y = 3;
		snakebody[2].x = 2;
		snakebody[2].y = 3;
		snakebody[3].x = 1;
		snakebody[3].y = 3;
	}
	pthread_mutex_unlock(&snakeGameData);
}

static void moveSnake(enum zenJoystickButton button)
{
	pthread_mutex_lock(&snakeGameData);
	{
		snakebody[3].x = snakebody[2].x;
		snakebody[3].y = snakebody[2].y;
		snakebody[2].x = snakebody[1].x;
		snakebody[2].y = snakebody[1].y;
		snakebody[1].x = snakebody[0].x;
		snakebody[1].y = snakebody[0].y;

		if (button == JOYSTICK_RIGHT) {
			snakebody[0].x++;
		}
		else if (button == JOYSTICK_LEFT) {
			snakebody[0].x--;
		}
		else if (button == JOYSTICK_UP) {
			snakebody[0].y--;
		}
		else {
			snakebody[0].y++;
		}
	}
	pthread_mutex_unlock(&snakeGameData);
}

static enum zenJoystickButton BannedDirection(enum zenJoystickButton button)
{
	if (button == JOYSTICK_RIGHT) {
		return(JOYSTICK_LEFT);
	}
	else if (button == JOYSTICK_LEFT) {
		return(JOYSTICK_RIGHT);
	}
	else if (button == JOYSTICK_UP) {
		return(JOYSTICK_DOWN);
	}
	else {
		return(JOYSTICK_UP);
	}
}


//-------------------------------------------------------
// "Balance the ball" game.
//-------------------------------------------------------
static void *snakeGameThread()
{
    struct timespec reqDelay;
	enum zenJoystickButton button;
	int samplecnt;

	apples_consumed = 0;
	gameStopped = false;
	snake_in_motion = false;

	// Initialize snake's body
	initSnakeBody();

	// 3, 2, 1 count down
	extLED8x8CountDown321(font8x8);

	// Draw ball in center of matrix
	generateNextAppleCoord();
	extLED8x8FillPixel(0);
	drawSnakeBody();
	extLED8x8DrawPixel(apple.x, apple.y, 1);
	extLED8x8DisplayUpdate();

    reqDelay.tv_sec = 0;
   	reqDelay.tv_nsec = 10000000;  // sampling rate

	while (1) {
		// Sample all joystick buttons
		for (button = JOYSTICK_UP; button < JOYSTICK_MAXBUTTONS; button++) {
			if (zenJoystickButtonPushed (button)) {
				break;
			}
		}

		if (gameStopped == false) {
			if (snake_in_motion == false) {
				if ((button < JOYSTICK_MAXBUTTONS) && (button != BannedDirection(JOYSTICK_RIGHT))) {
					moveSnake(button);

					extLED8x8FillPixel(0);
					drawSnakeBody();
					extLED8x8DrawPixel(apple.x, apple.y, 1);
					extLED8x8DisplayUpdate();

					snake_in_motion = true;
					samplecnt = 0;
					current_direction = button;
				}
			}
			else {
				if ((button < JOYSTICK_MAXBUTTONS) && (button != BannedDirection(current_direction))) {
					current_direction = button;
				}

				if (++samplecnt == 35) {
					samplecnt = 0;
					if (((snakebody[0].x == 7) && (current_direction == JOYSTICK_RIGHT)) ||
						((snakebody[0].x == 0) && (current_direction == JOYSTICK_LEFT)) ||
						((snakebody[0].y == 0) && (current_direction == JOYSTICK_UP)) ||
						((snakebody[0].y == 7) && (current_direction == JOYSTICK_DOWN))) {
						zenBuzzerBeep(110, 300);
						extLED8x8ScrollText(" Game Over!X", font8x8, 25, SCROLL_LEFT);
						gameStopped = true;
					}
					else {
						moveSnake(current_direction);
						extLED8x8FillPixel(0);
						drawSnakeBody();
						if ((snakebody[0].x == apple.x) && (snakebody[0].y == apple.y)) {
							apples_consumed++;
							generateNextAppleCoord();
							printf("Apples eaten:  %d\n", apples_consumed);
							zenBuzzerBeep(440, 100);
						}
						extLED8x8DrawPixel(apple.x, apple.y, 1);
						extLED8x8DisplayUpdate();
					}
				}
			}
		}

		// sampling delay
		nanosleep(&reqDelay, (struct timespec *) NULL);

		// Check for request to terminate thread
		int lockstat;
		lockstat = pthread_mutex_trylock(&snakeGameStat);
		if (lockstat == 0) {  // lock acquired (0) means terminate
			pthread_exit(0);
		}
	}

	pthread_exit(0);
}





//***** public functions ******************************

void snakeGame_start (void)
{
    printf("Starting game of 'Snake'\n");
	pthread_mutex_init(&snakeGameStat, NULL);

	// Lock mutex used by thread to check for request to end the thread
	pthread_mutex_lock(&snakeGameStat);

	// Start the thread
	pthread_create(&snakeGame_id, NULL, &snakeGameThread, NULL);
}


void snakeGame_stop (void)
{
	// Unlock mutex to let sorting thread know about request to finish
	pthread_mutex_unlock(&snakeGameStat);

    printf("Stopping game of 'Snake'\n");

	// Wait for thread to finish
	pthread_join(snakeGame_id, NULL);
}


void snakeGame_GetData (int *snakebody_length, snakegame_coords *snakebodycoords,
						snakegame_coords *applecoord, int *apples_eaten)
{
	pthread_mutex_lock(&snakeGameData);
	{
		*apples_eaten = apples_consumed;
		applecoord->x = apple.x;
		applecoord->y = apple.y;
		*snakebody_length = SNAKEBODY_LENGTH;
		for (int k = 0; k < SNAKEBODY_LENGTH; k++) {
			snakebodycoords[k].x = snakebody[k].x;
			snakebodycoords[k].y = snakebody[k].y;
		}
	}
	pthread_mutex_unlock(&snakeGameData);
}


