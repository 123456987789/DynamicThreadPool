CXX = g++
CXXFLAGS = -std=c++17 -pthread -g -Wall -Wextra
OBJDIR = obj
BINDIR = bin
TARGET = $(BINDIR)/main.exe
SRCS = main.cpp DynamicThreadPool.cpp
OBJS = $(addprefix $(OBJDIR)/, $(SRCS:.cpp=.o))

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(BINDIR)
