LIBS = -lGL -lglut -lm

CC=gcc
CFLAGS= -g -Wall 

APP= rubiks
 
all: $(APP)

$(APP): main.o
	$(CC) $(CFLAGS) -o $@ main.o $(LIBS)

clean:
	rm -f $(APP)
	rm -f *.o
