/**
 * @file /home/ryan/uml/robotics/checkers/test_checkers.c
 * @author Ryan Domigan <ryan_domigan@sutdents@uml.edu>
 * Created on Mar 05, 2014
 */

#include "./Board.hpp"
#include "./Grid.hpp"

#include "./AI.hpp"
#include "./NN_AI.hpp"

#include <iostream>

using namespace std;

int main() {
  UndoList b;

  /* Evolve<NNAI::NetType, 5> learner; */

  /* size_t best_index = learner.sample() */
  /*   , canidate_index = learner.sample(); */

  /* while(canidate_index == best_index) */
  /*   canidate_index = learner.sample(); */

  /* NNAI best_nn( learner.at(best_index) ) */
  /*   , canidate_nn( learner.at(canidate_index) ); */

  AI simple_ai;

  string input;
  int x, y;
  Move move;
  bool is_red_turn = true, legal;

  b.push() = Board();
  //b.push() = Board(State::empty);
  //b().at(0,0) = State::red;
  /* b().place(Board::black, 1,0); */
  /* b().place(Board::black, 3,1); */

  while(true) {
    cout << b() << endl;

    cin >> input;
    /*   cin >> input; */
    /*   cin.ignore(); */
    /* } else cin.get(); */

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
      Board tmp = b();
      tmp.unconditional_move(move);

      b().move_fold([&](const Board& bb, const Move&) {
	  legal = color_equal(State::red, tmp, bb);
	  return legal;
	}, static_cast<State>( b().at(move.src) & ~State::king) );

      if(legal)
	cout <<"Move was legal"<<endl;
      else
	cout <<"illegal move" << endl;

      b.push() = tmp;
    }

    else if("pdn-move" == input) {
      cin >> x;
      cin >> y;

      move.src = canonical2diag(x);
      move.dst = canonical2diag(y);

      cout << "Move: " << move << endl;

      b().unconditional_move(move);
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
      cout << "next play: \n";
      cin >> input;
      cin >> x;

      Board tmp = b();
      cout << "From\n" << tmp << endl;

      if(input == "red")
      	tmp = tmp.nth_move(State::red, x);
      else
      	tmp = tmp.nth_move(State::black, x);

      cout << "To\n" << tmp << endl;
      b.push() = tmp;
    }

    else if("reset" == input) {
      b.reset();
    }

    else if("quit" == input) {
      return 0;
    }

    else if("play" == input) {
      cout << "play: \n";
      cin >> input;

      if(input == "red")
    	move = simple_ai(State::red, b());
      else
    	move = simple_ai(State::black, b());
      //cout << move << endl;
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

