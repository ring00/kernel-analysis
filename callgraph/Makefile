CC = gcc
CXX = g++
CPPFLAGS = -std=c++11 --no-rtti -fPIC -I`llvm-config-5.0 --includedir`
LDFLAGS = -shared

all: callgraph.so

callgraph.so: callgraph.o
	$(CC) $(LDFLAGS) $^ -o $@

callgraph.o: callgraph.cpp
	$(CXX) $(CPPFLAGS) -c $^ -o $@

.PHONY: clean
clean:
	rm callgraph.so callgraph.o
