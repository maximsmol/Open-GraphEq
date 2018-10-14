#include "DTInterval.hpp"

#include <cmath>

DTInterval::DTInterval(Interval i, BoolInterval def) :
  i_(i),
  def_(def)
{
  assertInvariant();
}

void DTInterval::assertInvariant() const {
  assert(
    def_ == alltrue ||
    (isnan(i_.a()) && isnan(i_.b()))
  );
}

void swap(DTInterval& a, DTInterval& b) {
  a.assertInvariant(); b.assertInvariant();

  using std::swap;
  swap(a.i_, b.i_);
  swap(a.def_, b.def_);
}

Interval DTInterval::unsafeI() const {
  assert(def_ == alltrue);
  assertInvariant();
  return i_;
}
BoolInterval DTInterval::def() const {
  assertInvariant();
  return def_;
}

BoolInterval DTInterval::lt(const DTInterval& that) const {
  assertInvariant(); that.assertInvariant();

  if (def_ == notrue || that.def_ == notrue)
    return notrue;
  if (def_ == halftrue || that.def_ == halftrue)
    return halftrue;

  return i_.lt(that.i_);
}

DTInterval DTInterval::sqrt() const {
  assertInvariant();

  if (def_ == notrue)
    return *this;
  if (def_ == halftrue)
    return *this;

  BoolInterval def(i_.b() < 0, i_.a() < 0);

  if (def != alltrue)
    return DTInterval(allnan, def);

  return DTInterval(i_.sqrt(), def);
}
DTInterval& DTInterval::operator+=(const DTInterval& that) {
  assertInvariant(); that.assertInvariant();

  if (def_ == notrue || that.def_ == notrue) {
    def_ = notrue;
    i_ = allnan;
  }
  else if (def_ == halftrue || that.def_ == halftrue) {
    def_ = halftrue;
    i_ = allnan;
  }
  else {
    // here there are just 4 cases where the result is undefined
    // they are on the infinities of the number-line
    // thus we check if we get there at all,
    // and if some value in i_ lied between the infinities, giving a well-defined result
    bool nanInA =
      (i_.a() == -HUGE_VAL && that.i_.a() == HUGE_VAL) || // <-inf + inf, ?>
      (i_.a() == HUGE_VAL && that.i_.a() == -HUGE_VAL); // <inf + -inf, ?>
    bool nanInB =
      (i_.b() == -HUGE_VAL && that.i_.b() == HUGE_VAL) || // <?, -inf + inf>
      (i_.b() == HUGE_VAL && that.i_.b() == -HUGE_VAL); // <?, inf + -inf>

    def_ = BoolInterval(
      (!nanInA && !nanInB), // the result does contain a nan
      i_.a() != i_.b() || (!nanInA && !nanInB)); // some of the result had to cover a non-nan'ing area

    if (def_ != alltrue)
      i_ = allnan;
    else
      i_ += that.i_;
  }

  return *this;
}
DTInterval& DTInterval::operator-=(const DTInterval& that) {
  assertInvariant(); that.assertInvariant();

  if (def_ == notrue || that.def_ == notrue) {
    def_ = notrue;
    i_ = allnan;
  }
  else if (def_ == halftrue || that.def_ == halftrue) {
    def_ = halftrue;
    i_ = allnan;
  }
  else {
    // here there are just 4 cases where the result is undefined
    // they are on the infinities of the number-line
    // thus we check if we get there at all,
    // and if some value in i_ lied between the infinities, giving a well-defined result
    bool nanInA =
      (i_.a() == HUGE_VAL && that.i_.b() == HUGE_VAL) || // <inf - inf, ?>
      (i_.a() == -HUGE_VAL && that.i_.b() == -HUGE_VAL); // <-inf - -inf, ?>
    bool nanInB =
      (i_.b() == HUGE_VAL && that.i_.a() == HUGE_VAL) || // <?, inf - inf>
      (i_.b() == -HUGE_VAL && that.i_.a() == -HUGE_VAL); // <?,-inf - -inf>

    def_ = BoolInterval(
      (!nanInA && !nanInB), // the result does contain a nan
      i_.a() != i_.b() || (!nanInA && !nanInB)); // some of the result had to cover a non-nan'ing area

    if (def_ != alltrue)
      i_ = allnan;
    else
      i_ -= that.i_;
  }

  return *this;
}
DTInterval& DTInterval::operator*=(const DTInterval& that) {
  assertInvariant(); that.assertInvariant();

  if (def_ == notrue || that.def_ == notrue) {
    def_ = notrue;
    i_ = allnan;
  }
  else if (def_ == halftrue || that.def_ == halftrue) {
    def_ = halftrue;
    i_ = allnan;
  }
  else {
    // here we have a nan whenever 0 * inf arises from some choice of the interval values

    bool containsZero =
      i_.a() <= 0 != i_.b() < 0;
      // <= so that <0, 0> is still a true

    bool thatContainsZero =
      that.i_.a() <= 0 != that.i_.b() < 0;
      // <= so that <0, 0> is still a true

    bool zeroByInf = containsZero && (abs(that.i_.a()) == HUGE_VAL || abs(that.i_.b()) == HUGE_VAL);
    bool infByZero = thatContainsZero && (abs(i_.a()) == HUGE_VAL || abs(i_.b()) == HUGE_VAL);

    def_ = BoolInterval(
      (!zeroByInf && !infByZero), // no nans anywhere
      i_.a() != i_.b() || that.i_.a() != that.i_.b() || (!zeroByInf && !infByZero)); // some other result exists


    if (def_ != alltrue)
      i_ = allnan;
    else
      i_ *= that.i_;
  }

  return *this;
}
DTInterval& DTInterval::operator/=(const DTInterval& that) {
  assertInvariant(); that.assertInvariant();

  if (def_ == notrue || that.def_ == notrue) {
    def_ = notrue;
    i_ = allnan;
  }
  else if (def_ == halftrue || that.def_ == halftrue) {
    def_ = halftrue;
    i_ = allnan;
  }
  else {
    // here we have a nan whenever inf / inf or x / 0 arises from some choice of the interval values

    // bool containsZero =
    //   i_.a() <= 0 != i_.b() < 0;
    //   // <= so that <0, 0> is still a true

    bool thatContainsZero =
      that.i_.a() <= 0 != that.i_.b() < 0;
      // <= so that <0, 0> is still a true

    bool infByInf =
      (abs(i_.a()) == HUGE_VAL || abs(i_.b()) == HUGE_VAL) &&
      (abs(that.i_.a()) == HUGE_VAL || abs(that.i_.b()) == HUGE_VAL);
    // bool zeroByZero = containsZero && thatContainsZero;
    bool divByZero = thatContainsZero;

    // printf("%d %d %d %d\n", containsZero, thatContainsZero, infByInf, divByZero);
    // printf("%f %f / %f %f\n", i_.a(), i_.b(), that.i_.a(), that.i_.b());

    def_ = BoolInterval(
      (!infByInf && !divByZero), // no nans anywhere
      i_.a() != i_.b() || that.i_.a() != that.i_.b() || (!infByInf && !divByZero)); // some non-nan result exists

    if (def_ != alltrue)
      i_ = allnan;
    else
      i_ /= that.i_;
  }

  return *this;
}
