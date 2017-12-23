CC=clang
CFLAGS=-O -DYYMAXDEPTH=900
DESTDIR=/usr/local/bin
LINT= lint -p
TAR= tar -cv
INC = cmanifs.h cglobals.h y.tab.h

.PHONY: all cp install clean print lint tape

all: pass1

pass1: main.o y.tab.o yylex.o expr.o cglobals.o
	$(CC) $(CFLAGS) -o pass1 main.o y.tab.o yylex.o expr.o cglobals.o

cp install:
	cp pass1 $(DESTDIR)/pass1
	rm -f *.o y.tab.h y.tab.c pass1

clean:
	rm -f *.o y.tab.h y.tab.c pass1

print:

lint: main.c yylex.c expr.c cglobals.c
	$(LINT) main.c yylex.c expr.c cglobals.c

tape:
	$(TAR) main.c yylex.c expr.c cglobals.c cmanifs.h cglobals.h flucid.y

main.o: $(INC) main.c
y.tab.c y.tab.h: flucid.y
	yacc -d flucid.y
y.tab.o: $(INC) y.tab.h y.tab.c
yylex.o: $(INC) yylex.c
expr.o: $(INC) expr.c
cglobals.o: $(INC) cglobals.c
