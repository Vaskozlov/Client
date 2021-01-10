CC = clang
CXX = clang++
LD = clang++

CCFLAGS = -std=c11 -c -O0 -g -isystem code
CXXFLAGS = -std=c++17 -c -O0 -g -isystem code
LDFLAGS = -l zip -l crypto -l ssl

OBJECT_FILES = build/main.o build/archive.o build/io.o build/client.o build/encryption.o 

all: $(OBJECT_FILES)
	$(LD) $(LDFLAGS) $^ -o build/client

build/main.o: code/main.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

build/client.o: code/client/client.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

build/io.o: code/support/io.c 
	$(CC) $(CCFLAGS) $< -o $@

build/encryption.o: code/encryption/encryption.c 
	$(CXX) $(CXXFLAGS) $< -o $@

build/archive.o: code/support/archive.c 
	$(CC) $(CCFLAGS) $< -o $@

setup:
	mkdir -p build