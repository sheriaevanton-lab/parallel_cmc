CXX      := g++
CXXFLAGS := -O2 -std=c++17 -Wall -Wextra -I. -Igeometry -Iutils -Iproblem_data
LDFLAGS  :=

SRCS := main.cpp geometry/geometry.cpp utils/utils.cpp problem_data/problem_data.cpp
BIN  := solver

all: $(BIN)

$(BIN): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(BIN) $(LDFLAGS)

clean:
	rm -f $(BIN)

.PHONY: all clean