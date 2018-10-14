#include "BoolInterval.hpp"

#include <cassert>

const BoolInterval alltrue = BoolInterval(true, true);
const BoolInterval halftrue = BoolInterval(false, true);
const BoolInterval notrue = BoolInterval(false, false);

BoolInterval::BoolInterval(bool a, bool b) :
  a_(a), b_(b)
{
  assertInvariant();
}

void swap(BoolInterval& a, BoolInterval& b) {
  a.assertInvariant(); b.assertInvariant();

  using std::swap;
  swap(a.a_, b.a_);
  swap(a.b_, b.b_);
}

bool BoolInterval::a() const {
  return a_;
}
bool BoolInterval::b() const {
  return b_;
}

BoolInterval BoolInterval::not_() const {
  assertInvariant();
  return BoolInterval(!a_ && !b_, !b_ || !a_);
}

void BoolInterval::assertInvariant() const {
  assert(!a_ || b_);
}

bool operator==(const BoolInterval& l, const BoolInterval& r) {
  l.assertInvariant(); r.assertInvariant();
  return l.a_ == r.a_ && l.b_ == r.b_;
}
