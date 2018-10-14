#pragma once

#include <cstdio>
#include <cstdlib>
#include <execinfo.h>
#include <cxxabi.h>

[[noreturn]]
void handler(int sig);
[[noreturn]]
void handler(int sig) {
  void* trace[100];
  int n = backtrace(trace, 100);

  backtrace_symbols_fd(trace, n, fileno(stderr));

  exit(sig);
}
