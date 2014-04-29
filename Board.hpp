#ifndef BOARD_HPP
#define BOARD_HPP
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
#include <cstdint>
#include <array>
#include <functional>

#include "./points.hpp"


/*********************************/
/*  _  _     _                   */
/* | || |___| |_ __  ___ _ _ ___ */
/* | __ / -_) | '_ \/ -_) '_(_-< */
/* |_||_\___|_| .__/\___|_| /__/ */
/*            |_|                */
/*********************************/
template<class Num>
bool even(Num i) {
  static_assert( std::is_integral<Num>::value, "even is only defined for integral numbers." );
  return i % 2 == 0;
}

bool color_equal(Board::State color, Board b1, Board b2) {
  for(int i=0; i < Board::_rows; i++) {
    for(int j=0; j < Board::_columns; j++) {
      iPair a(i,j);
      if ( (has_same(color, b1.at(a)) && !has_same(color, b2.at(a)))
	   || has_same(color, b2.at(a)) )
	return false;
    }
  }
  return true;
}

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

std::ostream& operator<<(std::ostream &out, const Move &m) {
  return out << m.src << "->" << m.dst;
}

class Grid;

enum State {  empty = 0, black = 1, red = 2, king = 4 };
  
struct EmptyType { static bool is(const State &s) { return s == empty; } };
struct BlackType { static bool is(const State &s) { return s & black; } };
struct RedType { static bool is(const State &s) { return s & red; } };
struct KingType { static bool is(const State &s) { return s & king; }};
struct PawnType { static bool is(const State &s) { return !(s & king); }};

const static EmptyType Empty;
const static BlackType Black;
const static RedType   Red;
const static KingType  King;
const static PawnType  Pawn;

template<class T>
static bool is(T, State color) { return T::is(color);  }

template<class T, class U>
static bool is(T, U, State color) { return T::is(color) && U::is(color);  }

/**
 * Returns true if testing has the attribute state,
 * ie if state is red, has_same will return true if testing
 *    is a red king or a red pawn.
 *    
 * @param state: state to test for
 * @param testing: state being tested
 * @return: bool if testing has the 'state' bit set
 */
bool has_same(State state, State testing) { return (testing & state) == state; }


/*********************************/
/*  ____                      _  */
/* | __ )  ___   __ _ _ __ __| | */
/* |  _ \ / _ \ / _` | '__/ _` | */
/* | |_) | (_) | (_| | | | (_| | */
/* |____/ \___/ \__,_|_|  \__,_| */
/*********************************/
class Board {
public:
  static const int _rows = 8
    , _columns = 4;

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
    if(m.src.x < m.capture.x)
      m.dst.x = m.capture.x + 1;
    else
      m.dst.x = m.capture.x - 1;

    if( m.src.y == m.capture.y )
      /* if the source row is even, jump right */
      m.dst.y = m.capture.y + ((m.src.x % 2) == 0 ? 1 : -1);
    else
      m.dst.y = m.capture.y;

    return is(Empty, at(m.dst));
  }
