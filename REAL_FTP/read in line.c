#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int readline(int fd, char * buf, int nbytes);
int main()
{
	int buf[300];
	int fd = open("passwd.txt", O_RDONLY);
	int a = strlen("tracking\r\n");
	int b = strlen("tracking");

	printf("%d %d\n", a,b);
	return 0;
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
