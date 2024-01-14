# LogServer

 ----------------------------------------------- (ENG) -----------------------------------------------

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

There are two configuration file, one for the server, one for the client.
