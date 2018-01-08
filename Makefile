DESTDIR=/usr/local/bin

.PHONY: all cp install clean

all:
	make -C p1
	make -C p2
	make -C p3
	make -C p4
	make -C p5
	make -C evaluator

cp install:
	cp scripts/lucomp $(DESTDIR)/lucomp
	cp scripts/lucid $(DESTDIR)/lucid
	make -C p1 install
	make -C p2 install
	make -C p3 install
	make -C p4 install
	make -C p5 install
	make -C evaluator install

clean:
	make -C p1 clean
	make -C p2 clean
	make -C p3 clean
	make -C p4 clean
	make -C p5 clean
	make -C evaluator clean
