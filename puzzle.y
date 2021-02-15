%{
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "puzzleData.h"
#include "lex.yy.c"

PuzzleData pd;
int yyerror(const char *str) {
    fprintf(stderr, "line: %d | %s\n", yylineno, str);
    exit(1);
}

int aliasLegendId(char * name) {
    for (int i = 0; i < pd.aliasLegendCount; i++) {
        if (strcasecmp(pd.aliasLegend[i].key, name) == 0) {
            return i;
        }
    }
    fprintf(stderr, "err: '%s' which does not exist in alias legend\n", name);
    for (int i = 0; i < pd.aliasLegendCount; i++) {
      fprintf(stderr, "wasn't key: '%s'\n", pd.aliasLegend[i].key);
    }
    return -1;
}

int glyphLegendId(char glyph) {
    for (int i = 0; i < pd.glyphLegendCount; i++) {
        if (toupper(pd.glyphLegend[i].key) == toupper(glyph)) {
            return i;
        }
    }
    fprintf(stderr, "err: '%c' which does not exist in glyph legend\n", glyph);
    return -1;
}


void addObjectsToGlyphLegend(char * name) {
    int legId = aliasLegendId(name);
    int count = pd.aliasLegend[legId].objectCount;
    for (int i = 0; i < count; i++) {
        pd.glyphLegend[pd.glyphLegendCount].objects[pd.glyphLegend[pd.glyphLegendCount].objectCount] = pd.aliasLegend[legId].objects[i];
        incGlyphLegendObject(pd.glyphLegendCount);
    }
}

int objectId(char * name) {
    for (int i = 0; i < pd.objectCount; i++) {
        if (strcasecmp(pd.objects[i].name, name) == 0) {
            return i;
        }
    }
    fprintf(stderr, "objectId not found for '%s'\n", name);
    return -1;
}

int spriteIndex = 0;
int legendIsAlias = 0;
int rowWidth = 0;
%}

%union {
  char * identifier;
  char tile;
  float decimal;
  int enumValue;
  int number;
};

%define parse.error verbose

%token TITLE AUTHOR HOMEPAGE COLOR_PALETTE AGAIN_INTERVAL BACKGROUND_COLOR
%token FLICKSCREEN KEY_REPEAT_INTERVAL NOACTION NOREPEAT_ACTION NOUNDO NORESTART
%token REALTIME_INTERVAL REQUIRE_PLAYER_MOVEMENT
%token SCANLINE TEXT_COLOR THROTTLE_MOVEMENT ZOOMSCREEN
%token DEBUG VERBOSE_LOGGING RUN_RULES_ON_LEVEL_START
%token END_OF_FILE 0 "end of file"
%token MODE_HEADER EQUALS END_LAYER END_OF_RULE MESSAGE LEGEND_AND LEGEND_OR END_OF_OBJECT_LINE
%token <identifier> ID OBJID COLOR LEGEND_ID LEGEND_VALUE END_LEGEND_LINE LAYER_NAME RULE_POSTFIX
%token  <decimal> DECIMAL

%token  <number> DIGIT
%token  DIMENSION_SEP

%token MOVE_RIGHT MOVE_UP MOVE_LEFT MOVE_DOWN
%token OPEN_SQUARE CLOSE_SQUARE VERTICAL_PIPE ARROW
%token LEVEL_EOL LEVEL_EMPTY_LINE
%token <tile> GLYPH LEGEND_GLYPH
%token <enumValue> LOGIC_WORD LOGIC_ON DIRECTION EXECUTION_TIME

%%
puzzlescript: preamble
              modechange
              object_definitions
              modechange
              legend_lines
              modechange
              sounds
              modechange
              collision_lines
              modechange
              rules
              modechange
              winconditions
              modechange
              levels;

preamble: preamble preamble_option | %empty;

preamble_option: title
               | author
               | homepage
               | color_palette
               | again_interval
               | background_color
               | flickscreen
               | key_repeat_interval
               | realtime_interval
               | noaction
               | norepeat_action
               | noundo
               | norestart
               | scanline
               | text_color
               | throttle_movement
               | zoomscreen
               | debug
               | verbose_logging
               | run_rules_on_level_start
               | require_player_movement
                ;

title: TITLE ID { pd.title = $2; }

author: AUTHOR ID { pd.author = $2; }

homepage: HOMEPAGE ID { pd.homepage = $2; }

require_player_movement: REQUIRE_PLAYER_MOVEMENT { pd.requirePlayerMovement = 1; }
        ;

