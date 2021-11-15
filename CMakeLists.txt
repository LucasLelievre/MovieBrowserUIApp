project(MovieBrowser C CXX)
cmake_minimum_required(VERSION 3.3.2)

include(cmake/App.cmake)

set(SOURCES
"src/DirectoryContent.h"
"src/DirectoryContent.cpp"
"src/Directory.h"
"src/Directory.cpp"
"src/File.h"
"src/File.cpp"
"src/MovieBrowser.h"
"src/MovieBrowser.cpp"
"src/main.cpp")

add_app("${SOURCES}")