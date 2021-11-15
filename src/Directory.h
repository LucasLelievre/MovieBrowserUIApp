#pragma once

#include <vector>

#include "DirectoryContent.h"

class Directory : public DirectoryContent
{
private:
    std::vector<DirectoryContent*> content;
public:
    Directory(std::string name = "main directory");
    ~Directory();

    void addContent(DirectoryContent* newContent);

    std::string toJson();
};