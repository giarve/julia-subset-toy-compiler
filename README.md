# MiniJulia-lang

Using C++, bison and flex.

---

<!-- TOC depthFrom:1 depthTo:6 withLinks:1 updateOnSave:1 orderedList:0 -->

- [Setup Instructions](#setup-instructions)
- [Features](#features)
- [References](#references)
<!-- /TOC -->



## Setup Instructions

1. Set up dependencies such as `g++` (gcc compiler collection, tested on v11), `bison` and `flex`. Clang++ is currently not supported (although it can be quickly adapted) because of C++20 templating features (which are not entirely upstream yet).

2. Run `make` to compile the compiler or `make examples` directly to compile and run the `src/example.jl` test file (check `example.out.txt` or console).

3. A binary called `minijuliac` is generated. The first two arguments define the input and output file, and the third one (no matter which value) sets if debugging (flex and bison) is enabled. The input file should have an empty line at the end or the compiler will complain.

    - Debugging off: `./minijuliac inputfile.jl outputfile.txt`
    - Debugging on: `./minijuliac inputfile.jl outputfile.txt SURPRISE`

5. Run `make clean` to clean up repository (this won't delete output .txt files).

## Features

- Int32, Float64, String and Bool primitive types.
- Vectors, arrays/matrices (2 dimensions) and tuples.
- transpose(), ones(), zeros(), size(), div(), length() builtins.
- Arithmetic and boolean logic without short-circuit.
- Single and multiline comments.
- Structural equivalence for arrays, vectors and scalars of the same size.
- π (pi) constant
- Trigonometric functions (sin, cos, tan, asin, acos, atan)

Check out `example.jl` file.


## References

https://en.wikipedia.org/wiki/GNU_Bison#Use

https://stackoverflow.com/a/48879103

https://stackoverflow.com/a/43308337

https://www.gnu.org/software/bison/manual/html_node/Calc_002b_002b-Parser.html