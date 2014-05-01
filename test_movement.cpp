
/**
 * @file /home/ryan/uml/robotics/checkers/test_movement.cpp
 * @author Ryan Domigan <ryan_domigan@sutdents@uml.edu>
 * Created on Apr 04, 2014
 *
 * Some simple movements for the arm to take (testing the counters mostly)
 */

#include "movement.hpp"

using namespace std;

void test_board_move(Arm &arm) {
  Move move(iPair(6,3), iPair(5, 3));
  arm.execute_move(move);

  arm.move_to( iPair(0,0) );
  arm.reset();
}

void test_move(Arm &arm) {
  arm.move_to( iPair(1,3) );
  cout << "at 2,2" << endl;
  arm.move_to( iPair(0,0) );
}

int main() {
  Arm arm;
  cout << "Begin" << endl;

  arm.reset();

  test_board_move(arm);
  //test_move(arm);

  /* cout << "Centered" << endl; */
  /* SensorGroup &shoulder = arm._column._sense; */
  /* arm._column.set_velocity(Cart::_max_velocity); */
  /* int i = 0; */
  /* while(i < 3) { */
  /*   shoulder.step( 1 ); */
  /*   if(i != shoulder.position()) { */
  /*     i = shoulder.position(); */
  /*     cout << "New position " << i << endl; */
  /*   } */
  /* } */
  /* sleep(0.1); */
  /* Cart::Centering center(&arm._column); */
  /* while( !arm._column.step() ); */

  /* cout << "Finished positive movement" << endl; */

  /* arm._column.set_velocity(-Cart::_max_velocity); */
  /* while(i > 0) { */
  /*   shoulder.step( -1 ); */
  /*   if(i != shoulder.position()) { */
  /*     i = shoulder.position(); */
  /*     cout << "New position " << i << endl; */
  /*   } */
  /* } */
  /* center = Cart::Centering(&arm._column); */
  /* while( !arm._column.step() ); */

  arm.reset();
  cout << "Done." << endl;
  return 0;  
}
