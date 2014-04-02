#ifndef AI_HPP
#define AI_HPP
/**
 * @file /home/ryan/uml/robotics/checkers/AI.hpp
 * @author Ryan Domigan <ryan_domigan@sutdents@uml.edu>
 * Created on Mar 31, 2014
 *
 * A checkers playing AI.
 */

#include "./checkers.hpp"

/* A light-weight object which can evaluate the relitive strength of a move */
class WeightMove {
public:
  int operator()(Board::State color, const Board& b, const Move& m) const {
    int initial_count = 0
      , subsequent_count = 0;

    /* just look for peices lost */
    for(int i = 0; i < Board::_rows; ++i) {
      for(int j = 0; j < Board::_columns; ++j) {
	if( b.square_state(i,j) == color )
	  ++initial_count;
	if( b.square_state(i,j) == color )
	  ++subsequent_count;
      }
    }

    return subsequent_count - initial_count;
  }
};

class WeighBranch {
public:
  WeightMove compair_move;
  
  int compair(int depth
	      , Board::State color
	      , const Board& b
	      , const Move& move) {

    int sum = compair_move(color, b, move);
    Board::State other = b.opponent_color(color);

    Board tmp;

    if(depth == 0)
      return 0;

    for( int i = 0,
	   end = b.liberties(color);
	 i < end; ++i ) {
      tmp = b;
      sum -= compair(depth - 1
		     , other
		     , tmp
		     , tmp.nth_move(other, i)
		     );

    }
    return sum;
  }
};

class AI {
public:
  Move operator()(Board::State color, const Board& b) {
    WeighBranch weigh;

    int i_max = -1000
      , sum;
    Move m_max
      , m;

    for( int i = 0,
	   end = b.liberties(color);
	 i < end; ++i ) {

      m = b.nth_move(color, i);

      sum = weigh.compair(3, color, b
			  , m);

      if(sum > i_max) {
	i_max = sum;
	m_max = m;
      }
    }
    return m_max;
  } };


#endif
