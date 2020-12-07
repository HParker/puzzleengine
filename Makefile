BISON = bison
FLEX = flex
CC = gcc

default:
	${BISON} -d puzzle.y
	${FLEX} puzzle.lex
	${CC} -o ncpuzzlescript lex.yy.c puzzle.tab.c render.c runner.c puzzleData.c main.c `pkg-config --cflags --libs ncurses`
	${CC} -o debugpuzzlescript lex.yy.c puzzle.tab.c debugRender.c runner.c puzzleData.c main.c
	${CC} -g -o parseTest puzzle.tab.c lex.yy.c puzzleData.c parseTest.c `pkg-config --cflags --libs check`
	${CC} -g -o runnerTest puzzle.tab.c lex.yy.c puzzleData.c runner.c runnerTest.c `pkg-config --cflags --libs check`
	${CC} -o sdlpuzzlescript lex.yy.c puzzle.tab.c sdlRender.c runner.c puzzleData.c main.c `pkg-config --cflags --libs sdl2`
	${CC} -o raypuzzlescript lex.yy.c puzzle.tab.c rayRender.c runner.c puzzleData.c main.c `pkg-config --cflags --libs raylib`
	${CC} -o 3draypuzzlescript lex.yy.c puzzle.tab.c 3drayRender.c runner.c puzzleData.c main.c `pkg-config --cflags --libs raylib`

check:
	./parseTest && ./runnerTest

clean:
	rm puzzle.tab.h puzzle.tab.c lex.yy.c puzzlescript parseTest
