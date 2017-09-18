CXX = /opt/rh/devtoolset-4/root/usr/bin/g++
RM = rm -f
CFLAGS =
CXXFLAGS = -std=c++14 -O0 -fPIC
LDFLAGS = -I./
SRCS = $(wildcard *.cpp)
EXES = $(SRCS:.cpp=)

all: clean $(EXES)

$(EXES): %: %.cpp
	$(CXX) $(CFLAGS) $(CXXFLAGS) $(LDFLAGS) $< -o $@

clean:
	$(RM) $(EXES)

.PHONY: all clean
