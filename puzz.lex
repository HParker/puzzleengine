%{
#include <stdio.h>
#include <string.h>
#include "puzz.tab.h"
  int modeToEnter;
%}

%option noyywrap caseless

%s IDENTIFIER
%x MODE
%s OBJECT
%s LEGEND
%s COLLISIONLAYERS
%s RULES
%s WINCONDITIONS
%s LEVELS

direction (action|up|down|left|right|\^|v|\<|\>|moving|stationary|parallel|perpendicular|horizontal|orthogonal|vertical|no|randomdir|random)

color (black|white|lightgray|lightgrey|gray|grey|darkgray|darkgrey|red|darkred|lightred|brown|darkbrown|lightbrown|orange|yellow|green|darkgreen|lightgreen|blue|lightblue|darkblue|purple|pink|transparent)

%%
\(.*\) { printf("Skipping comment\n"); }


title[ ]+ { BEGIN IDENTIFIER; return TITLE; }
author[ ]+ { BEGIN IDENTIFIER; return AUTHOR; }
homepage[ ]+ { BEGIN IDENTIFIER; return HOMEPAGE; }
<IDENTIFIER>[a-zA-Z. ]+$ {
  BEGIN 0;
  yylval.identifier = malloc(sizeof(char) * 100);
  strcpy(yylval.identifier, yytext);
  return ID;
 }

={2,} {
  BEGIN MODE;
  return MODEHEADER;
}

<MODE>[a-zA-Z. ]+$ {
  yylval.identifier = malloc(sizeof(char) * 100);
  strcpy(yylval.identifier, yytext);

  if (strcmp(yytext, "OBJECTS") == 0) {
    modeToEnter = OBJECT;
  } else if (strcmp(yytext, "LEGEND") == 0) {
    modeToEnter = LEGEND;
  } else if (strcmp(yytext, "COLLISIONLAYERS") == 0) {
    modeToEnter = COLLISIONLAYERS;
  } else if (strcmp(yytext, "RULES") == 0) {
    modeToEnter = RULES;
  } else if (strcmp(yytext, "WINCONDITIONS") == 0) {
    modeToEnter = WINCONDITIONS;
  } else if (strcmp(yytext, "LEVELS") == 0) {
    modeToEnter = LEVELS;
  }

  return ID;
 }
<MODE>={2,} {
  BEGIN modeToEnter;
  return MODEHEADER;
}

<OBJECT>{color} {
  yylval.identifier = strdup(yytext);
  return COLOR;
}

<OBJECT>[a-zA-Z]+ {
  yylval.identifier = strdup(yytext);
  return OBJID;
}


<LEGEND>^[a-zA-Z\.#*@] {
  yylval.cell = yytext[0];
  return LEGEND_ID;
}

<LEGEND>= { return EQUALS; }

<LEGEND>[a-zA-Z]+$ {
  yylval.identifier = malloc(sizeof(char) * 100);
  strcpy(yylval.identifier, yytext);
  return LEGEND_VALUE;
}

<COLLISIONLAYERS>[A-Za-z]+ {
  yylval.identifier = malloc(sizeof(char) * 100);
  strcpy(yylval.identifier, yytext);
  return LAYER_NAME;
}

<COLLISIONLAYERS>[\n]+ {
  return END_LAYER;
}

<COLLISIONLAYERS>[, ]+ {
  // Ignore commas and spaces
}

<RULES>\[ {
  return OPEN_SQUARE;
}
<RULES>\] {
  return CLOSE_SQUARE;
}
<RULES>\| {
  return VIRTICAL_PIPE;
}
<RULES>{direction} {
  yylval.identifier = malloc(sizeof(char) * 100);
  strcpy(yylval.identifier, yytext);
  return DIRECTION;
}

<RULES>^late {
  yylval.identifier = strdup(yytext);
  return EXECUTION_TIME;
}

<RULES>-> {
  return ARROW;
}
<RULES>[a-zA-Z.]+ {
  yylval.identifier = malloc(sizeof(char) * 100);
  strcpy(yylval.identifier, yytext);
  return OBJID;
}
<RULES>[\n]+ {
              return END_OF_RULE;
}



<WINCONDITIONS>(all|no|on|some) {
  yylval.identifier = strdup(yytext);
  return LOGIC_WORD;
}

<WINCONDITIONS>[a-zA-Z.]+ {
  yylval.identifier = malloc(sizeof(char) * 100);
  strcpy(yylval.identifier, yytext);
  return OBJID;
}

<LEVELS>[a-zA-Z@*\.#] {
  yylval.cell = yytext[0];
  return LEVEL_CELL;
}

<LEVELS>[\n] {
  return LEVEL_EOL;
}

[ \n]
%%
