default:
	bison -d puzzle.y
	flex puzzle.lex
	gcc -o puzzlescript lex.yy.c runner.c

clean:
	rm puzz.tab.c puzz.tab.h lex.yy.c puzzlescript
