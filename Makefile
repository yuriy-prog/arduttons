CC = g++ -I/usr/local/include
LIB = -L/usr/local/lib -lpopt

all: arduttons

arduttons.o: arduttons.cc
	$(CC) $(CFLAGS) -c $< -o $@

arduttons: arduttons.o
	$(CC) $(LIB) -o arduttons arduttons.o
