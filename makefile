all: mis

mis: .obj/main.o .obj/Graph.o .obj/Utility.o .obj/PayAndTry.o
    g++ .obj/main.o .obj/Graph.o .obj/Utility.o .obj/PayAndTry.o -o mis
    rm .obj/*.o

.obj/main.o: main.cpp
    g++ -c -O3 -o .obj/main.o main.cpp

.obj/Graph.o: Graph.cpp
    g++ -c -O3 -o .obj/Graph.o Graph.cpp

.obj/Utility.o: Utility.cpp
    g++ -c -O3 -o .obj/Utility.o Utility.cpp

.obj/PayAndTry.o: PayAndTry.cpp ProfitDeleteIndex.cpp LastDeleter.cpp EdgeDeleteIndex.cpp
    g++ -c -O3 -o .obj/PayAndTry.o PayAndTry.cpp EdgeDeleteIndex.cpp ProfitDeleteIndex.cpp LastDeleter.cpp

clean:
    rm -rf *o .obj/
    mkdir .obj