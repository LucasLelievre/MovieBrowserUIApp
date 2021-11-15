project(MovieBrowser C CXX)
cmake_minimum_required(VERSION 3.3.2)

include(cmake/App.cmake)

set(SOURCES
"src/MovieBrowser.h"
"src/MovieBrowser.cpp"
"src/main.cpp")

add_app("${SOURCES}")