LIBS = -lGL -lglut -lm

CC=gcc
CFLAGS= -g -Wall -std=c99

APP= rubiks
 
all: $(APP)

$(APP): main.o rubiks.o cube.o vector.o utils.o
	$(CC) $(CFLAGS) -o $@ main.o rubiks.o cube.o vector.o utils.o $(LIBS)

rubiks.o:	rubiks.c cube.h vector.h utils.h
	$(CC) $(CFLAGS) -c rubiks.c

cube.o:	cube.c vector.h utils.h
	$(CC) $(CFLAGS) -c cube.c

vector.o:	vector.c vector.h
	$(CC) $(CFLAGS) -c vector.c

utils.o:	utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

clean:
	rm -f $(APP)
	rm -f *.o
