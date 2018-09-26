all: ma 

ma:	
	gcc -o Motion_detect Motion_detect.cpp `pkg-config --cflags --libs opencv` -L/usr/local/lib64 -lm

