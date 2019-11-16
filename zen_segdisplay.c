#include "zen_segdisplay.h"

//------------ variables and definitions -----------------------
static char *gpio61_outpath = "/sys/class/gpio/gpio61/value";  // Enable for left digit
static char *gpio44_outpath = "/sys/class/gpio/gpio44/value";  // Enable for right digit

static int i2cFileDesc;

// REG14 and REG15 pair of values for each digit
struct segdispregs
{
	unsigned char reg14;
	unsigned char reg15;
};

static int leftDigitNum = 0;
static int rightDigitNum = 0;

static pthread_t zenDisplay_id;
static pthread_mutex_t zenDisplay_mutex = PTHREAD_MUTEX_INITIALIZER;

// Look-up table for REG14 and REG15 registers for values 0 to 9
//      Bit 76543210
//  Byte0 = CXDJKLME
//  Byte1 = FPGHNAB-
//
//  AAAAAAAAA
// F P  G  H B
// F  P G H  B
//  NNNN JJJJ
// E  M L K  C
// E M  L  K C
//  DDDDDDDDD  XX

static struct segdispregs segdisp_lookup[] = 
{                  //      Byte0     Byte1
	{0xA1, 0x86},  // 0:  C-D----E, F----AB-
	{0x80, 0x12},  // 1:  C-------, ---H--B-
	{0x31, 0x0E},  // 2:  --DJ---E, ----NAB-
	{0xB0, 0x06},  // 3:  C-DJ----, -----AB-
	{0x90, 0x8A},  // 4:  C--J----, F---N-B-
	{0xB0, 0x8C},  // 5:  C-DJ----, F---NA--
	{0xB1, 0x8C},  // 6:  C-DJ---E, F---NA--
	{0x02, 0x14},  // 7:  ------M-, ---H-A--
	{0xB1, 0x8E},  // 8:  C-DJ---E, F---NAB-
	{0x90, 0x8E}   // 9:  C--J----, F---NAB-
};

//------------ functions ---------------------------------------
//***** static functions ******************************
static void assert_zenSegDigit_OK (enum zenSegDigit digit)
{
    assert((digit >= SEGDIGIT_LEFT) && (digit <= SEGDIGIT_BOTH));
}

static void assert_zenSegDigitValue_OK (int value)
{
    assert((value >= 0) && (value <= 9));
}

static unsigned int count(unsigned int i)
{
    unsigned int ret = 1;
    while (i /= 10) ret++;
    return ret;
}

//*****************************************************
// Turn on 15-segment digits
// Return value:  true=success, false=fail
//*****************************************************
static _Bool zenSegDisplayDigitOn(enum zenSegDigit digit)
{
	FILE *GPIOFile;

	// Parameters range checking
	assert_zenSegDigit_OK(digit);

	if ((digit == SEGDIGIT_LEFT) ||
	    (digit == SEGDIGIT_BOTH))
	{
		GPIOFile = fopen(gpio61_outpath, "w");
		if (GPIOFile == NULL) {
			printf("ERROR OPENING %s.\n", gpio61_outpath);
			return false;
		}
		fprintf(GPIOFile, "1");  // Set GPIO_61 to 1
		fclose(GPIOFile);
	}

	if ((digit == SEGDIGIT_RIGHT) ||
	    (digit == SEGDIGIT_BOTH))
	{
		GPIOFile = fopen(gpio44_outpath, "w");
		if (GPIOFile == NULL) {
			printf("ERROR OPENING %s.\n", gpio44_outpath);
			return false;
		}
		fprintf(GPIOFile, "1");  // Set GPIO_44 to 1
		fclose(GPIOFile);
	}

	return true;
}

//*****************************************************
// Turn off 15-segment digits
// Return value:  true=success, false=fail
//*****************************************************
static _Bool zenSegDisplayDigitOff(enum zenSegDigit digit)
{
	FILE *GPIOFile;

	// Parameters range checking
	assert_zenSegDigit_OK(digit);

	if ((digit == SEGDIGIT_LEFT) ||
	    (digit == SEGDIGIT_BOTH))
	{
		GPIOFile = fopen(gpio61_outpath, "w");
		if (GPIOFile == NULL) {
			printf("ERROR OPENING %s.\n", gpio61_outpath);
			return false;
		}
		fprintf(GPIOFile, "0");  // Set GPIO_61 to 0
		fclose(GPIOFile);
	}

	if ((digit == SEGDIGIT_RIGHT) ||
	    (digit == SEGDIGIT_BOTH))
	{
		GPIOFile = fopen(gpio44_outpath, "w");
		if (GPIOFile == NULL) {
			printf("ERROR OPENING %s.\n", gpio44_outpath);
			return false;
		}
		fprintf(GPIOFile, "0");  // Set GPIO_44 to 0
		fclose(GPIOFile);
	}

	return true;
}


