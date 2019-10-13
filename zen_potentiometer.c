#include "zen_potentiometer.h"

//------------ variables and definitions -----------------------
#define A2D_FILE_VOLTAGE0   "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"
#define A2D_VOLTAGE_REF_V   1.8
#define A2D_MAX_READING     4095


//------------ functions ---------------------------------------

//***** public functions ******************************

// Read A2D value of potentiometer.
// Return value:  0 to 4095 (inclusive), or <0 if error occurred.
int zenPotentiometerReadA2DValue ()
{
	// Open file
	FILE *A2DFile = fopen(A2D_FILE_VOLTAGE0, "r");
	if (A2DFile == NULL) {
	   printf("ERROR OPENING %s.\r\n", A2D_FILE_VOLTAGE0);
	   return -1;
	}

	// Read value
	int a2dvalue;
	int itemsread = fscanf(A2DFile, "%d", &a2dvalue);
	if (itemsread <= 0) {
		printf("ERROR reading A2D value. Too few items read.\n");
	   return -1;
	}

    // Make sure to close file
    fclose(A2DFile);

	return a2dvalue;
}


// Read voltage of potentiometer.
// Return value:  0 to 1.8V (inclusive), or <0 if error occurred.
double zenPotentiometerReadVoltage ()
{
	// Read A2D value
	int a2dvalue = zenPotentiometerReadA2DValue();
	if (a2dvalue < 0)  // check for error
		return -1.0;

	// Calculate voltage
	return((((double) a2dvalue)/A2D_MAX_READING)*A2D_VOLTAGE_REF_V);
}

