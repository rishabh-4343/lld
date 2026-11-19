@echo off
g++ -std=c++17 -Wall -Wextra -pedantic %1 -o %~n1.exe && %~n1.exe