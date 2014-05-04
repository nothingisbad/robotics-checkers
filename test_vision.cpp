/**
 * Ryan Domigan <ryan_domigan@sutdents@uml.edu>
 * Kaitlyn Carcia <kate.carcia@gmail.com>
 * @file /home/ryan/uml/robotics/checkers/test_checkers.c
 * Created on April 22, 2014
 * 
 * Testing caputuring grid, and rotating it
 */

#include "./vision.hpp"

int main() {
  Grid g = capture_grid();
  cout << "Begin" << endl;

  //cout << "origin " << origin << " edge " << edge << endl;
  g.squelch(2);
  cout << g.rotate() << flush;

  return 0;
}
