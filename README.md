Online Ping Pong: MileStone 1

Our Program is written using C++, HTML, and Javascript.
There are certain portions of the code that require C++11 or later 
(specifically chrono for storing time in milliseconds). 
However, that shouldn't be an issue since most computers use C++11 or later.

----------------------------- About the Game -----------------------------------
This milestone of our Ping Pong game encourages players to try to continuously hit the ball
using the paddle (each time incrementing their score by 1). Everytime the player hits the 
ball their score goes up by one. If the player misses the ball, their score is reverted 
back down to 0 and the ball is recentered. In order to connect to the games server, the 
player must input a valid IP Address and then click on the "Play" button. This will allow
the players to connect and play the Ping Pong Game. This may take a couple of tries
before the player is allowed into the game. Click on the "Disconnect" button to 
exit the game.

Our game runs using client-server based architecture. The Client is in charge of sending messages 
to the server with the players' key inputs, and rendering the gui for the game. While the server is 
in charge of updating (and storing) the gamestate, periodically sending gamestate updates to all 
players, parsing messages from the client, and updating the gamestate according to what key inputs 
the clients send in.

---------------------------- How to Connect -----------------------------------------
-In order to set up the game first open the the 167MultiplayerPingPong Folder
-Open the 'demo4-server' folder in visualstudios 2017 and build the 'main.cpp' file.
-Once the file is built go into the 'demo4-server', then into 'Debug', and then find and
start the 'demo4-server.exe'.
-The window it opens will display all valid IP Addresses that can be used to connect to the
game server.
-Now, go back into the 167MultiplayerPingPong folder and open the PongClient.html page.
-Using one of the IP Addresses from the .exe window input it into the IP Server box in
the .html page.
-Click on the Play button and wait for it to connect. If not connected immediately then try
clicking the Play button again with the same IP Address (or disconnecting and re-connecting).


---------------------------- How to Play -----------------------------------------

Use the left and right arrow keys to move the paddle.

The goal of the game is to keep the ball from hitting the wall behind the player, with each 
paddle hit raising the player's score.

Hitting the ball from the left side of the paddle will make the ball move to the left.
Hitting the ball from the right side of the paddle will make the ball move to the right.

Fail to hit the ball and your score is reverted back to 0.