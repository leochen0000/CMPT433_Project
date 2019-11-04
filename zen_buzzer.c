#include "zen_buzzer.h"

//------------ variables and definitions -----------------------

//------------ functions ---------------------------------------
//***** static functions ******************************

//***** public functions ******************************

// Initialize Zen buzzer.
void zenBuzzerInit ()
{
	FILE *PWMFile;
    struct timespec reqDelay;
    reqDelay.tv_sec = 0;
    reqDelay.tv_nsec = 300000000;  // 300ms

	// Export PWM channel 0 of PWMCHIP2
	PWMFile = fopen("/sys/class/pwm/pwmchip0/export", "w");
	if (PWMFile == NULL) {
		printf("ERROR!  Cannot open /sys/class/pwm/pwmchip0/export for writing!\n");
		exit(1);
	}
	fprintf(PWMFile, "0");
	fclose(PWMFile);
	nanosleep(&reqDelay, (struct timespec *) NULL);

	// Turn buzzer on and off briefly
	zenBuzzerOn(10000);
	zenBuzzerOff();
}


// Turn on the buzzer with a specific frequency (Hz).
void zenBuzzerOn(long freq)
{
	FILE *PWMFile;

	// Set the duty cycle to 0 first
	PWMFile = fopen("/sys/class/pwm/pwmchip0/pwm0/duty_cycle", "w");
	if (PWMFile == NULL) {
		printf("ERROR!  Cannot open /sys/class/pwm/pwmchip0/pwm0/duty_cycle for writing!\n");
		exit(1);
	}
	fprintf(PWMFile, "0");
	fclose(PWMFile);

	// Set the period
	long period;
	period = 1000000000;  // ns/s
	period = (period + (freq>>1))/freq;
	PWMFile = fopen("/sys/class/pwm/pwmchip0/pwm0/period", "w");
	if (PWMFile == NULL) {
		printf("ERROR!  Cannot open /sys/class/pwm/pwmchip0/pwm0/period for writing!\n");
		exit(1);
	}
	fprintf(PWMFile, "%ld", period);
	fclose(PWMFile);

	// Set the duty cycle (period/2)
	PWMFile = fopen("/sys/class/pwm/pwmchip0/pwm0/duty_cycle", "w");
	if (PWMFile == NULL) {
		printf("ERROR!  Cannot open /sys/class/pwm/pwmchip0/pwm0/duty_cycle for writing!\n");
		exit(1);
	}
	fprintf(PWMFile, "%ld", period/2);
	fclose(PWMFile);

	// Turn on buzzer
	PWMFile = fopen("/sys/class/pwm/pwmchip0/pwm0/enable", "w");
	if (PWMFile == NULL) {
		printf("ERROR!  Cannot open /sys/class/pwm/pwmchip0/pwm0/enable for writing!\n");
		exit(1);
	}
	fprintf(PWMFile, "1");
	fclose(PWMFile);
}


// Turn on the buzzer with a specific frequency (Hz) and duration (ms).
void zenBuzzerBeep(long freq, long duration)
{
	FILE *PWMFile;

	// Set the duty cycle to 0 first
	PWMFile = fopen("/sys/class/pwm/pwmchip0/pwm0/duty_cycle", "w");
	if (PWMFile == NULL) {
		printf("ERROR!  Cannot open /sys/class/pwm/pwmchip0/pwm0/duty_cycle for writing!\n");
		exit(1);
	}
	fprintf(PWMFile, "0");
	fclose(PWMFile);

	// Set the period
	long period;
	period = 1000000000;  // ns/s
	period = (period + (freq>>1))/freq;
	PWMFile = fopen("/sys/class/pwm/pwmchip0/pwm0/period", "w");
	if (PWMFile == NULL) {
		printf("ERROR!  Cannot open /sys/class/pwm/pwmchip0/pwm0/period for writing!\n");
		exit(1);
	}
	fprintf(PWMFile, "%ld", period);
	fclose(PWMFile);

	// Set the duty cycle (period/2)
	PWMFile = fopen("/sys/class/pwm/pwmchip0/pwm0/duty_cycle", "w");
	if (PWMFile == NULL) {
		printf("ERROR!  Cannot open /sys/class/pwm/pwmchip0/pwm0/duty_cycle for writing!\n");
		exit(1);
	}
	fprintf(PWMFile, "%ld", period/2);
	fclose(PWMFile);

	// Turn on buzzer
	PWMFile = fopen("/sys/class/pwm/pwmchip0/pwm0/enable", "w");
	if (PWMFile == NULL) {
		printf("ERROR!  Cannot open /sys/class/pwm/pwmchip0/pwm0/enable for writing!\n");
		exit(1);
	}
	fprintf(PWMFile, "1");
	fclose(PWMFile);

	// Delay for duration
    struct timespec reqDelay;
	if (duration >= 1000) {
    	reqDelay.tv_sec = duration / 1000;
	    reqDelay.tv_nsec = (duration % 1000) * 1000000;
	}
	else {
    	reqDelay.tv_sec = 0;
	    reqDelay.tv_nsec = duration * 1000000;
	}
	nanosleep(&reqDelay, (struct timespec *) NULL);

	// Turn off buzzer
	PWMFile = fopen("/sys/class/pwm/pwmchip0/pwm0/enable", "w");
	if (PWMFile == NULL) {
		printf("ERROR!  Cannot open /sys/class/pwm/pwmchip0/pwm0/enable for writing!\n");
		exit(1);
	}
	fprintf(PWMFile, "0");
	fclose(PWMFile);

}


// Turn off the buzzer.
void zenBuzzerOff()
{
	FILE *PWMFile;

	PWMFile = fopen("/sys/class/pwm/pwmchip0/pwm0/enable", "w");
	if (PWMFile == NULL) {
		printf("ERROR!  Cannot open /sys/class/pwm/pwmchip0/pwm0/enable for writing!\n");
		exit(1);
	}
	fprintf(PWMFile, "0");
	fclose(PWMFile);
}


