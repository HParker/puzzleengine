default:
	bison -d puzz.y
	flex puzz.lex
	gcc -c puzz.tab.c lex.yy.c
	gcc -o mypzl lex.yy.c runner.c

clean:
	rm *.o puzz.tab.c puzz.tab.h lex.yy.c lex.yy.o
