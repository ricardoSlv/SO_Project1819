CFLAGS=-g
LDFLAGS=-g

all : sv ma cv ag
	make ma sv ag

ma:ma.c ModuloDados.c ModuloDados.h
	gcc -Wall -O2 -o ma ma.c ModuloDados.c ModuloDados.h

sv:sv.c ModuloDados.h ModuloDados.c
	gcc -Wall -O2 -o sv sv.c ModuloDados.h ModuloDados.c

cv:cv.c ModuloDados.c ModuloDados.h
	gcc -Wall -O2 -o cv cv.c ModuloDados.c ModuloDados.h

ag: ag.c ModuloDados.c ModuloDados.h
	gcc -Wall -O2 -o ag ag.c ModuloDados.c ModuloDados.h

testador:testador.c
	gcc -O2 -o testador testador.c

ag:ag.c 
	gcc -O2 -o ag ag.c	





clean:
	rm -f ma sv cv
