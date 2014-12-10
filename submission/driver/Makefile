all: receiveDriver sendDriver

sendDriver: sendDriver.c
	gcc -ansi -Wall -I/usr/src/linux-headers/include/ -o sendDriver sendDriver.c

receiveDriver: receiveDriver.c
	gcc -ansi -Wall -I/usr/src/linux-headers/include/ -o receiveDriver receiveDriver.c
