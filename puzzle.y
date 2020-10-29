%{
#include <stdio.h>
#include <string.h>
#include "puzzleData.h"
PuzzleData pd;
int yyerror(const char *p) { printf("ERROR: %s\n", p); return 1; }

int legendId(char * name) {
    for (int i = 0; i < pd.legendCount; i++) {
        if (strcasecmp(pd.legend[i].key, name) == 0) {
            return i;
        }
    }
    printf("err: '%s' which does not exist in legend\n", name);
    return -1;
}

int spriteIndex = 0;
%}

%union {
  char * identifier;
  char cell;
  float decimal;
  int enumValue;
};

%token TITLE AUTHOR HOMEPAGE COLOR_PALETTE AGAIN_INTERVAL BACKGROUND_COLOR
%token FLICKSCREEN KEY_REPEAT_INTERVAL NOACTION NOREPEAT_ACTION NOUNDO NORESTART
%token REALTIME_INTERVAL REQUIRE_PLAYER_MOVEMENT RUN_RULES_ON_LEVEL_START
%token SCANLINE TEXT_COLOR THROTTLE_MOVEMENT ZOOMSCREEN
%token DEBUG VERBOSE_LOGGING

%token MODEHEADER EQUALS END_LAYER END_OF_RULE MESSAGE END_LEGEND_LINE LEGEND_AND LEGEND_OR
%token  <identifier> ID OBJID COLOR LEGEND_VALUE LAYER_NAME LEGEND_ID RULE_POSTFIX
%token  <decimal> DECIMAL

// Rules tokens (ALSO USES OBJID)
%token MOVE_RIGHT MOVE_UP MOVE_LEFT MOVE_DOWN
%token OPEN_SQUARE CLOSE_SQUARE VIRTICAL_PIPE ARROW
%token LEVEL_EOL
%token <cell> LEVEL_CELL SPRITE_CELL
%token <enumValue> LOGIC_WORD DIRECTION EXECUTION_TIME

%%
puzzlescript:
                preamble
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
                levels
        ;

preamble: preamble_option preamble | preamble_option

preamble_option: title
               | author
               | homepage
               | color_palette
               | again_interval
               | background_color
               | flickscreen
               | key_repeat_interval
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
               | require_player_movement

title: TITLE ID {
  pd.title = strdup($2);
}
author: AUTHOR ID {
  pd.author = strdup($2);
}
homepage: HOMEPAGE ID {
  pd.homepage = strdup($2);
}

require_player_movement: REQUIRE_PLAYER_MOVEMENT { pd.requirePlayerMovement = 1; }

color_palette: COLOR_PALETTE ID { pd.colorPalette = strdup($2); }
        ;
again_interval: AGAIN_INTERVAL ID { pd.againInterval = 0.1f; }
        ;
background_color: BACKGROUND_COLOR ID { pd.backgroundColor = strdup($2); }

        ;
flickscreen: FLICKSCREEN ID { yyerror("FLICKSCREEN IS NOT YET SUPPORTED\n"); }
        ;
key_repeat_interval: KEY_REPEAT_INTERVAL DECIMAL { pd.keyRepeatInterval = 0.1f; }

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
text_color: TEXT_COLOR ID { pd.textColor = strdup($2); }
        ;
throttle_movement: THROTTLE_MOVEMENT ID { pd.throttleMovement = 1; }

        ;
zoomscreen: ZOOMSCREEN ID { yyerror("ZOOMSCREEN IS NOT YET SUPPORTED\n"); }
        ;
debug: DEBUG { pd.debug = 1; }
        ;
verbose_logging: VERBOSE_LOGGING { pd.verboseLogging = 1; }

        ;

modechange: MODEHEADER ID MODEHEADER

object_definitions: object_definition object_definitions
                  | object_definition

object_definition: object_name colors sprite { pd.objectCount++; }
                 | object_name color { pd.objectCount++; }

