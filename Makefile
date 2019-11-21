# Makefile for building embedded application.

OUTFILE1 = miniGamesConsole
OUTDIR = $(HOME)/cmpt433/public/myApps

CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror

# Default is "make all"
all: miniGamesConsole node done

miniGamesConsole:
	@echo "Building miniGamesConsole"
	@echo " "
	$(CC_C) $(CFLAGS) miniGamesConsole.c snakeGame.c balanceGame.c catchGame.c simonGame.c floppyBirbGame.c bbg_led.c udpServer1.c ext_8x8led.c ext_photoresistor.c ext_buttons.c zen_buzzer.c zen_joystick.c zen_accelerometer.c zen_segdisplay.c zen_potentiometer.c -pthread -o $(OUTDIR)/$(OUTFILE1)
	@echo " "

node:
	mkdir -p $(OUTDIR)/miniGamesConsole-server-copy/
	cp -R server1/* $(OUTDIR)/miniGamesConsole-server-copy/
	cd $(OUTDIR)/miniGamesConsole-server-copy/ && npm install

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

