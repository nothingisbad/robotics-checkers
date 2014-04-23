
/**
 * @file /home/ryan/uml/robotics/checkers/learning/learn.cpp
 * @author Ryan Domigan <ryan_domigan@sutdents@uml.edu>
 * Created on Apr 20, 2014
 */

#include "../NN_AI.hpp"
using namespace std;

int main() {
  Board initial;
  NNAI ai;

  permute(ai.position_net, -2, 2);

  ai.output_net.layer[0][0] = 0.0; 	/* piece count */
  ai.output_net.layer[0][1] = 1.0; 	/* neural net */


  initial.move_fold([&](const Move& m) {
      Board b = initial;
      b.legal_move(m);

      float weight = ai.apply(Board::red, b);
      cout << "\n" << m;
      cout << "\n" << b << "\n Weight: " << weight << "\n";
    }, Board::red);

  return 0;
}
