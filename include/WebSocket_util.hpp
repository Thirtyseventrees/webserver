#ifndef WEBSOCKET_UTIL_HPP
#define WEBSOCKET_UTIL_HPP

#include <vector>
#include <string>
#include <cstdint>

#include "server.hpp"

std::string decode_websocket_frame(const std::vector<uint8_t>& buffer);
std::vector<uint8_t> build_websocket_text_frame(const std::string& message);

void websocket_response(void* ptr, EpollWrapper &ew);

#endif