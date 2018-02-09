var animate =  window.requestAnimationFrame ||
window.webkitRequestAnimationFrame ||
window.mozRequestAnimationFrame ||function(callback) {
	window.setTimeout(callback,1000/60)};
var canvas = document.createElement('canvas');
var width = 600;
var height = 600;
canvas.width = width;
canvas.height = height;
var context = canvas.getContext('2d');
var player = new Player();
var player2 = new Player2();
var ball = new Ball(300,300);

var keysDown = {};

window.onload = function(){
	document.body.appendChild(canvas);
	animate(step);
};

var render = function(){
	context.fillStyle = "#00FBF2";
	context.fillRect(0,0,width,height);
	player.render();
	player2.render();
	ball.render();
};

var update = function(){
	player.update();
	//player2.update();
	ball.update(player.paddle, player2.paddle);
};

var step = function(){
	update();
	render();
	animate(step);
};

function Paddle(x,y,width,height)
{
	this.x = x;
	this.y = y;
	this.width = width;
	this.height = height;
	this.x_speed = 0;
	this.y_speed = 0;
}

Paddle.prototype.render = function(){
	context.fillStyle = "#000000";
	context.fillRect(this.x,this.y,this.width,this.height);
};

Paddle.prototype.move = function(x,y){
	this.x += x;
	this.y += y;
	this.x_speed = x;
	this.y_speed = y;
	if(this.x < 0){
		this.x = 0;
		this.x_speed = 0;
	}else if(this.x + this.width > 600)
	{
		this.x = 600 - this.width;
		this.x_speed = 0;
	}
};

function Player2(){
	this.paddle = new Paddle(270,10,50,10);
}

Player2.prototype.render = function()
{
	this.paddle.render();
};

function Player(){
	this.paddle = new Paddle(270,580,50,10);
}

Player.prototype.render = function(){
	this.paddle.render();
};

Player.prototype.update = function(){
	for(var key in keysDown){
		var value = Number(key);
		if(value == 37){
			this.paddle.move(-4,0);
		}else if(value == 39){
			this.paddle.move(4,0);
		}else{
			this.paddle.move(0,0);
		}
	}
};

function Ball(x,y){
	this.x = x;
	this.y = y;
	this.x_speed = 0;
	this.y_speed = 3;
}

Ball.prototype.render = function(){
	context.beginPath();
	context.arc(this.x,this.y,5,2*Math.PI,false);
	context.fillStyle  = "#000000";
	context.fill();
};

Ball.prototype.update = function(paddle1,paddle2){
	this.x += this.x_speed;
	this.y += this.y_speed;
	var top_x = this.x - 5;
	var top_y = this.y - 5;
	var bottom_x  = this.x + 5;
	var bottom_y = this.y + 5;

	if(this.x - 5 < 0){ //hit left wall
		this.x = 5;
		this.x_speed = -this.x_speed;}
		else if(this.x + 5 > 600){ //hit right wall
			this.x = 595;
			this.x_speed = -this.x_speed;
		}
		else if(this.y - 5 < 0){ // hit top wall
			this.y = 5;
			this.y_speed = -this.y_speed;
		}
	//if(this.y < 0 || this.y > 600){ //a point was scored
		if(this.y > 600){
		this.x_speed = 0;
		this.y_speed = 3;
		this.x = 300;
		this.y = 300
	}
	if(top_y > 300){
		if (top_y < (paddle1.y + paddle1.height) && bottom_y > paddle1.y && top_x < (paddle1.x + paddle1.width) && bottom_x > paddle1.x){
			this.y_speed = -3;
			this.x_speed += (paddle1.x_speed / 2);
			this.y += this.y_speed;
		}
	}else{
		if(top_y < (paddle2.y + paddle2.height) && bottom_y > paddle2.y && top_x < (paddle2.x + paddle2.width) && bottom_x > paddle2.x){
			this.y_speed = 3;
			this.x_speed +=(paddle2.x_speed / 2);
			this.y += this.y_speed;
		}
	}
};
document.body.appendChild(canvas);
animate(step);


window.addEventListener("keydown",function(event){
	keysDown[event.keyCode] = true;
});

window.addEventListener("keyup",function(event){
	delete keysDown[event.keyCode];
});

