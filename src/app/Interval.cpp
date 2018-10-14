#include "Interval.hpp"

#include "updown.hpp"

#include <cmath>
#include <cassert>

const Interval allnan = Interval(nan(""), nan(""));

Interval::Interval(double a, double b) :
  a_(a), b_(b)
{
  // if (isnan(a_) || isnan(b_))
  //   a_ = b_ = nan("");
  assertInvariant();
}

double Interval::a() const {
  assertInvariant();
  return a_;
}
double Interval::b() const {
  assertInvariant();
  return b_;
}

inline void Interval::assertInvariant() const {
  assert(isnan(a_) == isnan(b_));
  assert(isnan(a_) || a_ <= b_);

  // assert(!isnan(b_) || isnan(a_));
  // assert(isnan(a_) || isnan(b_) || a_ <= b_);
}

void swap(Interval& a, Interval& b) {
  a.assertInvariant(); b.assertInvariant();

  using std::swap;
  swap(a.a_, b.a_);
  swap(a.b_, b.b_);
}

BoolInterval Interval::lt(const Interval& that) const {
  assertInvariant();
  that.assertInvariant();

  return BoolInterval(b_ < that.a_, a_ < that.b_);
}

Interval Interval::sqrt() const {
  assertInvariant();

  return Interval(sqrtDown(a_), sqrtUp(b_));
}
Interval& Interval::operator+=(const Interval& that) {
  assertInvariant(); that.assertInvariant();

  a_ = addDown(a_, that.a_);
  b_ = addUp(b_, that.b_);
  return *this;
}
Interval& Interval::operator-=(const Interval& that) {
  assertInvariant(); that.assertInvariant();

  a_ = subDown(a_, that.b_);
  b_ = subUp(b_, that.a_);
  return *this;
}
Interval& Interval::operator*=(const Interval& that) {
  assertInvariant(); that.assertInvariant();

  a_ = mulDown(a_, that.a_);
  b_ = mulUp(b_, that.b_);
  return *this;
}
Interval& Interval::operator/=(const Interval& that) {
  assertInvariant(); that.assertInvariant();

  a_ = divDown(a_, that.b_);
  b_ = divUp(b_, that.a_);
  return *this;
}

Interval& Interval::operator++() {
  assertInvariant();

  ++a_; ++b_;
  return *this;
}
Interval& Interval::operator--() {
  assertInvariant();

  --a_; --b_;
  return *this;
}

bool operator==(const Interval& l, const Interval& r) {
  l.assertInvariant(); r.assertInvariant();

  return l.a_ == r.a_ && l.b_ == r.b_;
}