color_palette: COLOR_PALETTE ID { pd.colorPalette = colorPaletteId($2); }
        ;

again_interval: AGAIN_INTERVAL ID { pd.againInterval = 0.1f; }
        ;

background_color: BACKGROUND_COLOR ID { pd.backgroundColor = $2; }
        ;

key_repeat_interval: KEY_REPEAT_INTERVAL DECIMAL { pd.keyRepeatInterval = 0.1f; }
        ;

realtime_interval: REALTIME_INTERVAL DECIMAL { pd.realTimeInterval = $2; }
        ;

noaction: NOACTION { pd.noAction = 1; }
        ;

norepeat_action: NOREPEAT_ACTION { pd.noRepeatAction = 1; }
        ;

noundo: NOUNDO { pd.noUndo = 1; }
        ;

norestart: NORESTART { pd.noRestart = 1; }
        ;

scanline: SCANLINE { pd.scanLine = 1; }
        ;

text_color: TEXT_COLOR ID { pd.textColor = $2; }
        ;

throttle_movement: THROTTLE_MOVEMENT ID { pd.throttleMovement = 1; }
        ;

zoomscreen: ZOOMSCREEN DIGIT DIGIT {
  pd.doesZoomScreen = 1;
  pd.zoomScreenX = $2;
  pd.zoomScreenY = $3;
}
        ;

flickscreen: FLICKSCREEN DIGIT DIGIT {
    pd.doesFlickScreen = 1;
    pd.flickScreenX = $2;
    pd.flickScreenY = $2;
 };


debug: DEBUG { pd.debug = 1; }
        ;

verbose_logging: VERBOSE_LOGGING { pd.verboseLogging = 1; }
        ;

run_rules_on_level_start: RUN_RULES_ON_LEVEL_START { pd.runRulesOnLevelStart = 1; }

modechange: MODE_HEADER

some_object_eol: some_object_eol END_OF_OBJECT_LINE | END_OF_OBJECT_LINE;
any_object_eol: some_object_eol | %empty;

object_definitions: object_definitions object_definition | object_definition;

object_definition: any_object_eol object_name some_object_eol colors some_object_eol sprite some_object_eol {

    incObject();
}
        |       any_object_eol object_name some_object_eol colors some_object_eol {
  for (int j = 0; j < 25; j++) {
    pd.objects[pd.objectCount].sprite[j] = '0';
  }
  incObject();
}

object_name: OBJID OBJID {
    pd.objects[pd.objectCount].name = $1;

    pd.aliasLegend[pd.aliasLegendCount].key = strdup($1);
    pd.aliasLegend[pd.aliasLegendCount].objects[0] = pd.objectCount;

    incAliasLegendObject(pd.aliasLegendCount);
    incAliasLegend();

    // single char key
    // They only reference AliasLegend names that have the actual object ID
    pd.glyphLegend[pd.glyphLegendCount].key = $2[0];
    pd.glyphLegend[pd.glyphLegendCount].objects[0] = pd.objectCount;

    incGlyphLegendObject(pd.glyphLegendCount);
    incGlyphLegend();
}
           | OBJID {
    pd.objects[pd.objectCount].name = $1;

    pd.aliasLegend[pd.aliasLegendCount].key = strdup($1);
    pd.aliasLegend[pd.aliasLegendCount].objects[0] = pd.objectCount;
    incAliasLegendObject(pd.aliasLegendCount);
    incAliasLegend();
}

colors: colors color | color;

color: OBJID {
  pd.objects[pd.objectCount].colors[pd.objects[pd.objectCount].colorCount] = $1;
  pd.objects[pd.objectCount].colorCount++;
}

sprite: sprite_row some_object_eol sprite_row some_object_eol sprite_row some_object_eol sprite_row some_object_eol sprite_row {
  spriteIndex = 0;
}

sprite_row: sprite_tile sprite_tile sprite_tile sprite_tile sprite_tile

sprite_tile: GLYPH {
    pd.objects[pd.objectCount].sprite[spriteIndex] = $1;
    spriteIndex++;
}

legend_lines: some_legend_lines | %empty;

some_legend_lines: some_legend_lines legend_line | legend_line;

