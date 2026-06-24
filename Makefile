# MAVN prevodilac - build za Linux (g++)
CXX      := g++
CXXFLAGS := -std=c++14 -Wall -Wextra -MMD -MP -Isrc
TARGET   := mavn

SRC := $(wildcard src/*.cpp)
OBJ := $(SRC:.cpp=.o)
DEP := $(OBJ:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET) examples/simple.mavn

clean:
	rm -f $(OBJ) $(DEP) $(TARGET)

.PHONY: all run clean

-include $(DEP)
