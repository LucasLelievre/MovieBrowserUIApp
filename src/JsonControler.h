#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>

class JsonControler {
private:
    static bool checkFile(std::string path);
public:
    JsonControler();
    ~JsonControler();

    static std::string readFile(std::string path);

    static bool writeFile(std::string path, std::string json);

    // create json strings
    static std::string makeObject(std::vector<std::string> properties); //TODO
    static std::string makeProperty(std::string key, std::string value); //TODO
    static std::string makeArray(std::vector<std::string> values); //TODO
};