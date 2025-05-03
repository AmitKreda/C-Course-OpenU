assembler: assembler.o pre_assembler.o trie.o ast.o lexer.o error.o first_pass.o second_pass.o output.o
	gcc -g -Wall -pedantic -ansi assembler.o pre_assembler.o trie.o ast.o lexer.o error.o first_pass.o second_pass.o output.o -o assembler

pre_assembler.o: pre_assembler.c trie.h pre_assembler.h
	gcc -c -g -Wall -pedantic -ansi pre_assembler.c -o pre_assembler.o
trie.o: trie.c trie.h
	gcc -c -g -Wall -pedantic -ansi trie.c -o trie.o

assembler.o: assembler.c lexer.h ast.h error.h pre_assembler.h first_pass.h second_pass.h output.h
	gcc -c -g -Wall -pedantic -ansi assembler.c -o assembler.o
ast.o: ast.c ast.h error.h lexer.h
	gcc -c -g -Wall -pedantic -ansi ast.c -o ast.o
lexer.o: lexer.c lexer.h error.h
	gcc -c -g -Wall -pedantic -ansi lexer.c -o lexer.o
error.o: error.c error.h
	gcc -c -g -Wall -pedantic -ansi error.c -o error.o

first_pass.o: first_pass.c first_pass.h symbol.h lexer.h ast.h trie.h
	gcc -c -g -Wall -pedantic -ansi first_pass.c -o first_pass.o
second_pass.o: second_pass.c second_pass.h first_pass.h symbol.h
	gcc -c -g -Wall -pedantic -ansi second_pass.c -o second_pass.o

output.o: output.c output.h
	gcc -c -g -Wall -pedantic -ansi output.c -o output.o
