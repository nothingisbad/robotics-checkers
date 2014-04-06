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
#include <vector>
#include <ostream>
#include <tuple>
#include <type_traits>

#include "./points.hpp"

/**
 * Three pairs currently represent a move
 */
struct Move {
  /* src: the beginning position of the piece
     dst: the destination of the piece
     capture: if a jump occured, capture is the location of the
        captured peice.  Otherwise capture should be iPair(-1,-1)
   */
  iPair src, dst, capture;

  bool is_capture() const { return capture.x != -1; }

  Move() = default;
  Move(const iPair& s, const iPair& d) : src(s), dst(d), capture(-1,-1) {}
  Move(const iPair& s, const iPair& d, const iPair& c) : src(s), dst(d), capture(c) {}
};

class Grid;

template<class Num>
bool even(Num i) {
  static_assert( std::is_integral<Num>::value
		 , "even is only defined for integral numbers." );
  return i % 2 == 0;
}

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
   * The input move should have the moving piece in m.src and
   * the peice to be jumped in m.capture.  m.dst will be set up
   * by this function.
   * 
   * @param other:
   * @param m: the move src and caputre
   * @return: false if jump could not be carried out (ie due to obstructing peice).
   */
  bool jump_capture(Move &m) const {
    iPair direction;

    if(m.src.x < m.capture.x)
      m.dst.x = m.capture.x + 1;
    else
      m.dst.x = m.capture.x - 1;

    if( m.src.y == m.capture.y )
      /* if the source row is even, jump right */
      m.dst.y = m.capture.y + ((m.src.x % 2) == 0 ? 1 : -1);
    else
      m.dst.y = m.capture.y;

    return is(Empty, m.dst);
  }

  /**
   * Should just be a function, but template rules are different for classes vs functions.
   * Named by analogy to fold in a functional language, MoveFold takes a function and a reference
   * to an accumulator 
   * 
   * @tparam R: Accumulator type; the type should not be a reference - the reference type is added implicitly.
   */
  template<class R>
  struct MoveFold {
    typedef void (*FnType)(R&, Move);

    /**
     * Applies a function to each availible move on the current board for a given color.
     * 
     * @param fn: function to apply
     * @param acc: an accumulator -- passed by reference; results are accumulated by modifying
     *             this variable
     * @param b: current board
     * @param self: the color which will be moved
     * @return: the accumulator (still a reference to the same value).
     */
    static R& a(const FnType fn, R& acc, const Board& b, State self) {
      using namespace std;
      State other = opponent_color(self);
      Move move;

      int next_row
	, offset_col
	, dst_i = -1
	, dst_j = -1 ;
	
      /* red goes low to high, black high to low */
      if(self == red)
	next_row = 1;
      else
	next_row = -1;

      bool did_jump = false;
      
      auto compute_column_offset = [](int row) {
	return (row % 2) == 1 ? 1 : -1;
      };
	

      /* By the rules; if I can jump I _must_, so loop over the jumps*/
      for(int i = 0; i < Board::_rows; ++i) {
	offset_col = compute_column_offset(i);
	dst_i = i + next_row;

	for(int j = 0; j < Board::_columns; ++j) {
	  /* todo: king movement */
	  if( b.at(i,j) == self ) {

	    if( b.has_same(other, dst_i, j) ) {
	      move = Move( iPair(i,j), iPair(), iPair(dst_i,j) );
	      if( b.jump_capture(move)) {
		did_jump = true;
		fn(acc, move);
	      }}

	    dst_j = j + offset_col;
	    /* jump, other column */
	    if( b.has_same(other, dst_i, dst_j) ) {
	      move = Move(iPair(i,j), iPair(), iPair(dst_i, dst_j));
	      if( b.jump_capture(move)) {
		did_jump = true;
		fn(acc, move);
	      }}
	  }}}

      /* if I did jump, none of the other moves are legal to take, so return now. */
      if(did_jump) return acc;

      /* If I didn't jump, check the free squares */
      for(int i = 0; i < Board::_rows; ++i) {
	offset_col = compute_column_offset(i);
	dst_i = i + next_row;

	for(int j = 0; j < Board::_columns; ++j) {
	  if( b.at(i,j) == self ) {
	    if( b.is(Empty, dst_i,  j) )
	      fn(acc, Move( iPair(i,j)
			    , iPair(dst_i, j))
		 );

	    dst_j = j + offset_col;
	    if( b.is(Empty, dst_i, dst_j) )
	      fn(acc, Move( iPair(i,j)
			    , iPair(dst_i,dst_j)
			    ));
	  }}}
      return acc;
    }
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
  void place(State color, const iPair& p) { _board[p.x][p.y] = color; }

  void unconditional_move(const iPair& src, const iPair& dst) {
    State piece = at(src);
    place(piece, dst);
    place(empty, src);
  }

  /* unconditional move */
  void unconditional_move(const Move& m) { unconditional_move(m.src, m.dst);  }

  /* perform a move and carries out the capture/makes king */
  void legal_move(const Move& m) {
    /* Jump */
    if( m.is_capture() )
      place(empty, m.capture);

    unconditional_move(m);
  }

  /* check a move for legality and move the piece, performing jump etc as applicable */
  void human_move(iPair src, iPair dst) {
    using namespace std;

    /* handle jump: */
    if( opposites(src,dst) ) {
      Move m(src,iPair(),dst);
 
      std::cout << "Canidate jump " << src << " -> " << dst << std::endl;
      if( jump_capture(m) )
	legal_move(m);
      else
	cout << "Illegal move" << endl;
    }

    else unconditional_move(src, dst);
  }

  /* picks the nth legal move for a given color.  Ordering is basically arbitrary, but
     each legal move maps to a unique n. */
  Move nth_move(State color, int nth_move) const {
    using namespace std;
    auto acc = make_tuple((int)0, Move(iPair(0,0), iPair(0,0)));

    auto nth_move_helper = [](std::tuple<int,Move>& a, Move m) -> void  {
       using namespace std;
       if(get<0>(a) < 0)
	 return;

       if( get<0>(a) == 0 )
	 get<1>(a) = m;

       --get<0>(a);
    };

    auto result = MoveFold< decltype(acc) >::a(nth_move_helper, acc, *this, color);

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

  bool opposites(iPair a, iPair b) {
    
    return !is(Empty, a) && !is(Empty, b)
      && ((at(a) & red) != (at(b) & red));
  }

  /**
   * counts the number of moves which can be made for the parameter color from
   * the current board state.
   * 
   * @param color: color I'm solving for
   * @return: the count.
   */
  int liberties(State color) const {
    int acc = 0;
    return
      MoveFold<int>::a( [](int& acc, Move _) -> void { ++acc; }
      , acc, *this, color);
  }

  State at(int i, int j) const { return _board[i][j]; }
  State at(const iPair& p) const { return _board[p.x][p.y]; }
  
  template<class T>
  bool is(T, int row, int column) const {
    return (row >= 0) && (row < _rows)
      && (column >= 0) && (column < _columns)
      && T::is( at(row,column) );
  }

  template<class T>
  bool is(T _, iPair p) const {
    return is(_,p.x,p.y);
  }

  bool has_same(State state, int row, int column) const {
    return (at(row,column) & state) == state;
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

  /* implemented in grid.hpp, after the Grid class. */
  Board(const Grid& g);

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
  /**
   * Folds fn across all availible moves for given color with accumulator.
   * The accumulator is passed along by reference, so be careful.
   * 
   * fn is of type void (*fn)(acc&, const Move&)
   * 
   * @tparam R: accumulator type
   * @param fn: function being folded
   * @param acc: accumulator
   * @param color: type who's moves are being evaluated
   * @return: the final state of the accumulator
   */
  template<class R>
  R move_fold(const typename MoveFold<R>::FnType fn, R acc, State color) const {
    return MoveFold<R>::a(fn,acc,*this,color);
  }
                   
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
  /**
   * Prints the checkers board.  I think it's more intuitive to have (0,0) at the bottom left of the
   * screen, so that's what print dose. Opinions, I'm sure, will vary.
   * 
   * @param out: the output stream
   * @return: the output stream
   */
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
      if(1 == (i % 2)) out << "_|";

      for(int j = 0; j < _columns - 1; ++j)
	out << square_char(i,j) << "|_|";
      out << square_char(i, _columns - 1) << "|";

      if(0 == (i % 2)) out << "_|";
      out << "\n";
    }
    return out;
  }

  void print() const;
};
void Board::print() const { print(std::cout); }

std::ostream& operator<<(std::ostream &out, const Board &b) { return b.print(out); }


/***********************************************/
/*  _   _           _         _     _     _    */
/* | | | |_ __   __| | ___   | |   (_)___| |_  */
/* | | | | '_ \ / _` |/ _ \  | |   | / __| __| */
/* | |_| | | | | (_| | (_) | | |___| \__ \ |_  */
/*  \___/|_| |_|\__,_|\___/  |_____|_|___/\__| */
/***********************************************/
class UndoList {
  std::vector<Board> _list;
public:
  const Board& operator()() {
    return _list.back();
  }

  Board& push() {
    _list.push_back(_list.back());
    return _list.back();
  }

  void undo() {
    _list.pop_back();
  }

  void reset() {
    _list.clear();
    _list.push_back(Board());
  }

  UndoList() { _list.push_back( Board() ); }
};

#endif
