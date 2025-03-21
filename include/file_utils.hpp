#pragma once

#include <fstream>
#include <string>
#include <string_view>
#include <algorithm>

inline std::string file_get_content(std::string const &path, std::string const &mime_types){
    std::ifstream file;
    if(mime_types.find("text") != std::string::npos){
        file.open(path);
        if(!file.is_open()){
            return {};
        }
    }
    else if(mime_types.find("image") != std::string::npos){
        file.open(path, std::ios::binary);
        if(!file.is_open()){
            return {};
        }
    }
    std::string content{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
    return content;
}

inline void file_put_content(std::string const &path, std::string_view content){
    std::ofstream file(path);
    if(!file.is_open()){
        return;
    }
    std::copy(content.begin(), content.end(), std::ostreambuf_iterator<char>(file));
}