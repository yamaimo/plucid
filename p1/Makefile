CC=clang
CFLAGS=-O -DYYMAXDEPTH=900
DESTDIR=/usr/local/bin

.PHONY: all cp install clean test

all: pass1

pass1: main.o lex.yy.o y.tab.o node.o common.o
	$(CC) $(CFLAGS) -o $@ $^

cp install:
	cp pass1 $(DESTDIR)/pass1
	rm -f *.o lex.yy.c y.tab.h y.tab.c pass1

clean:
	rm -f *.o lex.yy.c y.tab.h y.tab.c pass1

test: pass1
	bash test.sh
	bash include_test.sh

main.o: main.c
lex.yy.o: lex.yy.c
y.tab.o: y.tab.c
node.o: node.c
common.o: common.c

lex.yy.c: plucid.l
	lex plucid.l

y.tab.c y.tab.h: plucid.y
	yacc -d plucid.y

# add dependency

main.o: common.h plucid.h
lex.yy.o: plucid.h y.tab.h
y.tab.o: plucid.h y.tab.h
node.o: common.h node.h
common.o: common.h
lex.yy.c: plucid.h y.tab.h
y.tab.c y.tab.h: plucid.h
