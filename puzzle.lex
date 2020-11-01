%{
#include <stdio.h>
#include <string.h>
#include "puzzle.tab.h"
#include "puzzleData.h"
int modeToEnter;
int commentNestingLevel = 0;
%}

%option noyywrap caseless

%x MODE

%s IDENTIFIER
%s FLOAT
%s COMMENT
%s OBJECT
%s LEGEND
%s SOUNDS
%s COLLISIONLAYERS
%s RULES
%s WINCONDITIONS
%s LEVELS
%s MESSAGE_CONTENTS

gliph [\.0-9a-zA-Z\*#,@`'~]

direction (action|up|down|left|right|\^|v|\<|\>|moving|stationary|parallel|perpendicular|horizontal|orthogonal|vertical|no|randomdir|random)

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
^noundo[ ]+ { BEGIN IDENTIFIER; return NOUNDO; }
^norestart[ ]+ { BEGIN IDENTIFIER; return NORESTART; }
^scanline[ ]+ { BEGIN IDENTIFIER; return SCANLINE; }
^text_color[ ]+ { BEGIN IDENTIFIER; return TEXT_COLOR; }
^throttle_movement[ ]+ { BEGIN IDENTIFIER; return THROTTLE_MOVEMENT; }
^zoomscreen[ ]+ { BEGIN IDENTIFIER; return ZOOMSCREEN; }
^debug { return DEBUG; }
^verbose_logging[ ]+ { BEGIN IDENTIFIER; return VERBOSE_LOGGING; }

<IDENTIFIER>[a-zA-Z0-9\./ ]+/[\n] {
  BEGIN INITIAL;
  yylval.identifier = strdup(yytext);
  return ID;
}

<FLOAT>[0-9\.]+/[\n] {
  BEGIN INITIAL;
  yylval.identifier = strdup(yytext);
  return DECIMAL;
}

={2,}[\n]+ {
  BEGIN MODE;
  return MODEHEADER;
}

<MODE>[a-zA-Z\.]+ {
  yylval.identifier = strdup(yytext);
  if (strcasecmp(yytext, "OBJECTS") == 0) {
    modeToEnter = OBJECT;
  } else if (strcasecmp(yytext, "LEGEND") == 0) {
    modeToEnter = LEGEND;
  } else if (strcasecmp(yytext, "SOUNDS") == 0) {
    modeToEnter = SOUNDS;
  } else if (strcasecmp(yytext, "COLLISIONLAYERS") == 0) {
    modeToEnter = COLLISIONLAYERS;
  } else if (strcasecmp(yytext, "RULES") == 0) {
    modeToEnter = RULES;
  } else if (strcasecmp(yytext, "WINCONDITIONS") == 0) {
    modeToEnter = WINCONDITIONS;
  } else if (strcasecmp(yytext, "LEVELS") == 0) {
    modeToEnter = LEVELS;
  } else {
    printf("Err: '%s' didn't match a mode\n", yytext);
  }

  return ID;
 }
<MODE>[\n]+
<MODE>={2,}[\n]+ {
  BEGIN modeToEnter;
  return MODEHEADER;
}

<OBJECT>{colors} {
  yylval.identifier = strdup(yytext);
  return COLOR;
}

<OBJECT>[a-zA-Z][a-zA-Z_0-9]+ {
  yylval.identifier = strdup(yytext);
  return OBJID;
}


<OBJECT>{gliph} {
  yylval.cell = yytext[0];
  return SPRITE_CELL;
}

<LEGEND>^[a-zA-Z0-9_\.#\*@]+ {
  yylval.identifier = yytext;
  return LEGEND_ID;
}

<LEGEND>[ ]=[ ] { return EQUALS; }

<LEGEND>and { return LEGEND_AND; }

<LEGEND>or { return LEGEND_OR; }

<LEGEND>[\n] { return END_LEGEND_LINE; }

<LEGEND>[a-zA-Z0-9_]+ {
  // TODO: we can legend right here
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

<RULES>virtical {
  yylval.enumValue = VIRTICAL;
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
<RULES>[a-zA-Z0-9_.]+ {
  yylval.identifier = strdup(yytext);
  return OBJID;
}

<RULES>[a-zA-Z0-9_\. ]+$ {
  yylval.identifier = strdup(yytext);
  return RULE_POSTFIX;
}

<RULES>\n+ {
  return END_OF_RULE;
}

<WINCONDITIONS>(all|no|on|some) {
  // the enum is not available here
  // so we use the integer value directly.
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

<WINCONDITIONS>[a-zA-Z.]+ {
  yylval.identifier = strdup(yytext);
  return OBJID;
}

<LEVELS>{gliph} {
  yylval.cell = yytext[0];
  return LEVEL_CELL;
}

<LEVELS>\n {
  return LEVEL_EOL;
}

<LEVELS>message[ ]+ {
  BEGIN MESSAGE_CONTENTS;
  return MESSAGE;
}

<MESSAGE_CONTENTS>[a-zA-Z.0-9:\! ]+/[\n] {
  BEGIN LEVELS;
  yylval.identifier = strdup(yytext);
  return ID;
}

[ \n]

\( {
  commentNestingLevel++;
  BEGIN COMMENT;
}

<COMMENT>[^\)\(]* {
}
<COMMENT>\) {
  commentNestingLevel--;
  if (commentNestingLevel == 0) {
    BEGIN modeToEnter;
  }
}
%%
