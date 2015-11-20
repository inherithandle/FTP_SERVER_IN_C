//
//  Contorl__Part.c
//  REAL_FTP
//
//  Created by Tracking on 13. 1. 17..
//  Copyright (c) 2013년 Tracking. All rights reserved.
//

#include "Header.h"

int command_interpreter(FTPinfo * FTP_information)
{
    char        pure_command[MAXLINE]; 
    char        arg_opt[MAXLINE];
    int         fail;         // this variable make program code readable.
    char        buf[MAXLINE]; // This will be used for multiple purpose below.
    char*       userpath;     // user relative path.

    
    strcpy(pure_command, FTP_information->RAW_command); //unsafe.
    fail = command_parser(FTP_information->RAW_command, pure_command, arg_opt);
    /*  
        for example,
        command_parser("USER localhost -arg -opt", buffer-pointer, buffer-pointer2);
        buffer-pointer  -> USER
        buffer-pointer2 -> localhost -arg -opt
    */
    
    /*
    if(fail) // fail to parse command due to invalid command.
    {
        perror("parse error");
        return -1;
    }
    */
    
    /*        interpret command.        */
    if( strcmp(pure_command, "USER") == 0)
    {
        //strcpy(FTP_information->response_code, "331");
        strcpy(FTP_information->response_code, "331 Please specify the password.\r\n");
        strcpy(FTP_information->loginID, arg_opt);
        write(FTP_information->controlsockfd, FTP_information->response_code,
              strlen(FTP_information->response_code)); // send response.
        return 331;
    }
    else if( strcmp(pure_command, "PASS") == 0)
    {
        chdir("/Users/tracking/Desktop/old/REAL_FTP/REAL_FTP"); // for xcode debug. you should delete
        
        
        fail = checkID(FTP_information->loginID, arg_opt);
        if(fail)
        {
            strcpy(FTP_information->response_code, "530 Login incorrect.\r\n");
            return 530;
        }
        
        
        
        // if there not exist user directory, then the program need to make it.
        if( access(FTP_information->loginID,F_OK) == -1)
            mkdir(FTP_information->loginID, 0777); // you'd better correct permission.

                // configure user directory.
        chdir(FTP_information->loginID);
        getcwd(FTP_information->root_directory, BUFSIZE);
        getcwd(FTP_information->working_directory, BUFSIZE);
        
        strcpy(FTP_information->response_code, "230- Welcome to my FTP world.\r\n");
        strcat(FTP_information->response_code, "230-\r\n");
        strcat(FTP_information->response_code, "230- This system is for authorized users only. All access is logged.\r\n");
        strcat(FTP_information->response_code, "230 Login successful.\r\n");
        write(FTP_information->controlsockfd, FTP_information->response_code,
              strlen(FTP_information->response_code)); // send response.
        return 230;
    }
    else if( strcmp(pure_command, "SYST") == 0)
    {
        strcpy(FTP_information->response_code, "215 MACOS Type: L8\r\n");
        write(FTP_information->controlsockfd, FTP_information->response_code,
              strlen(FTP_information->response_code)); // send response.
        return 215;
    }
    else if( strcmp(pure_command, "FEAT") == 0)
    {
        strcpy(FTP_information->response_code, "211 \r\n");
        write(FTP_information->controlsockfd, FTP_information->response_code,
              strlen(FTP_information->response_code)); // send response.
        return 211;
    }
    else if( strcmp(pure_command, "PWD") == 0)
    {

//        strcpy(buf, FTP_information->working_directory);
        userpath    =   Generate_userpath(FTP_information->working_directory, FTP_information->root_directory);
        if( *userpath == 0) // NULL is user-root directory
        {
            buf[0] = '/';
            buf[1] = 0;
            userpath = buf;
        }
        sprintf(FTP_information->response_code, "257 \"%s\"\r\n", userpath);
        write(FTP_information->controlsockfd, FTP_information->response_code,
              strlen(FTP_information->response_code)); // send response.
        return 257;
    }
    else if( strcmp(pure_command, "CWD") == 0)
    {
        
        if(access(arg_opt, F_OK	) == -1)
        {
            sprintf(FTP_information->response_code, "550 %s: No such file or directory\r\n", arg_opt);
            return 550;
        }
        chdir(arg_opt);
        getcwd(buf, MAXLINE);
        // since user can't move upper directory than user direcory!
        if( strlen(buf) < strlen(FTP_information->root_directory))
            chdir(FTP_information->root_directory);
        getcwd(FTP_information->working_directory, BUFSIZE);
        strcpy(FTP_information->response_code, "250 CWD command successful.\r\n");
        write(FTP_information->controlsockfd, FTP_information->response_code,
              strlen(FTP_information->response_code)); // send response.
        return 250;
        
    }
    else if( strcmp(pure_command, "QUIT") == 0)
    {
        sprintf(FTP_information->response_code, "221 Goodbye \r\n");
        close(FTP_information->controlsockfd);
        write(FTP_information->controlsockfd, FTP_information->response_code,
              strlen(FTP_information->response_code)); // send response.
        return 221;
    }
    else if( strcmp(pure_command, "PORT") == 0)
    {
        /* previous code.
        Get_dataportandIP(FTP_information,arg_opt);
        strcpy(FTP_information->response_code, "200 PORT command successful.\r\n");
        strcpy(FTP_information->RAW_command, pure_command);
        */
        
        Get_dataportandIP(FTP_information,arg_opt);
        strcpy(FTP_information->response_code, "200 PORT command successful.\r\n");
        write(FTP_information->controlsockfd, FTP_information->response_code,
              strlen(FTP_information->response_code));
        
        active_open(FTP_information);
        strcpy(FTP_information->response_code, "150 Data Connection OK\r\n");
        write(FTP_information->controlsockfd, FTP_information->response_code,
              strlen(FTP_information->response_code));
        return 200;
    }
    else if( strcmp(pure_command, "NOOP") == 0)
    {
        strcpy(FTP_information->response_code, "200 NOOP command successful.\r\n");
        write(FTP_information->controlsockfd, FTP_information->response_code,
              strlen(FTP_information->response_code)); // send response.
        return 200;
    }
    else if( strcmp(pure_command, "PASV") == 0)
    {
        passive_open(FTP_information);
        
        // on failure, I would send the message "cannot open connection.".
        strcpy(FTP_information->response_code, "150 Data Connection OK\r\n"); // not need.
        write(FTP_information->controlsockfd, FTP_information->response_code,
              strlen(FTP_information->response_code)); // send response.
        
        return 227;
    }
    /*
    else if( strcmp(pure_command, "LIST") == 0)
    {
        active_open(FTP_information); // needs to return -1 or 0. for exception proces
        strcpy(FTP_information->response_code, "125 Data Connection OK\r\n");
        write(FTP_information->controlsockfd, FTP_information->response_code,
              strlen(FTP_information->response_code)); // send response.
        return 125;
    }
    */
    else if( strcmp(pure_command, "TYPE") == 0)
    {
        strcpy(FTP_information->response_code,"200 TYPE Command OK\r\n");
        write(FTP_information->controlsockfd, FTP_information->response_code,
              strlen(FTP_information->response_code)); // send response.
        return 200;
    }
    else if( strcmp(pure_command, "MKD") == 0)
    {
//        strcpy(FTP_information->response_code, ")
    
        fail = mkdir(arg_opt, 0644);
        
        if(fail)
        {
            perror("directory error : ");
            strcpy(FTP_information->response_code, "503 Bad sequence of commands.\r\n");
            return 503;
        }
        else
        {
            
            strcpy(FTP_information->response_code, "257 Directory Command succesful\r\n");
            return 257;
        }
        write(FTP_information->controlsockfd, FTP_information->response_code,
              strlen(FTP_information->response_code)); // send response.
    }
    else
    {
        fail = Data_gateway(FTP_information); // maybe data transfer command.
        if(fail)
        {
            sprintf(FTP_information->response_code, "502 Command not implemented.\r\n");
            write(FTP_information->controlsockfd, FTP_information->response_code,
                  strlen(FTP_information->response_code));
            return 502;
        }
        else
        {
            close(FTP_information->datasockfd); // transfer complete.
            write(FTP_information->controlsockfd, FTP_information->response_code,
                  strlen(FTP_information->response_code));
            return 226;
        }
    }
    return 0;
}

