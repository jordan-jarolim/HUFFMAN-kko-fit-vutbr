make:
	g++ -g -O3 -std=c++11 -o ahed tree.cpp tree.hpp code.cpp code.hpp ahed.cpp ahed.hpp main.cpp
clean:
	rm ahed
