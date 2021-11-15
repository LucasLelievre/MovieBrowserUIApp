#include "Directory.h"

Directory::Directory(std::string name) : DirectoryContent(true, name) {
    this->content = {};
}
Directory::~Directory() {}

/**
 * @brief add an element in the directory's list 
 * 
 * @param newContent 
 */
void Directory::addContent(DirectoryContent* newContent) {
    this->content.push_back(newContent);
}

/**
 * @brief return the json view of the object
 * 
 * @return std::string
 */
std::string Directory::toJson(){
    std::string output = "{";
    output.append("\"type\":\"directory\",")
            .append("\"name\":\"").append(this->getName()).append("\",")
            .append("\"content\":[");
    
    for (size_t i = 0; i < content.size(); i++) {
        output.append(content.at(i)->toJson());
        if (i < content.size()-1) output.append(",");
    }
            
    /*for (DirectoryContent* c : content) {
        output.append(c->toJson());
    }*/
    return output.append("]}");
};