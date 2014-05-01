
/**
 * @file /home/ryan/uml/robotics/checkers/main.cpp
 * @author Ryan Domigan <ryan_domigan@sutdents@uml.edu>
 * Created on Apr 30, 2014
 *
 * Main loop, should tie together the AI, vision and movement
 */

#include "./vision.hpp"
#include "./movment.hpp"
#include "./AI.hpp"

int main() {
  Board logical_board, vision_board;
  Grid grid;
  bool human_moved = false;
  Move move;
  AI ai();

  while( true ) {
    while( true ) { 
      vision_board = Board( capture_grid().rotate() );

      logical_board.move_fold([&](const Board& bb, const Move&) {
	  human_moved = color_equal(State::red, vision_board, bb);
	  return human_moved;
	}, State::red);
      if(!human_moved)
	sleep(0.1);
      else
	break;
    }
    
    move = ai(Sate::black, logical_board);
  }


  return 0;
}
