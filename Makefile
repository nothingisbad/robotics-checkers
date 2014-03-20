INCLUDES = -I../userlib/libcbc/src/
FLAGS = -Wall -std=c++0x

CROSS_CPP = arm-linux-gnueabi-g++ $(FLAGS) $(INCLUDES)

CPP = g++ $(FLAGS) $(INCLUDES)

#CPP = clang -stdlib=libc++ -lc++ $(FLAGS) $(INCLUDES)

CROSS_LIBS_DIR = ../cross-libs

CROSS_LIBS = $(CROSS_LIBS_DIR)/libcbc.a $(CROSS_LIBS_DIR)/libtrack.a $(CROSS_LIBS_DIR)/libshared.a $(CROSS_LIBS_DIR)/libstdc++.so $(CROSS_LIBS_DIR)/libgcc_s.so $(CROSS_LIBS_DIR)/init.o $(CROSS_LIBS_DIR)/libpthread-2.8.so $(CROSS_LIBS_DIR)/libm-2.8.so



test_checkers: checkers.hpp test_checkers.cpp
	$(CPP) -o test_checkers test_checkers.cpp

cbc_executable: *.hpp *.cpp
	$(CROSS_CPP) -o cbc_executable test_checkers.cpp $(CROSS_LIBS)

TARGETS = cbc_executable test_checkers
clean:
	@for item in $(TARGETS); do { if [ -e $${item} ]; then rm $${item} ; echo -n "cleaning up "; echo $${item} ; fi }; done	