public:
  /****************************************************/
  /*  __  __                                     _    */
  /* |  \/  | _____   _____ _ __ ___   ___ _ __ | |_  */
  /* | |\/| |/ _ \ \ / / _ \ '_ ` _ \ / _ \ '_ \| __| */
  /* | |  | | (_) \ V /  __/ | | | | |  __/ | | | |_  */
  /* |_|  |_|\___/ \_/ \___|_| |_| |_|\___|_| |_|\__| */
  /****************************************************/
  void unconditional_move(const iPair& src, const iPair& dst) {
    at(dst) = at(src);
    at(src) = State::empty;
  }

  /* unconditional move */
  void unconditional_move(const Move& m) { unconditional_move(m.src, m.dst);  }

  /* perform a move and carries out the capture/makes king */
  void legal_move(const Move& m) {
    /* Jump */
    if( m.is_capture() )
      at(m.capture) = State::empty;

    /* Gets piece to determine color  */
    State piece = at(m.src);

    /* yellow checker */
    if(piece == State::black) {
      if (m.dst.row() == 0) { /* black checker is at opposite end of board */
	at(m.src) = static_cast<State>(piece | State::king); /* marks checker as king */
      }

      /* red checker */
    } else if (piece == State::red) {
      if (m.dst.row() == 7) { /* red checker is at opposite end of board */
	at(m.src) = static_cast<State>(piece | State::king); /* marks checker as king */
      }
    }
    
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

    else legal_move( Move(src, dst) );
  }

  /* picks the nth legal move for a given color.  Ordering is basically arbitrary, but
     each legal move maps to a unique n. */
  Board nth_move(State color, int nth_move) const {
    using namespace std;
    Board store_move;
    cout << "Move " << nth_move << endl;

    move_fold( [&](const Board& b) -> void  {
	using namespace std;
	if(nth_move < 0)
	  return;

	if( nth_move == 0 ) {
	  cout << "Stored: " << endl;
	  b.print();
	  store_move = b;
	}

	--nth_move;
      }, color);

    return store_move;
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
    if( is(Red, color) )
      return State::black;
    return State::red;
  }

  bool opposites(iPair a, iPair b) {
    return !is(Empty, at(a) ) && !is(Empty, at(b) )
      && ((at(a) & red) != (at(b) & red));
  }

  void color_ordered(State color, std::array<float,32> &result, float piece_count) const {
    float value;

    auto piece_weight = [&](int i, int j) -> float {
      if(is(Empty, at(i,j)))  return 0;

      if(is(King, at(i,j)))
	return is(Red, at(i,j)) ? 1.3 : -1.3;
      
      return is(Red, at(i,j)) ? 1 : -1;
    };
    
    if(color == State::red) {
      for(int i = 0; i < _rows; ++i)
	for(int j = 0; j < _columns; ++j) {
	  value = piece_weight(i,j);
	  std::cout << value << " ";

	  result[i * _columns + j] = value;
	  piece_count += value;
	}}

    else {
      for(int i = _rows - 1; i >= 0; --i) {
	for(int j = _columns - 1; j >= 0; --j) {
	  value = -piece_weight(i,j);

	  result[i * _columns + j] = value;
	  piece_count += value;
	}}}
  }

  State at(int i, int j) const { return _board[i][j]; }
  State at(const iPair& p) const { return _board[p.x][p.y]; }

  State& at(int i, int j) { return _board[i][j]; }
  State& at(const iPair& p) { return _board[p.x][p.y]; }
  
  bool in_bounds(const iPair &p) const {
    return p < iPair(_rows, _columns) && p >= iPair(0,0);
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
    for(i = 0; i < 3; ++i)
      for(j = 0; j < _columns; ++j)
	at(i,j) = State::red;

    for(i = 3; i < 5; ++i)
      for(j = 0; j < _columns; ++j)
	at(i, j) = State::empty;

    for(i = 5; i < _rows; ++i)
      for(j = 0; j < _columns; ++j)
	at(i,j) = State::black;

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
   * Folds fn across all availible moves for given color.  It's
   * assumed fn will be bound to whatever data it is trying to accumulate.
   * 
   * fn is of type void (*fn)(const Move&)
   * 
   * @param fn: function being folded
   * @param color: type who's moves are being evaluated
   * @return: the final state of the accumulator
   */
  template<class FnType>
  void move_fold(const FnType fn, State color) const {
    using namespace std;
    State other = opponent_color(color);
    Move move;
    Board post_move = *this;

    int row_offset
      , column_offset
      , dst_row;

    iPair src;
	
    /* red goes low to high, black high to low */
    if(color == red)
      row_offset = 1;
    else
      row_offset = -1;

    bool did_jump = false;

    std::function<bool (const Board *tmp, const iPair&)> jump_check;

    /* multiple jumps are treated recursively, so I need to pass in which board
       I'm acting on (the call stack will deal with allocation) */
    auto jump = [&](const Board* base, const iPair& src, const iPair& dst) -> bool {
      if( in_bounds(dst) && has_same(other, base->at(dst) ) ) {
	Move move = Move( src, iPair(), dst );

	if( base->jump_capture(move)) {
	  Board tmp = *base;
	  tmp.legal_move(move);
	  fn( tmp );
	  jump_check(&tmp, move.dst);

	  return true;
	}}
      return false;
    };

    jump_check = [&](const Board *tmp, const iPair& src) -> bool {
      bool did_jump = false;
      if( is(King, tmp->at(src)) ) {
	did_jump |= jump(tmp, src, src + iPair(-row_offset, 0));
	did_jump |= jump(tmp, src, src + iPair(-row_offset, column_offset));
      }
 
      did_jump |= jump(tmp, src, src + iPair(row_offset, 0));
      did_jump |= jump(tmp, src, src + iPair(row_offset, column_offset));
      return did_jump;
    };

    auto do_move = [&](iPair src, iPair dst) -> bool {
      if( in_bounds(dst) && is(Empty, at(dst) ) ) {
	post_move.unconditional_move( Move( src, dst ) );
	fn( post_move );
	post_move = *this;
	return true;
      }
      return false;
    };

    /* By the rules; if I can jump I _must_, so loop over the jumps*/
    for(int i = 0; i < Board::_rows; ++i) {
      column_offset = even(i) ? -1 : 1;
      dst_row = i + row_offset;

      for(int j = 0; j < Board::_columns; ++j) {
	if( has_same(color, at(i,j) ) )
	  did_jump |= jump_check(this, iPair(i,j));
      }}

    /* if I did jump, none of the other moves are legal to take so return now. */
    if(did_jump) return;

    /* If I didn't jump, check the free squares */
    for(int i = 0; i < Board::_rows; ++i) {
      column_offset = even(i) ? -1 : 1;
      dst_row = i + row_offset;

      for(int j = 0; j < Board::_columns; ++j) {
	if( has_same(color, at(i,j)) ) {
	  src = iPair(i,j);
	  if( is(King, at(src) ) ) {
	    do_move(src, iPair(i - row_offset, j));
	    do_move(src, iPair(i - row_offset, j + column_offset));
	  }

	  do_move(src, iPair(dst_row, j));
	  do_move(src, iPair(dst_row, j + column_offset));
	}}}
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
      if( is(Empty, at(row, j)) )
	return '_';
      else
	return (is(Red, at(row, j)) ? 'r' : 'b') - (is(King, at(row,j)) ? 32 : 0);
    };

    /* top line */
    for(int i = 0; i < _columns; ++i)
      out << "____";
    out << endl;

    for(int i = _rows - 1; i >= 0; --i) {
      out << "|";
      if( !even(i) ) out << "_|";

      for(int j = 0; j < _columns - 1; ++j)
	out << square_char(i,j) << "|_|";
      out << square_char(i, _columns - 1) << "|";

      if( even(i) ) out << "_|";
      out << "\n";
    }
    return out;
  }

  void print() const; /* prevent from inlining so I can invoke in debugger */
};
void Board::print() const { print(std::cout); } 

