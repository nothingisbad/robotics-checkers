INCLUDE := -I../nnet
CBC_INCLUDE := $(INCLUDE) -I../userlib/libcbc/src/ -I../userlib/tracklib

CROSS_CXX ?= arm-linux-gnueabi-g++ -std=c++11 -Wall

CXX ?= g++
#CXX = clang -std=c++11 -stdlib=libc++ -lc++ 
CXX := $(CXX) -std=c++11 -Wall -ggdb $(INCLUDE)

# let the cross-compiler toolchain should include the stdc++ libs.
# $(CROSS_LIBS_DIR)/libstdc++.so

CROSS_LIBS_DIR = ../cross-libs
CROSS_LIBS = $(CROSS_LIBS_DIR)/libcbc.a $(CROSS_LIBS_DIR)/libtrack.a $(CROSS_LIBS_DIR)/libshared.a  $(CROSS_LIBS_DIR)/libgcc_s.so $(CROSS_LIBS_DIR)/init.o $(CROSS_LIBS_DIR)/libpthread-2.8.so $(CROSS_LIBS_DIR)/libm-2.8.so

test_checkers: AI.hpp NN_AI.hpp Board.hpp test_checkers.cpp
	$(CXX) -o test_checkers test_checkers.cpp

test_move_fold: AI.hpp NN_AI.hpp Board.hpp test_move_fold.cpp
	$(CXX) -o test_move_fold test_move_fold.cpp

test_AI: AI.hpp NN_AI.hpp Board.hpp test_ai.cpp
	$(CXX) -o test_ai test_ai.cpp

test_grid: Grid.hpp test_grid.cpp
	$(CXX) -o test_grid test_grid.cpp

gl_checkers: *.hpp *.cpp
	$(CXX) -o gl_checkers gl_checkers.cpp -lGL -lGLU `sdl2-config --cflags --libs`

cbc_executable: *.hpp *.cpp
	$(CROSS_CXX) $(CBC_INCLUDES) -o cbc_executable test_checkers.cpp $(CROSS_LIBS)

TARGETS = cbc_executable test_checkers gl_checkers test_ai test_vision test_grid test_move_fold
clean:
	@for item in $(TARGETS); do { if [ -e $${item} ]; then rm $${item} ; echo -n "cleaning up "; echo $${item} ; fi }; done	
