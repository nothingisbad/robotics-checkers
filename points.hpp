#ifndef POINTS_HPP
#define POINTS_HPP
/**
 * @file /home/ryan/uml/robotics/checkers/points.hpp
 * @author Ryan Domigan <ryan_domigan@sutdents@uml.edu>
 * Created on Mar 30, 2014
 *
 * Two and three dimentional points
 */

template<class Num>
class Pair {
public:
  Num _x, _y;
  Pair() = default;

  Pair(Num x, Num y) : _x(x), _y(y) {}

  Pair operator+(const Pair& p) const {
    return Pair(p._x + _x, p._y + _y);  }

  Pair operator-(const Pair& p) const {
    return Pair(p._x - _x, p._y - _y);  }


  bool operator<(const Pair& p) const {
    return (_x < p._x) && (_y < p._y);  }

  bool operator>(const Pair& p) const {
    return (_x > p._x) && (_y > p._y);  }


  bool operator==(const Pair& p) const {
    return (_x == p._x) && (_y == p._y);
  }
};

template<class Num>
std::ostream& operator<<(std::ostream &out, const Pair<Num>& p) {
  return out << "(" << p._x << "," << p._y << ")";
}

typedef Pair<int> iPair;
typedef Pair<double> dPair;

template<class Num>
struct Triplet : public Pair<Num> {
  typedef Pair<Num> P;
  
  Num _z;

  Triplet() = default;
  Triplet(P p, Num z) : P(p), _z(z) {}
  Triplet(Num x, Num y, Num z) : P(x, y), _z(z) {}


  Triplet& operator-() {
    P::_x = -P::_x;
    P::_y = -P::_y;
    _z = -_z;
    return *this;
  }

  Triplet operator+(const Triplet& input) const {
    return Triplet(P::_x + input._x, P::_y + input._y, input._z + _z);
  }

  Triplet operator-(const Triplet& input) const {
    return Triplet(P::_x - input._x, P::_y - input._y, _z - input._z);
  }

  Triplet& operator+=(const Triplet& in) {
    P::_x += in._x;
    P::_y += in._y;
    _z += in._z;
    return *this;
  }

  Triplet& operator=(const Triplet& in) {
    P::_x = in._x;
    P::_y = in._y;
    _z = in._z;
    return *this;
  }
};

typedef Triplet<double> dTriplet;

template<class Num>
std::ostream& operator<<(std::ostream &out, const Triplet<Num>& p) {
  return out << "(" << p._x << "," << p._y << "," << p._z << ")";
}


#endif
