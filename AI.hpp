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
  int weight_move(Board::State color, const Board& b, const Move& m) {
    if( m.is_capture() )
      return 1;

    return 0;
  }
    
  /**
   * Computes the move weight of move.
   * 
   * @param depth: must be >= 1 to start
   * @return: 
   */
  int compair(int depth, Board::State color
	      , Board b) {
    using namespace std;

    int sum = 0;
    Board::State opponent_color = b.opponent_color(color);

    if(depth <= 1) return 0;

    b.move_fold([&](Board b) -> void {
	sum -= compair( depth - 1
			  , opponent_color
			, b);
      }, opponent_color);

      return sum;
  }
}

class AI {
  public:
    Board operator()(Board::State color, const Board& b) {
      using namespace std;
      int best_weight = -100;
      Board best_move;

      b.move_fold( [&](Board bb) -> void {
	  int weight = checkers_AI::compair(3, color, bb);

	  if( weight > best_weight ) {
	    best_weight = weight;
	    best_move = bb;
	  }
	} , color );

      return best_move;
    }

  };

#endif
