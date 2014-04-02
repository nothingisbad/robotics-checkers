/**
 * @file /home/ryan/uml/robotics/checkers/test_checkers.c
 * @author Ryan Domigan <ryan_domigan@sutdents@uml.edu>
 * Created on Mar 05, 2014
 */

#include <cbc.h>

#include "./checkers.hpp"
#include "./AI.hpp"

#include <iostream>

using namespace std;

int main() {
  Board b, c(Board::empty);
  string input;
  int x, y;
  AI ai;
  Move move;

  c.place(Board::red,0,0);
  c.place(Board::black,3,2);

  cout << "* Board b:\n" << b << endl;

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
      move.src = iPair(x,y);
      cin >> x; cin >> y;
      move.dst = iPair(x,y);

      b.move(move);
    }

    else if("next" == input) {
      cin >> input;
      cin >> x;

      if(input == "red")
	move = b.nth_move(Board::State::red, x);
      else
	move = b.nth_move(Board::State::black, x);

      cout << "From " << move.src << " To " << move.dst << endl;

      b.move( move );
    }

    else if("reset" == input) {
      b = Board();
    }

    else if("quit" == input) {
      return 0;
    }

    else if("play" == input) {
      cin >> input;
      if("red" == input)
	move = ai(Board::State::red, b);
      else
	move = ai(Board::State::black, b);

      b.move( move );
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
