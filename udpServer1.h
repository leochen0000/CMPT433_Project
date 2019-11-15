// udpServer.h
// Module to spawn a thread to listen to UDP port 12345
// and process commands.

#ifndef _UDPSERVER_H_
#define _UDPSERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include "ext_8x8led.h"
#include "simonGame.h"
#include "catchGame.h"
#include "snakeGame.h"
#include "balanceGame.h"

// Begin/end the background thread which listens to UDP
void UDPServer_startListening(void);
void UDPServer_stopListening(void);

#endif
