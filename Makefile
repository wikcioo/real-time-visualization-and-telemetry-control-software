TARGET := app
CXX    := g++

BUILD_DIR := build
SRC_DIR   := src
IMGUI_DIR := vendor/imgui

CXX_INCLUDES := \
-I$(SRC_DIR) \
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

SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
SOURCES += $(wildcard $(SRC_DIR)/core/*.cpp)
SOURCES += $(wildcard $(SRC_DIR)/graphics/*.cpp)
SOURCES += $(wildcard $(SRC_DIR)/serial/*.cpp)
SOURCES += $(wildcard $(SRC_DIR)/ui/*.cpp)

OBJECTS := $(addprefix $(BUILD_DIR)/, $(addsuffix .cpp.o, $(basename $(notdir $(SOURCES)))))

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

$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	$(CXX) $(CXX_FLAGS) $(LD_FLAGS) $^ $(wildcard $(IMGUI_DIR)/objs/*.cpp.o) -o $@

$(BUILD_DIR)/%.cpp.o: $(SRC_DIR)/%.cpp
	$(CXX) -c $(CXX_FLAGS) $< -o $@

$(BUILD_DIR)/%.cpp.o: $(SRC_DIR)/core/%.cpp $(SRC_DIR)/core/%.hpp
	$(CXX) -c $(CXX_FLAGS) $< -o $@

$(BUILD_DIR)/%.cpp.o: $(SRC_DIR)/graphics/%.cpp $(SRC_DIR)/graphics/%.hpp
	$(CXX) -c $(CXX_FLAGS) $< -o $@

$(BUILD_DIR)/%.cpp.o: $(SRC_DIR)/serial/%.cpp $(SRC_DIR)/serial/%.hpp
	$(CXX) -c $(CXX_FLAGS) $< -o $@

$(BUILD_DIR)/%.cpp.o: $(SRC_DIR)/ui/%.cpp $(SRC_DIR)/ui/%.hpp
	$(CXX) -c $(CXX_FLAGS) $< -o $@

clean:
	rm -rf $(BUILD_DIR)

# Performs deep clean i.e. also clean dependencies
cleand:
	@echo "Performing deep clean"
	@$(MAKE) --no-print-directory clean
	@$(MAKE) --no-print-directory -C $(IMGUI_DIR) clean
