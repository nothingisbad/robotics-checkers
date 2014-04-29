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
  Board b;

  Evolve<NNAI::NetType, 5> learner;

  size_t canidate_index = learner.sample();

  NNAI canidate_nn( learner.at(canidate_index) );

  AI simple_ai;

  string input;

  Move move;

  for(int i = 0; i < 1000; ++i) {
    bool is_canidate = false;
    int counter = 0;

    for(; counter < 500; ++counter) {
      is_canidate ^= true;

      if(is_canidate)
	move = canidate_nn(Board::State::red, b);

      else
	move = simple_ai(Board::State::black, b);
	
      if( move.dst == iPair(-1,-1) )
	break; 		/* someone lost */

      b.legal_move(move);
    } 

    cout << "Run " << i << ": ";

    if(is_canidate) {
      cout << "simple ai won";

      learner.bump_down(canidate_index);
      learner.mutate();

      canidate_index = learner.sample();

      canidate_nn = learner.at(canidate_index);
    }

    else {
      cout << "Canidate #" << canidate_index << " won";

      /* bonus for winning quickly */
      if(counter < 45)
	learner.bump_up(canidate_index);

      learner.bump_up(canidate_index);
    }

    cout << " in " << counter << " turns" << endl;

    b = Board();

  }

  canidate_nn.branch_net.print_theta(cout) << endl;

  return 0;
}

