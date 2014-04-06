#ifndef GRID_HPP
#define GRID_HPP
/**
 * @file /home/ryan/uml/robotics/checkers/grid.hpp
 * @author Ryan Domigan <ryan_domigan@sutdents@uml.edu>
 * Created on Apr 04, 2014
 *
 * An 8x8 grid.  Hopefully suitable for computer vision.
 */

#include "points.hpp"

/* convert a co-ordinant relitive to the black daigonals (playable squares) to
   a co-ordinant on an 8x8 grid */
iPair daig2abs(const iPair &in) {
  return iPair(in.x, in.y * 2 + (even(in.x) ? 0 : 1));
}

/* convert a co-ordinant relitive to an (absolute) 8x8 grid the black daigonals
   (playable squares) on a checker board  */
iPair abs2diag(const iPair &in) {
  return iPair(in.x, (in.y - (even(in.x) ? 0 : 1)) / 2);
}


class Grid {
public:
  const static int _n = 8
    , _m = 8;

  //State defined as: enum State {  empty = 0, black = 1, red = 2, king = 4 };
  typedef Board::State State;
private:
  State _grid[_n][_m];
public:
  /* query the state */
  State at(int i, int j) const { return _grid[i][j]; }

  /* set the state */
  void  place(State value, int i, int j) { _grid[i][j] = value; }

  void clear() {
    for(int i = 0; i < _n; ++i)
      for(int j = 0; j < _m; ++j)
	_grid[i][j] = State::empty;
  }
  
  Grid() { clear(); }

  Grid(const Board& b) {
    clear();
    for(int i = 0; i < b._rows; ++i)
      for(int j = 0; j < b._columns; ++j)
	_grid[i][j * 2 + (even(i) ? 0 : 1)] = b.at(i,j);
  }

  void print(std::ostream &out) const {
    for(int i = 0; i < _m; ++i)
      out << "__";
    out << "\n";
    
    for(int i = _n - 1; i >= 0; --i) {
      out << "|";
      for(int j = 0; j < _m; ++j) {
	if( at(i,j) != State::empty )
	  out << at(i,j) << "|";
	else
	  out << "_|";
      }
      out << "\n";
    }}
};

Board::Board(const Grid& g) {
  for(int i = 0; i < _rows; ++i)
    for(int j = 0; j < _columns; ++j)
      place( g.at(i, 2 * j + (even(i) ? 0 : 1))
	    , i, j);
}

std::ostream& operator<<(std::ostream &out, const Grid &g) { return g.print(out); }

#endif
