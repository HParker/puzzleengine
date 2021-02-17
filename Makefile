BISON = bison
FLEX = flex
CC = gcc

default: parser
	mkdir -p bin
	${CC} -g -o ./bin/parseTest puzzle.tab.c puzzleData.c test/parseTest.c `pkg-config --cflags --libs check`
	${CC} -g -o ./bin/runnerTest puzzle.tab.c nullRender.c puzzleData.c runner.c test/runnerTest.c `pkg-config --cflags --libs check`

graphics_tests: parser
	${CC} -g -o ./bin/ncRunnerTest puzzle.tab.c ncRender.c puzzleData.c runner.c test/runnerTest.c `pkg-config --cflags --libs check ncurses`
	${CC} -g -o ./bin/rayRunnerTest puzzle.tab.c rayRender.c puzzleData.c runner.c test/runnerTest.c `pkg-config --cflags --libs check raylib`
	${CC} -g -o ./bin/3drayRunnerTest puzzle.tab.c 3drayRender.c puzzleData.c runner.c test/runnerTest.c `pkg-config --cflags --libs check raylib`
	${CC} -g -o ./bin/sdlRunnerTest puzzle.tab.c sdlRender.c puzzleData.c runner.c test/runnerTest.c `pkg-config --cflags --libs check sdl2`

check_graphics: graphics_tests
	CK_FORK=no ./bin/rayRunnerTest # && CK_FORK=no ./bin/3drayRunnerTest && CK_FORK=no ./bin/sdlRunnerTest

parser:
	${BISON} -d puzzle.y
	${FLEX} puzzle.lex

sdl: parser
	${CC} -o ./bin/sdlpuzzlescript puzzle.tab.c sdlRender.c runner.c puzzleData.c main.c `pkg-config --cflags --libs sdl2`


check: default
	./bin/parseTest && ./bin/runnerTest

nc: parser
	${CC} -o ./bin/ncpuzzlescript puzzle.tab.c ncRender.c runner.c puzzleData.c main.c `pkg-config --cflags --libs ncurses`

ray: parser
	${CC} -O3 -o ./bin/raypuzzlescript puzzle.tab.c rayRender.c runner.c puzzleData.c main.c `pkg-config --cflags --libs raylib`

3dray: parser
	${CC} -o ./bin/3draypuzzlescript puzzle.tab.c 3drayRender.c runner.c puzzleData.c main.c `pkg-config --cflags --libs raylib`

all: default sdl debug nc ray 3dray graphics_tests

clean:
	rm -p puzzle.tab.h puzzle.tab.c
	rm -rf bin/*
