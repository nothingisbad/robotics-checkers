#ifndef CHECKERS_H
#define CHECKERS_H
/**
 * @file /home/ryan/uml/robotics/checkers/checkers.h
 * @author Ryan Domigan <ryan_domigan@sutdents@uml.edu>
 * Created on Mar 05, 2014
 *
 * A basic checkers game board, with no logic to speak of.
 */

#include <iostream>

#include <ostream>
#include <tuple>

#include "./points.hpp"

struct Move {
  iPair src, dst;

  Move() = default;
  Move(const iPair& s, const iPair& d) : src(s), dst(d) {}
};


class Board {
public:
  static const int _rows = 8
    , _columns = 4;

  enum State {  empty = 0, black = 1, red = 2, king = 4 };
  
  struct EmptyType { static bool is(const State &s) { return s == empty; } };
  struct BlackType { static bool is(const State &s) { return s & black; } };
  struct RedType { static bool is(const State &s) { return s & red; } };
  struct KingType { static bool is(const State &s) { return s & king; }};

  const static EmptyType Empty;
  const static BlackType Black;
  const static RedType   Red;
  const static KingType  King;

private:
  State _board[_rows][_columns];

  /**
   * Should just be a function, but template rules are different for classes vs functions.
   * Because C++.
   * 
   * @tparam R:
   * @tparam *fn:
   */
  template<class R>
  struct MoveFold {
    typedef R (*FnType)(R, int, int, int, int);
    /* 'a' for apply */
    static R a(const FnType fn, R acc, const Board& b, State self) {
      State other = opponent_color(self);

      auto next_column = [](int row, int column) -> int {
	std::cout<< "  row is even?" << row % 2 << std::endl;
	return column + (row % 2 == 0 ? 1 : -1);
      };
    
      int next_row
	, dst_i = -1
	, dst_j = -1;

      /* red goes low to high, black high to low */
      if(self == red)
	next_row = 1;
      else
	next_row = -1;
    
      for(int i = 0; i < Board::_rows; ++i) {
	for(int j = 0; j < Board::_columns; ++j) {
	  /* todo: king movement */
	  if( b.square_state(i,j) == self ) {

	    /* check if I can move this piece */
	    dst_i = i + next_row;
	    dst_j = j;

	    if( b.is(Empty, dst_i, dst_j) )
	      acc = fn(acc, i,j,dst_i,dst_j);

	    else if( b.has_same(other, dst_i, dst_j)
		     &&
		     b.is(Empty, dst_i + next_row, dst_j) ) {
	      std::cout << "Jump" << std::endl;
	      dst_i = dst_i + next_row;
	      acc = fn(acc,i,j, dst_i
		       , dst_j );
	    }
	      
	    dst_j = next_column(dst_i
				, dst_j); 

	    if( b.is(Empty, dst_i, dst_j) )
	      acc = fn(acc, i,j,dst_i,dst_j);

	    else if( b.has_same(other, dst_i,dst_j)
		     && b.is(Empty
			     , dst_i + next_row
			     , next_column(dst_i + next_row
					   , dst_j)
			     )) {
	      std::cout << "Jump" << std::endl;
	      dst_i += next_row;
	      dst_j = next_column(dst_i, dst_j);

	      acc = fn(acc, i,j, dst_i,dst_j);
	    }
	      
	  }
	}
      }
      return acc;
    }
  };

  static auto nth_move_helper(std::tuple<int,Move> a, int i, int j
			      , int dst_i, int dst_j) -> decltype(a)  {
    using namespace std;
    if( get<0>(a) == 0 )
      return make_tuple(-1, Move(iPair(i,j), iPair(dst_i,dst_j)));
    if(get<0>(a) > 0)
      --get<0>(a);
    return a;
  };

public:
  /****************************************************/
  /*  __  __                                     _    */
  /* |  \/  | _____   _____ _ __ ___   ___ _ __ | |_  */
  /* | |\/| |/ _ \ \ / / _ \ '_ ` _ \ / _ \ '_ \| __| */
  /* | |  | | (_) \ V /  __/ | | | | |  __/ | | | |_  */
  /* |_|  |_|\___/ \_/ \___|_| |_| |_|\___|_| |_|\__| */
  /****************************************************/

  /**
   * Put a pice on the board
   * 
   * @param color: State of the piece (red/black/king/empty)
   * @param row: row to place the piece
   * @param column: column to place the piece
   */
  void place(State color, int row, int column) { _board[row][column] = color; }
  void place(State color, const iPair& p) { _board[p._x][p._y] = color; }

  void move(const iPair& src, const iPair& dst) {
    State piece = square_state(src);
    place(piece, dst);
    place(empty, src);
  }

  void move(const Move& m) { move(m.src, m.dst);  }