int command_parser(char* full_command, char* command, char* arg_opt)
{
    char* pCurr;
    char* Isarg = 0;
    pCurr = full_command + 2;
    
    while( !(*pCurr == '\r' && *(pCurr + 1) == '\n'))
    {
        if( *pCurr == ' ')
        {
            Isarg      =   pCurr+1;   // there exist arguments or options.
            *pCurr = 0; // seperation of pure command and the others.
            strcpy(command, full_command);
            break;
        }
        pCurr++;
    }
    
    if(Isarg) // in case there exist arg.
    {
        while( !(*Isarg == '\r' && *(Isarg + 1) == '\n'))
            Isarg++;
        Isarg[0]    =   0;
        strcpy(arg_opt, pCurr + 1);
    }
    else
    {
        pCurr[0]    =   0;
        strcpy(command, full_command);
    }
    

    return 0; // return 0 on successfully parsing.
    
}

int checkID(char * argID, char * argPass)
{
    int fd  =   open("passwd.txt", O_RDONLY);
    char buf[BUFSIZE];
    char* pSeperateID;
    char* pPass;
    int decision1, decision2;
    
    if( fd == -1 )
    {
        perror("login file error:");
        return -1;  // login fail
    }
    memset(buf, 0x00, BUFSIZE);
    
    if(strcmp(argID, "anonymous") == 0)
    {
        chdir("/Users/tracking/Desktop/REAL_FTP/REAL_FTP/tracking");
        return 0;
    }
    
    while( readline(fd, buf, BUFSIZE) )
    {
        pSeperateID     =   strtok(buf, ":\r\n");
        pPass           =   strtok(NULL, ":\r\n");
        decision1 = strcmp(pSeperateID, argID);
        decision2 = strcmp(pPass, argPass);
        if( decision1 == 0 && decision2 == 0) // login success.
        {
            close(fd);
            return 0; // there exist ID in passwd.txt.
        }

        memset(buf, 0x00, BUFSIZE); // get buffer clear.
    }
    
    
    close(fd);
    return -1; // login fail
}

