project(MovieBrowser C CXX)
cmake_minimum_required(VERSION 3.3.2)

include(cmake/App.cmake)

set(SOURCES
"src/JsonControler.h"
"src/JsonControler.cpp"
"src/DirScanner.h"
"src/DirScanner.cpp"
"src/JScallbackFunctions.h"
"src/JScallbackFunctions.cpp"
"src/MovieBrowser.h"
"src/MovieBrowser.cpp"
"src/main.cpp")

add_app("${SOURCES}")