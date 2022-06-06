#include "DirScanner.h"

DirScanner::DirScanner() {}

DirScanner::~DirScanner() {}

/**
 * @brief Scan the content in all the given paths
 * 
 * @param paths vector of std::string of paths
 * @return std::string JSON data of all the scanned content
 */
std::string DirScanner::scanPaths(std::vector<std::string> paths) {
    std::vector<std::string> paths_scanned;
    for (std::string path : paths) {
        std::vector<std::string> content = DirScanner::scanPath(path);
        paths_scanned.insert(paths_scanned.end(), content.begin(), content.end());
    }
    std::string pro = JsonControler::makeProperty("content", JsonControler::makeArray(paths_scanned));
    return JsonControler::makeObject(std::vector<std::string>{pro});
}
/**
 * @brief iterates through directory recursively to scan all the files in it
 * 
 * @param path complete string path of the directory
 * @return std::string JSON data of the files scanned
 */
std::vector<std::string> DirScanner::scanPath(std::string path) {
    // std::string jsonData = "";
    std::vector<std::string> files;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            std::vector<std::string> properties;

            // filename of the current entry (could be a file or a directory)
            std::string filename = entry.path().filename().string();

            // regex matches on the element's names
            std::cmatch m; // 1:name 3:quality 4:type 5:year 8:season 10:episode
            std::regex exp ("^([^\\(\\.]+)(\\ \\((imax|fullhd|4k)\\))?(\\ \\((\\d+)?(TV)?(S(\\d+)(E(\\d+))?)?\\))?(\\.\\w+)?$");
            std::regex_match(filename.c_str(), m, exp);

            // Add the complete filename and path
            #ifdef _WIN32
                properties.push_back(JsonControler::makeProperty("filename", std::regex_replace(entry.path().string(), std::regex("\\\\"), "\\\\")));
            #else
                properties.push_back(JsonControler::makeProperty("filename", entry.path().string()));
            #endif

            // Add the element's name only (not the series number, etc)
            properties.push_back(JsonControler::makeProperty("name", m[1]));
            // Add the year if there is one
            if (m[4].length() == 7) properties.push_back(JsonControler::makeProperty("fileYear", m[5]));
            else properties.push_back(JsonControler::makeProperty("fileYear", ""));

            // If the element is a directory
            if (entry.is_directory()) {
                switch (m[4].length()) {
                    case 5: // it's a TV show " (TV)"
                        properties.push_back(JsonControler::makeProperty("type", "tvshow"));
                        break;
                    case 6: // it's a TV season " (S01)"
                        properties.push_back(JsonControler::makeProperty("type", "tvseason"));
                        properties.push_back(JsonControler::makeProperty("season", std::to_string(std::stoi(m[8]))));
                        break;
                    default: //  it's collection of movies
                        properties.push_back(JsonControler::makeProperty("type", "collection"));
                        break;
                }
            } else { // it is a file
                if (m[3].length())
                    properties.push_back(JsonControler::makeProperty("quality", m[3]));
                if (m[4].length() > 7) {
                    // TV episode
                    properties.push_back(JsonControler::makeProperty("type", "tvep"));
                    // parse to int then back to string to get the actual number (01 to 1)
                    properties.push_back(JsonControler::makeProperty("season", std::to_string(std::stoi(m[8]))));
                    properties.push_back(JsonControler::makeProperty("episode", std::to_string(std::stoi(m[10]))));
                } else {
                    properties.push_back(JsonControler::makeProperty("type", "movie"));
                }
            }
            files.push_back(JsonControler::makeObject(properties));
        }
    } catch(const std::exception& e) {
        // Unreadable directory
        std::cerr << e.what() << '\n';
    }
    return files;
}