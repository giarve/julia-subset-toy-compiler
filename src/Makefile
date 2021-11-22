CODE_DIR = src

BISON = bison -Wcounterexamples
FLEX = flex

# I removed -fno-exceptions because I use them in bison. and -std=c++14
# Got this from `llvm-config --cxxflags --ldflags --system-libs --libs core`
LLVM_FLAGS = -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -L/usr/lib -lLLVM-13

# replace clang++ with g++ if you want
CXX = clang++ -std=c++20 -Wall -g

BASE = minijuliac

all: $(BASE)

%.cc %.hh %.html %.gv: %.yy
	$(BISON) $(BISONFLAGS) --html -o $*.cc $<

%.cc: %.ll
	$(FLEX) $(FLEXFLAGS) -o$@ $<

%.o: %.cc
	$(CXX) -c -o$@ $<

$(BASE): $(BASE).o driver.o parser.o scanner.o codegen.o
	$(CXX) $(LLVM_FLAGS) -o ../$@ $^

$(BASE).o: parser.hh
parser.o: parser.hh
scanner.o: parser.hh
codegen.o: parser.hh

examples: $(BASE)
	../$(BASE) ../example.jl ../example.out.ll

CLEANFILES =										\
  ../$(BASE) *.o										\
  parser.hh parser.cc parser.output parser.xml parser.html location.hh	\
  scanner.cc

clean:
	rm -f $(CLEANFILES)