/**
 * Ryan Domigan <ryan_domigan@sutdents@uml.edu>
 * Kaitlyn Carcia <kate.carcia@gmail.com>
 * @file /home/ryan/uml/robotics/checkers/test_checkers.c
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

  /* Set specific coordinates on board */
  b.at(0,0) = State::red;
  b.at(1,0) = State::black;
  b.at(3,1) = State::black;

  cout << "Begin:\n" << b << endl;
  
  /* Make move on board if legal */
  cout << "Moves:\n" << b << endl;
  b.move_fold([&](const Board& b, const Move& m) {
      cout << m << "\n" << b << endl;
      return false;
    }, State::red);

  return 0;
}

