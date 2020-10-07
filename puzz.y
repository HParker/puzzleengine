%{
#include <stdio.h>
#include <string.h>
#include "puzzleData.h"
#define MAX_STRING 100
extern FILE *yyin;
PuzzleData pd;
int yylex();
int yyerror();
int yyerror(const char *p) { printf("ERROR: %s\n", p); return 1; }
%}

%union {
  char * identifier;
  char cell;
};

%token TITLE AUTHOR HOMEPAGE MODEHEADER EQUALS END_LAYER
%token  <identifier> ID OBJID COLOR LEGEND_VALUE LAYER_NAME DIRECTION LOGIC_WORD

// Rules tokens (ALSO USES OBJID)
%token OPEN_SQUARE CLOSE_SQUARE VIRTICAL_PIPE MOVE_RIGHT MOVE_UP MOVE_LEFT MOVE_DOWN ARROW
%token END_OF_RULE LEVEL_EOL
%token <cell> LEVEL_CELL LEGEND_ID

%%
puzzlescript:
                preamble
                modechange
                object_definitions
                modechange
                legend_lines
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

modechange: MODEHEADER ID MODEHEADER { printf("FOUND MODE CHANGE '%s'\n", $2); }

object_definitions: object_definition object_definitions | object_definition
object_definition: OBJID COLOR {
  pd.objects[pd.objectCount].name = strdup($1);

  pd.objects[pd.objectCount].colors[pd.objects[pd.objectCount].colorCount] = strdup($2);
  pd.objects[pd.objectCount].colorCount++;
  pd.objectCount++;
}

legend_lines: legend_line legend_lines | legend_line
legend_line: LEGEND_ID EQUALS LEGEND_VALUE {
  int found = 0;
  for (int i = 0; i < pd.objectCount; i++) {
    if (strcmp(pd.objects[i].name, $3) == 0) {
      found = 1;
      pd.objects[i].legendKey = $1;
    }
  }
  if (found == 0) {
    char * errorMessage = malloc(sizeof(char) * 100);
    sprintf(errorMessage, "legend line references object '%s' that is not defined in the list of objects\n", $3);
    yyerror(errorMessage);
  }
}

collision_lines: maybe_endlines collision_line collision_lines maybe_endlines | maybe_endlines collision_line maybe_endlines
collision_line: layers END_LAYER {
  pd.layerCount++;
}
maybe_endlines: END_LAYER maybe_endlines | END_LAYER |
layers: layer layers | layer
layer: LAYER_NAME {
  pd.layers[pd.layerCount].objectNames[pd.layers[pd.layerCount].width] = strdup($1);
  pd.layers[pd.layerCount].width++;
}

rules: rule rules | rule
maybe_newline: END_OF_RULE |
rule: maybe_newline rule_side arrow rule_side END_OF_RULE {
  pd.ruleCount++;
}
arrow: ARROW {
  pd.rules[pd.ruleCount].matchStateDone = 1;
}
rule_side: rule_with_global_direction | state_definitions
rule_with_global_direction: DIRECTION state_definitions {
  pd.rules[pd.ruleCount].directionConstraint = strdup($1);
  pd.rules[pd.ruleCount].hasDirectionConstraint = 1;
}
state_definitions: state_definition state_definitions | state_definition
state_definition: OPEN_SQUARE state_internals CLOSE_SQUARE {
  if (pd.rules[pd.ruleCount].matchStateDone == 0) {
    pd.rules[pd.ruleCount].matchStateCount++;
  } else {
    pd.rules[pd.ruleCount].resultStateCount++;
  }
}
state_internals: state_part VIRTICAL_PIPE state_internals | state_part
state_part: state_part_with_direction | state_part_without_direction
state_part_with_direction: DIRECTION OBJID {
  if (pd.rules[pd.ruleCount].matchStateDone == 0) {
    Rule * r = &pd.rules[pd.ruleCount];
    RuleState * rs = &r->matchStates[r->matchStateCount];
    RuleStatePart * rsp = &rs->parts[rs->partCount];
    rsp->direction = strdup($1);
    rsp->identifier = strdup($2);
    rsp->hasDirection = 1;
    rs->partCount++;
  } else {
    Rule * r = &pd.rules[pd.ruleCount];
    RuleState * rs = &r->resultStates[r->resultStateCount];
    RuleStatePart * rsp = &rs->parts[rs->partCount];
    rsp->direction = strdup($1);
    rsp->identifier = strdup($2);
    rs->partCount++;
  }

}
state_part_without_direction: OBJID {
  if (pd.rules[pd.ruleCount].matchStateDone == 0) {
    Rule * r = &pd.rules[pd.ruleCount];
    RuleState * rs = &r->matchStates[r->matchStateCount];
    RuleStatePart * rsp = &rs->parts[rs->partCount];
    rsp->identifier = strdup($1);
    rs->partCount++;
  } else {
    Rule * r = &pd.rules[pd.ruleCount];
    RuleState * rs = &r->resultStates[r->resultStateCount];
    RuleStatePart * rsp = &rs->parts[rs->partCount];
    rsp->identifier = strdup($1);
    rs->partCount++;
  }
}

winconditions: wincondition winconditions | wincondition
wincondition: wincondition_unconditional | wincondition_conditional
wincondition_conditional: LOGIC_WORD OBJID LOGIC_WORD OBJID {
  pd.winConditions[pd.winConditionCount].hasOnQualifier = 1;
  pd.winConditions[pd.winConditionCount].winQualifier = strdup($1);
  pd.winConditions[pd.winConditionCount].winIdentifier = strdup($2);
  pd.winConditions[pd.winConditionCount].onIndentifier = strdup($4);
  pd.winConditionCount++;
}
wincondition_unconditional: LOGIC_WORD OBJID {
  pd.winConditions[pd.winConditionCount].winQualifier = strdup($1);
  pd.winConditions[pd.winConditionCount].winIdentifier = strdup($2);
  pd.winConditionCount++;
}

levels: level levels | level
level:  maybe_level_newlines rows maybe_level_newlines {
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
maybe_level_newlines: LEVEL_EOL maybe_level_newlines | LEVEL_EOL |
%%
