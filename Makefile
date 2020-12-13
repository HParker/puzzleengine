BISON = bison
FLEX = flex
CC = gcc

default: parser
	mkdir -p bin
	${CC} -g -o ./bin/parseTest puzzle.tab.c puzzleData.c test/parseTest.c `pkg-config --cflags --libs check`
	${CC} -g -o ./bin/runnerTest puzzle.tab.c debugRender.c puzzleData.c runner.c test/runnerTest.c `pkg-config --cflags --libs check`

parser:
	${BISON} -d puzzle.y
	${FLEX} puzzle.lex

sdl: parser
	${CC} -o ./bin/sdlpuzzlescript puzzle.tab.c sdlRender.c runner.c puzzleData.c main.c `pkg-config --cflags --libs sdl2`


check: default
	./bin/parseTest && ./bin/runnerTest

debug: parser
	${CC} -o ./bin/debugpuzzlescript puzzle.tab.c debugRender.c runner.c puzzleData.c main.c

nc: parser
	${CC} -o ./bin/ncpuzzlescript puzzle.tab.c render.c runner.c puzzleData.c main.c `pkg-config --cflags --libs ncurses`

ray: parser
	${CC} -o ./bin/raypuzzlescript puzzle.tab.c rayRender.c runner.c puzzleData.c main.c `pkg-config --cflags --libs raylib`
	${CC} -o ./bin/3draypuzzlescript puzzle.tab.c 3drayRender.c runner.c puzzleData.c main.c `pkg-config --cflags --libs raylib`

all: default sdl debug nc ray

clean:
	rm puzzle.tab.h puzzle.tab.c ./bin/parseTest ./bin/runnerTest ./bin/sdlpuzzlescript ./bin/ncpuzzlescript ./bin/raypuzzlescript ./bin/3draypuzzlescript
