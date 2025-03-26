# 编译器 & 编译选项
CC = g++
CFLAGS = -Wall -Iinclude -pthread
CFLAGS_CHECK = -Wall -Iinclude -pthread -fsanitize=address -fno-omit-frame-pointer
LDFLAGS = -lssl -lcrypto

# 目录定义
SRC_DIR = src
INC_DIR = include
BIN_DIR = bin
OBJ_DIR = build

# 代码文件 & 目标文件
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))

TARGET = $(BIN_DIR)/main

# 默认编译 & 运行主程序
all: $(TARGET)
	./bin/main 0.0.0.0 8080

$(TARGET): $(OBJ_FILES)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# ===============================
# 运行 AddressSanitizer (ASan) 版本的主程序
# ===============================
check: $(TARGET)_asan

$(TARGET)_asan: $(OBJ_FILES)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS_CHECK) $^ -o $@ $(LDFLAGS)
	./$(TARGET)_asan

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
