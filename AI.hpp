#ifndef AI_HPP
#define AI_HPP
/**
 * @file /home/ryan/uml/robotics/checkers/AI.hpp
 * @author Ryan Domigan <ryan_domigan@sutdents@uml.edu>
 * Created on Mar 31, 2014
 *
 * A checkers playing AI.
 */
#include <tuple>
#include "./Board.hpp"

namespace checkers_AI {
  /**
   * Computes the weight of a move.  For a proper MinMax, each layer should be returning the weight value only of the best move from that
   * position, but summing them together was a little simpler.  It's been doing fine against amateurs, but it will take a move which mostly
   * positive branches, even if there's a chance of catastrophe on that branch.  
   */
  float compair(int depth, int jump_bonus, State color
	      , Board b) {
    using namespace std;
    // cout << ".";
    
    float weight = b.piece_count(color)
      , sum = weight;
    
    State opponent_color = b.opponent_color(color);

    if(depth <= 1) return 0;

    b.move_fold([&sum,jump_bonus,weight,color,depth,opponent_color](Board local, Move m) -> bool {
	int depth_bump = 0;
	if( jump_bonus > 0 && (local.piece_count(color) - weight) > 1 )
	  depth_bump = 2;
	  
	sum -= compair( depth - 1
			, jump_bonus - depth_bump
			, opponent_color
			, local);
	return false;
      }, opponent_color);

      return sum;
  }
}

/* The AI wrapper.  I thought it would have more internal state, it could have been a simple function. */
struct AI {
  /* Given a color and a board, return the best move for that color. */
  Move operator()(State color, const Board& b) {
    using namespace std;
    int best_weight = -100;
    Move best_move;

    b.move_fold( [&best_move,&best_weight,color](Board bb, const Move& m) -> bool {
	int weight = checkers_AI::compair(3, 2, color, bb);

	if( weight > best_weight ) {
	  best_weight = weight;
	  best_move = m;
	}
	return false;
      } , color );

    return best_move;
  }

};

#endif
