CC=clang
CFLAGS=-O -DYYMAXDEPTH=900
DESTDIR=/usr/local/bin
INC = cmanifs.h cglobals.h y.tab.h

.PHONY: all cp install clean

all: pass1

pass1: main.o y.tab.o yylex.o expr.o cglobals.o
	$(CC) $(CFLAGS) -o pass1 main.o y.tab.o yylex.o expr.o cglobals.o

cp install:
	cp pass1 $(DESTDIR)/pass1
	rm -f *.o y.tab.h y.tab.c pass1

clean:
	rm -f *.o y.tab.h y.tab.c pass1

main.o: $(INC) main.c
y.tab.o: $(INC) y.tab.c
yylex.o: $(INC) yylex.c
expr.o: $(INC) expr.c
cglobals.o: $(INC) cglobals.c

y.tab.c y.tab.h: flucid.y
	yacc -d flucid.y
