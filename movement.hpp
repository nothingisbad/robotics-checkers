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

<<<<<<< HEAD
class SensorGroup {
public:
  enum Sensing { ok = 0, done = 1, reverse = 2, fail = 4 };
=======
/**
 * returns -1 for input < 0, 1 for input > 0
 */
int sign(int input) { return input > 0 ? 1 : -1; }


class SensorGroup {
public:
  enum State { done = 0, ok = 1, reverse = 2, fail = 4 };

>>>>>>> 8f73400739320b4ee2e1510273aa98a7ad41e43d
private:
  int _long
    , _behind
    , _ahead;

  bool _prev[3];

  int _position;

  bool beam_broken(int sensor) {
    return digital(sensor);
  };

  bool toggle(int n) {
    bool sensed = beam_broken(n);
    if( sensed  == _prev[n] )
      return false;
    else {
      _prev[n] = sensed;
      return true;
    }
  }

  int sensor_lead_follow(int direction, int &lead, int &follow) {
    if(direction > 0) {
      lead = _ahead;
      follow = _behind;
    } else {
      lead = _behind;
      follow = _ahead;
    }
  }

public:
  int grid_position() const { return _position; }
  
<<<<<<< HEAD
  /* true for OK, false for error */
  Sensing run(int direction) {
=======
  State run(int direction) {
>>>>>>> 8f73400739320b4ee2e1510273aa98a7ad41e43d
    int lead;
    if(direction > 0) {
      lead = _ahead;
      direction = 1;
    } else {
      lead = _behind;
      direction = -1;
    }
    
    if( toggle(lead) ) {
      _position += direction;
      if( sense(_long) && !even(_position) )
<<<<<<< HEAD
	return false;
    }
    return true;
  }

  /* assuming we've counted to the desired position, keep moving towards the centered-point. */
  Sensing is_centered(int direction) {
=======
	return running;
    }
    return fail;
  }

  /* assuming we've counted to the desired position, keep moving towards the centered-point. */
  State is_centered(int direction) {
>>>>>>> 8f73400739320b4ee2e1510273aa98a7ad41e43d
    int lead,follow;
    sensor_lead_follow(direction, lead, follow);

    switch( (beam_broken(lead) << 1) | beam_broken(follow) ) {
    case 0:
      return done;

    case 2:
      return ok;

<<<<<<< HEAD
    if( !beam_broken(lead) && beam_broken(follow) )
  }
};

class Cart {
  static const int _max_velocity = 750;
  
  int _position
    , _velocity;

  int _motor;
public:
  int propotional_distance(int i) {
    i -= _position;
    return i * i;
  }

=======
    case 1:
      return reverse;

    case 3:
      return fail; 		/* both sensors are blocked; position marker out of place? */
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
  static const int _max_velocity = 750
    , _slow_velocity = 100;

  int _motor, _pos, _destination, _velocity;

  SensorGroup _sense;

  State _state;
  
public:
>>>>>>> 8f73400739320b4ee2e1510273aa98a7ad41e43d
  /**
   * Estimate time to move to a point
   * 
   * @param : distance to move
   * @return: time it will take
   */
<<<<<<< HEAD
  int est_time(int dst) {
    return (_dst - _position) / _max_velocity;
  }


  /**
   * Trim the velocity to arrive at about time.
=======
  int estimate_time(int dst) {
    return (dst - _pos) / _max_velocity;
  }

  /**
   * Trim the velocity to arrive at about time.
   * The idea is I if the cart on one axis doesn't have to travel as far, I should
   * run it more slowely to save the gears and battery.
>>>>>>> 8f73400739320b4ee2e1510273aa98a7ad41e43d
   * 
   * @param time: when to arrive
   * @param dst: where to go
   */
<<<<<<< HEAD
  void move(int time, int dst) {
    
  }

  void run() {
    
  }
};

class Arm {
  iPair _velocity
    , _position;
  
public:
  /* Absolute co-ordinant on an 8x8 grid */
  void move_to(iPair pos) {
    
=======
  void set_velocity(int time, int dst) {
    _velocity = ((_destination - _pos) * _max_velocity) / time;
  }

  /* A step in the movement loop, returns the state of this cart. */
  State run() {
    SensorGroup::State sensor_state = _sense.run(_velocity);

    if( sensor_state == SensorGroup::State::fail )
      goto fail_state;


    switch( _state ) {
    case done:
      return done;

    case seeking:
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

	goto set_motor;

      case SensorGroup::State::reverse:
	_velocity = -_velocity;
	goto set_motor;

      case fail:
	goto fail_state;

      }
      break;


    case fail:
      return fail;
    }
    break;

  fail_state:
    _velocity = 0;
    _state = fail;

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
public:
  /* Absolute co-ordinant on an 8x8 grid */
  bool move_to(iPair pos) {
    int time_row = _row.estimate_time( pos.x )
      , time_column = _column.estimate_time( pos.y )
      , max_time = ((time_column > time_row) ? time_column : time_row)
      , row_state
      , column_state;

    _row.set_velocity( max_time );
    _column.set_velocity( max_time);

    while( true ) {
      switch( (row_state = _row.run()) | (column_state = _column.run()) ) {
      case done:
	return true;

      case fail:
	/* todo: retry */
	return false;
      }
    }
>>>>>>> 8f73400739320b4ee2e1510273aa98a7ad41e43d
  };
};

#endif
