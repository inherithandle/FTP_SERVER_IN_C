//
//  FTP_Gateway.c
//  REAL_FTP
//
//  Created by Tracking on 13. 1. 20..
//  Copyright (c) 2013년 Tracking. All rights reserved.
//

#include "Header.h"

void * FTP_gateway(void *data)
{
    int                 sockfd = *((int *)data);
    int                 readn;
    socklen_t           addrlen;
    char                RAW_command[MAXLINE], debugbuf[MAXLINE];
    int                 current_code, previous_code = 0;
    char*               FTP_banner  =   "220 My FTP SERVER 1.0\r\n";
    struct sockaddr_in  client_addr;
    char                loginID[30];
    FTPinfo             FTP_information;
    int                 debug_count= 0;
    // initilize
    memset(&FTP_information, 0x00, sizeof(FTP_information));
    getcwd(FTP_information.execute_location, BUFSIZE);
    FTP_information.controlsockfd   =   sockfd;
        /* for debug */
//    chdir("/Users/tracking/Desktop/REAL_FTP/REAL_FTP");
//    getcwd(RAW_command, MAXLINE);
//    printf("%s\n", RAW_command);
        /* for debug */
    
        /* for debug */
    addrlen = sizeof(client_addr);
    getpeername(sockfd, (struct sockaddr *)&client_addr, &addrlen);
        /* for debug */
    
    /*          FTP banner!!!           */
    write(sockfd, FTP_banner, strlen(FTP_banner));
    
    
    while((readn = read(sockfd, FTP_information.RAW_command, MAXLINE)) > 0)
    {
        
        // receive user command.

        sprintf(debugbuf, "Read Data %s(%d) : %s", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), FTP_information.RAW_command);
        write(STDOUT_FILENO, debugbuf, strlen(debugbuf));
        
        
        current_code = command_interpreter(&FTP_information);

        //write(FTP_information->controlsockfd, FTP_information.response_code,
            //strlen(FTP_information.response_code)); // send response.
        
        previous_code   =   current_code;
        
        // data transfer is going to start shortly.
        if( strcmp(FTP_information.RAW_command, "PORT") == 0)
        {
            /*
            // on failure, send "cannot open data connection." I need return value.
            active_open(&FTP_information);

            // on failure, I would send the message "cannot open connection.".
            strcpy(FTP_information.response_code, "150 Data Connection OK\r\n");
            write(sockfd, FTP_information.response_code,
                  strlen(FTP_information.response_code)); // send response.
            
            // on success, send data transfer complete.
            Data_gateway(&FTP_information);
            
            close(FTP_information.datasockfd); // trasnfer complete.
            
            write(sockfd, FTP_information.response_code,
                  strlen(FTP_information.response_code)); // send response.
            
            */
            
            
        }
        else if( strcmp(FTP_information.RAW_command, "PASV") == 0)
        {
            /*
            passive_open(&FTP_information);
            
            // on failure, I would send the message "cannot open connection.".
            strcpy(FTP_information.response_code, "150 Data Connection OK\r\n");
            write(sockfd,
                  FTP_information.response_code,
                  strlen(FTP_information.response_code)); // send response.

            // on success, send data transfer complete.
            Data_gateway(&FTP_information);
            close(FTP_information.datasockfd); // trasnfer complete.
            
            write(sockfd, FTP_information.response_code,
                  strlen(FTP_information.response_code)); // send response.
             */
        }
        
        
        memset(FTP_information.RAW_command, 0x00, MAXLINE); // get comamnd-buffer clean.
        memset(FTP_information.response_code, 0x00, MAXLINE); // get response-buffer clean.
        debug_count++;
        memset(debugbuf, 0x00, MAXLINE);
    }
    close(sockfd);
    
    
    printf("worker thread end\n");
    return 0;
}