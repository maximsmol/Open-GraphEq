#pragma once

#include <set>
#include <utility>
#include <numeric>
#include <cmath>

#include "Rolmodl.hpp"

#include "updown.hpp"

// double intervalHash(const Interval& x);
// double intervalHash(const Interval& x) {
//   return 59*(37*(17 + x.a()) + x.b());
// }
// size_t grapher::detail::IntervalPairHash::operator()(const std::pair<Interval, Interval>& x) const {
//   return static_cast<size_t>(59*(37*(17 + intervalHash(x.first)) + intervalHash(x.second)));
// }

namespace grapher::detail {
  inline int toInt(double x) {
    return static_cast<int>(x);
  }

  template<class I_type>
  struct IntervalPairComparator { // todo: check for validity
    constexpr bool operator()(const std::pair<I_type, I_type>& l, const std::pair<I_type, I_type>& r) const {
      return // use left side of x-axis
        l.first.unsafeA() < r.first.unsafeA() ||
        (l.first.unsafeA() == r.first.unsafeA() && l.second.unsafeA() < r.second.unsafeA());
    }
  };
  // struct IntervalPairHash {
  //   size_t operator()(const std::pair<I_type, I_type>& x) const;
  // };
}
template<class I>
struct Grapher {
  using I_type = I;
  using U_type = std::set<std::pair<I_type, I_type>, grapher::detail::IntervalPairComparator<I_type>>;

  using graphfx = I_type(I_type x);
  using graphfy = BoolInterval(I_type fx, I_type y);

  public:
    Grapher(
        rolmodl::Ren& ren,
        graphfx fx, graphfy fy,
        double L, double B, double R, double T,
        rolmodl::geom::RectWH screen)  :
      ren_(ren),
      fx_(fx), fy_(fy),
      L_(L), B_(B), R_(R), T_(T),
      screen_(screen),

      U_({}), U1_({}), k_(0)
    {
      k_ = std::gcd(screen.w, screen.h);
      k_ = static_cast<int>(log2(
        std::gcd(
          k_,
          1 << static_cast<int>(log2(k_)) )
      ));

      int k2 = 1 << k_;
      for (int a = 0; a < screen_.w; a += k2)
        for (int b = 0; b < screen_.h; b += k2)
          U_.insert(
            std::pair(
              intervalFromReals<I_type>(a, a+k2),
              intervalFromReals<I_type>(b, b+k2)) );
    }

    void finish()  {
      while (k_ >= 0 && !U_.empty()) {
        step();
      }
    }
    void drawCurGrid() {
      ren_.setColor(rolmodl::RGBA{255, 0, 255});
      for (const std::pair<I_type, I_type>& p : U_)
        ren_.drawRect(regionToRect(p.first, p.second));
    }
    void step() {
      --k_;
      printf("  U_len: %lu, U1_len: %lu\n", U_.size(), U1_.size());

      U1_.clear();

      bool firstRun = true;
      double lastX = -1;
      I_type fx = intervalFromReals<I_type>(0, 0);
      for (const std::pair<I_type, I_type>& p : U_) {
        const I_type& a = p.first;
        const I_type& b = p.second;

        if (lastX != a.unsafeA() || firstRun) {
          firstRun = false;

          lastX = a.unsafeA();
          fx = fx_(pixelToRealH(a));
        }

        BoolInterval r = fy_(fx, pixelToRealV(b));
        if (r.a())
          ren_.setColor(rolmodl::RGBA{0, 0, 0});
        else if (!r.b())
          ren_.setColor(rolmodl::RGBA{255, 255, 255});
        else {
          ren_.setColor(rolmodl::RGBA{255, 0, 0});

          double midXL = divDown(addDown(a.unsafeA(), a.unsafeB()), 2);
          double midXR = divUp(addUp(a.unsafeA(), a.unsafeB()), 2);

          double midYB = divDown(addDown(b.unsafeA(), b.unsafeB()), 2);
          double midYT = divUp(addUp(b.unsafeA(), b.unsafeB()), 2);

          U1_.insert(
            std::pair(
              intervalFromReals<I_type>(a.unsafeA(), midXR),
              intervalFromReals<I_type>(b.unsafeA(), midYT)
            )
          );
          U1_.insert(
            std::pair(
              intervalFromReals<I_type>(midXL, a.unsafeB()),
              intervalFromReals<I_type>(b.unsafeA(), midYT)
            )
          );
          U1_.insert(
            std::pair(
              intervalFromReals<I_type>(a.unsafeA(), midXR),
              intervalFromReals<I_type>(midYB, b.unsafeB())
            )
          );
          U1_.insert(
            std::pair(
              intervalFromReals<I_type>(midXL, a.unsafeB()),
              intervalFromReals<I_type>(midYB, b.unsafeB())
            )
          );
        }

        ren_.fillRect(regionToRect(a, b));
      }

      printf(" pre_swap:  U_len: %lu, U1_len: %lu\n", U_.size(), U1_.size());
      std::swap(U_, U1_);
      printf("post_swap:  U_len: %lu, U1_len: %lu\n", U_.size(), U1_.size());
    }

    double pixelToRealL(double x) const {
      return addDown(
          L_,
          divDown(
            subDown(mulDown(x, R_), mulUp(x, L_)),
            static_cast<double>(screen_.w))
        );
    }
    double pixelToRealR(double x) const {
      return addUp(
          L_,
          divUp(
            subUp(mulUp(x, R_), mulDown(x, L_)),
            static_cast<double>(screen_.w))
        );
    }

    double pixelToRealB(double y) const {
      return addDown(
          B_,
          divDown(
            subDown(mulDown(y, T_), mulUp(y, B_)),
            static_cast<double>(screen_.h))
        );
    }
    double pixelToRealT(double y) const {
      return addUp(
          B_,
          divUp(
            subUp(mulUp(y, T_), mulDown(y, B_)),
            static_cast<double>(screen_.h))
        );
    }

    I_type pixelToRealH(I_type x) const {
      return intervalFromReals<I_type>(pixelToRealL(x.unsafeA()), pixelToRealR(x.unsafeB()));
    }
    I_type pixelToRealV(I_type x) const {
      return intervalFromReals<I_type>(pixelToRealB(x.unsafeA()), pixelToRealT(x.unsafeB()));
    }

    rolmodl::geom::RectXY regionToRect(I_type h, I_type v) const {
      using grapher::detail::toInt;

      return rolmodl::geom::RectXY{
          screen_.x+toInt(h.unsafeA()), screen_.y+screen_.h-toInt(v.unsafeA()),
          screen_.x+toInt(h.unsafeB()), screen_.y+screen_.h-toInt(v.unsafeB())
        };
    }
  private:
    rolmodl::Ren& ren_;
    graphfx* fx_; graphfy* fy_;
    double L_, B_, R_, T_;
    rolmodl::geom::RectWH screen_;

    U_type U_, U1_;
    int k_;
};
