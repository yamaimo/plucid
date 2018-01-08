CC=clang
CFLAGS=-g
DESTDIR=/usr/local/bin
INC = imanifs.h iglobals.h

.PHONY: all cp install clean

all: luval

luval: main.o input.o dynamic.o util.o dump.o rupture.o memory.o \
		f_alg.o f_lucid.o f_flucid.o f_vars.o \
		string.o list.o real.o iglobals.o
	$(CC) $(CFLAGS) -o luval -lm \
		main.o input.o dynamic.o util.o dump.o rupture.o memory.o \
		f_alg.o f_lucid.o f_flucid.o f_vars.o \
		string.o list.o  real.o iglobals.o

cp install: luval
	cp luval $(DESTDIR)/luval
	rm luval *.o

clean:
	rm -f *.o luval

main.o: $(INC) main.c
input.o: $(INC) input.c
dynamic.o: $(INC) dynamic.c
util.o: $(INC) util.c
dump.o: $(INC) dump.c
rupture.o: $(INC) rupture.c
memory.o: $(INC) memory.c
f_alg.o: $(INC) f_alg.c
f_lucid.o: $(INC) f_lucid.c
f_flucid.o: $(INC) f_flucid.c
f_vars.o: $(INC) f_vars.c
string.o: $(INC) string.c
list.o: $(INC) list.c
real.o: $(INC) real.c
iglobals.o: $(INC) iglobals.c