//*****************************************************
// Display a numeric digit.
// 0 <= num <= 9.
// Return value:  true=success, false=fail
//*****************************************************
static _Bool zenSegDisplayNumDigit(int num)
{
	unsigned char buff[2];

	// Parameters range checking
	assert_zenSegDigitValue_OK(num);

		// Open I2C driver
	i2cFileDesc = open("/dev/i2c-1", O_RDWR);
	if (i2cFileDesc < 0) {
		printf("I2C DRV: Unable to open bus for read/write (/dev/i2c-1)\n");
		return false;
	}

		// Set slave address to 0x20
	int result = ioctl(i2cFileDesc, I2C_SLAVE, 0x20);
	if (result < 0) {
		printf("Unable to set I2C device to slave address to 0x20.\n");
		return false;
	}

	buff[0] = 0x14;
	buff[1] = segdisp_lookup[num].reg14;
	int res = write(i2cFileDesc, buff, 2);
	if (res != 2) {
		printf("ERROR!  Unable to write i2c register\n");
		return false;
	}

	buff[0] = 0x15;
	buff[1] = segdisp_lookup[num].reg15;
	res = write(i2cFileDesc, buff, 2);
	if (res != 2) {
		printf("ERROR!  Unable to write i2c register\n");
		return false;
	}

	close(i2cFileDesc);

	return true;
}

//*****************************************************
// Thread which updates seg display.
// Return value: None/NULL
//*****************************************************
static void *segDisplayThread()
{
	struct timespec reqDelay;
    reqDelay.tv_sec = 0;
    reqDelay.tv_nsec = 5000000;
	while(1) {
		// Update Left Display
		zenSegDisplayDigitOff(SEGDIGIT_BOTH);

		pthread_mutex_lock(&zenDisplay_mutex);
		
		zenSegDisplayNumDigit(leftDigitNum);
		zenSegDisplayDigitOn(SEGDIGIT_LEFT);
		
		nanosleep(&reqDelay, (struct timespec *) NULL);
		
		zenSegDisplayDigitOff(SEGDIGIT_LEFT);

		// Update Right Display
		zenSegDisplayDigitOff(SEGDIGIT_BOTH);
		
		zenSegDisplayNumDigit(rightDigitNum);
		zenSegDisplayDigitOn(SEGDIGIT_RIGHT);
		
		nanosleep(&reqDelay, (struct timespec *) NULL);

		pthread_mutex_unlock(&zenDisplay_mutex);
	}
	return NULL;
}

//***** public functions ******************************

