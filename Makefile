CC = gcc -Wall -ggdb -std=c99
CPP = g++ -Wall -ggdb -std=c++11
#CPP = clang -stdlib=libc++ -lc++ -std=c++11 -ggdb -Wall

test_checkers: checkers.hpp test_checkers.cpp
	$(CPP) -o test_checkers test_checkers.cpp
