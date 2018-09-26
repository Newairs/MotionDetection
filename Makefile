CC = g++
CFLAGS = -g -Wall
SRCS = Motion_detect.cpp
PROG = MD
WITH_JPG = OFF

OPENCV = `pkg-config opencv --cflags --libs`
LIBS = $(OPENCV)

$(PROG):$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)

run:
	./MD