#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "Rolmodl.hpp"

#include "BoolInterval.hpp"
#include "Interval.hpp"
#include "DTInterval.hpp"
#include "updown.hpp"
#include "Grapher.hpp"

#include "Tracexx.hpp"

using namespace rolmodl;

int main() {
  signal(SIGSEGV, handler);
  signal(SIGABRT, handler);

  geom::Size size = geom::Size{800, 800};
  geom::RectWH screen = geom::RectWH{0, 0, size.w, size.h};

  Win w("OpenGrapheq", size);
  Ren r(w);

  SDL_SetRenderDrawBlendMode(r.unsafeRaw(), SDL_BLENDMODE_BLEND);

  RenTex grid(r, pixelfmt::Id::rgba32, size);
  RenTex graph(r, pixelfmt::Id::rgba32, size);

  SDL_SetTextureBlendMode(grid.unsafeRaw(), SDL_BLENDMODE_BLEND);
  SDL_SetRenderTarget(r.unsafeRaw(), grid.unsafeRaw()); r.setColor(RGBA{0, 0, 0, 0}); r.clear();

  SDL_SetTextureBlendMode(graph.unsafeRaw(), SDL_BLENDMODE_BLEND);
  SDL_SetRenderTarget(r.unsafeRaw(), graph.unsafeRaw()); r.setColor(RGBA{0, 0, 0, 0}); r.clear();

  SDL_SetRenderTarget(r.unsafeRaw(), nullptr);

  Grapher<DTInterval>::graphfx* f_x = [](DTInterval x) {
      return intervalFromReals<DTInterval>(1, 1) / x;
      // if (x.def() != alldef)
      //   return x;

      // Interval i = x.unsafeI();

      // BoolInterval def = BoolInterval(i.a() < 0 == i.b() < 0, true);
      // if (def != alldef)
      //   return DTInterval(allnan, def);

      // double a = divDown(1, i.b());
      // double b = divUp(1, i.a());
      // return DTInterval(Interval(a, b), def);

      // // return x.sqrt();
    };
  Grapher<DTInterval>::graphfy* f_y = [](DTInterval fx, DTInterval y) {
      return y.lt(fx);
    };

  // Grapher<Interval>::graphfx* f_x = [](Interval x) {
  //     // return intervalFromReals<Interval>(1, 1) / x;
  //     double a = 0;
  //     double b = 0;

  //     if (x.a() <= 0 != x.b() < 0) {
  //       a = divDown(1, x.a());
  //       b = divUp(1, x.b());
  //     } else {
  //       a = divDown(1, x.b());
  //       b = divUp(1, x.a());
  //     }

  //     // printf("%f %f <- %f %f\n", a, b, x.a(), x.b());

  //     return Interval(a, b);

  //     // return x.sqrt();
  //   };
  // Grapher<Interval>::graphfy* f_y = [](Interval fx, Interval y) {
  //     return y.lt(fx);
  //   };

  Grapher<DTInterval> g(
      r,
      f_x, f_y,
      -10, -10, 10, 10,
      screen
    );
  using I_type = decltype(g)::I_type;

  bool running = true;
  bool rerender = true;
  bool nextStep = true;
  bool rendergrid = true;
  int renStep = 1;

  bool dragging = false;
  geom::XYInt32 dragStart{};
  geom::XYInt32 dragEnd{};

  while (running) {
    std::optional<Event> e_opt = event::poll();
    while (e_opt) {
      Event e = *e_opt;

      if (std::holds_alternative<event::Quit>(e))
        running = false;
      else if (std::holds_alternative<event::key::Up>(e)) {
        SDL_Keycode sym = std::get<event::key::Up>(e).sym.sym;
        if (sym == SDLK_ESCAPE)
          running = false;
        else if (sym == SDLK_SPACE) {
          rerender = true;
          nextStep = true;
          ++renStep;
        }
        else if (sym == SDLK_RETURN) {
          SDL_SetRenderTarget(r.unsafeRaw(), graph.unsafeRaw());
          g.finish();
          r.setColor(RGBA{0, 0, 0});
          SDL_SetRenderTarget(r.unsafeRaw(), nullptr);

          SDL_SetRenderTarget(r.unsafeRaw(), grid.unsafeRaw());

          g.drawCurGrid();

          r.setColor(RGBA{0, 0, 0});
          SDL_SetRenderTarget(r.unsafeRaw(), nullptr);

          rerender = true;
        }
        else if (sym == SDLK_g) {
          rendergrid = !rendergrid;
          rerender = true;
        }
      }
      else if (std::holds_alternative<event::mouse::button::Down>(e)) {
        rerender = true;
        dragging = true;
        dragStart = dragEnd = std::get<event::mouse::button::Down>(e).pos;
      }
      else if (std::holds_alternative<event::mouse::button::Up>(e)) {
        rerender = true;
        dragging = false;
        dragEnd = std::get<event::mouse::button::Up>(e).pos;

        I_type x = g.pixelToRealH(intervalFromReals<I_type>(dragStart.x, dragEnd.x));
        I_type y = g.pixelToRealV(intervalFromReals<I_type>(screen.h-dragStart.y, screen.h-dragEnd.y));
        I_type fx = f_x(x);
        BoolInterval fy = f_y(fx, y);
        printf("fx<%f %f> = <%f %f> -> fy = <%d %d> for <%f %f>\n", x.unsafeA(), x.unsafeB(), fx.unsafeA(), fx.unsafeB(), fy.a(), fy.b(), y.unsafeA(), y.unsafeB());
      }
      else if (std::holds_alternative<event::mouse::Motion>(e)) {
        if (dragging) {
          rerender = true;
          dragEnd = std::get<event::mouse::Motion>(e).pos;
        }
      }

      e_opt = event::poll();
    }

    if (!rerender) {
      SDL_Delay(1000/60);
      continue;
    }

    if (nextStep) {
      if (renStep == 1) {
        SDL_SetRenderTarget(r.unsafeRaw(), grid.unsafeRaw());

        g.drawCurGrid();

        r.setColor(RGBA{0, 0, 0});
        SDL_SetRenderTarget(r.unsafeRaw(), nullptr);
      }
      else if (renStep == 2) {
        SDL_SetRenderTarget(r.unsafeRaw(), graph.unsafeRaw());

        g.step();
        renStep = 0;

        r.setColor(RGBA{0, 0, 0});
        SDL_SetRenderTarget(r.unsafeRaw(), nullptr);
      }
      nextStep = false;
    }

    r.clear();

    r.drawTex(graph);
    if (rendergrid) r.drawTex(grid);

    if (dragging) {
      r.setColor(RGBA{164, 205, 255, 125});
      r.fillRect(
        geom::RectXY{dragStart.x, dragStart.y, dragEnd.x, dragEnd.y});
      r.setColor(RGBA{100, 0, 0});
    }

    r.present();
    rerender = false;
    SDL_Delay(1000/60);
  }

  return EXIT_SUCCESS;
}
