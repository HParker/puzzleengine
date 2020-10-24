default:
	bison -d puzzle.y
	flex puzzle.lex
	gcc -o puzzlescript lex.yy.c puzzle.tab.c debug.c runner.c
	gcc -g `pkg-config --cflags --libs check` -o test puzzle.tab.c lex.yy.c test.c

clean:
	rm puzz.tab.c puzz.tab.h lex.yy.c puzzlescript test