std::ostream& operator<<(std::ostream &out, const Board &b) { return b.print(out); }

/**************************************/
/*  _   _      _                      */
/* | | | | ___| |_ __   ___ _ __ ___  */
/* | |_| |/ _ \ | '_ \ / _ \ '__/ __| */
/* |  _  |  __/ | |_) |  __/ |  \__ \ */
/* |_| |_|\___|_| .__/ \___|_|  |___/ */
/*              |_|                   */
/**************************************/
/* convert a co-ordinant relitive to the black daigonals (playable squares) to
   a co-ordinant on an 8x8 grid */
iPair diag2abs(const iPair &in) {
  return iPair(in.x, in.y * 2 + (even(in.x) ? 0 : 1));
}

/* convert a co-ordinant relitive to an (absolute) 8x8 grid the black daigonals
   (playable squares) on a checker board  */
iPair abs2diag(const iPair &in) {
  return iPair(in.x, (in.y - (even(in.x) ? 1 : 0)) / 2);
}

/* takes a pdn style move and puts it into the 4x8 Board space*/
iPair canonical2diag(int i) {
  --i; 			/* counts 1 as the top left  */
  return iPair( ((31 - i) / Board::_columns), i % Board::_columns);
}


/***********************************************/
/*  _   _           _         _     _     _    */
/* | | | |_ __   __| | ___   | |   (_)___| |_  */
/* | | | | '_ \ / _` |/ _ \  | |   | / __| __| */
/* | |_| | | | | (_| | (_) | | |___| \__ \ |_  */
/*  \___/|_| |_|\__,_|\___/  |_____|_|___/\__| */
/***********************************************/
class UndoList {
public:
  std::vector<Board> _list;

  Board& operator()() {
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
