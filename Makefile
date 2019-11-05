# Makefile for building embedded application.
# by Dane Lim

OUTFILE1 = miniGamesConsole
OUTDIR = $(HOME)/cmpt433/public/myApps

CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror

# Default is "make all"
all: miniGamesConsole done

miniGamesConsole:
	@echo "Building miniGamesConsole"
	@echo " "
	$(CC_C) $(CFLAGS) miniGamesConsole.c catchGame.c simonGame.c bbg_led.c ext_8x8led.c ext_photoresistor.c ext_buttons.c zen_buzzer.c zen_joystick.c zen_accelerometer.c zen_segdisplay.c zen_potentiometer.c -pthread -o $(OUTDIR)/$(OUTFILE1)
	@echo " "

done: 
	@echo "Finished building application."
	@echo " "
	@echo "Application location:"
	ls -l $(OUTDIR)/$(OUTFILE1)
	@echo " "

clean:
ifeq (,$(wildcard $(OUTDIR)/$(OUTFILE1)))
else
	rm $(OUTDIR)/$(OUTFILE1)
endif

help: 
	@echo "Build sorter program for BeagleBone"
	@echo "Targets include all, app, and clean."

