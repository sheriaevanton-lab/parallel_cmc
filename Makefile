CXX      := g++
CXXFLAGS := -O2 -std=c++17 -Wall -Wextra -I. -Igeometry
LDFLAGS  :=

SRCS := main.cpp geometry/geometry.cpp geometry/utils.cpp
BIN  := solver

all: $(BIN)

$(BIN): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(BIN) $(LDFLAGS)

clean:
	rm -f $(BIN)

.PHONY: all clean