//
//  main.c
//  REAL_FTP
//
//  Created by Tracking on 13. 1. 17..
//  Copyright (c) 2013년 Tracking. All rights reserved.
//

#include "Header.h"

void Thread_manager(int client_fd);

#include <stdio.h>
#include <stdlib.h>
#define SIZE 80



int main(int argc, char * argv[])
{
    int listen_fd, client_fd;
    socklen_t addrlen;
    int readn;
    char buf[MAXLINE];
    pthread_t thread_id;
    
    struct sockaddr_in server_addr, client_addr;
    
    if( (listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return 1;
    
    memset((void*)&server_addr, 0x00, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORTNUM);
    
    if(bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("bind error");
        return 1;
    }
    if(listen(listen_fd, 5) == -1)
    {
        perror("listen error");
        return 1;
    }
    
    while(1)
    {
        addrlen = sizeof(client_addr);
        client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addrlen);
        if(client_fd == -1)
        {
            printf("accept error\n");
            // return -1;
        }
        else
        {
            printf("someone comes in.\n");
            fflush(stdout);
            pthread_create(&thread_id, NULL, FTP_gateway(&client_fd), (void *)&client_fd);
            pthread_detach(thread_id);
        }
    }
    return 0;
}

void Thread_manager(int client_fd)
{
    pthread_t thread_id;
    fflush(stdout);
    printf("someone comes in.\n");
    
    
    pthread_create(&thread_id, NULL, FTP_gateway, (void *)&client_fd);
    pthread_detach(thread_id);
}





/*
 char line[SIZE];
 
 char *firstTok,*secondTok;
 
 
 while(1)
 {
 printf("> ");
 fgets(line, SIZE, stdin);
 strcat(line, "\0");
 
 printf("You entered [%s]\n", line);
 
 firstTok = strtok(line, " ");
 secondTok = strtok(NULL, " ");
 
 if( strcmp(line, "quit") == 0 )
 exit(0);
 
 if(strcmp(line, "path")==0)
 {
 printf("path: %s", getenv("PATH"));
 }
 if( strcmp(firstTok, "where") ==0)
 {
 
 // find what directory secondTok is in
 }
 }
 */










