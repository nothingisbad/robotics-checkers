#ifndef NNAI_HPP
#define NNAI_HPP
/**
 * @file /home/ryan/uml/robotics/checkers/NNAI.hpp
 * @author Ryan Domigan <ryan_domigan@sutdents@uml.edu>
 * Created on Apr 07, 2014
 *
 * A neural network based AI for playing checkers
 */

#include <tuple>

#include <genetic.hpp>		/* from nnet */
#include <NNet.hpp>

#include "./Board.hpp"

class NNAI {
public:
  typedef NNet< Nums<4, 1> > Net3x3_odd;
  typedef NNet< Nums<5, 1> > Net3x3_even;

  typedef NNet< Nums<32, 10, 8, 8, 1> > BoardNet;
  typedef NNet< Nums<2, 1> > Output; /* combine the peice count part with the BoardNet part */

  BoardNet position_net;
  Output output_net; 

  /* typedef MetaNNet< MetaLayer<Net3x3_odd, Net3x3_even, Net3x3_odd, Net3x3_even, Net3x3_odd, Net3x3_even */
  /* 			      , Net3x3_even, Net3x3_odd, Net3x3_even, Net3x3_odd, Net3x3_even, Net3x3_odd */
  /* 			      , Net3x3_odd, Net3x3_even, Net3x3_odd, Net3x3_even, Net3x3_odd, Net3x3_even */
  /* 			      , Net3x3_even, Net3x3_odd, Net3x3_even, Net3x3_odd, Net3x3_even, Net3x3_odd */
  /* 			      , Net3x3_odd, Net3x3_even, Net3x3_odd, Net3x3_even, Net3x3_odd, Net3x3_even */
  /* 			      , Net3x3_even, Net3x3_odd, Net3x3_even, Net3x3_odd, Net3x3_even, Net3x3_odd */

  /* 			      , Net5x5_odd, Net5x5_even, Net5x5_odd, Net5x5_even */
  /* 			      , Net5x5_even, Net5x5_odd, Net5x5_even, Net5x5_odd */
  /* 			      , Net5x5_odd, Net5x5_even, Net5x5_odd, Net5x5_even */
  /* 			      , Net5x5_even, Net5x5_odd, Net5x5_even, Net5x5_odd */

  /* 			      , Net8x8> */

  /* MetaLayer< Nums<36 + 16 + 1> */

  float apply(Board::State color, const Board& board) {
    using namespace std;
    BoardNet::Feed board_feed;
    Output::Feed output_feed;

    board.color_ordered(color, board_feed.layer, output_feed.layer[0]);
    cout << "Input board: ";
    print_array(board_feed.layer) << endl;
    cout << "\nCount: " << output_feed.layer[0] << endl;
      

    predict(position_net, board_feed);
    output_feed.layer[1] = board_feed.output_layer()[0];
    
    predict(output_net, output_feed);

    return output_feed.output_layer()[0];
  }

  NNAI() : position_net{} , output_net{} {}
  NNAI(const BoardNet& b_init, const Output& o_init) : position_net(b_init), output_net(o_init) {}
};

#endif
