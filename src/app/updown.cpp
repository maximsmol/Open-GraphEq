#include "updown.hpp"

#include <cfenv>
#include <cmath>

double addUp(double a, double b) {
  fesetround(FE_UPWARD);
  return a+b;
}
double addDown(double a, double b) {
  fesetround(FE_DOWNWARD);
  return a+b;
}

double subUp(double a, double b)  {
  fesetround(FE_UPWARD);
  return a-b;
}
double subDown(double a, double b)  {
  fesetround(FE_DOWNWARD);
  return a-b;
}

double mulUp(double a, double b)  {
  fesetround(FE_UPWARD);
  return a*b;
}
double mulDown(double a, double b)  {
  fesetround(FE_DOWNWARD);
  return a*b;
}

double divUp(double a, double b)  {
  fesetround(FE_UPWARD);
  return a/b;
}
double divDown(double a, double b)  {
  fesetround(FE_DOWNWARD);
  return a/b;
}

double sqrtUp(double x)  {
  fesetround(FE_UPWARD);
  return sqrt(x);
}
double sqrtDown(double x)  {
  fesetround(FE_DOWNWARD);
  return sqrt(x);
}
