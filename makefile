all: mis

mis: .obj/main.o .obj/Graph.o .obj/Utility.o .obj/PayAndTry.o .obj/LastDeleter.o .obj/EdgeDeleteIndex.o .obj/ProfitDeleteIndex.o
	g++ .obj/main.o .obj/Graph.o .obj/Utility.o .obj/PayAndTry.o .obj/LastDeleter.o .obj/EdgeDeleteIndex.o .obj/ProfitDeleteIndex.o -o mis
	rm .obj/*.o

.obj/main.o: main.cpp
	g++ -c -O3 -o .obj/main.o main.cpp

.obj/Graph.o: Graph.cpp
	g++ -c -O3 -o .obj/Graph.o Graph.cpp

.obj/Utility.o: Utility.cpp
	g++ -c -O3 -o .obj/Utility.o Utility.cpp

.obj/PayAndTry.o: PayAndTry.cpp
	g++ -c -O3 -o .obj/PayAndTry.o PayAndTry.cpp

.obj/LastDeleter.o: LastDeleter.cpp
	g++ -c -O3 -o .obj/LastDeleter.o LastDeleter.cpp

.obj/EdgeDeleteIndex.o: EdgeDeleteIndex.cpp
	g++ -c -O3 -o .obj/EdgeDeleteIndex.o EdgeDeleteIndex.cpp

.obj/ProfitDeleteIndex.o: ProfitDeleteIndex.cpp
	g++ -c -O3 -o .obj/ProfitDeleteIndex.o ProfitDeleteIndex.cpp

clean:
	rm -rf *o .obj/
	mkdir .obj
