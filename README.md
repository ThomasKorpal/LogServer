# LogServer

This project is a client/server log server application. It lets you send messages to a server which will store them in a log file. 

In order to execute the program, you need to :

    - Open a terminal where the program is stored
    - Execute the command "make"
    - In one terminal, execute the command "./server"
    - In another terminal, execute the command "./client"
    - Enter your name in the client terminal

You can now send messages to the log server. The name of the file is available on the server terminal (printed)

Command options : 

./client (--server_port=<port>) (--server_ip=<server_ip>)

./server (--server_port=<port>) (--log_directory=<log_directory>)

You do not need a specific order for the options, nor do you need to enter both of them in the same command.
The program will also verify the validity and the existence of the log directory, the validity of the server port and the validity of the IP address (if they're given in the options).

There are two configuration file, one for the server, one for the client. If no options are given, the values inside these files will serve as default values.

Test protocol : 

  - ./server
  - Open the file given by the server (printed in the terminal)
  - Open another terminal
  - In that terminal -> ./client
  - Enter one name
  - Send a message
  - Open another terminal
  - Execute ./client
  - Enter another name
  - Send a message
  - Quit both clients by writing "exit"
  - Quit the server by writing "exit"
  
(You'll also find a Python script called test.py which is my attempt on test scripting this program. I'm not 100% sure it works as I didn't have the time to continue working on it)