legend_line: legend_id EQUALS legend_values end_legend_line {
  incAliasLegend();
  pd.aliasLegend[pd.aliasLegendCount].objectRelation = LEGEND_RELATION_UNKNOWN;
  legendIsAlias = 0;
}
           | legend_glyph EQUALS legend_values end_legend_line {
  incGlyphLegend();
  pd.glyphLegend[pd.glyphLegendCount].objectRelation = LEGEND_RELATION_UNKNOWN;
  legendIsAlias = 0;
}

legend_id: LEGEND_ID {
  legendIsAlias = 1;
  pd.aliasLegend[pd.aliasLegendCount].key = $1;
}

legend_glyph: LEGEND_GLYPH {
  legendIsAlias = 0;
  pd.glyphLegend[pd.glyphLegendCount].key = $1;
}

end_legend_line: END_LEGEND_LINE end_legend_line | END_LEGEND_LINE;

legend_values: legend_value legend_joiner legend_values
             | legend_value

legend_value: LEGEND_VALUE {
  if (legendIsAlias == 1) {
    addObjectsToAliasLegend($1);
  } else {
    addObjectsToGlyphLegend($1);
  }
  free($1);
}

legend_joiner: LEGEND_AND {
  if (legendIsAlias == 1) {
    pd.aliasLegend[pd.aliasLegendCount].objectRelation = LEGEND_RELATION_AND;
  } else {
    pd.glyphLegend[pd.glyphLegendCount].objectRelation = LEGEND_RELATION_AND;
  }
}
             | LEGEND_OR  {
  if (legendIsAlias == 1) {
    pd.aliasLegend[pd.aliasLegendCount].objectRelation = LEGEND_RELATION_OR;
  } else {
    pd.glyphLegend[pd.glyphLegendCount].objectRelation = LEGEND_RELATION_OR;
  }
}

sounds: %empty; // Nothing for now

collision_lines: collision_line collision_lines
               | collision_line

collision_line: layer_objects END_LAYER { incLayer(); }

layer_objects: layer_object layer_objects
             | layer_object

layer_object: LAYER_NAME {
  addObjectsToLayer($1);
  free($1);
}

rules: rules rule_line
     | rule_line
     ;

rule_line: any_eor rule some_eor {
    pd.rules[pd.ruleCount].lineNo = yylineno;
    incRule();
}

some_eor: some_eor END_OF_RULE | END_OF_RULE;
any_eor: some_eor | %empty;

rule:           rule_prefix match_states arrow result_states rule_postfixs
        |       rule_prefix match_states arrow result_states
        |       match_states arrow result_states rule_postfixs {
    pd.rules[pd.ruleCount].executionTime = NORMAL;
    pd.rules[pd.ruleCount].directionConstraint = NONE;
}
        |       match_states arrow result_states {
    pd.rules[pd.ruleCount].executionTime = NORMAL;
    pd.rules[pd.ruleCount].directionConstraint = NONE;
}
        |       match_states arrow rule_postfixs
                {
    pd.rules[pd.ruleCount].executionTime = NORMAL;
    pd.rules[pd.ruleCount].directionConstraint = NONE;
}
        |       rule_prefix match_states arrow rule_postfixs

arrow: ARROW

rule_prefix:    EXECUTION_TIME {
  pd.rules[pd.ruleCount].executionTime = $1;
  pd.rules[pd.ruleCount].directionConstraint = NONE;
}
        |       DIRECTION {
  pd.rules[pd.ruleCount].executionTime = NORMAL;
  pd.rules[pd.ruleCount].directionConstraint = $1;
}
        |       EXECUTION_TIME DIRECTION {
  pd.rules[pd.ruleCount].executionTime = $1;
  pd.rules[pd.ruleCount].directionConstraint = $2;
}

rule_postfixs: rule_postfixs rule_postfix | rule_postfix

