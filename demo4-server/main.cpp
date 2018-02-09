#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include "websocket.h"
#include <thread>
#include <chrono>

using namespace std;

webSocket server;

struct Vector2 {
	float x;
	float y;
};

struct Ball {
	Vector2 position;
	Vector2 speed;
};

struct Paddle {
	Vector2 position;
	int width = 50;
	int height = 10;
	Vector2 speed;
};

struct Player {
	std::string name;
	int score;
	Paddle paddle;
};

struct Arena {
	Vector2 dimensions;
};

struct GameState {
	Ball gameBall;

	/*Player player1;
	Player player2;
	Player player3;
	Player player4;*/
};



//need to set starting positions of each paddle
//Paddle paddle1;
//
//Paddle paddle2;
//
//Paddle paddle3;
//
//Paddle paddle4;

Player player1;

Player player2;

Player player_array[] = {player1};

GameState LocalGamestate;

//later change to function that looks through clientIDList to see if all players are present
bool allConnected = false;

void update() {
	//playerUpdate_Pos(player1,SOME_RECIEVED_VALUE, SOME_RECIEVED_VALUE);
	ball_update();
}

void playerUpdate_Pos(Player player, int x, int y) {
	player.paddle.position.x += player.paddle.speed.x;
	player.paddle.position.y += player.paddle.speed.y;
	player.paddle.speed.x = 0;
	player.paddle.speed.y = 0;

	if (player.paddle.position.x < 0) {
		player.paddle.position.x = 0;
		player.paddle.speed.x = 0;
	}
	else if (player.paddle.position.x + player.paddle.width > 600) {
		player.paddle.position.x = 600 - player.paddle.width;
		player.paddle.speed.x = 0;
	}
}

void setPlayerPos1(Player player) {
	player.paddle.position.x = 270;
	player.paddle.position.y = 580;
}


void setBall(int x, int y, int speed_x, int speed_y) {
	LocalGamestate.gameBall.position.x = x;
	LocalGamestate.gameBall.position.y = y;
	LocalGamestate.gameBall.speed.x = speed_x;
	LocalGamestate.gameBall.speed.y = speed_y;
}

void ball_update() {
	LocalGamestate.gameBall.position.x += LocalGamestate.gameBall.speed.x;
	LocalGamestate.gameBall.position.y += LocalGamestate.gameBall.speed.y;
	int top_x = LocalGamestate.gameBall.position.x - 5;
	int top_y = LocalGamestate.gameBall.position.y - 5;
	int bottom_x = LocalGamestate.gameBall.position.x + 5;
	int bottom_y = LocalGamestate.gameBall.position.y + 5;

	if (top_x < 0) //hit left wall
	{
		LocalGamestate.gameBall.position.x = 5;
		LocalGamestate.gameBall.speed.x = -LocalGamestate.gameBall.speed.x;

	}
	else if (bottom_x > 600) //hit right wall
	{
		LocalGamestate.gameBall.position.x = 595;
		LocalGamestate.gameBall.speed.x = -LocalGamestate.gameBall.speed.x;
	}
	else if (top_y < 0) 
	{
		LocalGamestate.gameBall.position.y = 5;
		LocalGamestate.gameBall.speed.y = -LocalGamestate.gameBall.speed.y;
	}

	if (LocalGamestate.gameBall.position.y > 600) 
	{
		LocalGamestate.gameBall.speed.x = 0;
		LocalGamestate.gameBall.speed.y = 3;

		LocalGamestate.gameBall.position.x = 300;
		LocalGamestate.gameBall.position.y = 300;
	}

	if (top_y > 300) 
	{
		if (top_y < (player1.paddle.position.y + player1.paddle.height) && bottom_y > player1.paddle.position.y
			&& top_x < (player1.paddle.position.x + player1.paddle.width) && bottom_x > player1.paddle.position.x)
		{
			LocalGamestate.gameBall.speed.y = -3;
			LocalGamestate.gameBall.speed.x += (player1.paddle.speed.x / 2);
			LocalGamestate.gameBall.position.y += LocalGamestate.gameBall.speed.y;
		}
	}
	else {
		if (top_y < (player2.paddle.position.y + player2.paddle.height) && bottom_y > player2.paddle.position.y
			&& top_x < (player2.paddle.position.x + player2.paddle.width) && bottom_x > player2.paddle.position.x)
		{
			LocalGamestate.gameBall.speed.y = 3;
			LocalGamestate.gameBall.speed.x += (player2.paddle.speed.x / 2);
			LocalGamestate.gameBall.position.y += LocalGamestate.gameBall.speed.y;
		}
	}
}

void parse_player_message() {
	//goes in message handler
}



std::string form_state_message() {
}

/* called when a client connects */
void openHandler(int clientID){
	if (clientID == 0) {
		setPlayerPos1(player1);
		allConnected = true;
		setBall(300,300,0,3);
	}
}


/* called when a client disconnects */
void closeHandler(int clientID){
}


/* called when a client sends a message to the server */
void messageHandler(int clientID, string message){
    ostringstream os;
    os << "Stranger " << clientID << " says: " << message;

    vector<int> clientIDs = server.getClientIDs();
    for (int i = 0; i < clientIDs.size(); i++){
        if (clientIDs[i] != clientID)
            server.wsSend(clientIDs[i], os.str());
    }
}


/* called once per select() loop */
void periodicHandler() {
	static auto next = std::chrono::system_clock::now() + std::chrono::milliseconds(1000);
	auto current = std::chrono::system_clock::now();
	if (current >= next) {
		//Update Gamestate Periodically
		//Send Updated Message To Clients
		ostringstream os;
		os << "The message that requires sending";
		vector<int> clientIDs = server.getClientIDs();
		for (int i = 0; i < clientIDs.size(); i++)
			server.wsSend(clientIDs[i], os.str());
		next = std::chrono::system_clock::now() + std::chrono::milliseconds(10);
	}
}


int main(int argc, char *argv[]){
    /*int port1 = 1000;
	int port2 = 2000;
	int port3 = 3000;
	int port4 = 4000;*/

	int port = 1000;

    /*cout << "Please set server port: ";
    cin >> port;*/

    /* set event handler */
    server.setOpenHandler(openHandler);
    server.setCloseHandler(closeHandler);
    server.setMessageHandler(messageHandler);
    server.setPeriodicHandler(periodicHandler);

	server.startServer(port);

}
