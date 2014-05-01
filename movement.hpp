#ifndef MOVEMENT_HPP
#define MOVEMENT_HPP
/**
 * @file /home/ryan/uml/robotics/checkers/movement.hpp
 * @author Ryan Domigan <ryan_domigan@sutdents@uml.edu>
 * Created on Apr 04, 2014
 *
 * Move and read position of the gantry arm
 */

#include <cbc.h>
#include <cstdlib>

#include "./points.hpp"
#include "./Board.hpp"

const static int sensor_shoulder_positive = 13
  , sensor_shoulder_negative = 15

  , sensor_arm_positive = 8 
  , sensor_arm_negative = 10

  , motor_arm = 3
  , motor_shoulder = 0

  , servo_elevation = 2
  , servo_grip = 0;

/**
 * returns -1 for input < 0, 1 for input > 0
 */
int sign(int input) { return input > 0 ? 1 : -1; }

struct SensorGroup {
  int _sensor[2]
    , _position;
  bool _gap_open;

  bool is_open(int n) { return digital( _sensor[n] );  };

  bool toggle() {
    if( is_open( 1 ) == _gap_open )
      return false;
    else {
      _gap_open = !_gap_open;
      return true;
    }}

  SensorGroup(int pos, int neg) {
    _sensor[0] = pos;
    _sensor[1] = neg;
    _position = 0;

    _gap_open = true;
  }
  
  int position() const { return _position; }
  
  void step(int direction) {
    if( toggle() && _gap_open ) {
      //std::cout << " Sensor " << _sensor[1] << " pos: " << _position << "\n";
      _position += sign(direction);
    }
  }

  /* assuming we've counted to the desired position, keep moving towards the centered-point. */
  int is_centered() {
    for(int i = 0; i < 2; ++i) {
      switch( is_open( 0 ) | (is_open( 1 ) << 1) ) {
      case 3:
	_gap_open = true;
	return 0;

      case 1: return -1;
      case 2: return 1;
      }
      sleep(0.1);
    }
    std::cout << "Can't center at " << _position << "; two sensors block at once (sensors "
	      << _sensor[0] << ", "<< _sensor[1] << ")" << std::endl;
    exit(1);
  }
};

/*************************/
/*   ____           _    */
/*  / ___|__ _ _ __| |_  */
/* | |   / _` | '__| __| */
/* | |__| (_| | |  | |_  */
/*  \____\__,_|_|   \__| */
/*************************/
struct Cart {
  static const int 
    _max_velocity = 600
    , _slow_velocity = 200
    , _slowest_velocity = 80;

  int _motor, _destination, _current_velocity;

  SensorGroup _sense;
  enum State { seeking, centering , done };
  State _state;

  Cart(int motor, int s_pos, int s_neg)
    : _motor(motor), _current_velocity(_max_velocity), _sense(s_pos, s_neg), _state(done) {}

  void set_velocity(int vel) {
    _current_velocity = vel;
    mav(_motor, vel);
  }

  bool start_centering() {
    if(_sense.is_centered() == 0) {
      set_velocity(0);
      return true;
    }

    set_velocity(direction() * Cart::_slowest_velocity);
    _state = centering;
    return false;
  }

  bool start_seeking(int dst) {
    _destination = dst;
    if( dst == _sense.position() ) {
      return start_centering();
    }

    mav(_motor, -150);
    sleep(0.1);
    mav(_motor, 0);

    _state = seeking;
    if( dst > _sense.position() )
      set_velocity(Cart::_max_velocity);
    else
      set_velocity(-Cart::_max_velocity);
    return false;
  }

  int abs_velocity() { return abs(_current_velocity); }
  int direction() { return _current_velocity > 0 ? 1 : -1; }

