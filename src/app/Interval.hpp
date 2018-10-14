#pragma once

#include <algorithm>

#include "IntervalUtils.hpp"
#include "BoolInterval.hpp"

struct Interval {
  public:
    Interval(double a, double b);
    friend void swap(Interval& a, Interval& b);

    double a() const;
    double b() const;
    inline double unsafeA() const {
      return a();
    }
    inline double unsafeB() const {
      return b();
    }

    BoolInterval lt(const Interval& that) const;
    inline BoolInterval gt(const Interval& that) const {
      return that.lt(*this);
    }
    // inline BoolInterval get(const Interval& that) const {
    //   return lt(that).not_();
    // }
    // inline BoolInterval let(const Interval& that) const {
    //   return gt(that).not_();
    // }

    Interval sqrt() const;
    Interval& operator+=(const Interval& that);
    Interval& operator-=(const Interval& that);
    Interval& operator*=(const Interval& that);
    Interval& operator/=(const Interval& that);

    Interval& operator++();
    inline Interval operator++(int) {
      Interval old(*this);
      operator++();
      return old;
    }

    Interval& operator--();
    inline Interval operator--(int) {
      Interval old(*this);
      operator--();
      return old;
    }

  private:
    double a_, b_;

    void assertInvariant() const;
    friend bool operator==(const Interval& l, const Interval& r);
};
inline Interval forceInterval(double a, double b) {
  return Interval(std::min(a, b), std::max(a, b));
}
template<>
inline Interval intervalFromReals<Interval>(double a, double b) {
  return forceInterval(a, b);
}

bool operator==(const Interval& l, const Interval& r);

inline Interval operator+(Interval l, const Interval& r) {
  l += r;
  return l;
}
inline Interval operator-(Interval l, const Interval& r) {
  l -= r;
  return l;
}
inline Interval operator*(Interval l, const Interval& r) {
  l *= r;
  return l;
}
inline Interval operator/(Interval l, const Interval& r) {
  l /= r;
  return l;
}

extern const Interval allnan;
