# BFWalk-C

This repository contains the C code for the BFWalk scoring algorithm. It does the heavy lifting behind our [python BFWalk](https://github.com/jedrzejkubica/BFWalk) software, or can be used stand-alone.


## Install BFWalk-C

```
git clone https://github.com/jedrzejkubica/BFWalk-C.git
cd BFWalk-C
make
```

This generates an executable `testAdjacency` and a shared library `bfwalk.so`.

To make sure the installation is correct, run `testAdjacency`, which computes and prints BFWalk scores for four example networks.

## Use BFWalk-C

BFWalk-C can be used stand-alone by calling the `bfwalk()` function in the shared library `bfwalk.so`. The function arguments are as follows (for details see `bfwalk.h`):
- pointer to a network structure defining a network (see network.h)
- pointer to a geneScores structure defining seeds (see scores.h)
- alpha attenuation parameter (0 < alpha < 1, typically 0.5 is good)
- pointer to allocated memory for another geneScores structure, the content will be filled with BFWalk scores


## Dependencies

- zlib, openmp (libraries for running, and header files for building -- the headers are often available in separate -devel packages, eg zlib-devel on RHEL9)
- For development you can also install "bear", which allows to build a compilation database for LSP (usable by emacs and other IDEs). Then call `make all` instead of `make`
