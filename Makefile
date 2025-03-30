CXX = g++
CXXFLAGS = -std=c++17 -pthread -g -Wall -Wextra
TARGET = main.exe
SRCS = main.cpp DynamicThreadPool.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
