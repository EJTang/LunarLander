#name:			Eddy J. Tang
#ONE Card number:	1430371
#Unix id: 		ejtang
#lecture section:	A1
#instructor's name:	Neil Burch
#lab section: 		D04
#TA's name:		Chao Gao


CFLAGS = -g -std=c99 -Wall

lander: lander.o ship.o
	gcc $(CFLAGS) -o lander lander.o ship.o  -lm -lcurses

lander.o: lander.c lander.h
	gcc $(CFLAGS) -o lander.o -c lander.c -lm -lcurses

ship: ship.o
	gcc $(CFLAGS) -o ship ship.o

ship.o: ship.c ship.h
	gcc $(CFLAGS) -o ship.o -c ship.c

clean:
	-rm -f *.o lander core

tar:
	tar cvf submit.tar lander.c lander.h ship.c ship.h Makefile README Sketchpad.jar landscape.txt
