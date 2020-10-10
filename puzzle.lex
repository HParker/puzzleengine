%{
#include <stdio.h>
#include <string.h>
#include "puzzle.tab.h"
  int modeToEnter;
%}

%option noyywrap caseless

%s IDENTIFIER
%x MODE
%x COMMENT
%s OBJECT
%s LEGEND
%s SOUNDS
%s COLLISIONLAYERS
%s RULES
%s WINCONDITIONS
%s LEVELS
%s MESSAGE_CONTENTS

direction (action|up|down|left|right|\^|v|\<|\>|moving|stationary|parallel|perpendicular|horizontal|orthogonal|vertical|no|randomdir|random)

color (black|white|lightgray|lightgrey|gray|grey|darkgray|darkgrey|red|darkred|lightred|brown|darkbrown|lightbrown|orange|yellow|green|darkgreen|lightgreen|blue|lightblue|darkblue|purple|pink|transparent)

%%
title[ ]+ { BEGIN IDENTIFIER; return TITLE; }
author[ ]+ { BEGIN IDENTIFIER; return AUTHOR; }
homepage[ ]+ { BEGIN IDENTIFIER; return HOMEPAGE; }
<IDENTIFIER>[a-zA-Z\./ ]+$ {
  BEGIN INITIAL;
  yylval.identifier = strdup(yytext);
  return ID;
}

={2,}$ {
  BEGIN MODE;
  return MODEHEADER;
}

<MODE>[a-zA-Z\. ]+$ {
  yylval.identifier = malloc(sizeof(char) * 100);
  strcpy(yylval.identifier, yytext);

  if (strcmp(yytext, "OBJECTS") == 0) {
    modeToEnter = OBJECT;
  } else if (strcmp(yytext, "LEGEND") == 0) {
    modeToEnter = LEGEND;
  } else if (strcmp(yytext, "SOUNDS") == 0) {
    modeToEnter = SOUNDS;
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
<MODE>={2,}[\n]+ {
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

<OBJECT>[0-9.] {
  yylval.cell = yytext[0];
  return SPRITE_CELL;
}


<LEGEND>^[a-zA-Z\.#*@] {
                        printf("KEY SENT %c\n", yytext[0]);
  yylval.cell = yytext[0];
  return LEGEND_ID;
}

<LEGEND>= { printf("EQ SENT =\n"); return EQUALS; }

<LEGEND>and // ignore and

<LEGEND>[\n] { return END_LEGEND_LINE; }

<LEGEND>[a-zA-Z]+ {
                   printf("VAL SENT %s\n", yytext);
  yylval.identifier = strdup(yytext);
  return LEGEND_VALUE;
}



<SOUNDS>[^=]+

<COLLISIONLAYERS>[A-Za-z]+ {
  yylval.identifier = strdup(yytext);
  return LAYER_NAME;
}

<COLLISIONLAYERS>[\n]+ {
  return END_LAYER;
}

<COLLISIONLAYERS>[, ]+

<RULES>\[ {
  return OPEN_SQUARE;
}
<RULES>\] {
  return CLOSE_SQUARE;
}
<RULES>\| {
  return VIRTICAL_PIPE;
}
<RULES>up {
  yylval.enumValue = 0;
  return DIRECTION;
}

<RULES>down {
  yylval.enumValue = 1;
  return DIRECTION;
}

<RULES>left {
  yylval.enumValue = 2;
  return DIRECTION;
}

<RULES>right {
  yylval.enumValue = 3;
  return DIRECTION;
}

<RULES>horizontal {
  yylval.enumValue = 4;
  return DIRECTION;
}

<RULES>virtical {
  yylval.enumValue = 5;
  return DIRECTION;
}

<RULES>\^ {
  yylval.enumValue = 6;
  return DIRECTION;
}

<RULES>v {
  yylval.enumValue = 7;
  return DIRECTION;
}

<RULES>\< {
  yylval.enumValue = 8;
  return DIRECTION;
}

<RULES>\> {
  yylval.enumValue = 9;
  return DIRECTION;
}

<RULES>^late {
  yylval.enumValue = 1;
  return EXECUTION_TIME;
}

<RULES>-> {
  return ARROW;
}
<RULES>[a-zA-Z0-9.]+ {
  yylval.identifier = strdup(yytext);
  return OBJID;
}

<WINCONDITIONS>(all|no|on|some) {
  // the enum is not available here
  // so we use the integer value directly.
  if (strcasecmp(yytext, "all")) {
    yylval.enumValue = 0;
  } else if (strcasecmp(yytext, "any")) {
    yylval.enumValue = 1;
  } else if (strcasecmp(yytext, "no")) {
    yylval.enumValue = 2;
  } else if (strcasecmp(yytext, "some")) {
    yylval.enumValue = 3;
  } else if (strcasecmp(yytext, "on")) {
    yylval.enumValue = 4;
  }
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

<LEVELS>message[ ]+ {
  BEGIN MESSAGE_CONTENTS;
  return MESSAGE;
}

<MESSAGE_CONTENTS>[a-zA-Z.0-9\! ]+ {
  BEGIN LEVELS;
  yylval.identifier = strdup(yytext);
  return ID;
}

[ \n]
\( {
  BEGIN COMMENT;
}

<COMMENT>[^\)]* {
 }
<COMMENT>\) {
  BEGIN INITIAL;
}
%%
