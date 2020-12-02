%{
#include <stdio.h>
#include <string.h>
#include "puzzle.tab.h"
#include "puzzleData.h"
int modeToEnter = -1;
int commentNestingLevel = 0;
int inMode = 0;
#define yyterminate() return END_OF_FILE
%}

%option noyywrap caseless

%s MODE
%s IDENTIFIER
%s FLOAT
%s COMMENT
%s OBJECTS
%s LEGEND
%s SOUNDS
%s COLLISIONLAYERS
%s RULES
%s WINCONDITIONS
%s LEVELS

glyph [;\.0-9a-zA-Z\*#,@`'~]

colors (black|white|lightgray|lightgrey|gray|grey|darkgray|darkgrey|red|darkred|lightred|brown|darkbrown|lightbrown|orange|yellow|green|darkgreen|lightgreen|blue|lightblue|darkblue|purple|pink|transparent)

color (color|colour)

%%
^title[ ]+ { BEGIN IDENTIFIER; return TITLE; }
^author[ ]+ { BEGIN IDENTIFIER; return AUTHOR; }
^homepage[ ]+ { BEGIN IDENTIFIER; return HOMEPAGE; }
^require_player_movement { return REQUIRE_PLAYER_MOVEMENT; }
^color_palette[ ]+ { BEGIN IDENTIFIER; return COLOR_PALETTE; }
^again_interval[ ]+ { BEGIN IDENTIFIER; return AGAIN_INTERVAL; }
^background_color[ ]+ { BEGIN IDENTIFIER; return BACKGROUND_COLOR; }
^flickscreen[ ]+ { BEGIN IDENTIFIER; return FLICKSCREEN; }
^key_repeat_interval[ ]+ { BEGIN FLOAT; return KEY_REPEAT_INTERVAL; }
^noaction[ ]+ { BEGIN IDENTIFIER; return NOACTION; }
^norepeat_action[ ]+ { BEGIN IDENTIFIER; return NOREPEAT_ACTION; }
^noundo { BEGIN IDENTIFIER; return NOUNDO; }
^norestart[ ]+ { BEGIN IDENTIFIER; return NORESTART; }
^scanline[ ]+ { BEGIN IDENTIFIER; return SCANLINE; }
^text_color[ ]+ { BEGIN IDENTIFIER; return TEXT_COLOR; }
^throttle_movement[ ]+ { BEGIN IDENTIFIER; return THROTTLE_MOVEMENT; }
^zoomscreen[ ]+ { BEGIN IDENTIFIER; return ZOOMSCREEN; }
^debug { return DEBUG; }
^verbose_logging[ ]+ { BEGIN IDENTIFIER; return VERBOSE_LOGGING; }

<IDENTIFIER>[a-zA-Z0-9\./:\!',\/~ ]+ {
  if (modeToEnter == -1) {
      BEGIN INITIAL;
  } else {
      BEGIN modeToEnter;
  }
  yylval.identifier = strdup(yytext);
  return ID;
}

<FLOAT>[0-9\.]+/[\n] {
  BEGIN INITIAL;
  yylval.decimal = 0.1f;
  return DECIMAL;
}

={2,}[\n]+ {
  if (inMode) {
    inMode = 0;
    BEGIN modeToEnter;
  } else {
    inMode = 1;
    BEGIN MODE;
  }
}

<MODE>^OBJECTS {
  modeToEnter = OBJECTS;
  return MODE_HEADER;
}

<MODE>^LEGEND {
  modeToEnter = LEGEND;
  return MODE_HEADER;
}

<MODE>^SOUNDS {
  modeToEnter = SOUNDS;
  return MODE_HEADER;
}

<MODE>^COLLISIONLAYERS {
  modeToEnter = COLLISIONLAYERS;
  return MODE_HEADER;
}

<MODE>^RULES {
  modeToEnter = RULES;
  return MODE_HEADER;
}

<MODE>^WINCONDITIONS {
  modeToEnter = WINCONDITIONS;
  return MODE_HEADER;
}

<MODE>^LEVELS {
  modeToEnter = LEVELS;
  return MODE_HEADER;
}

<OBJECTS>{colors} {
  yylval.identifier = strdup(yytext);
  return COLOR;
}

<OBJECTS>[a-zA-Z][a-zA-Z_0-9]+ {
  yylval.identifier = strdup(yytext);
  return OBJID;
}

<LEGEND>^[a-zA-Z0-9_\.#\*@]{2,} {
  yylval.identifier = strdup(yytext);
  return LEGEND_ID;
}

<LEGEND>^{glyph} {
  yylval.cell = yytext[0];
  return LEGEND_GLYPH;
}

<LEGEND>[ ]=[ ] { return EQUALS; }

<LEGEND>[ ]and[ ] { return LEGEND_AND; }

<LEGEND>[ ]or[ ] { return LEGEND_OR; }

<LEGEND>[\n] { return END_LEGEND_LINE; }

<LEGEND>[a-zA-Z0-9_]+ {
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
  return VERTICAL_PIPE;
}
<RULES>up {
  yylval.enumValue = UP;
  return DIRECTION;
}

<RULES>down {
  yylval.enumValue = DOWN;
  return DIRECTION;
}

<RULES>left {
  yylval.enumValue = LEFT;
  return DIRECTION;
}

<RULES>right {
  yylval.enumValue = RIGHT;
  return DIRECTION;
}

<RULES>horizontal {
  yylval.enumValue = HORIZONTAL;
  return DIRECTION;
}

<RULES>vertical {
  yylval.enumValue = VERTICAL;
  return DIRECTION;
}

<RULES>stationary {
  yylval.enumValue = STATIONARY;
  return DIRECTION;
}

<RULES>randomDir {
  yylval.enumValue = RANDOMDIR;
  return DIRECTION;
}

<RULES>random {
  yylval.enumValue = RANDOM;
  return DIRECTION;
}


<RULES>\^ {
  yylval.enumValue = REL_UP;
  return DIRECTION;
}

<RULES>v {
  yylval.enumValue = REL_DOWN;
  return DIRECTION;
}

<RULES>\< {
  yylval.enumValue = REL_LEFT;
  return DIRECTION;
}

<RULES>\> {
  yylval.enumValue = REL_RIGHT;
  return DIRECTION;
}

<RULES>no {
  yylval.enumValue = COND_NO;
  return DIRECTION;
}

<RULES>^late {
  yylval.enumValue = LATE;
  return EXECUTION_TIME;
}

<RULES>-> {
  return ARROW;
}
<RULES>[a-zA-Z0-9_\.]+ {
  yylval.identifier = strdup(yytext);
  return OBJID;
}

<RULES>[a-zA-Z0-9_\.]+$ {
  yylval.identifier = strdup(yytext);
  return RULE_POSTFIX;
}

<RULES>\n+ {
  return END_OF_RULE;
}

<WINCONDITIONS>(all|no|some) {
  if (strcasecmp(yytext, "all") == 0) {
    yylval.enumValue = ALL;
  } else if (strcasecmp(yytext, "any") == 0) {
    yylval.enumValue = ANY;
  } else if (strcasecmp(yytext, "no") == 0) {
    yylval.enumValue = NO;
  } else if (strcasecmp(yytext, "some") == 0) {
    yylval.enumValue = SOME;
  } else if (strcasecmp(yytext, "on") == 0) {
    yylval.enumValue = ON;
  }
  return LOGIC_WORD;
}

<WINCONDITIONS>on {
  if (strcasecmp(yytext, "on") == 0) {
    yylval.enumValue = ON;
  }
  return LOGIC_ON;
}



<WINCONDITIONS>[a-zA-Z\.]+ {
  yylval.identifier = strdup(yytext);
  return OBJID;
}

<LEVELS>\n {
  return LEVEL_EOL;
}

message[ ]+ {
  BEGIN IDENTIFIER;
  return MESSAGE;
}

{glyph} {
  yylval.cell = yytext[0];
  return GLYPH;
}

[ \n]

\( {
  commentNestingLevel++;
  BEGIN COMMENT;
}

<COMMENT>[^\)\(]* {
}
<COMMENT>\)[\n]* {
  commentNestingLevel--;
  if (commentNestingLevel == 0) {
    if (modeToEnter == -1) {
      BEGIN INITIAL;
    } else {
      BEGIN modeToEnter;
    }

  }
}
%%
