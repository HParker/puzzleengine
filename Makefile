default:
	bison -d puzzle.y
	flex puzzle.lex
	gcc -o ncpuzzlescript lex.yy.c puzzle.tab.c render.c runner.c puzzleData.c main.c `pkg-config --cflags --libs ncurses`
	gcc -o debugpuzzlescript lex.yy.c puzzle.tab.c debugRender.c runner.c puzzleData.c main.c `pkg-config --cflags --libs ncurses`
	gcc -lncurses -g -o parseTest puzzle.tab.c lex.yy.c puzzleData.c debugRender.c parseTest.c `pkg-config --cflags --libs check`
	gcc -lncurses -g -o runnerTest puzzle.tab.c lex.yy.c puzzleData.c debugRender.c runner.c runnerTest.c `pkg-config --cflags --libs check`
	gcc -o sdlpuzzlescript lex.yy.c puzzle.tab.c sdlRender.c runner.c puzzleData.c main.c `pkg-config --cflags --libs sdl2`

check:
	./parseTest && ./runnerTest

clean:
	rm puzzle.tab.h puzzle.tab.c lex.yy.c puzzlescript parseTest
