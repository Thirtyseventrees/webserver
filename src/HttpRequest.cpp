#include "../include/HttpRequest.hpp"

HttpRequest parse_HttpRequest(const std::string& request){
    std::istringstream stream(request);
    HttpRequest req;
    std::string line;

    if(std::getline(stream, line)){
        std::istringstream line_stream(line);
        std::string method_str, version_str;
        line_stream >> method_str >> req.url_ >> version_str;
        req.method_ = method_map_.find(method_str)->second;
        req.version_ = version_map_.find(version_str)->second;
    }

    while(std::getline(stream, line) && line != "\r"){
        size_t pos = line.find(": ");
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 2);
            req.headers_[key] = value;
        }
    }

    if (req.headers_.count("Content-Length")) {
        std::getline(stream, req.body, '\0');
    }

    return req;
}

std::string formatHttpRequest(const HttpRequest& request) {
    std::ostringstream output;

    // 1. 请求行
    output << method_to_string.at(request.method_) << " "
           << request.url_ << " "
           << version_to_string.at(request.version_) << "\r\n";

    // 2. 请求头
    for (const auto& [key, value] : request.headers_) {
        output << key << ": " << value << "\r\n";
    }

    // 3. 空行（分隔请求头和请求体）
    output << "\r\n";

    // 4. 请求体（如果存在）
    if (!request.body.empty()) {
        output << request.body;
    }

    return output.str();
}