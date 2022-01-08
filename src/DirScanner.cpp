#include "DirScanner.h"

DirScanner::DirScanner(/* args */)
{
}

DirScanner::~DirScanner()
{
}

/**
 * @brief Scan the content in all the given paths
 * 
 * @param paths vector of std::string of paths
 * @return std::string JSON data of all the scanned content
 */
std::string DirScanner::scanPaths(std::vector<std::string> paths) {
    std::string scanData = "{\"content\":[";
    for(std::string path : paths) {
        // append all the paths scanned data
        std::string output = DirScanner::scanPath(path);
        if (output.length() > 0) output.append(",");
        scanData.append(output);
    }
    // remove the last comma, before closing the array
    if (scanData.back() == ',') scanData.pop_back();
    scanData.append("]}");
    return scanData;
}
/**
 * @brief iterates through directory recursively to scan all the files in it
 * 
 * @param path complete string path of the directory
 * @return std::string JSON data of the files scanned
 */
std::string DirScanner::scanPath(std::string path) {
    std::string jsonData = "";
    try {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            // filename of the current entry (could be a file or a directory)
            std::string filename = entry.path().filename().string();
            // regex matches on the element's names
            std::cmatch m; // 1:name 3:quality 4:type 5:year 8:season 10:episode
            std::regex exp ("^([^\\(\\.]+)(\\ \\((imax|fullhd|4k)\\))?(\\ \\((\\d+)?(TV)?(S(\\d+)(E(\\d+))?)?\\))?(\\.\\w+)?$");
            std::regex_match(filename.c_str(), m, exp);

            // for (size_t i = 0; i < m.length(); i++)
            //     std::cout << i << ": " << m[i] << "\t";
            // std::cout << '\n';

            // Add the complete filename
            #ifdef _WIN32
                jsonData.append("{\"filename\":\"").append(std::regex_replace(entry.path().string(), std::regex("\\\\"), "\\\\")).append("\",");
            #else
                jsonData.append("{\"filename\":\"").append(entry.path().string()).append("\",");
            #endif
            // Add the element's name only (not the series number, etc)
            jsonData.append("\"name\":\"").append(m[1]).append("\",");
            // Add the year if there is one
            if (m[4].length() == 7) jsonData.append("\"fileYear\":\"").append(m[5]).append("\",");
            else jsonData.append("\"fileYear\":\"\",");

            // If the element is a directory
            if (entry.is_directory()) {
                switch (m[4].length()) {
                    case 5: // it's a TV show " (TV)"
                        jsonData.append("\"type\":\"tvshow\"");
                        break;
                    case 6: // it's a TV season " (S01)"
                        jsonData.append("\"type\":\"tvseason\",");
                        jsonData.append("\"season\":\"").append(std::to_string(std::stoi(m[8]))).append("\"");
                        break;
                    default: //  it's collection of movies
                        jsonData.append("\"type\":\"collection\"");
                        break;
                }
                // recursively add directory's content
                // jsonData.append("\"content\":[").append(scanDirectory(entry.path().string())).append("]");
            } else { // it is a file
                if (m[3].length())
                    jsonData.append("\"quality\":\"").append(m[3]).append("\",");
                // if m[3] is longer than " (xxxx)" then it can only be a tv episode " (SxxExx)"
                if (m[4].length() > 7) {
                    // TV episode
                    jsonData.append("\"type\":\"tvep\",");
                    // parse to int then back to string to get the actual number (01 to 1)
                    jsonData.append("\"season\":\"").append(std::to_string(std::stoi(m[8]))).append("\",");
                    jsonData.append("\"episode\":\"").append(std::to_string(std::stoi(m[10]))).append("\"");
                } else {
                    jsonData.append("\"type\":\"movie\"");
                }
            }
            jsonData.append("},");
        }
        if (jsonData.length()) {
            if (jsonData.back() == ',') jsonData.pop_back();
        }
    } catch(const std::exception& e) {
        // TODO unreadable directory
        std::cerr << e.what() << '\n';
    }
    // std::cout << jsonData << std::endl;
    return jsonData;
}