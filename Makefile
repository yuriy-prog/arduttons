ifeq ($(OSTYPE), FreeBSD)
   CC = g++47 --std=c++11 -Wno-deprecated
else
   CC = g++ --std=c++11 -Wno-deprecated
endif

all: arduttons

clean:
	rm *.o *~

toptparser.o: toptparser.h toptparser.cc
	$(CC) $(CFLAGS) -c toptparser.cc -o toptparser.o

arduttons.o: arduttons.cc
	$(CC) $(CFLAGS) -c $< -o $@

arduttons: toptparser.o arduttons.o
	$(CC) -o arduttons toptparser.o arduttons.o
