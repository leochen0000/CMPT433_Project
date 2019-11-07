var socketio = require('socket.io');
var io;

exports.listen = function(server) {
	io = socketio.listen(server);
	io.set('log level 1');
	
	io.sockets.on('connection', function(socket) {

		handleCommand(socket);
	});
	
	
};

function handleCommand(socket) {
	var errorTimer = setTimeout(function() {
		socket.emit("daError", "Oops: User too slow at sending first command.");
	}, 5000);
	
	socket.on('endRound', function(message) {
		 
		socket.emit('endOutput', message);
		clearTimeout(errorTimer);
	});

	socket.on('setBrightness', function(data){
		socket.emit('getBrightness', data)
	});

	socket.on('setCurrentScore', function(data){
		socket.emit('getCurrentScore', data)
	});


};


