CC = g++ -L/usr/local/lib -lpopt -I/usr/local/include
all:	arduttons.cc
	$(CC) -o arduttons arduttons.cc
