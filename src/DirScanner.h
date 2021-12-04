#include <string>
#include <filesystem>
#include <regex>
#include <iostream>
#include <vector>

class DirScanner {
private:
    
public:
    DirScanner();
    ~DirScanner();

    static std::string scanPaths(std::vector<std::string> paths);
    static std::string scanPath(std::string path);
};
