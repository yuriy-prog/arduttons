ifeq ($(OSTYPE), FreeBSD)
   CC = g++48 -static -pthread --std=c++11 -Wno-deprecated
    #trouble by my freebsd? solve later
else
   CC = g++ -static-libstdc++ -pthread --std=c++11 -Wno-deprecated
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
	$(CC) -o arduttons toptparser.o arduttons.o
