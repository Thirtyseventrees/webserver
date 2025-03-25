#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <unordered_map>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>

#include "../include/file_utils.hpp"
#include "../include/myjson.hpp"

enum ProcessState {
    STATE_PARSE_URI = 1,
    STATE_PARSE_HEADERS,
    STATE_RECV_BODY,
    STATE_ANALYSIS,
    STATE_FINISH
  };
  
  enum URIState {
    PARSE_URI_AGAIN = 1,
    PARSE_URI_ERROR,
    PARSE_URI_SUCCESS,
  };
  
  enum HeaderState {
    PARSE_HEADER_SUCCESS = 1,
    PARSE_HEADER_AGAIN,
    PARSE_HEADER_ERROR
  };
  
  enum AnalysisState { ANALYSIS_SUCCESS = 1, ANALYSIS_ERROR };
  
  enum ParseState {
    H_START = 0,
    H_KEY,
    H_COLON,
    H_SPACES_AFTER_COLON,
    H_VALUE,
    H_CR,
    H_LF,
    H_END_CR,
    H_END_LF
  };
  
  enum ConnectionState { H_CONNECTED = 0, H_DISCONNECTING, H_DISCONNECTED };
  
  enum HttpMethod { 
    GET,
    POST,
    HEAD,
    PUT,
    DELETE,
    OPTIONS,
    PATCH,
    UNKNOWN
  };
  
  enum HttpVersion { HTTP_1_0, HTTP_1_1, HTTP_2_0, UNKNOW };

const std::unordered_map<std::string, HttpMethod> method_map_ = {
    {"GET", HttpMethod::GET},
    {"POST", HttpMethod::POST},
    {"HEAD", HttpMethod::HEAD}
};

const std::unordered_map<std::string, HttpVersion> version_map_ = {
    {"HTTP/1.0", HttpVersion::HTTP_1_0},
    {"HTTP/1.1", HttpVersion::HTTP_1_1},
    {"HTTP/2.0", HttpVersion::HTTP_2_0}
};

const std::map<HttpMethod, std::string> method_to_string = {
    {HttpMethod::GET, "GET"},
    {HttpMethod::POST, "POST"},
    {HttpMethod::PUT, "PUT"},
    {HttpMethod::DELETE, "DELETE"},
    {HttpMethod::HEAD, "HEAD"},
    {HttpMethod::OPTIONS, "OPTIONS"},
    {HttpMethod::PATCH, "PATCH"},
    {HttpMethod::UNKNOWN, "UNKNOWN"}
};

const std::map<HttpVersion, std::string> version_to_string = {
    {HttpVersion::HTTP_1_0, "HTTP/1.0"},
    {HttpVersion::HTTP_1_1, "HTTP/1.1"},
    {HttpVersion::HTTP_2_0, "HTTP/2.0"},
    {HttpVersion::UNKNOW, "UNKNOWN"}
};

const std::unordered_map<std::string, std::string> mime_types = {
    {".html", "text/html"},
    {".htm", "text/html"},
    {".css", "text/css"},
    {".js", "application/javascript"},
    {".json", "application/json"},
    {".xml", "application/xml"},
    {".txt", "text/plain"},
    {".png", "image/png"},
    {".jpg", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".gif", "image/gif"},
    {".svg", "image/svg+xml"},
    {".ico", "image/x-icon"},
    {".mp4", "video/mp4"},
    {".mp3", "audio/mpeg"},
    {".pdf", "application/pdf"},
    {".zip", "application/zip"}
};

const std::unordered_map<std::string, std::string> file_path = {
    {"/", "html/index.html"},
    {"/dashboard", "html/dashboard.html"}
};

// Http Request
struct HttpRequest{
    HttpMethod method_;
    HttpVersion version_;
    std::string url_;
    std::unordered_map<std::string, std::string> query_params_;
    std::unordered_map<std::string, std::string> headers_;
    bool keep_alive_;
    std::string body;
};

void parse_url(HttpRequest& request, const std::string& url);
HttpRequest parse_HttpRequest(const std::string& request);
std::string url_to_filePath(const std::string& url);
std::string get_mime_type(const std::string &filename);

std::string formatHttpRequest(const HttpRequest& request);

std::string calculate_websocket_accept(const std::string& key);

// Http Response
class HttpResponse{
    public:
    HttpResponse() : version_("HTTP/1.1"), status_code_(200), reason_phrase_("OK") {}

    void set_version(const std::string& version);
    void set_statusCode(int code);
    void set_reasonPhrase(const std::string& reason);
    void set_header(const std::string& key, const std::string& value);
    void set_body(const std::string& body);
    std::string HttpResponse_to_string() const;

    private:
    std::string version_;
    int status_code_;       // e.g. 200
    std::string reason_phrase_;  // e.g. "OK"
    std::unordered_map<std::string, std::string> headers_;
    std::string body_;
};

HttpResponse make_ok_response(const HttpRequest& http_request_);
HttpResponse make_login_response(const HttpRequest& http_request_);
HttpResponse make_upgrade_response(const HttpRequest& http_request_);

#endif