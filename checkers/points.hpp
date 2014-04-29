#ifndef POINTS_HPP
#define POINTS_HPP
/**
 * @file /home/ryan/uml/robotics/checkers/points.hpp
 * @author Ryan Domigan <ryan_domigan@sutdents@uml.edu>
 * Created on Mar 30, 2014
 *
 * Two and three dimentional points
 */

#include <iostream>

template<class Num>
class Pair {
public:
  Num x, y;
  Pair() = default;

  template<class N>
  Pair(const Pair<N>& p) : x(p.x), y(p.y) {}

  Pair(Num i_x, Num i_y) : x(i_x), y(i_y) {}

  Num& operator[](int i) {
    if(i == 0) return x;
    return y;
  }

  const Num& operator[](int i) const {
    if(i == 0) return x;
    return y;
  }

  Pair operator+(const Pair& p) const {
    return Pair(p.x + x, p.y + y);  }

  Pair operator-(const Pair& p) const {
    return Pair(x - p.x, y - p.y);  }

  Pair operator/(int i) const {
    return Pair(x / i, y / i);  }

  /* scalar product */
  Num dot(const Pair& p) const { return (x * p.x) + (y * p.y);  }

  bool operator<(const Pair& p) const {
    return (x < p.x) && (y < p.y);  }

  bool operator>(const Pair& p) const {
    return (x > p.x) && (y > p.y);  }

  bool operator<=(const Pair& p) const {
    return (x <= p.x) && (y <= p.y);  }

  bool operator>=(const Pair& p) const {
    return (x >= p.x) && (y >= p.y);  }


  bool operator==(const Pair& p) const {
    return (x == p.x) && (y == p.y);
  }

  bool operator!=(const Pair& p) const { return !(*this == p); }

  Num& row() { return x; }
  Num& column() { return x; }

  Num row() const { return x; }
  Num column() const { return x; }
};

template<class Num>
std::ostream& operator<<(std::ostream &out, const Pair<Num>& p) {
  return out << "(" << p.x << "," << p.y << ")";
}

typedef Pair<int> iPair;
typedef Pair<double> dPair;
typedef Pair<float> fPair;

template<class Num>
struct Triplet : public Pair<Num> {
  typedef Pair<Num> P;
  
  Num z;

  Triplet() = default;
  Triplet(P p, Num z) : P(p), z(z) {}
  Triplet(Num x, Num y, Num z) : P(x, y), z(z) {}

  Triplet& operator-() {
    P::x = -P::x;
    P::y = -P::y;
    z = -z;
    return *this;
  }

  Triplet operator+(const Triplet& input) const {
    return Triplet(P::x + input.x, P::y + input.y, input.z + z);
  }

  Triplet operator-(const Triplet& input) const {
    return Triplet(P::x - input.x, P::y - input.y, z - input.z);
  }

  Triplet& operator+=(const Triplet& in) {
    P::x += in.x;
    P::y += in.y;
    z += in.z;
    return *this;
  }

  Triplet& operator=(const Triplet& in) {
    P::x = in.x;
    P::y = in.y;
    z = in.z;
    return *this;
  }
};

typedef Triplet<double> dTriplet;

template<class Num>
std::ostream& operator<<(std::ostream &out, const Triplet<Num>& p) {
  return out << "(" << p.x << "," << p.y << "," << p.z << ")";
}


#endif
