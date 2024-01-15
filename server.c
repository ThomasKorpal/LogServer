#include "server.h"

#define BUFFSIZE 1024
#define MAX_CLIENTS 5000

int server_port;
char log_dir[150];
int s_flag = 0;
char command[100];

sem_t sem_file;

//Function called when an error has been raised
void Die(char *mess) 
{ 
    perror(mess); 
    exit(EXIT_FAILURE); 
}

//Tells if a string is a decimal number (used for user-proofing the program)
int isANumber(const char *str) 
{
    char *endptr;
    strtol(str, &endptr, 10);
    //If str is a number, then when converted in base 10 no letter should be found. If \0 is the last caracter, all of the string has been converted and as such the string is a number
    return *endptr == '\0';
}

//Function that handles everything client related
int HandleClient(int sock, char* log_file, char* client_info) 
{
    //Opening the file is protected by a semaphore to avoid problems with multiple processes
    char bufferC[BUFFSIZE];
    sem_wait(&sem_file);
    int log = open(log_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
    sem_post(&sem_file);
    //Error raising if the program cannot open the file
    if (log == -1)
    {
        Die("Error opening log file.\n");
    }

    //Getting the current time to log a new connexion from a client
    time_t current_time;
    time(&current_time);

    //Generating the message to be logged
    char log_message[BUFFSIZE];
    snprintf(log_message,BUFFSIZE,"Client %s connected at %s", client_info, ctime(&current_time));

    //Write the message on the log file (protected by semaphores)
    sem_wait(&sem_file);
    ssize_t bytesWriteCon = write(log,log_message,strlen(log_message));
    sem_post(&sem_file);
    if(bytesWriteCon == -1)
    {
        Die("Error writing to log file.");
    }
    //Buffer reset if need to re-use later
    memset(log_message, 0, BUFFSIZE);

    while(1)
    {
        //Infinite loop to receive messages from the client
        memset(bufferC, 0, BUFFSIZE); //Buffer reset if it still has some data from a previous usage
        ssize_t bytes_received = recv(sock, bufferC, sizeof(bufferC), 0);
        if(bytes_received <= 0) 
        {
            break;
        }

        if(!strcmp(bufferC,"exit")) //If the exit command is typed on the client side, the server logs the disconnexion
        {
            time(&current_time);
            snprintf(log_message,BUFFSIZE,"Client %s disconnected at %s", client_info, ctime(&current_time));
            sem_wait(&sem_file);
            ssize_t bytesWriteCon = write(log,log_message,strlen(log_message));
            sem_post(&sem_file);
            if(bytesWriteCon == -1)
            {
                Die("Error writing to log file.");
            }
            memset(log_message, 0, BUFFSIZE);

        }
        else //If it's a "regular messsage", log the received message
        {
            strcat(bufferC,"\n");
            sem_wait(&sem_file);
            ssize_t bytesWriteMes = write(log,bufferC,strlen(bufferC));
            sem_post(&sem_file);
            if(bytesWriteMes == -1)
            {
                Die("Error writing to log file.");
            }
        }
        //Clear the buffer
        memset(bufferC, 0, BUFFSIZE); 
    }
    //End of connexion
    close(log);
    close(sock);
    exit(EXIT_SUCCESS);
}

//Get the values in the default server configuration file and from the options in the command line
void setConfigValuesServer(int argc, char* argv[])
{   
    //If we have a good number of options on the command line, retreive the default values from the configuration file
    if(argc <= 3 && argc > 0)
    {
        char buffer[BUFFSIZE];

        //Opening configuration file
        int config_file = open("config_file_server.txt", O_RDONLY);
        if(config_file == -1)
        {
            Die("Failed to open default configuration file.");
        }

        //Reading the first line of the configuration file (the file is not supposed to change) and put the value in server_port
        ssize_t bytesRead = read(config_file,buffer,18);
        buffer[bytesRead]='\0';
        strtok(buffer,"=");
        server_port = atoi(strtok(NULL,"="));

        //Buffer reset
        memset(buffer, 0, sizeof(buffer));

        //Reading the second line and copy the value in log_dir
        bytesRead = read(config_file,buffer,55);
        buffer[bytesRead]='\0';
        strtok(buffer,"=");
        strcpy(log_dir,strtok(NULL,"="));
        if(!strcmp(log_dir," "))
        {
            strcpy(log_dir,"");
        }
        close(config_file);
    }

    //If options have been added in the command line, overwrite log_dir and server_port with the options' values
    switch(argc)
    {
        case 1:
            //If no option, no need to go into the default case (no error)
            break; 

        case 2:
            //One option has been added, checking if its for the server port or for the log directory
            char* option = strtok(argv[1],"="); //split the string using '=' as the delimiter
            if(!strcmp(option,"--server_port"))
            {
                char* option_2 = strtok(NULL,"=");
                if(option_2 == NULL)
                {
                    Die("Incorrect server port format, no value given.");   //No value = error
                }

                if(isANumber(option_2)) // Verifying that the value given is indeed a decimal number
                {
                    server_port = atoi(option_2);   //If yes, put the value in server_port
                }
                else
                {
                    Die("Server port given is not a number");   //If not, raise an error
                }
            }
            else if(!strcmp(option,"--log_directory"))
            {
                char* option_2 = strtok(NULL,"=");
                if(option_2 == NULL)
                {
                    Die("Incorrect directory format, no value given."); //No value = error
                }
                int length = strlen(option_2);
                if(length > 0)
                {
                    if('/' == option_2[length - 1]) //Checking if the string given has the '/' caracter at the end (essential for after)
                    {
                        strcpy(log_dir,option_2);   //If yes, put the value in log_dir
                    }
                    else
                    {
                        Die("Incorrect directory format (need to terminate with '/').");    //If not, raise an error
                    }
                }
                else
                {
                    Die("Error reading directory.");
                }
            }
            else
            {
                //No possible option is found, raise an error
                Die("Incorrect usage of the command : ./server (--server_port=<port>) (--log_directory=<log_directory>)");
            }
            break;

        case 3:
            char* option3 = strtok(argv[1],"=");
            if(!strcmp(option3,"--server_port"))
            {
                char* option_23 = strtok(NULL,"=");
                if(option_23 == NULL)
                {
                    Die("Incorrect server port format, no value given");
                }

                if(isANumber(option_23))
                {
                    server_port = atoi(option_23);
                }
                else
                {
                    Die("Server port given is not a number");
                }
            }
            else if(!strcmp(option3,"--log_directory"))
            {
                char* option_23 = strtok(NULL,"=");
                if(option_23 == NULL)
                {
                    Die("Incorrect directory format, no value given.");
                }
                int length = strlen(option_23);
                if(length > 0)
                {
                    if('/' == option_23[length - 1])
                    {
                        strcpy(log_dir,option_23);
                    }
                    else
                    {
                        Die("Incorrect directory format (need to terminate with '/').");
                    }
                }
                else
                {
                    Die("Error reading directory.");
                }
            }
            else
            {
                //No possible option is found, raise an error
                Die("Incorrect usage of the command : ./server (--server_port=<port>) (--log_directory=<log_directory>)");
            }

            option3 = strtok(argv[2],"=");
            if(!strcmp(option3,"--server_port"))
            {
                char* option_23 = strtok(NULL,"=");
                if(option_23 == NULL)
                {
                    Die("Incorrect server port format, no value given.");
                }

                if(isANumber(option_23))
                {
                    server_port = atoi(option_23);
                }
                else
                {
                    Die("Server port given is not a number");
                }
            }
            else if(!strcmp(option3,"--log_directory"))
            {
                char* option_23 = strtok(NULL,"="); 
                if(option_23 == NULL)
                {
                    Die("Incorrect directory format, no value given.");
                }
                int length = strlen(option_23);
                if(length > 0)
                {
                    if('/' == option_23[length - 1])
                    {
                        strcpy(log_dir,option_23);
                    }
                    else
                    {
                        Die("Incorrect directory format (need to terminate with '/').");
                    }
                }
                else
                {
                    Die("Error reading directory.");
                }
            }
            else
            {
                //No possible option is found, raise an error
                Die("Incorrect usage of the command : ./server (--server_port=<port>) (--log_directory=<log_directory>)");
            }
            break;

        default:
            //Default case if the command is misused
            Die("Incorrect usage of the command : ./server (--server_port=<port>) (--log_directory=<log_directory>)");
            break;
    }

    //Checking if the given directory exists and if not, create it
    DIR* dir = opendir(log_dir);
    if(dir)
    {
        closedir(dir);
    }
    else
    {
        printf("Directory not found, creating directory...\n");
        if(mkdir(log_dir,0777) == -1)
        {
            Die("Error when creating log directory");
        }
        else
        {
            printf("Directory created!\n");
        }
    }
}

//Generate a unique log filename based on the current time, returns a .txt file
void generateUniqueFilename(char *filename, size_t size) 
{
    //Get the current local time in a variable
    time_t current_time = time(NULL);
    struct tm *local_time = localtime(&current_time);

    //Create the filename string using the current time
    strftime(filename, size, "log%d%m%Y_%H:%M:%S.txt", local_time); 
}

//Create a log file using the generateUniqueFilename function
char* createLogFile(char* log_dir)
{
    char filename[30];
    //generation of the filename
    generateUniqueFilename(filename,sizeof(filename));
    
    //adding the directory and the filename together 
    char* file = strcat(log_dir,filename);

    //Opening and closing the newly created file (Error if open failed)
    int log_file = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(log_file == -1)
    {
        Die("Failed to open new log file.");
    }

    close(log_file);
    return file;
}

int main(int argc, char* argv[]) 
{
    //Set the configuration values up
    setConfigValuesServer(argc,argv);
    
    //Initialize the semaphore for file protection
    sem_init(&sem_file,1,1);

    int serversock, clientsock;
    struct sockaddr_in echoserver, echoclient;

    //Creation of the server's socket
    serversock = socket(AF_INET, SOCK_STREAM, 0);
    if(serversock == -1) 
    {
        Die("Failed to create socket");
    }

    //Clearing the sockaddr_in server struct and filling up with a few informations
    memset(&echoserver, 0, sizeof(echoserver));
    echoserver.sin_family = AF_INET;                    //IP address              
    echoserver.sin_addr.s_addr = htonl(INADDR_ANY);     //Any incoming address accepted
    echoserver.sin_port = htons(server_port);           //Specified server port

    //Binding the serversocket
    if(bind(serversock, (struct sockaddr *) &echoserver, sizeof(echoserver)) == -1) 
    {
        close(serversock);
        Die("Failed to bind the server socket");
    }

    //makes the socket listening for incoming connections
    if(listen(serversock, MAX_CLIENTS) == -1) 
    {
        close(serversock);
        Die("Failed to listen on server socket");
    }

    printf("Server now listening on port %d.\n",server_port);
    printf("Creating log file...\n");
    char* log_file = createLogFile(log_dir);
    printf("New log file created : %s\n",log_file);

    //Make stdin in non-blocking mode 
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    //Structs for the select call
    fd_set read_fds;
    struct timeval timeout;

    while(1)
    {
        //Creating a set of file descriptors that need to be monitored
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);    //Adding stdin in the set
        FD_SET(serversock, &read_fds);      //Adding serversock in the set

        timeout.tv_sec = 0;
        timeout.tv_usec = 100000; // = 100 ms

        int select_result = select(serversock + 1, &read_fds, NULL, NULL, &timeout);    //Monitor the file descriptors in read_fds that are ready for I/O

        if (select_result == -1)    //If no fd is available, raise an error
        {
            Die("Error in select");
        }

        if (FD_ISSET(STDIN_FILENO, &read_fds))  //If stdin is available in non blocking mode..
        {
            if (fgets(command, sizeof(command), stdin) != NULL) //Get user input from stdin 
            {
                size_t command_len = strlen(command);
                if (command_len > 0 && command[command_len - 1] == '\n') //Replacing the final \n caracter by the null-terminator
                {
                    command[command_len - 1] = '\0';
                }

                if (!strcmp(command, "exit"))   //If user write exit, server should shutdown
                {
                    break;
                }
                if(!strcmp(command, "file"))    //If user write file, then print the name of the log file
                {
                    printf("Current log file : %s", log_file);
                }
            }
        }

        if (FD_ISSET(serversock, &read_fds))    //If the server socket is available in non-blocking mode..
        {
            unsigned int clientlen = sizeof(echoclient);

            if ((clientsock = accept(serversock, (struct sockaddr *) &echoclient, &clientlen)) == -1)   //Accepting the client's connection
            {
                Die("Failed to accept client connection");
            }
            printf("Client connected: %s\n", inet_ntoa(echoclient.sin_addr));   //Printing the new connection on the terminal

            pid_t childPid = fork();    //Forking to handle multiple clients

            if(childPid == -1)  //If the fork failed, raising an error
            {
                close(serversock);
                close(clientsock);
                Die("Unable to fork.");
            }
            if(childPid == 0)   //If in a child process, handle the client's messages
            {
                char client_info[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &echoclient.sin_addr, client_info, sizeof(client_info)); //Puting the IP address (in a readable format) in client_info
                HandleClient(clientsock,log_file,client_info);
            }
            else if(childPid > 0)
            {
                close(clientsock);
                waitpid(childPid, NULL, WNOHANG);   //Avoid zombie processes
            }
            else
            {
                close(serversock);
                close(clientsock);
                Die("Fork failed.");
            }
        }
    }
    //Shutting down sequence 
    printf("Server shutting down...\n");
    close(serversock);
    close(clientsock);
    sem_destroy(&sem_file);
    return 0;
}