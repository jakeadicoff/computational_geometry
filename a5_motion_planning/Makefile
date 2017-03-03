PLATFORM = $(shell uname)

## Compilation flags
##comment out one or the other
##debugging
CFLAGS = -g
##release
#CFLAGS = -O3 -DNDEBUG
LDFLAGS=

CFLAGS+= -Wall

ifeq ($(PLATFORM),Darwin)
## Mac OS X
CFLAGS += -m64 -isystem/usr/local/include -Wno-deprecated
LDFLAGS+= -m64 -lc -framework AGL -framework OpenGL -framework GLUT -framework Foundation
else
## Linux
CFLAGS += -m64
INCLUDEPATH  = -I/usr/include/GL/
LIBPATH = -L/usr/lib64 -L/usr/X11R6/lib
LDFLAGS+=  -lGL -lglut -lrt -lGLU -lX11 -lm -lXext
#LDFLAGS+=  -lGL -lglut -lrt -lGLU -lX11 -lm  -lXmu -lXext -lXi
endif

CC = g++ -O3 $(INCLUDEPATH)

PROGS = plan

default: $(PROGS)

plan: mouse.o motionPlan.o geom.o
	$(CC) -o $@ mouse.o motionPlan.o geom.o $(LDFLAGS)

mouse.o: mouse.cpp motionPlan.h geom.h
	$(CC) -c $(INCLUDEPATH) $(CFLAGS) mouse.cpp  -o $@

motionPlan.o: motionPlan.cpp motionPlan.h geom.o
	$(CC) -c $(INCLUDEPATH) $(CFLAGS) motionPlan.cpp -o $@

geom.o: geom.cpp geom.h
	$(CC) -c $(INCLUDEPATH) $(CFLAGS) geom.cpp -o $@

clean::
	rm *.o
	rm plan
