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
  bool is_red_turn = true;

  b.push() = Board(Board::empty);
  b().place(Board::red, 7,0);
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

      b.push().unconditional_move(move);
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
      	tmp = tmp.nth_move(Board::State::red, x);
      else
      	tmp = tmp.nth_move(Board::State::black, x);

      cout << "To\n" << tmp << endl;
      b.push() = tmp;

    }

    else if("reset" == input) {
      b.reset();
    }

    else if("quit" == input) {
      return 0;
    }

    /* else if("play" == input) { */
    /*   if(is_red_turn) */
    /* 	move = best_nn(Board::State::red, b()); */
    /*   else */
    /* 	move = best_nn(Board::State::black, b()); */

    /*   is_red_turn ^= true; */

    /*   if( move.dst != iPair(-1,-1) ) */
    /* 	b.push().legal_move( move ); */
    /* } */

    /* else if("evolve" == input) { */
    /*   for(int i = 0; i < 1000; ++i) { */
    /* 	bool is_canidate = false; */
    /* 	int counter = 0; */

    /* 	for(; counter < 500; ++counter) { */
    /* 	  is_canidate ^= true; */

    /* 	  if(is_canidate) */
    /* 	    move = best_nn(Board::State::red, b()); */

    /* 	  else */
    /* 	    move = canidate_nn(Board::State::black, b()); */
	
    /* 	  if( move.dst == iPair(-1,-1) ) */
    /* 	    break; 		/\* someone lost *\/ */

    /* 	  b().legal_move(move); */
    /* 	}  */

    /* 	cout << "Run " << i << ": "; */

    /* 	if(is_canidate) { */
    /* 	  cout << "Best #" << best_index << " won "; */

    /* 	  learner.bump_up(best_index); */
    /* 	  learner.bump_down(canidate_index); */
    /* 	} */

    /* 	else { */
    /* 	  cout << "Canidate #" << canidate_index << " won"; */

    /* 	  learner.bump_up(canidate_index); */
    /* 	  learner.bump_down(best_index); */

    /* 	  best_index = canidate_index; */
    /* 	  best_nn = canidate_nn; */
    /* 	} */

    /* 	cout << " in " << counter << " turns" << endl; */


    /* 	b() = Board(); */

    /* 	/\* bonus for winning quickly *\/ */
    /* 	if(counter < 200) { */
    /* 	  learner.bump_up(best_index); */
    /* 	} */

    /* 	learner.mutate(); */

    /* 	canidate_index = learner.sample(); */
    /* 	while(canidate_index == best_index) */
    /* 	  canidate_index = learner.sample(); */

    /* 	canidate_nn = learner.at(canidate_index); */
    /*   } */

    /*   best_nn.branch_net.print_theta(cout) << endl; */
    /* } */

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

