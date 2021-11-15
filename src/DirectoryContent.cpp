#include "DirectoryContent.h"

DirectoryContent::DirectoryContent(bool type, std::string name) {
    this->type = type;
    this->name = name;
}
DirectoryContent::~DirectoryContent() {}

/**
 * @brief Get this element's name
 * 
 * @return true if this is a dircetory
 * @return false if this is a file
 */
bool DirectoryContent::getType(){
    return this->type;
}
/**
 * @brief get the element's name
 * 
 * @return std::string the name
 */
std::string DirectoryContent::getName(){
    return this->name;
}

/**
 * @brief return the json view of the object
 * 
 * @return std::string 
 */
// std::string DirectoryContent::toJson(){
//     return "called the wrong method";
// };