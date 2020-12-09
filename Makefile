BISON = bison
FLEX = flex
CC = gcc

default: parser
	${CC} -g -o parseTest puzzle.tab.c lex.yy.c puzzleData.c parseTest.c `pkg-config --cflags --libs check`
	${CC} -g -o runnerTest puzzle.tab.c lex.yy.c puzzleData.c runner.c runnerTest.c `pkg-config --cflags --libs check`

parser:
	${BISON} -d puzzle.y
	${FLEX} puzzle.lex

sdl: parser
	${CC} -o sdlpuzzlescript lex.yy.c puzzle.tab.c sdlRender.c runner.c puzzleData.c main.c `pkg-config --cflags --libs sdl2`


check: default
	./parseTest && ./runnerTest


debug: parser
	${CC} -o debugpuzzlescript lex.yy.c puzzle.tab.c debugRender.c runner.c puzzleData.c main.c

nc: parser
	${CC} -o ncpuzzlescript lex.yy.c puzzle.tab.c render.c runner.c puzzleData.c main.c `pkg-config --cflags --libs ncurses`

ray: parser
	${CC} -o raypuzzlescript lex.yy.c puzzle.tab.c rayRender.c runner.c puzzleData.c main.c `pkg-config --cflags --libs raylib`
	${CC} -o 3draypuzzlescript lex.yy.c puzzle.tab.c 3drayRender.c runner.c puzzleData.c main.c `pkg-config --cflags --libs raylib`

all: default sdl debug nc ray

clean:
	rm puzzle.tab.h puzzle.tab.c lex.yy.c parseTest runnerTest sdlpuzzlescript ncpuzzlescript raypuzzlescript 3draypuzzlescript
