#pragma once

#include <algorithm>

struct BoolInterval {
  public:
    BoolInterval(bool a, bool b);
    friend void swap(BoolInterval& a, BoolInterval& b);

    bool a() const;
    bool b() const;

    BoolInterval not_() const;
  private:
    bool a_, b_;

    void assertInvariant() const;
    friend bool operator==(const BoolInterval& l, const BoolInterval& r);
};
inline BoolInterval forceBoolInterval(bool a, bool b) {
  return BoolInterval(a && b, a || b);
}

bool operator==(const BoolInterval& l, const BoolInterval& r);
inline bool operator!=(const BoolInterval& l, const BoolInterval& r) {return !(l == r);}

extern const BoolInterval alltrue;
extern const BoolInterval halftrue;
extern const BoolInterval notrue;