rule_postfix: OBJID {
    if (strcasecmp("sfx0", $1) == 0) {
        pd.rules[pd.ruleCount].commands[pd.rules[pd.ruleCount].commandCount] = SFX0;
    } else if (strcasecmp("sfx1", $1) == 0) {
        pd.rules[pd.ruleCount].commands[pd.rules[pd.ruleCount].commandCount] = SFX1;
    } else if (strcasecmp("sfx2", $1) == 0) {
        pd.rules[pd.ruleCount].commands[pd.rules[pd.ruleCount].commandCount] = SFX2;
    } else if (strcasecmp("sfx3", $1) == 0) {
        pd.rules[pd.ruleCount].commands[pd.rules[pd.ruleCount].commandCount] = SFX3;
    } else if (strcasecmp("sfx4", $1) == 0) {
        pd.rules[pd.ruleCount].commands[pd.rules[pd.ruleCount].commandCount] = SFX4;
    } else if (strcasecmp("sfx5", $1) == 0) {
        pd.rules[pd.ruleCount].commands[pd.rules[pd.ruleCount].commandCount] = SFX5;
    } else if (strcasecmp("sfx6", $1) == 0) {
        pd.rules[pd.ruleCount].commands[pd.rules[pd.ruleCount].commandCount] = SFX6;
    } else if (strcasecmp("sfx7", $1) == 0) {
        pd.rules[pd.ruleCount].commands[pd.rules[pd.ruleCount].commandCount] = SFX7;
    } else if (strcasecmp("sfx8", $1) == 0) {
        pd.rules[pd.ruleCount].commands[pd.rules[pd.ruleCount].commandCount] = SFX8;
    } else if (strcasecmp("sfx9", $1) == 0) {
        pd.rules[pd.ruleCount].commands[pd.rules[pd.ruleCount].commandCount] = SFX9;
    } else if (strcasecmp("again", $1) == 0) {
        pd.rules[pd.ruleCount].commands[pd.rules[pd.ruleCount].commandCount] = AGAIN;
    } else if (strcasecmp("cancel", $1) == 0) {
        pd.rules[pd.ruleCount].commands[pd.rules[pd.ruleCount].commandCount] = CANCEL;
    } else if (strcasecmp("checkpoint", $1) == 0) {
        pd.rules[pd.ruleCount].commands[pd.rules[pd.ruleCount].commandCount] = CHECKPOINT;
    } else if (strcasecmp("restart", $1) == 0) {
        pd.rules[pd.ruleCount].commands[pd.rules[pd.ruleCount].commandCount] = CMD_RESTART;
    } else if (strcasecmp("win", $1) == 0) {
        pd.rules[pd.ruleCount].commands[pd.rules[pd.ruleCount].commandCount] = WIN;
    }
    pd.rules[pd.ruleCount].commandCount++;
    free($1);
}
            | MESSAGE ID {
  fprintf(stderr, "this should show a message '%s'\n", $2);
}

match_states:   match_states match_state | match_state;

match_state:    OPEN_SQUARE match_state_internals CLOSE_SQUARE {
    incRuleMatchState(&pd.rules[pd.ruleCount]);
}

match_state_internals: match_state_internals VERTICAL_PIPE match_state_part
                |      match_state_part


match_state_part: match_on_square {
  Rule * r = &pd.rules[pd.ruleCount];
  Pattern * rs = &r->matchPatterns[r->matchPatternCount];
  incRulePart(rs);
  if (rs->partCount > 1) {
      r->hasMultipleParts = 1;
  }

}
                | %empty {
  Rule * r = &pd.rules[pd.ruleCount];
  Pattern * rs = &r->matchPatterns[r->matchPatternCount];
  RulePart * rsp = &rs->parts[rs->partCount];

  rsp->identity[rsp->identityCount].direction = UNSPECIFIED;
  rsp->identity[rsp->identityCount].legendId = aliasLegendId("_EMPTY_");

  incRuleIdent(rsp);
  incRulePart(rs);
}


match_on_square: match_on_square match_object_part | match_object_part;

match_object_part: OBJID {
    Rule * r = &pd.rules[pd.ruleCount];
    Pattern * rs = &r->matchPatterns[r->matchPatternCount];
    RulePart * rsp = &rs->parts[rs->partCount];
    RuleIdentity * rid = &rsp->identity[rsp->identityCount];

    if (strcasecmp("...", $1) == 0 ) {
      r->hasSpread = 1;
      rsp->isSpread = 1;
    }


    rid->direction = UNSPECIFIED;
    rid->legendId = aliasLegendId($1);

    free($1);
    incRuleIdent(rsp);
}
                 | DIRECTION OBJID {
    Rule * r = &pd.rules[pd.ruleCount];
    Pattern * rs = &r->matchPatterns[r->matchPatternCount];
    RulePart * rsp = &rs->parts[rs->partCount];
    RuleIdentity * rid = &rsp->identity[rsp->identityCount];



    if ($1 == PARALLEL ||
        $1 == PERPENDICULAR) {
        r->hasCompoundDirection = 1;
        r->hasRelativeDirection = 1;
    } else if ($1 == MOVING ||
        $1 == REL_UP ||
        $1 == REL_DOWN ||
        $1 == REL_LEFT ||
        $1 == REL_RIGHT ||
        $1 == HORIZONTAL ||
        $1 == VERTICAL
                ) {
        r->hasRelativeDirection = 1;
    }
    rid->direction = $1;
    rid->legendId = aliasLegendId($2);
    free($2);
    incRuleIdent(rsp);
}

