#ifndef GRID_HPP
#define GRID_HPP
/**
 * @file /home/ryan/uml/robotics/checkers/grid.hpp
 * @author Ryan Domigan <ryan_domigan@sutdents@uml.edu>
 * Created on Apr 04, 2014
 *
 * An 8x8 grid.  Hopefully suitable for computer vision.
 */

#include <iomanip>
#include "points.hpp"
#include "Board.hpp"

/* convert a co-ordinant relitive to the black daigonals (playable squares) to
   a co-ordinant on an 8x8 grid */
iPair diag2abs(const iPair &in) {
  return iPair(in.x, in.y * 2 + (even(in.x) ? 0 : 1));
}

/* convert a co-ordinant relitive to an (absolute) 8x8 grid the black daigonals
   (playable squares) on a checker board  */
iPair abs2diag(const iPair &in) {
  return iPair(in.x, (in.y - (even(in.x) ? 0 : 1)) / 2);
}

/**
 * Keeps an 8x8 grid with a short integer representing the state of each square, as captured by vision.
 * Unlike the Board class, Grid keeps an element for every square on the board, occuable or not.
 *
 * I'm positive for Red, negative for Black.
 */
class Grid {
public:
  const static int _n = 8
    , _m = 8;
private:
  typedef Board::State State;
  short _grid[_n][_m];
public:
  short& at(int i, int j) { return _grid[i][j]; }
  const short at(int i, int j) const { return _grid[i][j]; }

  short& at(const iPair& p) { return _grid[p.x][p.y]; }
  const short at(const iPair& p) const { return _grid[p.x][p.y]; }


  void clear() {
    for(int i = 0; i < _n; ++i)
      for(int j = 0; j < _m; ++j)
	_grid[i][j] = 0;
  }

  /**
   * Zeros all squares with absolute value less than threshold
   * @param threshold: 
   */
  void squelch(char threshold) {
    for(int i = 0; i < _n; ++i)
      for(int j = 0; j < _m; ++j)
	at(i,j) = (abs(at(i,j)) > threshold) ? at(i,j) : 0;
  }
  
  Grid() : _grid{} {}

  Grid(const Board& b) {
    clear();
    for(int i = 0; i < b._rows; ++i){
      for(int j = 0; j < b._columns; ++j) {
	Board::State board_value = b.at(i,j);
	if(Board::is(Board::Empty, board_value))
	  at( diag2abs(iPair(i,j)) ) = 0;
	else
	  at( diag2abs(iPair(i,j)) ) = Board::is(Board::Red, b.at(i,j)) ? 1 : -1;
      }}}
 
  std::ostream& print(std::ostream &out) const {
    for(int i = 0; i < _m; ++i)
      out << "___";
    out << "\n";
    
    for(int i = _n - 1; i >= 0; --i) {
      out << "|";
      for(int j = 0; j < _m; ++j) {
	if( at(i,j) != 0 )
	  out << std::setw(2) << at(i,j) << "|";
	else
	  out << "__|";
      }
      out << "\n";
    }
    return out;
  }
  
};

std::ostream& operator<<(std::ostream &out, const Grid &g) { return g.print(out); }

Board::Board(const Grid& g) {
  char grid_value;
  for(int i = 0; i < _rows; ++i)
    for(int j = 0; j < _columns; ++j) {
      grid_value = g.at( diag2abs(iPair(i,j)) );
      if(grid_value == 0)
	at(i,j) = Board::empty;
      else
	at(i,j) = grid_value > 0 ? Board::red : Board::black;
    }
}

#endif
