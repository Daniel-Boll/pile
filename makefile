# this is the build file for project 
# it is autogenerated by the xmake build system.
# do not edit by hand.

MM=/usr/bin/gcc
MXX=/usr/bin/gcc
FC=/usr/bin/gfortran
GC=/usr/bin/go
CXX=/usr/bin/gcc
CC=/usr/bin/gcc
RC=/home/danielboll/.cargo/bin/rustc
AS=/usr/bin/gcc
CU=/usr/bin/clang

GCAR=/usr/bin/go
AR=/usr/bin/ar
RCAR=/home/danielboll/.cargo/bin/rustc
SH=/usr/bin/g++
RCSH=/home/danielboll/.cargo/bin/rustc
FCSH=/usr/bin/gfortran
GCLD=/usr/bin/go
LD=/usr/bin/g++
RCLD=/home/danielboll/.cargo/bin/rustc
FCLD=/usr/bin/gfortran

pile_CXXFLAGS=-m64 -fvisibility=hidden -fvisibility-inlines-hidden -O3 -std=c++20 -Iinclude -isystem /home/danielboll/.xmake/packages/c/cxxopts/v3.0.0/1eea793b9f4b4ec0827a4c67495d00a5/include -isystem /home/danielboll/.xmake/packages/s/spdlog/v1.10.0/1902f227aa5444989a19c0e0928690f2/include -DNDEBUG
pile_ARFLAGS=-cr
peak_CXXFLAGS=-m64 -fvisibility=hidden -fvisibility-inlines-hidden -O3 -std=c++20 -Iinclude -isystem /home/danielboll/.xmake/packages/c/cxxopts/v3.0.0/1eea793b9f4b4ec0827a4c67495d00a5/include -isystem /home/danielboll/.xmake/packages/s/spdlog/v1.10.0/1902f227aa5444989a19c0e0928690f2/include -DNDEBUG
peak_LDFLAGS=-m64 -Lbuild/linux/x86_64/release -s -lpile -lfmt

default:  pile peak

all:  pile peak

.PHONY: default all  pile peak

pile: build/linux/x86_64/release/libpile.a
build/linux/x86_64/release/libpile.a: build/.objs/pile/linux/x86_64/release/source/pile/compiler.cpp.o build/.objs/pile/linux/x86_64/release/source/pile/parser.cpp.o build/.objs/pile/linux/x86_64/release/source/pile/repl.cpp.o build/.objs/pile/linux/x86_64/release/source/pile/utils/utils.cpp.o
	@echo linking.release libpile.a
	@mkdir -p build/linux/x86_64/release
	@$(AR) $(pile_ARFLAGS) build/linux/x86_64/release/libpile.a build/.objs/pile/linux/x86_64/release/source/pile/compiler.cpp.o build/.objs/pile/linux/x86_64/release/source/pile/parser.cpp.o build/.objs/pile/linux/x86_64/release/source/pile/repl.cpp.o build/.objs/pile/linux/x86_64/release/source/pile/utils/utils.cpp.o > build/.build.log 2>&1

build/.objs/pile/linux/x86_64/release/source/pile/compiler.cpp.o: source/pile/compiler.cpp
	@echo compiling.release source/pile/compiler.cpp
	@mkdir -p build/.objs/pile/linux/x86_64/release/source/pile
	@$(CXX) -c $(pile_CXXFLAGS) -o build/.objs/pile/linux/x86_64/release/source/pile/compiler.cpp.o source/pile/compiler.cpp > build/.build.log 2>&1

build/.objs/pile/linux/x86_64/release/source/pile/parser.cpp.o: source/pile/parser.cpp
	@echo compiling.release source/pile/parser.cpp
	@mkdir -p build/.objs/pile/linux/x86_64/release/source/pile
	@$(CXX) -c $(pile_CXXFLAGS) -o build/.objs/pile/linux/x86_64/release/source/pile/parser.cpp.o source/pile/parser.cpp > build/.build.log 2>&1

build/.objs/pile/linux/x86_64/release/source/pile/repl.cpp.o: source/pile/repl.cpp
	@echo compiling.release source/pile/repl.cpp
	@mkdir -p build/.objs/pile/linux/x86_64/release/source/pile
	@$(CXX) -c $(pile_CXXFLAGS) -o build/.objs/pile/linux/x86_64/release/source/pile/repl.cpp.o source/pile/repl.cpp > build/.build.log 2>&1

build/.objs/pile/linux/x86_64/release/source/pile/utils/utils.cpp.o: source/pile/utils/utils.cpp
	@echo compiling.release source/pile/utils/utils.cpp
	@mkdir -p build/.objs/pile/linux/x86_64/release/source/pile/utils
	@$(CXX) -c $(pile_CXXFLAGS) -o build/.objs/pile/linux/x86_64/release/source/pile/utils/utils.cpp.o source/pile/utils/utils.cpp > build/.build.log 2>&1

peak: build/linux/x86_64/release/peak
build/linux/x86_64/release/peak: build/linux/x86_64/release/libpile.a build/.objs/peak/linux/x86_64/release/standalone/main.cpp.o
	@echo linking.release peak
	@mkdir -p build/linux/x86_64/release
	@$(LD) -o build/linux/x86_64/release/peak build/.objs/peak/linux/x86_64/release/standalone/main.cpp.o $(peak_LDFLAGS) > build/.build.log 2>&1

build/.objs/peak/linux/x86_64/release/standalone/main.cpp.o: standalone/main.cpp
	@echo compiling.release standalone/main.cpp
	@mkdir -p build/.objs/peak/linux/x86_64/release/standalone
	@$(CXX) -c $(peak_CXXFLAGS) -o build/.objs/peak/linux/x86_64/release/standalone/main.cpp.o standalone/main.cpp > build/.build.log 2>&1

clean:  clean_pile clean_peak

clean_pile: 
	@rm -rf build/linux/x86_64/release/libpile.a
	@rm -rf build/linux/x86_64/release/pile.sym
	@rm -rf build/.objs/pile/linux/x86_64/release/source/pile/compiler.cpp.o
	@rm -rf build/.objs/pile/linux/x86_64/release/source/pile/parser.cpp.o
	@rm -rf build/.objs/pile/linux/x86_64/release/source/pile/repl.cpp.o
	@rm -rf build/.objs/pile/linux/x86_64/release/source/pile/utils/utils.cpp.o

clean_peak:  clean_pile
	@rm -rf build/linux/x86_64/release/peak
	@rm -rf build/linux/x86_64/release/peak.sym
	@rm -rf build/.objs/peak/linux/x86_64/release/standalone/main.cpp.o

