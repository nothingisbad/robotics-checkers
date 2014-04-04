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
#include "./checkers.hpp"

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
	      , Board b
	      , const Move& move) {
    using namespace std;

    struct AccType {
      int sum;
      Board::State opponent_color;
      int depth;
      Board &b;
      AccType(int i_sum, Board::State i_color, int i_depth, Board& i_b)
	: sum(i_sum), opponent_color(i_color), depth(i_depth), b(i_b) {}
    };

    AccType acc(weight_move(color, b, move)
		, b.opponent_color(color)
		, depth - 1 , b);

    b.legal_move(move);

    if(depth <= 1)
      return acc.sum;

    return b.move_fold< AccType >([](AccType& a, Move m) -> void {
	a.sum -= compair( a.depth - 1
			  , a.opponent_color
			  , a.b
			  , m);
      }
      , acc
      , acc.opponent_color
      ).sum;
  }
}

class AI {
  public:
    Move operator()(Board::State color, const Board& b) {
      using namespace std;
      struct AccType {
	Move best_move;
	int best_weight;
	Board::State color;
	Board b;
	AccType(Move i_m, int i_max_weight, Board::State i_color, Board i_b)
	  : best_move(i_m), best_weight(i_max_weight), color(i_color), b(i_b) {}
      };

      AccType acc = AccType(Move(),-1000,color,b);

      return
	b.move_fold< AccType >( [](AccType& a, Move m) -> void {
	    int weight = checkers_AI::compair(3, a.color, a.b, m);

	    // std::cout << "Move: " << m.dst << " weight: " << weight << std::endl;
	    if( weight > a.best_weight ) {
	      a.best_weight = weight;
	      a.best_move = m;
	    }
	  }
	  , acc
	  , color
	  ).best_move;
    }

  };


#endif
