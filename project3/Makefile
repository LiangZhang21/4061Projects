CC = gcc
CFLAGS = -g
CLINK = -lpthread

SRCDIR=src
INCLDIR=include
LIBDIR=lib

mapreduce: main.o producer.o consumer.o $(LIBDIR)/utils.o
	$(CC) -o $@ $^ -I$(INCLDIR) $(CFLAGS) $(CLINK)

main.o: $(SRCDIR)/main.c $(LIBDIR)/utils.o
	$(CC) -o $@ -I$(INCLDIR) -c $(SRCDIR)/main.c $(CFLAGS)

producer.o: $(SRCDIR)/producer.c $(LIBDIR)/utils.o
	$(CC) -o $@ -I$(INCLDIR) -c $(SRCDIR)/producer.c $(CFLAGS)

consumer.o: $(SRCDIR)/consumer.c $(LIBDIR)/utils.o
	$(CC) -o $@ -I$(INCLDIR) -c $(SRCDIR)/consumer.c $(CFLAGS)

$(LIBDIR)/utils.o: $(LIBDIR)/utils.c
	$(CC) -o $(LIBDIR)/utils.o -I$(INCLDIR) -c $(LIBDIR)/utils.c $(CFLAGS)

t0: mapreduce
	python3 test/test.py T0

t1: mapreduce
	python3 test/test.py T1

t2: mapreduce
	python3 test/test.py T2

t3: mapreduce
	python3 test/test.py T3

t4: mapreduce
	python3 test/test.py T4

test: mapreduce
	python3 test/test.py T0 T1 T2 T3 T4

.PHONY: clean

clean: 
	rm mapreduce main.o producer.o consumer.o $(LIBDIR)/utils.o