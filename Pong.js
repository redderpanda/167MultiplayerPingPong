var canvas = document.createElement('canvas');
var width = 400;
var height = 600;
canvas.width = width;
canvas.height = height;
var context = canvas.getContext('2d');


var conn;

var animate =  window.requestAnimationFrame ||
window.webkitRequestAnimationFrame ||
window.mozRequestAnimationFrame ||
function(callback) {window.setTimeout(callback,1000/60)};

window.onload = function(){
	document.body.appendChild(canvas);
	animate(step);
};

var step = function(){
	update();
	render();
	animate(step);
};

var update = function(){
};

var render = function(){
	context.fillStyle = "#00FBF2";
	context.fillRect(0,0,width,height);
};

this.disconnect = function() {
	this.conn.close();
};

this.connect = function() {
	if ( typeof(MozWebSocket) == 'function' )
		this.conn = new MozWebSocket(url);
	else
		this.conn = new WebSocket(url);

	// dispatch to the right handlers
	this.conn.onmessage = function(evt){
		dispatch('message', evt.data);
	};

	this.conn.onclose = function(){dispatch('close',null)}
	this.conn.onopen = function(){dispatch('open',null)}
};
