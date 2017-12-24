CC=clang
CFLAGS=-O -DYYMAXDEPTH=900
DESTDIR=/usr/local/bin
INC = cmanifs.h cglobals.h y.tab.h

.PHONY: all cp install clean

all: pass2

pass2: main.o y.tab.o yylex.o expr.o cglobals.o
	$(CC) $(CFLAGS) -o pass2 main.o y.tab.o yylex.o expr.o cglobals.o

cp install:
	cp pass2 $(DESTDIR)/pass2
	rm -f *.o y.tab.h y.tab.c pass2

clean:
	rm -f *.o y.tab.h y.tab.c pass2

main.o: $(INC) main.c
y.tab.o: $(INC) y.tab.c
yylex.o: $(INC) yylex.c
expr.o: $(INC) expr.c
cglobals.o: $(INC) cglobals.c

y.tab.c y.tab.h: tree.y
	yacc -d tree.y
