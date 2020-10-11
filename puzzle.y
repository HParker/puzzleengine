%{
#include <stdio.h>
#include <string.h>
#include "puzzleData.h"
extern FILE *yyin;
PuzzleData pd;
int yylex();
int yyerror();
int yyerror(const char *p) { printf("ERROR: %s\n", p); return 1; }
int spriteIndex = 0;
%}

%union {
  char * identifier;
  char cell;
  int enumValue;
};

%token TITLE AUTHOR HOMEPAGE MODEHEADER EQUALS END_LAYER END_OF_RULE MESSAGE END_LEGEND_LINE
%token  <identifier> ID OBJID COLOR LEGEND_VALUE LAYER_NAME

// Rules tokens (ALSO USES OBJID)
%token MOVE_RIGHT MOVE_UP MOVE_LEFT MOVE_DOWN
%token OPEN_SQUARE CLOSE_SQUARE VIRTICAL_PIPE ARROW
%token LEVEL_EOL
%token <cell> LEVEL_CELL LEGEND_ID SPRITE_CELL
// ADD EXECUTION_TIME
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

preamble: title author homepage

title: TITLE ID {
  pd.title = strdup($2);
}
author: AUTHOR ID {
  pd.author = strdup($2);
}
homepage: HOMEPAGE ID {
  pd.homepage = strdup($2);
}

modechange: MODEHEADER ID MODEHEADER { /* printf("FOUND MODE CHANGE '%s'\n", $2); */ }

object_definitions: object_definition object_definitions
                  | object_definition

object_definition: basic_object_definition
                 | sprite_object_definition

basic_object_definition: OBJID COLOR {
  pd.objects[pd.objectCount].name = strdup($1);

  pd.objects[pd.objectCount].colors[pd.objects[pd.objectCount].colorCount] = strdup($2);
  pd.objects[pd.objectCount].colorCount++;
  pd.objectCount++;
}

sprite_object_definition: OBJID colors sprite {
  pd.objects[pd.objectCount].name = strdup($1);
  pd.objectCount++;
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

legend_line: LEGEND_ID EQUALS legend_values end_legend_line {
  pd.legend[pd.legendCount].key = $1;
  pd.legendCount++;
  /* int found = 0; */
  /* for (int i = 0; i < pd.objectCount; i++) { */
  /*   if (strcmp(pd.objects[i].name, $3) == 0) { */
  /*     found = 1; */
  /*     pd.objects[i].legendKey = $1; */
  /*   } */
  /* } */
  /* if (found == 0) { */
  /*   char * errorMessage = malloc(sizeof(char) * 100); */
  /*   sprintf(errorMessage, "legend line references object '%s' that is not defined in the list of objects\n", $3); */
  /*   yyerror(errorMessage); */
  /* } */
}

end_legend_line: END_LEGEND_LINE end_legend_line | END_LEGEND_LINE

legend_values: legend_value legend_values
        |       legend_value

legend_value: LEGEND_VALUE {
                  pd.legend[pd.legendCount].values[pd.legend[pd.legendCount].valueCount] = strdup($1);
                  pd.legend[pd.legendCount].valueCount++;
}

sounds: // Nothing for now

collision_lines: collision_line collision_lines
               | collision_line

collision_line: layer_objects END_LAYER {
  pd.layerCount++;
}

layer_objects: layer_object layer_objects
             | layer_object

layer_object: LAYER_NAME {
  pd.layers[pd.layerCount].objectNames[pd.layers[pd.layerCount].width] = strdup($1);
  pd.layers[pd.layerCount].width++;
}

rules: rule rules
     | rule


rule: rule_prefix state_definitions arrow state_definitions rule_postfix
    | rule_prefix state_definitions arrow state_definitions
    | state_definitions arrow state_definitions rule_postfix
    | state_definitions arrow state_definitions {
  pd.ruleCount++;
}

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

rule_postfix: OBJID // sfx

state_definitions: state_definition state_definitions
                 | state_definition
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

state_part_with_direction: DIRECTION OBJID {
  if (pd.rules[pd.ruleCount].matchStateDone == 0) {
    Rule * r = &pd.rules[pd.ruleCount];
    RuleState * rs = &r->matchStates[r->matchStateCount];
    RuleStatePart * rsp = &rs->parts[rs->partCount];
    rsp->direction = $1;
    rsp->identifier = strdup($2);
    rs->partCount++;
  } else {
    Rule * r = &pd.rules[pd.ruleCount];
    RuleState * rs = &r->resultStates[r->resultStateCount];
    RuleStatePart * rsp = &rs->parts[rs->partCount];
    rsp->direction = $1;
    rsp->identifier = strdup($2);
    rs->partCount++;
  }
}

state_part_without_direction: OBJID {
  if (pd.rules[pd.ruleCount].matchStateDone == 0) {
    Rule * r = &pd.rules[pd.ruleCount];
    RuleState * rs = &r->matchStates[r->matchStateCount];
    RuleStatePart * rsp = &rs->parts[rs->partCount];
    rsp->direction = 10; // NONE
    rsp->identifier = strdup($1);
    rs->partCount++;
  } else {
    Rule * r = &pd.rules[pd.ruleCount];
    RuleState * rs = &r->resultStates[r->resultStateCount];
    RuleStatePart * rsp = &rs->parts[rs->partCount];
    rsp->direction = 10; // NONE
    rsp->identifier = strdup($1);
    rs->partCount++;
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
  pd.winConditions[pd.winConditionCount].winIdentifier = strdup($2);
  pd.winConditions[pd.winConditionCount].onIndentifier = strdup($4);
  pd.winConditionCount++;
}
wincondition_unconditional: LOGIC_WORD OBJID {
  pd.winConditions[pd.winConditionCount].winQualifier = $1;
  pd.winConditions[pd.winConditionCount].winIdentifier = strdup($2);
  pd.winConditionCount++;
}

levels: level any_level_newlines levels | level
level: MESSAGE ID LEVEL_EOL
     | rows {
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
any_level_newlines: LEVEL_EOL any_level_newlines | LEVEL_EOL
%%
