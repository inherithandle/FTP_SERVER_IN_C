//
//  Data_port.c
//  REAL_FTP
//
//  Created by Tracking on 13. 1. 24..
//  Copyright (c) 2013년 Tracking. All rights reserved.
//

#include "Header.h"

int active_open(FTPinfo *FTP_information)
{
    int sockfd;
    int client_len;
//    struct sockaddr_in ClientBintAddr;
    sockfd      =   socket(AF_INET, SOCK_STREAM, 0);
    client_len  =   sizeof(FTP_information->client_addr);
    

//    memset(&ClientBintAddr, 0, sizeof(ClientBintAddr));
//    ClientBintAddr.sin_family       = AF_INET;
//    ClientBintAddr.sin_addr.s_addr  = htonl(INADDR_ANY);
//    ClientBintAddr.sin_port         = htons(4020); // well-known data port 20.
    
    /*
    if( bind(sockfd, (struct sockaddr*)&ClientBintAddr, sizeof(ClientBintAddr)) == -1)
    {
           perror("bind error");
           return ;
    }
     */
    
    if(connect(sockfd, (struct sockaddr*)&(FTP_information->client_addr), client_len)
            ==  -1)
    {
        perror("connect error");
        return 0;
    }
    FTP_information->datasockfd     =   sockfd;
    
    return 0;
}


