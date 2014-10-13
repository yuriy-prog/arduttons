CC = clang++ --std=c++11 -Wno-deprecated -I/usr/local/include

all: arduttons

arduttons.o: arduttons.cc
	$(CC) $(CFLAGS) -c $< -o $@

arduttons: arduttons.o
	$(CC) -o arduttons arduttons.o
