PROGRAM    = cross_the_road
CC	   = gcc
#CFLAGS    = -Wall 
GLFLAGS    = -lglut -lGLU -lGL -lm

$(PROGRAM): projekat.c 
	$(CC) -o $(PROGRAM) projekat.c $(GLFLAGS)	

.PHONY: clean 

clean: 
	-rm *.o $(PROGRAM) *core
