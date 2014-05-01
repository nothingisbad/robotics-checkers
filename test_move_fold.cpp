
/**
 * @file /home/ryan/uml/robotics/checkers/test_move_fold.cpp
 * @author Ryan Domigan <ryan_domigan@sutdents@uml.edu>
 * Created on Apr 22, 2014
 */

/**
 * @file /home/ryan/uml/robotics/checkers/test_checkers.c
 * @author Ryan Domigan <ryan_domigan@sutdents@uml.edu>
 * Created on Mar 05, 2014
 */

#include "./Board.hpp"
#include "./Grid.hpp"

#include "./AI.hpp"
/* #include "./NN_AI.hpp" */

#include <iostream>

using namespace std;

int main() {
  string input;
  Move move;
  Board b = Board(State::empty);

  b.at(0,0) = State::red;
  b.at(1,0) = State::black;
  b.at(3,1) = State::black;

  cout << "Begin:\n" << b << endl;
  
  cout << "Moves:\n" << b << endl;
  b.move_fold([&](const Board& b, const Move& m) {
      cout << m << "\n" << b << endl;
      return false;
    }, State::red);

  return 0;
}

