CXXFLAGS += -std=c++17 -g3 -O0

all : main

main : LazyFile.o ObjectFileManager.o TranslationUnit.o main.o Diagnostic.o
	g++ $(LDFLAGS) LazyFile.o ObjectFileManager.o TranslationUnit.o main.o Diagnostic.o -o main

LazyFile.o : LazyFile.cc LazyFile.h
	g++ $(CXXFLAGS) LazyFile.cc -c -o LazyFile.o

TranslationUnit.o : TranslationUnit.cc TranslationUnit.h
	g++ $(CXXFLAGS) TranslationUnit.cc -c -o TranslationUnit.o

ObjectFileManager.o : ObjectFileManager.cc ObjectFileManager.h
	g++ $(CXXFLAGS) ObjectFileManager.cc -c -o ObjectFileManager.o

Diagnostic.o : Diagnostic.cc Diagnostic.h
	g++ $(CXXFLAGS) Diagnostic.cc -c -o Diagnostic.o

main.o : main.cc
	g++ $(CXXFLAGS) main.cc -c -o main.o


clean:
	rm -f *.o main

