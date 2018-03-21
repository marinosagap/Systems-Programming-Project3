#
# Makefile 
#
# Type  make    to compile all the programs
# in the chapter 
#
all: MirrorServer MirrorInitiator ContentServer
clean: 
	rm -f MirrorServer MirrorInitiator ContentServer 

MirrorServer: MirrorServer.c 
	gcc -o MirrorServer MirrorServer.c functions.c -lpthread
MirrorInitiator: MirrorInitiator.c
	gcc -o MirrorInitiator MirrorInitiator.c  -lpthread
ContentServer: ContentServer.c 
	gcc -o ContentServer ContentServer.c   -lpthread
