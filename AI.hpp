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
  int weight_move(State color, const Board& b, const Move& m) {
    return b.piece_count(color);
  }
    
  /**
   * Computes the move weight of move.
   * 
   * @param depth: must be >= 1 to start
   * @return: 
   */
  int compair(int depth, State color
	      , Board b) {
    using namespace std;
    // cout << ".";
    int sum = 0;
    State opponent_color = b.opponent_color(color);

    if(depth <= 1) return 0;

    b.move_fold([&](Board b, const Move&) -> bool {
	sum -= compair( depth - 1
			  , opponent_color
			, b);
	return false;
      }, opponent_color);

      return sum;
  }
}

class AI {
  public:
    Move operator()(State color, const Board& b) {
      using namespace std;
      int best_weight = -100;
      Move best_move;

      b.move_fold( [&](Board bb, const Move& m) -> bool {
	  int weight = checkers_AI::compair(3, color, bb);

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
