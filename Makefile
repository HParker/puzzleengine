default:
	bison -d puzzle.y
	flex puzzle.lex
	gcc -o puzzlescript lex.yy.c puzzle.tab.c render.c runner.c puzzleData.c main.c `pkg-config --cflags --libs ncurses`
	gcc -lncurses -g -o test puzzle.tab.c lex.yy.c puzzleData.c render.c test.c `pkg-config --cflags --libs ncurses check`
	gcc -lncurses -g -o runnerTest puzzle.tab.c lex.yy.c puzzleData.c render.c runner.c runnerTest.c `pkg-config --cflags --libs ncurses check`

check:
	./test && ./runnerTest

clean:
	rm puzz.tab.c puzz.tab.h lex.yy.c puzzlescript test
