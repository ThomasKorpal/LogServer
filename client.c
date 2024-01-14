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

void Die(char* mess)
{
    perror(mess); 
    exit(EXIT_FAILURE); 
}

int isIPValid(char* ip_str)
{
    int a, b, c, d;
    int number_of_parts = sscanf(ip_str, "%d.%d.%d.%d", &a, &b, &c, &d);

    if (number_of_parts != 4) 
    {
        return 0;
    }

    if (a < 0 || a > 255 || b < 0 || b > 255 || c < 0 || c > 255 || d < 0 || d > 255) 
    {
        return 0; 
    }

    return 1; 
}

int isANumber(const char *str) 
{
    char *endptr;
    strtol(str, &endptr, 10);
    return *endptr == '\0';
}

void setConfigValuesClient(int argc, char* argv[])
{   
    if(argc < 3 && argc > 0)
    {
        char buffer[BUFFSIZE];

        int config_file = open("config_file_client.txt", O_RDONLY);
        if(config_file == -1)
        {
            Die("Failed to open default configuration file.");
        }

        ssize_t bytesRead = read(config_file,buffer,18);
        buffer[bytesRead]='\0';
        strtok(buffer,"=");
        server_port = atoi(strtok(NULL,"="));

        memset(buffer, 0, sizeof(buffer));

        bytesRead = read(config_file,buffer,22);
        buffer[bytesRead]='\0';
        strtok(buffer,"=");
        strcpy(server_ip,strtok(NULL,"="));

        close(config_file);
    }

    switch(argc)
    {
        case 1:
            break; 
        case 2:
            char* option = strtok(argv[1],"=");
            if(!strcmp(option,"--server_port"))
            {
                char* option_2 = strtok(NULL,"=");
                if(option_2 == NULL)
                {
                    Die("Incorrect server port format");
                }

                if(isANumber(option_2))
                {
                    server_port = atoi(option_2);
                }
                else
                {
                    Die("Server port given is not a number");
                }
            }
            else if(!strcmp(option,"--server_ip"))
            {
                char* option_2 = strtok(NULL,"=");
                if(option_2 == NULL)
                {
                    Die("Incorrect IP address format.");
                }
                else
                {
                    if(isIPValid(option_2))
                    {
                        strcpy(server_ip,option_2);
                    }
                    else
                    {
                        Die("Input given is not a valid IP address.");
                    }
                }
            }
            break;
        case 3:
            char* option3 = strtok(argv[1],"=");
            if(!strcmp(option3,"--server_port"))
            {
                char* option_23 = strtok(NULL,"=");
                if(option_23 == NULL)
                {
                    Die("Incorrect server port format");
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
                    Die("Incorrect IP address format.");
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

            option3 = strtok(argv[2],"=");
            if(!strcmp(option3,"--server_port"))
            {
                char* option_23 = strtok(NULL,"=");
                if(option_23 == NULL)
                {
                    Die("Incorrect server port format.");
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
                    Die("Incorrect IP address format.");
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
            break;
        default:
            Die("Incorrect usage of the command : ./client (--server_port=<port>) (--server_ip=<server_ip>)");
            break;
    }
}

int main(int argc, char* argv[])
{
    setConfigValuesClient(argc,argv);

    int client_socket; 
    struct sockaddr_in server_addr;
    char buffer[BUFFSIZE];
    char headerBuff[BUFFSIZE];
    char client_name[50];

    if((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        Die("Failed to create socket");
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);

    if(inet_pton(AF_INET, server_ip, &(server_addr.sin_addr)) != 1)
    {
        close(client_socket);
        Die("Invalid IP address");
    }

    if(connect(client_socket, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1)
    {
        close(client_socket);
        Die("Failed to connect to the server with the specified inputs");
    }
    printf("Connected to server with address %s on port %d.\n",server_ip, server_port);

    printf("Please enter your name: ");
    scanf("%s", client_name);
    getchar();

    printf("Hello %s, and welcome on the log server.\n", client_name);
    while(1)
    {
        printf("Please enter your message (exit to quit):\n");
        if(fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            close(client_socket);
            Die("Failed to read from stdin\n");
        }

        size_t bufferlen = strlen(buffer);
        if(bufferlen > 0)
        {
            buffer[bufferlen - 1] = '\0';
        }

        if(!strcmp("exit", buffer))
        {
            break;
        }  

        time_t current_time = time(NULL);
        struct tm *local_time = localtime(&current_time);
        char timestamp[20];
        strftime(timestamp, BUFFSIZE, "%H:%M:%S", local_time);

        sprintf(headerBuff, "[%s, %s: %s] ", client_name, server_ip, timestamp);
        strcat(headerBuff,buffer);

        if(bufferlen != 0)
        {
            if(send(client_socket, headerBuff, strlen(headerBuff),0) != strlen(headerBuff))
            {
                close(client_socket);
                Die("Wrong number of sent bytes\n");
            }
        }

        memset(headerBuff, 0, BUFFSIZE);
        memset(buffer, 0, BUFFSIZE);
    }
    memset(headerBuff, 0, BUFFSIZE);
    memset(buffer, 0, BUFFSIZE);
    printf("Closing client socket... \n");
    close(client_socket);
    exit(EXIT_SUCCESS);
}