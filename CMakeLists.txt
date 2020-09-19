cmake_minimum_required(VERSION 3.10)
project(MIS_Deleting_K_Edges)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O3 -Wall")

add_executable(MIS_Deleting_K_Edges main.cpp  Utility.cpp Graph.cpp PayAndTry/PayAndTry.cpp PayAndTry/NearLinearPayAndTry.cpp PayAndTry/RiskPayAndTry.cpp LastDeleter/LastDeleter.cpp EdgeIndex/EdgeDeleteIndex.cpp EdgeIndex/ProfitDeleteIndex.cpp Widget/Notifier.cpp  ExactSolver.cpp LocalSearch/BasicVersion.cpp EdgeIndex/ProbabilityDeleteIndex.cpp EdgeIndex/ProbabilityDeleteIndex.h PayAndTry/ProbabilityPayAndTry.cpp PayAndTry/ProbabilityPayAndTry.h EdgeIndex/BaseEdgeIndex.cpp EdgeIndex/BaseEdgeIndex.h)