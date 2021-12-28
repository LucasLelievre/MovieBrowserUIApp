project(MovieBrowser C CXX)
cmake_minimum_required(VERSION 3.3.2)

include(cmake/App.cmake)

set(SOURCES
"src/SubProcessWin.h"
"src/SubProcessWin.cpp"
"src/DirScanner.h"
"src/DirScanner.cpp"
"src/MovieBrowser.h"
"src/MovieBrowser.cpp"
"src/main.cpp")

add_app("${SOURCES}")