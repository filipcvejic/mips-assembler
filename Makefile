# MAVN prevodilac - build za Linux (g++)
CXX      := g++
CXXFLAGS := -std=c++14 -Wall -Wextra -Isrc
TARGET   := mavn

SRC := $(wildcard src/*.cpp)
OBJ := $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET) examples/simple.mavn

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all run clean