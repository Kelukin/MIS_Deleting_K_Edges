CC=g++
CFLAGS=-O3
OBJ = main.o Graph.o Utility.o PayAndTry.o LastDeleter.o EdgeDeleteIndex.o ProfitDeleteIndex.o

all:mis

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

mis: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
	rm *.o

clean:
	rm *.o
