#include "JsonControler.h"

JsonControler::JsonControler() {}

JsonControler::~JsonControler() {}

std::string JsonControler::readFile(std::string path) {
    if (!JsonControler::checkFile(path)) return "file can't be created";

    std::string json = "";
    std::ifstream file(path);
    std::string line;

    if (file.is_open()) {
        while (std::getline(file, line)) {
            json.append(line);
        }
    } else {
        std::cout << "can't open file : " << path << std::endl;
        return "file can't be opened";
    }
    return json;
}

bool JsonControler::writeFile(std::string path, std::string json) {
    if (!JsonControler::checkFile(path)) return false;

    std::ofstream file(path);
    if (file.is_open()){
        file << json;
        file.close();
        return true;
    }

    return false;
}

bool JsonControler::checkFile(std::string path) {
    if (std::filesystem::exists(std::filesystem::path {path})) {
        return true;
    } else {
        return false;
    }
}

std::string JsonControler::makeObject(std::vector<std::string> properties) {
    std::string object = "{";
    for (std::string property : properties){
        object += property + ",";
    }
    if (object.back() == ',') object.pop_back();
    object.append("}");
    return object;
}

std::string JsonControler::makeProperty(std::string key, std::string value) {
    if (value[0] != '{' && value[0] != '[' ) value = std::string("\"") + value + "\"";
    return std::string("\"") + key + "\":" + value;
}

std::string JsonControler::makeArray(std::vector<std::string> values) {
    std::string arr = "[";
    for(std::string value : values) {
        arr += value + ",";
    }
    if (arr.back() == ',') arr.pop_back();
    arr.append("]");
    return arr;
}