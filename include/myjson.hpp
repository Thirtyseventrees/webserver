#ifndef MYJSON_HPP
#define MYJSON_HPP

#include <iostream>
#include <string>
#include <unordered_map>
#include <regex>

class myjson{
    public:
    static myjson parse(const std::string& text) noexcept{
        myjson j;
        std::regex pair_regex("\"(.*?)\"\\s*:\\s*\"(.*?)\"");
        auto begin = std::sregex_iterator(text.begin(), text.end(), pair_regex);
        auto end = std::sregex_iterator();

        for (auto it = begin; it != end; ++it) {
            std::string key = (*it)[1].str();
            std::string value = (*it)[2].str();
            j.data_[key] = value;
        }
        return j;
    }

    const std::string& operator[](const std::string& key) const{
        static const std::string empty;
        auto iter = data_.find(key);
        return iter != data_.end() ? iter->second : empty;
    }

    std::string& operator[](const std::string& key){
        return data_[key];
    }

    private:
    std::unordered_map<std::string, std::string> data_;
};

#endif