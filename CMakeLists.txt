cmake_minimum_required(VERSION 3.10)
project(MIS_Deleting_K_Edges)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O0 -Wall")

add_executable(MIS_Deleting_K_Edges main.cpp  Utility.cpp Graph.cpp PayAndTry/PayAndTry.cpp PayAndTry/NearLinearPayAndTry.cpp PayAndTry/RiskPayAndTry.cpp LastDeleter.cpp EdgeIndex/EdgeDeleteIndex.cpp EdgeIndex/ProfitDeleteIndex.cpp Widget/Notifier.cpp  ExactSolver.cpp)