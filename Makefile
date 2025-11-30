CXX = g++
CXXFLAGS = -Wall -std=c++11

BUILD_DIR = build
TARGET_NAME = app
TARGET = $(BUILD_DIR)/$(TARGET_NAME)

SOURCES = main.cpp \
          displ_add_line_ctrl.cpp\
		  tft.cpp\
		  spi_bus.cpp

OBJECTS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SOURCES))


all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

$(BUILD_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)