object_name: OBJID SPRITE_CELL {
                 pd.objects[pd.objectCount].name = strdup($1);
                 pd.legend[pd.legendCount].key = strdup($1);
                 pd.legend[pd.legendCount].objectValues[0].id = pd.objectCount;
                 pd.legend[pd.legendCount].objectValues[0].isLegend = 0;
                 pd.legend[pd.legendCount].objectCount++;
                 pd.legendCount++;
                 // TODO: this is an annoying hack...
                 char key[1];
                 key[0] = $2;
                 pd.legend[pd.legendCount].key = key;
                 pd.legend[pd.legendCount].objectValues[0].id = pd.objectCount;
                 pd.legend[pd.legendCount].objectValues[0].isLegend = 0;
                 pd.legend[pd.legendCount].objectCount++;
                 pd.legendCount++;
}
           | OBJID {
               pd.objects[pd.objectCount].name = strdup($1);
               pd.legend[pd.legendCount].key = strdup($1);
               pd.legend[pd.legendCount].objectValues[0].id = pd.objectCount;
               pd.legend[pd.legendCount].objectValues[0].isLegend = 0;
               pd.legend[pd.legendCount].objectCount++;
               pd.legendCount++;
}

colors: color colors | color

color: COLOR {
  pd.objects[pd.objectCount].colors[pd.objects[pd.objectCount].colorCount] = strdup($1);
  pd.objects[pd.objectCount].colorCount++;
}

sprite: sprite_row sprite_row sprite_row sprite_row sprite_row {
  spriteIndex = 0;
}

sprite_row: sprite_cell sprite_cell sprite_cell sprite_cell sprite_cell

sprite_cell: SPRITE_CELL {
    pd.objects[pd.objectCount].sprite[spriteIndex] = $1;
    spriteIndex++;
}

legend_lines: legend_line legend_lines
            | legend_line

legend_line: legend_id EQUALS legend_values end_legend_line {
  pd.legendCount++;
  pd.legend[pd.legendCount].objectRelation = LEGEND_RELATION_UNKNOWN;
}

legend_id: LEGEND_ID {
  pd.legend[pd.legendCount].key = strdup($1);
}

end_legend_line: END_LEGEND_LINE end_legend_line | END_LEGEND_LINE

legend_values: legend_value legend_joiner legend_values
             | legend_value

legend_value: LEGEND_VALUE {
  Legend l = pd.legend[pd.legendCount];
  l.objectValues[l.objectCount].id = legendId($1);
  l.objectValues[l.objectCount].isLegend = 1;
  l.objectCount++;
  pd.legend[pd.legendCount] = l;
}

legend_joiner: LEGEND_AND { pd.legend[pd.legendCount].objectRelation = LEGEND_RELATION_AND; }
             | LEGEND_OR  { pd.legend[pd.legendCount].objectRelation = LEGEND_RELATION_OR; }

sounds: // Nothing for now

collision_lines: collision_line collision_lines
               | collision_line

collision_line: layer_objects END_LAYER {
  pd.layerCount++;
}

layer_objects: layer_object layer_objects
             | layer_object

layer_object: LAYER_NAME {
  pd.layers[pd.layerCount].objectIds[pd.layers[pd.layerCount].width] = legendId($1);
  pd.layers[pd.layerCount].width++;
}

rules: rule_line  rules
     | rule_line


rule_line: any_eor rule some_eor
        ;

some_eor:       END_OF_RULE some_eor | END_OF_RULE
        ;

any_eor:        some_eor |
        ;

rule: rule_prefix rule_infix rule_postfix { pd.ruleCount++; }
    | rule_prefix rule_infix { pd.ruleCount++; }
    |             rule_infix rule_postfix { pd.ruleCount++; }
    | state_definitions arrow state_definitions { pd.ruleCount++; }

rule_infix: state_definitions arrow state_definitions


arrow: ARROW {
  pd.rules[pd.ruleCount].matchStateDone = 1;
}

rule_prefix: EXECUTION_TIME {
  pd.rules[pd.ruleCount].executionTime = $1;
  pd.rules[pd.ruleCount].directionConstraint = NONE;
}
           | DIRECTION {
  pd.rules[pd.ruleCount].executionTime = NORMAL;
  pd.rules[pd.ruleCount].directionConstraint = $1;
}

rule_postfix: RULE_POSTFIX { printf("sfx don't work yet\n"); }// sfx

state_definitions: state_definition state_definitions
                 | state_definition
                 | rule_postfix

