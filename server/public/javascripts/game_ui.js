"use strict";
// Client-side interactions with the browser.

// Web sockets: automatically establish a socket with the server
var socket = io.connect();

// Make connection to server when web page is fully loaded.
$(document).ready(function() {
	// Make the text-entry box have keyboard focus.
	$('#send-command').focus();
	
	// Allow sending the form (pressing enter).
	$('#send-form').submit(function() {
		readUserInput();
		
		// Return false to show we have handleded it.
		return false;
	});

	$('#brightnessUp').click(function() {
		AddBrightness();
		return false;
	});

	$('#brightnessDown').click(function() {
		MinusBrightness();
		return false;
	});

	$('#new-game').click(function() {
		$('#messages').html('');
		return false;
	});

	$('#reset').click(function() {
		$('#status').html('');
		return false;
	});



	// Handle data coming back from the server
	socket.on('endOutput', function(result) {
		$('#messages').append(divMessage(result));
		$('#status').append(listMessage(result));
	});

	socket.on('getBrightness', function(result) {
		$('#messages').append(divMessage('Brightness : ' + result));
		$('#brightness').val(result);
	});

	socket.on('getCurrentScore', function(result) {
		$('#score').val(result);
	});

	
	socket.on('daError', function(result) {
		var msg = divMessage('SERVER ERROR: ' + result);
		$('#messages').append(msg);
	});
	
});

function readUserInput() {
	// Get the user's input from the browser.
	var message = $('#send-command').val();
	
	// Display the command in the message list.
	$('#messages').append(divMessage(message));

	// Process the command
	var systemMessage = processCommand(message);
	if (systemMessage != false) {
		$('#messages').append(divMessage(systemMessage));
	}
	
	// Scroll window.
	$('#messages').scrollTop($('#messages').prop('scrollHeight'));
	
	// Clear the user's command (ready for next command).
	$('#send-command').val('');
	
}

function AddBrightness() {
	var value = $('#brightness').val();
	var temp = Number(value);
	if (temp == NaN) {
		return;
	}
	value = temp + 1;
	if (value > 100) {
		value = 100;
	}	
	$('#brightness').val(value);
}

function MinusBrightness() {
	var value = $('#brightness').val();
	var temp = Number(value);
	if (temp == NaN) {
		return;
	}
	value = temp - 1;
	if (value < 0) {
		value = 0;
	}	
	$('#brightness').val(value);
}

function processCommand(command) {
	var words = command.split(' ');
	var returnMessage = false;
	var scoreWord = Number(words[0]);

	if (words[0] == 'over') {
		var score = Number(words[1]);
		var today = new Date();
		var date = today.getFullYear()+'-'+(today.getMonth()+1)+'-'+today.getDate();
		var time = today.getHours() + ":" + today.getMinutes() + ":" + today.getSeconds();
		var dateTime = date+' '+time;
		// var name = words[2];
		// if (name == null || name == '') {
		// 	return returnMessage;
		// }
		// console.log("Gets here.\n");
		var name = window.prompt("Please enter your name", "John Smith");
		if (name == null || name == "") {
			return returnMessage;
		} 

		returnMessage = name + ' ' + score + ' ' + dateTime;
		socket.emit('endRound', returnMessage);
		
	} else if (words[0] == 'brightness') {
		// console.log("Gets here.\n");
		var value = Number(words[1]);
		if (value == NaN) {
			return returnMessage;
		}
		if (value > 100) {
			value = 100;
		}
		if (value < 0) {
			value = 0;
		}
		//console.log("Gets here.\n");
		socket.emit('setBrightness', value);

	} else if (Number.isInteger(scoreWord)) {
		socket.emit('setCurrentScore', scoreWord);
	} else {
		returnMessage = 'Unrecognized command.'
	}

	return returnMessage;
}

//Wrap a string in a new <div> tag
function divMessage(inString) {
	return $('<div></div>').text(inString);
}

function listMessage(inString) {
	return $('<li></li>').text(inString);
}

