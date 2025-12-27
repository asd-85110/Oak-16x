
cmake_minimum_required(VERSION 4.0.0)
project(Oak-16x)

# link_directories(D:/forNuitka/mingw64/lib)

set(CMAKE_C_COMPILER D:/forNuitka/mingw64/bin/gcc.exe)
set(CMAKE_CXX_COMPILER D:/forNuitka/mingw64/bin/g++.exe)
set(EXECUTABLE_OUTPUT_PATH ../Release)
add_executable(Oak ../main.cpp ../res.rc)
target_link_libraries(Oak gdi32 comdlg32 uxtheme)
