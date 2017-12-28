CC=clang
CFLAGS=-O -DYYMAXDEPTH=900
DESTDIR=/usr/local/bin
INC = cmanifs.h cglobals.h y.tab.h

.PHONY: all cp install clean

all: pass4

pass4: main.o y.tab.o yylex.o expr.o walk.o cglobals.o
	$(CC) $(CFLAGS) -o pass4 main.o y.tab.o yylex.o expr.o walk.o cglobals.o

cp install:
	cp pass4 $(DESTDIR)/pass4
	rm -f *.o y.tab.h y.tab.c pass4

clean:
	rm -f *.o y.tab.h y.tab.c pass4

main.o: $(INC) main.c
y.tab.o: $(INC) y.tab.c
yylex.o: $(INC) yylex.c
expr.o: $(INC) expr.c
walk.o: $(INC) walk.c
cglobals.o: $(INC) cglobals.c

y.tab.c y.tab.h: tree.y
	yacc -d tree.y
