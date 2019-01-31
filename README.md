# ASM
C++ Code for the Altitude Sensing Module

# Demo
To run the demo, ```cd``` into the project directory and run ```make```. <br>
Open 2 terminal windows. Make sure each window is ```cd```'d into the project directory.<br>
In one terminal, run ```./bin/server 4000```. This will start a TCP server listening on port 4000.<br>
In the other terminal, run ```./bin/client "127.0.0.1" 4000```. This will start a client and connect to the server.<br>
A prompt will appear like so ```Enter a message: ```.<br>Type a short message and hit enter. Observe in the other terminal window,
the window running the server, that the server has received the message from the client and echoed it back to the client. 
