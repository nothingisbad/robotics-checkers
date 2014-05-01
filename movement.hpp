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

public:
  SensorGroup(int pos, int neg) {
    _sensor[0] = pos;
    _sensor[1] = neg;
    _position = 0;

    _gap_open = true;
  }
  
  int position() const { return _position; }
  
  void step(int direction) {
    if( toggle() && _gap_open)
      _position += sign(direction);
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
      sleep(0.2);
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
    _max_velocity = 650
    , _slow_velocity = 200
    , _slowest_velocity = 200;

  int _motor, _destination, _current_velocity;


  struct State {
    virtual State* operator()(Cart *)=0;
  };

  struct Done : public State {
    virtual State* operator()(Cart*) { return this; }
  };

  static Done _done;

  struct Centering : public State {
    State* init(Cart *c) {
      if(c->_sense.is_centered() == 0) {
	c->set_velocity(0);
	c->_state = &Cart::_done;
	return &Cart::_done;
      }

      c->set_velocity(Cart::_slowest_velocity);
      c->_state = this;
      return this;
    }

    Centering(Cart *c)  { init(c); }
    Centering() {}

    virtual State* operator()(Cart *cart) {
      int sense_state = cart->_sense.is_centered();
      if( sense_state == 0 ) {
	cart->set_velocity(0);
	return init(cart);
      }

      else {
	if( sense_state > 0 )
	  cart->set_velocity( Cart::_slow_velocity );

	else
	  cart->set_velocity( -Cart::_slow_velocity );
      }
      return this;
    }
  };

  struct Narrowing : public State {
    Centering center;
    bool toggle;

    Narrowing() {}
    Narrowing(Cart *c) {
      c->set_velocity(Cart::_slow_velocity);
      toggle = c->_sense.is_open(0);
      c->_state = this;
    }
    
    virtual State* operator()(Cart *cart) {
      cart->_sense.step(cart->direction());
      if( toggle != cart->_sense.is_open(0) ) {
	center = Centering(cart);
	return &center;
      } 
      return this;
    }
  };

  struct Seeking : public State {
    int destination;
    bool going_slow;
    Narrowing narrowing;

    Seeking(Cart *c, int dst) {
      std::cout << "Starting seek; initial position " << c->_sense.position() << std::endl;
      if( dst > c->_sense.position() )
	c->set_velocity(Cart::_max_velocity);
      else
	c->set_velocity(-Cart::_max_velocity);

      destination = dst;
      going_slow = false;
      c->_state = this;
    }
    
    virtual State* operator()(Cart *cart) {
      cart->_sense.step( cart->direction() );

      if( cart->_sense.position() == destination ) {
	narrowing = Narrowing(cart);
	return &narrowing;

      } else if( !going_slow && abs( cart->_sense.position() - destination ) < 2 )  {
	cart->set_velocity( cart->direction() * _slow_velocity);
	going_slow = true;
      }
      return this;
    }
  };


  SensorGroup _sense;
  State *_state;

  Cart(int motor, int s_pos, int s_neg)
    : _motor(motor), _current_velocity(_max_velocity), _sense(s_pos, s_neg), _state(&_done) {}

  void set_velocity(int vel) {
    _current_velocity = vel;
    mav(_motor, vel);
  }

  int abs_velocity() { return abs(_current_velocity); }
  int direction() { return _current_velocity > 0 ? 1 : -1; }

  /* A step in the movement loop, returns the state of this cart. */
  bool step() {
    _state = (*_state)(this);
    return _state == &_done;
  }
};
Cart::Done Cart::_done = Cart::Done{};

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
  void close_hand() { set_servo_position(servo_elevation, 723);  }

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
    while( true ) {
      if( _row.step() & _column.step() )
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
    Cart::Seeking r_seek(&_row, pos.row() );
    Cart::Seeking c_seek(&_column, pos.column() );

    run_loop();
  }

  void board_move(iPair pos) {
    std::cout << "Going to " << pos;
    pos = abs2diag( pos );
    pos = iPair(7 - pos.x, pos.y); 
    
    std::cout << " translated to " << pos << std::endl;
    move_to( pos );
  }

  void reset() {
    Cart::Centering r_cnt(&_row);
    Cart::Centering c_cnt(&_column);

    run_loop();
  }

  void pick_up_all() {
    Servos s;

    open_hand();
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

  void execute_move(Move &m) {
    if( m.is_capture() ) {
      board_move(m.capture);
      pick_up_all();

      board_move(m.src);
      pick_up_all();
      
      board_move(m.dst);
      put_down_one();

      board_move( iPair(0,9) );
      put_down_all();

    } else {
      board_move(m.src);
      // pick_up_all();

      board_move(m.dst);
      // put_down_all();
    }
  }
};

#endif
