CFLAGS=-g
LDFLAGS=-g

exec: main.c ma.o cv.o sv.o testador ag
	gcc -Wall -O2 -o exec main.c sv.o cv.o ma.o

run: exec
	./exec

ma.o: ma.h ma.c

sv.o:sv.c sv.h

cv.o:cv.c cv.h

testador:testador.c
	gcc -O2 -o testador testador.c

ag:ag.c 
	gcc -O2 -o ag ag.c	





clean:
	rm -f ma sv cv
