#include "utils.h"
#include <filesystem>
#include <string>

void delete_slash(fs::path& path) {
    std::string path_str = path.string();
    if(path_str.back() == '/') {
        path_str.pop_back();
    } 
    path = fs::path(path_str);
}