#include "server.h"

#define BUFFSIZE 1024
#define MAX_CLIENTS 5000

int server_port;
char log_dir[150];
int s_flag = 0;
char command[100];

sem_t sem_file;

void Die(char *mess) 
{ 
    perror(mess); 
    exit(EXIT_FAILURE); 
}

int isANumber(const char *str) 
{
    char *endptr;
    strtol(str, &endptr, 10);
    return *endptr == '\0';
}

int HandleClient(int sock, char* log_file, char* client_info) 
{
    char bufferC[BUFFSIZE];
    sem_wait(&sem_file);
    int log = open(log_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
    sem_post(&sem_file);

    if (log == -1)
    {
        Die("Error opening log file.\n");
    }

    time_t current_time;
    time(&current_time);

    char log_message[BUFFSIZE];
    snprintf(log_message,BUFFSIZE,"Client %s connected at %s", client_info, ctime(&current_time));

    sem_wait(&sem_file);
    ssize_t bytesWriteCon = write(log,log_message,strlen(log_message));
    if(bytesWriteCon == -1)
    {
        Die("Error writing to log file.");
    }
    sem_post(&sem_file);
    memset(log_message, 0, BUFFSIZE);

    while(1)
    {
        memset(bufferC, 0, BUFFSIZE);
        ssize_t bytes_received = recv(sock, bufferC, sizeof(bufferC), 0);
        if(bytes_received <= 0) 
        {
            break;
        }

        if(!strcmp(bufferC,"exit"))
        {
            time(&current_time);
            snprintf(log_message,BUFFSIZE,"Client %s disconnected at %s", client_info, ctime(&current_time));
            sem_wait(&sem_file);
            ssize_t bytesWriteCon = write(log,log_message,strlen(log_message));
            if(bytesWriteCon == -1)
            {
                Die("Error writing to log file.");
            }
            sem_post(&sem_file);
            memset(log_message, 0, BUFFSIZE);

        }
        else
        {
            strcat(bufferC,"\n");
            sem_wait(&sem_file);
            ssize_t bytesWriteMes = write(log,bufferC,strlen(bufferC));
            if(bytesWriteMes == -1)
            {
                Die("Error writing to log file.");
            }
            sem_post(&sem_file);
        }
        
        memset(bufferC, 0, BUFFSIZE);
    }

    close(log);
    close(sock);
    exit(EXIT_SUCCESS);
}

void setConfigValuesServer(int argc, char* argv[])
{   
    if(argc < 3 && argc > 0)
    {
        char buffer[BUFFSIZE];

        int config_file = open("config_file_server.txt", O_RDONLY);
        if(config_file == -1)
        {
            Die("Failed to open default configuration file.");
        }

        ssize_t bytesRead = read(config_file,buffer,18);
        buffer[bytesRead]='\0';
        strtok(buffer,"=");
        server_port = atoi(strtok(NULL,"="));

        memset(buffer, 0, sizeof(buffer));

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
            else if(!strcmp(option,"--log_directory"))
            {
                char* option_2 = strtok(NULL,"=");
                if(option_2 == NULL)
                {
                    Die("Incorrect directory format (need to terminate with '/').");
                }
                int length = strlen(option_2);
                if(length > 0)
                {
                    if('/' == option_2[length - 1])
                    {
                        strcpy(log_dir,option_2);
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
            else if(!strcmp(option3,"--log_directory"))
            {
                char* option_23 = strtok(NULL,"=");
                if(option_23 == NULL)
                {
                    Die("Incorrect directory format (need to terminate with '/').");
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
            else if(!strcmp(option3,"--log_directory"))
            {
                char* option_23 = strtok(NULL,"="); 
                if(option_23 == NULL)
                {
                    Die("Incorrect directory format (need to terminate with '/').");
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

            break;
        default:
            Die("Incorrect usage of the command : ./server (--server_port=<port>) (--log_directory=<log_directory>)");
            break;
    }

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

void generateUniqueFilename(char *filename, size_t size) 
{
    time_t current_time = time(NULL);
    struct tm *local_time = localtime(&current_time);
    strftime(filename, size, "log%d%m%Y_%H:%M:%S.txt", local_time);
}

char* createLogFile(char* log_dir)
{
    char filename[30];
    generateUniqueFilename(filename,sizeof(filename));
    
    char* file = strcat(log_dir,filename);

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
    setConfigValuesServer(argc,argv);
    
    sem_init(&sem_file,1,1);

    int serversock, clientsock;
    struct sockaddr_in echoserver, echoclient;

    serversock = socket(AF_INET, SOCK_STREAM, 0);
    if(serversock == -1) 
    {
        Die("Failed to create socket");
    }

    memset(&echoserver, 0, sizeof(echoserver));
    echoserver.sin_family = AF_INET;                  
    echoserver.sin_addr.s_addr = htonl(INADDR_ANY);   
    echoserver.sin_port = htons(server_port);                

    if(bind(serversock, (struct sockaddr *) &echoserver, sizeof(echoserver)) == -1) 
    {
        close(serversock);
        Die("Failed to bind the server socket");
    }

    if(listen(serversock, MAX_CLIENTS) == -1) 
    {
        close(serversock);
        Die("Failed to listen on server socket");
    }

    printf("Server now listening on port %d.\n",server_port);
    printf("Creating log file...\n");
    char* log_file = createLogFile(log_dir);
    printf("New log file created : %s\n",log_file);

    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    fd_set read_fds;
    struct timeval timeout;

    while(1)
    {
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);
        FD_SET(serversock, &read_fds);

        timeout.tv_sec = 0;
        timeout.tv_usec = 100000; // 100 ms

        int select_result = select(serversock + 1, &read_fds, NULL, NULL, &timeout);

        if (select_result == -1)
        {
            Die("Error in select");
        }

        if (FD_ISSET(STDIN_FILENO, &read_fds))
        {
            if (fgets(command, sizeof(command), stdin) != NULL)
            {
                size_t command_len = strlen(command);
                if (command_len > 0 && command[command_len - 1] == '\n')
                {
                    command[command_len - 1] = '\0';
                }

                if (!strcmp(command, "exit"))
                {
                    break;
                }
            }
        }

        if (FD_ISSET(serversock, &read_fds))
        {
            unsigned int clientlen = sizeof(echoclient);

            if ((clientsock = accept(serversock, (struct sockaddr *) &echoclient, &clientlen)) == -1) 
            {
                Die("Failed to accept client connection");
            }
            printf("Client connected: %s\n", inet_ntoa(echoclient.sin_addr));

            pid_t childPid = fork();

            if(childPid == -1)
            {
                close(serversock);
                close(clientsock);
                Die("Unable to fork.");
            }
            if(childPid == 0)
            {
                char client_info[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &echoclient.sin_addr, client_info, sizeof(client_info));
                HandleClient(clientsock,log_file,client_info);
            }
            else if(childPid > 0)
            {
                close(clientsock);
                waitpid(childPid, NULL, WNOHANG);
            }
            else
            {
                close(serversock);
                close(clientsock);
                Die("Fork failed.");
            }
        }
    }
    printf("Server shutting down...\n");
    close(serversock);
    close(clientsock);
    sem_destroy(&sem_file);
    return 0;
}