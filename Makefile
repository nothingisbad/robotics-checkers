INCLUDE := -I../nnet
CBC_INCLUDE := $(INCLUDE) -I../userlib/libcbc/src/ -I../userlib/tracklib -I/home/ryan/programming/

CROSS_CXX ?= arm-linux-gnueabi-g++ 
CROSS_CXX := $(CROSS_CXX) -std=c++0x -Wall -static-libstdc++ 

CXX ?= g++ 
CXX := $(CXX) -std=c++0x -Wall -ggdb $(INCLUDE) $(FLAGS)
#CXX = clang -std=c++11 -stdlib=libc++ -lc++ 

# let the cross-compiler toolchain should include the stdc++ libs.
#$(CROSS_LIBS_DIR)/libstdc++.so
#$(CROSS_LIBS_DIR)/tracklib.o 

CROSS_LIBS_DIR = ../cross-libs
CROSS_LIBS = $(CROSS_LIBS_DIR)/libgcc_s.so $(CROSS_LIBS_DIR)/init.o $(CROSS_LIBS_DIR)/libpthread-2.8.so $(CROSS_LIBS_DIR)/libm-2.8.so $(CROSS_LIBS_DIR)/cbc.o $(CROSS_LIBS_DIR)/process.o $(CROSS_LIBS_DIR)/compat.o $(CROSS_LIBS_DIR)/shared_mem.o 

test_checkers: AI.hpp NN_AI.hpp Board.hpp test_checkers.cpp
	$(CXX) -o test_checkers test_checkers.cpp

test_move_fold: AI.hpp NN_AI.hpp Board.hpp test_move_fold.cpp
	$(CXX) -o test_move_fold test_move_fold.cpp

test_AI: AI.hpp NN_AI.hpp Board.hpp test_ai.cpp
	$(CXX) -o test_ai test_ai.cpp

test_grid: Grid.hpp test_grid.cpp
	$(CXX) -o test_grid test_grid.cpp

test_vision: vision.c *.hpp
	$(CXX) -o test_vision vision.c

gl_checkers: *.hpp *.cpp
	$(CXX) -o gl_checkers gl_checkers.cpp -lGL -lGLU `sdl2-config --cflags --libs`

# ../userlib/tracklib/tracklib.o
cbc_executable: *.hpp *.cpp
	$(CROSS_CXX) $(CBC_INCLUDE) -o cbc_executable vision.cpp $(CROSS_LIBS) 


TARGETS = cbc_executable test_checkers gl_checkers test_ai test_vision test_grid test_move_fold
clean:
	@for item in $(TARGETS); do { if [ -e $${item} ]; then rm $${item} ; echo -n "cleaning up "; echo $${item} ; fi }; done	
