INCLUDES = -I../userlib/libcbc/src/ -I../userlib/tracklib

CROSS_CXX ?= arm-linux-gnueabi-g++
CROSS_CXX := $(CROSS_CXX) $(FLAGS) $(INCLUDES)

CXX = g++ -Wall -std=c++11 -ggdb
#CXX = clang -std=c++11 -stdlib=libc++ -lc++ $(INCLUDES)
CXX := $(CXX) $(INCLUDES)


CROSS_LIBS_DIR = ../cross-libs
CROSS_LIBS = $(CROSS_LIBS_DIR)/libcbc.a $(CROSS_LIBS_DIR)/libtrack.a $(CROSS_LIBS_DIR)/libshared.a $(CROSS_LIBS_DIR)/libstdc++.so $(CROSS_LIBS_DIR)/libgcc_s.so $(CROSS_LIBS_DIR)/init.o $(CROSS_LIBS_DIR)/libpthread-2.8.so $(CROSS_LIBS_DIR)/libm-2.8.so



test_checkers: checkers.hpp test_checkers.cpp
	$(CXX) -o test_checkers test_checkers.cpp

gl_checkers: *.hpp *.cpp
	$(CXX) -std=c++11 -o gl_checkers GUI.cpp -lGL -lGLU `sdl-config --cflags --libs`

cbc_executable: *.hpp *.cpp
	$(CROSS_CXX) -o cbc_executable test_checkers.cpp $(CROSS_LIBS)


TARGETS = cbc_executable test_checkers gl_checkers
clean:
	@for item in $(TARGETS); do { if [ -e $${item} ]; then rm $${item} ; echo -n "cleaning up "; echo $${item} ; fi }; done	
