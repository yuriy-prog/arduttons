CC = g++
CFLAGS += -pthread -Wno-deprecated
LIBFLAGS += -pthread -static
#LIBFLAGS += -static-libstdc++

ifeq ($(OSTYPE), FreeBSD)
   #trouble by my freebsd? solve later
   #CC = g++48
   CC = g++
   LIBFLAGS += -static
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