int readline(int fd, char *buf, int nbytes) {
    int numread = 0;
    int returnval;
    
    while (numread < nbytes - 1)
    {
        returnval = read(fd, buf + numread, 1);
        
        if ((returnval == -1) && (errno == EINTR)) // interrupt.
            continue;
        if ( (returnval == 0) && (numread == 0) ) // no data to read.
            return 0;
        if (returnval == 0)         // unoknown error.
            break;
        if (returnval == -1)        // read error.
            return -1;
        numread++;
        if (buf[numread-1] == '\n') {
            buf[numread] = '\0';
            return numread;
        }
    }
    
    // in case that buffer size is too small to hold text string.
    errno = EINVAL;
    return -1;
}

// this function check whether arg1 is equal to arg2.
int equalstr(char* arg1, char* arg2)
{
    int i = 0;
    while( arg1[i] != 0 && arg2[i] != 0)
    {
        if( (arg1[i] == '\n' && arg2[i] == '\r') ||
           (arg1[i] == '\r' || arg2[i] == '\n') )
            return 0;
        
        if(arg1[i] != arg2[i])
            break;
        i++;
        
    }
    
    
    return 0;
}

char* Generate_userpath(char* working, char *root)
{
    char *re;
    int rootleng = strlen(root);
    
    re = working + rootleng;
    return re;
}

void Get_dataportandIP(FTPinfo *FTP_information, char* arg)
{
    char* pRefer;
    char userIP[16];
    int upper8bit, lower8bit;
    
    memset(userIP, 0x00, 16);
    
    pRefer = strtok(arg, ",");
    strcpy(userIP, pRefer);
    userIP[strlen(pRefer)] = '.';
    userIP[strlen(userIP)] = 0;
    
    pRefer = strtok(NULL, ",");
    strcat(userIP, pRefer);
    userIP[strlen(userIP)] = '.';
    userIP[strlen(userIP)] = 0;
    
    pRefer = strtok(NULL, ",");
    strcat(userIP, pRefer);
    userIP[strlen(userIP)] = '.'; //
    userIP[strlen(userIP)] = 0;
    
    pRefer = strtok(NULL, ",");
    strcat(userIP, pRefer);
    
    
    pRefer = strtok(NULL, ","); // upper 8bits.
    upper8bit = atoi(pRefer);
    pRefer = strtok(NULL, ","); // lower 8bits.
    lower8bit = atoi(pRefer);
    FTP_information->data_port  = upper8bit * 256 + lower8bit;
    FTP_information->client_addr.sin_family         =   AF_INET;
    FTP_information->client_addr.sin_addr.s_addr    =   inet_addr(userIP);
    FTP_information->client_addr.sin_port
                                =   htons(FTP_information->data_port);
}