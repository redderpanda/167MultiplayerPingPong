#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include "websocket.h"
#include <thread>
#include <chrono>
#include <vector>
#include <random>

using namespace std::chrono;
using namespace std;
string ID_array[4];
int Player_Count = 0;


webSocket server;

struct Vector2 {
	int x;
	int y;
};

struct Ball {
	Vector2 position;
	Vector2 speed;
	int lastHit = 0;
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

Player player3;

Player player4;

Player player_array[] = {player1, player2, player3, player4};

GameState LocalGamestate;

//dealing with latency
std::vector<std::pair<long long,std::pair<long long, string>>> message_Vector; //vector of commands and times

//fixed latency value
long long s_latency = 2000;
//rand latency min and max
long long r_latency_min = 1000;
long long r_latency_max = 3000;
//incremental latency value
long long i_latency_cur = 1000;
long long i_latency_max = 3000;

long long test_latency_num;
bool send_latency_pack = false;

std::default_random_engine generator;
std::uniform_int_distribution<int> distribution(1000,3000);


//later change to function that looks through clientIDList to see if all players are present
bool allConnected = false;

void Score() {
	if (LocalGamestate.gameBall.lastHit == 1) {
		player1.score += 1;
	}
	else if (LocalGamestate.gameBall.lastHit == 2) {
		player2.score += 1;
	}
	else if (LocalGamestate.gameBall.lastHit == 3) {
		player3.score += 1;
	}
	else if (LocalGamestate.gameBall.lastHit == 4) {
		player4.score += 1;
	}
	else {
		//do nothing
	}
}

void playerUpdate_Pos(Player & player) {
	player.paddle.position.x += player.paddle.speed.x;
	player.paddle.position.y += player.paddle.speed.y;
	player.paddle.speed.x = 0;
	player.paddle.speed.y = 0;

	if (player.paddle.width == 50) {
		if (player.paddle.position.x < 0) {
			player.paddle.position.x = 0;
			player.paddle.speed.x = 0;
		}
		else if (player.paddle.position.x + player.paddle.width > 600) {
			player.paddle.position.x = 600 - player.paddle.width;
			player.paddle.speed.x = 0;
		}
	}
	
	if (player.paddle.width == 10) {
		if (player.paddle.position.y < 0) {
			player.paddle.position.y = 0;
			player.paddle.speed.y = 0;
		}
		else if (player.paddle.position.y + player.paddle.height > 600) {
			player.paddle.position.y = 600 - player.paddle.height;
			player.paddle.speed.y = 0;
		}
	}
}

void setPlayerPos1(Player player) {
	player.paddle.position.x = 270;
	player.paddle.position.y = 580;
	//cout << "Player Position Set";
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

	if (top_x < 0) //hit left wall player 4
	{
		LocalGamestate.gameBall.speed.y = 0;
		LocalGamestate.gameBall.speed.x = -3;

		LocalGamestate.gameBall.position.x = 300;
		LocalGamestate.gameBall.position.y = 300;

		Score();
		LocalGamestate.gameBall.lastHit = 0;
	}
	else if (bottom_x > 600) //hit right wall player 3
	{
		LocalGamestate.gameBall.speed.y = 0;
		LocalGamestate.gameBall.speed.x = 3;

		LocalGamestate.gameBall.position.x = 300;
		LocalGamestate.gameBall.position.y = 300;

		Score();
		LocalGamestate.gameBall.lastHit = 0;
	}
	else if (top_y < 0) //hit top wall player 2
	{
		LocalGamestate.gameBall.speed.x = 0;
		LocalGamestate.gameBall.speed.y = -3;

		LocalGamestate.gameBall.position.x = 300;
		LocalGamestate.gameBall.position.y = 300;

		Score();
		LocalGamestate.gameBall.lastHit = 0;
	}

	if (LocalGamestate.gameBall.position.y > 600) //hit bot wall player 1
	{
		LocalGamestate.gameBall.speed.x = 0;
		LocalGamestate.gameBall.speed.y = 3;

		LocalGamestate.gameBall.position.x = 300;
		LocalGamestate.gameBall.position.y = 300;

		Score();
		LocalGamestate.gameBall.lastHit = 0;
	}

	if (top_y > 300) 
	{	//player1
		if (top_y < (player1.paddle.position.y + player1.paddle.height) && bottom_y > player1.paddle.position.y
			&& top_x < (player1.paddle.position.x + player1.paddle.width) && bottom_x > player1.paddle.position.x)
		{
			LocalGamestate.gameBall.speed.y = -3;
			if (LocalGamestate.gameBall.position.x > (player1.paddle.position.x + 25)) {
				//right
				LocalGamestate.gameBall.speed.x +=  5;
			}
			if (LocalGamestate.gameBall.position.x < (player1.paddle.position.x + 25)) {
				//left
				LocalGamestate.gameBall.speed.x += -5;
			}
			LocalGamestate.gameBall.position.y += LocalGamestate.gameBall.speed.y;
			LocalGamestate.gameBall.lastHit = 1;
		}
	}
	else {//player2
		if (top_y < (player2.paddle.position.y + player2.paddle.height) && bottom_y > player2.paddle.position.y
			&& top_x < (player2.paddle.position.x + player2.paddle.width) && bottom_x > player2.paddle.position.x)
		{
			LocalGamestate.gameBall.speed.y = 3;
			if (LocalGamestate.gameBall.position.x > (player2.paddle.position.x + 25)) {
				//right
				LocalGamestate.gameBall.speed.x += 5;
			}
			if (LocalGamestate.gameBall.position.x < (player2.paddle.position.x + 25)) {
				//left
				LocalGamestate.gameBall.speed.x += -5;
			}
			//LocalGamestate.gameBall.speed.x += (player2.paddle.speed.x / 2);
			LocalGamestate.gameBall.position.y += LocalGamestate.gameBall.speed.y;
			LocalGamestate.gameBall.lastHit = 2;
		}
	}

	if (top_x > 300)
	{ //player 3
		if (top_y < (player3.paddle.position.y + player3.paddle.height) && bottom_y > player3.paddle.position.y
			&& top_x < (player3.paddle.position.x + player3.paddle.width) && bottom_x > player3.paddle.position.x)
		{
			LocalGamestate.gameBall.speed.x = -3;
			if (LocalGamestate.gameBall.position.y > (player3.paddle.position.y + 25)) {
				//right
				LocalGamestate.gameBall.speed.y += 5;
			}
			if (LocalGamestate.gameBall.position.y < (player3.paddle.position.y + 25)) {
				//left
				LocalGamestate.gameBall.speed.y += -5;
			}
			LocalGamestate.gameBall.position.x += LocalGamestate.gameBall.speed.x;
			LocalGamestate.gameBall.lastHit = 3;
		}
	}
	else {
		//player 4
		if (top_y < (player4.paddle.position.y + player4.paddle.height) && bottom_y > player4.paddle.position.y
			&& top_x < (player4.paddle.position.x + player4.paddle.width) && bottom_x > player4.paddle.position.x)
		{
			LocalGamestate.gameBall.speed.x = 3;
			if (LocalGamestate.gameBall.position.y > (player4.paddle.position.y + 25)) {
				//right
				LocalGamestate.gameBall.speed.y += 5;
			}
			if (LocalGamestate.gameBall.position.y < (player4.paddle.position.y + 25)) {
				//left
				LocalGamestate.gameBall.speed.y += -5;
			}
			LocalGamestate.gameBall.position.x += LocalGamestate.gameBall.speed.x;
			LocalGamestate.gameBall.lastHit = 4;
		}
	}
}

void update() {
	//playerUpdate_Pos(player1,SOME_RECIEVED_VALUE, SOME_RECIEVED_VALUE);
	playerUpdate_Pos(player1);
	playerUpdate_Pos(player2);
	playerUpdate_Pos(player3);
	playerUpdate_Pos(player4);
	//cout << "-------player pos--------" + std::to_string(player1.paddle.position.x) + std::to_string(player1.paddle.position.y);
	ball_update();
}

void parse_player_message(string message) {
	//goes in message handler
	
}



//std::string form_state_message() {
//}

/* called when a client connects */
void openHandler(int clientID){
	if (clientID == 0) {
		//setPlayerPos1(player1);
		player1.paddle.position.x = 270;
		player1.paddle.position.y = 580;
		player1.score = 0;
		//cout << std::to_string(player1.paddle.position.x) + "--------" + std::to_string(player1.paddle.position.y);
		Player_Count += 1;
		setBall(300,300,0,3);
		//std::cout << "why";
	}
	if (clientID == 1) {
		//setPlayerPos1(player1);
		player2.paddle.position.x = 270;
		player2.paddle.position.y = 20;
		player2.score = 0;
		//cout << std::to_string(player1.paddle.position.x) + "--------" + std::to_string(player1.paddle.position.y);
		Player_Count += 1;
		//setBall(300, 300, 0, 3);
	}
	if (clientID == 2) {
		//setPlayerPos1(player1);
		player3.paddle.height = 50;
		player3.paddle.width = 10;
		player3.paddle.position.x = 580;
		player3.paddle.position.y = 270;
		player3.score = 0;
		//cout << std::to_string(player1.paddle.position.x) + "--------" + std::to_string(player1.paddle.position.y);
		Player_Count += 1;
		//setBall(300, 300, 0, 3);
		//allConnected = true;
	}
	if (clientID == 3) {
		//setPlayerPos1(player1);
		player4.paddle.height = 50;
		player4.paddle.width = 10;
		player4.paddle.position.x = 20;
		player4.paddle.position.y = 270;
		player4.score = 0;
		//cout << std::to_string(player1.paddle.position.x) + "--------" + std::to_string(player1.paddle.position.y);
		Player_Count += 1;
		//setBall(300, 300, 0, 3);
	}
	if (Player_Count == 4) {
		allConnected = true;
	}
}


/* called when a client disconnects */
void closeHandler(int clientID){
	Player_Count -= 1;
	if (Player_Count < 4) {
		allConnected = false;
	}
}


/* called when a client sends a message to the server */
void messageHandler(int clientID, string message){
	//set paddle/player values
	//string move = message.substr(0, 2);
	//string time_sent_in_miliseconds = message.substr(2,13);
	//Add instructions to queue, along with time to send
	//In periodic handler look through queue and then send 
	//the relevant packets back
	//string name = message.substr(3, message.length());
	
	string move = message.substr(0, message.find_first_of(':'));
	message.erase(0,message.find_first_of(':') + 1);
	string time = message.substr(0, message.find_first_of(':'));
	message.erase(0, message.find_first_of(':') + 1);
	string name = message.substr(0, message.length());

	milliseconds ms = duration_cast<milliseconds>(
		system_clock::now().time_since_epoch());
	long long ms_current = ms.count();

	long long send_from_time = std::stoll(time);


	//standard latency here--------------------------------------------------------------------------------------------------
	long long long_time = ms_current + (s_latency - ((ms_current - send_from_time) * 2));

	//random latency here----------------------------------------------------------------------------------------------------
	//long long r_latency_current = distribution(generator); //rand num
	//long long long_time = ms_current + (r_latency_current - ((ms_current - send_from_time) * 2));

	//incremental latency here------------------------------------------------------------------------------------------------
	//long long long_time = ms_current + (i_latency_cur - ((ms_current - send_from_time) * 2));


	//std::cout << to_string(long_time) + "<<< Long Time";





	if (allConnected) {
		if (clientID == 0) {
			if (stoi(move) == 65) {
				//left
				//cout << "moved left";
				message_Vector.push_back(std::make_pair(long_time, std::make_pair(send_from_time,"p1.-10")));
			}else if (stoi(move) == 68) {
				//right
				//cout << "moved right";
				message_Vector.push_back(std::make_pair(long_time, std::make_pair(send_from_time, "p1.+10")));
			}

			player1.name = name;
			//std::cout << name;
		}

		if (clientID == 1) {
			if (stoi(move) == 65) {
				//left
				//cout << "moved left";
				message_Vector.push_back(std::make_pair(long_time, std::make_pair(send_from_time, "p2.-10")));
			}
			else if (stoi(move) == 68) {
				//right
				//cout << "moved right";
				message_Vector.push_back(std::make_pair(long_time, std::make_pair(send_from_time, "p2.+10")));
			}

			player2.name = name;
			//std::cout << name;
		}

		if (clientID == 2) {
			if (stoi(move) == 87) {
				//up
				//cout << "moved left";
				message_Vector.push_back(std::make_pair(long_time, std::make_pair(send_from_time, "p3.-10")));
			}
			else if (stoi(move) == 83) {
				//down
				//cout << "moved right";
				message_Vector.push_back(std::make_pair(long_time, std::make_pair(send_from_time, "p3.+10")));
			}

			player3.name = name;
			//std::cout << name;
		}

		if (clientID == 3) {
			if (stoi(move) == 87) {
				//up
				//cout << "moved left";
				message_Vector.push_back(std::make_pair(long_time, std::make_pair(send_from_time, "p4.-10")));
			}
			else if (stoi(move) == 83) {
				//down
				//cout << "moved right";
				message_Vector.push_back(std::make_pair(long_time, std::make_pair(send_from_time, "p4.+10")));
			}

			player4.name = name;
			//std::cout << name;
		}
	}



    /*ostringstream os;
    os << "Stranger " << clientID << " says: " << message;*/

    /*vector<int> clientIDs = server.getClientIDs();*/

    /*for (int i = 0; i < clientIDs.size(); i++){
        if (clientIDs[i] != clientID)
            server.wsSend(clientIDs[i], os.str());
    }*/
}


std::string form_packet(long long send_from_time) {
	std::string update_string =
		std::to_string(LocalGamestate.gameBall.position.x) + ":" + std::to_string(LocalGamestate.gameBall.position.y) +
		":" + std::to_string(LocalGamestate.gameBall.speed.x) + ":" + std::to_string(LocalGamestate.gameBall.speed.y) + ":" +

		std::to_string(player1.paddle.position.x) + ":" + std::to_string(player1.paddle.position.y) + ":" + std::to_string(player1.paddle.speed.x)
		+ ":" + std::to_string(player1.paddle.speed.y) + ":" + std::to_string(player1.score) + ":" + player1.name + ":" +

		std::to_string(player2.paddle.position.x) + ":" + std::to_string(player2.paddle.position.y) + ":" + std::to_string(player2.paddle.speed.x)
		+ ":" + std::to_string(player2.paddle.speed.y) + ":" + std::to_string(player2.score) + ":" + player2.name

		+ ":" + std::to_string(player3.paddle.position.x) + ":" + std::to_string(player3.paddle.position.y) + ":" + std::to_string(player3.paddle.speed.x)
		+ ":" + std::to_string(player3.paddle.speed.y) + ":" + std::to_string(player3.score) + ":" + player3.name

		+ ":" + std::to_string(player4.paddle.position.x) + ":" + std::to_string(player4.paddle.position.y) + ":" + std::to_string(player4.paddle.speed.x)
		+ ":" + std::to_string(player4.paddle.speed.y) + ":" + std::to_string(player4.score) + ":" + player4.name + ":"
		+ std::to_string(send_from_time);

	return update_string;
}

/* called once per select() loop */
void periodicHandler() {
	static auto next = std::chrono::system_clock::now() + std::chrono::milliseconds(1000);
	auto current = std::chrono::system_clock::now();
	
	if (current >= next) {
		if (allConnected) {

			milliseconds ms = duration_cast<milliseconds>(
				system_clock::now().time_since_epoch());
			long long ms_out = ms.count();

			//change incremental latency;
			if (i_latency_cur < i_latency_max) {
				i_latency_cur += 1;
			}

			std::cout << std::to_string(i_latency_cur) + '\n';
			
			

			//check vector for message times, and update accordingly
			for (int i = 0; i < message_Vector.size(); i++) {
				if (message_Vector[i].first <= ms_out) {
					string command = message_Vector[i].second.second;
					//to_erase.push_back(i);

					send_latency_pack = true;

					if (command == "p1.-10") {
						player1.paddle.speed.x = -10;
						//break;
					}
					if (command == "p1.+10") {
						player1.paddle.speed.x = 10;
						//break;
					}
					if (command == "p2.-10") {
						player2.paddle.speed.x = -10;
						//break;
					}
					if (command == "p2.+10") {
						player2.paddle.speed.x = 10;
						//break;
					}
					if (command == "p3.-10") {
						player3.paddle.speed.y = -10;
						//break;
					}
					if (command == "p3.+10") {
						player3.paddle.speed.y = 10;
						//break;
					}
					if (command == "p4.-10") {
						player4.paddle.speed.y = -10;
						//break;
					}
					if (command == "p4.+10") {
						player4.paddle.speed.y = 10;
						//break;
					}

					//std::string time_est_pack = form_packet(message_Vector[i].second.first);

					test_latency_num = message_Vector[i].second.first;

					message_Vector.erase(message_Vector.begin() + i);
					i--;
					
					/*ostringstream us;
					us << time_est_pack;
					vector<int> clientIDs = server.getClientIDs();
					for (int i = 0; i < clientIDs.size(); i++)
						server.wsSend(clientIDs[i], us.str());*/

				}	
			}
			/*for (auto i = message_Vector.begin(); i != message_Vector.end();) {
				if ((*i).first <= ms_out) {
					message_Vector.erase(message_Vector.begin());
				}
			
			}*/

			if (send_latency_pack) {
				update();

				std::string time_est_pack = form_packet(test_latency_num);

				//cout << to_string(test_latency_num ) + "\n";

				ostringstream us;
				us << time_est_pack;
				vector<int> clientIDs = server.getClientIDs();
				for (int i = 0; i < clientIDs.size(); i++)
					server.wsSend(clientIDs[i], us.str());

				
			}
			else {
				update();

				std::string update_string = form_packet(0);

				

				ostringstream os;
				os << update_string;
				vector<int> clientIDs = server.getClientIDs();
				for (int i = 0; i < clientIDs.size(); i++)
					server.wsSend(clientIDs[i], os.str());

				
			}

			//Update Gamestate Periodically
			send_latency_pack = false;

			
		
			//Send Updated Message To Clients
			//ballinfo,playerinfo,score
			//std::string update_string = form_packet(test_latency_num);
			//using namespace std::chrono;
			
			//std::cout << ms_out;

			

			next = std::chrono::system_clock::now() + std::chrono::milliseconds(100);
		}
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
