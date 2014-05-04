#ifndef GRID_HPP
#define GRID_HPP
/**
 * Ryan Domigan <ryan_domigan@sutdents@uml.edu>
 * Kaitlyn Carcia <kate.carcia@gmail.com>
 * @file /home/ryan/uml/robotics/checkers/grid.hpp
 * Created on Apr 04, 2014
 *
 * An 8x8 grid.  Hopefully suitable for computer vision.
 */

#include <iomanip>
#include "points.hpp"
#include "Board.hpp"

/**
 * Keeps an 8x8 grid with a short integer representing the state of each square, as captured by vision.
 * Unlike the Board class, Grid keeps an element for every square on the board, occuable or not.
 *
 * Positive for Red (aka the red and yellow piece set), and negative for Black.
 */
class Grid {
public:
  const static int _n = 8
    , _m = 8;
private:
  short _grid[_n][_m];
public:
  /* Returns short representing state at a certain location on grid */
  short& at(int i, int j) { return _grid[i][j]; }
  const short at(int i, int j) const { return _grid[i][j]; }

  short& at(const iPair& p) { return _grid[p.x][p.y]; }
  const short at(const iPair& p) const { return _grid[p.x][p.y]; }

  /* Clears board */
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
	at(i,j) = (abs(at(i,j)) >= threshold) ? at(i,j) : 0;
  }

  /* Rotates the grid 90 degrees to account for the camera angle */
  Grid rotate(){
    Grid g;
    for( int i=0; i<_n; ++i) {
      for (int j=0; j<_m; ++j) {
	g.at(j, _n - i - 1) = at(i,j);
      }
    }
    return g;
  }
  
  Grid() : _grid{} {}

  /* Grid constructor - takes a board, which only includes all playable squares */
  Grid(const Board& b) {
    clear();
    for(int i = 0; i < b._rows; ++i){
      for(int j = 0; j < b._columns; ++j) {
	State board_value = b.at(i,j);
	if( is(Empty, board_value))
	  at( diag2abs(iPair(i,j)) ) = 0;
	else
	  at( diag2abs(iPair(i,j)) ) = is(Red, b.at(i,j)) ? 1 : -1;
      }}}

  /* print prints the grid includes the horizontal/vertical lines
   * Putting (0,0) at the bottom left */
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

/* Override << operator to print board */
std::ostream& operator<<(std::ostream &out, const Grid &g) { return g.print(out); }

Board::Board(const Grid& g) {
  char grid_value;
  for(int j = 0; j < _columns; ++j) {
    for(int i = 0; i < _rows; ++i) {
      grid_value = g.at( diag2abs(iPair(i,j)) );
      if(grid_value == 0)
	at(i,j) = State::empty;
      else
	at(i,j) = grid_value > 0 ? State::red : State::black;
    }}}

#endif