result_states:   result_states result_state | result_state;

result_state:    OPEN_SQUARE result_state_internals CLOSE_SQUARE { incRuleResultState(&pd.rules[pd.ruleCount]); }

result_state_internals: result_state_internals VERTICAL_PIPE result_state_part
                |      result_state_part


result_state_part: result_on_square {
  Rule * r = &pd.rules[pd.ruleCount];
  Pattern * rs = &r->resultPatterns[r->resultPatternCount];

  incRulePart(rs);
}
        |       %empty {
  Rule * r = &pd.rules[pd.ruleCount];
  Pattern * rs = &r->resultPatterns[r->resultPatternCount];
  RulePart * rsp = &rs->parts[rs->partCount];

  rsp->identity[rsp->identityCount].direction = UNSPECIFIED;
  rsp->identity[rsp->identityCount].legendId = aliasLegendId("_EMPTY_");

  incRuleIdent(rsp);
  incRulePart(rs);
}

result_on_square: result_on_square result_object_part | result_object_part;

result_object_part: OBJID {
    Rule * r = &pd.rules[pd.ruleCount];
    Pattern * rs = &r->resultPatterns[r->resultPatternCount];
    RulePart * rsp = &rs->parts[rs->partCount];
    RuleIdentity * rid = &rsp->identity[rsp->identityCount];

    rid->direction = UNSPECIFIED;
    rid->legendId = aliasLegendId($1);
    free($1);
    incRuleIdent(rsp);
}
                 | DIRECTION OBJID {
    Rule * r = &pd.rules[pd.ruleCount];
    Pattern * rs = &r->resultPatterns[r->resultPatternCount];
    RulePart * rsp = &rs->parts[rs->partCount];
    RuleIdentity * rid = &rsp->identity[rsp->identityCount];

    if ($1 == PARALLEL ||
        $1 == PERPENDICULAR) {
        r->hasCompoundDirection = 1;
    } else if ($1 == MOVING ||
        $1 == REL_UP ||
        $1 == REL_DOWN ||
        $1 == REL_LEFT ||
        $1 == REL_RIGHT) {
        r->hasRelativeDirection = 1;
    }
    rid->direction = $1;
    rid->legendId = aliasLegendId($2);
    free($2);
    incRuleIdent(rsp);
}

winconditions: winconditions wincondition | %empty;

wincondition:   LOGIC_WORD OBJID {
  pd.winConditions[pd.winConditionCount].hasOnQualifier = 0;
  pd.winConditions[pd.winConditionCount].winQualifier = $1;
  pd.winConditions[pd.winConditionCount].winIdentifier = aliasLegendId($2);
  free($2);
  incWinCondition();
}
        |       LOGIC_WORD OBJID LOGIC_ON OBJID {
  pd.winConditions[pd.winConditionCount].hasOnQualifier = 1;
  pd.winConditions[pd.winConditionCount].winQualifier = $1;
  pd.winConditions[pd.winConditionCount].winIdentifier = aliasLegendId($2);
  free($2);
  pd.winConditions[pd.winConditionCount].onIndentifier = aliasLegendId($4);
  free($4);
  incWinCondition();
}

some_level_eol: some_level_eol LEVEL_EOL | LEVEL_EOL;
any_level_eol: some_level_eol | %empty;

levels: any_level_eol the_levels;

the_levels: the_levels level | level;

level:          a_level some_level_eol { incLevel(); }
        |       a_level END_OF_FILE { incLevel(); }

a_level:        message_level
        |       rows


message_level:  MESSAGE ID {
  pd.levels[pd.levelCount].levelType = MESSAGE_TEXT;
  pd.levels[pd.levelCount].message = $2;
}

rows: rows row | row { pd.levels[pd.levelCount].levelType = SQUARES; }
row: tiles LEVEL_EOL {
  pd.levels[pd.levelCount].height++;
  pd.levels[pd.levelCount].width = rowWidth;
  rowWidth = 0;
}

tiles: tile tiles | tile

tile: GLYPH {
  rowWidth++;
  pd.levels[pd.levelCount].tiles[pd.levels[pd.levelCount].tileIndex] = $1;
  incTileIndex(pd.levelCount);
}
%%
