#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include "websocket.h"
#include <thread>

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

	Player player1;
	Player player2;
	Player player3;
	Player player4;
};



//need to set starting positions of each paddle
Paddle paddle1;

Paddle paddle2;

Paddle paddle3;

Paddle paddle4;

Paddle paddle_array[] = { paddle1,paddle2,paddle3,paddle4 };
GameState LocalGamestate;

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
}

void parse_player_message() {
	//goes in message handler
}



std::string form_state_message() {
}

/* called when a client connects */
void openHandler(int clientID){
	//forms welcome message
    /*ostringstream os;
    os << "Stranger " << clientID << " has joined.";*/

	if (clientID == 0) {
		LocalGamestate.player1.paddle.position.x = paddle_array[clientID].position.x;
		LocalGamestate.player1.paddle.position.x = paddle_array[clientID].position.y;
	}else if (clientID == 1) {
		LocalGamestate.player2.paddle.position.x = paddle_array[clientID].position.x;
		LocalGamestate.player2.paddle.position.x = paddle_array[clientID].position.y;
	}else if (clientID == 2) {
		LocalGamestate.player3.paddle.position.x = paddle_array[clientID].position.x;
		LocalGamestate.player3.paddle.position.x = paddle_array[clientID].position.y;
	}else if (clientID == 3) {
		LocalGamestate.player4.paddle.position.x = paddle_array[clientID].position.x;
		LocalGamestate.player4.paddle.position.x = paddle_array[clientID].position.y;
	}
	
	//sends welcome message to all chatroom members
    /*vector<int> clientIDs = server.getClientIDs();
    for (int i = 0; i < clientIDs.size(); i++){
        if (clientIDs[i] != clientID)
            server.wsSend(clientIDs[i], os.str());
    }
    server.wsSend(clientID, "Welcome!");*/
}


/* called when a client disconnects */
void closeHandler(int clientID){
    ostringstream os;
    os << "Stranger " << clientID << " has leaved.";

    vector<int> clientIDs = server.getClientIDs();
    for (int i = 0; i < clientIDs.size(); i++){
        if (clientIDs[i] != clientID)
            server.wsSend(clientIDs[i], os.str());
    }
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
void periodicHandler(){
	static clock_t clock_next = (clock() / CLOCKS_PER_SEC) + 5;
	static float float_next = float (clock_next);

	clock_t clock_current = clock()/CLOCKS_PER_SEC;
	float float_current = float(clock_current);

	time_t current = time(NULL);

	if (float_current >= float_next) {
		//Server Side Game Updates Go Here
		//Create Packets
		ostringstream os;
		char timecstring[26];
		ctime_s(timecstring, sizeof(timecstring), &current);
		string timestring(timecstring);
		timestring = timestring.substr(0, timestring.size() - 1);
		os << timestring;

		vector<int> clientIDs = server.getClientIDs();

		for (int i = 0; i < clientIDs.size(); i++) {
			server.wsSend(clientIDs[i], os.str());
		}


		float_next = float(clock()/CLOCKS_PER_SEC) + 0.5;
	}



  //  static time_t next = time(NULL) + 10;
  //  time_t current = time(NULL);
  //  if (current >= next){
		////Server Side Game Updates Go Here
		////Create Packets
  //      ostringstream os;
		//	char timecstring[26];
		//ctime_s(timecstring, sizeof(timecstring), &current);
		//string timestring(timecstring);
  //      timestring = timestring.substr(0, timestring.size() - 1);
  //      os << timestring;

  //      vector<int> clientIDs = server.getClientIDs();

		//for (int i = 0; i < clientIDs.size(); i++) {
		//	server.wsSend(clientIDs[i], os.str());
		//}
  //          

  //      next = time(NULL) + 0.5;
  //  }
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
