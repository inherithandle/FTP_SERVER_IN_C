//
//  Header.h
//  REAL_FTP
//
//  Created by Tracking on 13. 1. 20..
//  Copyright (c) 2013년 Tracking. All rights reserved.
//

#ifndef REAL_FTP_Header_h
#define REAL_FTP_Header_h


#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <netdb.h>
#include <stdlib.h>
#include <pwd.h> // file user id information.
#include <grp.h> // file group id information.


#define BUFSIZE             512
#define MAXLINE             1024
#define PORTNUM             3600

typedef struct FTPssesion
{
    char    loginID[30];
    char    root_directory[BUFSIZE];
    char    working_directory[BUFSIZE];
    char    user_realative_path[BUFSIZE];
    char    execute_location[BUFSIZE]; // ftpserver.exe path.
    int     transfer_mode;
    int     data_port;
    int     datasockfd;
    int     controlsockfd;
    char    RAW_command[MAXLINE];
    char    response_code[MAXLINE];
    struct sockaddr_in client_addr;
} FTPinfo;

int command_parser(char*, char*, char*);
int checkID(char* argID, char* argPass);
int command_interpreter(FTPinfo *FTP_information);
void * FTP_gateway(void *data);
int equalstr(char* arg1, char* arg2);
char* Generate_userpath(char* working_copy, char *root);
void Get_dataportandIP(FTPinfo *FTP_information, char* arg);
int active_open(FTPinfo *FTP_information);
int passive_open(FTPinfo *FTP_information);

int simple_ls(char* filename, int sockfd);
int display_attribute(struct stat buf, char* LineBuffer, int sockfd);
int display_permission_filetype(struct stat buf, char* LineBuffer, int sockfd);

void Test(FTPinfo *FTP_information);
int Data_gateway(FTPinfo *FTP_inforamtion);
int Sendtofile(FTPinfo * FTP_information, char* filename);
int Receivefile(FTPinfo* FTP_information, char* filename);
#endif

