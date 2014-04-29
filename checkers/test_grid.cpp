/**
 * @file /home/ryan/uml/robotics/checkers/test_grid.cpp
 * @author Ryan Domigan <ryan_domigan@sutdents@uml.edu>
 * Created on Apr 23, 2014
 *
 * Seems to be an off by one error in the grid; let's try some things out.
 */

#include "./Grid.hpp"
#include "./Board.hpp"

using namespace std;

int main() {
  Board b(Board::empty);
  b.place(Board::red, 0,0);
  b.place(Board::red, 2,0);

  Grid g(b);

  cout << b << endl;  

  ++g.at(6,6);

  cout << g << endl;

  b = Board(g);

  cout << b << endl;

  return 0;
}