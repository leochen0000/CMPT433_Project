#include "udpServer1.h"

#define MSG_MAX_LEN 1024
#define PORT        12345

//------------ variables and definitions -----------------------
static pthread_t udpServer_id;
extern _Bool game_running;
extern int game_selection;
extern char *(gamename[]);
extern unsigned char led_brightness;

//------------ functions ---------------------------------------
//***** static functions ******************************

//----------------------------------------------------
// Thread to continuously listen to UDP port and process
// commands from client.
//----------------------------------------------------
static char messageRx[MSG_MAX_LEN];

static void *udpServerThread ()
{
	// Address
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;                   // Connection may be from network
	sin.sin_addr.s_addr = htonl(INADDR_ANY);    // Host to Network long
	sin.sin_port = htons(PORT);                 // Host to Network short
	
	// Create the socket for UDP
	int socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);

	// Bind the socket to the port (PORT) that we specify
	bind (socketDescriptor, (struct sockaddr*) &sin, sizeof(sin));

	// Listening and processing loop
	while (1) {
		// Get the data (blocking)
		// Will change sin (the address) to be the address of the client.
		// Note: sin passes information in and out of call!
		struct sockaddr_in sinRemote;
		unsigned int sin_len = sizeof(sinRemote);
		int bytesRx = recvfrom(socketDescriptor,
			messageRx, MSG_MAX_LEN, 0,
			(struct sockaddr *) &sinRemote, &sin_len);

		// Make it null terminated (so string functions work):
		int terminateIdx = (bytesRx < MSG_MAX_LEN) ? bytesRx : MSG_MAX_LEN - 1;
		messageRx[terminateIdx] = 0;

		sin_len = sizeof(sinRemote);
		char *delimiters = " \r\n";  // for strtok()

		// Interpret commands
		char *token;
		token = strtok(messageRx, delimiters);

		char messageTx[20];
		if (token != NULL) {
			unsigned char dispbuff[8];

			// 'dispbuff' command
			if (strcmp(token, "dispbuff") == 0) {
				extLED8x8GetLocalBuffer(dispbuff);
				sprintf(messageTx, "%02X%02X%02X%02X%02X%02X%02X%02X",
						 dispbuff[0], dispbuff[1], dispbuff[2], dispbuff[3],
						 dispbuff[4], dispbuff[5], dispbuff[6], dispbuff[7]
						 );
				sendto( socketDescriptor, messageTx, strlen(messageTx), 0, (struct sockaddr *) &sinRemote, sin_len);
			}
			else if (strcmp(token, "gamerun") == 0) {
				if (game_running == true)
					sprintf(messageTx, "YES");
				else
					sprintf(messageTx, "NO");
				sendto( socketDescriptor, messageTx, strlen(messageTx), 0, (struct sockaddr *) &sinRemote, sin_len);
			}
			else if (strcmp(token, "gameselected") == 0) {
				sendto( socketDescriptor, gamename[game_selection], strlen(gamename[game_selection]), 0, (struct sockaddr *) &sinRemote, sin_len);
			}
			else if (strcmp(token, "ledbrightness") == 0) {
				sprintf(messageTx, "%02X", (led_brightness<<3) | 0x87);
				sendto( socketDescriptor, messageTx, strlen(messageTx), 0, (struct sockaddr *) &sinRemote, sin_len);
			}

		}
	}

	// Close
	close(socketDescriptor);
	
	pthread_exit(0);
}


//***** public functions ******************************

// Begin the background thread to read the potentiometer,
void UDPServer_startListening (void)
{
	// Start the thread
	pthread_create(&udpServer_id, NULL, &udpServerThread, NULL);
}


// End the background thread
void UDPServer_stopListening (void)
{
	// Wait for thread to finish
	pthread_join(udpServer_id, NULL);
}

