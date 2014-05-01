
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
  Move move(iPair(4,1), iPair(4, 2));
  arm.execute_move(move);

  arm.move_to(iPair(0,0));

  /* move = Move(iPair(5, 0), iPair(4,0)); */
  /* arm.execute_move(move); */

  /* arm.move_to( iPair(3,0) ); */

  /* arm.move_to(iPair(0,0) ); */
}

void test_move(Arm &arm) {
  arm.move_to( iPair(4,2) );

  arm.move_to( iPair(3,0) );
  
  arm.move_to( iPair(2,2) );

  arm.move_to( iPair(0,0) );
}

void test_sensors() {
  cout << "Shoulder pos: " << digital(sensor_shoulder_positive) << endl;
  cout << "Shoulder neg: " << digital(sensor_shoulder_negative) << endl;

  cout << "Arm pos: " << digital(sensor_arm_positive) << endl;
  cout << "Arm neg: " << digital(sensor_arm_negative) << endl;
}

void test_motion(Arm &arm) {
  cout << "Centered" << endl;
  SensorGroup &shoulder = arm._column._sense;
  arm._column.set_velocity(Cart::_max_velocity);
  int i = 0;
  while(i < 3) {
    shoulder.step( 1 );
    if(i != shoulder.position()) {
      i = shoulder.position();
      cout << "New position " << i << endl;
    }
  }
  sleep(0.1);
  arm._column.start_centering();
  while( !arm._column.step() );

  cout << "Finished positive movement" << endl;

  arm._column.set_velocity(-Cart::_max_velocity);
  while(i > 0) {
    shoulder.step( -1 );
    if(i != shoulder.position()) {
      i = shoulder.position();
      cout << "New position " << i << endl;
    }
  }
  arm._column.start_centering();
  while( !arm._column.step() );
}

int main() {
  cout << "Begin" << endl;
  //test_sensors();

  Arm arm;
  arm.reset();

  test_board_move(arm);
  //test_move(arm);

  arm.reset();
  cout << "Done." << endl;
  return 0;  
}
