default:
	bison -d puzzle.y
	flex puzzle.lex
	gcc -lcurses -o puzzlescript lex.yy.c puzzle.tab.c render.c runner.c puzzleData.c main.c
	gcc -lcurses -g `pkg-config --cflags --libs check` -o test puzzle.tab.c lex.yy.c puzzleData.c render.c test.c
	gcc -lcurses -g `pkg-config --cflags --libs check` -o runnerTest puzzle.tab.c lex.yy.c puzzleData.c render.c runner.c runnerTest.c

check:
	./test && ./runnerTest

clean:
	rm puzz.tab.c puzz.tab.h lex.yy.c puzzlescript test