  /* A step in the movement loop, returns the state of this cart. */
  bool step() {
    switch(_state) {
    case seeking:
      _sense.step( direction() );
      /* well, there's a gap in my encoder between column 0 and 1.
	 I didn't think it would be a pain in the neck, but it is. */
      if( _sense.position() == _destination ) {
	if( (_motor != motor_shoulder)
	    || !_sense.is_open(1)
	    || _destination != 1
	    || direction() < 0) {
	  _sense._position = _destination;
	  return start_centering();
	} else set_velocity(direction() * _slow_velocity);
      }
      return false;

    case centering: {
      int sense_state = _sense.is_centered();
      if( sense_state == 0 ) {
	set_velocity(0);
	_state = done;
	return true;
      }

      else {
	if( sense_state > 0 )
	  set_velocity( Cart::_slow_velocity );

	else
	  set_velocity( -Cart::_slow_velocity );
      }
      return false;
    }
    case done:
      return true;
    }
    return true;
  }
};

/****************************/
/*     _                    */
/*    / \   _ __ _ __ ___   */
/*   / _ \ | '__| '_ ` _ \  */
/*  / ___ \| |  | | | | | | */
/* /_/   \_\_|  |_| |_| |_| */
/****************************/
/**
 * The arm has two carts, one for the X and one for the Y axis.  The arm is given a position
 * and then runs the two carts concurrently (a single thread steps both carts) to reach that
 * position.
 */                        
class Arm {
public:
  Cart _row, _column;

private:
  static void nap() { sleep(0.2); }
  
  void open_hand() { set_servo_position(servo_grip, 1680);  }
  void close_hand() { set_servo_position(servo_grip, 723);  }

  static void raise_hand() { set_servo_position(servo_elevation, 1987);  }
  void lower_hand() { set_servo_position(servo_elevation, 1031); }

  void position_hand(int i) {
    set_servo_position(servo_elevation
		       , i + get_servo_position(servo_elevation));
  }

  struct Servos {
    Servos() { enable_servos(); }
    ~Servos() {
      Arm::raise_hand();
      nap();
      disable_servos(); }};

  void run_loop() {
    bool a, b;
    while( true ) {
      a = _row.step();
      b = _column.step();
      if( a && b  )
	return;
      }}
public:
  Arm() : _row(motor_arm, sensor_arm_positive, sensor_arm_negative)
	, _column(motor_shoulder, sensor_shoulder_positive, sensor_shoulder_negative) {
    open_hand();
    raise_hand();
  }

  ~Arm() {
    open_hand();
    raise_hand();
    ao();
  }
  
  /* Absolute co-ordinant on an 8x9 grid */
  void move_to(const iPair& pos) {
    _row.start_seeking( pos.row() );
    _column.start_seeking( pos.column() );

    run_loop();
  }

  void board_move(iPair pos) {
    std::cout << "Going to " << pos;
    pos = iPair(7 - pos.x, 2 * pos.y + (even(pos.x) ? 1 : 2)); 
    std::cout << " translated to " << pos << std::endl;
    move_to( pos );
    sleep(0.1);
    reset();
    
  }

  void reset() {
    _row.start_centering();
    _column.start_centering();

    run_loop();
  }

  void pick_up_all() {
    Servos s;
    open_hand();
    nap();
    nap();

    lower_hand();
    nap();

    close_hand();
    nap();
  }

  void put_down_all() {
    Servos s;
    lower_hand();
    nap();

    open_hand();
    nap();
  }

  void put_down_one() {
    Servos s;
    lower_hand();
    nap();
    open_hand();
    position_hand(10);
    close_hand();
  }

  void remove_piece(const iPair& capture) {
    std::cout << "Taking ";
    board_move(capture);
    pick_up_all();

    move_to( iPair(3,0) );
    put_down_all();
  }

  void execute_move(Move &m) {
    board_move(m.src);
    pick_up_all();

    board_move(m.dst);
    put_down_all();
  }
};

/* Keeps resetting the arm.  For reasons I can't explain,
   the arm looses track of its position count.  Since it's a lightweght class,
   and I'm running out of time, I'll just fource a reset
   every time the arm is used.
   */
struct Sling {
  Arm arm;
  Sling() : arm() {
    arm.reset();
  }

  ~Sling() {
    arm.move_to( iPair(0,0) );
    mav(motor_shoulder, -200);
    mav(motor_arm, -200);
    sleep(0.2);
    ao();
  }

  Arm& operator()() { return arm; }
};


#endif
