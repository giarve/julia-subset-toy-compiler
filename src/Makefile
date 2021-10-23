CODE_DIR = src

BISON = bison
CXX = g++
CXX_FLAGS = -Wall -g -lm -std=c++20
FLEX = flex
BASE = minijuliac

all: $(BASE)

%.cc %.hh %.html %.gv: %.yy
	$(BISON) $(BISONFLAGS) --html -o $*.cc $<

%.cc: %.ll
	$(FLEX) $(FLEXFLAGS) -o$@ $<

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c -o$@ $<

$(BASE): $(BASE).o driver.o parser.o scanner.o variant.o
	$(CXX) $(CXX_FLAGS) -o $@ $^

$(BASE).o: parser.hh
parser.o: parser.hh
variant.o: parser.hh
scanner.o: parser.hh

examples: $(BASE)
	./$(BASE) example_minimal.jl example.out.txt
	#./$(BASE) example.jl example.out.txt

run: $(BASE)
	@echo "Type an expression. Quit with ctrl-d."
	./$< -

CLEANFILES =										\
  $(BASE) *.o										\
  parser.hh parser.cc parser.output parser.xml parser.html location.hh	\
  scanner.cc

clean:
	rm -f $(CLEANFILES)