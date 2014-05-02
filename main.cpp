
/**
 * @file /home/ryan/uml/robotics/checkers/main.cpp
 * @author Ryan Domigan <ryan_domigan@sutdents@uml.edu>
 * Created on Apr 30, 2014
 *
 * Main loop, should tie together the AI, vision and movement
 */

#include "./Board.hpp"
#include "./vision.hpp"
#include "movement.hpp"
#include "./AI.hpp"

int main() {
  Board logical_board, vision_board;
  Grid grid;
  bool human_moved = false;
  Move move;
  AI ai;
  { Sling arm; }

  //cout << "logical_board:\n" << logical_board;

  vision_board = Board( capture_grid().rotate() );
  cout << "vision_board\n" << vision_board << "\n";

  cout << "Begin" << endl;

  while( true ) {
    while( true ) { 
      vision_board = Board( capture_grid().rotate() );

      logical_board.move_fold([&](const Board& bb, const Move&) {
	  human_moved = color_equal(State::red, vision_board, bb);
	  if(human_moved) logical_board = bb;
	  return human_moved;
	}, State::red);
      if(!human_moved) {
	cout << ",";
	sleep(0.1);
      } else break;
    }
    //cout << "Human moved\n" << logical_board << endl;;

    move = ai(State::black, logical_board);

    if( !is(Empty, logical_board.at(move.dst)) ) {
      do {
	{
	  Sling arm;
	  arm().remove_piece(move.dst);
	}

	{
	  Sling arm;
	  arm().execute_move( Move(move.src, logical_board.jump_dst(move)));
	}

	logical_board.legal_move(move);
	move = ai(State::black, logical_board);
      } while ( !is(Empty, logical_board.at(move.dst)) );
    } else {
      logical_board.legal_move(move);
      {
	Sling arm;

	arm().reset();
	arm().execute_move(move);
      }
    }
  }


  return 0;
}
