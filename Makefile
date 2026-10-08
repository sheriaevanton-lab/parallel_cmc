CXX      := mpicxx
CXXFLAGS := -O2 -std=c++17 -fopenmp -Wall -Wextra -I. -Isrc -MMD -MP

SRCS := $(shell find src -name '*.cpp') main.cpp
OBJ_DIR := build
OBJS := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

BIN := prog

all: $(BIN)

$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(DEPS)

clean:
	rm -rf $(OBJ_DIR) $(BIN)

.PHONY: all clean