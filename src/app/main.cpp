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
  SDL_version compiled;
  SDL_version linked;

  SDL_VERSION(&compiled);
  SDL_GetVersion(&linked);
  printf("We compiled against SDL version %d.%d.%d ...\n",
         compiled.major, compiled.minor, compiled.patch);
  printf("But we are linking against SDL version %d.%d.%d.\n",
         linked.major, linked.minor, linked.patch);

  SDL_Window* win = SDL_CreateWindow("test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
  SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_TARGETTEXTURE);
  SDL_Texture* tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA8888, 400, 300, SDL_TEXTUREACCESS_TARGET);

  SDL_RenderClear(ren);

  SDL_SetRenderTarget(ren, tex);
  SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
  SDL_RenderClear(ren);
  SDL_SetRenderTarget(ren, nullptr);

  bool running = true;
  int i = 0;
  SDL_Event e{};
  while (running) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT)
        running = false;
      else if (i <= 0)
        printf("%d\n", e.type);
    }

    SDL_Delay(1000/60);

    if (i > 0)
      continue;
    ++i;
    SDL_SetRenderTarget(ren, nullptr);
    SDL_SetRenderDrawColor(ren, 0, 0, 100, 255);
    SDL_RenderClear(ren);

    SDL_SetRenderTarget(ren, tex);
    SDL_SetRenderDrawColor(ren, 0, 100, 100, 255);
    SDL_RenderClear(ren);

    SDL_SetRenderDrawColor(ren, 0, 0, 255, 255);
    SDL_RenderDrawLine(ren, 0, 0, 100, 100);

    // SDL_SetRenderTarget(ren, nullptr);
    // SDL_SetRenderDrawColor(ren, 0, 0, 100, 255);
    // SDL_RenderClear(ren);

    SDL_Rect src{0, 0, 400, 300};
    SDL_Rect dist{0, 0, 400, 300};
    SDL_RenderCopy(ren, tex, &src, &dist);

    SDL_RenderPresent(ren);
  }

  SDL_DestroyTexture(tex);
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);

  return 0;

  /*signal(SIGSEGV, handler);
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

        // I_type x = g.pixelToRealH(intervalFromReals<I_type>(dragStart.x, dragEnd.x));
        // I_type y = g.pixelToRealV(intervalFromReals<I_type>(screen.h-dragStart.y, screen.h-dragEnd.y));
        // I_type fx = f_x(x);
        // BoolInterval fy = f_y(fx, y);
        // printf("fx<%f %f> = <%f %f> -> fy = <%d %d> for <%f %f>\n", x.unsafeA(), x.unsafeB(), fx.unsafeA(), fx.unsafeB(), fy.a(), fy.b(), y.unsafeA(), y.unsafeB());
      }
      else if (std::holds_alternative<event::mouse::Motion>(e)) {
        if (dragging) {
          rerender = true;
          dragEnd = std::get<event::mouse::Motion>(e).pos;
        }
      }

      e_opt = event::poll();
    }

    // if (!rerender) {
    //   SDL_Delay(1000/60);
    //   continue;
    // }

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

    // r.drawTex(graph);
    // if (rendergrid) r.drawTex(grid);

    if (dragging) {
      r.setColor(RGBA{164, 205, 255, 125});
      r.fillRect(
        geom::RectXY{dragStart.x, dragStart.y, dragEnd.x, dragEnd.y});
      r.setColor(RGBA{100, 0, 0});
    }

    r.setColor(RGBA{0, 255, 0});
    r.drawLine(geom::Pos{0, 0}, geom::Pos{100, 100});
    r.setColor(RGBA{100, 0, 0});

    r.present();
    rerender = false;
    SDL_Delay(1000/60);
  }

  return EXIT_SUCCESS;*/
}