//*****************************************************
// Initializes 2-digit x 15-segment LED display
// Return value:  true=success, false=fail
//*****************************************************
_Bool zenSegDisplayInit()
{
	FILE *GPIOFile;
    struct timespec reqDelay;
    reqDelay.tv_sec = 0;
    reqDelay.tv_nsec = 300000000;  // 300ms

	//----------------------------------------------------------------
	// Export GPIO_44 and GPIO_61
	//----------------------------------------------------------------
	GPIOFile = fopen("/sys/class/gpio/export", "w");
	if (GPIOFile == NULL) {
		printf("ERROR!  Cannot open /sys/class/gpio/export for writing!\n");
		exit(1);
	}
	fprintf(GPIOFile, "44");
	fclose(GPIOFile);

	GPIOFile = fopen("/sys/class/gpio/export", "w");
	if (GPIOFile == NULL) {
		printf("ERROR!  Cannot open /sys/class/gpio/export for writing!\n");
		exit(1);
	}
	fprintf(GPIOFile, "61");
	fclose(GPIOFile);
	nanosleep(&reqDelay, (struct timespec *) NULL);

	//----------------------------------------------------------------
	// Configure P8_26 (GPIO_61) and P8_12 (GPIO_44) pins as outputs
	// to turn on/off each 15-segment digit.  Initialize as "off" (0).
	//----------------------------------------------------------------
		// Configure GPIO_44 as output (enable for right digit)
	GPIOFile = fopen("/sys/class/gpio/gpio44/direction", "w");
	if (GPIOFile == NULL) {
		printf("ERROR!  Cannot open /sys/class/gpio/gpio44/direction for writing!\n");
		exit(1);
	}
	fprintf(GPIOFile, "out");  // Configure GPIO_44 as output
	fclose(GPIOFile);

		// Configure GPIO_61 as output (enable for left digit)
	GPIOFile = fopen("/sys/class/gpio/gpio61/direction", "w");
	if (GPIOFile == NULL) {
		printf("ERROR!  Cannot open /sys/class/gpio/gpio64/direction for writing!\n");
		exit(1);
	}
	fprintf(GPIOFile, "out");  // Configure GPIO_61 as output
	fclose(GPIOFile);

		// Set GPIO_61 output to 0 (turn off left digit)
	GPIOFile = fopen(gpio61_outpath, "w");
	if (GPIOFile == NULL) {
		printf("ERROR OPENING %s.\n", gpio61_outpath);
		return false;
	}
	fprintf(GPIOFile, "0");  // Set GPIO_61 to 0
	fclose(GPIOFile);

		// Set GPIO_44 output to 0 (turn off right digit)
	GPIOFile = fopen(gpio44_outpath, "w");
	if (GPIOFile == NULL) {
		printf("ERROR OPENING %s.\n", gpio44_outpath);
		return false;
	}
	fprintf(GPIOFile, "0");  // Set GPIO_44 to 0
	fclose(GPIOFile);


	//----------------------------------------------------------------
	// Initialize I2C interface to control GPIO outputs for 2x15 segmented LED display
	//----------------------------------------------------------------
		// Open I2C driver
	i2cFileDesc = open("/dev/i2c-1", O_RDWR);
	if (i2cFileDesc < 0) {
		printf("I2C DRV: Unable to open bus for read/write (/dev/i2c-1)\n");
		return false;
	}

		// Set slave address to 0x20
	int result = ioctl(i2cFileDesc, I2C_SLAVE, 0x20);
	if (result < 0) {
		printf("Unable to set I2C device to slave address to 0x20.\n");
		return false;
	}

		// Set registers 0x00 and 0x01 to 0x00.
	unsigned char buff[2];
	buff[0] = 0x00;
	buff[1] = 0x00;
	result = write(i2cFileDesc, buff, 2);
	if (result != 2) {
		printf("ERROR!  Unable to write i2c register\n");
		return false;
	}

	buff[0] = 0x01;
	result = write(i2cFileDesc, buff, 2);
	if (result != 2) {
		printf("ERROR!  Unable to write i2c register\n");
		return false;
	}

	close(i2cFileDesc);

	return true;  // success
}

//*****************************************************
// Converts new number to left and right digits
// Return value: void
//*****************************************************
void zenSegDisplayUpdateNum(int newNum)
{
	pthread_mutex_lock(&zenDisplay_mutex);

	// Make sure newNum between [0 - 99]
	// Put left digit in temp[0], right digit in temp[1]
	int temp[2];
	if (newNum < 0) {
        temp[SEGDIGIT_LEFT] = 0;
        temp[SEGDIGIT_RIGHT] = 0;
	}
	else if ( newNum < 10) {
        temp[SEGDIGIT_LEFT] = 0;
        temp[SEGDIGIT_RIGHT] = newNum;
    }
	else if (newNum > 99) {
		temp[SEGDIGIT_LEFT] = 9;
		temp[SEGDIGIT_RIGHT] = 9;
	}
	else
	{
		unsigned int digit = count(newNum);
		while (digit--) {
			temp[digit] = newNum % 10;
			newNum /= 10;
		}
    }

	// Update left and right , segDisplayThread will automatically read them
	leftDigitNum = temp[SEGDIGIT_LEFT];
	rightDigitNum = temp[SEGDIGIT_RIGHT];

	pthread_mutex_unlock(&zenDisplay_mutex);
}

void zenSegDisplayStart()
{
	// Start the thread
	pthread_create(&zenDisplay_id, NULL, &segDisplayThread, NULL);
}

void zenSegDisplayStop()
{
	zenSegDisplayDigitOff(SEGDIGIT_BOTH);
	pthread_join(zenDisplay_id, NULL);
}