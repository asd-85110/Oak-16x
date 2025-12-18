
cmake_minimum_required(VERSION 4.2.0)
project(Oak-16x)

link_directories(D:/forNuitka/mingw64/lib)


set(CMAKE_C_COMPILER D:/forNuitka/mingw64/bin/gcc.exe)
set(CMAKE_CXX_COMPILER D:/forNuitka/mingw64/bin/g++.exe)
set(EXECUTABLE_OUTPUT_PATH ./../Release)
add_executable(OAK16 ../main.cpp)
target_link_libraries(OAK16 gdi32 comdlg32 uxtheme)
