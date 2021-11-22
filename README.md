# MiniJulia-lang toy language

Using C++, LLVM, bison and flex.

---

<!-- TOC depthFrom:1 depthTo:3 withLinks:1 updateOnSave:1 orderedList:0 -->

- [Setup Instructions](#setup-instructions)
- [Features](#features)
- [References](#references)
<!-- /TOC -->


## Setup Instructions

1. Set up dependencies such as `g++` (gcc compiler collection, tested on v11) or `clang` (use either one), `llvm` (tested with v13), `bison` and `flex`. Both clang++ and g++ are supported (check `src/Makefile`).

2. Run `make` to compile the compiler or `make examples` directly to compile and run the `src/example.jl` test file (check `example.out.ll` or console).

3. A binary called `minijuliac` is generated. The first two arguments define the input and output file, and the third one ('D') sets if debugging (flex and bison) is enabled or if the code is optimized 'O'. The input file should have an empty line at the end or the parser will complain.

    - Debugging and optimizations off: `./minijuliac inputfile.jl outputfile.ll`
    - Debugging on: `./minijuliac inputfile.jl outputfile.ll D`
    - Optimizations on: `./minijuliac inputfile.jl outputfile.ll O`

4. You can locally run `lli` binary (from LLVM tools) to directly interpret (run) the .ll output or `llc` to compile to assembly.

    Example: `lli < example.out.ll` or `llc < example.out.ll`

    Example (compiling a binary): `llc < example.out.ll | as && gcc -static a.out -o executablefile` and then run it `./executablefile` (not best way to compile, LLVM could be integrated with its JIT comp/interpreter). You can also use clang instead of gcc.

5. Run `make clean` to clean up repository (this won't delete output IR *.ll files).

## Features

- Int64 and Float64, primitive types.
- Const arrays (single dimension) always using the stack and not as globals (inefficient but cool), (cannot access their values yet).
- Arithmetic logic.
- Print function call (can only print integers or doubles).
- Power of doubles, integer division and floating point division (pow using intrinsics, not working for some types).
- Broken single-line comments on the same line as an statement (a = 3 # hi), breaks parser (TODO).
- While and for (with integer range) loops.
- Conditionals (elseif list not supported yet).


Check out the `example.jl` file.

## References

https://en.wikipedia.org/wiki/GNU_Bison#Use

https://stackoverflow.com/a/48879103

https://stackoverflow.com/a/43308337

https://www.gnu.org/software/bison/manual/html_node/Calc_002b_002b-Parser.html

https://gnuu.org/2009/09/18/writing-your-own-toy-compiler/

https://llvm.org/docs/LangRef.html

https://github.com/llvm/llvm-project/tree/main/mlir/examples/toy

https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/index.html

https://groups.seas.harvard.edu/courses/cs153/2019fa/lectures/Lec07-Datastructs.pdf

https://godbolt.org/

