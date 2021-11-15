#include "File.h"
#include <iostream>
File::File(std::string fileName) : DirectoryContent(false, nameFromFileName(fileName)){
    this->fileName = fileName;
}

File::~File() {}

/**
 * @brief get the name of the file
 * 
 * @return std::string 
 */
std::string File::getFileName(){
    return this->fileName;
}

/**
 * @brief return the json view of the object
 * 
 * @return std::string 
 */
std::string File::toJson(){
    std::string output = "{\"type\": \"movie file\",\"name\": \"";
    output.append(this->getName()).append("\",\"file name\": \"");
    output.append(this->getFileName()).append("\"}");
    return output;
}

/**
 * @brief returns the movie's name from the file's name
 * 
 * @param fileName 
 * @return std::string 
 */
std::string File::nameFromFileName(std::string fileName) {
    std::cmatch m;
    std::regex_search(fileName.c_str(), m, std::regex("^(.*)(.zip|( \\())"));
    return m[1];
}