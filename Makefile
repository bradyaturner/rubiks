CC=gcc
CFLAGS= -g -Wall

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	LIBS = -lGL -lglfw -lm
endif
ifeq ($(UNAME_S),Darwin) # macOS
	FRAMEWORK = -framework OpenGL -I/usr/local/include -lglfw
endif

APP= rubiks
 
all: $(APP)

$(APP): main.o rubiks.o cube.o vector.o utils.o quaternion.o
	$(CC) $(FRAMEWORK) $(CFLAGS) -o $@ main.o rubiks.o cube.o vector.o utils.o quaternion.o $(LIBS)

test:	test.o singlecube.o cube.o vector.o utils.o quaternion.o
	$(CC) $(FRAMEWORK) $(CFLAGS) -o $@ test.o singlecube.o cube.o vector.o utils.o quaternion.o $(LIBS)

singlecube.o:	singlecube.c cube.h vector.h
	$(CC) $(CFLAGS) -c singlecube.c

rubiks.o:	rubiks.c cube.h vector.h utils.h logger.h
	$(CC) $(CFLAGS) -c rubiks.c

cube.o:	cube.c cube.h vector.h utils.h
	$(CC) $(CFLAGS) -c cube.c

quaternion.o:	quaternion.c quaternion.h vector.h
	$(CC) $(CFLAGS) -c quaternion.c

vector.o:	vector.c vector.h
	$(CC) $(CFLAGS) -c vector.c

utils.o:	utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

clean:
	rm -f $(APP) test
	rm -f *.o
