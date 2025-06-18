CXX = g++
CXXFLAGS = -std=c++17 -I src
LDFLAGS = -lX11 -lGL -lpthread -lpng -lstdc++fs

SRCS = src/main.cpp src/bus.cpp src/cpu.cpp
OBJS = $(SRCS:.cpp=.o)

TARGET = 6502_sim

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