int passive_open(FTPinfo *FTP_information)
{
    int listen_fd, client_fd;
    int nSockOpt;
    int portnum = 45239;
    int upper8bits, lower8bits;
    int addrlen;
    struct sockaddr_in server_addr, client_addr;
    char IPandPORT[BUFSIZE];
    char temp[20];
    
    
    if( (listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return 1;
    
    /* for purpose of debug */
    nSockOpt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &nSockOpt, sizeof(nSockOpt));
    /* for purpose of debug */
    
    
    memset((void*)&server_addr, 0x00, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(portnum);
    
    if(bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("bind error");
        return 1;
    }
    if(listen(listen_fd, 1) == -1)
    {
        perror("listen error");
        return 1;
    }
    
    
    
    // manipulate message(PORT,IP) to send the client.

    strcpy(IPandPORT, "227 Entering Passive Mode (127,0,0,1,");
    
    
    upper8bits      =   portnum >> 8;
    lower8bits      =   portnum & 0x00FF;
    
    // spliting port into message.
    sprintf(temp, "%d,", upper8bits);
    strcat(IPandPORT, temp);
    sprintf(temp, "%d)\r\n", lower8bits);
    strcat(IPandPORT, temp);
    
    write(FTP_information->controlsockfd, IPandPORT, strlen(IPandPORT));
    
    client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addrlen);
    if(client_fd == -1)
    {
        printf("accept error\n");
        // return -1;
    }
    FTP_information->datasockfd     =   client_fd;
    return 0;
}


void Test(FTPinfo* FTP_information)
{
    //write(FTP_information->datasockfd, "LIST\r\nHithere\r\n", 15);
    simple_ls(FTP_information->working_directory, FTP_information->datasockfd);
//    close(FTP_information->datasockfd);
}

int Data_gateway(FTPinfo *FTP_information)
{

    char        pure_command[MAXLINE];
    char        arg_opt[MAXLINE];
    

    //read(FTP_information->controlsockfd, FTP_information->RAW_command, MAXLINE);
    command_parser(FTP_information->RAW_command, pure_command, arg_opt);

    
    if( strcmp(pure_command, "LIST") == 0)
    {
        simple_ls(FTP_information->working_directory, FTP_information->datasockfd);
        strcpy(FTP_information->response_code, "226 Directory send OK.\r\n");
        return 0;
    }
    else if( strcmp(pure_command, "RETR") == 0)
    {
        Sendtofile(FTP_information, arg_opt);
        strcpy(FTP_information->response_code, "226 Transfer Complete.\r\n");
        // send to file.
        return 0;
    }
    else if( strcmp(pure_command, "STOR") == 0)
    {
        Receivefile(FTP_information, arg_opt);
        strcpy(FTP_information->response_code, "226 Transfer Complete.\r\n");
        return 0;
        
    }
    else if( strcmp(pure_command, "SIZE") == 0)
    {
        strcpy(FTP_information->response_code, "200 SIZE command OK.\r\n");
        return 0;
    }
    else
    {
        return -1; // return value indacates that it does not anything for transfer.
    }
    
//    close(FTP_information->datasockfd);
    return 0; // data transfer success.
}


int simple_ls(char* filename, int sockfd)
{
    DIR *dp;
    struct dirent *iter;
    struct stat buf;        // used for stat function.
    
    char LineBuffer[BUFSIZE]; // is sended to data connection.
    char EntireBuffer[BUFSIZE] = {0, };
    char ConcatenateBuffer[BUFSIZE]; // temporary buffer.
    int n;
    
    dp = opendir(filename);
    
    if(dp == NULL)
    {
        // filename is not directory.
        // That is, ls [filename].
        
        if(stat(filename, &buf) == -1)
        {
            // invalid filename.
            perror("error : no such file\n");
            return -1;
        }
        
        display_permission_filetype(buf, LineBuffer, sockfd); // permission bit, file type
        display_attribute(buf, LineBuffer, sockfd); // print link count, user id, etc.
        sprintf(ConcatenateBuffer, "%15s",filename); // print file name.
        strcat(LineBuffer, ConcatenateBuffer);
        write(sockfd, LineBuffer, strlen(LineBuffer));
        return 0;
    }
    
    while( (iter = readdir(dp)) != NULL)
    {
        stat(iter->d_name, &buf);
        display_permission_filetype(buf, LineBuffer, sockfd); // permission bit, file type
        display_attribute(buf, LineBuffer, sockfd); // print link count, user id, etc.
        sprintf(ConcatenateBuffer, "%15s",iter->d_name); // print file name.
        strcat(LineBuffer, ConcatenateBuffer);
        strcat(LineBuffer, "\r\n");
//        strcat(EntireBuffer, LineBuffer); // here here here
        write(sockfd, LineBuffer, strlen(LineBuffer));
        memset(LineBuffer,0x00, strlen(LineBuffer));
    }
    
    // dp = opendir(filename);
    // pointer = readdir(dp);
    // 
    return 0;
}
int display_permission_filetype(struct stat buf, char* LineBuffer, int sockfd)
{

    if(S_ISREG(buf.st_mode))
    {
        strcpy(LineBuffer, "-");
    }
    else if(S_ISDIR(buf.st_mode))
    {
        strcpy(LineBuffer, "d"); //
    }
    else if(S_ISCHR(buf.st_mode))
    {
        strcpy(LineBuffer, "c"); //
    }
    else if(S_ISBLK(buf.st_mode))
    {
        strcpy(LineBuffer, "b"); //
    }
    else if(S_ISFIFO(buf.st_mode))
    {
       strcpy(LineBuffer, "f"); //
    }
    else if(S_ISLNK(buf.st_mode))
    {
        strcpy(LineBuffer, "l"); //
    }
    else if(S_ISSOCK(buf.st_mode))
    {
        strcpy(LineBuffer, "s"); //
    }
    
    if(buf.st_mode & S_IRUSR)
    {
        strcat(LineBuffer, "r"); //
    }
    else
    {
        strcat(LineBuffer, "-"); //
    }
    
    if(buf.st_mode & S_IWUSR)
    {
        strcat(LineBuffer, "w"); //
    }
    else
    {
        strcat(LineBuffer, "-"); //
    }
    
    if(buf.st_mode & S_IXUSR)
    {
        strcat(LineBuffer, "x"); //
    }
    else
    {
        strcat(LineBuffer, "-"); //
    }
    
    if(buf.st_mode & S_IRGRP)
    {
        strcat(LineBuffer, "r"); //
    }
    else
    {
        strcat(LineBuffer, "-"); //
    }
    
    if(buf.st_mode & S_IWGRP)
    {
        strcat(LineBuffer, "w"); //
    }
    else
    {
        strcat(LineBuffer, "-"); //
    }
    
    if(buf.st_mode & S_IXGRP)
    {
        strcat(LineBuffer, "x"); //
    }
    else
    {
        strcat(LineBuffer, "-"); //
    }
    
    if(buf.st_mode & S_IROTH)
    {
        strcat(LineBuffer, "r"); //
    }
    else
    {
        strcat(LineBuffer, "-"); //
    }
    
    if(buf.st_mode & S_IWOTH)
    {
        strcat(LineBuffer, "w"); //
    }
    else
    {
        strcat(LineBuffer, "-"); //
    }
    
    if(buf.st_mode & S_IXOTH)
    {
        strcat(LineBuffer, "x"); //
    }
    else
    {
        strcat(LineBuffer, "-"); //
    }
    
    return 0;
}

int display_attribute(struct stat buf, char* LineBuffer, int sockfd)
{
    struct passwd *userID;
    struct group *groupID;
    char ConcatenateBuffer[BUFSIZE];
    userID  =   getpwuid(buf.st_uid);
    groupID =   getgrgid(buf.st_gid);
    sprintf(ConcatenateBuffer, "%4d ",buf.st_nlink);
    strcat(LineBuffer, ConcatenateBuffer);
    sprintf(ConcatenateBuffer, "%15s ", userID->pw_name);
    strcat(LineBuffer, ConcatenateBuffer);
    sprintf(ConcatenateBuffer, "%10s ", groupID->gr_name);
    strcat(LineBuffer, ConcatenateBuffer);
    sprintf(ConcatenateBuffer, "%7lld ", buf.st_size);
    strcat(LineBuffer, ConcatenateBuffer);
    strftime(ConcatenateBuffer, BUFSIZE, "%m %d %H:%M", localtime(&buf.st_mtime));
    strcat(LineBuffer, ConcatenateBuffer);
    
    return 0;
}

int Sendtofile(FTPinfo * FTP_information, char* filename)
{
    int     fd      =   open(filename, O_RDONLY);
    FILE*   fp      =   fopen(filename, "rb");
    size_t  leng;
    char    filebuf[BUFSIZE];
    
    //while( read(fd, filebuf, BUFSIZE - 3) > 0 ) // I will append '\r'.
    while( (leng = fread(filebuf, sizeof(char),  BUFSIZE, fp)) > 0 )
    {
        /*
        leng    =   strlen(filebuf);
        if( filebuf[leng] == 0 && filebuf[leng - 1] != '\n')
        {
            filebuf[leng] = '\r';
            filebuf[leng + 1] = '\n';
            filebuf[leng + 2] = 0;
        }
        else if( filebuf[leng] == 0 && filebuf[leng - 1] == '\n')
        {
            filebuf[leng - 1] = '\r';
            filebuf[leng] = '\n';
            filebuf[leng + 1] = 0;
        }
        */
        write(FTP_information->datasockfd, filebuf, leng);
        memset(filebuf, 0x00, BUFSIZE);
    }
    close(fd);
    return 0; // we need to do exception processing.
}

int Receivefile(FTPinfo* FTP_information, char* filename)
{
    int         fd      =   open(filename, O_WRONLY | O_CREAT, 0644);
    char        filebuf[BUFSIZE];
    while( read(FTP_information->datasockfd, filebuf, BUFSIZE) > 0 )
    {
        write(fd, filebuf, strlen(filebuf));
    }
    close(fd);
    return 0;
}

void getmyIP(char* buf)
{
    struct utsname utsname;
    struct hostent *h;
    
    uname(&utsname);
    
    if(!(h=gethostbyname(utsname.nodename)))
    {
        fprintf(stderr,"%s\n", hstrerror(h_errno));
        exit(1);
    }
    
    strcpy(buf, inet_ntoa(*((struct in_addr *)h->h_addr)) );

}