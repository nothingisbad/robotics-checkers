
/**
 * @file /home/ryan/uml/robotics/checkers/test_grid.cpp
 * @author Ryan Domigan <ryan_domigan@sutdents@uml.edu>
 * Created on Apr 17, 2014
 */

#include <iostream>
#include "./Grid.hpp"

int main() {
  using namespace std;

  Board b;
  Grid g(b);

  cout << "Grid: \n" << g << endl;

  g.at(0,0) = 0;
  b = Board(g);
  cout << "Board:\n" << b << endl;

  return 0;
}
