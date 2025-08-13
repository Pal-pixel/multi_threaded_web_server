#include <fstream>
#include <map>
#include <string>

std::map<std::string, std::string> loadConfig(const std::string &filename) {
    std::map<std::string, std::string> config;
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        size_t delim = line.find('=');
        if (delim != std::string::npos) {
            std::string key = line.substr(0, delim);
            std::string value = line.substr(delim + 1);
            config[key] = value;
        }
    }
    return config;
}
