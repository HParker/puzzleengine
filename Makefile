default:
	bison -d puzzle.y
	flex puzzle.lex
	gcc -o puzzlescript lex.yy.c puzzle.tab.c runner.c puzzleData.c main.c
	gcc -g `pkg-config --cflags --libs check` -o test puzzle.tab.c lex.yy.c puzzleData.c test.c
	gcc -g `pkg-config --cflags --libs check` -o runnerTest puzzle.tab.c lex.yy.c puzzleData.c runner.c runnerTest.c

check:
	./test && ./runnerTest

clean:
	rm puzz.tab.c puzz.tab.h lex.yy.c puzzlescript test
