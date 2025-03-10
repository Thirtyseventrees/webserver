#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

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

const std::unordered_map<HttpMethod, std::string> method_to_string = {
    {HttpMethod::GET, "GET"},
    {HttpMethod::POST, "POST"},
    {HttpMethod::PUT, "PUT"},
    {HttpMethod::DELETE, "DELETE"},
    {HttpMethod::HEAD, "HEAD"},
    {HttpMethod::OPTIONS, "OPTIONS"},
    {HttpMethod::PATCH, "PATCH"},
    {HttpMethod::UNKNOWN, "UNKNOWN"}
};

const std::unordered_map<HttpVersion, std::string> version_to_string = {
    {HttpVersion::HTTP_1_0, "HTTP/1.0"},
    {HttpVersion::HTTP_1_1, "HTTP/1.1"},
    {HttpVersion::HTTP_2_0, "HTTP/2.0"},
    {HttpVersion::UNKNOW, "UNKNOWN"}
};

struct HttpRequest{
    HttpMethod method_;
    HttpVersion version_;
    std::string url_;
    std::unordered_map<std::string, std::string> headers_;
    std::string body;
};

HttpRequest parse_HttpRequest(const std::string& request);
std::string formatHttpRequest(const HttpRequest& request);

#endif