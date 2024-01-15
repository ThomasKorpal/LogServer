#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <semaphore.h>

#define BUFFSIZE 1024

int server_port = 0;
char server_ip[INET_ADDRSTRLEN + 10];

//Function called when an error has been raised
void Die(char* mess)
{
    perror(mess); 
    exit(EXIT_FAILURE); 
}

//Function that verifies the validity of the string given as an input (used for user-proofing the program)
int isIPValid(char* ip_str)
{
    int a, b, c, d;
    int number_of_parts = sscanf(ip_str, "%d.%d.%d.%d", &a, &b, &c, &d);

    if (number_of_parts != 4) 
    {
        return 0;
    }

    //Every number of an IP address is supposed to be between 0 and 255 included
    if (a < 0 || a > 255 || b < 0 || b > 255 || c < 0 || c > 255 || d < 0 || d > 255) 
    {
        return 0; 
    }

    return 1; 
}

//Tells if a string is a decimal number (used for user-proofing the program)
int isANumber(const char *str) 
{
    char *endcarac;
    strtol(str, &endcarac, 10);
    //If str is a number, then when converted in base 10 no letter should be found. If \0 is the last caracter, all of the string has been converted and as such the string is a number
    return *endcarac == '\0';
}

//Get the values in the default client configuration file and from the options in the command line
void setConfigValuesClient(int argc, char* argv[])
{   
    //If we have a good number of options on the command line, retreive the default values from the configuration file
    if(argc <= 3 && argc > 0)
    {
        char buffer[BUFFSIZE];

        //Opening configuration file
        int config_file = open("config_file_client.txt", O_RDONLY);
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

        //Reading the second line and copy the value in server_ip
        bytesRead = read(config_file,buffer,22);
        buffer[bytesRead]='\0';
        strtok(buffer,"=");
        strcpy(server_ip,strtok(NULL,"="));

        close(config_file);
    }

    //If options have been added in the command line, overwrite server_ip and server_port with the options' values
    switch(argc)
    {
        case 1:
            //If no option, no need to go into the default case (no error)
            break; 

        case 2:
            char* option = strtok(argv[1],"="); //split the string using '=' as the delimiter
            if(!strcmp(option,"--server_port"))
            {
                char* option_2 = strtok(NULL,"=");
                if(option_2 == NULL)    //No value = error
                {
                    Die("Incorrect server port format, no value given");
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
            else if(!strcmp(option,"--server_ip"))
            {
                char* option_2 = strtok(NULL,"=");
                if(option_2 == NULL)
                {
                    Die("Incorrect IP address format, no value given");
                }
                else
                {
                    if(isIPValid(option_2)) //Veryfing that the input is a valid IP address
                    {
                        strcpy(server_ip,option_2);
                    }
                    else
                    {
                        Die("Input given is not a valid IP address.");
                    }
                }
            }
            else
            {
                //No possible option is found, raise an error
                Die("Incorrect usage of the command : ./client (--server_port=<port>) (--server_ip=<server_ip>)");
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
            else if(!strcmp(option3,"--server_ip"))
            {
                char* option_23 = strtok(NULL,"=");
                if(option_23 == NULL)
                {
                    Die("Incorrect IP address format, no value given");
                }
                else
                {
                    if(isIPValid(option_23))
                    {
                        strcpy(server_ip,option_23);
                    }
                    else
                    {
                        Die("Input given is not a valid IP address.");
                    }
                }
            }
            else
            {
                //No possible option is found, raise an error
                Die("Incorrect usage of the command : ./client (--server_port=<port>) (--server_ip=<server_ip>)");
            }

            option3 = strtok(argv[2],"=");
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
            else if(!strcmp(option3,"--server_ip"))
            {
                char* option_23 = strtok(NULL,"="); 
                if(option_23 == NULL)
                {
                    Die("Incorrect IP address format, no value given");
                }
                else
                {
                    if(isIPValid(option_23))
                    {
                        strcpy(server_ip,option_23);
                    }
                    else
                    {
                        Die("Input given is not a valid IP address.");
                    }
                }
            }
            else
            {
                //No possible option is found, raise an error
                Die("Incorrect usage of the command : ./client (--server_port=<port>) (--server_ip=<server_ip>)");
            }
            break;

        default:
            Die("Incorrect usage of the command : ./client (--server_port=<port>) (--server_ip=<server_ip>)");
            break;
    }
}

int main(int argc, char* argv[])
{
    //Setting the server_port and the server_ip values
    setConfigValuesClient(argc,argv);

    int client_socket; 
    struct sockaddr_in server_addr;
    char buffer[BUFFSIZE];
    char headerBuff[BUFFSIZE];
    char client_name[50];

    //Creating the client socket
    if((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        Die("Failed to create socket");
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);

    if(inet_pton(AF_INET, server_ip, &(server_addr.sin_addr)) != 1) //Convert the ascii IP address into binary format to put in server_addr.sin_addr
    {
        close(client_socket);
        Die("Invalid IP address");
    }

    if(connect(client_socket, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1)  //Connection to the server with the specified values
    {
        close(client_socket);
        Die("Failed to connect to the server with the specified inputs");
    }
    printf("Connected to server with address %s on port %d.\n",server_ip, server_port);

    printf("Please enter your name: "); //User input for its name
    scanf("%s", client_name);
    getchar();  //Get the '\n' caracter to avoid problems with fgets afterwards

    printf("Hello %s, and welcome on the log server.\n", client_name);
    while(1)
    {
        printf("Please enter your message (exit to quit):\n");
        if(fgets(buffer, sizeof(buffer), stdin) == NULL)    //Waits for user input
        {
            close(client_socket);
            Die("Failed to read from stdin\n");
        }

        size_t bufferlen = strlen(buffer);
        if(bufferlen > 0)
        {
            buffer[bufferlen - 1] = '\0';   //add the null-terminator
        }

        if(!strcmp("exit", buffer)) //If user writes exit, the client shutdown
        {
            break;
        }  

        time_t current_time = time(NULL);
        struct tm *local_time = localtime(&current_time);
        char timestamp[20];
        strftime(timestamp, BUFFSIZE, "%H:%M:%S", local_time); 
        sprintf(headerBuff, "[%s, %s: %s] ", client_name, server_ip, timestamp);    //Header with client info and timestamp creation

        strcat(headerBuff,buffer);

        if(bufferlen != 0)
        {
            if(send(client_socket, headerBuff, strlen(headerBuff),0) != strlen(headerBuff)) //Sending the header + message to the server
            {
                close(client_socket);
                Die("Wrong number of sent bytes\n");
            }
        }

        //Reset of the buffers for a new message
        memset(headerBuff, 0, BUFFSIZE);
        memset(buffer, 0, BUFFSIZE);
    }
    //Client shutdown sequence
    memset(headerBuff, 0, BUFFSIZE);
    memset(buffer, 0, BUFFSIZE);
    printf("Closing client socket... \n");
    strcpy(headerBuff,"exit");
    //Sending exit to the server in order to log the disconnection
    if(send(client_socket, headerBuff, strlen(headerBuff),0) != strlen(headerBuff))
    {
        close(client_socket);
        Die("Wrong number of sent bytes\n");
    }
    close(client_socket);
    exit(EXIT_SUCCESS);
}