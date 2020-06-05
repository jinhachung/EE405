PC_CC = g++
BB_CC = arm-linux-gnueabihf-gcc
FLAGS = -g -w
LIBS = -lSDL2 -lSDL2_image -lpthread

all: Control_Camera Control_Viewer

Control_Camera: Control_Camera.c UDP.c
	$(BB_CC) $(FLAGS) -o Control_Camera Control_Camera.c UDP.c -lpthread
 
Control_Viewer: Control_Viewer.cpp UDP.c
	$(PC_CC) $(FLAGS) -o Control_Viewer Control_Viewer.cpp UDP.c $(LIBS)

clean:
	rm Control_Camera Control_Viewer
