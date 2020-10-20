int printTest()
{
  yyparse();
printf("title %s\n", pd.title);
  printf("author %s\n", pd.author);
  printf("homepage %s\n", pd.homepage);
for (int i = 0; i < pd.objectCount; i++) {
    printf("  <OBJ '%s', LegendKey: '%c', Colors: (%s)>\n", pd.objects[i].name, pd.objects[i].legendKey, pd.objects[i].colors[0]);
  }
printf("LAYERCOUNT: %i\n", pd.layerCount);
  for (int i = 0; i < pd.layerCount; i++) {
    printf("width: %i - ", pd.layers[i].width);
    for (int j = 0; j < pd.layers[i].width - 1; j++) {
      printf("%s, ", pd.layers[i].objectNames[j]);
    }
    printf("%s", pd.layers[i].objectNames[pd.layers[i].width - 1]);
    printf("\n");
  }
printf("RULECOUNT: %i\n", pd.ruleCount);
  for (int i = 0; i < pd.ruleCount; i++) {
    printf("rule (m: %i, r: %i) -\n", pd.rules[i].matchStateCount, pd.rules[i].resultStateCount);
    if (pd.rules[i].hasDirectionConstraint == 1) {
      printf("  dir constraint: %s\n", pd.rules[i].directionConstraint);
    }
    for (int j = 0; j < pd.rules[i].matchStateCount; j++) {
      printf("  match part count: %i\n", pd.rules[i].matchStates[j].partCount);
      for (int k = 0; k < pd.rules[i].matchStates[j].partCount; k++) {
        if (pd.rules[i].matchStates[j].parts[k].hasDirection == 1) {
          printf("    mp - %i: (%s) %s\n", k, pd.rules[i].matchStates[j].parts[k].direction, pd.rules[i].matchStates[j].parts[k].identifier);
        } else {
          printf("    mp - %i: %s\n", k, pd.rules[i].matchStates[j].parts[k].identifier);
        }
      }
}
    for (int j = 0; j < pd.rules[i].resultStateCount; j++) {
      printf("  result part count: %i\n", pd.rules[i].resultStates[j].partCount);
      for (int k = 0; k < pd.rules[i].resultStates[j].partCount; k++) {
        if (pd.rules[i].resultStates[j].parts[k].hasDirection == 1) {
          printf("    rp - %i: (%s) %s\n", k, pd.rules[i].resultStates[j].parts[k].direction, pd.rules[i].resultStates[j].parts[k].identifier);
        } else {
          printf("    rp - %i: %s\n", k, pd.rules[i].resultStates[j].parts[k].identifier);
        }
      }
    }
  }
printf("WINCONDITIONS: %i\n", pd.winConditionCount);
printf("LEVEL COUNT: %i\n", pd.levelCount);
  for (int i = 0; i < pd.levelCount; i++) {
    printf("h: %i, w: %i c: %i\n", pd.levels[i].height, pd.levels[i].width, pd.levels[pd.levelCount].cellIndex);
    for (int j = 0; j < pd.levels[i].cellIndex; j++) {
      printf("%c", pd.levels[i].cells[j]);
      if ((j + 1) % (pd.levels[i].width) == 0 && j != 0) {
        printf("\n");
      }
    }
  }
  return 0;
}
