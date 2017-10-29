CC = gcc
CXX = g++
CPPFLAGS = -std=c++11 -O3 --no-rtti -fPIC -I`llvm-config-5.0 --includedir`
LDFLAGS = -shared

all: analysis.so

analysis.so: analysis.o
	$(CC) $(LDFLAGS) $^ -o $@

analysis.o: analysis.cpp
	$(CXX) $(CPPFLAGS) -c $^ -o $@

.PHONY: clean
clean:
	rm analysis.so analysis.o
