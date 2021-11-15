#pragma once

#include <regex>

#include "DirectoryContent.h"

class File : public DirectoryContent {
private:
    std::string fileName;
public:
    File(std::string fileName = "movie file");
    ~File();

    std::string getFileName();
    std::string toJson();

    std::string nameFromFileName(std::string fileName);
};