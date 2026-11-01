CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -Isrc

TARGET   := detector
SRC_DIR  := src
BUILD_DIR := build

SRCS     := $(wildcard $(SRC_DIR)/*.cpp)
OBJS     := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "Build successful: ./$(TARGET)"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) $(TARGET) cpp-plagiarism-detector.zip
	@echo "Cleaned build artifacts."

.PHONY: all clean
