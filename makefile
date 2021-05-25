flags=-std=c++11

all: lexer.o parser.o symstack.o backend.o compiler

lexer.o: lexer.cpp lexer.h
	clang++ $(flags) -c lexer.cpp
	
parser.o: parser.cpp parser.h
	clang++ $(flags) parser.cpp -c
	
symstack.o: symstack.cpp symstack.h
	clang++ $(flags) symstack.cpp -c
	
backend.o: backend.cpp backend.h
	clang++ $(flags) backend.cpp -c

compiler: main.cpp lexer.o parser.o symstack.o backend.o
	clang++ $(flags) main.cpp lexer.o parser.o symstack.o backend.o -o compiler
	
run: compiler lexer.o parser.o symstack.o backend.o
	./compiler
	
clean:
	rm *.o compiler
