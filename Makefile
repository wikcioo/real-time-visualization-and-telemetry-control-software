EXE = ccs
INC_DIR = include
SRC_DIR = src
BUILD_DIR = build
IMGUI_DIR = vendor/imgui
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(addprefix $(BUILD_DIR)/, $(addsuffix .cpp.o, $(basename $(notdir $(SOURCES)))))
IMGUI_OBJS =
UNAME_S := $(shell uname -s)

CXXFLAGS = -I$(INC_DIR) -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends
CXXFLAGS += -g -Wall -Wformat -Wpedantic -Wshadow
LIBS =

ifeq ($(UNAME_S), Linux)
	ECHO_MESSAGE = "Linux"
	LIBS += `pkg-config --static --libs glfw3`
	LIBS += `pkg-config --libs glew`
endif

all:
	@if [ ! -d "$(IMGUI_DIR)/objs" ]; then\
		echo "Compiling imgui objects";\
		$(MAKE) --no-print-directory -C $(IMGUI_DIR)/;\
	fi
	@mkdir -p build
	@echo "Compiling local objects"
	@make --no-print-directory $(BUILD_DIR)/$(EXE)
	@echo "Build complete for $(ECHO_MESSAGE)"

$(BUILD_DIR)/$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(wildcard $(IMGUI_DIR)/objs/*.cpp.o) $(CXXFLAGS) $(LIBS)

$(BUILD_DIR)/%.cpp.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -rf $(BUILD_DIR)

# Performs deep clean i.e. also clean dependencies
cleand:
	@echo "Performing deep clean"
	@$(MAKE) --no-print-directory clean
	@$(MAKE) --no-print-directory -C $(IMGUI_DIR) clean
