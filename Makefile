EXE = ccs
INC_DIR = include
SRC_DIR = src
BUILD_DIR = build
SOURCES += $(SRC_DIR)/main.cpp\
		   $(SRC_DIR)/application.cpp\
		   $(SRC_DIR)/window.cpp\
		   $(SRC_DIR)/shader.cpp\
		   $(SRC_DIR)/opengl_buffer_manager.cpp
OBJS = $(addprefix $(BUILD_DIR)/, $(addsuffix .cpp.o, $(basename $(notdir $(SOURCES)))))
UNAME_S := $(shell uname -s)

CXXFLAGS = -I$(INC_DIR)
CXXFLAGS += -g -Wall -Wformat -Wpedantic -Wshadow
LIBS =

ifeq ($(UNAME_S), Linux)
	ECHO_MESSAGE = "Linux"
	LIBS += `pkg-config --static --libs glfw3`
	LIBS += `pkg-config --libs glew`
endif

all:
	@mkdir -p build
	@make --no-print-directory $(BUILD_DIR)/$(EXE)
	@echo "Build complete for $(ECHO_MESSAGE)"

$(BUILD_DIR)/$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

$(BUILD_DIR)/%.cpp.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -rf $(BUILD_DIR)
