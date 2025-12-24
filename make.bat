@echo off
g++.exe -std=c++11 main.cpp -o .\Debug\main.exe -lcomctl32 -luxtheme -lshell32 -O2 -finput-charset=UTF-8 -fexec-charset=GBK -lgdi32 -lcomdlg32 -lcomctl32 -luxtheme -g
D:\ResHacker\ResourceHacker -o .\Debug\main.exe -a addoverwrite -r resource.res -save Debug\main.exe
.\Debug\main.exe