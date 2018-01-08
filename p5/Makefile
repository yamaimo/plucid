CC=clang
CFLAGS=-g -DYYMAXDEPTH=900
DESTDIR=/usr/local/bin
INC = cmanifs.h cglobals.h y.tab.h

.PHONY: all cp install clean

all: pass5

pass5: main.o y.tab.o yylex.o expr.o ident.o cglobals.o
	$(CC) $(CFLAGS) -o pass5 main.o y.tab.o yylex.o expr.o ident.o cglobals.o

cp install:
	cp pass5 $(DESTDIR)/pass5
	rm -f *.o y.tab.h y.tab.c pass5

clean:
	rm -f *.o y.tab.h y.tab.c pass5

main.o: $(INC) main.c
y.tab.o: $(INC) y.tab.c
yylex.o: $(INC) yylex.c
expr.o: $(INC) expr.c
ident.o: $(INC) ident.c
cglobals.o: $(INC) cglobals.c

y.tab.c y.tab.h: tree.y
	yacc -d tree.y