  /* picks the nth legal move for a given color.  Ordering is basically arbitrary, but
     each legal move maps to a unique n. */
  Move nth_move(State color, int nth_move) const {
    using namespace std;
    auto acc = make_tuple((int)0, Move(iPair(0,0), iPair(0,0)));

    auto result = MoveFold<decltype(acc)>::a(nth_move_helper, acc, *this, color);

    if(get<0>(result) == -1)
      return get<1>(result);
    
    return Move(iPair(-1,-1), iPair(-1,-1));
  }

  /*********************************/
  /*   ___                         */
  /*  / _ \ _   _  ___ _ __ _   _  */
  /* | | | | | | |/ _ \ '__| | | | */
  /* | |_| | |_| |  __/ |  | |_| | */
  /*  \__\_\\__,_|\___|_|   \__, | */
  /*                        |___/  */
  /*********************************/
  /**
   * Returns the color opposite the input color (red for black and vice versa).
   * 
   * @param color: color of the current peice
   * @return: color of the opposing piece
   */
  static State opponent_color(State color) {
    if( color == State::red )
      return State::black;
    return State::red;
  }

  /**
   * counts the number of moves which can be made for the parameter color from
   * the current board state.
   * 
   * @param color: color I'm solving for
   * @return: the count.
   */
  int liberties(State color) const {
    return
      MoveFold<int>::a( [](int acc, int i, int j, int ii, int jj) { return acc + 1; }
      , 0 , *this, color);
  }

  State square_state(int i, int j) const { return _board[i][j]; }
  State square_state(const iPair& p) const { return _board[p._x][p._y]; }
  
  template<class T>
  bool is(T, int row, int column) const {
    return T::is( square_state(row,column) );
  }

  bool has_same(State state, int row, int column) const {
    return (square_state(row,column) & state) == state;
  }


  /****************************************************************/
  /*   ____                _                   _                  */
  /*  / ___|___  _ __  ___| |_ _ __ _   _  ___| |_ ___  _ __ ___  */
  /* | |   / _ \| '_ \/ __| __| '__| | | |/ __| __/ _ \| '__/ __| */
  /* | |__| (_) | | | \__ \ |_| |  | |_| | (__| || (_) | |  \__ \ */
  /*  \____\___/|_| |_|___/\__|_|   \__,_|\___|\__\___/|_|  |___/ */
  /****************************************************************/
  Board() {
    int i, j;
    for(i = 0; i < 3; ++i) {
      for(j = 0; j < _columns; ++j) {
	place(red, i, j);
      }}

    for(i = 3; i < 5; ++i) {
      for(j = 0; j < _columns; ++j) {
	place(empty, i, j);
      }}

    for(i = 5; i < _rows; ++i) {
      for(j = 0; j < _columns; ++j) {
	place(black, i, j);
      }}
  }

  Board(const Board& b) {
    for(int i = 0; i < _rows; ++i)
      for(int j = 0; j < _columns; ++j)
	_board[i][j] = b._board[i][j];
  }

  /**
   * fills in all squares with same state
   */
  Board(enum State s) {
    for(int i = 0; i < _rows; ++i) {
      for(int j = 0; j < _columns; ++j){
	_board[i][j] = s;
      }}}

  /***********************/
  /*  __  __ _           */
  /* |  \/  (_)___  ___  */
  /* | |\/| | / __|/ __| */
  /* | |  | | \__ \ (__  */
  /* |_|  |_|_|___/\___| */
  /***********************/
                   
  /*************/
  /* Overloads */
  /*************/
  Board& operator=(const Board &input) {

    for(int i = 0; i < _rows; ++i)
      for(int j = 0; j < _columns; ++j)
	_board[i][j] = input._board[i][j];

    return *this;
  }

  /**************/
  /* Comparison */
  /**************/

  /* difference in pieces between two boards; returns a new board which only has pieces whose
     position differ between this and other */
  Board operator-(const Board& other) {
    Board result(empty);
    for(int i = 0; i < _rows; ++i) {
      for(int j = 0; j < _columns; ++j) {
	if( other._board [i][j] == _board[i][j] )
	  result._board[i][j] = State::empty;

	else if(other._board[i][j] == empty)
	  result._board[i][j] = _board[i][j];

	else
	  result._board[i][j] = other._board[i][j];
      }
    }
    return result;
  }

  /*********/
  /* Print */
  /*********/
  std::ostream& print(std::ostream & out) const {
    using namespace std;

    auto square_char = [this](int row, int j) -> char {
      if( is(Empty, row, j) )
    	return '_';
      else
    	return (is(Red, row, j) ? 'r' : 'b') - (is(King, row,j) ? 32 : 0);
    };

    /* top line */
    for(int i = 0; i < _columns; ++i)
      out << "____";
    out << endl;

    for(int i = _rows - 1; i >= 0; --i) {
      out << "|";
      if(0 == (i % 2)) out << "_|";

      for(int j = 0; j < _columns - 1; ++j)
	out << square_char(i,j) << "|_|";
      out << square_char(i, _columns - 1) << "|";

      if(1 == (i % 2)) out << "_|";
      out << "\n";
    }
    return out;
  }
};

std::ostream& operator<<(std::ostream &out, const Board &b) { return b.print(out); }

#endif
