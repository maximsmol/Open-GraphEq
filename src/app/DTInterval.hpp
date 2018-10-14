#pragma once

#include <algorithm>

#include "IntervalUtils.hpp"
#include "Interval.hpp"
#include "BoolInterval.hpp"

struct DTInterval {
  public:
    DTInterval(Interval i, BoolInterval def);
    friend void swap(DTInterval& a, DTInterval& b);

    Interval unsafeI() const;
    BoolInterval def() const;

    inline double unsafeA() const {
      return unsafeI().unsafeA();
    }
    inline double unsafeB() const {
      return unsafeI().unsafeB();
    }

    BoolInterval lt(const DTInterval& that) const;
    inline BoolInterval gt(const DTInterval& that) const {
      return that.lt(*this);
    }
    // inline BoolInterval get(const DTInterval& that) const {
    //   return lt(that).not_();
    // }
    // inline BoolInterval let(const DTInterval& that) const {
    //   return gt(that).not_();
    // }

    DTInterval sqrt() const;
    DTInterval& operator+=(const DTInterval& that);
    DTInterval& operator-=(const DTInterval& that);
    DTInterval& operator*=(const DTInterval& that);
    DTInterval& operator/=(const DTInterval& that);

    DTInterval& operator++();
    inline DTInterval operator++(int) {
      DTInterval old(*this);
      operator++();
      return old;
    }

    DTInterval& operator--();
    inline DTInterval operator--(int) {
      DTInterval old(*this);
      operator--();
      return old;
    }

  private:
    Interval i_;
    BoolInterval def_;

    void assertInvariant() const;
    friend bool operator==(const DTInterval& l, const DTInterval& r);
};
template<>
inline DTInterval intervalFromReals<DTInterval>(double a, double b) {
  return DTInterval(forceInterval(a, b), alltrue);
}

bool operator==(const DTInterval& l, const DTInterval& r);

inline DTInterval operator+(DTInterval l, const DTInterval& r) {
  l += r;
  return l;
}
inline DTInterval operator-(DTInterval l, const DTInterval& r) {
  l -= r;
  return l;
}
inline DTInterval operator*(DTInterval l, const DTInterval& r) {
  l *= r;
  return l;
}
inline DTInterval operator/(DTInterval l, const DTInterval& r) {
  l /= r;
  return l;
}
