// Kaitlyn Carcia
// Last Updated April 22, 2014

#include <vector>
#include <track.h>
#include <iostream>
#include <algorithm>
#include <tuple>

#include <compat.h>

#include "./Grid.hpp"
#include "./points.hpp"
#include "./Track.hpp"

#include <nnet/utility.hpp>

#include <cbc.h>


int main() {
  Grid g = capture_grid();
  cout << "Begin" << endl;


  //cout << "origin " << origin << " edge " << edge << endl;
  cout << "transform\n";
  print_array(transform[0]) << endl;
  print_array(transform[1]) << endl;
  g.squelch(2);
  cout << g << flush;

  return 0;
}
