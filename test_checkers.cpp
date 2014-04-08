/**
 * @file /home/ryan/uml/robotics/checkers/test_checkers.c
 * @author Ryan Domigan <ryan_domigan@sutdents@uml.edu>
 * Created on Mar 05, 2014
 */

#include "./Board.hpp"
#include "./Grid.hpp"
#include "./AI.hpp"

#include <iostream>

using namespace std;

int main() {
  UndoList b;

  string input;
  int x, y;
  AI ai;
  Move move;
  bool is_red_turn;

  b() = Board(Board::empty);
  b().place(static_cast<Board::State>(Board::red | Board::king)
	    , iPair(0, 0));

  while(true) {
    cout << b() << endl;

    if(cin.peek() != '\n') {
      cin >> input;
      cin.ignore();
    } else cin.get();

    if("at" == input) {
      cin >> x; cin >> y;
      cout << b().at(x,y) << endl;
    }

    /* unconditional movement */
    else if("move" == input) {
      cin >> x; cin >> y;
      move.src = iPair(x,y);
      cin >> x; cin >> y;
      move.dst = iPair(x,y);

      b.push().unconditional_move(move);
    }

    /* human move; exec jump or what have you */
    else if("exec" == input) {
      cin >> x; cin >> y;
      move.src = iPair(x,y);

      cin >> x; cin >> y;
      move.dst = iPair(x,y);

      b.push().human_move(move.src, move.dst);
    }

    else if("next" == input) {
      cin >> input;
      cin >> x;

      if(input == "red")
	move = b().nth_move(Board::State::red, x);
      else
	move = b().nth_move(Board::State::black, x);

      cout << "From " << move.src << " To " << move.dst << endl;

      b.push().legal_move( move );
    }

    else if("reset" == input) {
      b.reset();
    }

    else if("quit" == input) {
      return 0;
    }

    else if("play" == input) {
      if(is_red_turn)
	move = ai(Board::State::red, b());
      else
	move = ai(Board::State::black, b());

      is_red_turn ^= true;

      if( move.dst != iPair(-1,-1) )
	b.push().legal_move( move );
    }


    else if("undo" == input) {
      b.undo();
    }

    /* just see if the grid interpretation looks like it should */
    else if("grid" == input) {
      Grid g(b());
      g.print(std::cout);

      Board c(g);
      c.print(std::cout);
    }

    else if("simple" == input) {
      for(int i = 0; i < Board::_rows; ++i) {
	for(int j = 0; j < Board::_columns; ++j) {
	  cout << b().at(i,j);
	}
	cout << endl;
      }
    }
  }

  return 0;
}
