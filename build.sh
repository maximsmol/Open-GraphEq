#!/bin/bash
export PATH="/usr/local/opt/llvm/bin:$PATH"
stack runhaskell -- shake.hs $@
