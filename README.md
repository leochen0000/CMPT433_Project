// How to set up the BeagleBone Minigame Console

	
1. Go to the main directory to find our Makefile.
2. Type: $make
3. This will automatically compile and copy the binaries to the remote beaglebone folder
4. SSH into the beaglebone
5. Go to /mnt/remote/myApps
6. Run with: #sudo ./miniGamesConsole
7. To open our website, open another SSH beaglebone terminal
8. Go to directory /mnt/remote/myApps/miniGamesConsole-server-copy
9. Run server with: #nodejs server.js
10. Go to website at 192.168.7.2:8088 


