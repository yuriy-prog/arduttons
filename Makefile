CC = clang++ --std=c++11 -Wno-deprecated -I/usr/local/include

all: arduttons

toptparser.o: toptparser.h toptparser.cc
	$(CC) $(CFLAGS) -c toptparser.cc -o toptparser.o

arduttons.o: arduttons.cc
	$(CC) $(CFLAGS) -c $< -o $@

arduttons: toptparser.o arduttons.o
	$(CC) -o arduttons toptparser.o arduttons.o
