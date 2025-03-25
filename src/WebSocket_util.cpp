#include "../include/WebSocket_util.hpp"

std::string decode_websocket_frame(const std::vector<uint8_t>& buffer) {
    if (buffer.size() < 6) return "";

    size_t i = 0;
    uint8_t fin = (buffer[i] & 0x80) >> 7;
    uint8_t opcode = buffer[i] & 0x0F;
    i++;

    uint8_t mask = (buffer[i] & 0x80) >> 7;
    uint64_t payload_len = buffer[i] & 0x7F;
    i++;

    if (payload_len == 126) {
        payload_len = (buffer[i] << 8) | buffer[i + 1];
        i += 2;
    } else if (payload_len == 127) {
        payload_len = 0;
        for (int j = 0; j < 8; ++j) {
            payload_len = (payload_len << 8) | buffer[i + j];
        }
        i += 8;
    }

    if (mask != 1) return "";  // 客户端发来的必须带掩码

    // 读取 masking key
    uint8_t masking_key[4];
    for (int j = 0; j < 4; ++j) {
        masking_key[j] = buffer[i++];
    }

    // 解码 payload
    std::string decoded;
    for (uint64_t j = 0; j < payload_len && i + j < buffer.size(); ++j) {
        char byte = buffer[i + j] ^ masking_key[j % 4];
        decoded += byte;
    }

    return decoded;
}

std::vector<uint8_t> build_websocket_text_frame(const std::string& message) {
    std::vector<uint8_t> frame;
    size_t len = message.size();

    // 第一个字节：FIN=1, opcode=0x1 (text)
    frame.push_back(0x81);

    // 第二个字节：mask=0（服务器不用掩码）
    if (len <= 125) {
        frame.push_back(static_cast<uint8_t>(len));
    } else if (len <= 65535) {
        frame.push_back(126);
        frame.push_back((len >> 8) & 0xFF);
        frame.push_back(len & 0xFF);
    } else {
        frame.push_back(127);
        for (int i = 7; i >= 0; --i) {
            frame.push_back((len >> (8 * i)) & 0xFF);
        }
    }

    // 加入 payload
    frame.insert(frame.end(), message.begin(), message.end());
    return frame;
}

void websocket_response(void* ptr, EpollWrapper &ew){
    uint8_t buffer[4096];
    std::vector<uint8_t> recv_buffer;
    while (true){
        ssize_t n = read(((connection*)ptr)->fd, buffer, sizeof(buffer));
        if(n <= 0)
            break;
        recv_buffer.insert(recv_buffer.end(), buffer, buffer + n);
    }

    std::string msg = decode_websocket_frame(recv_buffer);
    std::string combined_msg = fd_to_user[((connection*)ptr)->fd] + ": " + msg;
    std::vector<uint8_t> frame = build_websocket_text_frame(combined_msg);

    for(auto iter : user_to_connection){
        if(iter.second->fd != ((connection*)ptr)->fd)
            send(iter.second->fd, frame.data(), frame.size(), 0);
    }

    ew.mod_fd(ptr, ((connection*)ptr)->fd, EPOLLONESHOT | EPOLLIN | EPOLLERR | EPOLLRDHUP | EPOLLHUP | EPOLLET);
}