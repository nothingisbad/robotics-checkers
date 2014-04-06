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

class SensorGroup {
public:
  enum Sensing { ok = 0, done = 1, reverse = 2, fail = 4 };
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
  
  /* true for OK, false for error */
  Sensing run(int direction) {
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
	return false;
    }
    return true;
  }

  /* assuming we've counted to the desired position, keep moving towards the centered-point. */
  Sensing is_centered(int direction) {
    int lead,follow;
    sensor_lead_follow(direction, lead, follow);

    switch( (beam_broken(lead) << 1) | beam_broken(follow) ) {
    case 0:
      return done;

    case 2:
      return ok;

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

  /**
   * Estimate time to move to a point
   * 
   * @param : distance to move
   * @return: time it will take
   */
  int est_time(int dst) {
    return (_dst - _position) / _max_velocity;
  }


  /**
   * Trim the velocity to arrive at about time.
   * 
   * @param time: when to arrive
   * @param dst: where to go
   */
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
    
  };
};

#endif
