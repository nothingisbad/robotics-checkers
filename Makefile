INCLUDES = -I../userlib/libcbc/src/

CPP = g++ -Wall -ggdb -std=c++11 
CROSS_CPP = arm-unknown-linux-gnueabi-g++ -Wall -std=c++11 $(INCLUDES)

CROSS_LIBS_DIR = ../cross-libs

CROSS_LIBS = $(CROSS_LIBS_DIR)/libcbc.a $(CROSS_LIBS_DIR)/libtrack.a $(CROSS_LIBS_DIR)/libshared.a $(CROSS_LIBS_DIR)/libstdc++.so $(CROSS_LIBS_DIR)/libgcc_s.so $(CROSS_LIBS_DIR)/init.o $(CROSS_LIBS_DIR)/libpthread-2.8.so $(CROSS_LIBS_DIR)/libm-2.8.so

#CPP = clang -stdlib=libc++ -lc++ -std=c++11 -ggdb -Wall

test_checkers: checkers.hpp test_checkers.cpp
	$(CPP) -o test_checkers test_checkers.cpp

cbc_executable: *.hpp *.cpp
	$(CROSS_CPP) -o cbc_executable test_checkers.cpp $(CROSS_LIBS)
