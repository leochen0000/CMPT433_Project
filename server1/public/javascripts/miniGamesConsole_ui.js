"use strict";
// Client-side interactions with the browser.

// ---- Commands sent to server --------------------
// 'dispbuff' = get 8x8 display buffer
// 'ledbrightness' = get "LED brightness" status
// 'gamerun' = get "game running" status
// 'gameselected' = get "game selected" status
// -------------------------------------------------

// ---- Commands/Responses from Server -------------
// 'dispbuff' = 8-byte response of requested 'dispbuff' command
// 'ledbrightness' = 2-digit hex string
// 'gamerun' = 'YES' or 'NO' response to 'gamerun' command
// 'gameselected' = name of game selected
// -------------------------------------------------

// Timers
var clearErrTimer;
var dispbuffTimer;

// Flags
var dispbuffReceived;
var gamerunReceived;
var gameselectedReceived;
var ledbrightnessReceived;

var dispbuff_not_working = 0;

var led_brightness = 'FF';

// Websocket connection to server
var socket = io.connect();

// Make connection to server when web page is fully loaded.
$(document).ready(function() {
	console.log("Document loaded");

	blackScreen();

	// Interval timer for getting minigame console status.  (100ms)
	window.setInterval(function() {updateStatus()}, 100);

	// Interval timer for checking error status (1-second)
	window.setInterval(function() {checkErrors()}, 1000);

	// --------- Handle data coming back from the server -----------------

	// Handle 'dispbuff' response
	socket.on('dispbuff', function(dispbuff) {
		render8x8Screen(dispbuff);
		dispbuffReceived = 1;
		dispbuff_not_working = 0;
		clearTimeout(dispbuffTimer);
	});
	
	// Handle 'gamerun' response
	socket.on('gamerun', function(gamerun) {
		$('#running').html(gamerun);
		gamerunReceived = 1;
	});
	
	// Handle 'gameselected' response
	socket.on('gameselected', function(gameselected) {
		$('#gameselect').html(gameselected);
		switch (gameselected) {
			case 'SIMON':
				$('#desc').html('Test your memory by repeating the patterns!');
				break;
			case 'CATCH':
				$('#desc').html('Catch the ball!');
				break;
			case 'BALANCE':
				$('#desc').html('Can you keep the BeagleBone balanced?');
				break;
			case 'SNAKE':
				$('#desc').html('Ever wished you had no arms and no legs? Try this remake of the apple-eating classic!');
				break;
			case 'FLOPPY BIRB':
				$('#desc').html('Use the joystick to flap your wings and avoid pipes!');
				break;
		}
		gameselectedReceived = 1;
	});
	
	// Handle 'ledbrightness' response
	socket.on('ledbrightness', function(ledbrightness) {
		ledbrightnessReceived = 1;
		led_brightness = ledbrightness;
	});
	
});

// Function to send commands to get status
function updateStatus() {

	// Send command to get display buffer
	dispbuffReceived == 0;
	sendminiGamesConsoleCommand('dispbuff', 'none');
	while (dispbuffReceived == 0);

	// Send command to get "game running" status
	gamerunReceived == 0;
	sendminiGamesConsoleCommand('gamerun', 'none');
	while (gamerunReceived == 0);

	// Send command to get "game selected" status
	gameselectedReceived == 0;
	sendminiGamesConsoleCommand('gameselected', 'none');
	while (gameselectedReceived == 0);

	// Send command to get "LED brightness" status
	ledbrightnessReceived == 0;
	sendminiGamesConsoleCommand('ledbrightness', 'none');
	while (ledbrightnessReceived == 0);
}

// Function to send command to miniGamesConsole
function sendminiGamesConsoleCommand(cmd, parm) {

	// Set timers to get response back from miniGamesConsole
	if (cmd == 'dispbuff') {
		dispbuffTimer = window.setTimeout(function() {dispbuffTimeout(cmd, parm)}, 2000);
	}

	socket.emit(cmd, parm);
}

// Server timeout handler (when no response from server) for 'dispbuff' command
function dispbuffTimeout(cmd, parm) {
	dispbuff_not_working = 1;
}

function blackScreen() {
	let canvas = document.querySelector("canvas");
	let context = canvas.getContext("2d");
	context.fillStyle = "black";
	context.fillRect(0, 0, 247, 247);
}



function render8x8Screen(dispbuff) {
	let canvas = document.querySelector("canvas");
	let context = canvas.getContext("2d");
	context.fillStyle = "black";
	context.fillRect(0, 0, 247, 247);

	context.fillStyle = '#00' + led_brightness + '00';
	var x, y;
	for (y = 0; y < 8; y++) {
		var rowbyte = dispbuff.substring(y*2, (y+1)*2);
		var hexval = parseInt(rowbyte, 16);
		for (x = 0; x < 8; x++) {
			if (hexval & 0x80) {
				context.beginPath();
				context.arc(x*31+15, y*31+15, 15, 0, 6.28318);
				context.fill();
			}
			hexval = hexval<<1;
		}
	}
}



// Timeout handler to check error conditions
function checkErrors() {
	var errmsg = '';
	var error = 0;

	// Check communication errors to miniGamesConsole
	if (dispbuff_not_working == 1) {
		errmsg = errmsg + 'Commands to miniGamesConsole not working: ';
		if (dispbuff_not_working == 1)
			errmsg = errmsg + '-dispbuff-';
		errmsg = errmsg + '<br>';
		error = 1;
	}

	// Make error box visible if there are errors
	if (error == 1) {
		$('#error-text').html(errmsg);
		$('#error-box').show();
		clearTimeout(clearErrTimer);
		clearErrTimer = window.setTimeout(function() {clearErrTimeout()}, 10000);
	}
	else {  // Otherwise, make error box disappear
		clearTimeout(clearErrTimer);
		$('#error-box').hide();
	}
}

// "Clear error box" timeout handler after 10 seconds
function clearErrTimeout() {
	$('#error-box').hide();
}

