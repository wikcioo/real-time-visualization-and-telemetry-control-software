TARGET := app
CXX    := g++

BUILD_DIR   := build
USR_INC_DIR := include
USR_SRC_DIR := src
IMGUI_DIR   := vendor/imgui

CXX_INCLUDES := \
-I$(USR_INC_DIR) \
-I$(IMGUI_DIR) \
-I$(IMGUI_DIR)/backends

WARNINGS := \
-Wall \
-Wformat \
-Wpedantic \
-Wshadow

CXX_FLAGS := \
-std=c++14 \
$(CXX_INCLUDES) \
$(WARNINGS)

LD_FLAGS := \

USR_SOURCES := $(wildcard $(USR_SRC_DIR)/*.cpp)
USR_OBJECTS := $(addprefix $(BUILD_DIR)/, $(addsuffix .cpp.o, $(basename $(notdir $(USR_SOURCES)))))

OS_NAME := $(shell uname -s)

ifeq ($(OS_NAME), Linux)
	LD_FLAGS += `pkg-config --static --libs glfw3`
	LD_FLAGS += `pkg-config --libs glew`
endif

all:
	@if [ ! -d "$(IMGUI_DIR)/objs" ]; then\
		echo "Compiling imgui objects";\
		$(MAKE) --no-print-directory -C $(IMGUI_DIR)/;\
	fi
	@mkdir -p $(BUILD_DIR)
	@echo "Compiling local objects"
	@make --no-print-directory $(BUILD_DIR)/$(TARGET)
	@echo "Build complete for $(OS_NAME)"

$(BUILD_DIR)/$(TARGET): $(USR_OBJECTS)
	$(CXX) $(CXX_FLAGS) $(LD_FLAGS) $^ $(wildcard $(IMGUI_DIR)/objs/*.cpp.o) -o $@

$(BUILD_DIR)/%.cpp.o: $(USR_SRC_DIR)/%.cpp $(USR_INC_DIR)/%.hpp
	$(CXX) -c $(CXX_FLAGS) $< -o $@

clean:
	rm -rf $(BUILD_DIR)

# Performs deep clean i.e. also clean dependencies
cleand:
	@echo "Performing deep clean"
	@$(MAKE) --no-print-directory clean
	@$(MAKE) --no-print-directory -C $(IMGUI_DIR) clean
