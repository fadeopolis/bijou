
# bijou

A little treasure stolen from a mighty dragons den.

bijou repackages [LLVM's](https://llvm.org/) arbitrary precision arithmetic code
as a standalone library.
It supports signed/unsigned int, fixed point, and floating point types with
arbitrary bit width.

bijou supports arbitrary width types, but has fast paths for "normal" 16, 32,
and 64 bit numbers.
It is not particularly optimized for working with large bit widths.
If you want high-speed arithmetic on large number you're better off with
something like the [GNU Multiple Precision Artihmetic Library](https://gmplib.org/).

bijou packages as little code from LLVM as possible.
Most helper APIs like ArrayRef and StringRef have been replaced with standard
C++20 APIs like std::span and std::string_view.

## Building

bijou uses the CMake build system.

```bash
  # configure & build
  cmake -S . -B build
  cmake --build build

  # list configuration options
  cmake -S . -B build -LH
```

## License

bijou is under the same Apach 2.0 license with LLVM exceptions as LLVM itself.
See the LICENSE.TXT file for details.
