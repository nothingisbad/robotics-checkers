/**
 * @file /home/ryan/uml/robotics/checkers/test_checkers.c
 * @author Ryan Domigan <ryan_domigan@sutdents@uml.edu>
 * Created on Mar 05, 2014
 */

#include <cbc.h>

#include "./checkers.hpp"
#include <iostream>

using namespace std;

int main() {
  Board b, c(Board::empty);
  string input;
  int x, y;
  iPair src, dst;

  c.place(Board::red,0,0);
  c.place(Board::black,3,2);

  cout<<"* Board c:\n" << c << endl;

  cout<<"* Difference:\n"<< b - c << endl;

  while(true) {
    cout << b << endl;
    cin >> input;

    if("at" == input) {
      cin >> x; cin >> y;
      cout << b.square_state(x,y) << endl;
    }

    else if("move" == input) {
      cin >> x; cin >> y;
      src = iPair(x,y);
      cin >> x; cin >> y;
      dst = iPair(x,y);

      b.move(src,dst);
    }

    else if("quit" == input) {
      return 0;
    }

    else if("simple" == input) {
      for(int i = 0; i < Board::_rows; ++i) {
	for(int j = 0; j < Board::_columns; ++j) {
	  cout << b.square_state(i,j);
	}
	cout << endl;
      }
    }
  }

  return 0;
}
