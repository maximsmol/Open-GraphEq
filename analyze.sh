#!/bin/bash
/usr/local/opt/llvm/bin/clang-check -p=bld/ -analyze src/tst/main.cpp
/usr/local/opt/llvm/bin/clang-tidy -p=bld/ src/tst/main.cpp
