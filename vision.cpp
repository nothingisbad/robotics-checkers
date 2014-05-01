// Kaitlyn Carcia
// Last Updated April 22, 2014

#include "./vision.hpp"

int main() {
  Grid g = capture_grid();
  cout << "Begin" << endl;


  //cout << "origin " << origin << " edge " << edge << endl;
  g.squelch(2);
  cout << g.rotate() << flush;

  return 0;
}
