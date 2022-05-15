#include <string>
#include <filesystem>
#include <regex>
#include <iostream>
#include <vector>

#include "JsonControler.h"

class DirScanner {
private:
    
public:
    DirScanner();
    ~DirScanner();

    static std::string scanPaths(std::vector<std::string> paths);
    static std::vector<std::string> scanPath(std::string path);
};