state_definition: OPEN_SQUARE state_internals CLOSE_SQUARE {
  if (pd.rules[pd.ruleCount].matchStateDone == 0) {
    pd.rules[pd.ruleCount].matchStateCount++;
  } else {
    pd.rules[pd.ruleCount].resultStateCount++;
  }
}

state_internals: state_part VIRTICAL_PIPE state_internals
               | state_part

state_part: state_part_with_direction
          | state_part_without_direction
          |

state_part_with_direction: DIRECTION objects_on_same_square {
  if (pd.rules[pd.ruleCount].matchStateDone == 0) {
    Rule * r = &pd.rules[pd.ruleCount];
    RuleState * rs = &r->matchStates[r->matchStateCount];
    RuleStatePart * rsp = &rs->parts[rs->partCount];
    rsp->direction = $1;
    rs->partCount++;
  } else {
    Rule * r = &pd.rules[pd.ruleCount];
    RuleState * rs = &r->resultStates[r->resultStateCount];
    RuleStatePart * rsp = &rs->parts[rs->partCount];
    rsp->direction = $1;
    rs->partCount++;
  }
}

state_part_without_direction: objects_on_same_square {
  if (pd.rules[pd.ruleCount].matchStateDone == 0) {
    Rule * r = &pd.rules[pd.ruleCount];
    RuleState * rs = &r->matchStates[r->matchStateCount];
    RuleStatePart * rsp = &rs->parts[rs->partCount];
    rsp->direction = NONE;
    rs->partCount++;
  } else {
    Rule * r = &pd.rules[pd.ruleCount];
    RuleState * rs = &r->resultStates[r->resultStateCount];
    RuleStatePart * rsp = &rs->parts[rs->partCount];
    rsp->direction = NONE;
    rs->partCount++;
  }
}

objects_on_same_square: object objects_on_same_square | object

object: OBJID {
  if (pd.rules[pd.ruleCount].matchStateDone == 0) {
    Rule * r = &pd.rules[pd.ruleCount];
    RuleState * rs = &r->matchStates[r->matchStateCount];
    RuleStatePart * rsp = &rs->parts[rs->partCount];
    rsp->direction = NONE;
    rsp->legendId = legendId($1);
  } else {
    Rule * r = &pd.rules[pd.ruleCount];
    RuleState * rs = &r->resultStates[r->resultStateCount];
    RuleStatePart * rsp = &rs->parts[rs->partCount];
    rsp->direction = NONE;
    rsp->legendId = legendId($1);
  }
}


winconditions: wincondition winconditions
             | wincondition
             | // PLEASE DON'T GET MAD BISON, SOMETIMTES YOU CAN'T WIN

wincondition: wincondition_unconditional
            | wincondition_conditional

wincondition_conditional: LOGIC_WORD OBJID LOGIC_WORD OBJID {
  pd.winConditions[pd.winConditionCount].hasOnQualifier = 1;
  pd.winConditions[pd.winConditionCount].winQualifier = $1;
  pd.winConditions[pd.winConditionCount].winIdentifier = legendId($2);
  pd.winConditions[pd.winConditionCount].onIndentifier = legendId($4);
  pd.winConditionCount++;
}

wincondition_unconditional: LOGIC_WORD OBJID {
  pd.winConditions[pd.winConditionCount].winQualifier = $1;
  pd.winConditions[pd.winConditionCount].winIdentifier = legendId($2);
  pd.winConditionCount++;
}



levels: level levels
      | level

level: MESSAGE ID any_level_newlines
     | rows any_level_newlines {
  pd.levelCount++;
}
rows: row rows | row
row: cells LEVEL_EOL {
  pd.levels[pd.levelCount].height++;
  if (pd.levels[pd.levelCount].width == 0) {
    pd.levels[pd.levelCount].width = pd.levels[pd.levelCount].cellIndex;
  }
}

cells: cell cells | cell
cell: LEVEL_CELL {
  pd.levels[pd.levelCount].cells[pd.levels[pd.levelCount].cellIndex] = $1;
  pd.levels[pd.levelCount].cellIndex++;

}
any_level_newlines: LEVEL_EOL any_level_newlines | LEVEL_EOL |
%%

PuzzleData * parsePuzzle(FILE * file) {
  yyin = file;
  yyparse();
  return &pd;
}
