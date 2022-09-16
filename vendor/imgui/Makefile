OBJ_DIR = objs
SOURCES = $(wildcard *.cpp)
SOURCES += $(wildcard backends/*.cpp)
OBJS = $(addsuffix .cpp.o, $(basename $(notdir $(SOURCES))))
TARGET_OBJS = $(addprefix $(OBJ_DIR)/, $(OBJS))

CXXFLAGS = -std=c++11 -Ibackends -I.
CXXFLAGS += -g -Wall -Wformat

all:
	@mkdir -p objs
	@make --no-print-directory $(TARGET_OBJS)

$(OBJ_DIR)/%.cpp.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.cpp.o: backends/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJ_DIR)
