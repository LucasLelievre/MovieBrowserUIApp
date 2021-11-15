#pragma once

#include <string>

class DirectoryContent 
{
private:
    // True = directory, False = File
    bool type;
    std::string name;
public:
    DirectoryContent(bool type, std::string name);
    ~DirectoryContent();

    bool getType();
    std::string getName();

    virtual std::string toJson() {
        return "called the wrong method";
    }
};