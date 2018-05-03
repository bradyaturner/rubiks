CC=gcc
CFLAGS= -g -Wall -std=c99

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
  LIBS = -lGL -lglut -lm
endif
ifeq ($(UNAME_S),Darwin) # macOS
  FRAMEWORK = -framework GLUT
  FRAMEWORK += -framework OpenGL
  CFLAGS += -Wno-deprecated-declarations # GLUT is depreciated in OSX >= 10.9, replace with glfw
endif

APP= rubiks
 
all: $(APP)

$(APP): main.o rubiks.o cube.o vector.o utils.o
	$(CC) $(FRAMEWORK) $(CFLAGS) -o $@ main.o rubiks.o cube.o vector.o utils.o $(LIBS)

test:	test.o singlecube.o cube.o vector.o utils.o
	$(CC) $(FRAMEWORK) $(CFLAGS) -o $@ test.o singlecube.o cube.o vector.o utils.o $(LIBS)

singlecube.o:	singlecube.c cube.h vector.h
	$(CC) $(CFLAGS) -c singlecube.c

rubiks.o:	rubiks.c cube.h vector.h utils.h
	$(CC) $(CFLAGS) -c rubiks.c

cube.o:	cube.c vector.h utils.h
	$(CC) $(CFLAGS) -c cube.c

vector.o:	vector.c vector.h
	$(CC) $(CFLAGS) -c vector.c

utils.o:	utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

clean:
	rm -f $(APP) test
	rm -f *.o
