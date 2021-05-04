##Adapted from http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

CC = gcc
CFLAGS = -Wall -Wextra -std=gnu99 -I.
OBJ = bfs.o socket.o parser.o
EXE = crawler


##Create .o files from .c files. Searches for .c files with same .o names given in OBJ
%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

##Create executable linked file from object files. 
$(EXE): $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

##Other dependencies
bfs.o: queue.h bfs.c
	$(CC) $(CFLAGS) -c bfs.c

parser.o: socket.h parser.h parser.c
	$(CC) $(CFLAGS) -c parser.c

socket.o: socket.h socket.c
	$(CC) $(CFLAGS) -c socket.c

##Performs clean (i.e. delete object files) and deletes executable
clean:
	rm -f $(EXE) $(OBJ)

