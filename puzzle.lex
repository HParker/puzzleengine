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

%option noyywrap caseless yylineno

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

glyph [\+\-\!\?:\"\{\}\[\];\.0-9a-zA-Z\*#,@`\'~\%&\/\|_\>\$]
hexcode #([a-fA-F0-9]{6}|[a-fA-F0-9]{3})

name ([a-zA-Z0-9_#]*[a-zA-Z][a-zA-Z0-9_]*){2,}

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
^realtime_interval[ ]+ { BEGIN FLOAT; return REALTIME_INTERVAL; }
^noaction { return NOACTION; }
^norepeat_action { return NOREPEAT_ACTION; }
^noundo { return NOUNDO; }
^norestart { return NORESTART; }
^run_rules_on_level_start { return RUN_RULES_ON_LEVEL_START; }
^scanline[ ]+ { BEGIN IDENTIFIER; return SCANLINE; }
^text_color[ ]+ { BEGIN IDENTIFIER; return TEXT_COLOR; }
^throttle_movement[ ]+ { BEGIN IDENTIFIER; return THROTTLE_MOVEMENT; }
^zoomscreen[ ]+ { BEGIN IDENTIFIER; return ZOOMSCREEN; }
^enable_level_select {}

^debug { return DEBUG; }
^verbose_logging { return VERBOSE_LOGGING; }

<IDENTIFIER>.*/[\n] {
  if (modeToEnter == -1) {
      BEGIN INITIAL;
  } else {
      BEGIN modeToEnter;
  }
  yylval.identifier = strdup(yytext);
  return ID;
}

<IDENTIFIER>{hexcode} {
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

<OBJECTS>\n {
  return END_OF_OBJECT_LINE;
}

<OBJECTS>{name} {
  yylval.identifier = strdup(yytext);
  return OBJID;
}

<OBJECTS>{hexcode} {
  yylval.identifier = strdup(yytext);
  return OBJID;
}

<OBJECTS>{glyph} {
  yylval.cell = yytext[0];
  return GLYPH;
}

<LEGEND>^[a-zA-Z0-9_\.#\*@]{2,} {
  yylval.identifier = strdup(yytext);
  return LEGEND_ID;
}

<LEGEND>^{glyph}/[ ] {
  yylval.cell = yytext[0];
  return LEGEND_GLYPH;
}

<LEGEND>[ \t]=[ \t] { return EQUALS; }

<LEGEND>[ \t]and[ \t] { return LEGEND_AND; }

<LEGEND>[ \t]or[ \t] { return LEGEND_OR; }

<LEGEND>[\n] { return END_LEGEND_LINE; }

<LEGEND>[a-zA-Z0-9_]+ {
  yylval.identifier = strdup(yytext);
  return LEGEND_VALUE;
}

<SOUNDS>[^=]+

<COLLISIONLAYERS>[A-Za-z0-9]+ {
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

<RULES>moving {
  yylval.enumValue = MOVING;
  return DIRECTION;
}

<RULES>action {
  yylval.enumValue = USE;
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

<RULES>{name} {
  yylval.identifier = strdup(yytext);
  return OBJID;
}

<RULES>\.\.\. {
  yylval.identifier = strdup(yytext);
  return OBJID;
}

<RULES>\n+ {
  return END_OF_RULE;
}

<WINCONDITIONS>(all|no|some|any) {
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

<WINCONDITIONS>{name} {
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

<LEVELS>{glyph} {
  yylval.cell = yytext[0];
  return GLYPH;
}

[ \t\n]

\( {
  commentNestingLevel++;
  BEGIN COMMENT;
}

<COMMENT>[^\)^\(]* {
}
<COMMENT>\) {
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
