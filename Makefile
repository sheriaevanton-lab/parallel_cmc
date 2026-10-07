CXX      := g++
CXXFLAGS := -O2 -std=c++17 -Wall -Wextra -I. -Igeometry -Iutils -Iproblem_data -Isequential -I solver
LDFLAGS  :=

SRCS := main.cpp geometry/geometry.cpp utils/utils.cpp problem_data/problem_data.cpp solver/sequential.cpp solver/solver.cpp
BIN  := prog

all: $(BIN)

$(BIN): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(BIN) $(LDFLAGS)

clean:
	rm -f $(BIN)

.PHONY: all clean