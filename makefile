CC=g++
CFLAGS=-O3
OBJ = main.o Graph.o Utility.o PayAndTry.o NearLinearPayAndTry.o RiskPayAndTry.o LastDeleter.o EdgeDeleteIndex.o ProfitDeleteIndex.o ExactSolver.o

all:mis

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

mis: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
	rm *.o

clean:
	rm *.o
