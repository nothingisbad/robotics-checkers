#ifndef CHECKERS_H
#define CHECKERS_H
/**
 * @file /home/ryan/uml/robotics/checkers/checkers.h
 * @author Ryan Domigan <ryan_domigan@sutdents@uml.edu>
 * Created on Mar 05, 2014
 *
 * A basic checkers game board, with no logic to speak of.
 */

#include <ostream>

class Board {
public:
  enum State {  empty = 0, black = 1, red = 2, king = 4 };
  static const int _rows = 8
    , _columns = 4;

private:
  State _board[_rows][_columns];

public:
  /**
   * Put a pice on the board
   * 
   * @param color: State of the piece (red/black/king/empty)
   * @param row: row to place the piece
   * @param column: column to place the piece
   */
  void place(State color, int row, int column) { _board[row][column] = color; }

  /**
   * Query the state of a specific row/column
   */
  State square_state(int row, int column) const { return _board[row][column]; }

  /**************/
  /* Predicates */
  /**************/
  int is_red(int row, int column) const { return (square_state(row, column) & State::red) == State::red; }

  int is_king(int row, int column) const { return (square_state(row, column) & king) == king; }

  int is_empty(int row, int column) const { return (square_state(row,column) == empty); }

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

  /* comparse the current board to another, returns true if other is reachable from
     this */
  bool is_legal_subsiquent(const Board& other) const { return false; }


  /****************/
  /* Constructors */
  /****************/
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

  /********/
  /* Misc */
  /********/
  std::ostream& print(std::ostream & out) const {
    using namespace std;

    auto square_char = [this](int row, int j) -> char {
      if( is_empty(row, j) )
	return '_';
      else
	return (is_red(row, j) ? 'r' : 'b') - (is_king(row,j) ? 32 : 0);
    };

    /* top line */
    for(int i = 0; i < _columns; ++i)
      out << "____";
    out << endl;

    for(int i = 0; i < _rows; ++i) {
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
