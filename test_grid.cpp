/**
 * Ryan Domigan <ryan_domigan@sutdents@uml.edu>
 * Kaitlyn Carcia <kate.carcia@gmail.com>
 * @file /home/ryan/uml/robotics/checkers/test_grid.cpp
 * Created on Apr 23, 2014
 *
 * Verifies board/grid placement is accurate.
 * Board contains all playable squares,
 * Grid contains all squares.
 */

#include "./Grid.hpp"
#include "./Board.hpp"

using namespace std;

int main() {
  Board b(State::empty);
  b.at(0,0) = State::red;
  b.at(2,0) = State::red;

  Grid g(b);

  cout << b << endl;  

  ++g.at(6,6);

  cout << g << endl;

  cout << g.rotate() << endl;

  b = Board(g);

  cout << b << endl;

  return 0;
}
