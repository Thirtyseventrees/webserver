#include "../include/HttpData.hpp"

std::string utf8_decode(const std::string& str)
{
    std::string decoded;
    decoded.reserve(str.size());  // 先分配足够空间

    for (size_t i = 0; i < str.size(); ++i) {
        char c = str[i];
        if (c == '%') {
            // 若剩余长度不足2，说明格式错误，这里简单处理
            if (i + 2 < str.size()) {
                // 解析 '%xx' 两位十六进制
                std::string hexValue = str.substr(i+1, 2);
                // 将hexValue转为int
                unsigned int val;
                std::istringstream iss(hexValue);
                iss >> std::hex >> val;
                // 放入decoded字符串
                decoded.push_back(static_cast<char>(val));
                i += 2; // 跳过这2位
            }
            // 若不够2位，这里你可根据需求处理错误
        } 
        else if (c == '+') {
            // 有些情况下 '+' 表示空格
            decoded.push_back(' ');
        }
        else {
            // 普通字符原样加入
            decoded.push_back(c);
        }
    }

    return decoded;
}

void parse_url(HttpRequest& request, const std::string& url){
    auto pos = url.find('?');
    std::string qs;
    if(pos == std::string::npos){
        request.url_ = url;
    }
    else{
        request.url_ = url.substr(0, pos);
        qs = url.substr(pos + 1);
    }
    if(qs.empty())
        return;
    std::istringstream iss(qs);
    std::string kv;
    while(std::getline(iss, kv, '&')){
        auto pos = kv.find('=');
        if(pos == std::string::npos){
            request.query_params_[kv] = "";
        }
        else{
            auto key = kv.substr(0, pos);
            auto value = utf8_decode(kv.substr(pos + 1));
            request.query_params_[key] = value;
        }
    }
}

HttpRequest parse_HttpRequest(const std::string& request){
    std::istringstream stream(request);
    HttpRequest req;
    std::string line;

    if(std::getline(stream, line)){
        std::istringstream line_stream(line);
        std::string method_str, url_str, version_str;
        line_stream >> method_str >> url_str >> version_str;
        req.method_ = method_map_.find(method_str)->second;
        req.version_ = version_map_.find(version_str)->second;
        parse_url(req, url_str);
    }

    while(std::getline(stream, line) && line != "\r"){
        size_t pos = line.find(": ");
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 2);
            if (!value.empty() && value.back() == '\r') {
                value.pop_back();
            }
            req.headers_[key] = value;
        }
    }

    if (req.headers_.count("Content-Length")) {
        size_t content_length = std::stoul(req.headers_["Content-Length"]);  // 获取 `Content-Length`
        req.body.resize(content_length);  // 预分配 `body` 空间
        stream.read(&req.body[0], content_length);  // **精确读取 `content_length` 字节**
    }

    //Connection
    if(req.headers_["Connection"] == "keep-alive")
        req.keep_alive_ = true;
    else
        req.keep_alive_ = false;

    return req;
}

std::string url_to_filePath(const std::string& url){
    if(url.find("..") != std::string::npos){
        return "";
    }
    if(file_path.find(url) != file_path.end()){
        return file_path.at(url);
    }

    return url.substr(1);
}

std::string get_mime_type(const std::string &filename){
    size_t dot_pos = filename.find_last_of(".");
    if (dot_pos != std::string::npos) {
        std::string ext = filename.substr(dot_pos);  // 提取后缀
        auto it = mime_types.find(ext);
        if (it != mime_types.end()) {
            return it->second;  // 返回匹配的 MIME 类型
        }
    }
    return "application/octet-stream";  // 未知类型，默认二进制流
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

std::string calculate_websocket_accept(const std::string& key){
    std::string accept_key = key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    // 计算 SHA1 哈希
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(accept_key.c_str()), accept_key.size(), hash);

    // Base64 编码
    BIO* bio, * b64;
    BUF_MEM* bufferPtr;
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(bio, hash, SHA_DIGEST_LENGTH);
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);
    std::string accept_value(bufferPtr->data, bufferPtr->length);
    BIO_free_all(bio);

    return accept_value;
}

/******************************************************************* */
// Http response functions
void HttpResponse::set_version(const std::string& version){
    version_ = version;
}

void HttpResponse::set_statusCode(int code){
    status_code_ = code;
}

void HttpResponse::set_reasonPhrase(const std::string& reason){
    reason_phrase_ = reason;
}

void HttpResponse::set_header(const std::string& key, const std::string& value){
    headers_[key] = value;
}

void HttpResponse::set_body(const std::string& body) {
    body_ = body;
}

std::string HttpResponse::HttpResponse_to_string() const{
    std::ostringstream response;

    response << version_ << " " << status_code_ << " " << reason_phrase_ << "\r\n";

    if(headers_.find("Content-Length") == headers_.end()){
        response << "Content-Length: " << body_.size() << "\r\n";
    }

    for (auto &kv : headers_) {
        response << kv.first << ": " << kv.second << "\r\n";
    }

    response << "\r\n";

    response << body_;

    return response.str();
}

HttpResponse make_ok_response(const HttpRequest& http_request_){
    HttpResponse http_response_;

    std::string file_path_ = url_to_filePath(http_request_.url_);
    std::string mime_type_ = get_mime_type(file_path_);
    std::string content = file_get_content(file_path_, mime_type_);

    http_response_.set_version(version_to_string.at(http_request_.version_));
    http_response_.set_header("Content-Type: ", mime_type_);
    http_response_.set_body(content);

    return http_response_;
}

HttpResponse make_login_response(const HttpRequest& http_request_){
    HttpResponse http_response_;

    std::string content_type_ = http_request_.headers_.at("Content-Type");

    if(content_type_ == "application/json"){
        myjson json = myjson::parse(http_request_.body);
        std::string username = json["username"];
        std::string password = json["password"];
        //to do
        //check the password

        std::string response_body = R"({"success": true})";
        http_response_.set_version(version_to_string.at(http_request_.version_));
        http_response_.set_header("Content-Type", "application/json");
        //to do
        //implement cookie generate
        http_response_.set_header("Set-Cookie", "session_id=12345; HttpOnly;");
        http_response_.set_body(response_body);
    }
    return http_response_;
}

HttpResponse make_upgrade_response(const HttpRequest& http_request_){
    HttpResponse http_response_;
    std::string sec_key = http_request_.headers_.at("Sec-WebSocket-Key");
    http_response_.set_statusCode(101);
    http_response_.set_reasonPhrase("Switching Protocols");
    http_response_.set_header("Upgrade", "websocket");
    http_response_.set_header("Connection", "Upgrade");
    http_response_.set_header("Sec-WebSocket-Accept", calculate_websocket_accept(sec_key));
    return http_response_;
}