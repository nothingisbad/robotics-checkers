#ifndef MOVEMENT_HPP
#define MOVEMENT_HPP
/**
 * @file /home/ryan/uml/robotics/checkers/movement.hpp
 * @author Ryan Domigan <ryan_domigan@sutdents@uml.edu>
 * Created on Apr 04, 2014
 *
 * Move and read position of the gantry arm
 */

#include "./checkers.hpp"
#include "./grid.hpp"

#include <cbc.h>

/**
 * returns -1 for input < 0, 1 for input > 0
 */
int sign(int input) { return input > 0 ? 1 : -1; }


class SensorGroup {
public:
  enum State { done = 0, ok = 1, reverse = 2, fail = 4 };

private:
  int _front_sensor
    , _back_sensor;

  bool _gap_open[2];

  int _front_position, _back_position;

  bool beam_broken(int sensor) { return digital(sensor); };

  bool toggle(int n) {
    bool sensed = beam_broken(n);
    if( sensed  == _gap_open[n] )
      return false;
    else {
      _gap_open[n] = sensed;
      return true;
    }}

public:
  SensorGroup(int front, int back)
    : _front_sensor(ahead), _back_sensor(behind), _front_position(1), _back_position(0) {}
  
  int grid_position() const { return _back_position; }
  
  State run(int direction) {
    if(direction > 0)
      direction = 1;
    else
      direction = -1;
    
    if( toggle(_front) )
      _position += direction;

    if( toggle(_back) )
      _back_position += direction;

    if( _back_position + 1 != _front_position)
      return fail;
      
    return ok;  
  }

  /* assuming we've counted to the desired position, keep moving towards the centered-point. */
  State is_centered(int direction) {
    int lead,follow;
    sensor_lead_follow(direction, lead, follow);

    switch( beam_broken(lead) | (beam_broken(follow) << 1) ) {
    case 0: return done;

    case 1: return ok;

    case 2: return reverse;

    case 3: return fail; 		/* both sensors are blocked; position marker out of place? */
    }
  }
};

/*************************/
/*   ____           _    */
/*  / ___|__ _ _ __| |_  */
/* | |   / _` | '__| __| */
/* | |__| (_| | |  | |_  */
/*  \____\__,_|_|   \__| */
/*************************/
class Cart {
public:
  enum State { done = 0, seeking = 1, centering = 2 , fail = 4};

private
static const int _max_velocity = (750 / 8);
  , _slow_velocity = (_max_velocity / 4)
  , _max_acceleration = 1;

  int _motor, _pos
    , _destination
    , _half_way
    , _current_velocity, _trip_max_velocity
    , _counter
    , _acceleration;

  SensorGroup _sense;

  State _state;
  
public:
  /**
   * Trim the velocity to arrive at approximately a given time.
   * 
   * @param time: when to arrive
   * @param dst: where to go
   */
  void set_destination(int dest) {
    _destination = dest;
    _half_way = (dest + _pos) / 2;
    _trip_max_velocity = ((_destination - _pos) * _max_velocity) / time_bound;
  }

  void reset_velocity() {
    _current_velocity = 0;
    _counter = 0;
    _acceleration = _max_acceleration;
  }

  /* A step in the movement loop, returns the state of this cart. */
  State run() {
    SensorGroup::State sensor_state = _sense.run(_velocity);

    if( sensor_state == SensorGroup::State::fail )
      goto fail_state;
    
    /* check on acceleration */
    if( (++_counter % 50) == 0 ) {
      _counter = 0;
      _current_velocity != acceleration;
      set_motor();
    }

    switch( _state ) {
    case done:
      return done;

    case seeking:
      if(_sense.position() == _half_way)
	_acceleration = -_acceleration;

      if(_current_velocity <= _slow_velocity
	 || abs(_sense.position() - _destination) <= 2)
	_acceleration = 0;
      
      if( _sense.position() == _destination ) {
	_state = centering;
	_velocity = sign(_velocity) * _slow_velocity;

	goto set_motor;
      }
      break;

    case centering:
      switch( _sense.is_centered(_velocity) ) {
      case SensorGroup::State::ok:
	break;

      case SensorGroup::State::done:
	_velocity = 0;
	_state = done;
	set_motor();
	break;

      case SensorGroup::State::reverse:
	_velocity = -_velocity;
	set_motor();
	break;

      case fail:
	goto fail_state;
      }
      break;

    case fail:
      return fail;
    }
    return _state;

  fail_state:
    _velocity = 0;
    _state = fail;
    set_motor();

  set_motor:
    /* todo: put the mav statement here. */
    return _state;
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
  Cart _row, _column;
  const static int _hand_elevation = 0
    , _hand_open = 1;

  void open_hand() {
    set_servo_positon(_hand_open, 1694);  }
  void close_hand() {
    set_servo_positon(_hand_open, 990);  }

  static void raise_hand() {
    set_servo_positon(_hand_elevation, 1987);  }

  void lower_hand() {
    set_servo_positon(_hand_elevation, 930);
  }

  void position_hand(int i) {
    set_servo_position(_hand_elevation
		       , i + get_servo_position(_hand_elevation));
  }

  struct Servos {
    Servos() { enable_servos(); }
    ~Servos() {
      Arm::raise_hand();
      disable_servos(); }};

public:
  /* Absolute co-ordinant on an 8x8 grid */
  bool move_to(iPair pos) {
    int row_state
      , column_state;

    _row.reset_velocity();
    _column.reset_velocity();

    while( true ) {
      switch( (row_state = _row.run()) && (column_state = _column.run()) ) {
      case done:
	return true;

      case fail:
	/* todo: retry */
	return false;

      default:
	continue;
      }
    }}

  void pick_up_all() {
    Servos s;

    open_hand();
    lower_hand();
    close_hand();
  }

  void put_down_all() {
    Servos s;
    drop_hand();
    open_hand();
  }

  void put_down_one() {
    Servos s;
    drop_hand();
    open_hand();
    position_hand(10);
    close_hand();
  }

  void execute_move(Move &m) {
    if( m.is_capture() ) {
      move(m.capture);
      pick_up_all();

      move(m.src);
      pick_up_all();
      
      move(m.dst);
      put_down_one();

      move( iPair(0,9) );
      put_down_all();

    } else {
      move(m.src);
      pick_up_all();

      move(m.dst);
      put_down_all();
    }
  }
};

#endif
