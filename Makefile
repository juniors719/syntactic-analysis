CXX = g++
CXXFLAGS = -Iinclude -std=c++11
SOURCES = main.cpp src/automata.cpp src/file_reader.cpp src/first_follow.cpp src/globals.cpp src/parser.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = syntactic_analysis

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

run: $(TARGET)
	./$(TARGET) parse

first_follow: $(TARGET)
	./$(TARGET) first_follow

automaton: $(TARGET)
	./$(TARGET) automaton

parse: $(TARGET)
	./$(TARGET) parse