INCLUDE := -I../../nnet -I/home/ryan/programming/grammar 

# I get less heartburn on my system with -std=c++11, but g++-4.6.x wants the -std=c++0x flag
FLAGS := -std=c++11 -Wall -ggdb

CXX ?= g++
#CXX = clang -std=c++11 -stdlib=libc++ -lc++ 
CXX := $(CXX) $(INCLUDE) $(FLAGS)

learn: learn.cpp ../*.hpp
	$(CXX) -o learn learn.cpp

test_parser: *.cpp *.hpp
	$(CXX) -o test_parser test_parser.cpp -lboost_regex

TARGETS = learn test_parser
clean:
	@for item in $(TARGETS); do { if [ -e $${item} ]; then rm $${item} ; echo -n "cleaning up "; echo $${item} ; fi }; done	
