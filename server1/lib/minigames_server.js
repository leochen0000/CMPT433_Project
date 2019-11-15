"use strict";
/*
 * Relay commands and responses between web client and miniGamesConsole program
 */
// ---- Commands received from web client ----------
// 'dispbuff' = get 8x8 display buffer
// 'ledbrightness' = get "LED brightness" status
// 'gamerun' = get "game running" status
// 'gameselected' = get "game selected" status
// -------------------------------------------------

// ---- Commands/Responses sent to web client ------
// 'dispbuff' = 8-byte response of requested 'dispbuff' command
// 'ledbrightness' = 2-digit hex string
// 'gamerun' = 'YES' or 'NO' response to 'gamerun' command
// 'gameselected' = name of game selected
// -------------------------------------------------

var fs   = require('fs');
var socketio = require('socket.io');
var dgram = require('dgram');
var io;

exports.listen = function(server) {
	io = socketio.listen(server);
	io.set('log level 1');
	
	io.sockets.on('connection', function(socket) {
		handleCommand(socket);
	});
};

function handleCommand(socket) {

	// Handle 'dispbuff' command
	socket.on('dispbuff', function(dispbuffName) {
		relayminiGamesConsoleCommand(socket, 'dispbuff', 'none');
	});

	// Handle 'gamerun' command
	socket.on('gamerun', function(gamerunName) {
		relayminiGamesConsoleCommand(socket, 'gamerun', 'none');
	});

	// Handle 'gameselected' command
	socket.on('gameselected', function(gameselectedName) {
		relayminiGamesConsoleCommand(socket, 'gameselected', 'none');
	});

	// Handle 'ledbrightness' command
	socket.on('ledbrightness', function(ledbrightnessName) {
		relayminiGamesConsoleCommand(socket, 'ledbrightness', 'none');
	});

};

// Function to relay command from web client to miniGamesConsole,
// and response from miniGamesConsole back to web client.

function relayminiGamesConsoleCommand(socket, cmd, parm) {
	// Info for connecting to the local process via UDP
	var PORT = 12345;
	var HOST = '127.0.0.1';
	var buffer = new Buffer(cmd + ' ' + parm);

	var client = dgram.createSocket('udp4');
	client.send(buffer, 0, buffer.length, PORT, HOST, function(err, bytes) {
		if (err) 
			throw err;
		console.log('UDP message sent to ' + HOST +':'+ PORT);
	});

	client.on('listening', function () {
		var address = client.address();
		console.log('UDP Client: listening on ' + address.address + ":" + address.port);
	});

	// Handle an incoming message over the UDP from the local application.
	client.on('message', function (message, remote) {
		console.log("UDP Client: message Rx" + remote.address + ':' + remote.port +' - ' + message);

		var reply = message.toString('utf8')
		socket.emit(cmd, reply);  // Forward to web client
		client.close();

	});

	client.on("UDP Client: close", function() {
		console.log("closed");
	});

	client.on("UDP Client: error", function(err) {
		console.log("error: ",err);
	});
}

