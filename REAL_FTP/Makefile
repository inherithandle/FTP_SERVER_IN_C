server : Data_Part.o Control__Part.o FTP_Gateway.o main.o
	gcc -o server Control__part.o FTP_Gateway.o main.o Data_Part.c Header.h -lpthread -g
Control__Part.o : Header.h Control__Part.c
	gcc -c Control__Part.c -g
FTP_Gateway.o : Header.h FTP_Gateway.c
	gcc -c FTP_Gateway.c -g
Data_Part.o : Header.h Data_Part.c
	gcc -c Data_Part.c -g
main.o : Header.h main.c
	gcc -c main.c -lpthread -g
