CC = g++
CFLAGS += -pthread --std=c++11 -Wno-deprecated
LIBFLAGS += -static-libstdc++

ifeq ($(OSTYPE), FreeBSD)
   CC = g++47
   #trouble by my freebsd? solve later
endif

all: arduttons

clean:
	rm *.o *~

install: all
	mkdir -p ~/arduttons
	cp arduttons test.sh down.sh up.sh ~/arduttons/

toptparser.o: toptparser.h toptparser.cc
	$(CC) $(CFLAGS) -c toptparser.cc -o toptparser.o

arduttons.o: arduttons.cc
	$(CC) $(CFLAGS) -c $< -o $@

arduttons: toptparser.o arduttons.o
	$(CC) $(LIBFLAGS) -o arduttons toptparser.o arduttons.o
