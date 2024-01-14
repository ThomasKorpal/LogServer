#ifndef __SERVER_H__
#define __SERVER_H__

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <semaphore.h>
#include <dirent.h>
#include <sys/stat.h>

void Die(char *mess);
int HandleClient(int sock, char* log_file, char* client_info) ;
void setConfigValuesServer(int argc, char* argv[]);
char* createLogFile(char* log_dir);
void generateUniqueFilename(char *filename, size_t size);

#endif 