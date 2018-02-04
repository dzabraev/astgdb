all: dwarf.cc
	g++ -g3 -O0 -Wall -std=c++17 dwarf.cc $(LDFLAGS) -o dwarf

visitor: visitor.cc
	g++ -g3 -O0 -Wall -std=c++17 visitor.cc $(LDFLAGS) -o visitor -lclang

clean :
	rm -f dwarf


