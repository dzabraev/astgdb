all: dwarf.cc
	g++ -g3 -O0 -Wall -std=c++17 dwarf.cc $(LDFLAGS) -o dwarf

clean :
	rm -f dwarf


